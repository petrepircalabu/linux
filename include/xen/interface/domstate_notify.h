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

#define XEN_DOMSTATE_NOTIFY_register        1
#define XEN_DOMSTATE_NOTIFY_unregister      2
#define XEN_DOMSTATE_NOTIFY_enable          3
#define XEN_DOMSTATE_NOTIFY_disable         4

#endif /* __XEN_PUBLIC_DOMSTATE_NOTIFY_H__ */
