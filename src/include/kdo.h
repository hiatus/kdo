#ifndef KDO_H
#define KDO_H

#ifndef __KERNEL_PRINTK__
#include <linux/printk.h>
#endif


#define kdo_log(level, s) \
	printk(level "[kdo] %s: " s, __FUNCTION__)

#define kdo_log_fmt(level, f, ...) \
	printk(level "[kdo] %s: " f, __FUNCTION__, __VA_ARGS__)
#endif
