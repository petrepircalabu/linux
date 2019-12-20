/* SPDX-License-Identifier: GPL-2.0 */
/******************************************************************************
 * domstate_notify.h
 *
 * Domain state notification interface.
 *
 * Copyright (c) 2019 Bitdefender S.R.L.
 */

#ifndef __XEN_PUBLIC_DOMSTATE_NOTIFY_H__
#define __XEN_PUBLIC_DOMSTATE_NOTIFY_H__

#include <xen/interface/xen.h>
#include "io/ring.h"

#define XEN_DOMSTATE_NOTIFY_register        1
#define XEN_DOMSTATE_NOTIFY_unregister      2
#define XEN_DOMSTATE_NOTIFY_enable          3
#define XEN_DOMSTATE_NOTIFY_disable         4

/*
 * XEN_DOMSTATE_NOTIFY_register: registers a page as a domain state change
 * notify sink.
 */
struct domstate_notify_register {
	uint64_t page_gfn;
	uint32_t port;
};

typedef struct domstate_notify_st {
	uint32_t version;
	uint32_t domain_id;
	uint32_t state;
	uint32_t extra;
} domstate_notify_event_t;

DEFINE_RING_TYPES(domstate_notify, domstate_notify_event_t, domstate_notify_event_t);
#endif /* __XEN_PUBLIC_DOMSTATE_NOTIFY_H__ */
