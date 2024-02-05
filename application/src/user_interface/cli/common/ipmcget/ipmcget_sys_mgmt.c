

#include "ipmcget_sys_mgmt.h"

const gchar* event_alarm_level[EVENT_ALARM_LEVEL] = {
    "Normal",
    "Minor",
    "Major",
    "Critical",
};

// 从数据库查询数据序号
enum {
    SEL_EVENT_ID = 0,
    SEL_ALERT_TIME = 1,
    SEL_LEVEL = 2,
    SEL_EVENT_CODE = 3,
    SEL_STATUS = 4,
    SEL_EVENT_SUBJECT = 5,
    SEL_TRIG_MODE = 6,
    SEL_EVENT_DESC = 7,
    SEL_EVENT_SUGG = 8,
    SEL_OLD_EVENT_CODE = 9,
    SEL_SUBJECT_TYPE = 10,
    SEL_SN = 11,
    SEL_PN = 12,
};

void gvar_array_to_byte(GVariant *gvar, guint8 *buf, guint32 len)
{
    GVariantIter iter;
    GVariant *element_gvar = NULL;
    guint8 element_byte = 0;
    guint32 count = 0;

    g_variant_iter_init(&iter, gvar);

    while ((element_gvar = g_variant_iter_next_value(&iter)) != NULL) {
        element_byte = g_variant_get_byte(element_gvar);
        g_variant_unref(element_gvar);

        if (count >= len) {
            g_printf("gvar_array_to_byte error: count=%u len=%u\r\n", count, len);
            return;
        }

        buf[count] = element_byte;
        count++;
    }

    return;
}

gint32 get_file_transfer_state(const gchar *class_name, const gchar *obj_name, const gchar *property_name)
{
    guint8 process_bar;
    gint32 ret = 0;
    GVariant *value = NULL;
    const gint32 GET_TRANSFER_STATUS_TIMES = 3000;

    for (gint32 i = 0; i < GET_TRANSFER_STATUS_TIMES; i++) {
        ret = uip_get_object_property(class_name, obj_name, property_name, &value);
        if (ret != RET_OK) {
            return RET_ERR;
        }

        process_bar = g_variant_get_byte(value);
        g_variant_unref(value);
        value = NULL;
        if (process_bar == TASK_FINISH) {
            g_printf("\r100%%\r\n");
            (void)fflush(stdout);
            return RET_OK;
        } else if (process_bar < TASK_FINISH) {
            g_printf("\r%2u%%", process_bar);
            (void)fflush(stdout);
        } else {
            return RET_ERR;
        }

        (void)vos_task_delay(DELAY_TIME);
    }
    return RET_ERR;
}

LOCAL gint32 _get_sqlite_lock_and_handle(gint32 *sel_sqlite_lock, sqlite3 **db)
{
    gint32 tmp_lock;

    tmp_lock = open(EO_SQLITE_SYNC_FILE, O_RDONLY);
    if (tmp_lock < 0) {
        g_printf("Open DB Lock failed.\n");
        return RET_ERR;
    }

    
    db_sqlite_lock(tmp_lock);

    
    (void)db_sqlite_open(EO_SQLITE_DB_FILE, db);
    if (db == 0) {
        g_printf("Open DB failed.\n");
        db_sqlite_unlock(tmp_lock);
        close(tmp_lock);
        return RET_ERR;
    }

    
    db_sqlite_unlock(tmp_lock);

    *sel_sqlite_lock = tmp_lock;

    return RET_OK;
}


gint32 ipmc_get_timeoffset(gint16 *time_offset)
{
    gint32 ret;
    if (time_offset == NULL) {
        return RET_ERR;
    }

    ret = dal_get_localtime_offset(time_offset);
    if (ret != RET_OK) {
        *time_offset = 0;
        return RET_ERR;
    }

    return RET_OK;
}

LOCAL xmlDocPtr get_xml_doc_ptr(void)
{
    gchar filename[256] = {0};

    gint32 ret = snprintf_s(filename, sizeof(filename), sizeof(filename) - 1, "%s/%s", EVENT_LANGUAGE_FILE_PATH,
        EVENT_LANGUAGE_FILE);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, ret);
        return NULL;
    }
    xmlInitParser();
    return dal_open_xml_doc(filename);
}

LOCAL gint32 make_event_code_str(gchar *code_str, gsize str_len, gint32 event_code, gint32 old_event_code)
{
    gint32 ret = -1;
    if (event_code == 0) { // 新的事件码为0，表示当前为旧的传感器事件
        ret = snprintf_s(code_str, str_len, str_len - 1, "0x%08X", old_event_code);
    } else {
        ret = snprintf_s(code_str, str_len, str_len - 1, "0x%08X", event_code);
    }

    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s failed, ret:%d", __FUNCTION__, ret);
        return RET_ERR;
    }

    return RET_OK;
}

LOCAL void print_row(xmlDocPtr doc_ptr, gchar **azResult, gint32 col)
{
    gchar event_code_str[256] = {0};
    gchar *desc_buf = NULL;
    gchar trigmode[EVENT_MONITOR_TRIGMODE_LEN] = {0};

    dal_convert_raw_to_formated(doc_ptr, (guint8 *)azResult[col + 7],
        strlen(azResult[col + 7]), EVENT_EXT_INFO_SPLIT_CHAR, EVENT_LANGUAGE_EN,
        EVENT_LANGUAGE_DESC_NODE, &desc_buf, NULL, azResult[col + 10], azResult[col + 11]);

    gint32 event_code = -1;
    if (dal_strtoi(azResult[col + 3], &event_code, NUMBER_BASE10) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_strtoi failed", __FUNCTION__);
        goto EXIT;
    }

    gint32 old_event_code = -1;
    if (dal_strtoi(azResult[col + 8], &old_event_code, NUMBER_BASE10) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_strtoi failed", __FUNCTION__);
        goto EXIT;
    }

    if (make_event_code_str(event_code_str, sizeof(event_code_str), event_code, old_event_code) != RET_OK) {
        goto EXIT;
    }

    gint32 mode = -1;
    if (dal_strtoi(azResult[col + 6], &mode, NUMBER_BASE10) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_strtoi failed", __FUNCTION__);
        goto EXIT;
    }
    dal_get_event_desc_prefix(mode, azResult[col + 5], event_code, trigmode, sizeof(trigmode));

    gint32 level = -1;
    if (dal_strtoi(azResult[col + 2], &level, NUMBER_BASE10) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_strtoi failed", __FUNCTION__);
        goto EXIT;
    }

    gint32 status = -1;
    if (dal_strtoi(azResult[col + 4], &status, NUMBER_BASE10) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_strtoi failed", __FUNCTION__);
        goto EXIT;
    }

    gchar *desc = "UNKNOWN ERROR";
    if (desc_buf != NULL) {
        desc = desc_buf;
    }

    g_printf("%-8s | %-20s | %-12s | %-12s | %-12s | %s%s\r\n", azResult[col + 0],
        azResult[col + 1], (level > EVENT_ALARM_LEVEL_NUM - 1) ? "Unknown" : event_alarm_level[level],
        event_code_str, status ? "Asserted" : "Deasserted", trigmode, desc);

EXIT:
    g_free(desc_buf);
}


gint32 get_sel_list(void)
{
    gint32 i = 0;
    gint32 curr_id = -1;
    gint32 page_offset = 0;
    gint32 sel_num = 0;
    sqlite3 *db = NULL;
    gint32 nrow = 0;
    gint32 ncolumn = 0;
    gchar **azResult = NULL;
    gchar *zErrMsg = 0;
    gint32 user_input = 0;
    gchar sql[SQL_STR_MAXLENGTH] = {0};
    gint16 time_zone = 0;
    gint32 sel_sqlite_lock = -1;

    xmlDocPtr doc_ptr = get_xml_doc_ptr();
    if (doc_ptr == NULL) {
        return RET_ERR;
    }

    
    gint32 ret = _get_sqlite_lock_and_handle(&sel_sqlite_lock, &db);
    if (ret != RET_OK) {
        dal_free_xml_parser_object(doc_ptr, NULL);
        return ret;
    }

    if (ipmc_get_timeoffset(&time_zone) != RET_OK) {
        g_printf("Get system time failed.\r\n");
        ret = RET_ERR;
        goto EXIT;
    }
    g_printf("%-8s | %-20s | %-12s | %-12s | %-12s | %s\r\n", "ID", "Generation Time", "Severity", "Event Code",
        "Status", "Event Description");

    while (curr_id != 1) {
        // 拼装sql语句，每次取20行数据
        errno_t securec_rv = snprintf_s(sql, SQL_STR_MAXLENGTH, SQL_STR_MAXLENGTH - 1,
            "select eventid, datetime(alerttime + %d, 'unixepoch') as alerttime, level, eventcode, status, eventsubject, trigmode, eventdesc, oldeventcode, subjecttype, sn, pn \
                from %s left join monitor_table on monitor_table.evtid=event_table.eventid and monitor_table.evttime=event_table.alerttime order by eventid desc limit %d, %d;",
            time_zone * 60, EO_SQLITE_TABLE_NAME, page_offset * ONEPAGE_RECORD_NUMBER, ONEPAGE_RECORD_NUMBER);
        if (securec_rv <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, securec_rv);
        }

        
        db_sqlite_lock(sel_sqlite_lock);
        gint32 rc = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
        if (rc != SQLITE_OK) {
            g_printf("SQL error: %s\n", zErrMsg);
            db_sqlite_unlock(sel_sqlite_lock);
            ret = RET_ERR;
            goto EXIT;
        }

        
        if (nrow == 0) {
            sqlite3_free_table(azResult);
            db_sqlite_unlock(sel_sqlite_lock);
            ret = RET_OK;
            break;
        }

        // 开始逐行打印
        for (i = 1; i < (nrow + 1); i++) {
            gint32 col = i * ncolumn;
            print_row(doc_ptr, azResult, col);
            // 每次都把当前行的selid赋值给curr_id
            if (dal_strtoi(azResult[col], &curr_id, NUMBER_BASE10) != RET_OK) {
                debug_log(DLOG_ERROR, "%s: dal_strtoi failed", __FUNCTION__);
            }
            sel_num++;

            // 打印是否退出的提示
            if ((sel_num == ONEPAGE_RECORD_NUMBER) && (curr_id != 1)) {
                db_sqlite_unlock(sel_sqlite_lock);
                g_printf("\r\n");

                user_input = ipmc_get_user_input("Input 'q' quit:"); // 仅取第一个字符进行判断
                ret = (user_input == 'q') || (user_input == 'Q');
                if (ret) {
                    sqlite3_free_table(azResult);
                    ret = RET_OK;
                    goto EXIT;
                } else {
                    db_sqlite_lock(sel_sqlite_lock);
                }

                sel_num = 0;
            }
        }

        page_offset++;
        sqlite3_free_table(azResult);
        db_sqlite_unlock(sel_sqlite_lock);
    }

EXIT:
    dal_free_xml_parser_object(doc_ptr, NULL);
    db_sqlite_close(db);
    close(sel_sqlite_lock);
    return ret;
}


LOCAL void _get_info_from_config(gchar *version, guint32 version_len, guint32 *max_event_count)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;

    ret = dfl_get_object_handle(CLASS_EVENT_CONFIGURATION, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s] get obj handle %s failed!\n", __FUNCTION__, CLASS_EVENT_CONFIGURATION);
        return;
    }

    ret = dal_get_property_value_string(obj_handle, PROPERTY_EVENT_CONFIGURATION_VERSION, version, version_len);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] get property %s failed!\n", __FUNCTION__, PROPERTY_EVENT_CONFIGURATION_VERSION);
        return;
    }

    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_EVENT_CONFIGURATION_MAX_EVENT_NUM, max_event_count);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] get property %s failed!\n", __FUNCTION__,
            PROPERTY_EVENT_CONFIGURATION_MAX_EVENT_NUM);
        return;
    }
    return;
}


LOCAL void _get_info_from_db(gint32 *cur_event_count)
{
    gchar data_buf[SQL_STR_MAXLENGTH] = {0};
    gchar **result = NULL;
    gchar *errmsg = NULL;
    gint32 nrow = 0;
    gint32 ncolumn = 0;
    gint32 sel_sqlite_lock = -1;
    sqlite3 *db = NULL;

    gint32 ret = snprintf_s(data_buf, SQL_STR_MAXLENGTH, SQL_STR_MAXLENGTH - 1, "SELECT max(eventid) FROM %s;",
        EO_SQLITE_TABLE_NAME);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        return;
    }

    
    ret = _get_sqlite_lock_and_handle(&sel_sqlite_lock, &db);
    if (ret != RET_OK) {
        return;
    }

    db_sqlite_lock(sel_sqlite_lock);
    ret = sqlite3_get_table(db, data_buf, &result, &nrow, &ncolumn, &errmsg);
    if (ret != SQLITE_OK) {
        g_printf("[%s] get count sql error:%s\n", __FUNCTION__, errmsg);
        goto EXIT;
    }

    if (nrow == 0) {
        g_printf("[%s]get count:nrow = 0\n", __FUNCTION__);
        goto EXIT;
    }

    if (result[1] != NULL) {
        gint32 event_count = -1;
        if (dal_strtoi(result[1], &event_count, NUMBER_BASE10) != RET_OK) {
            goto EXIT;
        }

        *cur_event_count = event_count;
    }

EXIT:
    sqlite3_free_table(result);
    db_sqlite_unlock(sel_sqlite_lock);
    db_sqlite_close(db);
    close(sel_sqlite_lock);

    return;
}


void get_sel_info(void)
{
    gchar version[MAX_VERSION_LEN] = {0};
    gint32 cur_event_count = 0;
    guint32 max_event_count = 0;

    _get_info_from_config(version, sizeof(version), &max_event_count);
    _get_info_from_db(&cur_event_count);

    
    g_printf("SEL Information\r\n");
    g_printf("Version               :  %s\r\n", version);
    g_printf("Current Event Number  :  %d\r\n", cur_event_count);
    g_printf("Max Event Number      :  %d\r\n", max_event_count);

    return;
}

LOCAL void print_basics_info(gchar **azResult, gint32 ncolumn, const gchar *event_code_str, gint32 status)
{
    gint32 level = -1;
    if (dal_strtoi(azResult[ncolumn + SEL_LEVEL], &level, NUMBER_BASE10) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_strtoi failed", __FUNCTION__);
    }

    g_printf("%-20s:  %s\r\n", "ID", azResult[ncolumn + SEL_EVENT_ID]);
    g_printf("%-20s:  %s\r\n", "Generation Time", azResult[ncolumn + SEL_ALERT_TIME]);
    g_printf("%-20s:  %s\r\n", "Severity", (level > EVENT_ALARM_LEVEL_NUM - 1) ? "Unknown" : event_alarm_level[level]);
    g_printf("%-20s:  %s\r\n", "Event Code", event_code_str);
    g_printf("%-20s:  %s\r\n", "Status", status ? "Asserted" : "Deasserted");
}

LOCAL void print_description(xmlDocPtr doc_ptr, gchar **azResult, gint32 ncolumn, gint32 event_code)
{
    gchar *desc_buf = NULL;
    gchar trigmode[EVENT_MONITOR_TRIGMODE_LEN] = {0};
    gchar *eventsubject = azResult[ncolumn + SEL_EVENT_SUBJECT];
    gchar *eventdesc = azResult[ncolumn + SEL_EVENT_DESC];
    gchar *sn = azResult[ncolumn + SEL_SN];
    gchar *pn = azResult[ncolumn + SEL_PN];

    dal_convert_raw_to_formated(doc_ptr, (guint8 *)eventdesc, strlen(eventdesc),
        EVENT_EXT_INFO_SPLIT_CHAR, EVENT_LANGUAGE_EN, EVENT_LANGUAGE_DESC_NODE, &desc_buf, NULL, sn, pn);

    gint32 mode = -1;
    if (dal_strtoi(azResult[ncolumn + SEL_TRIG_MODE], &mode, NUMBER_BASE10) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_strtoi failed", __FUNCTION__);
    }

    dal_get_event_desc_prefix(mode, eventsubject, event_code, trigmode, sizeof(trigmode));

    if (desc_buf == NULL) {
        g_printf("%-20s:  %s%s\r\n", "Event Description", trigmode, EVENT_LANGUAGE_DEFAULT_NODE_CONTENT);
    } else {
        g_printf("%-20s:  %s%s\r\n", "Event Description", trigmode, desc_buf);
    }

    g_free(desc_buf);
}

LOCAL void print_suggestion(xmlDocPtr doc_ptr, gchar **azResult, gint32 ncolumn, gint32 status)
{
    gchar *eventsugg = azResult[ncolumn + SEL_EVENT_SUGG];
    gchar *sugg_buf = NULL;
    gchar sugg_split[] = "@#AB;";
    gchar *p = NULL;

    dal_convert_raw_to_formated(doc_ptr, (guint8 *)eventsugg, strlen(eventsugg),
        EVENT_EXT_INFO_SPLIT_CHAR, EVENT_LANGUAGE_EN, EVENT_LANGUAGE_SUGG_NODE, &sugg_buf, NULL, NULL,
        NULL); // 处理建议不追加BOM/SN

    if (status == 0 || sugg_buf == NULL) {
        g_printf("%-20s:  %s\r\n", "Suggestion", EVENT_LANGUAGE_DEFAULT_NODE_CONTENT);
        return;
    }

    gchar *p_start = sugg_buf;

    for (gint32 i = 0; i < 100; i++) {
        p = strstr(p_start, sugg_split);
        if (p == NULL) {
            if (i == 0) {
                g_printf("%-20s:  %s\r\n", "Suggestion", p_start);
            } else {
                g_printf("%-20s   %s\r\n", "", p_start);
            }

            break;
        }

        p[0] = 0;

        if (i == 0) {
            g_printf("%-20s:  %s\r\n", "Suggestion", p_start);
        } else {
            g_printf("%-20s   %s\r\n", "", p_start);
        }

        p_start = p + strlen(sugg_split);
        p = NULL;
    }

    g_free(sugg_buf);
}


gint32 get_sel_suggestion(gint32 selid)
{
    sqlite3 *db = NULL;
    gint32 nrow = 0;
    gint32 ncolumn = 0;
    gchar sql[SQL_STR_MAXLENGTH] = {0};
    gint16 time_zone = 0;
    gint32 sel_sqlite_lock = -1;
    gchar event_code_str[256] = {0};
    gchar **azResult = NULL;
    gchar *errmsg = NULL;

    xmlDocPtr doc_ptr = get_xml_doc_ptr();
    if (doc_ptr == NULL) {
        return RET_ERR;
    }

    
    gint32 ret = _get_sqlite_lock_and_handle(&sel_sqlite_lock, &db);
    if (ret != RET_OK) {
        dal_free_xml_parser_object(doc_ptr, NULL);
        return ret;
    }

    if (ipmc_get_timeoffset(&time_zone) != RET_OK) {
        g_printf("Get system time failed.\r\n");
        ret = RET_ERR;
        goto FREE_EXIT;
    }

    // 拼装查询的sql语句
    ret = snprintf_s(sql, SQL_STR_MAXLENGTH, SQL_STR_MAXLENGTH - 1,
        "select eventid, datetime(alerttime + %d, 'unixepoch') as alerttime, level, eventcode, status, eventsubject, trigmode, \
            eventdesc, eventsugg, oldeventcode, subjecttype, sn, pn from %s left join monitor_table on monitor_table.evtid=event_table.eventid and monitor_table.evttime=event_table.alerttime where eventid = %d order by eventid desc;",
        time_zone * 60, EO_SQLITE_TABLE_NAME, selid);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, ret);
        ret = RET_ERR;
        goto FREE_EXIT;
    }

    db_sqlite_lock(sel_sqlite_lock);
    ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &errmsg);
    if (ret != RET_OK) {
        g_printf("Get suggestion failed: %s\r\n", errmsg);
        ret = RET_ERR;
        goto UNLOCK_EXIT;
    }

    if (nrow == 0) {
        g_printf("Get event suggestion failed\n");
        ret = RET_ERR;
        goto UNLOCK_EXIT;
    }

    gint32 event_code = -1;
    if (dal_strtoi(azResult[ncolumn + SEL_EVENT_CODE], &event_code, NUMBER_BASE10) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_strtoi failed", __FUNCTION__);
        ret = RET_ERR;
        goto UNLOCK_EXIT;
    }

    gint32 old_event_code = -1;
    if (dal_strtoi(azResult[ncolumn + SEL_OLD_EVENT_CODE], &old_event_code, NUMBER_BASE10) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_strtoi failed", __FUNCTION__);
        ret = RET_ERR;
        goto UNLOCK_EXIT;
    }

    if (event_code == 0) {  // 新的事件码为0，表示当前为旧的传感器事件
        snprintf_s(event_code_str, sizeof(event_code_str), sizeof(event_code_str) - 1, "0x%08X", old_event_code);
    } else {
        snprintf_s(event_code_str, sizeof(event_code_str), sizeof(event_code_str) - 1, "0x%08X", event_code);
    }

    gint32 status = -1;
    if (dal_strtoi(azResult[ncolumn + SEL_STATUS], &status, NUMBER_BASE10) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_strtoi failed", __FUNCTION__);
        ret = RET_ERR;
        goto UNLOCK_EXIT;
    }

    print_basics_info(azResult, ncolumn, event_code_str, status);
    print_description(doc_ptr, azResult, ncolumn, event_code);
    print_suggestion(doc_ptr, azResult, ncolumn, status);

UNLOCK_EXIT:
    db_sqlite_unlock(sel_sqlite_lock);

FREE_EXIT:
    sqlite3_free_table(azResult);
    dal_free_xml_parser_object(doc_ptr, NULL);
    db_sqlite_close(db);
    close(sel_sqlite_lock);

    return ret;
}

void __destruct_fru_basic_props(struct fru_primary_props *p)
{
    if (p == NULL) {
        return;
    }
    if (p->fru_name != NULL) {
        g_free(p->fru_name);
    }
    if (p->ref_obj_elabel != NULL) {
        g_free(p->ref_obj_elabel);
    }
    return;
}

gint32 __extract_fru_basic_props(OBJ_HANDLE obj_handle, struct fru_primary_props *fru_basic_props, gint32 *p_retval)
{
    GSList *property_value_fru = NULL;
    GSList *property_name_fru = NULL;
    gsize len;

    property_name_fru = g_slist_append(property_name_fru, PROPERTY_FRU_ID);
    property_name_fru = g_slist_append(property_name_fru, PROPERTY_FRU_TYPE);
    property_name_fru = g_slist_append(property_name_fru, PROPERTY_FRU_NAME);
    property_name_fru = g_slist_append(property_name_fru, PROPERTY_FRU_ELABEL_RO);
    gint32 ret = dfl_multiget_property_value(obj_handle, property_name_fru, &property_value_fru);
    g_slist_free(property_name_fru);
    if (ret) {
        *p_retval = RET_ERR;
        debug_log(DLOG_ERROR, "%s: dfl_multiget_property_value failed", __FUNCTION__);
        return RET_ERR;
    }

    fru_basic_props->fru_id = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_fru, FRU_ID_IDX));
    fru_basic_props->fru_type = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_fru, FRU_TYPE_IDX));
    fru_basic_props->fru_name =
        g_variant_dup_string((GVariant *)g_slist_nth_data(property_value_fru, FRU_NAME_IDX), &len);
    fru_basic_props->ref_obj_elabel =
        g_variant_dup_string((GVariant *)g_slist_nth_data(property_value_fru, REF_OBJ_ELABEL_IDX), &len);
    uip_free_gvariant_list(property_value_fru);
    if (fru_basic_props->fru_name == NULL || fru_basic_props->ref_obj_elabel == NULL) {
        *p_retval = RET_ERR;
        debug_log(DLOG_ERROR, "%s: g_variant_dup_string failed", __FUNCTION__);
        return RET_ERR;
    }
    guchar mgmt_software_type = 0xff;
    (void)dal_get_software_type(&mgmt_software_type);
    if (mgmt_software_type != MGMT_SOFTWARE_TYPE_EM) {
        if (fru_basic_props->fru_type == FRU_TYPE_POWER) {
            *p_retval = RET_OK;
            debug_log(DLOG_ERROR, "%s: fru_type == FRU_TYPE_POWER", __FUNCTION__);
            return RET_ERR;
        }
    }
    return RET_OK;
}

gint32 __extract_fru_props(OBJ_HANDLE obj_handle, struct fru_primary_props *fru_basic_props, GSList **property_value)
{
    GSList *property_name_list = NULL;
    OBJ_HANDLE ref_handle = -1;
    gchar *elable_property_macros[ELABLE_PROPERTIES_NUM] = {
        PROPERTY_ELABEL_CHASSIS_TYPE, PROPERTY_ELABEL_CHASSIS_PN, PROPERTY_ELABEL_CHASSIS_SN,
        PROPERTY_ELABEL_CHASSIS_EL, PROPERTY_ELABEL_BOARD_MFG_TIME, PROPERTY_ELABEL_BOARD_MFG_NAME,
        PROPERTY_ELABEL_BOARD_PRO_NAME, PROPERTY_ELABEL_BOARD_SN, PROPERTY_ELABEL_BOARD_PN,
        PROPERTY_ELABEL_BOARD_ID, PROPERTY_ELABEL_BOARD_EL, PROPERTY_ELABEL_PRODUCT_MFG_NAME,
        PROPERTY_ELABEL_PRODUCT_NAME, PROPERTY_ELABEL_PRODUCT_PN, PROPERTY_ELABEL_PRODUCT_VERSION,
        PROPERTY_ELABEL_PRODUCT_SN, PROPERTY_ELABEL_PRODUCT_ASSET_TAG, PROPERTY_ELABEL_PRODUCT_ID,
        PROPERTY_ELABEL_PRODUCT_EL
    };

    for (int i = 0; i < ELABLE_PROPERTIES_NUM; ++i) {
        property_name_list = g_slist_append(property_name_list, elable_property_macros[i]);
    }
    g_printf("FRU Device Description : Builtin FRU Device (FRUID %d, %s)\n", fru_basic_props->fru_id,
        fru_basic_props->fru_name);

    gint32 ret = dfl_get_referenced_object(obj_handle, PROPERTY_FRU_ELABEL_RIMM_OBJ, &ref_handle);
    if (ret == 0) {
        ret = uip_multiget_object_property(CLASS_PERIPHERAL_ELABEL, dfl_get_object_name(ref_handle), property_name_list,
            property_value);
    } else {
        ret = uip_multiget_object_property(CLASS_ELABEL, fru_basic_props->ref_obj_elabel, property_name_list,
            property_value);
    }
    g_slist_free(property_name_list);
    if (ret) {
        debug_log(DLOG_ERROR, "%s: uip_multiget_object_property failed", __FUNCTION__);
        return RET_ERR;
    }
    return RET_OK;
}

LOCAL int __print_fru_basic_info(GSList *property_value, const gchar **fru_prop_names, const gint32 props_len,
    gint32 begin, gint32 end)
{
    const gchar *val = NULL;
    gsize len;

    if (begin < 0 || begin > end || end > props_len) {
        debug_log(DLOG_ERROR, "%s: print out of range. props_len: %d, begin: %d, end: %d", __FUNCTION__, props_len,
            begin, end);
        return RET_ERR;
    }

    for (int i = begin; i <= end; ++i) {
        val = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, i), &len);
        if (len) {
            g_printf("\r %s: %s\n", fru_prop_names[i], val);
        }
    }
    return RET_OK;
}

LOCAL int __print_fru_extend_info(GSList *property_value, const gchar **fru_prop_names, const gint32 props_len,
    gint32 idx)
{
    const gchar *val = NULL;
    gsize len;
    errno_t safe_fun_ret;
    gchar *p = NULL;
    gchar *delim = ";";
    gchar *new_elabel = NULL;
    gchar elabel[ELABEL_MAX_LEN + 1] = {0};

    if (idx < 0 || idx >= props_len) {
        debug_log(DLOG_ERROR, "%s: print out of range. props_len: %d, idx: %d", __FUNCTION__, props_len, idx);
        return RET_ERR;
    }

    val = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, idx), &len);
    if (len == 0) {
        return RET_OK;
    }

    g_printf("\r %s: ", fru_prop_names[idx]);
    safe_fun_ret = strncpy_s(elabel, ELABEL_MAX_LEN + 1, val, ELABEL_MAX_LEN);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
    // 分割PRODUCT扩展电子标签字符串
    p = strtok_r(elabel, delim, &new_elabel);
    if (p == NULL) {
        return RET_ERR;
    }
    g_printf("%s\n", p);
    while ((p = strtok_r(new_elabel, delim, &new_elabel))) {
        g_printf("\r                         %s\n", p);
    }

    return RET_OK;
}

gint32 __print_fru_props(GSList *fru_prop_value, const gchar **fru_prop_names, const gint32 props_len)
{
    gint32 ret = __print_fru_basic_info(fru_prop_value, fru_prop_names, ELABLE_PROPERTIES_NUM, CHASSIS_TYPE_IDX,
        CHASSIS_SERIAL_NUM_IDX);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: __print_fru_basic_info failed within \
            [CHASSIS_TYPE_IDX, CHASSIS_SERIAL_NUM_IDX]",
            __FUNCTION__);
        return RET_ERR;
    }

    ret = __print_fru_extend_info(fru_prop_value, fru_prop_names, ELABLE_PROPERTIES_NUM, CHASSIS_EXTEND_LABLE_IDX);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: __print_fru_extend_info failed at CHASSIS_EXTEND_LABLE_IDX", __FUNCTION__);
        return RET_ERR;
    }

    ret = __print_fru_basic_info(fru_prop_value, fru_prop_names, ELABLE_PROPERTIES_NUM, BOARD_MFG_DATE_IDX,
        BOARD_FRU_FILE_ID_IDX);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: __print_fru_basic_info failed within \
            [BOARD_MFG_DATE_IDX, BOARD_FRU_FILE_ID_IDX]",
            __FUNCTION__);
        return RET_ERR;
    }

    ret = __print_fru_extend_info(fru_prop_value, fru_prop_names, ELABLE_PROPERTIES_NUM, EXTEND_LABEL_IDX);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: __print_fru_extend_info failed at EXTEND_LABEL_IDX", __FUNCTION__);
        return RET_ERR;
    }

    ret = __print_fru_basic_info(fru_prop_value, fru_prop_names, ELABLE_PROPERTIES_NUM, PRODUCT_MANU_IDX,
        PRODUCT_FRU_FILE_IDX);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: __print_fru_basic_info failed within \
            [BOARD_MFG_DATE_IDX, BOARD_FRU_FILE_ID_IDX]",
            __FUNCTION__);
        return RET_ERR;
    }

    ret = __print_fru_extend_info(fru_prop_value, fru_prop_names, ELABLE_PROPERTIES_NUM, PRODUCT_EXTEND_LABEL_IDX);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: __print_fru_extend_info failed at PRODUCT_EXTEND_LABEL_IDX", __FUNCTION__);
        return RET_ERR;
    }

    g_printf("\n");
    return RET_OK;
}

void array_object_handle_by_fruid(FRU_OBJECT_S *fru_object, guint32 count)
{
    guint32 i;
    guint32 j = 0;
    FRU_OBJECT_S object_tmp;

    if (fru_object == NULL) {
        return;
    }

    memset_s(&object_tmp, sizeof(FRU_OBJECT_S), 0, sizeof(FRU_OBJECT_S));

    for (i = 0; i < count; i++) {
        for (j = 0; j < count - i - 1; j++) {
            if (fru_object[j].fru_id > fru_object[j + 1].fru_id) {
                memcpy_s(&object_tmp, sizeof(FRU_OBJECT_S), &fru_object[j], sizeof(FRU_OBJECT_S));
                memcpy_s(&fru_object[j], sizeof(FRU_OBJECT_S), &fru_object[j + 1], sizeof(FRU_OBJECT_S));
                memcpy_s(&fru_object[j + 1], sizeof(FRU_OBJECT_S), &object_tmp, sizeof(FRU_OBJECT_S));
            }
        }
    }

    return;
}
