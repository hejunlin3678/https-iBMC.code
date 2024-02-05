/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : property_method.h
  版 本 号   : 初稿
  作    者   : zhenggenqiang 207985
  生成日期   : 2013年7月13日
  最近修改   :
  功能描述   : PME软件的所有类的属性方法描述文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年7月13日
    作    者   : zhenggenqiang 207985
    修改内容   : 创建文件

******************************************************************************/

#ifndef __PME_PROPERTY_METHOD_H__
#define __PME_PROPERTY_METHOD_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/*Object 类定义*/
#define OBJECT_CLASS_NAME                   "Object"
#define OBJECT_NAME                         "ObjName"
#define OBJECT_CLASS                        "ObjClass"
#define PROPERTY_SEMID                      "SemId"
#define PROPERTY_SEM_MEMBER                 "SemMember"
#define PROPERTY_POSITION                   "Position"

/*Connector 类定义*/
#define CONNECTOR_CLASS_NAME                "Connector"
#define CONNECTOR_ID                        "Id"
#define CONNECTOR_PRODUCT                   "ProductId"
#define CONNECTOR_SLOT                      "Slot"
#define CONNECTOR_PRESENT                   "Present"
#define CONNECTOR_POSITION                  "Position"
#define CONNECTOR_BOM                       "Bom"
#define CONNECTOR_SILKTEXT                  "SilkText"
#define CONNECTOR_LIB                       "Lib"
#define CONNECTOR_BUS                       "Bus"
#define CONNECTOR_CHIP                      "Chip"
#define CONNECTOR_TYPE                      "Type"
#define CONNECTOR_SLOT_INFO                 "SlotInfo"
#define CONNECTOR_GL_INFO                   "GlobalInfo"
#define CONNECTOR_LEGACY                    "Legacy"
#define CONNECTOR_AUXID						"AuxId"

/*Anchor 类定义*/
#define ANCHOR_CLASS_NAME                   "Anchor"
#define ANCHOR_SLOT                         CONNECTOR_SLOT
#define ANCHOR_POSITION                     CONNECTOR_POSITION
#define ANCHOR_SILKTEXT                     CONNECTOR_SILKTEXT
#define ANCHOR_BUS                          CONNECTOR_BUS
#define ANCHOR_SLOT_INFO                    CONNECTOR_SLOT_INFO
#define ANCHOR_GL_INFO                      CONNECTOR_GL_INFO

/*Accessor 类定义*/
// accessor类名
#define ACCESSOR_CLASS_NAME                 "Accessor"
// accessor指向的chip
#define ACCESSOR_PROPERTY_CHIP              "Chip"
// chip的寄存器偏移
#define PROPERTY_REG_INDEX                  "Offset"
// 寄存器掩码
#define PROPERTY_REG_MASK                   "Mask"
// 器件寄存器类型: 单个寄存器(如cpld单个寄存器)、块设备(如EEPROM块读写)
#define PROPERTY_REG_TYPE                  "Type"
// 寄存器字节宽度(1,2,4字节，以字节为单位)
#define PROPERTY_REG_SIZE                   "Size"


/*Chip 类定义*/
#define CHIP_CLASS_NAME                     "Chip"
// 器件的地址属性名称
#define PROPERTY_CHIP_ADDR                  "Addr"
// 器件地址位宽属性名称
#define PROPERTY_ADDR_WIDTH                 "AddrWidth"
// 器件偏移位宽属性名称
#define PROPERTY_OFFSET_WIDTH               "OffsetWidth"
// 器件在拓扑中的级联级数
#define PROPERTY_CHIP_LEVEL                 "Level"
// 器件面向BMC cpu侧的总线类型
#define PROPERTY_LBUS_TYPE                  "LBusType"
// 器件背向BMC cpu侧的总线类型
#define PROPERTY_RBUS_TYPE                  "RBusType"
// 器件的前一级器件对象名称
#define PROPERTY_PREV_CHIP                  "PrevChip"
// 器件的 复位操作器件对象名称
#define PROPERTY_RESET_CHIP                 "ResetChip"
// 器件面向BMC CPU侧的总线对象名称  注意与dflcomm.h BUS_PROPERTY_NAME保持一致
#define PROPERTY_HOST_BUS                   "LBus"
// 器件读超时时间
#define PROPERTY_READ_TIMEOUT               "ReadTmout"
// 器件写超时时间
#define PROPERTY_WRITE_TIMEOUT              "WriteTmout"

/*Bus 类定义*/
#define BUS_CLASS_NAME                      "Bus"
// 总线Id  (第一级总线依据Hi1710分配，和驱动对应， 第二级以后，依据器件决定，和器件驱动对应)
#define PROPERTY_BUS_ID                     "Id"
// 总线类型 (I2C, LOCALBUS, GPIO, PWM, FANTACH ...
#define PROPERTY_BUS_TYPE                   "Type"
// 本地总线忙信号
#define PROPERTY_NODE_BUSY                  "NodeBusy"
// 对端总线忙信号
#define PROPERTY_PEER_BUSY                  "PeerBusy"
// 本端总线忙指示寄存器偏移地址
#define PROPERTY_NODE_BUSY_BYTE             "NodeBusyByte"
// 本端总线忙指示寄存器位掩码
#define PROPERTY_NODE_BUSY_MASK             "NodeBusyMask"
// 对端总线忙指示寄存器偏移地址
#define PROPERTY_PEER_BUSY_BYTE             "PeerBusyByte"
// 对端总线忙指示寄存器位掩码
#define PROPERTY_PEER_BUSY_MASK             "PeerBusyMask"
// 切换开关
#define PROPERTY_I2C_SWITCH                 "Switch"

/*Collection 类定义*/
#define CLASS_COLLECTION_NAME               "Collection"
#define COLLECTION_OBJECT_COUNT             "Count"
#define COLLECTION_OBJECT_LIST              "List"

/*Expresion 类定义*/
#define CLASS_EXPRESSION_NAME               "Expression"
#define PROPERTY_EXPRESSION_VALUE           "Value"
#define PROPERTY_EXPRESSION_X               "X"
#define PROPERTY_EXPRESSION_Y               "Y"
#define PROPERTY_EXPRESSION_Z               "Z"
#define PROPERTY_EXPRESSION_X_CACHE         "Xcache"
#define PROPERTY_EXPRESSION_Y_CACHE         "Ycache"
#define PROPERTY_EXPRESSION_Z_CACHE         "Zcache"
#define PROPERTY_EXPRESSION_FORMULA         "Formula"

#define  BMC_BOARD_NAME                   ("BOARD")
#define  BMC_BOARD_OBJ_NAME               BMC_BOARD_NAME
#define  BMC_LOCATION_ID_NAME             ("LocationId")
#define  BMC_SLOT_ID_NAME                 ("SlotId")
#define  BMC_HW_ADDR_NAME                 ("HardAddress")
#define  BMC_SLOT_ID_SRC_NAME             ("SlotIdSrc")
#define  PROPERTY_BOADE_NAME              ("Name")
#define  PROPERTY_HW_ADDR_FAULT           ("HwAddrFault")
#define  PROPERTY_SLOTID_FAULT            ("SlotIdFault")

#define  BMC_PRODUCT_NAME                 ("PRODUCT")
#define  BMC_PRODUCT_OBJ_NAME             BMC_PRODUCT_NAME
#define  PROPERTY_CHASSIS_NUM             ("ChassisNum")
#define  PROPERTY_SERVER_TYPE             ("ServerType")

/* 调测日志类对象及属性 */
#define CLASS_LOG                           "Log"                   /* PME日志全局类 */
#define OBJECT_LOG                          "log_0"                 /* 日志对象名称 */
#define PROPERTY_LOG_DLOG_DEF_LEVEL         "DlogDefLevel"          /* 调试日志默认级别 */
#define PROPERTY_LOG_DIR                    "LogDir"                /* 日志文件存放路径 */
#define PROPERTY_LOG_DEBUGLOG_FILE          "DebugLogFile"          /* 调试日志文件名称 */
#define PROPERTY_LOG_OPERATELOG_FILE        "OperateLogFile"        /* 操作日志文件名称 */
#define PROPERTY_LOG_STRATEGYLOG_FILE       "StrategyLogFile"       /* 策略日志(运行日志)文件名称 */
#define PROPERTY_LOG_LINUXOPERATELOG_FILE   "LinuxOperateLogFile"   /* Linux操作日志(安全日志/登陆日志)文件名称 */
#define METHOD_OPERATE_LOG_PROXY            "OperateLogProxy"       /* 操作日志记录代理 */

/* 信息收集类对象及属性 */
#define CLASS_DUMP                          "Dump"                  /* Dump全局类 */
#define OBJECT_DUMP                         "dump_0"                /* dump对象 */
#define PROPERTY_DUMP_THIRD_APP_LOG_PATH    "ThirdAppLogPath"       /* 需要收集的第三方程序日志文件路径 */
#define PROPERTY_DUMP_CUSTOM_INFO_PATH      "CustomInfoPath"        /* 用户自定义信息收集路径 */
#define PROPERTY_DUMP_SCRIPT                "DumpScript"            /* 收集脚本 */
#define PROPERTY_DUMP_FILE_PATH             "DumpFilePath"          /* 信息收集路径 */
#define METHOD_DUMP_DUMPINFO                "DumpInfo"              /* 收集方法名称 */
#define METHOD_DUMP_DUMPINFO_ASYNC          "DumpInfoAsync"         /* 异步收集方法名称 */
#define METHOD_DUMP_GET_DUMP_PHASE          "GetDumpPhase"          /* 获取收集阶段 */

/* 脚本引擎类 */
#define CLASS_SCRIPT_ENGINE                 "ScriptEngine"          /* 脚本引擎类 */
#define PROPERTY_SE_SCRIPT_FILE_PATH        "ScriptFilePath"        /* 执行脚本文件路径 */
#define PROPERTY_SE_INIT_FILE_PATH          "InitScriptPath"        /* 初始化脚本文件路径 */


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __PME_PROPERTY_METHOD_H__ */


