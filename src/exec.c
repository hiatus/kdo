#include <linux/umh.h>

#include "cdev.h"
#include "exec.h"


static char _argv2[CDEV_MAX_WRITE];

static char *_envp[] = {"HOME=/root", NULL};
static char *_argv[] = {"/bin/sh", "-c", _argv2, NULL};


int kdo_exec(const char *cmd)
{
	int ret = 0;

	_argv2[strlen(cmd)] = 0x00;
	strncpy(_argv2, cmd, CDEV_MAX_WRITE);

	printk(KERN_INFO "kdo_exec: %s", _argv2);

	ret = call_usermodehelper(_argv[0], _argv, _envp, UMH_NO_WAIT);

#ifdef DEBUG
	if (ret)
		printk(KERN_ERR "kdo_exec: call_usermodehelper failed with code %i", ret);
#endif
	return ret;
}
