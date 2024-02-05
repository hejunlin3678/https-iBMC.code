/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.

FileName   : ipsi_mem.h
Version    : 1
Author     : Jayaraghavendran K
Create: 2012/10/20
Description: This file contains all the Memory Management related functions.
History:<author>   <date>          <desc>
*/

#ifndef __IPSI_MEM_H__
#define __IPSI_MEM_H__

#include "ipsi_types.h"

// Max malloc size : 512 MB
#define IPSI_MAX_OSAL_MALLOC_SIZE 536870912

#ifdef __cplusplus
extern "C" {
#endif

/*
    @brief          This function allocates memory of size bytes long
                    and places it in *ptr

    @param[out]     *ptr            Pointer to the allocated memory ,
                                    which is suitably aligned
                                    for any variable, or NULL
                                    if the request fails.
    @param[in]      uiSize            Size of the memory block to be allocated.
    @retval         IPSI_SUCCESS   On Success
                    IPSI_FAILURE   On Failure
*/
IPSILINKDLL int ipsi_malloc(void **ptr, size_t uiSize);

/*
    @brief          ipsi_free frees the memory space pointed to by ptr,
                    which must have been returned by a previous call to
                    ipsi_malloc, otherwise,if ipsi_free(ptr) has already
                    been called before, undefined behavior occurs.
                    If ptr is NULL no operation is performed.

    @param[in]      *ptr    Pointer to the allocated memory
                            which is to be freed.
    @retval         void    returns no value.
*/
IPSILINKDLL void ipsi_free(void *ptr);

/*
    @brief          This function used to set PID value for VPP security components
                       to track memory allocated and freed. Default value if not set using this
                       function is 1. This functions works only in DOPRA,DOPRAV2 and VISPDOPRAv2 OSAL.
                       This is a global function and it should be called before calling any API of VPP Infra

    @param[in]      iPid - PID value to be set.
    @retval         IPSI_SUCCESS on success. IPSI_FAILURE if PID value is less than or equal to zero.
*/
IPSILINKDLL int SEC_setMemPid(SEC_INT32 iPid);

/*
    @brief          This function allocates memory of size bytes long
                    and places it in *ptr and memsets the allocated buffer to 0

    @param[out]     *ptr            Pointer to the allocated memory ,
                                    which is suitably aligned
                                    for any variable, or NULL
                                    if the request fails.
    @param[in]      uiSize            Size of the memory block to be allocated.
    @retval         IPSI_SUCCESS   On Success
                    IPSI_FAILURE   On Failure
*/
IPSILINKDLL int ipsi_initialized_malloc(void **ptr, size_t uiSize);

#ifdef __cplusplus
}
#endif
#endif
