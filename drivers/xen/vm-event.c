// SPDX-License-Identifier: GPL-2.0-only
/******************************************************************************
 * vm-event.c
 *
 * VM Event socket based interface
 *
 * Copyright (c) 2019, Bitdefender S.R.L.
 */

#define pr_fmt(fmt) "xen:" KBUILD_MODNAME ": " fmt
/* Enable debug ouput */
#define DEBUG

#include <linux/module.h>

#include <net/netlink.h>
#include <net/genetlink.h>

#include <asm/xen/hypercall.h>

#include <xen/xen.h>
#include <xen/interface/xen.h>
#include <xen/xen-ops.h>

#define VERSION "0.1"

#define VM_EVENT_GENL_FAMILY_NAME	"xen_vm_event"
#define VM_EVENT_GENL_VERSION		0x01

/* Commands supported by the vm_event_genl_family */
enum {
	VM_EVENT_CMD_UNSPEC,
	VM_EVENT_CMD_OPEN,
	VM_EVENT_CMD_DESTROY,
	__VM_EVENT_CMD_MAX,
};
#define VM_EVENT_GENL_CMD_MAX (__VM_EVENT_GENL_CMD_MAX - 1)

/* Configuration policy attributes */
enum {
	VM_EVENT_ATTR_UNSPEC,
	VM_EVENT_ATTR_DOMID,
	VM_EVENT_ATTR_TYPE,
	__VM_EVENT_ATTR_MAX,
};
#define VM_EVENT_ATTR_MAX (__VM_EVENT_ATTR_MAX - 1)

/* Netlink interface. */
static const struct nla_policy vm_event_attr_policy[VM_EVENT_ATTR_MAX + 1] = {
	[VM_EVENT_ATTR_DOMID]	= { .type = NLA_U32 },
	[VM_EVENT_ATTR_TYPE]	= { .type = NLA_U32 },
};

static int vm_event_genl_open(struct sk_buff *skb, struct genl_info *info)
{
	uint32_t domid, type;
	int rc;

	pr_debug("%s:\n", __func__);

	if (!info->attrs[VM_EVENT_ATTR_DOMID] ||
	    !info->attrs[VM_EVENT_ATTR_TYPE]) {
		pr_err("Please make sure that the domid and type fields are set");
		return -EINVAL;
	}

	domid = nla_get_u32(info->attrs[VM_EVENT_ATTR_DOMID]);
	type = nla_get_u32(info->attrs[VM_EVENT_ATTR_TYPE]);

	pr_debug("%s:Open vm_event domid = %d type= %d\n", __func__, domid, type);

	xen_preemptible_hcall_begin();
	rc = HYPERVISOR_vm_event_op(domid, 1, type);
	xen_preemptible_hcall_end();

	return rc;
}

static int vm_event_genl_destroy(struct sk_buff *skb, struct genl_info *info)
{
	uint32_t domid, type;
	int rc;

	pr_debug("%s:\n", __func__);

	if (!info->attrs[VM_EVENT_ATTR_DOMID] ||
	    !info->attrs[VM_EVENT_ATTR_TYPE]) {
		pr_err("Please make sure that the domid and type fields are set");
		return -EINVAL;
	}

	domid = nla_get_u32(info->attrs[VM_EVENT_ATTR_DOMID]);
	type = nla_get_u32(info->attrs[VM_EVENT_ATTR_TYPE]);

	pr_debug("%s:Destroy vm_event domid = %d type= %d\n", __func__, domid, type);

	xen_preemptible_hcall_begin();
	rc = HYPERVISOR_vm_event_op(domid, 2, type);
	xen_preemptible_hcall_end();

	return rc;
}

static const struct genl_ops vm_event_genl_ops[] = {
	{
		.cmd	= VM_EVENT_CMD_OPEN,
		.validate = GENL_DONT_VALIDATE_STRICT | GENL_DONT_VALIDATE_DUMP,
		.doit	= vm_event_genl_open,
	},
	{
		.cmd	= VM_EVENT_CMD_DESTROY,
		.validate = GENL_DONT_VALIDATE_STRICT | GENL_DONT_VALIDATE_DUMP,
		.doit	= vm_event_genl_destroy,
	},
};

static struct genl_family vm_event_genl_family __ro_after_init = {
	.module		= THIS_MODULE,
	.name 		= VM_EVENT_GENL_FAMILY_NAME,
	.version	= VM_EVENT_GENL_VERSION,
	.ops		= vm_event_genl_ops,
	.n_ops		= ARRAY_SIZE(vm_event_genl_ops),
	.maxattr	= VM_EVENT_ATTR_MAX,
};

static __init int vm_event_init(void)
{
	int error;

	pr_debug("%s: Module loaded\n", __func__);

	error = genl_register_family(&vm_event_genl_family);

	return error;
}

static __exit void vm_event_exit(void)
{
	pr_debug("%s: Module unloaded\n", __func__);
}

module_init(vm_event_init);
module_exit(vm_event_exit);

MODULE_AUTHOR("Petre Pircalabu <ppircalabu@bitdefender.com>");
MODULE_DESCRIPTION("Xen VM event socket ver " VERSION);
MODULE_VERSION(VERSION);
MODULE_LICENSE("GPL");
