/* 
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2020. All rights reserved. 
* Description: KVM相关的定义。
* Author: h00282621 
* Create: 2020-3-10 
* Notes: 用于跨模块使用的相关的定义。
*/

#ifndef __KVM_DEFINE_H__
#define __KVM_DEFINE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define VIDEO_JNLP     "video jnlp file"
#define FILE_TYPE_JNLP "jnlp"

#define SCREEN_FILE_PATH    "/data/share/img/"
#define AUTO_SCREEN_FILE_PREFIX   "img"
#define AUTO_SCREEN_FILE_SUFFIX   ".jpeg"


#define IMG_PATH            "/data/share/img/manualscreen.jpeg"
#define IMG_WEB_PATH        "/dev/shm/web/manualscreen.jpeg"
#define IMG_TMP_PATH        "/tmp/manualscreen.jpeg"
#define IMG_TMP_WEB_PATH    "/tmp/web/manualscreen.jpeg"    // 供urest进行截屏文件下载


#define VIDEO_PATH_CATERROR     "/data/share/video/video_caterror.rep"
#define VIDEO_PATH_CATERROR_DELETE_RECORD    "/data/share/video/video_caterror_rep_is_deleted.info"
#define VIDEO_PATH_OSRESET     "/tmp/osreset/video_osreset.rep"
#define VIDEO_PATH_POWEROFF    "/tmp/poweroff/video_poweroff.rep"
#define VIDEO_OSRESET          "osreset video"
#define VIDEO_CATERROR         "caterror video"
#define VIDEO_POWEROFF         "poweroff video"
#define SCREEN_FILE_WEB_PATH  "/dev/shm/web/"

#define USER_KVM_LINK_MODE_SHARED           0
#define USER_KVM_LINK_MODE_PRIVATE          1
#define USER_VNC_LINK_MODE_SHARED           1   // 共享模式
#define USER_VNC_LINK_MODE_PRIVATE          0   // 独占模式
#define USER_KVM_LINK_MODE_SHARED_STR       "Shared"
#define USER_KVM_LINK_MODE_PRIVATE_STR      "Private"

#define VIDEO_UNCONNECTED     (-1001)  /* 录像未连接 */

#define VMM_CLIENT_ON_CONNECTION          0x80
#define VMM_CLIENT_CONNECT_SUCCESSFULLY   0x00
#define VMM_CLIENT_ALREADY_CONNECTED      0x01
#define VMM_CLIENT_CONNECT_FAILED         0x02
#define VMM_CLIENT_FILE_TYPE_WRONG        0x03
#define VMM_CLIENT_PROTOCOL_MISMATCH      0x04
#define VMM_CLIENT_PARAMERTER_LENGTH      0x05
#define VMM_CLIENT_HTTPS_SSL_VERIFY_FAILED 0x06

#define KVM_TIMEOUT_MIN_VALUE 0
#define KVM_TIMEOUT_MAX_VALUE 480

#define REALTIME_DISPLAY_INTERVAL 30 * 1000

// kvm状态码，前端页面依据收到的状态码做出提示
#define KVM_STAT_BUSY 0x02          // 对不起，已经达到用户最大支持数或者被管理员剔除。
#define KVM_STAT_KICK 0x07          // 您的用户信息已经发生变更，请重新登录。
#define KVM_STAT_IDEL 0x01          // 当前空闲
#define KVM_STAT_KEYERROR 0x03      // key值认证不通过
#define KVM_INVALID_FRAME 0x04      // 无效的数据帧  分辨率无效
#define KVM_NOT_SUPPORT 0x06        // 不支持非安全的KVM
#define KVM_STAT_TIMEOUT 0x11       // KVM超时

// 键值描述
#define STR_KEY_WINDOWS                         "Meta"
#define STR_KEY_BACK_SPACE                      "Backspace"
#define STR_KEY_TAB                             "Tab"
#define STR_KEY_ENTER                           "Enter"
#define STR_KEY_CLEAR                           "Clear"
#define STR_KEY_CANCEL                          ""    //没有匹配的字符串
#define STR_KEY_SHIFT                           "Shift"
#define STR_KEY_CONTROL                         "Control"
#define STR_KEY_ALT_1                           "Alt"
#define STR_KEY_ALT_2                           "AltGraph"
#define STR_KEY_PAUSE                           "Pause"
#define STR_KEY_CAPS_LOCK                       "CapsLock"
#define STR_KEY_ESCAPE                          "Escape"
#define STR_KEY_SPACE                           "Space"
#define STR_KEY_PAGE_UP                         "PageUp"
#define STR_KEY_PAGE_DOWN                       "PageDown"
#define STR_KEY_END                             "End"
#define STR_KEY_HOME                            "Home"
#define STR_KEY_LEFT                            "ArrowLeft"
#define STR_KEY_UP                              "ArrowUp"
#define STR_KEY_RIGHT                           "ArrowRight"
#define STR_KEY_DOWN                            "ArrowDown"
#define STR_KEY_COMMA_1                         ","
#define STR_KEY_COMMA_2                         "<"
#define STR_KEY_MINUS_1                         "-"
#define STR_KEY_MINUS_2                         "_"
#define STR_KEY_PERIOD_1                        "."
#define STR_KEY_PERIOD_2                        ">"
#define STR_KEY_SLASH_1                         "/"
#define STR_KEY_SLASH_2                         "?"
#define STR_KEY_0                               "0"
#define STR_KEY_1                               "1"
#define STR_KEY_2                               "2"
#define STR_KEY_3                               "3"
#define STR_KEY_4                               "4"
#define STR_KEY_5                               "5"
#define STR_KEY_6                               "6"
#define STR_KEY_7                               "7"
#define STR_KEY_8                               "8"
#define STR_KEY_9                               "9"
#define STR_KEY_BACK_QUOTE_1                    "`"
#define STR_KEY_BACK_QUOTE_2                    "~"
#define STR_KEY_SEMICOLON_1                     ";"
#define STR_KEY_SEMICOLON_2                     ":"
#define STR_KEY_QUOTE_1                         "'"
#define STR_KEY_QUOTE_2                         "\""
#define STR_KEY_EQUALS_1                        "="
#define STR_KEY_EQUALS_2                        "+"
#define STR_KEY_a                               "a"
#define STR_KEY_b                               "b"
#define STR_KEY_c                               "c"
#define STR_KEY_d                               "d"
#define STR_KEY_e                               "e"
#define STR_KEY_f                               "f"
#define STR_KEY_g                               "g"
#define STR_KEY_h                               "h"
#define STR_KEY_i                               "i"
#define STR_KEY_j                               "j"
#define STR_KEY_k                               "k"
#define STR_KEY_l                               "l"
#define STR_KEY_m                               "m"
#define STR_KEY_n                               "n"
#define STR_KEY_o                               "o"
#define STR_KEY_p                               "p"
#define STR_KEY_q                               "q"
#define STR_KEY_r                               "r"
#define STR_KEY_s                               "s"
#define STR_KEY_t                               "t"
#define STR_KEY_u                               "u"
#define STR_KEY_v                               "v"
#define STR_KEY_w                               "w"
#define STR_KEY_x                               "x"
#define STR_KEY_y                               "y"
#define STR_KEY_z        	                    "z"
#define STR_KEY_NUMPAD0_1                       "0(Numpad)"
#define STR_KEY_NUMPAD0_2                       "Insert(Numpad)"
#define STR_KEY_NUMPAD1_1                       "1"
#define STR_KEY_NUMPAD1_2                       "End(Numpad)"
#define STR_KEY_NUMPAD2_1                       "2(Numpad)"
#define STR_KEY_NUMPAD2_2                       "ArrowDown(Numpad)"
#define STR_KEY_NUMPAD3_1                       "3(Numpad)"
#define STR_KEY_NUMPAD3_2                       "PageDown(Numpad)"
#define STR_KEY_NUMPAD4_1                       "4(Numpad)"
#define STR_KEY_NUMPAD4_2                       "ArrowLeft(Numpad)"
#define STR_KEY_NUMPAD5_1                       "5(Numpad)"
#define STR_KEY_NUMPAD5_2                       "Clear(Numpad)"
#define STR_KEY_NUMPAD6_1                       "6(Numpad)"
#define STR_KEY_NUMPAD6_2                       "ArrowRight(Numpad)"
#define STR_KEY_NUMPAD7_1                       "7(Numpad)"
#define STR_KEY_NUMPAD7_2                       "Home(Numpad)"
#define STR_KEY_NUMPAD8_1                       "8(Numpad)"
#define STR_KEY_NUMPAD8_2                       "ArrowUp(Numpad)"
#define STR_KEY_NUMPAD9_1                       "9(Numpad)"
#define STR_KEY_NUMPAD9_2                       "PageUp(Numpad)"
#define STR_KEY_MULTIPLY                        "*"
#define STR_KEY_ADD                             "+"
#define STR_KEY_SUBTRACT                        "-"
#define STR_KEY_DECIMAL                         "Delete"
#define STR_KEY_DIVIDE                          "/"
#define STR_KEY_DELETE                          "Delete"
#define STR_KEY_NUM_LOCK                        "NumLock"
#define STR_KEY_SCROLL_LOCK                     "ScrollLock"
#define STR_KEY_F1                              "F1"
#define STR_KEY_F2                              "F2"
#define STR_KEY_F3                              "F3"
#define STR_KEY_F4                              "F4"
#define STR_KEY_F5                              "F5"
#define STR_KEY_F6                              "F6"
#define STR_KEY_F7                              "F7"
#define STR_KEY_F8                              "F8"
#define STR_KEY_F9                              "F9"
#define STR_KEY_F10                             "F10"
#define STR_KEY_F11                             "F11"
#define STR_KEY_F12                             "F12"
#define STR_KEY_PRINTSCREEN                     ""   // 前端不显示具体的字符串
#define STR_KEY_INSERT                          "Insert"
#define STR_KEY_OPEN_BRACKET_1                  "["
#define STR_KEY_OPEN_BRACKET_2                  "{"
#define STR_KEY_BACK_SLASH_1                    "\\"
#define STR_KEY_BACK_SLASH_2                    "|"
#define STR_KEY_CLOSE_BRACKET_1                 "]"
#define STR_KEY_CLOSE_BRACKET_2                 "]"
#define STR_KEY_CONTEXT_MENU                    "ContextMenu"
#define STR_KEY_KANJI1                          "IntlRo"
#define STR_KEY_KANJI2                          "Hiragana"
#define STR_KEY_KANJI3                          ""   // 前端不显示具体的字符串
#define STR_KEY_KANJI4                          ""   // 前端没有匹配的字符串
#define STR_KEY_KANJI5                          ""   // 前端没有匹配的字符串
#define STR_KEY_LESS                            "IntlBackslash"

#define USER_KVM_AUTO_OS_LOCK_TYPE_CUSTOM_STR    "Custom"
#define USER_KVM_AUTO_OS_LOCK_TYPE_WINDOWS_STR    "Windows"
#define AUTO_LOCK_KEY_ARRAY_LENGTH 4
#define AUTO_LOCK_KEY_MAX_LENGTH 20
#define ALL_LOCK_KEY_ARRAY_LENGTH 128

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __KVM_DEFINE_H__ */

