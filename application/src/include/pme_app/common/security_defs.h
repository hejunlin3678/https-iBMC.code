/* 
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2020. All rights reserved. 
* Description: 安全策略相关的定义。
* Author: h00282621 
* Create: 2020-3-10 
* Notes: 用于跨模块使用的相关的定义。
*/

#ifndef __SECURITY_DEFINE_H__
#define __SECURITY_DEFINE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


/*********禁用长时间不活动用户的时间周期********/
#define SECURITYENHANCE_INACTIVEUSERTIME_MIN  30
#define SECURITYENHANCE_INACTIVEUSERTIME_MAX  365
#define SECURITYENHANCE_INACTIVEUSERTIME_DEFAULT 0   /* 0表示不开启禁用长时间不活动用户的功能*/

#define INACTIVE_USER_YES              1             /* 禁用长时间不活动用户 */
#define INACTIVE_USER_NO               0             /* 不禁用用户 */

#define SECURITYENHANCE_AUTOUPDATEINTERVAL_DEFAULT 0
#define SECURITYENHANCE_AUTOUPDATEINTERVAL_MIN 1
#define SECURITYENHANCE_AUTOUPDATEINTERVAL_MAX 365

#define COMPLEX_CHECK_OFF 0
#define COMPLEX_CHECK_ON  1


#define USER_MODIFY_PASSWORD_WARNING_DAYS 10

#define MOTD_FILE_MAX_LEN 1024  /* ssh服务banner文件最大长度 */

/**************工作密钥相关定义*****************/
#define COMPONENT_SIZE           16
#define ENCRYPT_WORKKEY_SIZE     32
#define ENCRYPT_WORKKEY_IV_SIZE  16
#define ENCRYPT_WORKKEY_STR_SIZE (2 * ENCRYPT_WORKKEY_SIZE + 2 * ENCRYPT_WORKKEY_IV_SIZE + 1 + 1)
#define PLAIN_WORKKEY_SZIE       128

#define MAX_ENCRYPTO_DATA_LEN    256

#define LEN_AES_IV         16
#define LEN_AES_PADDING    16

/*保密性算法 encrypt Arithmetic*/
#define ENCRYPT_ARITH_XRC4_128 2       /* xRC4-128 */
#define ENCRYPT_ARITH_XRC4_40  3       /* xRC4-40 */


/* 登录规则 */
#define MAX_RULE_COUNT 3
#define MAX_RULE_VALUE    7     //000~111 代表三种登录规则的任意组合


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __SECURITY_DEFINE_H__ */
