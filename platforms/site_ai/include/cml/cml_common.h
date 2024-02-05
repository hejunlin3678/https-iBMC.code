/*******************************************************************************
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 *
 * -------------------------------------------------------------------------------
 *
 * cml_common.h
 *
 * Project Code: DOPAI
 * Module Name: Machine Learning
 * Date Created: 2021-07-21
 * Author:
 * Description: common content of cml
 *
 * -------------------------------------------------------------------------------
 * Modification History
 * DATE          NAME                       DESCRIPTION
 * -----------------------------------------------------------------------------
 *
 * ******************************************************************************/

#ifndef DOPAI_CML_COMMON_H_INCLUDE
#define DOPAI_CML_COMMON_H_INCLUDE

#include "util_data_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup cml_common
 *
 * CallBack函数结构体定义，回调于CML训练算法中的每轮迭代更新中。\n
 *
 * 用户自定义CallBack函数结构体中若包含不合法操作，可能会导致程序崩溃。\n
 * iter[IN] Number of training-iteration at current stage
 * metrics[IN] Performance of model at current stage.
 */
typedef uint8_t (*DOPAICMLCallback)(const uint32_t iter, const float metrics);

/**
 * @ingroup cml_common
 *
 * CML数据类型。
 */
#define DOPAI_CML_DataType DOPAI_DataType              /**< cml data type */
#define DOPAI_CML_BOOL DOPAI_BOOL                      /**< type bool */
#define DOPAI_CML_UINT8 DOPAI_UINT8                    /**< type uin8_t */
#define DOPAI_CML_UINT16 DOPAI_UINT16                  /**< type uin16_t */
#define DOPAI_CML_UINT32 DOPAI_UINT32                  /**< type uin32_t */
#define DOPAI_CML_UINT64 DOPAI_UINT64                  /**< type uin64_t */
#define DOPAI_CML_INT8 DOPAI_INT8                      /**< type in8_t */
#define DOPAI_CML_INT16 DOPAI_INT16                    /**< type in16_t */
#define DOPAI_CML_INT32 DOPAI_INT32                    /**< type in32_t */
#define DOPAI_CML_INT64 DOPAI_INT64                    /**< type in64_t */
#define DOPAI_CML_FLOAT DOPAI_FLOAT                    /**< type float */
#define DOPAI_CML_DOUBLE DOPAI_DOUBLE                  /**< type double */
#define DOPAI_CML_LONG_DOUBLE DOPAI_LONG_DOUBLE        /**< type long double */

/**
 * @ingroup cml_common
 *
 * 开源一致性类型。
 */
enum DOPAI_CML_ConsistencyType {
    DOPAI_CML_ConsistencyType_DEFAULT = 0,       /**< SiteAI CML自研实现 */
    DOPAI_CML_ConsistencyType_SKLEARN = 1,       /**< 开源scikit-learn实现 */
    DOPAI_CML_ConsistencyType_MIN = DOPAI_CML_ConsistencyType_DEFAULT,
    DOPAI_CML_ConsistencyType_MAX = DOPAI_CML_ConsistencyType_SKLEARN,
};

/**
 * @ingroup cml_common
 *
 * CML特殊常量。
 */
#define DOPAI_CML_RANDOMLY_INITIALIZED (-1)               /**< reserved value of randomly initializing random state */
#ifdef __cplusplus
}
#endif

#endif