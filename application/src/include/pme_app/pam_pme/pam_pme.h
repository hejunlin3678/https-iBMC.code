/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : pam_pme.h
  版 本 号   : 初稿
  作    者   : maoali
  生成日期   : 2015年6月9日
  最近修改   :
  功能描述   : pam头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2015年6月9日
    作    者   : maoali
    修改内容   : 创建文件

******************************************************************************/

#ifndef __PAM_PME_H__
#define __PAM_PME_H__

#ifdef __cplusplus
extern "C" {
#endif

extern int pam_sm_ibmc_check_password(const gchar *user);

#ifdef __cplusplus
}
#endif
#endif /* __PAM_PME_H__ */

