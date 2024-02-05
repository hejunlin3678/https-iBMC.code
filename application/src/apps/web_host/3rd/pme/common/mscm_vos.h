#ifndef _IPMC_VOS_H_
#define _IPMC_VOS_H_
#include <glib.h>

extern gint32 vos_file_copy(const gchar* desfilepath, const gchar* sourcepath);
extern gint32 vos_rm_filepath(const gchar* path);
extern glong vos_get_file_length(const gchar* filename);
extern gulong vos_get_cur_time_stamp(void);
extern gint32 vos_get_file_accessible(const gchar* filename);
extern gint vos_system_s(const gchar* path, const gchar* cmdstring[]);
extern gint32 vos_get_random_array(guchar* buffer, gint32 length);

#endif /* _IPMC_VOS_H_ */
