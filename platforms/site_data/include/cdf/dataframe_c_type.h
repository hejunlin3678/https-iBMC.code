/*******************************************************************************
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 *
 * -------------------------------------------------------------------------------
 *
 * dataframe_c_type.h
 *
 * Project Code: DOPAI
 * Module Name: Type
 * Date Created: 2020-07-25
 * Author:
 * Description: 类型及枚举定义
 *
 * -------------------------------------------------------------------------------
 * Modification History
 * DATE          NAME                       DESCRIPTION
 * -----------------------------------------------------------------------------
 *
 * ***************************************************************************** */
#ifndef DOPAI_DATAFRAME_TYPE
#define DOPAI_DATAFRAME_TYPE

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup  CDataFrame
 *
 * CDataFrame支持的数据类型ID
 */
typedef enum {
    CDATAFRAME_BOOL = 0, // bool
    CDATAFRAME_INT8 = 1,
    CDATAFRAME_UINT8 = 2,
    CDATAFRAME_INT16 = 3,
    CDATAFRAME_UINT16 = 4,
    CDATAFRAME_INT32 = 5,
    CDATAFRAME_UINT32 = 6,
    CDATAFRAME_INT64 = 7,
    CDATAFRAME_UINT64 = 8,
    CDATAFRAME_FLOAT = 9,
    CDATAFRAME_DOUBLE = 10,
    CDATAFRAME_STRING = 11, // std::string
    CDATAFRAME_DATETIME = 12,
    CDATAFRAME_DATAFRAME = 13, // DataFrame
    CDATAFRAME_TYPE_END = 14,
    CDATAFRAME_TYPE_BEGIN = 0,
    CDATAFRAME_INVALID_TYPE = -1,
    CDATAFRAME_MIX_TYPE = CDATAFRAME_TYPE_END + 1,
} DOPAI_CDATAFRAME_TYPEID;

/**
 * @ingroup  CDataFrame
 *
 * CDataFrame Merge支持的JoinType
 */
typedef enum {
    INNER = 0,
    RIGHT = 1,
    LEFT = 2,
    OUTTER = 3,
} DOPAI_CDATAFRAME_MERGE_JOINTYPE;

#ifdef __cplusplus
}
#endif

#endif