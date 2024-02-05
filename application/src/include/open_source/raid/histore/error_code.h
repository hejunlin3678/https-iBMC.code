/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: 定义对外错误码，暂定2个字节，各功能块占据一个错误码区间，以便根据错误码能大概界定错误范围
 * Create: 2021-10-26
 */
#ifndef INCLUDE_OPEN_API_ERROR_CODE_H
#define INCLUDE_OPEN_API_ERROR_CODE_H

enum tool_error_code {
    // 工具逻辑信息错误码，由于卡侧返回值最大16位，故工具错误从其上开始定义，并使用负值
    RESULT_INNER_PARAM_NULL         = -0x10001,  // 工具内部函数参数为空
    RESULT_CMD_UNSUPPORT            = -0x10003,  // 主或子命令不支持
    RESULT_PARAM_MISSING            = -0x10004,  // 缺少必要参数
    RESULT_PARAM_MISMATCH           = -0x10005,  // 参数不匹配
    RESULT_DRIVER_NONACCESS         = -0x10007,  // 驱动无法访问
    RESULT_OPEN_FAIL                = -0x10008,  // 文件或设备打开失败
    RESULT_PARAM_OUTRANGE           = -0x10009,  // 参数超出范围
    RESULT_CANCEL_OPERATE           = -0x1000A,  // 用户取消操作
    RESULT_RG_PD_ID_ERROR           = -0x1000C,  // 创vd的时候，RG 和 PD 只能输入其一
    RESULT_VD_NUM_ERROR             = -0x1000D,  // 创vd的时候，物理盘数量有误
    RESULT_FILE_DIR_EXIST           = -0x1000E,  // 文件或目录已存在
    RESULT_INDEX_ERROR              = -0x10010,  // 日志index文件有误
    RESULT_SYSTEM_ERROR             = -0x10013,  // 系统内部错误
    RESULT_LOG_INVALID_ERROR        = -0x10014,  // 无效的固件日志文件
    RESULT_NO_PARAM                 = -0x10015,  // 未输入任何参数
    RESULT_PARAM_TOO_LONG           = -0x10016,  // 用户输入参数过长
    RESULT_CTRL_NOT_FOUND           = -0x10017,  // 控制卡未找到
    RESULT_FILE_SIZE_ERROR          = -0x10018,  // 文件大小为0或太大
    RESULT_PARAM_UNSUPPORT          = -0x10019,  // 在某些条件下不支持输入某参数
    RESULT_FILE_SIZE_SMALL          = -0x10020,  // 文件太小
    // 超时错误码
    RESULT_DRIVER_OVER_TIME         = 0xFFFF,    // 驱动返回超时
};

// spraiaadm 管理命令错误码 （厂商自定义区域 700-7ff）
enum external_error_code {
    // 提取nvme公版命令，对我们可能有用
    SC_SUCCESS                      = 0,
    SC_INVALID_OPCODE               = 0x1,      // 不支持的opcode

    SC_SANITIZE_FAILED              = 0x1C,     // sanitize失败
    SC_SANITIZE_IN_PROGRESS         = 0x1D,     // 正在擦除
    SC_LBA_RANGE                    = 0x80,     // 超过块范围
    SC_CAP_EXCEEDED                 = 0x81,     // 命令导致命名空间容量超过
    SC_NS_NOT_READY                 = 0x82,     // 未准备好
    SC_RESERVATION_CONFLICT         = 0x83,     // 与保留空间冲突
    SC_FORMAT_IN_PROGRESS           = 0x84,     // 正在格式化
    SC_FIRMWARE_IMAGE               = 0x107,    // 升级固件无效
    SC_FW_NEEDS_CONV_RESET          = 0x10b,    // 固件激活需要常规重置
    SC_SELT_TEST_IN_PROGRESS        = 0x11d,    // 设备正在自检
    SC_BP_WRITE_PROHIBITED          = 0x11e,    // 启动分区禁止写入

    // 管理命令通用错误 0x700-0x71f 32条
    SC_SUBOPCODE_INVALID            = 0x700,     // 不支持的命令（操作码）
    SC_PARAM_INVALID                = 0x701,     // 无效的命令参数（通常工具版本和FW不配套时会产生）
    SC_OPERATE_UNSUPPORT            = 0x702,     // 不支持的操作（将SMR盘设置为free盘、成员盘设置设置直通等）
    SC_SYSTEM_ABNORMAL              = 0x703,     // 系统异常（内存申请、拷贝失败等）
    SC_SYSTEM_BUSY                  = 0x704,     // 系统繁忙（正在停巡检过程中启动巡检、固件传输过程中不支持再次下发传输命令等）
    SC_DEV_BUSY                     = 0x705,     // 设备未就绪或正在统计，稍后再试（如备电自学习等）
    SC_FEATURE_HAS_BEEN_ENABLED     = 0x706,     // 功能已经启动/打开（如巡检、擦除等）
    SC_FEATURE_HAS_BEEN_DISABLED    = 0x707,     // 功能已经停止/关闭
    SC_INCORRECT_FORMAT             = 0x708,     // 格式错误
    SC_INVALID_ADDRESS              = 0x709,     // 地址非法
    SC_CID_NOT_MATCH                = 0x70a,     // 应答消息cid与请求消息中cid不匹配
    SC_OOB_EVENT_NOT_REGISTER       = 0x70b,     // 带外关注事件未注册
    SC_OOB_EVENT_QUEUE_FULL         = 0x70c,     // 事件队列已满
    SC_OOB_EVENT_TYPE_INVALID       = 0x70d,     // 事件类型非法
    SC_OOB_EVENT_LEVEL_INVALID      = 0x70e,     // 事件级别非法
    SC_BOARD_ID_NOT_SUPPORT         = 0x70f,     // 获取单板型号名称使用的board id不支持
    SC_CTRLER_ID_INVALID            = 0x710,     // 控制器ID(cx)无效
    SC_EXP_ID_INVALID               = 0x711,     // EXPANDER号无效
    SC_ENC_ID_INVALID               = 0x712,     // 框位号无效
    SC_SLOT_ID_INVALID              = 0x713,     // 槽位号无效
    SC_PHY_ID_INVALID               = 0x714,     // phyID无效
    SC_RG_ID_INVALID                = 0x715,     // raid组ID无效
    SC_VD_ID_INVALID                = 0x716,     // 逻辑盘ID无效
    SC_DISK_ID_INVALID              = 0x717,     // Disk ID无效
    SC_LIB_VER_NOT_MATCH_FW_VER     = 0x718,     // lib库版本与固件版本不匹配
    SC_CMD_TIMEOUT                  = 0x719,     // 管理命令执行超时
    SC_SYSTEM_UNINITIALIZED         = 0x71A,     // 系统未初始化
    SC_MALLOC_FAILED                = 0x71B,     // 申请内存失败

    // 前端及SS错误码 0x720-0x73f 32条
    SC_SS_ACTIVE_NOT_ALLOWED         = 0x720,     // 当前不支持激活
    SC_SS_DEV_NOT_BOOT               = 0x721,     // 设备非BOOT
    SC_SS_NOT_EXIST_MATCH_PF         = 0x722,     // 不存在匹配的PF类型
    SC_SS_UPDATE_INTERRUPTED         = 0x723,     // 升级传包过程中被新的升级传包命令打断
    SC_SS_UPDATE_BUSY                = 0x724,     // 系统正在执行热激活，命令处理繁忙

    SC_FE_REPORT_TYPE_NOT_MATCH_DISK = 0x730,     // 设备上报类型与磁盘类型不匹配
    SC_FE_SELECT_PF_NOT_MATCH_TYPE   = 0x731,     // 上报的PF类型与选择的类型不匹配
    SC_FE_DEV_EXIST                  = 0x732,     // 设备已存在(或上报)
    SC_FE_CMD_INVALID                = 0x733,     // 命令输入异常（如写类型的命令工具未传输数据长度）

    // cache错误码 0x740-0x75f 32条
    SC_CACHE_TSP_RETURN_ERROR       = 0x740,     // CACHE_TSP返回错误，暂不明确
    SC_CACHE_RETURN_ERROR           = 0x741,     // CACHE返回错误，暂不明确
    SC_CACHE_FLUSH_TIMEOUT          = 0x742,     // CACHE下刷超时
    SC_CACHE_PINNED                 = 0x743,     // CACHE下刷失败产生pinnedcache
    SC_CACHE_TSP_FLUSH_FAIL         = 0x744,     // CACHE_TSP下刷失败
    SC_CACHE_TSP_FLUSH_TIMEOUT      = 0x745,     // CACHE_TSP下刷超时
    SC_CACHE_TSP_EVICT_RB_FAIL      = 0x746,     // CACHE_TSP淘汰读buffer失败
    SC_CACHE_TSP_CLEAR_WB_FAIL      = 0x747,     // CACHE_TSP清空写buffer失败
    SC_CACHE_DELETE_HW_FAIL         = 0x748,     // CACHE删硬化lun失败
    SC_CACHE_DELETE_SW_FAIL         = 0x749,     // CACHE删软化lun失败
    SC_CACHE_CREATE_HW_FAIL         = 0x74a,     // CACHE创硬化lun失败
    SC_CACHE_GET_W_TYPE_FAIL        = 0x74b,     // CACHE获取写策略失败
    SC_CACHE_SET_W_TYPE_FAIL        = 0x74c,     // CACHE设置写策略失败
    SC_CACHE_GET_R_TYPE_FAIL        = 0x74d,     // CACHE获取读策略失败
    SC_CACHE_SET_R_TYPE_FAIL        = 0x74e,     // CACHE设置读策略失败
    SC_CACHE_TSP_SET_W_TYPE_FAIL    = 0x74f,     // CACHE_TSP设置写策略失败
    SC_CACHE_TSP_SET_R_TYPE_FAIL    = 0x750,     // CACHE_TSP设置读策略失败
    SC_CACHE_ENQUEUE_REQ_FAIL       = 0x751,     // CACHE_REQ入队失败
    SC_CACHE_EXIST_PINNED_ERROR     = 0x752,     // 卡上存在pinnedcache导致命令失败
    SC_CACHE_VD_PINNED_NOT_EXIST    = 0x753,     // 逻辑盘上不存在pinnedcache导致命令失败
    SC_CACHE_VD_EXTEND_FAIL         = 0x754,     // 扩展逻辑盘失败
    SC_CACHE_VD_CLEAR_FAIL          = 0x755,     // 清空逻辑盘失败

    // 后端错误码 0x780-0x79f 32条
    SC_BDE_PHY_BUSY                = 0x780,        // phy状态忙
    SC_BDE_DEV_UNEXIST             = 0x781,        // 盘不存在
    SC_BDE_SMART_POLLING_TIME      = 0x782,        // 非法例测时间
    SC_BDE_OP_FAIL                 = 0x783,        // 命令失败(后端)
    SC_BDE_DISK_STATUS_INVALID     = 0x784,        // 硬盘状态无效
    SC_BDE_LED_TYPE_INVALID        = 0x785,        // 点灯类型无效
    SC_BDE_TYPE_INVALID            = 0x786,        // 类型无效
    SC_BDE_GET_DISK_LIFE_FAIL      = 0x787,        // 获取盘寿命操作失败
    SC_BDE_GET_WRITE_CACHE_FAIL    = 0x788,        // 获取WRITE CACHE操作失败
    SC_BDE_SET_WRITE_CACHE_FAIL    = 0x789,        // 设置WRITE CACHE操作失败
    SC_BDE_SEND_SANITIZE_FAIL      = 0x78a,        // 发送SANITIZE失败
    SC_BDE_SET_QUEUE_DEPTH_FAIL    = 0x78b,        // 设置队列深度失败
    SC_BDE_GET_IO_STAT_FAIL        = 0x78c,        // 获取IO状态失败
    SC_BDE_GET_SMART_INFO_FAIL     = 0x78d,        // 获取smart info失败
    SC_BDE_SET_NCQ_FAIL            = 0x78e,        // 设置ncq失败
    SC_BDE_STARTSTOP_FAIL          = 0x78f,        // 休眠唤醒失败
    SC_BDE_GET_ERASE_PROGRESS_FAIL = 0x790,        // 获取安全擦除进度失败
    SC_BDE_STOP_ERASE_FAIL         = 0x791,        // 停止安全擦除失败
    SC_BDE_START_ERASE_FAIL        = 0x792,        // 启动安全擦除失败
    SC_BDE_DISK_SLEEP              = 0x793,        // 磁盘处于休眠状态
    SC_BDE_DISK_ACTIVE_FAIL        = 0x794,        // 磁盘激活失败
    SC_BDE_SET_DISK_PT_FAIL        = 0x795,        // 设置磁盘直通模式失败
    SC_BDE_SANITIZE_CMD_INVALID    = 0x796,        // 发送SANITIZE命令非法
    SC_BDE_DISK_NOT_IN_ERASE       = 0x797,        // 盘不在擦除状态
    SC_BDE_DISK_NOT_IN_ERASE_LAST_FAIL     = 0x798,        // 盘不在擦除状态,且上一次超时
    SC_BDE_DISK_IN_ERASE           = 0x799,        // 盘在擦除状态
    SC_BDE_DISK_NOT_SUPPORT_WRITE_CACHE    = 0x79a,        // 盘不支持cache
    SC_BDE_SD_LEVEL_1_THRESHOLD_INVALID    = 0x79b,        // 设置L1门限参数非法
    SC_BDE_SD_LEVEL_2_THRESHOLD_INVALID    = 0x79c,        // 设置L2门限参数非法
    SC_BDE_SD_LEVEL_3_THRESHOLD_INVALID    = 0x79d,        // 设置L3门限参数非法
    SC_BDE_SD_RATIO_INVALID        = 0x79e,        // 设置ratio参数非法
    SC_BDE_DISK_TX_HOLD_TIMEOUT    = 0x79f,        // BI设备盘TX HOLD超时
    // Raid错误码 0x7a0-0x7ff(96)、0x2c0-0x2ff(64) 160条
    /* Raid 模块公共错误码 */
    SC_RAID_RETURN_ERROR                   = 0x7a0, // 返回RETURN_ERROR,修改不明确,暂时替换
    SC_RAID_WAKE_UP_DISK_FAILED            = 0x7a1, // 磁盘唤醒失败
    SC_RAID_DISK_IN_ERASE_CFG              = 0x7a2,

    /* CFG操作 */
    SC_RAID_NO_FOREIGN_CONFIG              = 0x7aa, // 没有外来配置
    SC_RAID_CONFIG_CONFLICT                = 0x7ab, // 配置冲突
    SC_RAID_DYNAMIC_CONFIG_OVERFLOW        = 0x7ac, // 动态配置冲突
    SC_RAID_CONFIG_INCOMPLETE              = 0x7ad, // 配置不全
    SC_RAID_CONFIG_ERROR                   = 0x7ae, // 配置错误
    SC_RAID_CONFIG_NO_LUN                  = 0x7af, // 外来配置中没有lun

    /* 盘操作 */
    SC_RAID_LOCAL_HSP_TOO_SMALL_CAPACITY   = 0x7b0, // 热备盘容量小于成员盘最小容量
    SC_RAID_LOCAL_HSP_RG_NOT_SUPPORT       = 0x7b1, // 该RAID组不支持局部热备
    SC_RAID_LOCAL_HSP_MEDIA_MISMATCH       = 0x7b2, // 局部热备与RAID组介质不匹配
    SC_RAID_LOCAL_HSP_BLOCK_SIZE_MISMATCH  = 0x7b3, // 局部热备与成员盘块大小不匹配
    SC_RAID_LOCAL_HSP_INCOMPATIBE          = 0x7b4, // 局部热备与RAID组不匹配
    SC_RAID_LOCAL_HSP_REPEATE_SET          = 0x7b5, // 局部热备重复设置
    SC_RAID_LOCAL_HSP_NUM_EXCEED           = 0x7b6, // 局部热备设置数目超上限
    SC_RAID_HSP_IS_HALF_LIFE               = 0x7b7, // 盘半条命,不能设置为热备盘
    SC_RAID_HSP_IS_FAULT                   = 0x7b8, // 盘已失效,不能设置为热备盘
    SC_RAID_HSP_SET_INVALID                = 0x7b9, // 未选择空闲盘设置设置热备,或未选择热备盘设置设置空闲
    SC_RAID_DISK_ONLINE_INVALID            = 0x7ba, // 不支持上线的盘
    SC_RAID_GLOBAL_HSP_NUM_EXCEED          = 0x7bb, // 全局热备设置数目超上限

    /* 全局开关 */
    SC_RAID_COPYBACK_SW_INVALID            = 0x7be, // 设置回拷开关不是开或关
    SC_RAID_PRECOPY_SW_INVALID             = 0x7bf, // 设置预拷贝开关不是开或关
    SC_RAID_EHSP_SW_INVALID                = 0x7c0, // 设置全局热备开关不是开或关
    SC_RAID_POWERSAVE_SW_INVALID           = 0x7c1, // 设置全局节能开关不是开或关
    SC_RAID_POWERSAVE_DELAY_INVALID        = 0x7c2, // 设置定时节能时间超阈值

    /* RAID卡配置信息 */
    SC_RAID_WRITE_EQUIP_FAILED             = 0x7c8, // 配置写入装备失败
    SC_RAID_RECOVER_CFG_PARTIAL_FAIL       = 0x7c9, // 恢复配置写信息失败

    /* BST */
    SC_RAID_BST_SEARCH_LBA_EXCEED          = 0x7d2, // 单盘BST查询lba超过磁盘容量

    /* 后台任务进度/速率 */
    SC_RAID_BG_TYPE_INVALID                = 0x7dc, // 后台任务类型输入为非法值
    SC_RAID_BG_RATE_INVALID                = 0x7dd, // 后台任务速率输入为非法值

    /* 巡检 */
    SC_RAID_PR_SINGLE_START_FAIL_UNSUP     = 0x7e5, // 单盘巡检启动失败或不支持
    SC_RAID_PATROLREAD_PERIOD_INVALID      = 0x7e6, // 磁盘巡检周期为非法值
    SC_RAID_PATROLREAD_RATE_INVALID        = 0x7e7, // 磁盘巡检速率为非法值
    SC_RAID_PATROLREAD_DISKNUM_INVALID     = 0x7e8, // 磁盘巡检磁盘并发为非法值
    SC_RAID_PATROLREAD_DISK_UNSUPPORT      = 0x7e9, // 单盘巡检进度查询失败,因为该盘类型或状态不支持

    /* RAID组管理 */
    SC_RAID_RG_LEVEL_NOT_MATCH             = 0x7f0, // 创建RAID组失败,因为级别不匹配
    SC_RAID_RG_MEMBER_DISK_NUM_INVALID     = 0x7f1, // 创建RAID组失败,因为成员盘数量非法
    SC_RAID_RG_ID_USE_UP                   = 0x7f2, // 创建RAID组失败,因为RAID组数目超上限
    SC_RAID_RG_NAME_REPEAT                 = 0x7f3, // 创建RAID组失败,因为RAID组名字重复
    SC_RAID_RG_CACHE_SW_INVALID            = 0x7f4, // 创建RAID组失败,因为cache开关非法
    SC_RAID_RG_PHYSICS_TYPE_INVALID        = 0x7f5, // 创建RAID组失败,因为成员盘物理类型不兼容
    SC_RAID_RG_MEMBER_DISK_REPEAT          = 0x7f6, // 创建RAID组失败,因为成员盘重复选择
    SC_RAID_RG_DISK_NOT_BELONG             = 0x7f7, // 创建RAID组失败,因为盘不属于该RAID组
    SC_RAID_RG_NAME_LENGTH_EXCEED          = 0x7f8, // 创建RAID组失败,因为RAID组名字太长
    SC_RAID_RG_MEMBER_DISK_STATUS_INVALID  = 0x7f9, // 创建RAID组失败,因为成员盘状态非法
    SC_RAID_RG_PDCACHE_CONFIG_FAIL         = 0x7fa, // RAID组pdcache配置失败，盘返回错误或不支持配置
    SC_RAID_RG_DISK_CAPACITY_EXCEEDS_MAX   = 0x7fb, // 成员盘容量超过最大值，不支持

    /* LUN管理(预留32条) */
    SC_RAID_LUN_PF_NOT_MATCH               = 0x2c0, // 创建LUN失败,因为PF类型和上报类型不一致
    SC_RAID_LUN_WRITE_CACHE_NOT_MATCH      = 0x2c1, // 创建LUN失败,因为LUN cache类型和RAID组不匹配
    SC_RAID_LUN_RG_LEVEL_NOT_MATCH         = 0x2c2, // 创建LUN失败,因为LUN 级别和RAID组不匹配
    SC_RAID_LUN_TYPE_NOT_MATCH             = 0x2c3, // 创建LUN失败,因为LUN物理类型和RAID组不匹配
    SC_RAID_LUN_DIRTY_DATA_EXIST           = 0x2c4, // 创建LUN失败,因为失效LUN脏数据未清除
    SC_RAID_LUN_NUM_PER_RG_EXCEED          = 0x2c5, // 创建LUN失败,因为单RAID组LUN数量超上限
    SC_RAID_LUN_DYNAMIC_IS_RUNNING         = 0x2c6, // 创建LUN失败,因为扩盘任务存在
    SC_RAID_LUN_DISK_STATUS_INVALID        = 0x2c7, // 创建LUN失败,因为成员盘状态非法
    SC_RAID_LUN_SIZE_ZERO                  = 0x2c8, // 创建LUN失败,因为创LUN容量为0
    SC_RAID_LUN_TYPE_INVALID               = 0x2c9, // 创建LUN失败,因为物理类型(ST/NT)非法
    SC_RAID_LUN_CACHE_WRITE_MODE_INVALID   = 0x2ca, // 创建LUN失败,因为cache类型非法
    SC_RAID_LUN_READ_AHEAD_MODE_INVALID    = 0x2cb, // 创建LUN失败,因为预读类型非法
    SC_RAID_LUN_SU_SIZE_INVALID            = 0x2cd, // 创建LUN失败,因为SU SIZE非法
    SC_RAID_LUN_STRIPE_SIZE_INVALID        = 0x2ce, // 创建LUN失败,因为STRIPE SIZE非法
    SC_RAID_LUN_COLD_HOT_INVALID           = 0x2cf, // 创建LUN失败,因为冷热类型非法
    SC_RAID_LUN_PF_TYPE_INVALID            = 0x2d0, // 创建LUN失败,因为PF类型非法
    SC_RAID_LUN_NAME_REPEAT                = 0x2d1, // 创建LUN失败,因为LUN名字重复（该错误码已弃用）
    SC_RAID_LUN_NAME_LENGTH_EXCEED         = 0x2d2, // 创建LUN失败,因为LUN名字太长
    SC_RAID_LUN_NO_CAPACITY                = 0x2d3, // 创建LUN失败,因为RAID组容量不够
    SC_RAID_LUN_CRATE_MODE_INVALID         = 0x2d4, // 创建LUN失败,因为创LUN方式错误
    SC_RAID_LUN_ID_USE_UP                  = 0x2d5, // 创建LUN失败,因为LUN数目超上限
    SC_RAID_LUN_SIZE_EXCEEDS_MAX           = 0x2d6, // 创建LUN失败,因为创LUN容量超过上限
    SC_RAID_LUN_EXPAND_EXCEEDED_LIMIT      = 0x2d7, // 扩展lun次数超过限制

    /* 分条校验 */
    SC_RAID_CCHECK_PERIOD_INVALID          = 0x2e0, // 一致性校验周期为非法值
    SC_RAID_CCHECK_RATE_INVALID            = 0x2e1, // 一致性校验速率为非法值
    SC_RAID_CCHECK_REPAIRE_MODE_INVALID    = 0x2e2, // 一致性校验修复模式为非法值
    SC_RAID_CCHECK_IS_ABNORMAL             = 0x2e3, // 一致性校验被打断或未执行,不支持查询一致性记录
    SC_RAID_CCHECK_DELAY_INVALID           = 0x2e4, // 一致性校验延迟启动时间为非法值

    /* 慢盘踢盘周期 */
    SC_RAID_KICK_PERIOD_INVALID            = 0x2e6, // 慢盘踢盘清除周期有误
    SC_RAID_KICK_SUM_INVALID               = 0x2e7, // 慢盘踢盘总数有误
    SC_RAID_KICK_FRESH_INVALID             = 0x2e8, // 慢盘重新检测周期有误【已弃用】
    SC_RAID_KICK_FRESH_BIGGER              = 0x2e9, // 刷新周期大于踢盘周期【已弃用】

    /* 在线扩盘 */
    SC_RAID_EXP_DISK_NUM_INVALID           = 0x2ea, // 扩盘失败,因为扩盘盘数非法
    SC_RAID_EXP_DISK_REPEAT                = 0x2eb, // 扩盘失败,因为选盘重复
    SC_RAID_EXP_LUN_EXCEED                 = 0x2ec, // 扩盘失败,因为该RAID组LUN数超过8个
    SC_RAID_EXP_DISK_NUM_EXCEED_SPEC       = 0x2ed, // 扩盘失败,因为扩盘后磁盘总数超规格

    /* 安全擦除 */
    SC_RAID_VD_SANITIZE_TIMES_INVALID      = 0x2f4, // 安全擦除失败,因为擦除次数非法

    // SOC错误码 0x300-0x33f 64条
    SC_SOC_DATA_VERT_ERR            = 0x300,     // 数据检验失败
    SC_SOC_SCMI_NOT_INIT            = 0x301,     // SCMI未初始化
    SC_SOC_SCMI_WAIT_TIMEOUT        = 0x302,     // SCMI等待超时
    SC_SOC_SCMI_PRAM_ERR            = 0x303,     // SCMI参数错误
    SC_SOC_SCMI_RESP_VERT_ERR       = 0x304,     // SCMI返回参数校验失败
    SC_SOC_CRC_VERT_ERR             = 0x305,     // CRC校验失败
    SC_SOC_CFG_ENTRY_GET_ERR        = 0x306,     // 获取配置信息失败
    SC_SOC_PCIE_PORT_ERR            = 0x307,     // PCIE PORT ID错误
    SC_SOC_SGPIO_PARAM_ERR          = 0x308,     // SGPIO参数错误
    SC_SOC_EFUSE_PRAM_ERR           = 0x309,     // EFUSE参数错误
    SC_SOC_FW_INFO_PRAM_ERR         = 0x30a,     // 获取版本信息参数错误
    SC_SOC_I2C_PRAM_ERR             = 0x30b,     // I2C参数错误
    SC_SOC_SMBUS_ERR                = 0x30c,     // SMBUS通道错误
    SC_SOC_I2C_ERR                  = 0x30d,     // I2C通道错误
    SC_SOC_DJTAG_ERR                = 0x30e,     // DJTAG错误
    SC_SOC_SFC_PRAM_ERR             = 0x30f,     // SFC参数错误
    SC_SOC_SFC_TIMROUT_ERR          = 0x310,     // SFC超时错误
    SC_SOC_IMU_EXCEPTION            = 0x311,     // IMU响应超时
    SC_SOC_FW_UPDATE_SYNC_BUSY      = 0x312,     // 固件升级同步繁忙
    SC_SOC_FW_UPDATE_SYNC_FAIL      = 0x313,     // 固件升级同步失败
    SC_SOC_FW_UPDATE_TRANS_FAIL     = 0x314,     // 固件升级传输失败
    SC_SOC_FW_ACTIVE_FAIL           = 0x315,     // 固件激活失败
    SC_SOC_FW_ACTIVE_RAID_BUSY      = 0x316,     // 固件激活RAID繁忙
    SC_SOC_FW_ACTIVE_BDE_BUSY       = 0x317,     // 固件激活BACKEND繁忙
    SC_SOC_FW_ACTIVE_BP_BUSY        = 0x318,     // 固件激活BP繁忙
    SC_SOC_FW_ACTIVE_MCTP_BUSY      = 0x319,     // 固件激活MCTP繁忙
    SC_SOC_FW_ACTIVE_SS_BUSY        = 0x31a,     // 固件激活SS繁忙
    SC_SOC_FW_ACTIVE_CACHE_BUSY     = 0x31b,     // 固件激活CACHE繁忙
    SC_SOC_FW_ACTIVE_RAID_STOP_ERR  = 0x31c,     // 固件激活RAID停业务失败
    SC_SOC_FW_ACTIVE_BDE_STOP_ERR   = 0x31d,     // IMU响应BACKEND停业务失败
    SC_SOC_FW_ACTIVE_MCTP_STOP_ERR  = 0x31e,     // IMU响应MCTP停业务失败
    SC_SOC_SFC_READ_LOG_FAIL        = 0x31f,
    SC_SOC_SFC_READ_LW_FAIL         = 0x320,
    SC_SOC_SFC_READ_INDEX_FAIL      = 0x321,
    SC_SOC_SFC_READ_DUMP_FAIL       = 0x322,
    SC_SOC_SFC_ERASE_DUMP_FAIL      = 0x323,
    SC_SOC_SFC_WRITE_DUMP_FAIL      = 0x324,
    SC_SOC_FW_ACTIVE_CFG_STOP_ERR   = 0x325,     // 固件激活CFG停业务失败
    SC_SOC_SCMI_HW_LOCK_ERR         = 0x326,     // SCMI获取硬件锁失败
    SC_SOC_FW_ACTIVE_QUERY_FAIL     = 0x327,     // 固件激活查询失败
    SC_SOC_FW_UPDATE_NOT_INIT_ERR   = 0x328,     // 升级驱动未初始化
    SC_SOC_FW_UPDATE_PRAM_ERR       = 0x329,     // 固件升级参数错误
    SC_SOC_INVALID_NAND_LOG         = 0x32a,     // 指定收集的nandlog不存在
    SC_SOC_CLEAR_NAND_LOG_ERR       = 0x32b,     // 清除nandlog失败
    SC_SOC_FUNC_ERR                 = 0x32c,     // 命令执行结果失败
    SC_SOC_FW_ACTIVE_CACHE_STOP_ERR = 0x32d,     // 固件激活CACHE停业务失败

    SC_ERROR_CODE_MAX                       = 0x800, // 错误码最大值，不包含此值
};

struct status_code_info {
    int sc_code;
    const char *sc_info;
};

#endif
