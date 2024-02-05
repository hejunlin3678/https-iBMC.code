#ifndef _MSCM_MACRO_H_
#define _MSCM_MACRO_H_

#include <assert.h>

#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif
#endif

#ifdef UNIT_TEST
 #ifdef LOCAL
  #undef LOCAL
 #endif
 #define LOCAL
#else
 #define LOCAL static
#endif

#define LONGB0(a) ((guint8)((a) & 0xff))
#define LONGB1(a) ((guint8)(((a) >> 8) & 0xff))
#define LONGB2(a) ((guint8)(((a) >> 16) & 0xff))
#define LONGB3(a) ((guint8)(((a) >> 24) & 0xff))
#define MAKE_WORD(a, b) ((guint16)(((guint8)(b)) | (((guint16)((guint8)(a))) << 8)))
#define MAKE_DWORD(a, b, c, d) (guint32)((((guint32)((guint8)(a))) << 24) | (((guint32)((guint8)(b))) << 16) | (((guint32)((guint8)(c))) << 8) | ((guint8)(d)))

// 字序无关算法
#define LOW(a) ((guint8)((a) & 0xff))
#define HIGH(a) ((guint8)(((a) >> 8) & 0xff))


#ifdef PME_DEBUG

 #define return_val_if_fail(expr, val) if (!(expr)) {assert(expr); return val;}
 #define return_if_fail(expr) if (!(expr)) {assert(expr); return;}
 #define DFL_ASSERT(expr) assert(expr);

#else

// 使用的时候注意是否有资源未释放，有则不能用此宏
 #define return_val_if_fail(expr, val) if (!(expr)) return val
    
 #define return_if_fail(expr) if (!(expr)) return
 #define DFL_ASSERT(expr)
#endif

/* BEGIN: Modified by maoali, 2014/3/11   PN:AR-0000650354*/
#define break_if_fail(expr) if (!(expr)) break
#define continue_if_fail(expr)   if (!(expr)) continue
#define break_do_info_if_fail(expr, info) if(!(expr)) {info; break;}
/* END:   Modified by maoali, 2014/3/11 */

/*expr不成立时，执行info，返回val。info可以是一条或多条C语句，如free(p);g_printf("hello world!\n");*/
#define return_val_do_info_if_fail(expr, val, info) do { \
        if (!(expr)){\
            info; \
            return val;} \
    } while (0)

/*expr不成立时，执行info，返回。info可以是一条或多条C语句，如free(p);g_printf("hello world!\n");*/
#define return_do_info_if_fail(expr, info) do { \
        if (!(expr)){\
            info; \
            return;} \
    } while (0)

/*expr成立时，执行info，返回val。info可以是一条或多条C语句，如free(p);g_printf("hello world!\n");*/
#define do_val_if_expr(expr, val) do { \
        if (expr){\
            val;} \
    } while (0)
#ifdef __cplusplus
 #if __cplusplus
}
 #endif/* __cpluscplus */
#endif /* __cpluscplus */

#endif /*_MSCM_MACRO_H_*/
