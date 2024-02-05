#ifndef IPMCGETMAINT_H
#define IPMCGETMAINT_H
#define MAX_PARAMETER 9

extern gint32 ipmc_main(gint32 argc, gchar **argv);
extern void ipmc_data_info(guchar l_level, guchar level, gchar *location, gchar *target, gchar *cmd, gint32 argc,
    gchar **argv);
#endif
