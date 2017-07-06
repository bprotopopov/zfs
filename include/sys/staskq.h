/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License (the "License").
 * You may not use this file except in compliance with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */

#ifndef	_STASKQ_H
#define	_STASKQ_H

#ifdef	__cplusplus
extern "C" {
#endif

typedef enum {
	STASKQ_ACTIVE,
	STASKQ_SUSPENDED
} staskq_state_t;

typedef struct {
	kmutex_t stq_lock;
	staskq_state_t stq_state;
	taskq_t *stq_tq;
} staskq_t;

typedef taskqid_t staskqid_t;

extern staskq_t *staskq_create(const char *, int, pri_t, int, int, uint_t);
extern void staskq_destroy(staskq_t *);
extern void staskq_suspend(staskq_t *);
extern void staskq_resume(staskq_t *);
extern void staskq_wait_id(staskq_t *, staskqid_t);
extern void staskq_wait(staskq_t *);
extern taskqid_t staskq_dispatch(staskq_t *, task_func_t, void *, uint_t);

#ifdef	__cplusplus
}
#endif

#endif
