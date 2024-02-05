#ifndef __VOS_ERRORCODE_H__
#define __VOS_ERRORCODE_H__

#ifdef __cplusplus
 #if __cplusplus
extern "C" {
#endif
#endif

/*下面的枚举定义了系统中的模块列表*/
enum ERR_MODULE_ID_ENUM
{
    ERR_MODULE_ID_CORE = 0,
    ERR_MODULE_ID_UIP  = 1,
    ERR_MODULE_ID_CLI  = 2,
	ERR_MODULE_ID_CIM  = 3,
    ERR_MODULE_ID_FM   = 4, /* 文件管理 */
    ERR_MODULE_ID_PER  = 5, /* 持久化 */
    ERR_MODULE_ID_NET  = 6, /* 网络配置管理 */
    ERR_MODULE_ID_FRU  = 7, /* FRUDATA */
    ERR_MODULE_ID_MD   = 8, /* 维护调测模块 */
    ERR_MODULE_ID_PSM  = 9, /* 电源管理模块 */
    ERR_MODULE_ID_SCAN  = 10, /* 环境监控模块 */
    ERR_MODULE_ID_BUTT
};

#define ERR_MODULE_RANGE_SIZE 1000000 /*给每个模块分配的错误码数*/

/*由模块ID + 错误ID 计算错误码*/
#define DEF_ERROR_NUMBER(module_id, err_num) (-1 * ((module_id) * ERR_MODULE_RANGE_SIZE + (err_num)))
#ifdef __cplusplus
 #if __cplusplus
}
#endif
#endif
#endif
