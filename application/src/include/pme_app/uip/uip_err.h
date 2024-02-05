#ifndef __UIP_ERRORCODE_H__
#define __UIP_ERRORCODE_H__


#ifdef __cplusplus
extern "C" {
#endif

/*UIP模块的功能列表*/
enum UIP_FUNC_ID_ENUM
{
    UIP_COMMON  = 0,  
	UIP_FUNC_OBJECT_INTERFACE  = 1,  
	UIP_FUNC_USER_MANAGE  = 2,  
	UIP_FUNC_XXX3  = 3,  
    UIP_FUNC_OBJECT_BUTT
};

/*每个功能分配1000个错误码*/
#define UIP_ERR_FUNC_RANGE_SIZE	1000

/*由功能ID + 错误ID 计算错误码*/
#define UIP_FUNC_DEF_ERR(func_id, err_num) DEF_ERROR_NUMBER(ERR_MODULE_ID_UIP, ((func_id * UIP_ERR_FUNC_RANGE_SIZE) + err_num))

#define UIP_COMMON_DEF_ERR(func_id, err_num) UIP_FUNC_DEF_ERR(UIP_COMMON, err_num)
#define UIP_OBJECT_INTERFACE_DEF_ERR(func_id, err_num) UIP_FUNC_DEF_ERR(UIP_FUNC_OBJECT_INTERFACE, err_num)
#define UIP_XXX2_DEF_ERR(func_id, err_num) UIP_FUNC_DEF_ERR(UIP_FUNC_XXX2, err_num)
#define UIP_XXX3_DEF_ERR(func_id, err_num) UIP_FUNC_DEF_ERR(UIP_FUNC_XXX3, err_num)

//UIP_COMMON_DEF_ERR 定义基本的，所有功能都通用的错误码.
#define UIP_ERR_PARA_INVALID                        UIP_COMMON_DEF_ERR(UIP_COMMON, 0)
#define UIP_ERR_MALLOC_FILED                        UIP_COMMON_DEF_ERR(UIP_COMMON, 1)
#define UIP_ERR_JSON_STR_ERR                        UIP_COMMON_DEF_ERR(UIP_COMMON, 2)
#define UIP_ERR_GET_PROTERY                         UIP_COMMON_DEF_ERR(UIP_COMMON, 3)
#define UIP_ERR_JSON_GV2JSON_FAILED                 UIP_COMMON_DEF_ERR(UIP_COMMON, 4)
#define UIP_ERR_JSON_JSON2GV_FAILED                 UIP_COMMON_DEF_ERR(UIP_COMMON, 5)
#define UIP_ERR_GV_TYPE_MISMATCH                    UIP_COMMON_DEF_ERR(UIP_COMMON, 6)
#define UIP_ERR_UNSUPPORT_JSON_TYPE                 UIP_COMMON_DEF_ERR(UIP_COMMON, 7)
#define UIP_ERR_GET_SEL_FAILED                      UIP_COMMON_DEF_ERR(UIP_COMMON, 8)
#define UIP_ERR_GET_SEL_COL_NUM_ERR                 UIP_COMMON_DEF_ERR(UIP_COMMON, 9)




//用户管理
#define UIP_ERR_INVALID_USERNAME                    UIP_COMMON_DEF_ERR(UIP_FUNC_USER_MANAGE, 0)
#define UIP_ERR_INVALID_USERTYPE                    UIP_COMMON_DEF_ERR(UIP_FUNC_USER_MANAGE, 1)
#define UIP_ERR_AUTH_FAILED				            UIP_COMMON_DEF_ERR(UIP_FUNC_USER_MANAGE, 2)

#ifdef __cplusplus
}
#endif
#endif
