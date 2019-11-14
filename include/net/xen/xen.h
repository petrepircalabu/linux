/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __NET_XEN_H
#define __NET_XEN_H

#include <net/sock.h>

/* protocols of the protocol family PF_XEN */
#define XENPROTO_VM_EVENT		0 /* VM_EVENT */
#define XENPROTO_MAX			1

/* Connection and socket states */
enum {
	XEN_CONNECTED = 1,
	XEN_OPEN,
};

struct xen_vm_event_dev {
};

struct xen_sock_list {
	struct hlist_head head;
	rwlock_t          lock;
};

int xen_sock_register(int proto, const struct net_proto_family *ops);
void xen_sock_unregister(int proto, const struct net_proto_family *ops);

void xen_sock_link(struct xen_sock_list *l, struct sock *sk);
void xen_sock_unlink(struct xen_sock_list *l, struct sock *sk);

int vm_event_sock_init(void);
void vm_event_sock_cleanup(void);

#endif /* __NET_XEN_H */
