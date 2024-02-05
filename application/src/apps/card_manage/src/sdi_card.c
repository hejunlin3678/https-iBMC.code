

#include <arpa/inet.h>
#include "pme_app/pme_app.h"
#include "../../../libs/pcie_mgmt/inc/pcie_mgmt_by_std_smbus.h"

#include "sdi_card.h"
#include "pcie_card.h"

#define INPUT_LIST_PARAM 2

LOCAL OBJ_HANDLE sdi_obj_tbl[SD100_CARD_MAX_NUM] = {SDI_HANDLE_NOT_EXIST};


LOCAL void sdi_card_uptade_slot(OBJ_HANDLE object_handle);
LOCAL gint32 set_sdi_card_referenced_chip_value(const OBJ_HANDLE object_handle, guint32 chip_addr,
    const guint8 chip_offset, const gsize chip_data_size, guint8 *in_data);
LOCAL gint32 get_sdi_card_referenced_chip_value(const OBJ_HANDLE object_handle, guint32 chip_addr,
    const guint8 chip_offset, const gsize chip_data_size, guint8 *out_data);
LOCAL guint8 get_sdi_id_by_handle(OBJ_HANDLE obj_hnd);
LOCAL void sdi_card_init_hash_table(void);
LOCAL guint32 g_ip_digit_last = 0;
LOCAL guchar ipv6_addr_digit[IPV6_IP_SIZE + 1] = {0};





gint32 get_sdi_card_handle_by_logic_num(guint8 logicalNum, OBJ_HANDLE *obj_handle)
{
    guint32 sd100_max_num = 0;

    if (!obj_handle) {
        debug_log(DLOG_ERROR, "%s:obj_handle is null!", __FUNCTION__);
        return COMP_CODE_UNKNOWN;
    }

    (void)dfl_get_object_count(CLASS_PCIE_SDI_CARD, &sd100_max_num);

    
    if (logicalNum > sd100_max_num || logicalNum == 0) {
        debug_log(DLOG_ERROR, "%s:logicalNum is out of range!", __FUNCTION__);
        return COMP_CODE_OUTOF_RANGE;
    }

    gint32 ret = dal_get_object_handle_nth(CLASS_PCIE_SDI_CARD, logicalNum - 1, obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:(%s)dal_get_object_handle_nth(%d)!", __FUNCTION__, CLASS_PCIE_SDI_CARD,
            logicalNum - 1);
        return COMP_CODE_UNKNOWN;
    }

    return RET_OK;
}


LOCAL gboolean check_hw_is_older_than_sdi_ver3(guint32 boardid)
{
    if (boardid == SDI_CARD_BOARDID_V3 || boardid == SDI_CARD_BOARDID_V5 || boardid == SDI_CARD_BOARDID_FD) {
        return TRUE;
    }

    return FALSE;
}


void sdi_card_cpuTemp_start(OBJ_HANDLE obj_hnd)
{
    guint8 status = 0xff;
    guint8 cpu_temp = 0;
    guint8 m3_status = 0xff;
    OBJ_HANDLE sdi_hnd = obj_hnd;
    OBJ_HANDLE tmp_hnd = 0;

    guint8 inner_accessor_data[M3_INNER_ADDR_DATA_SIZE] = {0x04, 0xf0, 0x00, 0xd0, 0x02, 0x00, 0x00, 0x00, 0x00};
    guint8 read_enable = READ_ENABLE;

    
    gint32 ret = dfl_get_referenced_object(sdi_hnd, PROPERTY_SDI_CARD_M3_ACCESSOR, &tmp_hnd);
    if (DFL_OK != ret) {
        debug_log(DLOG_ERROR, "%s:dfl_get_referenced_object(PROPERTY_SDI_CARD_M3_ACCESSOR) fail, ret=%d", __FUNCTION__,
            ret);
        return;
    }

    
    const gchar *m3_name = dfl_get_object_name(tmp_hnd);
    if (m3_name == NULL) {
        debug_log(DLOG_ERROR, "%s:dfl_get_object_name(%" OBJ_HANDLE_FORMAT ") fail, ret=%d", __FUNCTION__, tmp_hnd,
            ret);
        return;
    }

    
    ret = dfl_get_referenced_object(sdi_hnd, PROPERTY_SDI_CARD_CPLD_ACCESSOR, &tmp_hnd);
    if (DFL_OK != ret) {
        debug_log(DLOG_ERROR, "%s:dfl_get_referenced_object(PROPERTY_SDI_CARD_CPLD_ACCESSOR) fail, ret=%d",
            __FUNCTION__, ret);
        return;
    }

    
    const gchar *cpld_name = dfl_get_object_name(tmp_hnd);
    if (cpld_name == NULL) {
        debug_log(DLOG_ERROR, "%s:dfl_get_object_name(%" OBJ_HANDLE_FORMAT ") fail, ret=%d", __FUNCTION__, tmp_hnd,
            ret);
        return;
    }

    guint8 id = get_sdi_id_by_handle(sdi_hnd);
    if (id >= SD100_CARD_MAX_NUM) {
        debug_log(DLOG_ERROR, "%s: sdi id is out of range", __FUNCTION__);
        return;
    }

    

    

    ret = dal_get_extern_value_byte(sdi_hnd, PROPERTY_SDI_CARD_SYSTEM_STATUS, &status, DF_AUTO);
    if (ret == RET_ERR) {
        debug_log(DLOG_ERROR, "%s:dal_get_extern_value_byte(PROPERTY_SDI_CARD_SYSTEM_STATUS) fail, ret=%d",
            __FUNCTION__, ret);
        return;
    }

    if (BIOS_LOAD_FAIL == status) {
        debug_log(DLOG_INFO, "SDI(%s) Bios status isn`t up", dfl_get_object_name(sdi_hnd));
        return;
    }

    
    
    ret = dfl_chip_blkwrite(m3_name, M3_INNER_ADDR_OFFSET, M3_INNER_ADDR_DATA_SIZE, inner_accessor_data);
    if (DFL_OK != ret) {
        debug_log(DLOG_ERROR, "%s:dfl_chip_blkwrite fail, ret=%d", __FUNCTION__, ret);
        return;
    }

    
    ret = dfl_chip_blkwrite(m3_name, M3_OPERATER_OFFSET, M3_OPERATER_DATA_SIZE, &read_enable);
    if (DFL_OK != ret) {
        debug_log(DLOG_ERROR, "%s:dfl_chip_blkwrite fail, ret=%d", __FUNCTION__, ret);
        return;
    }

    
    ret = dfl_chip_blkread(m3_name, M3_IS_WORKING_OFFSET, 1, &m3_status);
    if (DFL_OK != ret) {
        debug_log(DLOG_ERROR, "%s:dfl_chip_blkread fail, ret=%d", __FUNCTION__, ret);
        return;
    }

    

    if (M3_ACTIVE != m3_status) {
        debug_log(DLOG_ERROR, "%s:fireware(m3) is inactive, status=%d", __FUNCTION__, m3_status);
        return;
    }

    
    ret = dfl_chip_blkread(m3_name, CPU_TEMP_OFFSET, 1, &cpu_temp);
    if (DFL_OK != ret) {
        debug_log(DLOG_ERROR, "%s:dfl_chip_blkread fail, ret=%d", __FUNCTION__, ret);
        return;
    }

    
    ret = dfl_chip_blkwrite(cpld_name, CPU_TEMP_CPLD_OFFSET, 1, &cpu_temp);
    if (DFL_OK != ret) {
        debug_log(DLOG_ERROR, "%s:dfl_chip_blkwrite fail, ret=%d", __FUNCTION__, ret);
        return;
    }
}


gint32 method_sdi_card_read_reg(GSList *input_list)
{
    OBJ_HANDLE obj_hnd = 0;
    guint8 read_value = 0;

    if (input_list == NULL) {
        debug_printf("param is null!\n");
        return RET_ERR;
    }

    guint32 input_len = g_slist_length(input_list);
    if (input_len != SDI_DEBUG_PARAM_NUM_3) {
        debug_printf("Usage: sdi_reg_read [logic num] [cpld addr] [offset]\nRead SDI CPLD register\n");
        return RET_ERR;
    }

    guint8 logic_num = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    guint32 cpld_addr = g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, 1));
    guint32 offset = g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, 2));

    gint32 ret = get_sdi_card_handle_by_logic_num(logic_num, &obj_hnd);
    if (ret != RET_OK) {
        debug_printf("get object handle bye logic num failed !\n");
        return ret;
    }

    ret = get_sdi_card_referenced_chip_value(obj_hnd, cpld_addr, offset, 0x01, &read_value);
    if (ret != RET_OK) {
        debug_printf("sdi_card_read_reg failed !\n");
        return ret;
    }

    debug_printf("0x%02x\n", read_value);

    return RET_OK;
}


gint32 method_sdi_card_write_reg(GSList *input_list)
{
    OBJ_HANDLE obj_hnd = 0;

    if (input_list == NULL) {
        debug_printf("param is null!\n");
        return RET_ERR;
    }

    guint32 input_len = g_slist_length(input_list);
    if (input_len != SDI_DEBUG_PARAM_NUM_4) {
        debug_printf("Usage: sdi_reg_write [logic num] [cpld addr] [offset] [value]\nWrite SDI CPLD register\n");
        return RET_ERR;
    }

    guint8 logic_num = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    guint32 cpld_addr = g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, 1));
    guint32 offset = g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, 2));
    guint8 set_value = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 3));

    gint32 ret = get_sdi_card_handle_by_logic_num(logic_num, &obj_hnd);
    if (ret != RET_OK) {
        debug_printf("get object handle bye logic num failed !\n");
        return ret;
    }

    ret = set_sdi_card_referenced_chip_value(obj_hnd, cpld_addr, offset, 0x01, &set_value);
    if (ret != RET_OK) {
        debug_printf("sdi_card_write_reg failed !\n");
        return ret;
    }

    return RET_OK;
}


static gint32 sdi_card_get_refchip_info(OBJ_HANDLE obj_hnd, const gchar *property_name, guint32 *offset, guint8 *size,
    gchar *cpld_name, guint32 name_len)
{
    OBJ_HANDLE chip_handle = 0;
    OBJ_HANDLE cpld_chip_handle = 0;

    if (property_name == NULL || offset == NULL || size == NULL || cpld_name == NULL) {
        debug_log(DLOG_ERROR, "parameter error ");
        return RET_ERR;
    }

    gint32 retv = dfl_get_referenced_object(obj_hnd, property_name, &chip_handle);
    if (DFL_OK != retv) {
        debug_log(DLOG_ERROR, "dfl_get_referenced_object(%s) fail, obj_name=%s, ret=%d", property_name,
            dfl_get_object_name(obj_hnd), retv);
        return RET_ERR;
    }

    
    retv = dfl_get_referenced_object(chip_handle, PROPERTY_CPLD_ACCESSOR_CHIP, &cpld_chip_handle);
    if (retv != DFL_OK) {
        debug_log(DLOG_ERROR, "get %s chip ret=%d", dfl_get_object_name(chip_handle), retv);
        return retv;
    }

    
    retv = dal_get_property_value_uint32(chip_handle, PROPERTY_CPLD_ACCESSOR_OFFSET, offset);
    if (retv != RET_OK) {
        debug_log(DLOG_ERROR, "get %s offset ret=%d", dfl_get_object_name(chip_handle), retv);
        return retv;
    }

    
    retv = dal_get_property_value_byte(chip_handle, PROPERTY_CPLD_ACCESSOR_SIZE, size);
    if (retv != RET_OK) {
        debug_log(DLOG_ERROR, "get %s size ret=%d", dfl_get_object_name(chip_handle), retv);
        return retv;
    }

    errno_t safe_fun_ret = strncpy_s(cpld_name, name_len, dfl_get_object_name(cpld_chip_handle),
        strlen(dfl_get_object_name(cpld_chip_handle)));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }

    return RET_OK;
}


static gint32 sdi_card_get_refchip_value(OBJ_HANDLE obj_hnd, const gchar *property_name, void *out_data,
    guint32 out_size)
{
    guint32 offset = 0;
    guint8 size = 0;
    gchar cpld_name[CPLD_OBJ_MAX_NAME_LEN + 1] = {0};

    gint32 retv = sdi_card_get_refchip_info(obj_hnd, property_name, &offset, &size, cpld_name, sizeof(cpld_name));
    if (retv != RET_OK) {
        debug_log(DLOG_ERROR, "get refchip info err obj_name=%s, ret=%d", dfl_get_object_name(obj_hnd), retv);
        return retv;
    }

    if (size > out_size) {
        debug_log(DLOG_ERROR, "read %s size err, obj_name=%s, size=%d, out_size=%d", property_name, cpld_name, size,
            out_size);
        return RET_ERR;
    }

    retv = dfl_chip_blkread((const gchar *)cpld_name, offset, size, out_data);
    if (retv != DFL_OK) {
        debug_log(DLOG_ERROR, "read %s value err obj_name=%s, ret=%d", property_name, cpld_name, retv);
        return retv;
    }

    return RET_OK;
}


static gint32 sdi_card_set_refchip_value(OBJ_HANDLE obj_hnd, const gchar *property_name, guint8 *in_data,
    guint32 in_size)
{
    guint32 offset = 0;
    guint8 size = 0;
    gchar cpld_name[CPLD_OBJ_MAX_NAME_LEN + 1] = {0};

    gint32 retv = sdi_card_get_refchip_info(obj_hnd, property_name, &offset, &size, cpld_name, sizeof(cpld_name));
    if (retv != RET_OK) {
        debug_log(DLOG_ERROR, "get refchip info err obj_name=%s, ret=%d", dfl_get_object_name(obj_hnd), retv);
        return retv;
    }

    if (size > in_size) {
        debug_log(DLOG_ERROR, "write %s value err obj_name=%s, size = %d, in_size=%d", property_name, cpld_name, size,
            in_size);
        return RET_ERR;
    }

    for (guint8 i = 0; i < size; i++) {
        retv = dfl_chip_blkwrite((const gchar *)cpld_name, offset + i, 1, in_data + i);
        if (retv != DFL_OK) {
            debug_log(DLOG_ERROR, "write %s value err obj_name=%s, ret=%d", property_name, cpld_name, retv);
            return retv;
        }
    }

    return RET_OK;
}



static gint32 sdi_card_set_ip(OBJ_HANDLE obj_hnd, OBJ_HANDLE group_obj, guint32 addrOffset, guint8 size,
    const gchar *cpld_name)
{
    guint8 i = 0;
    guint32 ip_digit = 0;
    gchar ipv4_addr[IPV4_ADDR_LEN] = {0};

    guint8 ipv4_addr_digit[IPV4_ADDR_SIZE] = {0};

    if (addrOffset == 0 || size == 0 || size > IPV4_ADDR_SIZE) {
        debug_log(DLOG_ERROR, "offset or size check err, obj_name=%s, addrOffset=%d, size=%d",
            dfl_get_object_name(obj_hnd), addrOffset, size);
        return RET_ERR;
    }

    
    gint32 retv = dal_get_property_value_string(group_obj, ETH_GROUP_ATTRIBUTE_IP_ADDR, ipv4_addr, IPV4_ADDR_LEN);
    if (retv != RET_OK) {
        debug_log(DLOG_ERROR, "dal_get_property_value_string(ETH_GROUP_ATTRIBUTE_IP_ADDR) failed, obj_name=%s, ret=%d",
            dfl_get_object_name(obj_hnd), retv);
        return RET_ERR;
    }

    (void)inet_pton(AF_INET, ipv4_addr, &ip_digit);

    for (i = 0; i < size; i++) {
        retv = dfl_chip_blkread(cpld_name, addrOffset + i, 1, ipv4_addr_digit + i);
        if (retv != DFL_OK) {
            debug_log(DLOG_ERROR, "dfl_chip_blkread fail, obj_name=%s, ret=%d", cpld_name, retv);
            return RET_ERR;
        }
    }

    guint32 ip_digit_temp =
        (guint32)MAKE_DWORD(ipv4_addr_digit[3], ipv4_addr_digit[2], ipv4_addr_digit[1], ipv4_addr_digit[0]);
    
    if ((g_ip_digit_last == ip_digit) && (ip_digit_temp == ip_digit)) {
        return RET_OK;
    }

    g_ip_digit_last = ip_digit;

    ipv4_addr_digit[0] = LONGB0(ip_digit);
    ipv4_addr_digit[1] = LONGB1(ip_digit);
    ipv4_addr_digit[2] = LONGB2(ip_digit);
    ipv4_addr_digit[3] = LONGB3(ip_digit);

    for (i = 0; i < size; i++) {
        retv = dfl_chip_blkwrite(cpld_name, addrOffset + i, 1, ipv4_addr_digit + i);
        if (retv != DFL_OK) {
            debug_log(DLOG_ERROR, "dfl_chip_blkwrite fail, obj_name=%s, ret=%d", cpld_name, retv);
            return RET_ERR;
        }
    }

    return RET_OK;
}


static void sdi_switch_ipv6_order(guint8 *ipv6_buff, guint8 ipv6_size)
{
    guint8 temp = 0;
    for (guint8 i = 0; i < ipv6_size - 1; i += 2) {
        temp = ipv6_buff[i];
        ipv6_buff[i] = ipv6_buff[i + 1];
        ipv6_buff[i + 1] = temp;
    }
    return;
}


static gint32 sdi_card_set_ipv6(OBJ_HANDLE obj_hnd, OBJ_HANDLE group_obj, guint32 addrOffset, guint8 size,
    const gchar *cpld_name)
{
    guint8 i = 0;
    struct in6_addr ipv6_digit;
    struct in6_addr ipv6_digit_temp;
    gchar ipv6_str[IPV6_IP_STR_SIZE + 1] = {0};
    gboolean ip_is_changed = FALSE;

    
    if (addrOffset == 0) {
        return RET_OK;
    }

    if (size == 0 || size > IPV6_ADDR_SIZE) {
        debug_log(DLOG_ERROR, "size check err, obj_name=%s, size=%d", dfl_get_object_name(obj_hnd), size);
        return RET_ERR;
    }

    
    gint32 retv = dal_get_property_value_string(group_obj, ETH_GROUP_ATTRIBUTE_IPV6_ADDR, ipv6_str, IPV6_IP_STR_SIZE);
    if (retv != RET_OK) {
        debug_log(DLOG_ERROR,
            "dal_get_property_value_string(ETH_GROUP_ATTRIBUTE_IPV6_ADDR) failed, obj_name=%s, ret=%d",
            dfl_get_object_name(group_obj), retv);
        return RET_ERR;
    }

    (void)memset_s(ipv6_digit_temp.s6_addr, sizeof(ipv6_digit_temp.s6_addr), 0, sizeof(ipv6_digit_temp.s6_addr));
    for (i = 0; i < size; i++) {
        retv = dfl_chip_blkread(cpld_name, addrOffset + i, 1, ipv6_digit_temp.s6_addr + i);
        if (retv != DFL_OK) {
            (void)memset_s(ipv6_digit_temp.s6_addr, sizeof(ipv6_digit_temp.s6_addr), 0,
                sizeof(ipv6_digit_temp.s6_addr));
            debug_log(DLOG_ERROR, "dfl_chip_blkread fail, obj_name=%s, ret=%d", cpld_name, retv);
            break;
        }
    }

    sdi_switch_ipv6_order(ipv6_digit_temp.s6_addr, sizeof(ipv6_digit_temp.s6_addr));

    (void)memset_s((void *)(&ipv6_digit), sizeof(ipv6_digit), 0, sizeof(ipv6_digit));
    (void)inet_pton(AF_INET6, ipv6_str, &ipv6_digit);

    
    for (i = 0; i < IPV6_IP_SIZE; i++) {
        if ((ipv6_digit.s6_addr[i] != ipv6_digit_temp.s6_addr[i]) || (ipv6_digit.s6_addr[i] != ipv6_addr_digit[i])) {
            ip_is_changed = TRUE;
            break;
        }
    }

    if (ip_is_changed == FALSE) {
        return RET_OK;
    }

    errno_t safe_fun_ret = memcpy_s(ipv6_addr_digit, sizeof(ipv6_addr_digit), ipv6_digit.s6_addr, IPV6_IP_SIZE);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }
    sdi_switch_ipv6_order(ipv6_digit.s6_addr, sizeof(ipv6_digit.s6_addr));
    for (i = 0; i < size; i++) {
        retv = dfl_chip_blkwrite(cpld_name, addrOffset + i, 1, ipv6_digit.s6_addr + i);
        if (retv != DFL_OK) {
            debug_log(DLOG_ERROR, "dfl_chip_blkwrite fail, obj name=%s, ret=%d", cpld_name, retv);
            return RET_ERR;
        }
    }

    return RET_OK;
}


void sdi_card_get_bmc_ip(OBJ_HANDLE obj_hnd)
{
    OBJ_HANDLE cpld_hnd = 0;
    OBJ_HANDLE group_obj = 0;
    guint32 boardid = 0;
    guint32 ipv4_offset = 0;
    guint32 ipv6_offset = 0;
    guint8 ipv4_size = 0;
    guint8 ipv6_size = 0;
    gchar ipv4_cpld_name[CPLD_OBJ_MAX_NAME_LEN + 1] = {0};
    gchar ipv6_cpld_name[CPLD_OBJ_MAX_NAME_LEN + 1] = {0};

    
    gint32 retv = dfl_get_object_handle(OUT_ETHGROUP_OBJ_NAME, &group_obj);
    if (retv != DFL_OK) {
        debug_log(DLOG_ERROR, "dfl_get_property_value(OUT_ETHGROUP_OBJ_NAME) failed, ret=%d", retv);
        return;
    }

    retv = dal_get_property_value_uint32(obj_hnd, PROPERTY_SDI_CARD_BOARDID, &boardid);
    if (retv != RET_OK) {
        debug_log(DLOG_ERROR, "get sdi card boardid faild, obj_name=%s, retv=%d", dfl_get_object_name(obj_hnd), retv);
        return;
    }

    if (check_hw_is_older_than_sdi_ver3(boardid)) {
        
        retv = dfl_get_referenced_object(obj_hnd, PROPERTY_SDI_CARD_CPLD_ACCESSOR, &cpld_hnd);
        if (DFL_OK != retv) {
            debug_log(DLOG_ERROR, "dfl_get_referenced_object(PROPERTY_SDI_CARD_CPLD_ACCESSOR) fail, ret = %d", retv);
            return;
        }

        ipv4_offset = BMC_IP_ADDR_OFFSET0;
        ipv4_size = IPV4_ADDR_SIZE;

        
        errno_t safe_fun_ret = strncpy_s(ipv4_cpld_name, sizeof(ipv4_cpld_name), dfl_get_object_name(cpld_hnd),
            strlen(dfl_get_object_name(cpld_hnd)));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        }
    } else {
        retv = sdi_card_get_refchip_info(obj_hnd, PROPERTY_SDI_IPV4_ADDRREG, &ipv4_offset, &ipv4_size, ipv4_cpld_name,
            sizeof(ipv4_cpld_name));
        if (retv != RET_OK) {
            debug_log(DLOG_ERROR, "%s: sdi get ipv4 refchip info fail, ret = %d", dfl_get_object_name(cpld_hnd), retv);
            return;
        }

        retv = sdi_card_get_refchip_info(obj_hnd, PROPERTY_SDI_IPV6_ADDRREG, &ipv6_offset, &ipv6_size, ipv6_cpld_name,
            sizeof(ipv6_cpld_name));
        if (retv != RET_OK) {
            debug_log(DLOG_ERROR, "%s: sdi get ipv6 refchip info fail, ret = %d", dfl_get_object_name(cpld_hnd), retv);
            return;
        }
    }

    guint8 id = get_sdi_id_by_handle(obj_hnd);
    if (id >= SD100_CARD_MAX_NUM) {
        debug_log(DLOG_ERROR, "%s: sdi id is out of range", dfl_get_object_name(cpld_hnd));
        return;
    }

    (void)sdi_card_set_ip(obj_hnd, group_obj, ipv4_offset, ipv4_size, (const gchar *)ipv4_cpld_name);
    (void)sdi_card_set_ipv6(obj_hnd, group_obj, ipv6_offset, ipv6_size, (const gchar *)ipv6_cpld_name);
}


LOCAL void sdi_card_uptade_slot(OBJ_HANDLE object_handle)
{
    guint8 slot = 0;
    OBJ_HANDLE cpld_hnd = 0;
    guint8 read_value = 0;

    if (!object_handle) {
        debug_log(DLOG_ERROR, "%s:obj_handle is null!", __FUNCTION__);
        return;
    }

    
    gint32 ret = dfl_get_referenced_object(object_handle, PROPERTY_SDI_CARD_CPLD_ACCESSOR, &cpld_hnd);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: Get sdi cpld handle failed, ret:%d", __FUNCTION__, ret);
    }

    const gchar *cpld_name = dfl_get_object_name(cpld_hnd);

    guint8 id = get_sdi_id_by_handle(object_handle);
    if (id >= SD100_CARD_MAX_NUM) {
        debug_log(DLOG_ERROR, "%s: sdi id is out of range, id = %d", __FUNCTION__, id);
        return;
    }

    
    ret = dal_get_property_value_byte(object_handle, PROPERTY_SDI_CARD_SLOT, &slot);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Get sdi card slot failed, ret:%d", __FUNCTION__, ret);
    }

    
    ret = dfl_chip_blkread(cpld_name, BMC_SET_SDI_SLOT_OFFSET, 0x01, &read_value);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: dfl_chip_blkread failed, ret:%d", __FUNCTION__, ret);
    }

    if (slot != read_value) {
        
        ret = dfl_chip_blkwrite(cpld_name, BMC_SET_SDI_SLOT_OFFSET, 0x01, &slot);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s: dfl_chip_blkwrite failed, ret:%d", __FUNCTION__, ret);
        }
    }
}


gint32 sdi_bios_print(GSList *input_list)
{
    guint8 bios_print_switch = 0;

    OBJ_HANDLE obj_hnd = 0;
    OBJ_HANDLE cpld_hnd = 0;

    const gchar *cpld_name = NULL;

    GSList *sdi_hnd = NULL;
    GSList *objhnd_node = NULL;

    if (input_list == NULL) {
        debug_printf("param is null!\n");
        return RET_ERR;
    }

    guint32 input_len = g_slist_length(input_list);
    if (input_len != 1) {
        debug_printf("usage: sdi_bios_print option\r\noption\n0: Close bios debug log\r\n1:Open bios debug log");
        return RET_ERR;
    }

    guint8 option = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));

    switch (option) {
        case 1:
            bios_print_switch = (OPEN_DEBUG);
            break;

        case 0:
            bios_print_switch = (CLOSE_DEBUG);
            break;

        default:
            debug_printf("Input invalid! \r\nusage: sdi_bios_print option\r\noption\n0: Close bios debug log\r\n1: "
                "Open bios debug log");
            return RET_ERR;
    }

    gint32 ret = dfl_get_object_list(CLASS_PCIE_SDI_CARD, &sdi_hnd);
    if (ret != DFL_OK) {
        debug_printf("no sdi card!\n");
        return RET_ERR;
    }

    for (objhnd_node = sdi_hnd; objhnd_node; objhnd_node = g_slist_next(objhnd_node)) {
        obj_hnd = (OBJ_HANDLE)(objhnd_node->data);

        ret = dfl_get_referenced_object(obj_hnd, PROPERTY_SDI_CARD_CPLD_ACCESSOR, &cpld_hnd);
        if (DFL_OK != ret) {
            debug_printf("dfl_get_referenced_object(PROPERTY_SDI_CARD_CPLD_ACCESSOR) fail!, ret = %d", ret);
            continue;
        }

        cpld_name = dfl_get_object_name(cpld_hnd);
        if (cpld_name == NULL) {
            debug_printf("dfl_get_object_name(cpld) fail! ret = %d", ret);
            continue;
        }

        ret = dfl_chip_blkwrite(cpld_name, CPLD_BIOS_PRINT_FLAG_OFFSET, 1, &bios_print_switch);
        if (DFL_OK != ret) {
            debug_printf("dfl_get_object_name(cpld) fail! ret = %d", ret);
            continue;
        }

        debug_printf("SDI card(%s) %s bios print", dfl_get_object_name(obj_hnd),
            OPEN_DEBUG == bios_print_switch ? "Open" : "Close");
        vos_task_delay(500);
    }

    g_slist_free(sdi_hnd);
    return RET_OK;
}

LOCAL gboolean is_valid_boot_order_by_boardid(guint8 boot_order, guint32 boardid, OBJ_HANDLE obj_handle)
{
    if (boardid == SDI_CARD_BOARDID_5E || boardid == SDI_CARD_BOARDID_AC) {
        if (boot_order != HDD_ORDER && boot_order != PXE_ORDER && boot_order != NET_CARD_PXE_ORDER &&
            boot_order != SINGLE_HOST_PXE_ORDER && boot_order != EXTERND_PXE_ORDER) {
            return FALSE;
        }
    } else if (boardid == SDI_CARD_36_BOARDID_AB) {
        if (boot_order != HDD_ORDER && boot_order != PXE_ORDER) {
            return FALSE;
        }
    } else if (is_valid_sdi_or_dpu_boardid(obj_handle) == RET_OK) {
        if (boot_order != HDD_ORDER && boot_order != PXE_ORDER && boot_order!= NONE_ORDER) {
            return FALSE;
        }
    } else {
        if (boot_order != HDD_ORDER && boot_order != PXE_ORDER && boot_order != NET_CARD_PXE_ORDER) {
            return FALSE;
        }
    }
    return TRUE;
}


gint32 ipmi_cmd_set_sdi_boot_order(const void *msg_data, gpointer user_data)
{
    guint8 slot = 0;
    guint32 boardid = 0;
    OBJ_HANDLE obj_hnd = 0;
    const SDI_SET_BOOT_ORDER_REQ_S *boot_order_req = NULL;

    boot_order_req = (const SDI_SET_BOOT_ORDER_REQ_S *)get_ipmi_src_data(msg_data);
    if (boot_order_req == NULL) {
        debug_log(DLOG_ERROR, "%s: get boot_order_req failed", __FUNCTION__);
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    if (boot_order_req->manufacturer != HUAWEI_MFG_ID) {
        debug_log(DLOG_ERROR, "%s: invalid manufacturer id(%d)", __FUNCTION__, boot_order_req->manufacturer);
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    gint32 ret = get_sdi_card_handle_by_logic_num(boot_order_req->logical_num, &obj_hnd);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get sdi card handle failed ,ret=%d", __FUNCTION__, ret);
        ipmi_operation_log(msg_data, "Set the boot option of PCIeCard failed.");
        return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
    }

    ret = dal_get_extern_value_byte(obj_hnd, PROPERTY_CARD_SLOT, &slot, DF_AUTO);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get sdi card slot failed ,ret=%d", __FUNCTION__, ret);
        ipmi_operation_log(msg_data, "Set the boot option of PCIeCard failed.");
        return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
    }

    ret = dal_get_property_value_uint32(obj_hnd, PROPERTY_SDI_CARD_BOARDID, &boardid);
    if (ret < 0) {
        debug_log(DLOG_ERROR, "%s: get sdi board failed ,ret=%d", __FUNCTION__, ret);
        ipmi_operation_log(msg_data, "Set the boot option of PCIeCard failed.");
        return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
    }

    ret = is_valid_boot_order_by_boardid(boot_order_req->boot_order, boardid, obj_hnd);
    if (ret != TRUE) {
        debug_log(DLOG_ERROR, "%s: invalid boot_order(%d)", __FUNCTION__, boot_order_req->boot_order);
        ipmi_operation_log(msg_data, "Set the boot option of PCIeCard failed.");
        return ipmi_send_simple_response(msg_data, COMP_CODE_OUTOF_RANGE);
    }

    guint8 mcu_support = 0;
    (void)dal_get_property_value_byte(obj_hnd, PROPERTY_SDI_CARD_MCUSUPPORTED, &mcu_support);
    if (mcu_support == 1) {
        return sdi_card_set_boot_order_mcu(msg_data, obj_hnd, boardid, boot_order_req);
    } else {
        return sdi_card_set_boot_order(msg_data, obj_hnd, boardid, boot_order_req);
    }
}


LOCAL guint8 get_sdi_os_load_status(OBJ_HANDLE object_handle)
{
    guint32 boardid = 0;
    (void)dal_get_property_value_uint32(object_handle, PROPERTY_SDI_CARD_BOARDID, &boardid);

    if (boardid == SDI_CARD_BOARDID_5E || boardid == SDI_CARD_36_BOARDID_AB) {
        return SDI_BOARD_5E_OS_LOAD_SUCCESS;
    }

    return OS_LOAD_SUCCESS;
}

LOCAL gint32 set_sdi_ip_by_cpld(const void *msg_data, OBJ_HANDLE obj_hnd, guint8 *in_data, guint8 in_data_len)
{
    guint32 boardid = 0;
    guint8 WriteDone = 1;
    OBJ_HANDLE cpld_hnd = 0;
    gint32 offset = BMC_SET_FSA_IP_OFFSET;

    gint32 ret = dal_get_property_value_uint32(obj_hnd, PROPERTY_SDI_CARD_BOARDID, &boardid);
    if (ret != RET_OK) {
        return COMP_CODE_UNKNOWN;
    }

    if (check_hw_is_older_than_sdi_ver3(boardid)) {
        (void)dfl_get_referenced_object(obj_hnd, PROPERTY_SDI_CARD_CPLD_ACCESSOR, &cpld_hnd);
        const gchar *cpld_name = dfl_get_object_name(cpld_hnd);
        
        for (guint8 i = 0; i < IPMI_IP_DATA_LENGTH; i++) {
            ret = dfl_chip_blkwrite(cpld_name, offset + i, 1, in_data + i);
            if (ret != DFL_OK) {
                return COMP_CODE_UNKNOWN;
            }
        }

        
        ret = dfl_chip_bytewrite(cpld_name, BMC_SET_FSA_IP_DONE_OFFSET, 0x01, &WriteDone);
        if (ret != DFL_OK) {
            return COMP_CODE_UNKNOWN;
        }
    } else {
        ret = sdi_card_set_refchip_value(obj_hnd, PROPERTY_SDI_FSA_WRITE_IP_REG, in_data, in_data_len);
        if (ret != RET_OK) {
            return COMP_CODE_UNKNOWN;
        }
        ret = sdi_card_set_refchip_value(obj_hnd, PROPERTY_SDI_FSA_WRITE_STATUS_REG, &WriteDone, sizeof(guint8));
        if (ret != RET_OK) {
            return COMP_CODE_UNKNOWN;
        }
    }
    return RET_OK;
}


gint32 ipmi_cmd_set_sdi_ip(const void *msg_data, gpointer user_data)
{
    guint8 slot = 0;
    guint8 os_status = 0;
    OBJ_HANDLE obj_hnd = 0;
    guint8 in_data[IPMI_IP_DATA_LENGTH + 1] = {0};
    guint8 mcu_support = 0;
    SDI_IPMI_RESP_S fsa_ip_resp;
    gchar sdi_desc[MAX_STRING_LENGTH] = {0};

    const SDI_SET_FSA_IP_REQ_S *fsa_ip_req = (const SDI_SET_FSA_IP_REQ_S *)get_ipmi_src_data(msg_data);
    if (fsa_ip_req == NULL) {
        debug_log(DLOG_ERROR, "%s: fsa_ip_req is NULL", __FUNCTION__);
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    if (fsa_ip_req->manufacturer != HUAWEI_MFG_ID) {
        debug_log(DLOG_ERROR, "%s: fsa_ip_req->manufacturer(%d) is invalid", __FUNCTION__, fsa_ip_req->manufacturer);
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    if (fsa_ip_req->device_type != DEVICE_TYPE_SDI_CARD) {
        debug_log(DLOG_ERROR, "%s: fsa_ip_req->device_type(%d) is invalid", __FUNCTION__, fsa_ip_req->device_type);
        return ipmi_send_simple_response(msg_data, COMP_CODE_CMD_INVALID);
    }

    gint32 ret = get_sdi_card_handle_by_logic_num(fsa_ip_req->logical_num, &obj_hnd);
    if (ret != RET_OK) {
        ipmi_operation_log(msg_data, "Set sdi ip failed");
        return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
    }
    ret = dal_get_extern_value_byte(obj_hnd, PROPERTY_CARD_SLOT, &slot, DF_AUTO);
    if (ret != RET_OK) {
        ipmi_operation_log(msg_data, "Set sdi ip failed");
        return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
    }

    (void)memcpy_s(in_data, IPMI_IP_DATA_LENGTH + 1, fsa_ip_req->write_data, IPMI_IP_DATA_LENGTH);

    
    ret = dal_get_extern_value_byte(obj_hnd, PROPERTY_SDI_CARD_SYSTEM_STATUS, &os_status, DF_AUTO);
    if (ret != RET_OK) {
        ipmi_operation_log(msg_data, "Set sdi ip failed");
        return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
    }
    (void)dal_get_property_value_byte(obj_hnd, PROPERTY_SDI_CARD_MCUSUPPORTED, &mcu_support);
    if (mcu_support == 0) {
        if (os_status != get_sdi_os_load_status(obj_hnd)) {
            return ipmi_send_simple_response(msg_data, COMP_CODE_CANNOT_RESPONSE);
        }
        ret = set_sdi_ip_by_cpld(msg_data, obj_hnd, in_data, sizeof(in_data) - 1);
    } else {
        OBJ_HANDLE pcie_obj = 0;
        (void)dal_get_specific_object_byte(CLASS_PCIE_CARD, PROPERTY_PCIE_CARD_SLOT_ID, slot, &pcie_obj);
        guint8 write_data[IPMI_IP_DATA_LENGTH + 1] = {0};
        write_data[0] = 1; // Request data  0 --BMC, 1 --卡上os
        in_data[8] = fsa_ip_req->write_data[9];  // 由于sditool和ipmitool设置vlanid时data[8]和data[9]不一致
        in_data[9] = fsa_ip_req->write_data[8];  // 因此使用ipmitool向mcu写入数据时，此处data[8]和data[9]转换下
        ret = memcpy_s(&write_data[1], IPMI_IP_DATA_LENGTH, in_data, IPMI_IP_DATA_LENGTH);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "memcpy_s failed");
            ipmi_operation_log(msg_data, "Set sdi ip failed");
            return ipmi_send_simple_response(msg_data, ret);
        }
        ret = set_sdi_info_to_mcu(pcie_obj, REGISTER_KEY_SET_IP, write_data, sizeof(in_data));
    }
    if (ret != RET_OK) {
        ipmi_operation_log(msg_data, "Set sdi ip failed");
        return ipmi_send_simple_response(msg_data, ret);
    }
    (void)dal_get_property_value_string(obj_hnd, PROPERTY_SDI_CARD_DESC, sdi_desc, MAX_STRING_LENGTH);
    fsa_ip_resp.comp_code = COMP_CODE_SUCCESS;
    fsa_ip_resp.manufacturer = fsa_ip_req->manufacturer;
    guint16 vlanid = MAKE_WORD(fsa_ip_req->write_data[8], fsa_ip_req->write_data[9]); // data[8]和data[9]为vlanid
    ipmi_operation_log(msg_data,
        "Sent the command for setting the IPv4 address (%d.%d.%d.%d/%d.%d.%d.%d) and VLAN (%d) of the management "
        "network port of PCIeCard%d(%s) successfully.",
        fsa_ip_req->write_data[0], fsa_ip_req->write_data[1], fsa_ip_req->write_data[2], fsa_ip_req->write_data[3],
        fsa_ip_req->write_data[4], fsa_ip_req->write_data[5], fsa_ip_req->write_data[6], fsa_ip_req->write_data[7],
        vlanid, slot, (sdi_desc[0] == 0) ? "SD100" : sdi_desc);
    return ipmi_send_response(msg_data, sizeof(fsa_ip_resp), (guint8 *)&fsa_ip_resp);
}

LOCAL gboolean is_sdi_used_as_ncsi_nic(OBJ_HANDLE obj_sdi_card)
{
    guint16 board_id = 0;

    (void)dal_get_property_value_uint16(obj_sdi_card, PROPERTY_SDI_CARD_BOARDID, &board_id);

    if (board_id == SDI_CARD_36_BOARDID_AB) {
        return TRUE;
    }

    return FALSE;
}

LOCAL gint32 is_valid_reset_sdi_ipmi_cmd(const void *msg_data, const SDI_RESET_REQ_S *reset_req)
{
    if (reset_req == NULL) {
        debug_log(DLOG_ERROR, "%s: fsa_ip_req is NULL", __FUNCTION__);
        (void)ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
        return RET_ERR;
    }

    if (reset_req->manufacturer != HUAWEI_MFG_ID) {
        debug_log(DLOG_ERROR, "%s: fsa_ip_req->manufacturer(%d) is invalid", __FUNCTION__, reset_req->manufacturer);
        (void)ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
        return RET_ERR;
    }

    if (reset_req->device_type != DEVICE_TYPE_SDI_CARD) {
        debug_log(DLOG_ERROR, "%s: fsa_ip_req->device_type(%d) is invalid", __FUNCTION__, reset_req->device_type);
        (void)ipmi_send_simple_response(msg_data, COMP_CODE_CMD_INVALID);
        return RET_ERR;
    }

    if (reset_req->reset_flag != BMC_RESET_SDI_VALUE) {
        debug_log(DLOG_ERROR, "%s: reset_req->reset_flag(%d) is invalid", __FUNCTION__, reset_req->reset_flag);
        ipmi_operation_log(msg_data, "Reset sdi failed");
        return ipmi_send_simple_response(msg_data, COMP_CODE_OUTOF_RANGE);
    }

    return RET_OK;
}

LOCAL void sdi_card_reset_task(void *param)
{
    guint8 set_value = BMC_RESET_SDI_VALUE;

    vos_task_delay(VOS_TICK_PER_SECOND);

    OBJ_HANDLE obj_hnd = (OBJ_HANDLE)param;

    gint32 ret = set_sdi_card_referenced_chip_value(obj_hnd, CPLD_SDI_ADDR, BMC_RESET_SDI_OFFSET, 0x01, &set_value);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: reset sdi(%s) failed, ret=%d", __FUNCTION__, dfl_get_object_name(obj_hnd), ret);
        return;
    }

    debug_log(DLOG_INFO, "%s: reset sdi(%s) successfully", __FUNCTION__, dfl_get_object_name(obj_hnd));

    return;
}

LOCAL gint32 reset_sdi_by_cpld(OBJ_HANDLE obj_hnd)
{
    guint8 set_value = BMC_RESET_SDI_VALUE;
    gulong sdi_card_reset_task_id = 0;

    
    if (is_sdi_used_as_ncsi_nic(obj_hnd) == TRUE) {
        return vos_task_create(&sdi_card_reset_task_id, "reset_sdi", (TASK_ENTRY)sdi_card_reset_task,
            (void *)obj_hnd, DEFAULT_PRIORITY);
    }

    return set_sdi_card_referenced_chip_value(obj_hnd, CPLD_SDI_ADDR, BMC_RESET_SDI_OFFSET, 0x01, &set_value);
}


gint32 ipmi_cmd_reset_sdi(const void *msg_data, gpointer user_data)
{
    guint8 slot = 0;
    guint16 mcu_support = 0;
    OBJ_HANDLE obj_hnd = 0;
    OBJ_HANDLE pcie_obj = 0;
    SDI_IPMI_RESP_S reset_resp;
    gchar sdi_desc[MAX_STRING_LENGTH] = {0};

    const SDI_RESET_REQ_S *reset_req = (const SDI_RESET_REQ_S *)get_ipmi_src_data(msg_data);

    gint32 ret = is_valid_reset_sdi_ipmi_cmd(msg_data, reset_req);
    if (ret != RET_OK) {
        return RET_OK;
    }

    ret = get_sdi_card_handle_by_logic_num(reset_req->logical_num, &obj_hnd);
    if (ret != RET_OK) {
        ipmi_operation_log(msg_data, "Reset sdi failed");
        return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
    }

    
    ret = dal_get_extern_value_byte(obj_hnd, PROPERTY_CARD_SLOT, &slot, DF_AUTO);
    if (ret == RET_ERR) {
        ipmi_operation_log(msg_data, "Reset sdi failed");
        return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
    }

    (void)dal_get_property_value_string(obj_hnd, PROPERTY_SDI_CARD_DESC, sdi_desc, MAX_STRING_LENGTH);
    reset_resp.comp_code = COMP_CODE_SUCCESS;
    reset_resp.manufacturer = reset_req->manufacturer;

    // 调用MCU接口复位SDI5X
    (void)dal_get_property_value_uint16(obj_hnd, PROPERTY_SDI_CARD_MCUSUPPORTED, &mcu_support);
    if (mcu_support == 1) {
        (void)dal_get_specific_object_byte(CLASS_PCIE_CARD, PROPERTY_PCIE_CARD_SLOT_ID, slot, &pcie_obj);
        guint8 in_data = 0;
        ret = set_sdi_info_to_mcu(pcie_obj, REGISTER_KEY_RESET_MCU, &in_data, in_data);  // data_lenth：0
    } else {
        // SDI3.X调用写cpld复位
        ret = reset_sdi_by_cpld(obj_hnd);
    }

    if (ret != RET_OK) {
        ipmi_operation_log(msg_data, "Reset sdi failed");
        return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
    }

    ipmi_operation_log(msg_data, "Reset PCIeCard%d(%s) successfully.", slot, (sdi_desc[0] == 0) ? "SD100" : sdi_desc);
    return ipmi_send_response(msg_data, sizeof(reset_resp), (guint8 *)&reset_resp);
}

gint32 ipmi_cmd_reset_sdi_imu(const void *msg_data, gpointer user_data)
{
    guint8 slot = 0;
    guint32 boardid = 0;
    gchar sdi_desc[MAX_STRING_LENGTH] = {0};
    OBJ_HANDLE obj_hnd = 0;
    guint8 set_value = BMC_RESET_SDI_IMU_VALUE;
    SDI_IPMI_RESP_S reset_resp;

    const SDI_RESET_REQ_S *reset_req = (const SDI_RESET_REQ_S *)get_ipmi_src_data(msg_data);
    if (reset_req == NULL) {
        debug_log(DLOG_ERROR, "%s: fsa_ip_req is NULL", __FUNCTION__);
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    if (reset_req->manufacturer != HUAWEI_MFG_ID) {
        debug_log(DLOG_ERROR, "%s: fsa_ip_req->manufacturer(%d) is invalid", __FUNCTION__, reset_req->manufacturer);
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    if (reset_req->device_type != DEVICE_TYPE_SDI_CARD) {
        debug_log(DLOG_ERROR, "%s: fsa_ip_req->device_type(%d) is invalid", __FUNCTION__, reset_req->device_type);
        return ipmi_send_simple_response(msg_data, COMP_CODE_CMD_INVALID);
    }

    gint32 ret = get_sdi_card_handle_by_logic_num(reset_req->logical_num, &obj_hnd);
    if (ret != RET_OK) {
        ipmi_operation_log(msg_data, "Reset PCIeCard IMU failed");
        return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
    }

    ret = dal_get_property_value_uint32(obj_hnd, PROPERTY_SDI_CARD_BOARDID, &boardid);
    if (ret != RET_OK) {
        ipmi_operation_log(msg_data, "Reset PCIeCard IMU failed");
        return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
    }

    if (check_hw_is_older_than_sdi_ver3(boardid)) {
        ipmi_operation_log(msg_data, "Reset PCIeCard IMU failed");
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_CMD);
    }

    if (reset_req->reset_flag != BMC_RESET_SDI_IMU_VALUE) {
        ipmi_operation_log(msg_data, "Reset PCIeCard IMU failed");
        return ipmi_send_simple_response(msg_data, COMP_CODE_OUTOF_RANGE);
    }

    
    ret = set_sdi_card_referenced_chip_value(obj_hnd, CPLD_SDI_ADDR, BMC_RESET_SDI_IMU_OFFSET, 0x01, &set_value);
    if (ret != RET_OK) {
        ipmi_operation_log(msg_data, "Reset PCIeCard IMU failed");
        return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
    }
    
    reset_resp.comp_code = COMP_CODE_SUCCESS;
    reset_resp.manufacturer = reset_req->manufacturer;
    ret = ipmi_send_response(msg_data, sizeof(reset_resp), (guint8 *)&reset_resp);
    if (ret != RET_OK) {
        ipmi_operation_log(msg_data, "Reset PCIeCard IMU failed");
        return RET_ERR;
    }

    (void)dal_get_property_value_string(obj_hnd, PROPERTY_SDI_CARD_DESC, sdi_desc, MAX_STRING_LENGTH);

    
    ret = dal_get_extern_value_byte(obj_hnd, PROPERTY_CARD_SLOT, &slot, DF_AUTO);
    if (ret != RET_OK) {
        ipmi_operation_log(msg_data, "Reset PCIeCard IMU failed");
        return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
    }

    ipmi_operation_log(msg_data, "Reset PCIeCard%d(%s) IMU successfully", slot,
        (sdi_desc[0] == 0) ? "SDI V3" : sdi_desc);

    return RET_OK;
}


LOCAL gchar *transfer_rst_reason_to_string(const guint8 in_data)
{
    switch (in_data) {
        case 1:
            return "Poweron Reset";

        case 2:
            return "OS Reset";

        case 4:
            return "Watchdog Reset";

        case 8:
            return "BIOSTimeout Reset";

        case 16:
            return "OSTimeout Reset";

        default:
            return "N/A";
    }
}


LOCAL gchar *transfer_pst_accessor_value_to_string(const guint8 in_data, const guint8 mask)
{
    gchar *ret_str = NULL;

    if ((in_data & mask) == 0) {
        ret_str = "Absence";
    } else {
        ret_str = "Presence";
    }

    return ret_str;
}


LOCAL OBJ_HANDLE sdi_get_chip_handle_by_addr(const OBJ_HANDLE object_handle, guint32 chip_addr)
{
    GSList *chip_list = NULL;
    GSList *chip_node = NULL;
    guint32 temp_addr = 0;
    OBJ_HANDLE chip_hand = 0;

    gint32 ret = dal_get_object_list_position(object_handle, CLASS_CHIP, &chip_list);
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "get chip handle list failed, obj_name=%s, ret=%d", dfl_get_object_name(object_handle),
            ret);
        return 0;
    }

    for (chip_node = chip_list; chip_node; chip_node = g_slist_next(chip_node)) {
        (void)dal_get_property_value_uint32((OBJ_HANDLE)chip_node->data, PROPERTY_CHIP_ADDR, &temp_addr);
        if (chip_addr == temp_addr) {
            chip_hand = (OBJ_HANDLE)chip_node->data;
            g_slist_free(chip_list);
            return chip_hand;
        }
    }
    g_slist_free(chip_list);
    return 0;
}


LOCAL gint32 get_sdi_card_referenced_chip_value(const OBJ_HANDLE object_handle, guint32 chip_addr,
    const guint8 chip_offset, const gsize chip_data_size, guint8 *out_data)
{
    if (out_data == NULL) {
        return RET_ERR;
    }

    OBJ_HANDLE chip_handle = sdi_get_chip_handle_by_addr(object_handle, chip_addr);
    if (chip_handle == 0) {
        debug_log(DLOG_ERROR, "get chip addr err, obj_name=%s", dfl_get_object_name(object_handle));
        return RET_ERR;
    }

    gint32 ret = dfl_chip_blkread(dfl_get_object_name(chip_handle), chip_offset, chip_data_size, (void *)out_data);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "read chip failed, obj_name=%s, ret=%d", dfl_get_object_name(chip_handle), ret);
        return ret;
    }
    return ret;
}


LOCAL gint32 set_sdi_card_referenced_chip_value(const OBJ_HANDLE object_handle, guint32 chip_addr,
    const guint8 chip_offset, const gsize chip_data_size, guint8 *in_data)
{
    if (in_data == NULL) {
        return RET_ERR;
    }

    OBJ_HANDLE chip_handle = sdi_get_chip_handle_by_addr(object_handle, chip_addr);
    if (chip_handle == 0) {
        debug_log(DLOG_ERROR, "get chip addr err, obj_name=%s", dfl_get_object_name(object_handle));
        return RET_ERR;
    }

    gint32 ret = dfl_chip_blkwrite(dfl_get_object_name(chip_handle), chip_offset, chip_data_size, (void *)in_data);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "write chip failed, obj_name=%s, ret=%d", dfl_get_object_name(chip_handle), ret);
        return ret;
    }

    return RET_OK;
}


LOCAL gint32 sdi_read_i2c(OBJ_HANDLE obj_handle, guint32 chip_addr, const guint8 chip_offset,
    const gsize chip_data_size, guint8 *out_data)
{
    gint32 ret = 0;
    guint8 retry_time = 0;
    for (retry_time = 0; retry_time < SDI_CPLD_RETRY_TIME; ++retry_time) {
        ret = get_sdi_card_referenced_chip_value(obj_handle, chip_addr, chip_offset, chip_data_size, out_data);
        if (ret == RET_OK) {
            break;
        }

        vos_task_delay(100);
    }
    return ret;
}


LOCAL void sdi_card_get_cpld_reg_info(OBJ_HANDLE obj_handle, guint32 chip_addr, gchar *cpld_info, guint32 cpld_info_len)
{
    guint32 i = 0;
    gint32 ret = RET_OK;
    errno_t safe_fun_ret = EOK;
    gchar tmp[4] = {0};
    guint8 read_value[SDI_CPLD_PRINT_NUMBER] = {0};
    gchar sdi_cpld_info[SDI_CPLD_DUMPINFO_MAX_LEN + 1] = {0};
    gchar tmp_line[SDI_CPLD_DUMPINFO_MAX_LEN + 1] = {0};
    (void)memset_s(sdi_cpld_info, sizeof(sdi_cpld_info), 0, sizeof(sdi_cpld_info));

    // 0到f行
    while (i < SDI_CPLD_MAX_NUMBER) {
        (void)memset_s(tmp_line, sizeof(tmp_line), 0, sizeof(tmp_line));
        (void)snprintf_s(tmp_line, sizeof(tmp_line), sizeof(tmp_line) - 1, "%04X:  ", i);
        // 0到f列
        (void)memset_s(read_value, sizeof(read_value), 0, sizeof(read_value));
        ret = sdi_read_i2c(obj_handle, chip_addr, i, SDI_CPLD_PRINT_NUMBER, read_value);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "sdi_card_read_reg failed!  offset =%d, ret = %d", i, ret);
            return;
        }

        for (guint32 j = 0; j < SDI_CPLD_PRINT_NUMBER; j++, i++) {
            (void)memset_s(tmp, sizeof(tmp), 0, sizeof(tmp));
            (void)snprintf_s(tmp, sizeof(tmp), sizeof(tmp) - 1, "%02X ", read_value[j]);

            safe_fun_ret = strncat_s(tmp_line, sizeof(tmp_line), tmp, strlen(tmp));
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
            }
        }
        safe_fun_ret = strncat_s(tmp_line, sizeof(tmp_line), "\n", strlen("\n"));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        }
        safe_fun_ret = strncat_s(sdi_cpld_info, sizeof(sdi_cpld_info), tmp_line, strlen(tmp_line));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        }
    }

    safe_fun_ret = strncpy_s(cpld_info, cpld_info_len, sdi_cpld_info, strlen(sdi_cpld_info));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }
    return;
}


LOCAL gint32 sdi_card_cpld_reg_write(OBJ_HANDLE obj_handle, gchar *sdi_desc, guint8 slot_id, FILE *fp,
    guint32 slave_address)
{
    gchar tmp[10] = {0};
    gchar sdi_cpld_info[SD100_CARD_DUMPINFO_MAX_LEN + 1] = {0};
    gchar cpld_info_detial[SDI_CPLD_DUMPINFO_MAX_LEN + 1] = {0};

    gint32 ret = snprintf_s(sdi_cpld_info,
        sizeof(sdi_cpld_info), sizeof(sdi_cpld_info) - 1, "%s SLOT%u Info\n", sdi_desc, slot_id);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }

    (void)snprintf_s(tmp, sizeof(tmp), sizeof(tmp) - 1, "0x%02X ", slave_address);

    errno_t safe_fun_ret = strncat_s(sdi_cpld_info, sizeof(sdi_cpld_info), tmp, strlen(tmp));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }

    safe_fun_ret = strncat_s(sdi_cpld_info, sizeof(sdi_cpld_info), "slave address \n", strlen("slave address \n"));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }

    safe_fun_ret =
        strncat_s(sdi_cpld_info, sizeof(sdi_cpld_info), "offset 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n",
        strlen("offset 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n"));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }

    safe_fun_ret =
        strncat_s(sdi_cpld_info, sizeof(sdi_cpld_info), "------------------------------------------------------\n",
        strlen("------------------------------------------------------\n"));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }

    // 写导出数据的头部和格式部分
    ret = fwrite(sdi_cpld_info, strlen(sdi_cpld_info), 1, fp);
    if (ret != 1) {
        debug_log(DLOG_ERROR, "fwrite title failed!");
        return RET_ERR;
    }
    // 真正读CPLD并更新
    sdi_card_get_cpld_reg_info(obj_handle, slave_address, cpld_info_detial, sizeof(cpld_info_detial));

    // 写导出数据的CPLD内容部分
    ret = fwrite(cpld_info_detial, strlen(cpld_info_detial), 1, fp);
    if (ret != 1) {
        debug_log(DLOG_ERROR, "fwrite detial failed!");
        return RET_ERR;
    }

    return RET_OK;
}

LOCAL gint32 dump_cpld_reg_of_one_sdi(OBJ_HANDLE obj_pcie_sdi_card, FILE *fp)
{
    gchar sdi_desc[MAX_STRING_LENGTH] = {0};
    guint8 slot_id = 0;
    guint8 cpld_ver = 0;
    guint32 boardid = 0;

    gint32 ret = dal_get_property_value_string(obj_pcie_sdi_card, PROPERTY_SDI_CARD_DESC, sdi_desc, MAX_STRING_LENGTH);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get sdi card desc failed!");
        return RET_ERR;
    }

    ret = dal_get_property_value_byte(obj_pcie_sdi_card, PROPERTY_SDI_CARD_SLOT, &slot_id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get sdi card slot failed!");
        return RET_ERR;
    }

    ret = sdi_card_cpld_reg_write(obj_pcie_sdi_card, sdi_desc, slot_id, fp, CPLD_SDI_ADDR);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "fwrite 0x36 addr failed!");
        return RET_ERR;
    }

    (void)dal_get_property_value_byte(obj_pcie_sdi_card, PROPERTY_SDI_CPLD_VERSION_REG, &cpld_ver);
    (void)dal_get_property_value_uint32(obj_pcie_sdi_card, PROPERTY_SDI_CARD_BOARDID, &boardid);
    // SDI 3.0的CPLD版本为20以上的版本有0x38的地址，SDI 2.2及以前的没有
    if (((cpld_ver >= SDI_CPLD_VERSION_20) && (boardid == SDI_CARD_BOARDID_5E || boardid == SDI_CARD_BOARDID_AC)) ||
        boardid == SDI_CARD_36_BOARDID_AB) {
        ret = sdi_card_cpld_reg_write(obj_pcie_sdi_card, sdi_desc, slot_id, fp, CPLD_SDI_EXT_ADDR);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "fwrite 0x38 addr failed!");
            return RET_ERR;
        }
    }

    return RET_OK;
}


gint32 sdi_card_dump_cpld_reg(const gchar *path)
{
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    gchar file_name[SD100_CARD_DUMPINFO_MAX_LEN + 1] = {0};

    if (path == NULL) {
        return RET_ERR;
    }

    gint32 ret = dfl_get_object_list(CLASS_PCIE_SDI_CARD, &obj_list);
    if (ret != DFL_OK) {
        if (ret == ERRCODE_OBJECT_NOT_EXIST) {
            return RET_OK;
        }

        return RET_ERR;
    }

    // 创建文件
    (void)snprintf_truncated_s(file_name, sizeof(file_name), "%s/sdi_card_cpld_info", path);
    FILE *fp = dal_fopen_check_realpath(file_name, "a+", file_name);
    if (fp == NULL) {
        g_slist_free(obj_list);
        return RET_ERR;
    }
    (void)fchmod(fileno(fp), COMMON_DUMP_FILE_UMASK);

    // 写文件
    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        ret = dump_cpld_reg_of_one_sdi((OBJ_HANDLE)obj_note->data, fp);
        if (ret != RET_OK) {
            g_slist_free(obj_list);
            (void)fclose(fp);
            debug_log(DLOG_ERROR, "%s: dump_cpld_reg_of_one_sdi failed, ret=%d", __FUNCTION__, ret);
            return ret;
        }
    }

    g_slist_free(obj_list);
    (void)fclose(fp);

    return RET_OK;
}


LOCAL void get_sdi_netcard_presence_info(guint8 status, gchar *info, gint32 info_len)
{
    if (info == NULL) {
        return;
    }

    switch (status) {
        case SDI_NETCARD_PRESENCE_ABSENCE:
            (void)strncpy_s(info, info_len, "Presence/Absence", strlen("Presence/Absence"));
            return;
        case SDI_NETCARD_ABSENCE_PRESENCE:
            (void)strncpy_s(info, info_len, "Absence/Presence", strlen("Absence/Presence"));
            return;
        case SDI_NETCARD_PRESENCE_PRESENCE:
            (void)strncpy_s(info, info_len, "Presence/Presence", strlen("Presence/Presence"));
            return;
        default:
            (void)strncpy_s(info, info_len, "Absence/Absence", strlen("Absence/Absence"));
            return;
    }
}


gint32 sdi_card_dump_info(const gchar *path)
{
    gchar file_name[SD100_CARD_DUMPINFO_MAX_LEN + 1] = {0};
    gchar sd100_card_info[SD100_CARD_DUMPINFO_MAX_LEN * 2 + 1] = {0};
    gchar netcard_pst_info[MAX_STRING_LENGTH] = {0};
    guint8 m2_pst = 0xff;
    guint8 dimm_pst = 0xff;
    guint8 netcard_pst = 0xff;
    guint8 rst_reason[RST_REASON_DATA_SIZE] = {0};
    guint8 port80[PORT_DATA_SIZE] = {0};
    guint8 slot_id = 0;
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;
    size_t fwrite_back = 0;
    guint8 m2_1_mask = 0x02;
    guint8 m2_0_mask = 0x01;
    guint8 dimm_1_mask = 0x02;
    guint8 dimm_0_mask = 0x01;
    guint32 boardid = 0;

    if (path == NULL) {
        return RET_ERR;
    }

    gint32 ret = dfl_get_object_list(CLASS_PCIE_SDI_CARD, &obj_list);
    if (ret != DFL_OK) {
        if (ERRCODE_OBJECT_NOT_EXIST == ret) {
            return RET_OK;
        }

        return RET_ERR;
    }

    
    // 在dump_dir目录下创建文件
    ret = snprintf_truncated_s(file_name, sizeof(file_name), "%s/card_info", path);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }
    FILE *fp = dal_fopen_check_realpath(file_name, "a+", file_name);
    if (fp == NULL) {
        g_slist_free(obj_list);
        debug_log(DLOG_ERROR, "Open card_info failed !");
        return RET_ERR;
    }

    
    (void)fchmod(fileno(fp), COMMON_DUMP_FILE_UMASK);
    
    (void)snprintf_s(sd100_card_info, sizeof(sd100_card_info), sizeof(sd100_card_info) - 1, "%s", "SDICard Info\n");
    fwrite_back = fwrite(sd100_card_info, strlen(sd100_card_info), 1, fp);
    if (fwrite_back != 1) {
        (void)fclose(fp);
        g_slist_free(obj_list);
        debug_log(DLOG_ERROR, "fwrite failed!");
        return RET_ERR;
    }

    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        (void)memset_s(sd100_card_info, sizeof(sd100_card_info), 0, sizeof(sd100_card_info));
        (void)memset_s(rst_reason, RST_REASON_DATA_SIZE, 0, RST_REASON_DATA_SIZE);
        (void)memset_s(port80, PORT_DATA_SIZE, 0, PORT_DATA_SIZE);

        property_name_list = g_slist_append(property_name_list, PROPERTY_SDI_CARD_SLOT);
        property_name_list = g_slist_append(property_name_list, PROPERTY_SDI_CARD_M2);
        property_name_list = g_slist_append(property_name_list, PROPERTY_SDI_CARD_DIMM);
        property_name_list = g_slist_append(property_name_list, PROPERTY_SDI_CARD_NETCARD);

        ret = dfl_multiget_property_value((OBJ_HANDLE)obj_note->data, property_name_list, &property_value);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "Get sd100 card information failed!");
            g_slist_free(property_name_list);
            g_slist_free(obj_list);
            (void)fclose(fp);
            return RET_ERR;
        }

        slot_id = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 0));
        m2_pst = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 1));
        dimm_pst = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 2));
        netcard_pst = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 3));

        (void)get_sdi_card_referenced_chip_value((OBJ_HANDLE)obj_note->data, CPLD_SDI_ADDR, RST_REASON_OFFSET,
            RST_REASON_DATA_SIZE, rst_reason);

        ret = dal_get_property_value_uint32((OBJ_HANDLE)obj_note->data, PROPERTY_SDI_CARD_BOARDID, &boardid);
        if (ret != RET_OK) {
            (void)fclose(fp);
            debug_log(DLOG_ERROR, "get boardid err obj_name=%s, ret=%d !",
                dfl_get_object_name((OBJ_HANDLE)obj_note->data), ret);
            g_slist_free(obj_list);
            g_slist_free(property_name_list);
            g_slist_free_full(property_value, (GDestroyNotify)g_variant_unref);
            return ret;
        }

        
        if (check_hw_is_older_than_sdi_ver3(boardid)) {
            (void)get_sdi_card_referenced_chip_value((OBJ_HANDLE)obj_note->data, CPLD_SDI_ADDR, PORT_OFFSET,
                PORT_DATA_SIZE, port80);
            ret = snprintf_s(sd100_card_info, sizeof(sd100_card_info), sizeof(sd100_card_info) - 1,
                "%-4s | %-25s | %-30s | %-25s | %-15s | %-15s | %-15s | %-15s | %-10s  \n"
                "%-4u | %-10s/%-14s | %-10s/%-19s | %-25s | %-15s | %-15s | %-15s | %-15s | "
                "%02x-%02x-%02x-%02x-%02x-%02x \n",
                "Slot", "M.2 Presence(M.2_1/M.2_2)", "SODIMM Presence(DIMM00/DIMM10)", "Network Board Presence",
                "Reset Reason 1", "Reset Reason 2", "Reset Reason 3", "Reset Reason 4", "80Port", slot_id,
                transfer_pst_accessor_value_to_string(m2_pst, m2_0_mask),
                transfer_pst_accessor_value_to_string(m2_pst, m2_1_mask),
                transfer_pst_accessor_value_to_string(dimm_pst, dimm_0_mask),
                transfer_pst_accessor_value_to_string(dimm_pst, dimm_1_mask), netcard_pst == 1 ? "Presence" : "Absence",
                transfer_rst_reason_to_string(rst_reason[0]), transfer_rst_reason_to_string(rst_reason[1]),
                transfer_rst_reason_to_string(rst_reason[2]), transfer_rst_reason_to_string(rst_reason[3]), port80[0],
                port80[1], port80[2], port80[3], port80[4], port80[5]);
        } else {
            (void)memset_s(netcard_pst_info, sizeof(netcard_pst_info), 0, sizeof(netcard_pst_info));
            get_sdi_netcard_presence_info(netcard_pst, (gchar *)netcard_pst_info, sizeof(netcard_pst_info));
            (void)sdi_card_get_refchip_value((OBJ_HANDLE)obj_note->data, PROPERTY_SDI_80_PORT_REG, port80,
                sizeof(port80));
            ret = snprintf_s(sd100_card_info, sizeof(sd100_card_info), sizeof(sd100_card_info) - 1,
                "%-4s | %-25s | %-25s | %-15s | %-15s | %-15s | %-15s | %-10s  \n"
                "%-4u | %-10s/%-14s | %-25s | %-15s | %-15s | %-15s | %-15s | %02x-%02x-%02x-%02x-%02x-%02x \n",
                "Slot", "M.2 Presence(M.2_0/M.2_1)", "Network Board Presence", "Reset Reason 1", "Reset Reason 2",
                "Reset Reason 3", "Reset Reason 4", "80Port", slot_id,
                transfer_pst_accessor_value_to_string(m2_pst, m2_0_mask),
                transfer_pst_accessor_value_to_string(m2_pst, m2_1_mask), netcard_pst_info,
                transfer_rst_reason_to_string(rst_reason[0]), transfer_rst_reason_to_string(rst_reason[1]),
                transfer_rst_reason_to_string(rst_reason[2]), transfer_rst_reason_to_string(rst_reason[3]), port80[0],
                port80[1], port80[2], port80[3], port80[4], port80[5]);
        }

        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        }
        fwrite_back = fwrite(sd100_card_info, strlen(sd100_card_info), 1, fp);
        if (fwrite_back != 1) {
            (void)fclose(fp);
            g_slist_free(obj_list);
            g_slist_free(property_name_list);
            g_slist_free_full(property_value, (GDestroyNotify)g_variant_unref);
            debug_log(DLOG_ERROR, "fwrite failed!");
            return RET_ERR;
        }

        (void)memset_s(sd100_card_info, sizeof(sd100_card_info), 0, sizeof(sd100_card_info));
        g_slist_free(property_name_list);
        g_slist_free_full(property_value, (GDestroyNotify)g_variant_unref);
        property_name_list = NULL;
    }

    (void)snprintf_s(sd100_card_info, sizeof(sd100_card_info), sizeof(sd100_card_info) - 1, "%s", "\n\n");

    fwrite_back = fwrite(sd100_card_info, strlen(sd100_card_info), 1, fp);
    if (fwrite_back != 1) {
        (void)fclose(fp);
        g_slist_free(obj_list);
        debug_log(DLOG_ERROR, "fwrite failed!");
        return RET_ERR;
    }

    (void)memset_s(sd100_card_info, SD100_CARD_DUMPINFO_MAX_LEN + 1, 0, SD100_CARD_DUMPINFO_MAX_LEN + 1);
    
    g_slist_free(obj_list);
    // 关闭文件
    (void)fclose(fp);
    return RET_OK;
}


gint32 get_fsa_ip_status(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 offset = 0;
    guint8 is_done = 0;
    guint32 ipaddr = 0;
    guint16 vlanid = 0;

    OBJ_HANDLE cpld_obj = 0;
    guint32 boardid = 0;

    
    if (output_list == NULL) {
        debug_log(DLOG_ERROR, "Get fsa ip status failed, output_list can not be NULL.");
        return RET_ERR;
    }

    
    gint32 ret = dal_get_property_value_uint32(object_handle, PROPERTY_SDI_CARD_BOARDID, &boardid);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get boardid failed, obj_name=%s, ret=%d.", dfl_get_object_name(object_handle), ret);
        return RET_ERR;
    }

    if (check_hw_is_older_than_sdi_ver3(boardid)) {
        ret = dfl_get_referenced_object(object_handle, PROPERTY_SDI_CARD_CPLD_ACCESSOR, &cpld_obj);
        if (ret != DFL_OK) {
            return COMP_CODE_DATA_NOT_AVAILABLE;
        }

        offset = CPLD_SDI_FSA_STATUS;
        
        ret = dfl_chip_blkread(dfl_get_object_name(cpld_obj), offset, 1, &is_done);
        if (ret < 0) {
            debug_log(DLOG_ERROR, "Chip blkread failed!, obj_name=%s", dfl_get_object_name(cpld_obj));
            return RET_ERR;
        }
    } else {
        ret = sdi_card_get_refchip_value(object_handle, PROPERTY_SDI_FSA_STATUS_REG, &is_done, sizeof(guint8));
        if (ret != RET_OK) {
            return COMP_CODE_DATA_NOT_AVAILABLE;
        }
    }

    ret = dal_get_extern_value_uint32(object_handle, PROPERTY_SDI_CARD_STORAGE_IP_ADDR, &ipaddr, DF_AUTO);
    if (ret != RET_OK) {
        return COMP_CODE_UNKNOWN;
    }

    ret = dal_get_extern_value_uint16(object_handle, PROPERTY_SDI_CARD_STORAGE_VLAN, &vlanid, DF_AUTO);
    if (ret != RET_OK) {
        return COMP_CODE_UNKNOWN;
    }

    

    *output_list = g_slist_append(*output_list, (gpointer)g_variant_new_byte(is_done));
    *output_list = g_slist_append(*output_list, (gpointer)g_variant_new_uint32(ipaddr));
    *output_list = g_slist_append(*output_list, (gpointer)g_variant_new_uint16(vlanid));

    return RET_OK;
}


gint32 get_os_set_ip_status(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 offset = 0;
    guint8 is_done = 0;
    guint32 ipaddr = 0;
    guint16 vlanid = 0;

    OBJ_HANDLE cpld_obj = 0;
    guint32 boardid = 0;

    
    if (output_list == NULL) {
        debug_log(DLOG_ERROR, "Get os set_ip_status failed, output_list can not be NULL.");
        return RET_ERR;
    }

    

    gint32 ret = dal_get_property_value_uint32(object_handle, PROPERTY_SDI_CARD_BOARDID, &boardid);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get property err, obj_name=%s, ret=%d.", dfl_get_object_name(cpld_obj), ret);
        return ret;
    }

    if (check_hw_is_older_than_sdi_ver3(boardid)) {
        ret = dfl_get_referenced_object(object_handle, PROPERTY_SDI_CARD_CPLD_ACCESSOR, &cpld_obj);
        if (ret != DFL_OK) {
            return COMP_CODE_DATA_NOT_AVAILABLE;
        }

        offset = CPLD_SDI_ETH01_STATUS;
        
        ret = dfl_chip_blkread(dfl_get_object_name(cpld_obj), offset, 1, &is_done);
        if (ret < 0) {
            debug_log(DLOG_ERROR, "%s:Chip blkread failed!", __FUNCTION__);
            return RET_ERR;
        }

        

        offset = CPLD_SDI_ETH01_IP_ADDR;
        ret = dfl_chip_blkread(dfl_get_object_name(cpld_obj), offset, 4, &ipaddr);
        if (ret < 0) {
            debug_log(DLOG_ERROR, "%s:Chip blkread failed!", __FUNCTION__);
            return RET_ERR;
        }

        offset = CPLD_SDI_ETH01_VLAN;
        ret = dfl_chip_blkread(dfl_get_object_name(cpld_obj), offset, 2, &vlanid);
        if (ret < 0) {
            debug_log(DLOG_ERROR, "%s:Chip blkread failed!", __FUNCTION__);
            return RET_ERR;
        }

        
    } else {
        ret = sdi_card_get_refchip_value(object_handle, PROPERTY_SDI_STATUS_REG, &is_done, sizeof(guint8));
        if (ret != RET_OK) {
            return COMP_CODE_DATA_NOT_AVAILABLE;
        }

        ret = sdi_card_get_refchip_value(object_handle, PROPERTY_SDI_IP_REG, &ipaddr, sizeof(guint32));
        if (ret != RET_OK) {
            return COMP_CODE_DATA_NOT_AVAILABLE;
        }

        ret = sdi_card_get_refchip_value(object_handle, PROPERTY_SDI_VLAN_REG, &vlanid, sizeof(guint16));
        if (ret != RET_OK) {
            return COMP_CODE_DATA_NOT_AVAILABLE;
        }
    }

    *output_list = g_slist_append(*output_list, (gpointer)g_variant_new_byte(is_done));
    *output_list = g_slist_append(*output_list, (gpointer)g_variant_new_uint32(ipaddr));
    *output_list = g_slist_append(*output_list, (gpointer)g_variant_new_uint16(vlanid));

    return RET_OK;
}


gint32 sdi_set_pcie_test_reg(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    guint8 slot_id = 0;
    gchar sdi_desc[MAX_STRING_LENGTH] = {0};

    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "Input_list can not be NULL.");
        return RET_ERR;
    }

    guint8 status_temp = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    (void)dal_get_property_value_string(object_handle, PROPERTY_SDI_CARD_DESC, sdi_desc, sizeof(sdi_desc));
    (void)dal_get_property_value_byte(object_handle, PROPERTY_SDI_CARD_SLOT, &slot_id);
    gint32 ret = dal_set_property_value_byte(object_handle, PROPERTY_SDI_PCIE_TEST_REG, status_temp, DF_AUTO);
    method_operation_log(caller_info, NULL, "Set the Slot%d %s card test register value %d %s.", slot_id, sdi_desc,
        status_temp, ret == RET_OK ? "successfully" : "failed");

    return ret;
}

gint32 sdi_card_set_eid(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    guint8 is_support = 0;

    if (input_list == NULL || g_slist_length(input_list) != INPUT_LIST_PARAM) {
        debug_log(DLOG_ERROR, "%s:input param null.", __FUNCTION__);
        return RET_ERR;
    }

    (void)dal_get_property_value_byte(object_handle, PROPERTY_SDI_CARD_SUPPORT, &is_support);
    if (is_support == 0) {
        debug_log(DLOG_DEBUG, "%s, bmc do not support net card %s mctp function currently.", __FUNCTION__,
            dfl_get_object_name(object_handle));
        return RET_OK;
    }

    GVariant *eid = (GVariant *)g_slist_nth_data(input_list, 0);
    gint32 ret = dfl_set_property_value(object_handle, PROPERTY_SDI_CARD_EID, eid, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, Set sdi card eid failed. ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    GVariant *phy_addr = (GVariant *)g_slist_nth_data(input_list, 1);
    ret = dfl_set_property_value(object_handle, PROPERTY_SDI_CARD_PHY_ADDR, phy_addr, DF_NONE);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s, Set sdi card phy addr failed. ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL guint8 add_hash_table(OBJ_HANDLE obj_hnd)
{
    guint8 value = 0;

    for (value = 0; value < SD100_CARD_MAX_NUM; value++) {
        if (SDI_HANDLE_NOT_EXIST == sdi_obj_tbl[value]) {
            sdi_obj_tbl[value] = obj_hnd;
            return value;
        }
    }

    return SDI_HANDLE_OUT_OF_RANGE;
}


LOCAL guint8 delete_hash_table(OBJ_HANDLE obj_hnd)
{
    guint8 value = 0;

    for (value = 0; value < SD100_CARD_MAX_NUM; value++) {
        if (obj_hnd == sdi_obj_tbl[value]) {
            sdi_obj_tbl[value] = SDI_HANDLE_NOT_EXIST;
            return value;
        }
    }

    return SDI_HANDLE_OUT_OF_RANGE;
}


LOCAL guint8 get_sdi_id_by_handle(OBJ_HANDLE obj_hnd)
{
    guint8 value = 0;

    for (value = 0; value < SD100_CARD_MAX_NUM; value++) {
        if (obj_hnd == sdi_obj_tbl[value]) {
            return value;
        }
    }

    return SDI_HANDLE_OUT_OF_RANGE;
}

LOCAL gint32 set_sdi_card_name_for_v3(OBJ_HANDLE object_handle)
{
    const gchar *sdi100name_v3 = "BC61SHSA";
    (void)dal_set_property_value_string(object_handle, PROPERTY_SDI_CARD_BOARD_NAME, sdi100name_v3, DF_NONE);
    return RET_OK;
}

LOCAL gint32 set_sdi_card_name_for_v5(OBJ_HANDLE object_handle)
{
    const gchar *sdi100name_v5 = "IT21SHSA";
    (void)dal_set_property_value_string(object_handle, PROPERTY_SDI_CARD_BOARD_NAME, sdi100name_v5, DF_NONE);
    return RET_OK;
}

LOCAL gint32 set_sdi_card_name_for_fd(OBJ_HANDLE object_handle)
{
    const gchar *sdi100name_fd = "IT21SHSB";
    OBJ_HANDLE pcie_obj_handle = OBJ_HANDLE_COMMON;

    (void)dal_set_property_value_string(object_handle, PROPERTY_SDI_CARD_BOARD_NAME, sdi100name_fd, DF_NONE);

    gint32 ret = dal_get_specific_object_position(object_handle, CLASS_PCIE_CARD, &pcie_obj_handle);
    if (ret == RET_OK) {
        ret = dal_set_property_value_string(pcie_obj_handle, PROPERTY_PCIE_CARD_DESC, "SDI V2", DF_NONE);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:set sdi card CardDesc faild,ret:%d", __FUNCTION__, ret);
            return ret;
        }
    }

    return RET_OK;
}

LOCAL gint32 set_sdi_card_name_for_5e(OBJ_HANDLE object_handle)
{
    const gchar *sdi100name_5e = "IT21SHSC";
    OBJ_HANDLE pcie_obj_handle = OBJ_HANDLE_COMMON;

    (void)dal_set_property_value_string(object_handle, PROPERTY_SDI_CARD_BOARD_NAME, sdi100name_5e, DF_NONE);

    gint32 ret = dal_get_specific_object_position(object_handle, CLASS_PCIE_CARD, &pcie_obj_handle);
    if (ret == RET_OK) {
        ret = dal_set_property_value_string(pcie_obj_handle, PROPERTY_PCIE_CARD_DESC, "SDI V3", DF_NONE);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:set sdi card CardDesc faild,ret:%d", __FUNCTION__, ret);
            return ret;
        }
        (void)dal_set_property_value_string(pcie_obj_handle, PROPETRY_PCIECARD_MANUFACTURER, PCIE_HUAWEI_MANU, DF_NONE);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:set sdi card Manufacturer faild,ret:%d", __FUNCTION__, ret);
            return ret;
        }
    }

    return RET_OK;
}

LOCAL gint32 set_sdi_card_name_for_ab(OBJ_HANDLE object_handle)
{
    const gchar *sdi100name_ab = "IT21SHSG";
    OBJ_HANDLE pcie_obj_handle = OBJ_HANDLE_COMMON;

    (void)dal_set_property_value_string(object_handle, PROPERTY_SDI_CARD_BOARD_NAME, sdi100name_ab, DF_NONE);

    gint32 ret = dal_get_specific_object_position(object_handle, CLASS_PCIE_CARD, &pcie_obj_handle);
    if (ret == RET_OK) {
        ret = dal_set_property_value_string(pcie_obj_handle, PROPERTY_PCIE_CARD_DESC, "SDI V3 Lite", DF_NONE);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:set sdi card CardDesc faild,ret:%d", __FUNCTION__, ret);
            return ret;
        }
        (void)dal_set_property_value_string(pcie_obj_handle, PROPETRY_PCIECARD_MANUFACTURER, PCIE_HUAWEI_MANU, DF_NONE);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:set sdi card Manufacturer faild,ret:%d", __FUNCTION__, ret);
            return ret;
        }
    }

    return RET_OK;
}

LOCAL gint32 set_sdi_card_name_for_ac(OBJ_HANDLE object_handle)
{
    OBJ_HANDLE pcie_obj_handle = OBJ_HANDLE_COMMON;

    (void)dal_set_property_value_string(object_handle, PROPERTY_SDI_CARD_BOARD_NAME, "IT21SHSE", DF_NONE);

    gint32 ret = dal_get_specific_object_position(object_handle, CLASS_PCIE_CARD, &pcie_obj_handle);
    if (ret == RET_OK) {
        ret = dal_set_property_value_string(pcie_obj_handle, PROPERTY_PCIE_CARD_DESC, "SDI V3.5", DF_NONE);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:set sdi card CardDesc faild,ret:%d", __FUNCTION__, ret);
            return ret;
        }
        (void)dal_set_property_value_string(pcie_obj_handle, PROPETRY_PCIECARD_MANUFACTURER, PCIE_HUAWEI_MANU, DF_NONE);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:set sdi card Manufacturer faild,ret:%d", __FUNCTION__, ret);
            return ret;
        }
    }

    return RET_OK;
}

typedef gint32 (*SET_SDI_CARD_NAME_FUNC)(OBJ_HANDLE object_handle);
typedef struct {
    guint32 boardid;
    SET_SDI_CARD_NAME_FUNC func;
} SET_SDI_CARD_NAME_PAIR;

LOCAL SET_SDI_CARD_NAME_PAIR g_set_sdi_card_name_table[] = {
    {SDI_CARD_BOARDID_V3, set_sdi_card_name_for_v3},
    {SDI_CARD_BOARDID_V5, set_sdi_card_name_for_v5},
    {SDI_CARD_BOARDID_FD, set_sdi_card_name_for_fd},
    {SDI_CARD_BOARDID_5E, set_sdi_card_name_for_5e},
    {SDI_CARD_36_BOARDID_AB, set_sdi_card_name_for_ab},
    {SDI_CARD_BOARDID_AC, set_sdi_card_name_for_ac},
};


LOCAL gint32 set_sdi_card_name(OBJ_HANDLE object_handle)
{
    guint32 boardid = 0;

    gint32 ret = dal_get_property_value_uint32(object_handle, PROPERTY_SDI_CARD_BOARDID, &boardid);
    if (ret < 0) {
        debug_log(DLOG_ERROR, "%s:get sdi card boardid faild", __FUNCTION__);
        return RET_ERR;
    }

    for (guint32 i = 0; i < G_N_ELEMENTS(g_set_sdi_card_name_table); i++) {
        if (boardid == g_set_sdi_card_name_table[i].boardid) {
            ret = g_set_sdi_card_name_table[i].func(object_handle);
            break;
        }
    }

    return ret;
}


gint32 sdi_card_add_object_callback(OBJ_HANDLE object_handle)
{
    guint8 id = add_hash_table(object_handle);
    if (SDI_HANDLE_OUT_OF_RANGE == id || id >= SD100_CARD_MAX_NUM) {
        debug_log(DLOG_ERROR, "%s:SDI OBJ_HANDLE is out of range", __FUNCTION__);
        return RET_ERR;
    }

    
    gint32 ret = set_sdi_card_name(object_handle);
    if (ret < 0) {
        debug_log(DLOG_ERROR, "%s:set sdi card name faild", __FUNCTION__);
    }

    

    return ret;
}


gint32 sdi_card_del_object_callback(OBJ_HANDLE object_handle)
{
    gint32 ret = 0;

    guint8 id = delete_hash_table(object_handle);
    if (SDI_HANDLE_OUT_OF_RANGE == id || id >= SD100_CARD_MAX_NUM) {
        debug_log(DLOG_ERROR, "%s:SDI OBJ_HANDLE is out of range", __FUNCTION__);
        return RET_ERR;
    }

    return ret;
}


LOCAL gint32 sdi_card_get_accessor_value(OBJ_HANDLE sdi_handle, const gchar *prop_name, guint8 *data_size,
    gchar *val_buff, gint32 buff_len)
{
    guint32 offset = 0;
    guint8 size = 0;
    gchar cpld_name[CPLD_OBJ_MAX_NAME_LEN + 1] = {0};
    if (prop_name == NULL || val_buff == NULL) {
        debug_log(DLOG_ERROR, "%s: invalid param", __FUNCTION__);
        return RET_ERR;
    }
    gint32 ret = sdi_card_get_refchip_info(sdi_handle, prop_name, &offset, &size, cpld_name, sizeof(cpld_name));
    if (ret != RET_OK || size > buff_len) {
        debug_log(DLOG_ERROR, "%s: sdi get card type refchip info fail %d", __FUNCTION__, ret);
        return ret;
    }

    ret = dfl_chip_blkread(cpld_name, offset, size, val_buff);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: dfl_chip_blkread fail, obj_name=%s, ret=%d", __FUNCTION__, cpld_name, ret);
        return RET_ERR;
    }

    *data_size = size;
    return RET_OK;
}

LOCAL void get_sdi_extcard_mac(OBJ_HANDLE sdi_handle, OBJ_HANDLE extend_handle)
{
    guint8 card_mac_size = 0;
    gchar card_mac_buff[MAX_MAC_STR_LEN + 1] = {0};
    gchar mac_addr[MAX_MAC_STR_LEN + 1] = {0};
    PM_CHIP_INFO *pm_chip_info = NULL;
    const guint32 mac_addr_0 = 0;
    const guint32 mac_addr_1 = 1;
    const guint32 mac_addr_2 = 2;
    const guint32 mac_addr_3 = 3;
    const guint32 mac_addr_4 = 4;
    const guint32 mac_addr_5 = 5;
    guchar protocol = PCIE_MGNT_INVALID_PROTOCOL;

    if (dal_get_property_value_byte(extend_handle, PROPERTY_PCIE_CARD_PROTOCOL, &protocol) != RET_OK ||
        (protocol != PCIE_MGNT_PROTOCOL_SDI_CPLD && protocol != PCIE_MGNT_PROTOCOL_SDI_MULTI_HOST_CPLD)) {
        debug_log(DLOG_DEBUG, "%s:%s has wrong protocol%d", __FUNCTION__, dfl_get_object_name(extend_handle), protocol);
        return;
    }

    gint32 ret = dfl_get_binded_data(extend_handle, (gpointer *)&pm_chip_info);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: binded data fail, ret: %d", __FUNCTION__, ret);
        return;
    }

    if (protocol == PCIE_MGNT_PROTOCOL_SDI_CPLD) {
        ret = sdi_card_get_accessor_value(sdi_handle, PROPERTY_SDI_EXT_CARD_MAC_ADDR, &card_mac_size, card_mac_buff,
            sizeof(card_mac_buff));
    } else if (protocol == PCIE_MGNT_PROTOCOL_SDI_MULTI_HOST_CPLD) {
        ret = sdi_card_get_accessor_value(sdi_handle, PROPERTY_SDI_MULTI_HOST_EXT_CARD_MAC_ADDR, &card_mac_size,
            card_mac_buff, sizeof(card_mac_buff));
    }

    if (ret != RET_OK || card_mac_size <= 0) {
        debug_log(DLOG_ERROR, "%s: get extend card type fail", __FUNCTION__);
        return;
    }

    for (gint32 mac_index = 0; mac_index + MAC_ADDR_LEN < card_mac_size; mac_index += MAC_ADDR_LEN) {
        ret = snprintf_s(mac_addr, sizeof(mac_addr), sizeof(mac_addr) - 1, "%02X:%02X:%02X:%02X:%02X:%02X",
            card_mac_buff[mac_addr_0 + mac_index], card_mac_buff[mac_addr_1 + mac_index],
            card_mac_buff[mac_addr_2 + mac_index], card_mac_buff[mac_addr_3 + mac_index],
            card_mac_buff[mac_addr_4 + mac_index], card_mac_buff[mac_addr_5 + mac_index]);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret=%d", __FUNCTION__, ret);
            continue;
        }

        ret = pcie_card_set_pfinfo(extend_handle, pm_chip_info, mac_index / MAC_ADDR_LEN, mac_index / MAC_ADDR_LEN,
            (gchar *)mac_addr);

        debug_log(DLOG_DEBUG, "%s:%s MAC: %d [%s]", __FUNCTION__, dfl_get_object_name(extend_handle), mac_index,
            mac_addr);
    }
    return;
}

LOCAL void sdi_extcard_update_mac(OBJ_HANDLE object_handle)
{
    GSList *card_list = NULL;
    GSList *list_item = NULL;
    const gchar *pcie_card_name = NULL;
    OBJ_HANDLE extcard_object_handle;
    OBJ_HANDLE pciecard_object_handle;
    gchar basecard_obj_name[MAX_OBJECT_NAME_LEN] = {0};

    // 找到同位置PCIECard卡对象
    if (dal_get_specific_object_position(object_handle, CLASS_PCIE_CARD, &pciecard_object_handle) != RET_OK) {
        debug_log(DLOG_INFO, "%s: get pcie card object fail: %s", __FUNCTION__, dfl_get_object_name(object_handle));
        return;
    }

    pcie_card_name = dfl_get_object_name(pciecard_object_handle);
    if (pcie_card_name == NULL) {
        debug_log(DLOG_ERROR, "%s: get object name fail.", __FUNCTION__);
        return;
    }
    gint32 ret = dfl_get_object_list(CLASS_EXTEND_PCIECARD_NAME, &card_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: Get extendcard handle list fail %d", __FUNCTION__, ret);
        return;
    }
    for (list_item = card_list; list_item; list_item = g_slist_next(list_item)) {
        extcard_object_handle = (OBJ_HANDLE)list_item->data;
        ret = dal_get_property_value_string(extcard_object_handle, PROPETRY_EXTEND_PCIECARD_BASECARD, basecard_obj_name,
            sizeof(basecard_obj_name));
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s: Get extendcard basecard fail %d", __FUNCTION__, ret);
            continue;
        }

        if (g_strcmp0(pcie_card_name, basecard_obj_name) == 0) {
            
            get_sdi_extcard_mac(object_handle, extcard_object_handle);
        }
    }
    g_slist_free(card_list);
    return;
}

LOCAL gint32 __get_sdi_serial_record_connect(OBJ_HANDLE obj_handle, GSList **sdi_conn, GSList **mutex_sdi_conn)
{
#define SDI_CONNECT_ARR_LEN 4
    guint32 prop_val = 0;
    // SerialRecordConnect属性值表示串口录音需要建立的串口连接，如0x4152321 表示 0x4<->0x15 0x23<->0x21
    int ret = dal_get_property_value_uint32(obj_handle, PROPERTY_SDI_SERIAL_RECORD_CONNECT, &prop_val);
    if (prop_val == 0xffffffff || ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: %s does not support serial record, ret: %d", __func__,
            dfl_get_object_name(obj_handle), ret);
        return RET_ERR;
    }
    guint8 *sdi_connect_arr = (guint8 *)&prop_val;

    GSList *connect_list = NULL;
    ret = dfl_get_object_list(CLASS_NAME_SERIAL_MANAGEMENT, &connect_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get %s list failed, ret: %d", __func__, CLASS_NAME_SERIAL_MANAGEMENT, ret);
        return RET_ERR;
    }

    for (GSList *Node = connect_list; Node; Node = Node->next) {
        OBJ_HANDLE connect_object = (OBJ_HANDLE)Node->data;

        guint8 src_serial = 0;
        guint8 dst_serial = 0;
        dal_get_property_value_byte(connect_object, PROPERTY_SRC_SERIAL, &src_serial);
        dal_get_property_value_byte(connect_object, PROPERTY_DEST_SERIAL, &dst_serial);

        debug_log(DLOG_DEBUG, "CHECK SDI Conn: %s", dfl_get_object_name(connect_object));
        gboolean is_related_connect = FALSE;
        for (int i = 0; i < SDI_CONNECT_ARR_LEN; i++) {
            debug_log(DLOG_DEBUG, "sdi_connect_arr[%d]: %u", i, sdi_connect_arr[i]);
            if (src_serial == sdi_connect_arr[i] || dst_serial == sdi_connect_arr[i]) {
                is_related_connect = TRUE;
                break;
            }
        }
        if (!is_related_connect) {
            debug_log(DLOG_DEBUG, "NOT RELATED SDI Conn: %s", dfl_get_object_name(connect_object));
            continue;
        }

        // 3: bmc src_serial, 2: bmc dst_serial, 1: sdi src_serial， 0: sdi dst_serial
        if ((src_serial == sdi_connect_arr[3] && dst_serial == sdi_connect_arr[2]) ||
            (src_serial == sdi_connect_arr[1] && dst_serial == sdi_connect_arr[0])) {
            *sdi_conn = g_slist_append(*sdi_conn, (gpointer)connect_object);
            debug_log(DLOG_DEBUG, "SDI Conn: %s: src: %u, dst: %u", dfl_get_object_name(connect_object), src_serial,
                dst_serial);
        } else {
            *mutex_sdi_conn = g_slist_append(*mutex_sdi_conn, (gpointer)connect_object);
            debug_log(DLOG_DEBUG, "MUTEX SDI Conn: %s", dfl_get_object_name(connect_object));
        }
    }

    g_slist_free(connect_list);
    return RET_OK;
}


LOCAL void __maintain_port_record(OBJ_HANDLE obj_handle)
{
    GSList *sdi_record_list = NULL;
    GSList *mutex_sdi_record_list = NULL;
    gint32 ret = __get_sdi_serial_record_connect(obj_handle, &sdi_record_list, &mutex_sdi_record_list);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: %s get serial record connect failed", __func__, dfl_get_object_name(obj_handle));
        return;
    }

    // 检查互斥连接，若被占用则不建立连接
    guint8 connect_status;
    guint8 availability;
    for (GSList *Node = mutex_sdi_record_list; Node; Node = Node->next) {
        OBJ_HANDLE mutex_sdi_record_object = (OBJ_HANDLE)Node->data;
        connect_status = 0;
        availability = 0;

        dal_get_property_value_byte(mutex_sdi_record_object, PROPERTY_CONNECT_STATUS, &connect_status);
        dal_get_property_value_byte(mutex_sdi_record_object, PROPERTY_AVAILABILITY, &availability);
        if (connect_status == TRUE && availability == TRUE) {
            debug_log(DLOG_DEBUG, "%s: sdi connect %s is busy", __func__, dfl_get_object_name(mutex_sdi_record_object));
            g_slist_free(mutex_sdi_record_list);
            g_slist_free(sdi_record_list);
            return;
        }
    }
    g_slist_free(mutex_sdi_record_list);

    // 建立连接
    for (GSList *Node = sdi_record_list; Node; Node = Node->next) {
        OBJ_HANDLE sdi_record_object = (OBJ_HANDLE)Node->data;
        connect_status = 0;
        availability = 0;

        dal_get_property_value_byte(sdi_record_object, PROPERTY_CONNECT_STATUS, &connect_status);
        dal_get_property_value_byte(sdi_record_object, PROPERTY_AVAILABILITY, &availability);
        if (connect_status == FALSE && availability == TRUE) {
            
            ret = dfl_call_class_method(sdi_record_object, METHOD_SET_SERIAL_DIRECTION, NULL, NULL, NULL);
            if (ret != RET_OK) {
                debug_log(DLOG_DEBUG, "%s: set serial sw: %s failed", __func__, dfl_get_object_name(sdi_record_object));
                g_slist_free(sdi_record_list);
                return;
            }
            debug_log(DLOG_DEBUG, "%s: set serial sw: %s succ", __func__, dfl_get_object_name(sdi_record_object));
        }
    }
    g_slist_free(sdi_record_list);
}


LOCAL gint32 sdi_card_update_some_hw_info(OBJ_HANDLE object_handle, gpointer data)
{
    guint8 read_value = 0;
    guint32 boardid = 0;
    guint8 mcu_support = 0;

    
    gint32 ret = dal_get_extern_value_byte(object_handle, PROPERTY_SDI_CARD_BOOT_ORDER, &read_value, DF_AUTO);
    debug_log(DLOG_DEBUG, "%s: read bootorder=%d, ret:%d", __FUNCTION__, read_value, ret);
    vos_task_delay(SDI_CARD_TASK_DELAY);

    (void)dal_get_property_value_byte(object_handle, PROPERTY_SDI_CARD_MCUSUPPORTED, &mcu_support);
    if (mcu_support == 0) {
        
        sdi_card_get_bmc_ip(object_handle);
        vos_task_delay(SDI_CARD_TASK_DELAY);

        
        sdi_card_uptade_slot(object_handle);
        vos_task_delay(SDI_CARD_TASK_DELAY);

        
        sdi_extcard_update_mac(object_handle);
        vos_task_delay(SDI_CARD_TASK_DELAY);

        
        ret = dal_get_property_value_uint32(object_handle, PROPERTY_SDI_CARD_BOARDID, &boardid);
        if (ret < 0) {
            debug_log(DLOG_ERROR, "get boardid failed ret:%d", ret);
        } else {
            if (check_hw_is_older_than_sdi_ver3(boardid)) {
                sdi_card_cpuTemp_start(object_handle);
            }
    }
    }

    
    __maintain_port_record(object_handle);

    
    return RET_OK;
}


LOCAL void sdi_card_init_hash_table(void)
{
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    OBJ_HANDLE obj_handle;
    guint8 sdi_id;

    gint32 ret = dfl_get_object_list(CLASS_PCIE_SDI_CARD, &obj_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get obj list %s failed, ret=%d", __FUNCTION__, CLASS_PCIE_SDI_CARD, ret);
        return;
    }

    for (obj_node = obj_list; obj_node != NULL; obj_node = g_slist_next(obj_node)) {
        obj_handle = (OBJ_HANDLE)obj_node->data;
        
        if (get_sdi_id_by_handle(obj_handle) == SDI_HANDLE_OUT_OF_RANGE) {
            sdi_id = add_hash_table(obj_handle);
            if (sdi_id == SDI_HANDLE_OUT_OF_RANGE) {
                debug_log(DLOG_ERROR, "%s: add sdi card obj to table failed, exceed %d", __FUNCTION__,
                    SD100_CARD_MAX_NUM);
            } else {
                debug_log(DLOG_INFO, "%s: add sdi card obj to table, sdi_id=%d", __FUNCTION__, sdi_id);
            }
        }
    }

    g_slist_free(obj_list);

    return;
}


LOCAL void sdi_card_update_hw_info_task(void)
{
    (void)prctl(PR_SET_NAME, (gulong) "updatesdiinfo");

    
    sdi_card_init_hash_table();

    for (;;) {
        vos_task_delay(5000);
        (void)dfl_foreach_object(CLASS_PCIE_SDI_CARD, sdi_card_update_some_hw_info, NULL, NULL);
    }
}


gint32 sdi_card_start(void)
{
    gulong task_update_hw_info_id = 0;

    gint32 ret = vos_task_create(&task_update_hw_info_id, "update_hw_info",
        (TASK_ENTRY)sdi_card_update_hw_info_task, 0, DEFAULT_PRIORITY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Creat task_update_hw_info_id task failed!(ret = 0x%x)", ret);
        return RET_ERR;
    }
    return RET_OK;
}
