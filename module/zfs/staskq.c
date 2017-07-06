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

#include <sys/zfs_context.h>
#include <sys/staskq.h>


staskq_t *
staskq_create(const char *name, int nthreads, pri_t pri,
    int minalloc, int maxalloc, uint_t flags)
{
	taskq_t *tq;
	staskq_t *stq;

	tq = taskq_create(name, nthreads, pri, minalloc, maxalloc, flags);
	if (tq == NULL)
		return (NULL);

	stq = kmem_alloc(sizeof (staskq_t), KM_SLEEP);
	mutex_init(&stq->stq_lock, NULL, MUTEX_DEFAULT, NULL);
	stq->stq_tq = tq;
	stq->stq_state = STASKQ_ACTIVE;

	return (stq);
}

void
staskq_destroy(staskq_t *stq)
{
	taskq_destroy(stq->stq_tq);
	stq->stq_tq = NULL;
	kmem_free(stq, sizeof (staskq_t));
}

void
staskq_suspend(staskq_t *stq)
{
	mutex_enter(&stq->stq_lock);
	stq->stq_state = STASKQ_SUSPENDED;
	mutex_exit(&stq->stq_lock);
}

void
staskq_resume(staskq_t *stq)
{
	mutex_enter(&stq->stq_lock);
	stq->stq_state = STASKQ_ACTIVE;
	mutex_exit(&stq->stq_lock);
}

void
staskq_wait_id(staskq_t *stq, staskqid_t id)
{
	taskq_wait_id(stq->stq_tq, id);
}

void
staskq_wait(staskq_t *stq)
{
	taskq_wait(stq->stq_tq);
}

staskqid_t
staskq_dispatch(staskq_t *stq, task_func_t func, void *arg, uint_t flags)
{
	taskqid_t id;
	mutex_enter(&stq->stq_lock);
	if (stq->stq_state == STASKQ_ACTIVE)
		id = taskq_dispatch(stq->stq_tq, func, arg, flags);
	else
		id = TASKQID_INVALID;
	mutex_exit(&stq->stq_lock);
	return (id);
}
