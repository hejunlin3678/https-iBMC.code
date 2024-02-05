/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2015-2018. All rights reserved.
 * Description: Signature Verify CBB Library
 * Author: tangzonglei
 * Create: 2015
 * History: 2018/11/23 yebin code rule fixes
 */

/*
 * File Name          : cmscbb_list.h
 * Author             : tangzonglei
 * Creation Date      : 2015/07/22 15:52:05
 * Detail Description : The list is not thread safe, so the pVrf should in the same thread.
 * History
 *      Date time           Author        Description
 *      2015/07/22 15:52    tangzonglei     new
 */
#ifndef H_CMSCBB_LIST_H
#define H_CMSCBB_LIST_H
#include "cmscbb_list_def.h"
#include "cmscbb_types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * Prototype    : CmscbbListFreeCallback
 * Description  : function type of free list item callback
 * Params
 *   [IN] CVB_VOID*: function pointer
 * Return Value : CVB_VOID
 *   Date              Author     Modification
 *   2015/11/10 17:31  tangzonglei  Create
 */
typedef CVB_VOID (*CmscbbListFreeCallback)(CVB_VOID*);

/*
 * Prototype    : CmscbbListItemCmpCallback
 * Description  : function type of list search and sort callback
 * Params
 *   [IN] pExpect: expect function
 *   [IN] pActual: actual
 * Return Value : typedef CVB_INT
 *   Date              Author     Modification
 *   2015/11/10 18:05  tangzonglei  Create
 */
typedef CVB_INT (*CmscbbListItemCmpCallback)(const CVB_VOID* expect, const CVB_VOID* actual);

/*
 * Prototype    : CmscbbListAdd
 * Description  : add item into list.
 * Params
 *   [IN] pList: cmscbb list
 *   [IN] pItem: cms item object
 * Return Value : CMSCBB_ERROR_CODE
 *   Date              Author     Modification
 *   2015/11/10 17:30  tangzonglei  Create
 */
CMSCBB_ERROR_CODE CmscbbListAdd(CmscbbListDummy* list, CVB_VOID* item);

/*
 * Prototype    : CmscbbListSort
 * Description  : sort items in the list.
 * Params
 *   [IN] pList: cmscbb list
 *   [IN] cmpCb:compare callback
 * Return Value : CMSCBB_ERROR_CODE
 *   Date              Author     Modification
 *   2015/11/10 17:30  tangzonglei  Create
 */
CMSCBB_ERROR_CODE CmscbbListSort(CmscbbListDummy* list, CmscbbListItemCmpCallback cmpCb);

/*
 * Prototype    : CmscbbListFree
 * Description  : free the list.
 * Params
 *   [IN] pList: cmscbb list
 *   [IN] freeCb: free callback
 * Return Value : CVB_VOID
 * Remarks      : all items in the list will be free through callback.
 *   Date              Author     Modification
 *   2015/11/10 17:30  tangzonglei  Create
 */
CVB_VOID CmscbbListFree(CmscbbListDummy* list, CmscbbListFreeCallback freeCb);

#define CMSCBB_LIST_ADD(list, pItem) CmscbbListAdd((CmscbbListDummy*)(CVB_VOID*)(list), pItem)
#define CMSCBB_LIST_SORT(list, cmp_cb) CmscbbListSort((CmscbbListDummy*)(CVB_VOID*)(list), (CmscbbListItemCmpCallback)(cmp_cb))
#define CMSCBB_LIST_FREE(list, free_cb) CmscbbListFree((CmscbbListDummy*)(CVB_VOID*)(list), (CmscbbListFreeCallback)(free_cb))

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* H_CMSCBB_LIST_H */
