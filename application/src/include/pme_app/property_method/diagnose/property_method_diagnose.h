/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: diagnose模块管理的MemChannelMap类相关宏定义
 * History: 2022/2/15 首次创建
 */

#ifndef PROPERTY_METHOD_DIAGNOSE_H
#define PROPERTY_METHOD_DIAGNOSE_H


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define CLASS_MEM_CHANNEL_MAP                                      "MemChannelMap"
#define PROPERTY_PHYSICAL_CHANNEL_ID                               "PhysicalChannelId"
#define PROPERTY_LOGICAL_CHANNEL_ID                                "LogicalChannelId"

#define PROPERTY_FDM_DPS_ENABLE                                    "DPSEnable"

#define CLASS_DA_SERVICE                                           "DataAcquisitionService"
#define OBJ_NAME_DASERVICE                                         CLASS_DA_SERVICE
#define PROPERTY_DA_SVC_ENABLE                                     "Enable"
#define PROPERTY_DA_SVC_ACTIVE_DB_FILE_NAME                        "ActiveDBFileName"
#define PROPERTY_DA_SVC_DB_FILE_SPLIT_THRE_MB                      "DBFileSplitThresholdMB"
#define PROPERTY_DA_SVC_DB_SYNC_TO_FLASH                           "MemSyncToFlashMin"
#define PROPERTY_DA_SVC_DB_TAG_EXPORT_STATE                        "TagExportState"
#define PROPERTY_DA_SVC_DB_METRIC_EXPORT_STATE                     "MetricExportState"
#define PROPERTY_DA_SVC_DB_DATAPOINT_EXPORT_STATE                  "DataPointExportState"
#define PROPERTY_DA_SVC_DB_ITEM_REPAIR_STATE                       "ItemDBRepairState"
#define PROPERTY_DA_SVC_DB_DATAPOINT_CLEAR_STATE                   "ClearState"
#define PROPERTY_DA_SVC_ROLLBACK_PARTITION_OMITED                  "RollbackDBPartitionOmited"
#define METHOD_SET_SERVICE_ENABLE                                  "SetServiceEnable"
#define METHOD_EXPORT_TABLE                                        "ExportTable"
#define METHOD_CLEAR_TABLE                                         "ClearTable"
#define METHOD_GET_TABLE_INFO                                      "GetTableInfo"

#define CLASS_EXTERNAL_REF_INFO                                    "ExternalRefInfo"
#define PROPERTY_EXTERNAL_REF_INFO_TARGET_CLASS                    "TargetClass"
#define PROPERTY_EXTERNAL_REF_INFO_TARGET_CLS_MATCH_PROP           "TargetClassMatchProp"
#define PROPERTY_EXTERNAL_REF_INFO_BASED_CLS_MATCH_PROP            "BasedClassMatchProp"
#define PROPERTY_EXTERNAL_REF_INFO_TARGET_CLS_RELATION_PROP        "TargetClassRelationProp"
#define PROPERTY_EXTERNAL_REF_INFO_TARGET_CLS_REF_PROP             "TargetClassRefProp"

#define CLASS_DA_TAG_DEFINITION                                    "DataAcquisitionTagDefinition"
#define PROPERTY_DA_TAG_DEF_ID                                     "Id"
#define PROPERTY_DA_TAG_DEF_TAG_TYPE                               "TagType"
#define PROPERTY_DA_TAG_DEF_TAG_REF_CLS                            "TagRefClass"
#define PROPERTY_DA_TAG_DEF_SLOT_PROP_REF                          "SlotPropRef"
#define PROPERTY_DA_TAG_DEF_PRESENCE_PROP_REF                      "PresencePropRef"
#define PROPERTY_DA_TAG_DEF_CLASSIFICATION_PROP_LIST               "ClassificationPropList"
#define PROPERTY_DA_TAG_DEF_IDENTIFICATION_PROP_LIST               "IdentificationPropList"
#define PROPERTY_DA_TAG_DEF_EXT_CLASSIFICATION_INFO_REF_LIST       "ExtClassificationInfoRefList"
#define PROPERTY_DA_TAG_DEF_EXT_IDENTIFICATION_INFO_REF_LIST       "ExtIdentificationInfoRefList"
#define PROPERTY_DA_TAG_DEF_RELATED_METRIC_OBJ_LIST                "RelatedMetricObjList"
#define METHOD_UPDATE_DATA_EXTENALLY                               "UpdateDataExternally"
#define METHOD_DFP_COLLECT_HDD_INFO                                "DfpCollectHddInfo"
#define METHOD_DATA_ACQ_DIMM_ALM                                   "DataAcquDimmAlmLog"

#define CLASS_DA_METRIC                                            "DataAcquisitionMetric"
#define PROPERTY_DA_METRIC_ID                                      "Id"
#define PROPERTY_DA_METRIC_NAME                                    "Name"
#define PROPERTY_DA_METRIC_DISPLAY_NAME                            "DisplayName"
#define PROPERTY_DA_METRIC_TYPE                                    "MetricType"
#define PROPERTY_DA_METRIC_SRC_TYPE                                "SourceType"
#define PROPERTY_DA_METRIC_SRC_SENSOR_NAME                         "SourceSensorName"
#define PROPERTY_DA_METRIC_SRC_PROP_REF_INFO                       "SourcePropertyRefInfo"
#define PROPERTY_DA_METRIC_RESERVED_ID_RANGE                       "ReservedIdRange"
#define PROPERTY_DA_METRIC_UNIT                                    "Unit"
#define PROPERTY_DA_METRIC_CACHE_INTERVAL_INSEC                    "CacheIntervalInSec"
#define PROPERTY_DA_METRIC_CACHE_DURATION_INHOUR                   "CacheDurationInHour"

#define CLASS_SENSOR_NUM_DYNAMIC_APPLY_CFG                         "SensorNumDynamicApplyCfg"
#define PROPERTY_SENSOR_NUM_DYNAMIC_APPLY_CFG_TYPE                 "Type"
#define PROPERTY_SENSOR_NUM_DYNAMIC_APPLY_CFG_NUMBER_ARRAY         "NumberArray"

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* PROPERTY_METHOD_DIAGNOSE_H */

