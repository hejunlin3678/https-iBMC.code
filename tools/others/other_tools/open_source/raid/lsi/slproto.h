//This header will be used with StoreLib library

//All function definitions must be within the extern C block below for
//allowing C applications to access
#ifndef __SLPROTO_H__
#define __SLPROTO_H__

#ifdef __cplusplus
extern "C" {
#endif
/******************************************************************************
Function Name: ProcessLibCommand
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
U32 ProcessLibCommand(SL_LIB_CMD_PARAM_T *pLcp); 
#ifdef __cplusplus
} //extern "C"
#endif
#endif //__SLPROTO_H__
