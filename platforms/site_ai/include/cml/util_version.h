/***********************************************************************
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 *
 * -----------------------------------------------------------------------
 * util_version.h
 *
 * Project Code:  util
 * Module Name :  DOPAI
 * Create Date :  2020-09-08
 * Version     :  Initial Draft
 * Author      :  wutao
 * Description :  SiteAI版本号信息
 * -----------------------------------------------------------------------
 * Modification History
 * DATE        NAME             DESCRIPTION
 * ----------------------------------------------------------------------
 * 2020-09-08 gaofan            Created
 * ***********************************************************************/
#ifndef DOPAI_VERSION_H
#define DOPAI_VERSION_H

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup DOPAIVersion DOPAI版本信息接口 */
#define SITEAI_VERSION "SiteAI V100R023C00B500"
/**
 * @ingroup  DOPAIVersion
 * @brief 获取DOPAI版本信息的接口
 *
 * @par 描述:
 * DOPAI版本信息接口，调用后可以在日志中获得SiteAI版本信息, 并返回版本字符串
 * @par 依赖
 * <ul><li>util_version.h：该接口声明所在的头文件。</li></ul>
 * @since XX
 */
const char *DOPAI_VERSION(void);

#ifdef __cplusplus
}
#endif

#endif /* DOPAI_VERSION_H */
