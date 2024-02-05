/*******************************************************************************
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 *
 * -------------------------------------------------------------------------------
 *
 * util_data_type.h
 *
 * Project Code: DOPAI
 * Module Name: Executor
 * Date Created: 2021-04-29
 * Author: wutao
 * Description: definition of CML data type
 *
 * -------------------------------------------------------------------------------
 * Modification History
 * DATE          NAME                       DESCRIPTION
 * -----------------------------------------------------------------------------
 *
 * ******************************************************************************/

#ifndef DOPAI_C_DATA_TYPE_H_INCLUDE
#define DOPAI_C_DATA_TYPE_H_INCLUDE

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    DOPAI_BOOL,        /**< type bool */
    DOPAI_UINT8,       /**< type uint_8 */
    DOPAI_UINT16,      /**< type uint_16 */
    DOPAI_UINT32,      /**< type uint_32 */
    DOPAI_UINT64,      /**< type uint_64 */
    DOPAI_INT8,        /**< type int_8 */
    DOPAI_INT16,       /**< type int_16 */
    DOPAI_INT32,       /**< type int_32 */
    DOPAI_INT64,       /**< type int_64 */
    DOPAI_FLOAT,       /**< type folat */
    DOPAI_DOUBLE,      /**< type double */
    DOPAI_LONG_DOUBLE, /**< type long double */
    DOPAI_COMPLEX64,   /**< type complex64 */
    DOPAI_COMPLEX128,  /**< type complex128 */
    DOPAI_COMPLEX256   /**< type complex256 */
} DOPAI_DataType;

#ifdef __cplusplus
}
#endif
#endif