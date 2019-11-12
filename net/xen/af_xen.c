// SPDX-License-Identifier: GPL-2.0-only
/******************************************************************************
 * af_xen.c
 *
 * Implementation of XEN vm_event sockets
 *
 * Copyright (c) 2019, Bitdefender S.R.L.
 */

#define pr_fmt(fmt) "xen:" KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <net/sock.h>

#include <xen/xen.h>

#define VERSION "0.1"
static int xen_sock_create(struct net *net, struct socket *sock, int proto,
			   int kern)
{
	if (net != &init_net)
		return -EAFNOSUPPORT;

	if (proto < 0 || proto >= XENPROTO_MAX)
		return -EINVAL;

	return 0;
}

static const struct net_proto_family xen_sock_family_ops = {
	.owner	= THIS_MODULE,
	.family	= PF_XEN,
	.create	= xen_sock_create,
};

static __init int xen_init(void)
{
	int err;

	if (!xen_domain())
		return -ENODEV;

	err = sock_register(&xen_sock_family_ops);
	if (err)
		return err;

	return 0;
}

static __exit void xen_exit(void)
{
}

module_init(xen_init);
module_exit(xen_exit);

MODULE_AUTHOR("Petre Pircalabu <ppircalabu@bitdefender.com>");
MODULE_DESCRIPTION("Xen VM event socket ver " VERSION);
MODULE_VERSION(VERSION);
MODULE_LICENSE("GPL");
MODULE_ALIAS_NETPROTO(PF_XEN);
