/****************************************************************  
 * FILE        : slprotoir2.h
 * DESCRIPTION : StoreLibIR2's header used with StoreLibIR2 library
 * AUTHOR      : AN
 * DATE        : 03-12-2010
 ***************************************************************** 
       Copyright 2008-2014 Avago Technologies.  All rights reserved.
 *****************************************************************

 *****************************************************************
 * Revision History:
 * 03-10-2010 : AN : Initial Release.
 ******************************************************************/
#ifndef __SLPROTO_IR2_H__
#define __SLPROTO_IR2_H__

//All function definitions must be within the extern C block below for
//allowing C applications to access
#ifdef __cplusplus
extern "C" {
#endif
/******************************************************************************
Function Name: ProcessLibCommandIR2
Description: Processes the command and returns the relevant data
Parameters: Pointer to SL_LIB_CMD_PARAM_T struct containing cmd details 
Return Value: 
SL_SUCCESS                      - Success
Error returned by firmware. Please refer to firmware doc for detailed error defn.
SL_ERR_INVALID_CMD_TYPE
SL_ERR_INVALID_CMD
SL_ERR_INCORRECT_DATA_SIZE      - When data transfer is involved
SL_ERR_NULL_DATA_PTR            - When data transfer is involved
Command specific errors are described in command description
******************************************************************************/
U32 ProcessLibCommandIR2(SL_LIB_CMD_PARAM_T *pLcp); 
#ifdef __cplusplus
} //extern "C"
#endif

#endif //__SLPROTO_IR2_H__

