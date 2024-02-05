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

#ifndef __WORKER_H__
#define __WORKER_H__

typedef struct _worker worker_t;
typedef struct _work   work_t;

typedef void (*work_cb)(void* data);
typedef void (*destory_cb)(void* data);
typedef int (*query_cb)(void* data, void* context);

worker_t* new_worker(int count, int cap);
void      ref_worker(worker_t* worker);
int       unref_worker(worker_t* worker);
int       ref_worker_count(const worker_t* worker);
int       add_work(worker_t* worker, work_t* work);

work_t* new_work(work_cb cb, destory_cb destory, query_cb query, void* data);
void    ref_work(work_t* p);
int     unref_work(work_t* p);
int     query_work(work_t* p, void* context);

#endif  // __WORKER_H__
