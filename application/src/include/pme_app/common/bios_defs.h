/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2020. All rights reserved.
 * Description: BIOS配置相关的定义。
 * Author: h00282621
 * Create: 2020-3-10
 * Notes: 用于跨模块使用的BIOS配置相关定义。
 */

#ifndef __BIOS_DEFINE_H__
#define __BIOS_DEFINE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define REGRIST_CHECK_FILE_PATH "/data/opt/pme/conf/bios/regrist.json"
#define REGRIST_FILE_NAME_LENGTH 256

#define BIOS_JSON_FILE_REGISTRY_NAME "registry.json"
#define BIOS_JSON_FILE_CURRENT_VALUE_NAME "currentvalue.json"
#define BIOS_JSON_FILE_SETTING_NAME "setting.json"
#define BIOS_JSON_FILE_RESULT_NAME "result.json"
#define BIOS_JSON_FILE_POLICYONFIGREGISTRY_NAME "PolicyConfigRegistry.json"
#define BIOS_JSON_FILE_CONFIGVALUE_NAME "ConfigValue.json"
#define BIOS_JSON_FILE_COMPONENTVERSION_NAME "ComponentVersion.json"


#define ERR_INVALID_FILE_NAME (-5001)      /* 文件名无效 */
#define ERR_INVALID_WRITE_CONTENT (-5002)  /* 要写入文件的内容无效 */
#define ERR_FILE_NOT_SUPPORT_WRITE (-5003) /* 文件不支持写操作 */
#define ERR_NOT_SUPPORT_BIOS_MGNT (-5004)  /* 环境不支持BIOS资源管理 */

#define REGRIST_TYPE_PROP_UNKNOWN 2              // MSGID_PROP_UNKNOWN
#define REGRIST_TYPE_PROP_IMMUTABLE 3            // PropertyImmutable
#define REGRIST_TYPE_PROP_READONLY 4             // MSGID_PROP_NOT_WRITABLE
#define REGRIST_TYPE_PROP_NOT_LIST 5             // MSGID_PROP_NOT_IN_LIST
#define REGRIST_TYPE_PROP_LENGTH_ERR 6           // MSGID_PROP_FORMAT_ERR
#define REGRIST_TYPE_PROP_FORMAT_ERR 7           // MSGID_PROP_FORMAT_ERR
#define REGRIST_TYPE_PROP_NOT_RANGE 8            // MSGID_VALUE_OUT_OF_RANGE
#define REGRIST_TYPE_PROP_TYPE_ERROR 9           // MSGID_PROP_TYPE_ERR
#define REGRIST_TYPE_PROP_SCALAR_INCREMENT 10    // PropertyScalarIncrement
#define REGRIST_TYPE_PROP_HIDE 11                // PropertyHidden
#define REGRIST_TYPE_PROP_BOOTTYPEORDER_ERROR 20 // SettingBootOrderFailed


#define REGRIST_PROP_COMPARE_EQU "EQU" // : "The logical operation for 'Equal'.",
#define REGRIST_PROP_COMPARE_NEQ "NEQ" // : "The logical operation for 'Not Equal'.",
#define REGRIST_PROP_COMPARE_GTR "GTR" // : "The logical operation for 'Greater than'.",
#define REGRIST_PROP_COMPARE_GEQ "GEQ" // : "The logical operation for 'Greater than or Equal'.",
#define REGRIST_PROP_COMPARE_LSS "LSS" // : "The logical operation for 'Less than'.",
#define REGRIST_PROP_COMPARE_LEQ "LEQ" // : "The logical operation for 'Less than or Equal'."


#define REGRIST_PROP_TYPE_ENUM "Enumeration"
#define REGRIST_PROP_TYPE_STRING "String"
#define REGRIST_PROP_TYPE_INTEGER "Integer"
#define REGRIST_PROP_TYPE_BOOLEAN "Boolean"
#define REGRIST_PROP_TYPE_PASSWORD "Password"
#define REGRIST_PROP_REGISTRYENTRIES "RegistryEntries"
#define REGRIST_PROP_ATTRIBUTES "Attributes"
#define REGRIST_PROP_DEPENDENCIES "Dependencies"
#define REGRIST_PROP_ATTRIBUTENAME "AttributeName"
#define REGRIST_PROP_IMMUTABLE "Immutable"
#define REGRIST_PROP_HIDDEN "Hidden"
#define REGRIST_PROP_READONLY "ReadOnly"
#define REGRIST_PROP_TYPE "Type"
#define REGRIST_PROP_VALUE "Value"
#define REGRIST_PROP_VALUENAME "ValueName"
#define REGRIST_PROP_MAXLENGTH "MaxLength"
#define REGRIST_PROP_MINLENGTH "MinLength"
#define REGRIST_PROP_VALUEEXPRESSION "ValueExpression"
#define REGRIST_PROP_UPPERBOUND "UpperBound"
#define REGRIST_PROP_LOWERBOUND "LowerBound"
#define REGRIST_PROP_SCALARINCREMET "ScalarIncrement"
#define REGRIST_PROP_CURRENTVALUE "CurrentValue"
#define REGRIST_PROP_DEPENDENCYFOR "DependencyFor"
#define REGRIST_PROP_DEPENDENCY "Dependency"
#define REGRIST_PROP_MAPFORM "MapFrom"
#define REGRIST_PROP_MAPFORMATTRIBUTE "MapFromAttribute"
#define REGRIST_PROP_MAPFROMPROPERTY "MapFromProperty"
#define REGRIST_PROP_MAPTERMS "MapTerms"
#define REGRIST_PROP_MAPFROMCONDITION "MapFromCondition"
#define REGRIST_PROP_MAPFROMVALUE "MapFromValue"
#define REGRIST_PROP_MAPTOATTRIBUTE "MapToAttribute"
#define REGRIST_PROP_MAPTOPROPERTY "MapToProperty"
#define REGRIST_PROP_MAPTOVALUE "MapToValue"
#define REGRIST_PROP_BOOTTYPEORDER "BootTypeOrder"
#define REGRIST_PROP_BOOTTYPEORDER0 "BootTypeOrder0"
#define REGRIST_PROP_BOOTTYPEORDER1 "BootTypeOrder1"
#define REGRIST_PROP_BOOTTYPEORDER2 "BootTypeOrder2"
#define REGRIST_PROP_BOOTTYPEORDER3 "BootTypeOrder3"
#define REGRIST_PROP_BOOTTYPEORDERNUM 4


#define REGRIST_STRING_AND "AND"
#define REGRIST_STRING_OR "OR"


#define WOL_STATE_ON 1
#define WOL_STATE_OFF 0

#define NO_ONCE_UPDATE_SMBIOS 0
#define UPDATED_SMBIOS_AT_LEASE_ONCE 1


#define BIOS_BOOT_DEVICE_NUM 4

#define BOOT_ORDER_TPYE_HDD 0
#define BOOT_ORDER_TPYE_CD 1
#define BOOT_ORDER_TPYE_PXE 2
#define BOOT_ORDER_TPYE_OTHERS 3

#define BIOS_BOOT_LEGACY 0
#define BIOS_BOOT_UEFI 1

#define BIOS_BOOT_MODE_SW_OFF 0
#define BIOS_BOOT_MODE_SW_ON 1

#define BIOS_FILE_STREAM_SIZE 1

#define BIOS_BOOT_MODE_SW_ENABLE_OFF 0
#define BIOS_BOOT_MODE_SW_ENABLE_ON 1

/* 启动设备的生效次数 */
#define EFFECTIVE_NONE 0    // 不生效
#define EFFECTIVE_ONCE 1    // 生效一次
#define EFFECTIVE_FOREVER 2 // 永久生效

#define BIOS_CFG_ITEM_DATA_MAX_LEN 256
#define BIOS_CFG_ITEM_SUCCESS 0
#define BIOS_CFG_ITEM_FAILED 1
#define BIOS_CFG_ITEM_SAME_FLAG_OFFSET 0x80

#define BIOS_DEMT_ENABLED "Enabled"
#define BIOS_DEMT_DISABLED "Disabled"
#define BIOS_CFG_DESP_NULL ""

#define BIOS_INDEX_INTERVAL_LEN 2

#define BIOS_ERROR_INVALID_INPUT_PARA 0xe001                 // 入参校验错误
#define BIOS_ERROR_STATUS_UNSUPPORTED 0xe002                 // SMI中断第一次触发BIOS未取数据再次触发时报此错误
#define BIOS_ERROR_MATHOD_OPERATION 0xe003                   // 方法操作内部错误
#define BIOS_ERROR_BIOS_STATUS_UNSUPPORTED 0xe004            // BIOS当前状态不支持(BIOS版本不匹配或者BIOS未启动)
#define BIOS_ERROR_ENERGY_SAVING_DISABLED 0xe005             // 智能节能开关未开启
#define BIOS_ERROR_LICENSE_NOT_INSTALLED 0xe006              // License未安装
#define BIOS_ERROR_PRODUCT_UNSUPPORTED 0xe007                // 产品不支持
#define BIOS_ERROR_ENERGY_SAVING_CFG_NOT_ALLOWED 0xe008      // 取消智能节能10分钟时间到，不允许下发新的配置
#define BIOS_ERROR_ENERGY_SAVING_ENABLED_NOT_ALLOWED 0xe009  // 取消智能节能增加BMA运行状态判断，BMA未运不允许使能
#define BIOS_ERROR_ENERGY_SAVING_POWER_MODE_NOT_ALLOW 0xe00a // N+R备电使能后，不允许智能节能设置电源模式

#define BIOS_PWD_LEN_MAX 32
#define METHOD_BIOS_CHANGE_PWD_UNSUPPORTED 0xe001   /* 不支持的密码类型 */
#define METHOD_BIOS_CHANGE_PWD_LEN_INVALID 0xe002   /* 长度不合要求 */

// 安全启动相关类方法返回码
#define SECUREBOOT_METHOD_OK                    RET_OK
#define SECUREBOOT_METHOD_ERR                   RET_ERR
#define SECUREBOOT_METHOD_INVALID_PARAM         (-2) // 参数无效
#define SECUREBOOT_METHOD_EXPIRED_CERT          (-3) // 过期证书
#define SECUREBOOT_METHOD_BIOS_STATE_UNSUPPORT  (-4) // BIOS正在启动阶段，不支持相关操作
#define SECUREBOOT_METHOD_PEM_FORMAT_ERR        (-5) // PEM格式错误
#define SECUREBOOT_METHOD_STRING_TOO_LONG       (-6) // 字符串参数过长
#define SECUREBOOT_METHOD_IMPORT_LIMIT          (-7) // 超过导入限制
 

// 安全启动相关类方法返回码
#define SECUREBOOT_METHOD_OK                    RET_OK
#define SECUREBOOT_METHOD_ERR                   RET_ERR
#define SECUREBOOT_METHOD_INVALID_PARAM         (-2) // 参数无效
#define SECUREBOOT_METHOD_EXPIRED_CERT          (-3) // 过期证书
#define SECUREBOOT_METHOD_BIOS_STATE_UNSUPPORT  (-4) // BIOS正在启动阶段，不支持相关操作
#define SECUREBOOT_METHOD_PEM_FORMAT_ERR        (-5) // PEM格式错误
#define SECUREBOOT_METHOD_STRING_TOO_LONG       (-6) // 字符串参数过长
#define SECUREBOOT_METHOD_IMPORT_LIMIT          (-7) // 超过导入限制

// 启动证书状态
#define BOOT_CERT_STATUS_COMPLETE    0     // 更新完成
#define BOOT_CERT_STATUS_UPDATE      1     // 更新中
#define BOOT_CERT_STATUS_NA          0xff  // 不涉及证书状态

#define CERT_APPLY_MODE_NOT_SUPPORT  0     // 不支持
#define CERT_APPLY_MODE_BT_CHANNEL   1     // BT通道
#define CERT_APPLY_MODE_MCU_OPCODE   2     // MCU功能码

typedef enum {
    BIOS_ENERGY_MODE_DISABLED = 0,
    BIOS_ENERGY_MODE_ENABLED,
    BIOS_ENERGY_MODE_INVALID
} BIOS_ENERGY_MODE;

typedef enum {
    BIOS_DEMT_CFG_DISABLED = 0,
    BIOS_DEMT_CFG_ENABLED,
    BIOS_DEMT_CFG_INVALID
} BIOS_DEMT_CFG_VALUE;

typedef enum {
    BIOS_DYNAMIC_INVALID_ITEM = 0x0,
    BIOS_DYNAMIC_DEMT,
    BIOS_DYNAMIC_UFS,
    BIOS_DYNAMIC_ENERGY_PERF_BIAS_CFG,
    BIOS_DYNAMIC_PL1_POWER_LIMIT,
    BIOS_DYNAMIC_PL2_POWER_LIMIT,
    BIOS_DYNAMIC_PL1_TIME_WINDOWS,
    BIOS_DYNAMIC_PL2_TIME_WINDOWS,
    BIOS_DYNAMIC_LOADLINE_SWITCH
} BIOS_DYNAMCI_CFG_ITEM;

/* 写SMBIOS 操作步骤 */
#define SMBIOS_WRITE_PREPARE 0x00
#define SMBIOS_WRITE_DATA 0x01
#define SMBIOS_WRITE_FINISH 0x03
#define SMBIOS_WRITE_NOT_START 0x04


#define BIOS_STARTUP_STATE_OFF 0x00
#define BIOS_STARTUP_ME_EOP 0x20
#define BIOS_STARTUP_PCIE_INFO_REPORTED 0x40
#define BIOS_STARTUP_SMBIOS_WRITE_PREPARE 0x60
#define BIOS_STARTUP_SMBIOS_WRITE_DATA 0x63
#define BIOS_STARTUP_SMBIOS_WRITE_FINISH 0x66
#define BIOS_STARTUP_POST_STAGE_FINISH 0xFE

#define BIOS_REPORT_POST_FINISH_FLAG 0x01

#define BIOS_BOOT_STAGE_OFF 0x00
#define BIOS_BOOT_STAGE_PCIE_INFO_REPORTED 0x01

/* X86产品中ME固件的产品ID */
typedef enum {
    PROD_ID_NO_ID = 0,
    PROD_ID_BROMOLOW = 0x0B01,
    PROD_ID_ROMLEY = 0x0B02,
    PROD_ID_DENLOW = 0x0B03,
    PROD_ID_BRICKLAND = 0x0B04,
    PROD_ID_GRANTLEY = 0x0B05,
    PROD_ID_GRANGEVILLE = 0x0B06,
    PROD_ID_PURLEY = 0x0B0A,
    PROD_ID_WHITLEY = 0x0B0F,
} PROD_ID_TYPE_E;

#define VERIFY_RESULT_SUCCESS 0
#define VERIFY_RESULT_FAILED 1
#define VERIFY_BY_M3 0
#define VERIFY_BY_BOOTGUARD 1


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __BIOS_DEFINE_H__ */
