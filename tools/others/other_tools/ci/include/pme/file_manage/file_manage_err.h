#ifndef FILE_MANAGE_ERR_H
#define FILE_MANAGE_ERR_H

#include <pme/common/mscm_err.h>
#include <glib.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef enum tag_fm_error
{
    FM_OK = 0,
    FM_NO_EVENT,                            /* 没有文件事件 */
    FM_ERROR_PARA_0_INVALID = DEF_ERROR_NUMBER(ERR_MODULE_ID_FM, 0),        /* 第1参数无效 */
    FM_ERROR_PARA_1_INVALID,                /* 第2参数无效 */
    FM_ERROR_PARA_2_INVALID,                /* 第3参数无效 */
    FM_ERROR_PARA_3_INVALID,                /* 第4参数无效 */
    FM_ERROR_PARA_4_INVALID,                /* 第5参数无效 */
    FM_ERROR_PARA_OTHER_INVALID,            /* 其他输入参数（比如从全局变量输入的参数）无效 */
    FM_ERROR_SYNC_REGISTER_FAIL,            /* 文件同步注册失败 */
    FM_ERROR_UNSUPPORT_PROTECT_MODE,        /* 不支持的文件保护模式 */
    FM_ERROR_MALLOC_FAIL,                   /* 资源分配失败 */
    FM_ERROR_NO_SYNC_REGISTER_INFO,         /* 没有文件同步注册信息 */
    FM_ERROR_ITEM_INIT_FAIL,                /* 文件管理项目初始化失败 */
    FM_ERROR_FILE_OPEN_FAIL,                /* 文件打开失败 */
    FM_ERROR_FILE_LOCK_FAIL,                /* 文件加锁失败 */
    FM_ERROR_FILE_WRITE_FAIL,               /* 文件写入失败 */
    FM_ERROR_FILE_READ_FAIL,                /* 文件读取失败 */
    FM_ERROR_FILE_NOT_EXIST,                /* 文件不存在 */
    FM_ERROR_FILE_CRC_INVALID,              /* 文件CRC校验错误 */
    FM_ERROR_FILE_COPY_FAIL,                /* 文件拷贝失败 */
    FM_ERROR_SET_CRC_FAIL,                  /* 文件写入CRC校验信息失败 */
    FM_ERROR_NO_CRC_DATA,                   /* 文件CRC校验数据丢失 */
    FM_ERROR_DESTINATE_FILE_INVALID,        /* 目标文件检查错误 */
    FM_ERROR_FILE_PACKAGE_FAIL,             /* 文件打包失败 */
    FM_ERROR_INOTIFY_FAIL,                  /* INOTIFY出现异常 */
    FM_ERROR_NO_RECOVER_POINT,              /* 没有出厂设置还原点 */
    FM_ERROR_RECOVER_FAIL,                  /* 设置恢复失败 */
    FM_ERROR_NO_CALLBACK_OBJ,               /* 不存在回调对象 */
    FM_ERROR_CALLBACK_FAIL,                 /* 回调失败 */
    FM_ERROR_SET_PRIVILEGE_FAIL,            /* 设置权限失败 */
    FM_ERROR_REBOOT_CAUSE,                  /* 重启导致操作失败 */
}FM_ERROR_E;

/* BEGIN: Added by licheng, 2015/5/8 16:35:40   问题单号:DTS2015050705315 */
#define BASE_NAME(name) ((g_strrstr((name), "/") != NULL) ? (g_strrstr((name), "/") + 1) : (name))
/* END:   Added by licheng, 2015/5/8 16:35:43 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif