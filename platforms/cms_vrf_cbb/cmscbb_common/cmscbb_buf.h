/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2015-2018. All rights reserved.
 * Description: Signature Verify CBB Library
 * Author: tangzonglei
 * Create: 2015
 * History: 2018/11/23 yebin code rule fixes
 */
#ifndef H_CMSCBB_BUF_H
#define H_CMSCBB_BUF_H
#include "cmscbb_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* End OF Buffer */
#define CMSCBB_EOB (CVB_UINT32)(-1)

typedef enum CmscbbBufSeekWayEm {
    CBSW_SET = 0,
    CBSW_CURRENT = 1
} CmscbbBufSeekWay;

typedef struct CmscbbBufSt {
    const CVB_BYTE* val;
    CVB_UINT32 bufLen;
    CVB_UINT32 cursor;
} CmscbbBuf;

/*
 * Prototype    : CmscbbBufInit
 * Description  : Initialize read-only buffer with memory data.
 * Params
 *   [IN] buf: buffer
 *   [IN] val: valid
 *   [IN] valLen: valid length
 * Return Value : CMSCBB_ERROR_CODE
 * Remarks      : extern memory data is attached into this buffer,
 *   so don't free the memory before buffer destroy.
 *   Date              Author     Modification
 *   2015/11/10 19:17  tangzonglei  Create
 */
CMSCBB_ERROR_CODE CmscbbBufInit(CmscbbBuf* buf, const CVB_BYTE* val, CVB_UINT32 valLen);

/*
 * Prototype    : CmscbbBufGet
 * Description  : get 1 byte from the buffer.
 * Params
 *   [IN] buf: buffer
 *   [IN] byte: byte
 * Return Value : CMSCBB_ERROR_CODE
 * Remarks      : buffer's read point will increase by 1.
 *   Date              Author     Modification
 *   2015/11/10 19:19  tangzonglei  Create
 */
CMSCBB_ERROR_CODE CmscbbBufGet(CmscbbBuf* buf, CVB_BYTE* byte);

/*
 * Prototype    : CmscbbBufSeek
 * Description  : Change the current read point.
 * Params
 *   [IN] buf: buffer
 *   [IN] pos: position
 *   [IN] way: way to seek the buffer
 * Return Value : CMSCBB_ERROR_CODE
     Date               Author      Modification
 *   2015/11/10 19:22  tangzonglei  Create
 */
CMSCBB_ERROR_CODE CmscbbBufSeek(CmscbbBuf* buf, CVB_INT32 pos, CmscbbBufSeekWay way);

#ifdef __cplusplus
}
#endif

#endif /* H_CMSCBB_BUF_H */
