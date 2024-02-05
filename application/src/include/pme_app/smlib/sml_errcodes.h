/******************************************************************************

                  版权所有 (C), 2016-2021, 华为技术有限公司

 ******************************************************************************
  文 件 名   : sml_errcodes.h
  版 本 号   : 初稿
  作    者   : huanghan (h00282621)
  生成日期   : 2016年2月26日
  最近修改   :
  功能描述   : 定义SML对外接口的错误码
  函数列表   :
  修改历史   :
  1.日    期   : 2016年2月26日
    作    者   : huanghan (h00282621)
    修改内容   : 创建文件

******************************************************************************/
#ifndef __SML_ERROR_CODE_H__
#define __SML_ERROR_CODE_H__
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */
/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/
#define SML_SUCCESS                        0
#define SML_ERR_CODE_BASE                  0x1000
#define SML_CTRL_ERR_CODE_BASE             (SML_ERR_CODE_BASE + 0x100)
#define SML_LD_ERR_CODE_BASE               (SML_ERR_CODE_BASE + 0x200)
#define SML_PD_ERR_CODE_BASE               (SML_ERR_CODE_BASE + 0x300)
#define SML_CONFIG_ERR_CODE_BASE           (SML_ERR_CODE_BASE + 0x400)
#define SML_ARRAY_ERR_CODE_BASE            (SML_ERR_CODE_BASE + 0x500)
#define SML_EXP_ERR_CODE_BASE              (SML_ERR_CODE_BASE + 0x600)
#define SML_DIAG_ERR_CODE_BASE             (SML_ERR_CODE_BASE + 0x700)

#define SML_ERR_I2C_READ_WRITE_FAILED               (SML_ERR_CODE_BASE + 0x01)  //I2C通信失败 -- add by linzhen l00389091 DTS2017061508091
#define SML_ERR_MCTP_READ_WRITE_FAILED              (SML_ERR_CODE_BASE + 0x02)  // MCTP通信失败
#define SML_ERR_INVALID_PARAMETER                   (SML_ERR_CODE_BASE + 0x03)  // 入参无效，对应MFI_STAT_INVALID_PARAMETER
#define SML_ERR_REBOOT_REQUIRED                     (SML_ERR_CODE_BASE + 0x74)  // 操作已完成 需要重启 对应MFI_STAT_OK_REBOOT_REQUIRED
#define SML_ERR_OPERATION_NOT_POSSIBLE              (SML_ERR_CODE_BASE + 0x76)  // 操作不可能，对应MFI_STAT_OPERATION_NOT_POSSIBLE
#define SML_ERR_INVALID_POLICY                      (SML_ERR_CODE_BASE + 0x97)  // 策略无效 对应MFI_STAT_INVALID_POLICY
#define SML_ERR_CONFIG_PRESENT_ERROR                (SML_ERR_CODE_BASE + 0x98)  // 由于存在配置操作不能执行，对应MFI_STAT_CONFIG_PRESENT_ERROR
#define SML_ERR_NULL_DATA                           (SML_ERR_CODE_BASE + 0xC0)  //传入了空指针
#define SML_ERR_INVALID_CMD                         (SML_ERR_CODE_BASE + 0xC1)
#define SML_ERR_NULL_INFTERFACE                     (SML_ERR_CODE_BASE + 0xC2)  //sml中还没有实现这个接口
#define SML_ERR_CANNOT_ALLOC_MEM                    (SML_ERR_CODE_BASE + 0xC3)  //动态申请内存失败
#define SML_ERR_FILE_PATH_ILLEGAL                   (SML_ERR_CODE_BASE + 0xC4)  //文件路径不合法
#define SML_ERR_CANNOT_OPEN_FILE                    (SML_ERR_CODE_BASE + 0xC5)  //无法打开文件
#define SML_ERR_BUF_NOT_ENOUGH                      (SML_ERR_CODE_BASE + 0xC6)
#define SML_ERR_DATA_LEN_INVALID                    (SML_ERR_CODE_BASE + 0xC7)
#define SML_ERR_DATA_INVALID                        (SML_ERR_CODE_BASE + 0xC8)
#define SML_ERR_EXCEED_LIMIT                        (SML_ERR_CODE_BASE + 0xE0)
#define SML_ERR_SEC_FUNC_FAILED                     (SML_ERR_CODE_BASE + 0xE1)  // 安全函数执行失败

#define SML_ERR_CTRL_INDEX_INVALID                  (SML_CTRL_ERR_CODE_BASE + 0x01) //指定的控制器索引号无效
#define SML_ERR_CTRL_DUPLICATE_REGISTERED           (SML_CTRL_ERR_CODE_BASE + 0x02) //控制器重复注册
#define SML_ERR_CTRL_NOT_REGISTERED                 (SML_CTRL_ERR_CODE_BASE + 0x03) //控制器没有注册
#define SML_ERR_CTRL_INIT_NOT_COMPLETED             (SML_CTRL_ERR_CODE_BASE + 0x04) //控制器初始化未完成
#define SML_ERR_CTRL_INIT_FAILED                    (SML_CTRL_ERR_CODE_BASE + 0x05) //控制器初始化失败
#define SML_ERR_CTRL_TOO_BUSY_TO_RESP_OOB           (SML_CTRL_ERR_CODE_BASE + 0x06) // 控制器处于繁忙状态以致于无法及时响应OOB命令
#define SML_ERR_CTRL_SET_PROP_UPLEVEL_FUNC_DISABLED (SML_CTRL_ERR_CODE_BASE + 0x10) //控制器子功能不能开启，因为上级功能处于关闭状态
#define SML_ERR_CTRL_LSI_SL_LOAD_FAILED             (SML_CTRL_ERR_CODE_BASE + 0x40) //StoreLib库加载失败
#define SML_ERR_CTRL_LSI_SL_GET_FUNC_ADDR_FAILED    (SML_CTRL_ERR_CODE_BASE + 0x41) //StoreLib库获取函数地址失败
#define SML_ERR_CTRL_PMC_SC_LOAD_FAILED             (SML_CTRL_ERR_CODE_BASE + 0x42) // storageCore库加载失败
#define SML_ERR_CTRL_PMC_SC_GET_FUNC_ADDR_FAILED    (SML_CTRL_ERR_CODE_BASE + 0x43) // storageCore库获取函数地址失败
#define SML_ERR_CTRL_PMC_SC_UNLOAD_FAILED           (SML_CTRL_ERR_CODE_BASE + 0x44) // storageCore库卸载失败
#define SML_ERR_CTRL_HISTORE_LOAD_FAILED            (SML_CTRL_ERR_CODE_BASE + 0x42) // historelib库加载失败
#define SML_ERR_CTRL_HISTORE_GET_FUNC_ADDR_FAILED   (SML_CTRL_ERR_CODE_BASE + 0x43) // historelib库获取函数地址失败
#define SML_ERR_CTRL_STATUS_INVALID                 (SML_CTRL_ERR_CODE_BASE + 0xD5) //控制器状态无效
#define SML_ERR_CTRL_PARAM_ILLEGAL                  (SML_CTRL_ERR_CODE_BASE + 0xD6) //控制器参数不合法
#define SML_ERR_CTRL_OPERATION_NOT_SUPPORT          (SML_CTRL_ERR_CODE_BASE + 0xD7) //控制器操作不支持
#define SML_ERR_CTRL_LIB_LOAD_FAILED                (SML_CTRL_ERR_CODE_BASE + 0xD8) //适配层so加载失败
#define SML_ERR_CTRL_NO_FUNC_SYMBOL                 (SML_CTRL_ERR_CODE_BASE + 0xD9) //适配层函数地址获取失败
#define SML_ERR_CTRL_BBU_STATUS_ABNORMAL            (SML_CTRL_ERR_CODE_BASE + 0xDA) // 控制器的BBU状态异常
#define SML_ERR_CTRL_NO_EDITABLE_LD                 (SML_CTRL_ERR_CODE_BASE + 0xDB) // 控制器没有可配置的逻辑盘
#define SML_ERR_CTRL_RCP_NOT_IN_RANGE               (SML_CTRL_ERR_CODE_BASE + 0xDC) // 控制器读缓存百分比不在有效范围

#define SML_ERR_LD_INVALID_TARGET_ID                    (SML_LD_ERR_CODE_BASE + 0x01)   //逻辑盘ID无效
#define SML_ERR_LD_DELETE_ON_SNAPSHOTS_ENABLED          (SML_LD_ERR_CODE_BASE + 0x10)   //要删除的逻辑盘上已经允许快照
#define SML_ERR_LD_INIT_IN_PROGRESS                     (SML_LD_ERR_CODE_BASE + 0x18)   // 逻辑盘正在初始化，对应MFI_STAT_LD_INIT_IN_PROGRESS
#define SML_ERR_LD_PROPERTY_SET_NOT_ALLOWED             (SML_LD_ERR_CODE_BASE + 0x20)   //不允许设置逻辑盘属性值
#define SML_ERR_LD_PROPERTY_SET_ONLY_DEFAULT_ALLOWED    (SML_LD_ERR_CODE_BASE + 0x21)   //逻辑盘属性值只允许设置默认值
#define SML_ERR_LD_NO_SSCD_OR_INVALID_NUM_OF_SSCD       (SML_LD_ERR_CODE_BASE + 0x22)   //没有CacheCade逻辑盘或者CacheCade逻辑盘的个数无效
#define SML_ERR_LD_SET_CACHING_ENABLE_FOR_LD_WITH_SSD   (SML_LD_ERR_CODE_BASE + 0x23)   //在包含SSD物理盘的逻辑盘上设置缓存使能
#define SML_ERR_LD_SIZE_SHRINK_NOT_ALLOWED              (SML_LD_ERR_CODE_BASE + 0x24)   // 不允许收缩逻辑盘的容量（扩展可以）
#define SML_ERR_LD_STATE_UNSUPPORTED_TO_SET             (SML_LD_ERR_CODE_BASE + 0x25)   // 当前逻辑盘状态不支持指定的设置操作
#define SML_ERR_LD_INVALID_POLICY                       (SML_LD_ERR_CODE_BASE + 0x97)   // 策略无效 对应MFI_STAT_INVALID_POLICY
#define SML_ERR_LD_OPERATION_NOT_SUPPORT                (SML_LD_ERR_CODE_BASE + 0xD5)   //不支持的逻辑盘操作

#define SML_ERR_PD_INVALID_DEVICE_ID                (SML_PD_ERR_CODE_BASE + 0x01)   //物理盘ID无效
#define SML_ERR_PD_MAKESPARE_NOT_ALLOWED            (SML_PD_ERR_CODE_BASE + 0x10)   //物理盘不允许设置热备
#define SML_ERR_PD_SPARE_SDD_SAS_SATA_MIXED         (SML_PD_ERR_CODE_BASE + 0x11)   //作为热备的物理盘与逻辑盘成员盘同为SSD,但是接口不同
#define SML_ERR_PD_SPARE_SDD_HDD_MIXED              (SML_PD_ERR_CODE_BASE + 0x12)   //作为热备的物理盘与逻辑盘成员盘为不同介质的硬盘(SSD/HDD)
#define SML_ERR_PD_SPARE_TOO_MANY_ARRAYS            (SML_PD_ERR_CODE_BASE + 0x13)   //物理盘已经设置了太多需要作为热备的Array
#define SML_ERR_PD_SPARE_FOR_RAID0_LD               (SML_PD_ERR_CODE_BASE + 0x14)   //试图将物理盘设置为RAID0逻辑盘的热备盘
#define SML_ERR_PD_STATE_UNSUPPORTED_TO_SET         (SML_PD_ERR_CODE_BASE + 0x1f)   //当前物理盘状态不支持指定的设置操作
#define SML_ERR_PD_MAX_UNCONFIGURED                 (SML_PD_ERR_CODE_BASE + 0x5f)   // 存在允许的最大未配置PD
#define SML_ERR_PD_SCSI_RESP_INCORRECT              (SML_PD_ERR_CODE_BASE + 0x80)   //物理盘SCSI命令响应头信息不正确
#define SML_ERR_PD_SCSI_RESP_NO_DATA                (SML_PD_ERR_CODE_BASE + 0x81)   //物理盘SCSI命令响应没有数据
#define SML_ERR_PD_SCSI_RESP_TRUNCATED              (SML_PD_ERR_CODE_BASE + 0x82)   //物理盘SCSI命令响应数据太长，被截断处理
#define SML_ERR_PD_SCSI_STATUS_BUSY                 (SML_PD_ERR_CODE_BASE + 0xC0)   //物理盘SCSI状态忙
#define SML_ERR_PD_SCSI_TIMEOUT                     (SML_PD_ERR_CODE_BASE + 0xC3)   //物理盘SCSI命令超时
#define SML_ERR_PD_SCSI_DEVICE_FAULT                (SML_PD_ERR_CODE_BASE + 0xC4)   // 物理盘SCSI命令校验Device故障
#define SML_ERR_PD_SCSI_STATUS_FAIL                 (SML_PD_ERR_CODE_BASE + 0xC5)   // 物理盘SCSI命令校验Status失败
#define SML_ERR_PD_SCSI_CMD_FAIL                    (SML_PD_ERR_CODE_BASE + 0xD4)   //物理盘SCSI命令失败
#define SML_ERR_PD_OPERATION_NOT_SUPPORT            (SML_PD_ERR_CODE_BASE + 0xD5)   //物理盘操作不支持
#define SML_ERR_PD_LOG_PAGE_UNSUPPORT               (SML_PD_ERR_CODE_BASE + 0xE0)   //物理盘LOG PAGE不支持
#define SML_ERR_PD_IS_SMART_TEST                    (SML_PD_ERR_CODE_BASE + 0xF0)   // 物理盘正在自检状态

#define SML_ERR_CONFIG_INCORRECT_CONFIG_DATA_SIZE           (SML_CONFIG_ERR_CODE_BASE + 0x00)   //RAID配置数据大小不正确
#define SML_ERR_CONFIG_RESOURCE_CONFLICT                    (SML_CONFIG_ERR_CODE_BASE + 0x0B)   // 某些配置资源相互冲突或与当前配置冲突
#define SML_ERR_CONFIG_INVALID_PARAM_SPAN_DEPTH             (SML_CONFIG_ERR_CODE_BASE + 0x10)   //创建逻辑盘的span depth参数无效
#define SML_ERR_CONFIG_INVALID_PARAM_NUM_DRIVE_PER_SPAN     (SML_CONFIG_ERR_CODE_BASE + 0x11)   //创建逻辑盘的num drive per span参数无效
#define SML_ERR_CONFIG_INVALID_PARAM_REPEATED_PD_ID         (SML_CONFIG_ERR_CODE_BASE + 0x12)   //创建逻辑盘的物理盘列表中有重复ID
#define SML_ERR_CONFIG_INVALID_PARAM_RAID_LEVEL             (SML_CONFIG_ERR_CODE_BASE + 0x13)   //创建逻辑盘的RAID级别参数无效
#define SML_ERR_CONFIG_INVALID_PARAM_ARRAY_REF              (SML_CONFIG_ERR_CODE_BASE + 0x14)   //创建逻辑盘的Array ID参数无效
#define SML_ERR_CONFIG_INVALID_PARAM_CAPACITY_TOO_SMALL     (SML_CONFIG_ERR_CODE_BASE + 0x15)   //创建逻辑盘指定的容量空间太小
#define SML_ERR_CONFIG_INVALID_PARAM_CAPACITY_TOO_LARGE     (SML_CONFIG_ERR_CODE_BASE + 0x16)   //创建逻辑盘指定的容量空间太大(大于Array上可用的最大空间)
#define SML_ERR_CONFIG_ARRAY_NUM_REACHED_LIMIT              (SML_CONFIG_ERR_CODE_BASE + 0x30)   //Array个数已经到达最大值
#define SML_ERR_CONFIG_ARRAY_SIZE_TOO_SMALL                 (SML_CONFIG_ERR_CODE_BASE + 0x31)   //指定用于创建逻辑盘的Array上空间太小
#define SML_ERR_CONFIG_ARRAY_NO_AVAILABLE_SPACE             (SML_CONFIG_ERR_CODE_BASE + 0x32)   //指定用于创建逻辑盘的Array上没有可用空间
#define SML_ERR_CONFIG_ARRAY_INCORRECT_DATA_SIZE            (SML_CONFIG_ERR_CODE_BASE + 0x33)   //用于创建逻辑盘的Array个数和span个数不相等
#define SML_ERR_CONFIG_INVALID_PD_GETINFO_FAILED            (SML_CONFIG_ERR_CODE_BASE + 0x40)   //获取用于创建逻辑盘的物理盘信息失败
#define SML_ERR_CONFIG_INVALID_PD_BOOT_SECTOR_FOUND         (SML_CONFIG_ERR_CODE_BASE + 0x41)   //在用于创建逻辑盘的物理盘上有启动扇区存在
#define SML_ERR_CONFIG_INVALID_PD_NON_SUPPORTED             (SML_CONFIG_ERR_CODE_BASE + 0x42)   //用于创建逻辑盘的物理盘ID指向的是不支持的物理设备
#define SML_ERR_CONFIG_INVALID_PD_SCSI_DEV_TYPE             (SML_CONFIG_ERR_CODE_BASE + 0x43)   //用于创建逻辑盘的物理盘ID指向的是不支持的SCSI设备
#define SML_ERR_CONFIG_INVALID_PD_IN_USE                    (SML_CONFIG_ERR_CODE_BASE + 0x44)   //用于创建逻辑盘的物理盘已经被使用
#define SML_ERR_CONFIG_INVALID_PD_SDD_HDD_MIXED             (SML_CONFIG_ERR_CODE_BASE + 0x45)   //用于创建逻辑盘的物理盘中同时有SSD和HDD，而控制器不支持混接在一个逻辑盘
#define SML_ERR_CONFIG_INVALID_PD_SDD_SAS_SATA_MIXED        (SML_CONFIG_ERR_CODE_BASE + 0x46)   //用于创建逻辑盘的SSD物理盘中同时有SAS和SATA接口，而控制器不支持混接在一个逻辑盘
#define SML_ERR_CONFIG_INVALID_PD_WITH_FOREIGN_CONFIG       (SML_CONFIG_ERR_CODE_BASE + 0x47)   //用于创建逻辑盘的物理盘有外部配置信息
#define SML_ERR_CONFIG_INVALID_PD_NON_SDD_FOR_CACHECADE     (SML_CONFIG_ERR_CODE_BASE + 0x48)   //用于创建CacheCade逻辑盘的物理盘不是SSD硬盘
#define SML_ERR_CONFIG_INVALID_PD_OTHER_ERROR               (SML_CONFIG_ERR_CODE_BASE + 0x4F)   //用于创建逻辑盘的物理盘有其他不适于创建的错误
#define SML_ERR_CONFIG_TARGET_LD_ID_EXHAUSTED               (SML_CONFIG_ERR_CODE_BASE + 0x50)   //无法分配ID给新建逻辑盘，因为ID已经分配完
#define SML_ERR_CONFIG_BLOCK_SIZE_NOT_SAME                  (SML_CONFIG_ERR_CODE_BASE + 0x51)   //用于创建逻辑盘的物理盘的block size不一致
#define SML_ERR_CONFIG_OPERATION_NOT_SUPPORT                (SML_CONFIG_ERR_CODE_BASE + 0xD5)   //无法支持的RAID配置操作
#define SML_ERR_CONFIG_NO_FOREIGN_CONFIG                    (SML_CONFIG_ERR_CODE_BASE + 0xD6)   // 没有外部配置
#define SML_ERR_CONFIG_INCOMPLETE_FOREIGN_CONFIG            (SML_CONFIG_ERR_CODE_BASE + 0xD7)   // 不完整的外部配置
#define SML_ERR_CONFIG_ASSOCIATED_LD_SIZE_OUT_OF_RANGE      (SML_CONFIG_ERR_CODE_BASE + 0xD8)   // 被关联的普通逻辑盘容量超范围

#define SML_ERR_ARRAY_INVALID_ARRAY_REF                     (SML_ARRAY_ERR_CODE_BASE + 0x01)    //无效的Array ID

#define SML_ERR_EXP_NO_EXPANDER                             (SML_EXP_ERR_CODE_BASE + 0x01)      // 获取Expander数量为0
#define SML_ERR_EXP_NO_PHY                                  (SML_EXP_ERR_CODE_BASE + 0x02)      // 获取Expander的PHY数量为0
#define SML_ERR_EXP_NO_UPPER_EXP                            (SML_EXP_ERR_CODE_BASE + 0x03)      // Expander上行PHY连接的设备类型不为Expander
#define SML_ERR_EXP_UPPER_EXP_NOT_FOUND                     (SML_EXP_ERR_CODE_BASE + 0x04)      // 未找到Expander上行PHY连接的Expander

#define SML_ERR_DIAG_TOPO_CAN_NOT_FIND_PD                   (SML_DIAG_ERR_CODE_BASE + 0x01)     // 遍历拓扑结构中找不到物理盘
#define SML_ERR_DIAG_LACK_PHY_DIAG_DATA                     (SML_DIAG_ERR_CODE_BASE + 0x02)     // PHY误码历史数据不满足诊断
#define SML_ERR_DIAG_DATA_OVER_TIME                         (SML_DIAG_ERR_CODE_BASE + 0x03)     // 诊断数据超时失效
#define SML_ERR_DIAG_TOPO_NOT_MATCH                         (SML_DIAG_ERR_CODE_BASE + 0x04)     // PHY误码诊断时拓扑数据不匹配
#define SML_ERR_DIAG_MOCK_FILE_CANNOT_ACCESS                (SML_DIAG_ERR_CODE_BASE + 0x05)     // 模拟文件不能访问
#define SML_ERR_DIAG_MOCK_FILE_FORMAT_ERR                   (SML_DIAG_ERR_CODE_BASE + 0x06)     // 模拟文件格式错误
#define SML_ERR_DIAG_MOCK_NOT_SUPPORT_CODE                  (SML_DIAG_ERR_CODE_BASE + 0x07)     // 不支持的模拟事件码

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif

