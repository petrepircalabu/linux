// SPDX-License-Identifier: GPL-2.0-only
/******************************************************************************
 * domstate-notify.c
 *
 * XEN domain state change notifications
 *
 * Copyright (c) 2019, Bitdefender S.R.L.
 */

#define pr_fmt(fmt) "xen:"KBUILD_MODNAME ": " fmt
/* Enable debug ouput */
#define DEBUG

#include <linux/module.h>
#include <net/netlink.h>
#include <net/genetlink.h>

#define VERSION "0.1"

#define DOMSTATE_NOTIFY_GENL_FAMILY_NAME	"domstate_notify"
#define DOMSTATE_NOTIFY_GENL_VERSION		0x01

/* Supported commands */
enum {
	DOMSTATE_NOTIFY_CMD_UNSPEC,
	DOMSTATE_NOTIFY_CMD_OPEN,
	DOMSTATE_NOTIFY_CMD_DESTROY,
	__DOMSTATE_NOTIFY_CMD_MAX,
};
#define DOMSTATE_NOTIFY_CMD_MAX (__DOMSTATE_NOTIFY_CMD_MAX - 1)

/* Configuration policy attributes */
enum {
	DOMSTATE_NOTIFY_ATTR_UNSPEC,
	__DOMSTATE_NOTIFY_ATTR_MAX,
};
#define DOMSTATE_NOTIFY_ATTR_MAX (__DOMSTATE_NOTIFY_ATTR_MAX - 1)

static const struct nla_policy domstate_notify_attr_policy[DOMSTATE_NOTIFY_ATTR_MAX + 1] = {
};

static int domstate_notify_genl_open(struct sk_buff *skb, struct genl_info *info)
{
	pr_debug("%s:\n", __func__);

	return 0;
}

static int domstate_notify_genl_destroy(struct sk_buff *skb,
					struct genl_info *info)
{
	pr_debug("%s:\n", __func__);

	return 0;
}

static const struct genl_ops domstate_notify_genl_ops[] = {
	{
		.cmd	= DOMSTATE_NOTIFY_CMD_OPEN,
		.validate = GENL_DONT_VALIDATE_STRICT | GENL_DONT_VALIDATE_DUMP,
		.doit	= domstate_notify_genl_open,
	},
	{
		.cmd	= DOMSTATE_NOTIFY_CMD_DESTROY,
		.validate = GENL_DONT_VALIDATE_STRICT | GENL_DONT_VALIDATE_DUMP,
		.doit	= domstate_notify_genl_destroy,
	},
};

static struct genl_family domstate_notify_genl_family __ro_after_init = {
	.module		= THIS_MODULE,
	.name 		= DOMSTATE_NOTIFY_GENL_FAMILY_NAME,
	.version	= DOMSTATE_NOTIFY_GENL_VERSION,
	.ops		= domstate_notify_genl_ops,
	.n_ops		= ARRAY_SIZE(domstate_notify_genl_ops),
	.maxattr	= DOMSTATE_NOTIFY_ATTR_MAX,
};
static __init int xen_domstate_notify_init(void)
{
	int rc;

	pr_debug("%s: Module loaded\n", __func__);

	rc = genl_register_family(&domstate_notify_genl_family);

	return rc;
}

static __exit void xen_domstate_notify_exit(void)
{
	pr_debug("%s: Module unloaded\n", __func__);

	genl_unregister_family(&domstate_notify_genl_family);
}

module_init(xen_domstate_notify_init);
module_exit(xen_domstate_notify_exit);

MODULE_AUTHOR("Petre Pircalabu <ppircalabu@bitdefender.com>");
MODULE_DESCRIPTION("Xen domain state change notification ver " VERSION);
MODULE_VERSION(VERSION);
MODULE_LICENSE("GPL");
