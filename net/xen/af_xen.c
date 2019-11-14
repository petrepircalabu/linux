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
#include <net/xen/xen.h>

#include <xen/xen.h>

#define VERSION "0.1"

static const struct net_proto_family *xen_proto[XENPROTO_MAX];
static DEFINE_RWLOCK(xen_proto_lock);

int xen_sock_register(int proto, const struct net_proto_family *ops)
{
	int err = 0;

	if (proto < 0 || proto >= XENPROTO_MAX)
		return -EINVAL;

	write_lock(&xen_proto_lock);

	if (xen_proto[proto])
		err = -EEXIST;
	else
		xen_proto[proto] = ops;

	write_unlock(&xen_proto_lock);

	return err;
};
EXPORT_SYMBOL(xen_sock_register);

void xen_sock_unregister(int proto, const struct net_proto_family *ops)
{
	if (proto < 0 || proto >= XENPROTO_MAX)
		return;

	write_lock(&xen_proto_lock);
	xen_proto[proto] = NULL;
	write_unlock(&xen_proto_lock);
};
EXPORT_SYMBOL(xen_sock_unregister);

void xen_sock_link(struct xen_sock_list *l, struct sock *sk)
{
	write_lock(&l->lock);
	sk_add_node(sk, &l->head);
	write_unlock(&l->lock);
}
EXPORT_SYMBOL(xen_sock_link);

void xen_sock_unlink(struct xen_sock_list *l, struct sock *sk)
{
	write_lock(&l->lock);
	sk_del_node_init(sk);
	write_unlock(&l->lock);
}
EXPORT_SYMBOL(xen_sock_unlink);

static int xen_sock_create(struct net *net, struct socket *sock, int proto,
			   int kern)
{
	int err;

	if (net != &init_net)
		return -EAFNOSUPPORT;

	if (proto < 0 || proto >= XENPROTO_MAX)
		return -EINVAL;

	if (!xen_proto[proto])
		request_module("xen-proto-%d", proto);

	err = -EPROTONOSUPPORT;

	read_lock(&xen_proto_lock);

	if (xen_proto[proto] && try_module_get(xen_proto[proto]->owner)) {
		err = xen_proto[proto]->create(net, sock, proto, kern);
		module_put(xen_proto[proto]->owner);
	}

	read_unlock(&xen_proto_lock);
	return err;
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

	err = vm_event_sock_init();
	if (err)
		goto unregister_socket;

	return 0;

unregister_socket:
	sock_unregister(PF_XEN);

	return err;
}

static __exit void xen_exit(void)
{
	vm_event_sock_cleanup();
}

module_init(xen_init);
module_exit(xen_exit);

MODULE_AUTHOR("Petre Pircalabu <ppircalabu@bitdefender.com>");
MODULE_DESCRIPTION("Xen VM event socket ver " VERSION);
MODULE_VERSION(VERSION);
MODULE_LICENSE("GPL");
MODULE_ALIAS_NETPROTO(PF_XEN);
