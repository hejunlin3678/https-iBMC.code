/* 
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2020. All rights reserved. 
* Description: 数据收集相关的定义。
* Author: h00282621 
* Create: 2020-3-10 
* Notes: 用于跨模块使用的相关的定义。
*/

#ifndef __DATAACQURISITION_DEFINE_H__
#define __DATAACQURISITION_DEFINE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define COLLECTION_INTERVAL_10_MIN          600
#define COLLECTION_INTERVAL_20_MIN          (2 * COLLECTION_INTERVAL_10_MIN)
#define COLLECTION_INTERVAL_6_HOUR          (6 * 6 * COLLECTION_INTERVAL_10_MIN)
#define COLLECTION_INTERVAL_24_HOUR         (24 * 6 * COLLECTION_INTERVAL_10_MIN)

#define DA_SERVICE_FILE_PATH  "/dev/shm"
#define DA_FAIL_RETRY_TIMES   3 // 删除csv文件或者导出文件或者清空文件的重试次数

/* 支持通过iRM汇聚本柜设备的标准数据采集结果 */
#define RACK_MGNT_CFG_FILE_PATH             "/data/opt/pme/conf/rack_mgnt"	/* 机柜管理配置文件夹 */
#define URI_FORMAT_DA_SVC                   "/redfish/v1/DataAcquisitionService"
#define URI_FORMAT_MANAGERS			        "/redfish/v1/Managers"
#define RFPROP_MANAGER_DATATIME				"DateTime"              /* 时间 */
#define PROPERTY_DA_SERVICE_ENABLE          "ServiceEnabled"        /* 数据采集服务开启状态 */
#define PROP_DA_SERVICE_HEALTH              "Health"                /* 数据采集服务功能健康状态 */
#define PROP_DA_DATA                        "Data"                  /* 柜内设备数据采集数据 */
#define PROP_RACK_DA_CFG_CONTENT            "ConfigurationContent"  /* 柜内设备数据采集配置数据内容 */
#define PROP_RACK_DA_SER_NUM                "SerialNumber"          /* 柜内设备数据采集配置序列号属性 */
#define PROP_RACK_DA_ITEMS                  "AcquisitionItems"      /* 柜内设备数据采集配置项 */

#define PROPERTY_DA_REPORT_METRIC_INFO          "MetricInfo"
#define PROPERTY_DA_REPORT_TAG_INFO             "TagInfo"
#define PROPERTY_DA_REPORT_DATA_POINT           "DataPoint"
#define PROPERTY_DA_REPORT_KEY_TYPE             "Type"
#define PROPERTY_DA_REPORT_KEY_SOURCE           "Source"
#define PROPERTY_DA_REPORT_KEY_TIMESTAMP        "TimeStamp"
#define PROPERTY_DA_REPORT_KEY_VALUE            "Value"

#define MKDIR_DATA_ACQ                      "/data/opt/pme/dataacquisition"   /* 提取到公共宏 */

#define TURN_TO_KB          				1024
#define TABLE_TAG                           "Tag"
#define TABLE_METRIC                        "Metric"
#define TABLE_DATA_POINT                    "DataPoint"
#define RACK_DA_TABLE_ITEM                  "Item"
#define SQLITE_DB_FILE_TYPE					".db"
#define ITEM_DB_SYNC_FILE_NAME              "item_sync"
#define DATA_ACQ_DB_SYNC_FILE_NAME          "data_acq_sync"
#define ITEM_DB_FILE_NAME                   "item.db"
#define DATA_ACQ_DB_FILE_NAME               "data_acq.db"
#define DATA_ACQ_ANOTHER_DB_FILE_NAME       "data_acq_another.db"
#define ACTIVE_DB_SYMLINK					"active_db"

#define DATA_ACQ_DATAPOINT_TABLE_KEY        "datapointid"
#define DATA_ACQ_TAG_TABLE_KEY              "tagid"
#define DATA_ACQ_SELECT_MIN_CMD             0x00
#define DATA_ACQ_SELECT_MAX_CMD             0x01

#define DA_SERVICE_METRIC_TABLE             "Metric"
#define DA_SERVICE_TAG_TABLE                "Tag"
#define DA_SERVICE_DATAPOINT_TABLE          "DataPoint"

#define EXPORT_NOT_START    101
#define EXPORT_RUNNING      102
#define EXPORT_SUCCEED      103
#define EXPORT_FAILED       104


#define CLEAR_NOT_START      101
#define CLEAR_RUNNING        102
#define CLEAR_SUCCEED        103
#define CLEAR_UNKNOWN_FAILED 104
#define CLEAR_ERROR_BASIC_START 110
#define CLEAR_FAILED_1       111
#define CLEAR_FAILED_2       112
#define CLEAR_FAILED_3       113
#define CLEAR_FAILED_4       114
#define CLEAR_FAILED_5       115
#define CLEAR_FAILED_6       116
#define CLEAR_FAILED_7       117


/* 数据收集项ID定义 */
#define SATA_SMART_ALL_INFO_METRIC_ID        0x00080108   /* 所有SMART数据，和xml中HddSATAALLSMARTMetric对象中的配置一致 */

#define DISK_ERROR_LOG_COUNT_METRIC_ID       0x0008010A
#define DISK_ERROR_LOG_METRIC_ID             0x0008010B
#define DISK_SELF_TEST_LOG_COUNT_METRIC_ID   0x0008010C
#define DISK_SELF_TEST_LOG_METRIC_ID         0x0008010D
#define DISK_PYH_EVENT_ICRC_METRIC_ID        0x0008010E
#define DISK_GLIST_COUNT_METRIC_ID           0x0008010F
#define DISK_GLIST_METRIC_ID                 0x00080110
#define DISK_PLIST_COUNT_METRIC_ID           0x00080111
#define DISK_PLIST_METRIC_ID                 0x00080112
#define DISK_CRITICAL_EVENT_COUNT_METRIC_ID  0x00080113
#define DISK_CRITICAL_EVENT_LOG_METRIC_ID    0x00080114
#define DISK_ERROR_COUNT_LOG_METRIC_ID       0x00080115
#define DISK_TEMP_LOG_METRIC_ID              0x00080116
#define DISK_IE_LOG_METRIC_ID                0x00080117
#define DISK_BMS_COUNT_METRIC_ID             0x00080118
#define DISK_BMS_LOG_METRIC_ID               0x00080119
#define DISK_PROTOCOL_SPECIFIC_LOG_METRIC_ID 0x0008011A
#define DISK_SOLID_STATE_MEDIA_LOG_METRIC_ID 0x0008011B
#define DISK_SOLID_GEN_STATS_LOG_METRIC_ID   0x0008011C
#define DISK_VENDOR_SPECIFIC_LOG_METRIC_ID   0x0008011D


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __DATAACQURISITION_DEFINE_H__ */
