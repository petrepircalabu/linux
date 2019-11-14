// SPDX-License-Identifier: GPL-2.0-only
/******************************************************************************
 * vm_event_sock.c
 *
 * VM_EVENT protocol
 *
 * Copyright (c) 2019, Bitdefender S.R.L.
 */

#include <linux/sched.h>
#include <net/xen/xen.h>

struct vm_event_pinfo {
	struct sock 		sk;
	struct xen_vm_event_dev *dev;
};

static struct xen_sock_list vm_event_sk_list = {
	.lock = __RW_LOCK_UNLOCKED(vm_event_sk_list.lock)
};

static const struct proto_ops vm_event_sock_ops = {
	.family		= PF_XEN,
	.owner		= THIS_MODULE,
	.release	= NULL,
	.bind		= NULL,
	.getname	= NULL,
	.sendmsg	= NULL,
	.recvmsg	= NULL,
	.ioctl		= NULL,
	.poll		= NULL,
	.listen		= NULL,
	.shutdown	= NULL,
	.setsockopt	= NULL,
	.getsockopt	= NULL,
	.connect	= NULL,
	.socketpair	= NULL,
	.accept		= NULL,
	.mmap		= NULL,
};

static struct proto vm_event_sk_proto = {
	.name 		= "VM_EVENT",
	.owner		= THIS_MODULE,
	.obj_size	= sizeof(struct vm_event_pinfo),
};

static int vm_event_sock_create(struct net *net, struct socket *sock,
				int protocol, int kern)
{
	struct sock *sk;

	sock->ops = &vm_event_sock_ops;

	sk = sk_alloc(net, PF_XEN, GFP_ATOMIC, &vm_event_sk_proto, kern);
	if (!sk)
		return -ENOMEM;

	sock_init_data(sock, sk);

	sk->sk_protocol = protocol;

	sock->state = SS_UNCONNECTED;
	sk->sk_state = XEN_OPEN;

	xen_sock_link(&vm_event_sk_list, sk);

	return 0;
}

static const struct net_proto_family vm_event_sock_family_ops = {
	.family	= PF_XEN,
	.owner	= THIS_MODULE,
	.create	= vm_event_sock_create,
};

int __init vm_event_sock_init(void)
{
	return xen_sock_register(XENPROTO_VM_EVENT, &vm_event_sock_family_ops);
}

void vm_event_sock_cleanup(void)
{
}
