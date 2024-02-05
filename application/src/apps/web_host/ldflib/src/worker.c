/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 *
 * this file licensed under the Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 * http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR
 * PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author: zhangjianping
 * Create: 2020-12-25 06:48:47
 * Description: export libdflib to lua
 */

#include "worker.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

typedef struct _work {
    struct _queue* queue;
    struct _work*  prev;
    struct _work*  next;
    int            ref;

    spinlock_t lock;
    work_cb    cb;
    destory_cb destory;
    query_cb   query;
    int        complete;
    void*      data;
} work_t;

// queue
typedef struct _queue {
    pthread_cond_t  cond;
    pthread_mutex_t mutex;
    work_t          head;
    int             size;
    int             cap;
    int             quit;
} queue_t;

LOCAL int queue_init(queue_t* q, int cap)
{
    if (pthread_mutex_init(&q->mutex, NULL)) {
        (void)fprintf(stderr, "Init mutex error");
        return 0;
    }
    if (pthread_cond_init(&q->cond, NULL)) {
        (void)fprintf(stderr, "Init cond error");
        return 0;
    }
    q->head.next = &q->head;
    q->head.prev = &q->head;
    q->size      = 0;
    q->cap       = cap;
    q->quit      = 0;

    return 1;
}

LOCAL int queue_push(queue_t* q, work_t* p)
{
    pthread_mutex_lock(&q->mutex);

    if (q->quit != 0 || q->size == q->cap) {
        pthread_mutex_unlock(&q->mutex);
        return 0;
    }

    work_t* last = q->head.prev;
    p->next      = &q->head;
    p->prev      = last;
    last->next   = p;
    q->head.prev = p;
    q->size++;
    p->queue = q;

    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->mutex);

    return 1;
}

LOCAL work_t* queue_pop(queue_t* q)
{
    pthread_mutex_lock(&q->mutex);
    while (q->size == 0) {
        pthread_cond_wait(&q->cond, &q->mutex);
    }

    if (q->quit != 0) {
        pthread_mutex_unlock(&q->mutex);
        return NULL;
    }

    work_t* p     = q->head.next;
    p->next->prev = &q->head;
    q->head.next  = p->next;
    p->next       = NULL;
    p->prev       = NULL;
    p->queue      = NULL;
    q->size--;
    pthread_mutex_unlock(&q->mutex);

    return p;
}

LOCAL int queue_remove(queue_t* q, work_t* p)
{
    if (p->next == NULL) {
        return 0;
    }

    pthread_mutex_lock(&q->mutex);
    if (p->next == NULL) {
        pthread_mutex_unlock(&q->mutex);
        return 0;
    }

    work_t* prev  = p->prev;
    p->next->prev = prev;
    prev->next    = p->next;
    p->next       = NULL;
    p->prev       = NULL;
    p->queue      = NULL;
    q->size--;
    pthread_mutex_unlock(&q->mutex);
    return 1;
}

LOCAL void queue_quit(queue_t* q)
{
    pthread_mutex_lock(&q->mutex);
    q->quit = 1;
    pthread_cond_broadcast(&q->cond);
    pthread_mutex_unlock(&q->mutex);
}

// worker
typedef struct _worker {
    spinlock_t lock;
    int        count;
    queue_t    queue;
    int        ref;
    pthread_t  threads[0];
} worker_t;

LOCAL void* thread_worker(void* p)
{
    work_t*   work;
    worker_t* worker = p;
    while ((work = queue_pop(&worker->queue)) != NULL) {
        work->cb(work->data);
        spinlock_lock(&work->lock);
        work->complete = 1;
        spinlock_unlock(&work->lock);
        unref_work(work);
    }
    return NULL;
}

worker_t* new_worker(int count, int cap)
{
    worker_t* worker = malloc(sizeof(worker_t) + sizeof(pthread_t) * count);
    if (worker == NULL) {
        return NULL;
    }

    worker->ref = 0;
    worker->count = 0;
    if (queue_init(&worker->queue, cap) == 0) {
        (void)fprintf(stderr, "queue_init failed");
        unref_worker(worker);
        return NULL;
    }

    spinlock_init(&worker->lock);
    for (int i = 0; i < count; i++) {
        if (pthread_create(&worker->threads[i], NULL, thread_worker, worker)) {
            (void)fprintf(stderr, "Create thread failed");
            unref_worker(worker);
            return NULL;
        }
    }

    worker->ref   = 1;
    worker->count = count;
    return worker;
}

void ref_worker(worker_t* worker)
{
    spinlock_lock(&worker->lock);
    ++worker->ref;
    spinlock_unlock(&worker->lock);
}

int unref_worker(worker_t* worker)
{
    spinlock_lock(&worker->lock);
    int ref = --worker->ref;
    spinlock_unlock(&worker->lock);

    if (ref != 0) {
        return ref;
    }

    queue_quit(&worker->queue);
    for (int i = 0; i < worker->count; i++) {
        pthread_join(worker->threads[i], NULL);
    }

    work_t* head = &worker->queue.head;
    for (work_t* p = head->next; p != head;) {
        p->queue = NULL;
        spinlock_lock(&p->lock);
        p->complete = -1;
        spinlock_unlock(&p->lock);
        work_t* next = p->next;
        unref_work(p);
        p = next;
    }

    pthread_mutex_destroy(&worker->queue.mutex);
    pthread_cond_destroy(&worker->queue.cond);
    free(worker);
    return 0;
}

int ref_worker_count(const worker_t* worker)
{
    return worker->ref;
}

int add_work(worker_t* worker, work_t* work)
{
    ref_work(work);
    if (queue_push(&worker->queue, work) != 0) {
        return 1;
    }

    unref_work(work);
    return 0;
}

work_t* new_work(work_cb cb, destory_cb destory, query_cb query, void* data)
{
    work_t* p = malloc(sizeof(work_t));
    if (p == NULL) {
        return NULL;
    }

    p->data     = data;
    p->destory  = destory;
    p->cb       = cb;
    p->query    = query;
    p->queue    = NULL;
    p->complete = 0;
    p->ref      = 1;
    spinlock_init(&p->lock);
    return p;
}

void ref_work(work_t* p)
{
    spinlock_lock(&p->lock);
    ++p->ref;
    spinlock_unlock(&p->lock);
}

int unref_work(work_t* p)
{
    if (p->queue != NULL && queue_remove(p->queue, p) != 0) {
        unref_work(p);
    }

    spinlock_lock(&p->lock);
    int ref = --p->ref;
    spinlock_unlock(&p->lock);

    if (ref != 0) {
        return ref;
    }

    p->destory(p->data);
    p->data = NULL;
    free(p);
    return 0;
}

int query_work(work_t* p, void* context)
{
    int ret;
    spinlock_lock(&p->lock);
    if (p->complete > 0) {
        ret = p->query(p->data, context);
    } else if (p->complete == 0) {
        ret = p->query(NULL, context);
    } else {
        // 未完成就被移除出队列
        ret = -1;
    }
    spinlock_unlock(&p->lock);
    return ret;
}
