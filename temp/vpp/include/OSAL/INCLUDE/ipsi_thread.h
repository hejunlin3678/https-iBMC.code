/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.


Filenmae   : ipsi_thread.h
Version    : 1
Author     : Jayaraghavendran K
Create: 2012/10/20
Description: This file contains all the thread related functions.
History:<author>   <date>          <desc>
*/

#ifndef __IPSI_THREAD_H__
#define __IPSI_THREAD_H__

#include "ipsi_types.h"
#include "ipsi_misc.h"

#ifdef __cplusplus
extern "C" {
#endif


/* Pointer to read write lock */
typedef void *ipsi_rwlock_t;

/* Pointer to thread id */
typedef void *ipsi_thread_id_t;

typedef void *ipsi_cond_var_t;
typedef void *ipsi_mutex_t;
typedef void *ipsi_thread_handle_t;

/*
    @brief        The ipsi_rwlock_create function creates the read/write lock
                  and initializes it.

    @param[out]   lock   An initialised read/write lock pointer.

    @retval       IPSI_SUCCESS  On Success
    @retval       IPSI_FAILURE  On Failure
*/
IPSILINKDLL int ipsi_rwlock_create(ipsi_rwlock_t *lock);

/*
    @brief        The ipsi_rwlock_destroy function destroys the read/write lock.

    @param[in]    lock  lock variable which has been created using
                        ipsi_rwlock_create function.

    @retval       IPSI_SUCCESS  On Success
    @retval       IPSI_FAILURE  On Failure
*/
IPSILINKDLL int ipsi_rwlock_destroy(ipsi_rwlock_t lock);

/*
    @brief        The ipsi_rwlock_rdlock functions locks a read/write lock
                  for reading,blocks until the lock can be acquired.

    @param[in]    lock  lock variable which has been created using
                              ipsi_rwlock_create function.

    @retval       IPSI_SUCCESS  On Success
    @retval       IPSI_FAILURE  On Failure
*/
IPSILINKDLL int ipsi_rwlock_rdlock(ipsi_rwlock_t lock);

/*
    @brief        The ipsi_rwlock_tryrdlock function attempts to lock a
                  read/write lock for reading without blocking if the lock
                  is unavailable.

    @param[in]    lock           lock variable which has been created using
                                 ipsi_rwlock_create function.

    @retval       IPSI_SUCCESS  On Success
    @retval       IPSI_FAILURE  On Failure
*/
IPSILINKDLL int ipsi_rwlock_tryrdlock(ipsi_rwlock_t lock);

/*
    @brief        The ipsi_rwlock_wrlock function locks a read/write lock
                  for writing,blocks until the lock can be acquired.

    @param[in]    lock           lock variable which has been created using
                                 ipsi_rwlock_create function.

    @retval       IPSI_SUCCESS  On Success
    @retval       IPSI_FAILURE  On Failure
*/
IPSILINKDLL int ipsi_rwlock_wrlock(ipsi_rwlock_t lock);

/*
    @brief        The ipsi_rwlock_trywrlock function attempts to lock a
                  read/write lock for writing without blocking if the lock
                  is unavailable.

    @param[in]    lock           lock variable which has been created using
                                 ipsi_rwlock_create function.

    @retval       IPSI_SUCCESS  On Success
    @retval       IPSI_FAILURE  On Failure
*/
IPSILINKDLL int ipsi_rwlock_trywrlock(ipsi_rwlock_t lock);

/*
    @brief        The ipsi_rwlock_unlock unlocks the read/write lock.

    @param[in]    lock           lock variable which has been created using
                                 ipsi_rwlock_create function.
    @retval       IPSI_SUCCESS  On Success
    @retval       IPSI_FAILURE  On Failure
*/
IPSILINKDLL int ipsi_rwlock_unlock(ipsi_rwlock_t lock);

/*
    @brief        The ipsi_get_thread_id function will return the Thread ID of the
                  current thread.

    @param[in]    NA

    @retval       ipsi_thread_id_t Thread ID of the current Thread.
*/
IPSILINKDLL ipsi_thread_id_t ipsi_get_thread_id(void);

/*
    @brief        The ipsi_thread_create function will create the thread.

    @param[in]    pThread          Thread to be spawned.
                       pFnstartRoutine  Rotine to be executed by the thread
                       pvArg               Thread input argument.
                       pThreadHandle   Thread handle.

    @retval       IPSI_SUCCESS     On Success
    @retval       IPSI_FAILURE       On Failure
*/
IPSILINKDLL SEC_INT ipsi_thread_create(ipsi_thread_id_t *pThread, SEC_VOID *(*pFnstartRoutine)(SEC_VOID *arg),
                                       const SEC_VOID *pvArg, ipsi_thread_handle_t *pThreadHandle);

/*
    @brief           The ipsi_thread_close function will close the thread.

    @param[in]    ThreadId          Thread to close.
    @param[in]    ThreadHandle Pointer to handle.

    @retval          NA

*/
IPSILINKDLL SEC_VOID ipsi_thread_close(ipsi_thread_id_t ThreadId, ipsi_thread_handle_t ThreadHandle);

/*
    @brief         The ipsi_createCondVar function cretes conditional variable.

    @param[in]  pcondvar         Pointer to conditional var

    @retval       IPSI_SUCCESS  On Success
    @retval       IPSI_FAILURE    On Failure
*/
IPSILINKDLL SEC_INT ipsi_createCondVar(ipsi_cond_var_t *pcondvar);

/*
    @brief        The ipsi_condTimedwait waits until the timeout or get the signal

    @param[in]    timeout        Wait time in seconds, timeout should be non -ve valid input.
    @param[in]    condvar        Pointer to conditional var.
    @param[in]    condmutex    Pointer to mutex.


    @retval       IPSI_SUCCESS  On Success
    @retval       IPSI_FAILURE    On Failure
*/
IPSILINKDLL SEC_INT ipsi_condTimedwait(SEC_INT timeout, ipsi_cond_var_t condvar, ipsi_mutex_t condmutex);

/*
    @brief        The ipsi_condTimedwaitMs waits until the timeout or get the signal

    Note : Implemented Platforms:dopra dopraV2 windows
           Unimplemented Platforms:linux rtos_ck vxworks, The behavior is the same as ipsi_condTimedwait.

    @param[in]    timeout        Wait time in milliseconds, timeout should be between [100, 60000].
                                 The precision is 100 ms.
    @param[in]    condvar        Pointer to conditional var.
    @param[in]    condmutex    Pointer to mutex.


    @retval       IPSI_SUCCESS  On Success
    @retval       IPSI_FAILURE    On Failure
*/
IPSILINKDLL SEC_INT ipsi_condTimedwaitMs(SEC_INT timeout, ipsi_cond_var_t condvar, ipsi_mutex_t condmutex);

/*
    @brief         The ipsi_condSignal waits until the timeout or get the signal

    @param[in]  condvar          Pointer to conditional var

    @retval       IPSI_SUCCESS  On Success
    @retval       IPSI_FAILURE    On Failure
*/
IPSILINKDLL SEC_INT ipsi_condSignal(ipsi_cond_var_t condvar);

/*
    @brief           The ipsi_deleteCondVar waits until the timeout or get the signal

    @param[in]    condvar            Pointer to conditional var


    @retval         IPSI_SUCCESS   On Success
    @retval         IPSI_FAILURE     On Failure
*/
IPSILINKDLL SEC_INT ipsi_deleteCondVar(ipsi_cond_var_t condvar);

/*
    @brief        The API ipsi_thread_create_ex is to create the new thread

    @param[in]   pFnstartRoutine  Pointer to start Routine.
    @param[in]   pvArg               Arguments to pFnstartRoutine.
    @param[in]   pThread            Poninter to thread.

    @retval        IPSI_SUCCESS     On Success
    @retval        IPSI_FAILURE       On Failure
*/
IPSILINKDLL SEC_INT ipsi_thread_create_ex(SEC_VOID *(*pFnstartRoutine)(SEC_VOID *arg), SEC_VOID *pvArg,
                                          ipsi_thread_id_t *pThread);

/*
    @brief         The API ipsi_thread_close_ex is used to closed thread.

    @param[in]  ThreadId Thread to close.

    @retval  NA
*/
IPSILINKDLL SEC_VOID ipsi_thread_close_ex(ipsi_thread_id_t ThreadId);

/*
    @brief        The API ipsi_opensemaphore creates/opens the named semaphore object

    @param      pcString       The name of the semaphore
    @param      pSemHandlem    The semaphore handle
    @param      piError        The open semaphore error
    @param      piCloseStatus  The close semaphore status

    @retval      IPSI_SUCCESS  On Success
    @retval      IPSI_FAILURE  On Failure
*/
IPSILINKDLL int ipsi_opensemaphore(const char *pcString, ipsi_sem_handle_t *pSemHandlem, int *piError,
                                   int *piCloseStatus);

/*
    @brief        The API ipsi_closesemaphore Closes the semaphore object

    @param      pcString      The name of the semaphore
    @param      pSemHandlem   The semaphore handle

    @retval      IPSI_SUCCESS  On Success
    @retval      IPSI_FAILURE  On Failure
*/
IPSILINKDLL int ipsi_closesemaphore(const char *pcString, ipsi_sem_handle_t pSemHandlem);

/*
    @brief         ipsi_create_atomic_glbInitLock

    @param       NA

    @retval       IPSI_SUCCESS  On Success
    @retval       IPSI_FAILURE  On Failure
*/
IPSILINKDLL int ipsi_create_atomic_glbInitLock(void);

/*
    @brief         ipsi_acquire_glbInitLock

    @param       NA

    @retval       IPSI_SUCCESS  On Success
    @retval       IPSI_FAILURE  On Failure
*/
IPSILINKDLL int ipsi_acquire_glbInitLock(void);

/*
    @brief         ipsi_release_glbInitLock

    @param       NA

    @retval       IPSI_SUCCESS  On Success
    @retval       IPSI_FAILURE  On Failure
*/
IPSILINKDLL int ipsi_release_glbInitLock(void);

#ifdef __cplusplus
}
#endif
#endif
