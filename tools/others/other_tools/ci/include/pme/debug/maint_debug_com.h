#ifndef __MD_COM_H__
#define __MD_COM_H__

#ifdef __cplusplus
 #if __cplusplus
    extern "C" {
 #endif
#endif /* __cplusplus */

#include <glib.h>
#include "pme/debug/maint_debug_error.h"

#ifdef UNIT_TEST
	#ifdef LOCAL
	#undef LOCAL
	#endif
	#define LOCAL
#else
	#define LOCAL static
#endif

#define MD_MODULE_NAME  ("MaintDebug")

extern gchar *get_err_string(gint err_val);
extern gint str_to_int32(const gchar *str, gint base, gint32 *result);
extern gint str_to_uint32(const gchar *str, gint base, guint32 *result);
extern gboolean strlike_equal(const gchar *str1, const gchar *str2);
extern gboolean is_print_debug_msg(void);


#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif /* __cplusplus */

#endif

