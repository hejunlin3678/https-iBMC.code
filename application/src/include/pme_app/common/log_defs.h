/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2020. All rights reserved.
* Description: 日志管理相关的定义。
* Author: h00282621
* Create: 2020-3-10
* Notes: 用于跨模块使用的相关的定义。
*/

#ifndef __LOG_DEFINE_H__
#define __LOG_DEFINE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define APP_DEBUG_LOG_SIZE (200 * 1024)


#define RAM_IPMI_DEBUG_LOG         "/opt/pme/pram/md_so_ipmi_debug_log"
#define RAM_IPMI_MASS_OPERATE_LOG  "/opt/pme/pram/md_so_ipmi_mass_operate_log"
#define RAM_KVM_DEBUG_LOG          "/opt/pme/pram/md_so_kvm_vmm_debug_log"
#define OPERATE_LOG_PATH           "/var/log/pme/operate_log"
#define OPERATE_LOG_BAK_PATH       "/var/log/pme/operate_log.tar.gz"
#define RUN_LOG_PATH               "/var/log/pme/strategy_log"
#define SECURITY_LOG_PATH          "/var/log/pme/security_log"
#define SECURITY_LOG_BAK_PATH      "/var/log/pme/security_log.1"

#define CLEAR_OPERATION_LOG 0
#define CLEAR_RUN_LOG       1
#define CLEAR_SECURITY_LOG  2


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __LOG_DEFINE_H__ */

