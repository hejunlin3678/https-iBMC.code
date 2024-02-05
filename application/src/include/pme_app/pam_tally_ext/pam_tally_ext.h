/******************************************************************************

                  版权所有 (C), 2001-2014, 华为技术有限公司

******************************************************************************
  文 件 名   : pam_tally_ext.h
  版 本 号   : 初稿
  作    者   : h00188524
  生成日期   : 2014年12月8日
  最近修改   :
  功能描述   : libpam_tally_ext.so库的对外接口
  函数列表   :
  修改历史   :
  1.日    期   : 2014年12月8日
    作    者   : h00188524
    修改内容   : 创建文件

******************************************************************************/

#ifndef __PAM_TALLY_EXT_H__
#define __PAM_TALLY_EXT_H__

#include "glib.h"

#ifdef  __cplusplus
extern "C" {
#endif

#define PAM_TALLY_LOG_DIR "/dev/shm/tallylog"
#define TALLY_FAIL_INTERVAL_ADD_SECONDS        300  /* from faillock, the record fail_interval = unlock_time + 5min */

typedef struct tallylog_ext_t {
    guint16 fail_cnt;    /* failures since last success */
    guint64 fail_time;    /* time of last failure */
} tallylog_ext_t;

gint32 get_uid_by_name(const gchar *user_name, uid_t *uid);
gint32 get_pam_tally(const gchar *user, const gchar *tally_dir, guint64 fail_interval, tallylog_ext_t *tally);
gint32 reset_pam_tally(const gchar *user, const gchar *tally_dir);
gint32 increment_pam_tally(const gchar *user, const gchar *tally_dir);

#ifdef  __cplusplus
}
#endif
#endif /* __PAM_TALLY_EXT_H__ */

