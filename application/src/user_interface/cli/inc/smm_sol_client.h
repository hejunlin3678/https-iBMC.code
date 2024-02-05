#ifndef SMM_SOL_CLIENT
#define SMM_SOL_CLIENT


enum smm_sol_request_type {
    COM_INFO,
    ACTIVATE_SOL
};

gint32 smm_sol_start_process(enum smm_sol_request_type request, guint32 blade_number, guint32 com_number,
    gchar *user_name, gchar *user_ip);

#endif