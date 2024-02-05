 

#include "pme/pme.h"
#include "pme_app/pme_app.h"

#include "pme_app/common/debug_log_macro.h"
#include "ar_card.h"

gint32 update_ar_card_id(OBJ_HANDLE obj_handle, gpointer data)
{
    gint32 ret;
    const guint32 offset = 1024; // ID保存在eeprom的偏移为1024
    guint8 frudata[MAC_LEN] = { 0 };
    gchar mac_addr[MACADDRESS_LEN + 1] = { 0 };

    for (guint8 i = 0; i < 5; i++) { // 最多重试5次
        ret = dfl_block_read(obj_handle, PROPERTY_ARCARD_ID_STRG_MD, offset, sizeof(frudata), frudata);
        if (ret == DFL_OK) {
            break;
        }
        vos_task_delay(200); // 失败等待200ms
    }
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s] Read %s.%s eeprom failed!(ret:%d).", __func__, dfl_get_object_name(obj_handle),
            PROPERTY_ARCARD_ID_STRG_MD, ret);
        return ret;
    }
    // 产品用该字段用于唯一标识一个AR卡，所以属性名为"ID"
    // 实际存储的数据为AR卡对应的MAC地址，这里拼接一个MAC地址
    ret = snprintf_s(mac_addr, sizeof(mac_addr), sizeof(mac_addr) - 1, "%02X:%02X:%02X:%02X:%02X:%02X",
        frudata[0], frudata[1], frudata[2], frudata[3], frudata[4], frudata[5]); // 0 1 2 3 4 5为数组下标
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "[%s] snprintf_s failed.(ret:%d)", __func__, ret);
        return RET_ERR;
    }
    debug_log(DLOG_ERROR, "[%s] %s with mac_addr: %s", __func__, dfl_get_object_name(obj_handle), mac_addr);

    ret = dal_set_property_value_string(obj_handle, PROPERTY_ARCARD_ID, mac_addr, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] Set property("PROPERTY_ARCARD_ID") failed.(ret:%d)", __func__, ret);
    }
    return ret;
}

gint32 arcard_add_object_callback(OBJ_HANDLE obj_handle)
{
    maintenance_log_v2(MLOG_INFO, FC__PUBLIC_OK, "Obj(%s) is loaded.", dfl_get_object_name(obj_handle));
    
    if (is_smm_active() == FALSE) {
        return RET_OK;
    }

    return update_ar_card_id(obj_handle, NULL);
}

gint32 arcard_del_object_callback(OBJ_HANDLE obj_handle)
{
    return RET_OK;
}

