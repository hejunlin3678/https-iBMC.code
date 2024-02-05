

#ifndef __DPU_CARD_H__
#define __DPU_CARD_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 


#define DPU_CARD_DUMPINFO_MAX_LEN           255
#define DPU_IPMI_IP_ADDR_RES_DATA_LEN       10
#define DPU_IPMI_PORT_STATE_RES_DATA_LEN    9
#define DPU_IPMI_RESP_DATA_BASE_LEN         5
#define DPU_IPMI_GET_IPV4_ADDR              0
#define DPU_IPMI_GET_IPV6_ADDR              1
#define DPU_IPMI_IPV4_ADDR_LEN              4
#define DPU_IPMI_IPV6_ADDR_LEN              16

#define CPLD_OBJ_MAX_NAME_LEN               64
#define DPU_EACH_DATA_LEN                   10
#define DPU_CPLD_LOG_ROW_NUM                16
#define DPU_CPLD_LOG_COL_NUM                16
#define DPU_GET_CPLD_LOG_RETRY_TIMES        3
#define DPU_CPLD_DUMPINFO_MAX_LEN           255
#define DPU_CPLD_INFO_MAX_LEN               2000

#define DPU_CARD_IPADDR_READ_LEN            32
#define DPU_CARD_IPADDR_START_REG           0x40
#define DPU_CARD_DATA_ONLY_IP_LEN           22

#define DPU_CARD_IP_UPDATED                 1
#define DPU_CARD_IP_NOT_UPDATED             0


gint32 dpu_card_dump_info(const gchar *path);
gint32 dpu_card_add_object_callback(OBJ_HANDLE object_handle);
gint32 dpu_card_del_object_callback(OBJ_HANDLE object_handle);
gint32 ipmi_get_dpucard_ipaddr(gconstpointer req_msg, gpointer user_data);
gint32 ipmi_get_dpucard_port_status(gconstpointer req_msg, gpointer user_data);
gint32 dpu_card_dump_cpld_reg(const gchar *path);
gint32 method_get_dpucard_ipaddr(OBJ_HANDLE card_obj, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 method_dpu_card_set_eid(OBJ_HANDLE object_handle, GSList* caller_info, GSList* input_list, GSList** output_list);
gint32 dpu_card_start(void);

typedef struct {
    guint8 manu_id[MANUFAC_ID_LEN]; 
    guint8 sub_cmd;                 
    guint16 selector;               
    guint16 param_len;              
    guint8 slot;                    
    guint8 ip_type;                 
} __attribute__((packed)) IPMIMSG_GET_DPUCARD_IPADDR_REQ_S;

typedef struct {
    guint8 comp_code;                   
    guint8 manu_id[MANUFAC_ID_LEN];     
    guint8 slot;                        
    gchar data[DPU_IPMI_IPV6_ADDR_LEN]; 
} IPMIMSG_GET_DPUCARD_IPADDR_RESP_S;

typedef struct {
    guint8 manu_id[MANUFAC_ID_LEN]; 
    guint8 sub_cmd;                 
    guint16 selector;               
    guint16 param_len;              
    guint8 slot;                    
    guint8 port_num;                
} __attribute__((packed)) IPMIMSG_GET_DPUCARD_PORT_STATUS_REQ_S;

typedef struct {
    guint8 comp_code;               
    guint8 manu_id[MANUFAC_ID_LEN]; 
    guint8 slot;                    
    guint8 link_status;             
} IPMIMSG_GET_DPUCARDD_PORT_STATUS_RESP_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 


#endif 
