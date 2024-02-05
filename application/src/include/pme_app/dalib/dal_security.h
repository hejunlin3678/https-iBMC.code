/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2021. All rights reserved.
 * Description: BMC安全模块的公共接口
 * Author: p00483289
 * History: 2021年11月09日 p00483289 创建文件
 */

#ifndef __DAL_SECURITY_H__
#define __DAL_SECURITY_H__

#include "pme/pme.h"
#include "pme_app/pme_app.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define SSH_SERVICE_START    1
#define SSH_SERVICE_STOP     2
#define SSH_SERVICE_RESTART  3

gchar* dal_get_ssl_ciphersuites(void);
gboolean dal_check_first_login_plcy_valid(guint8 first_login_plcy);
void *ssh_server_reset(gchar *option_desc);
void dal_ssh_service_control(const guint8 option, const gboolean updated, const gchar* desc);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* __DAL_SECURITY_H__ */