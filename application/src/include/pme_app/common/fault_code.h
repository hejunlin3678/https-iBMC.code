/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : fault_code.h
  版 本 号   : 初稿
  作    者   : wangzong (w00213782)
  生成日期   : 2018年09月22日
  最近修改   :
  功能描述   : BMC 各类日志故障码定义
               该故障码可以在OMRP系统上查询
               增加该故障码需要在OMRP系统上增加对应的信息
  函数列表   :
  修改历史   :
  1.日    期   : 2018年09月22日
    作    者   : wangzong (w00213782)
    修改内容   : AR.SR.SFEA02130924.053.001 : 统一故障通过公司网站承载

  故障码格式 :
  --------------------------------------------
  | 前缀 | 1位大类号 | 2位模块号 | 3位故障码 |
  --------------------------------------------
  | SVR- |    0~9    |   00~99   |  000~999  |
  --------------------------------------------

  注：
  故障码规则:
     1、只能增加
     2、不能删除
     3、不能修改(可优化描述，不能改变含义)


        模块         大类    模块起始ID   数量
        _Public       00        00        10
        agentapp      00        10        1
        MCTP          00        11        1
        BIOS          00        12        3
        BMC           00        15        10
        card_manage   00        25        3
        cooling_app   00        28        1
        CpuMem        00        29        2
        data_sync     00        31        1
        Dft           00        32        1
        discovery     00        33        2
        switch_card   00        35        1
        diagnose      00        36        4
        hpc_mgmt      00        40        3
        ipmi_app      00        43        2
        kvm_vmm       00        45        3
        Lcd           00        48        3
        MaintDebug    00        51        1
        net_nat       00        52        1
        PcieSwitch    00        53        1
        PowerMgnt     00        54        2
        redfish       00        56        15
        rimm          00        71        1
        sensor_alarm  00        72        5
        Smm_Mgnt      00        77        2
        Snmp          00        79        1
        StorageMgnt   00        80        2
        UPGRADE       00        82        4
        User          00        86        3
        Asm           00        89        1
        data_sync2    00        90        1
        smm_lsw       00        91        1
        LicenseMgnt   00        92        1
        arm_diagnose  00        93        1
        CoolingSMM    00        94        1
        SmmSol        00        95        1
        LIBs          00        96        4
        PME           01        00        5
        FileManage    01        05        1

******************************************************************************/
#ifndef __FAULT_CODE_H__
#define __FAULT_CODE_H__
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define FC_FMT                           "%s,"
#define FC_BUFFER_LENGTH                 16
#define FC_CONTENT_LENGTH                11
#define FC_FIX_PRE                       "SVR-"
#define FC_PRE                           FC_FIX_PRE"00"

/* 维护日志后续必须使用该接口 */
#ifdef UNIT_TEST
#define maintenance_log_v2(level, fault_code, format, arg...) \
do  \
{   \
    printf("maintenance_log: level=%d fault_code=%s ", level, fault_code);  \
    printf(format, ##arg);  \
    printf("\r\n");  \
} while (0)
#else
#define maintenance_log_v2(level, fault_code, format, arg...) /*lint -save -e506 */ \
do  \
{   \
    maintenance_log((level), FC_FMT format, (fault_code), ##arg);  \
} while (0) /*lint -restore */
#endif

/*
   注意:
   故障码规则:
   1、只能增加
   2、不能删除
   3、不能修改(可优化描述，不能改变含义)
*/

/* _Public 公共模块 00-09 */
#define FC__PUBLIC_PRE                   FC_PRE"00"
#define FC__PUBLIC_OK                    FC__PUBLIC_PRE"000"
#define FC__PUBLIC_CALL_REMOTE           FC__PUBLIC_PRE"001"
#define FC__PUBLIC_SET_UBOOT_ENV_FAIL    FC__PUBLIC_PRE"002"
#define FC__PUBLIC_BMC_BUSY              FC__PUBLIC_PRE"003"
#define FC__PUBLIC_BMC_HEART_LOSS        FC__PUBLIC_PRE"004"

/* agentapp 模块 10 */
#define FC_AGENTAPP_PRE                  FC_PRE"10"
#define FC_AGENTAPP_SYN_BIOS_CFG_FAIL    FC_AGENTAPP_PRE"001"
#define FC_AGENTAPP_SW_FLASH_FAIL        FC_AGENTAPP_PRE"002"

/* BMC 模块 15-24 */
#define FC_BMC_PRE                       FC_PRE"15"
#define FC_BMC_CAFE_ERR                  FC_BMC_PRE"001"
#define FC_BMC_MEZZ_MAC_FAIL             FC_BMC_PRE"002"
#define FC_BMC_OPM_V_LOW                 FC_BMC_PRE"003"
#define FC_BMC_OPM_V_HIGH                FC_BMC_PRE"004"
#define FC_BMC_OPM_PWR_TX_L              FC_BMC_PRE"005"
#define FC_BMC_OPM_PWR_TX_H              FC_BMC_PRE"006"
#define FC_BMC_OPM_PWR_RX_L              FC_BMC_PRE"007"
#define FC_BMC_OPM_PWR_RX_H              FC_BMC_PRE"008"
#define FC_BMC_SET_MAC_FAIL              FC_BMC_PRE"009"
#define FC_BMC_NOT_INSERT                FC_BMC_PRE"010"
#define FC_BMC_REBOOT_FAIL               FC_BMC_PRE"011"
#define FC_BMC_MEM_INSUFFICIENT          FC_BMC_PRE"012"
#define FC_BMC_DHCP_FAIL                 FC_BMC_PRE"013"
#define FC_BMC_NCSI_TO_PORT1             FC_BMC_PRE"014"
#define FC_BMC_SW_FLASH                  FC_BMC_PRE"015"
#define FC_BMC_UPG_MEM_MGNT              FC_BMC_PRE"016"
#define FC_BMC_WDT_PWR_OFF_FAIL          FC_BMC_PRE"017"
#define FC_BMC_WDT_PWR_CYCLE_FAIL        FC_BMC_PRE"018"
#define FC_BMC_WDT_TIMEOUT_INFO          FC_BMC_PRE"019"
#define FC_BMC_POWER_ON_FAIL             FC_BMC_PRE"020"
#define FC_BMC_FORCE_POWER_OFF_FAIL      FC_BMC_PRE"021"

/* card_manage 模块 25-27 */
#define FC_CARD_MNG_PRE                  FC_PRE"25"
#define FC_CARD_MNG_PWR_CHIP_FAIL        FC_CARD_MNG_PRE"001"
#define FC_CARD_MNG_PCIE_ERR             FC_CARD_MNG_PRE"002"
#define FC_CARD_MNG_PCIE_XML_ERR         FC_CARD_MNG_PRE"003"
#define FC_CARD_MNG_NVME_XML_ERR         FC_CARD_MNG_PRE"004"
#define FC_CARD_MNG_MIGRATE_FAIL         FC_CARD_MNG_PRE"005"
#define FC_CARD_MNG_MCU_BOOT_FAIL        FC_CARD_MNG_PRE"006"
#define FC_CARD_MNG_MCU_UPGRADE_FAIL     FC_CARD_MNG_PRE"007"

/* CpuMem 模块 29-30 */
#define FC_CPUMEM_PRE                    FC_PRE"29"
#define FC_CPUMEM_NO_RESPONSE            FC_CPUMEM_PRE"001"
#define FC_CPUMEM_MISMATCH               FC_CPUMEM_PRE"002"
#define FC_CPUMEM_BIST_FAIL              FC_CPUMEM_PRE"003"
#define FC_CPUMEM_OFFLINE_FAIL           FC_CPUMEM_PRE"004"
#define FC_CPUMEM_UNSUPPORT_OP           FC_CPUMEM_PRE"005"
#define FC_CPUMEM_VMSE_ERR               FC_CPUMEM_PRE"006"
#define FC_CPUMEM_QPI_ERR                FC_CPUMEM_PRE"007"
#define FC_CPUMEM_PCI_RES_ERR            FC_CPUMEM_PRE"008"
#define FC_CPUMEM_ISOLATION              FC_CPUMEM_PRE"009"
#define FC_CPUMEM_PRESENT                FC_CPUMEM_PRE"010"
#define FC_CPUMEM_CE_LMT                 FC_CPUMEM_PRE"011"
#define FC_CPUMEM_CONFIG_ERR             FC_CPUMEM_PRE"012"
#define FC_CPUMEM_DDDC_SPR               FC_CPUMEM_PRE"013"
#define FC_CPUMEM_RANK_SPR               FC_CPUMEM_PRE"014"
#define FC_CPUMEM_DEVICE_TAG             FC_CPUMEM_PRE"015"
#define FC_CPUMEM_MIRROR_FAIL            FC_CPUMEM_PRE"016"
#define FC_CPUMEM_CE_OVERFLOW            FC_CPUMEM_PRE"017"
#define FC_CPUMEM_UCE                    FC_CPUMEM_PRE"018"
#define FC_CPUMEM_ECMA_ERR               FC_CPUMEM_PRE"019"
#define FC_CPUMEM_CE_BUCKET              FC_CPUMEM_PRE"020"
#define FC_CPUMEM_POST_REPAIR            FC_CPUMEM_PRE"021"
#define FC_CPUMEM_FAULT_INFO             FC_CPUMEM_PRE"022"
#define FC_CPUMEM_NO_SYS_MEM             FC_CPUMEM_PRE"023"
#define FC_CPUMEM_KEYBOARD_ERR           FC_CPUMEM_PRE"024"
#define FC_CPUMEM_VIDEO_ERR              FC_CPUMEM_PRE"025"
#define FC_CPUMEM_PCLS                   FC_CPUMEM_PRE"026"

/* switch_card 模块 35 */
#define FC_SWITCH_CARD_PRE               FC_PRE"35"
#define FC_SWITCH_CARD_LIN_MODE_FAIL     FC_SWITCH_CARD_PRE"001"
#define FC_SWITCH_CARD_UPG_PHY_FAIL      FC_SWITCH_CARD_PRE"002"
#define FC_SWITCH_CARD_CLOSE_PORT_FAIL   FC_SWITCH_CARD_PRE"003"
#define FC_SWITCH_CARD_LINK_RESET        FC_SWITCH_CARD_PRE"004"

/* diagnose 模块 36-39 */
#define FC_DIAGNOSE_PRE                  FC_PRE"36"
#define FC_DIAGNOSE_CLEAR_DB_FAIL        FC_DIAGNOSE_PRE"001"
#define FC_DIAGNOSE_DB_EXCEED_LIMIT      FC_DIAGNOSE_PRE"002"
#define FC_DIAGNOSE_MEMORY_PREFAULT      FC_DIAGNOSE_PRE"003"

/* hpc_mgmt 模块 40-42 */
#define FC_HPC_MGMT_PRE                  FC_PRE"40"
#define FC_HPC_MGMT_RPC_SVR_ERR          FC_HPC_MGMT_PRE"001"
#define FC_HPC_MGMT_GET_CPU_SPEED_FAIL   FC_HPC_MGMT_PRE"002"

/* MaintDebug 模块 51 */
#define FC_DEBUG_PRE                     FC_PRE"51"
#define FC_DEBUG_CLCT_FAIL_UPG           FC_DEBUG_PRE"001"
#define FC_DEBUG_CLCT_FAIL_MEM           FC_DEBUG_PRE"002"
#define FC_DEBUG_DEL_REP                 FC_DEBUG_PRE"003"
#define FC_DEBUG_DEL_REP_FAIL            FC_DEBUG_PRE"004"

/* PcieSwitch 模块 53 */
#define FC_PCIE_SW_PRE                   FC_PRE"53"
#define FC_PCIE_SW_UPG_PSW_FAIL          FC_PCIE_SW_PRE"001"
#define FC_PCIE_SW_PSW_EEP_EMP           FC_PCIE_SW_PRE"002"
#define FC_PCIE_SW_UPG_RETIMER_FAIL      FC_PCIE_SW_PRE"003"

/* PowerMgnt 模块 54-55 */
#define FC_POWER_MGNT_PRE                FC_PRE"54"
#define FC_POWER_MGNT_OVER_TEMP          FC_POWER_MGNT_PRE"001"
#define FC_POWER_MGNT_OUT_FAULT          FC_POWER_MGNT_PRE"002"
#define FC_POWER_MGNT_FAN_FAULT          FC_POWER_MGNT_PRE"003"
#define FC_POWER_MGNT_FAULT_INFO         FC_POWER_MGNT_PRE"004"

/* rimm 模块 71 */
#define FC_RIMM_PRE                      FC_PRE"71"
#define FC_RIMM_CLCT_FAIL_COMM           FC_RIMM_PRE"001"
#define FC_RIMM_CLCT_FAIL_RMT            FC_RIMM_PRE"002"

/* sensor_alarm 模块 72-76 */
#define FC_SENSOR_ALM_PRE                FC_PRE"72"
#define FC_SENSOR_ALM_VDDQ_OUT_RNG       FC_SENSOR_ALM_PRE"001"
#define FC_SENSOR_ALM_HW_SIG_CHANGE      FC_SENSOR_ALM_PRE"002"
#define FC_SENSOR_ALM_ALM_DES_FLAG_FAIL  FC_SENSOR_ALM_PRE"003"
#define FC_SENSOR_ALM_NO_BOOT_DEV        FC_SENSOR_ALM_PRE"004"
#define FC_SENSOR_ALM_NO_BOOT_DISK       FC_SENSOR_ALM_PRE"005"
#define FC_SENSOR_ALM_PXE_SVR_ERR        FC_SENSOR_ALM_PRE"006"
#define FC_SENSOR_ALM_INVALID_BOOT_DEV   FC_SENSOR_ALM_PRE"007"
#define FC_SENSOR_ALM_PWR_DROP           FC_SENSOR_ALM_PRE"008"
#define FC_SENSOR_ALM_PWR_ON_TMOUT       FC_SENSOR_ALM_PRE"009"
#define FC_SENSOR_ALM_CPU_EEP_RD_FAIL    FC_SENSOR_ALM_PRE"010"

/* Smm_Mgnt 模块 77-78 */
#define FC_SMM_MGNT_PRE                  FC_PRE"77"
#define FC_SMM_MGNT_START_SW_PANEL       FC_SMM_MGNT_PRE"001"

/* StorageMgnt 模块 80-81 */
#define FC_STORAGE_PRE                   FC_PRE"80"
#define FC_STORAGE_ERR_CODE              FC_STORAGE_PRE"001"
#define FC_STORAGE_EVENT_CODE            FC_STORAGE_PRE"002"
#define FC_STORAGE_NO_IDENTIFY           FC_STORAGE_PRE"003"
#define FC_STORAGE_PHYCIAL_FAULT         FC_STORAGE_PRE"004"
#define FC_STORAGE_LOGICAL_FAULT         FC_STORAGE_PRE"005"
#define FC_STORAGE_RAID_COM_LOSS         FC_STORAGE_PRE"006"
#define FC_STORAGE_RAID_CE               FC_STORAGE_PRE"007"
#define FC_STORAGE_RAID_UCE              FC_STORAGE_PRE"008"
#define FC_STORAGE_RAID_ECC_LMT          FC_STORAGE_PRE"009"
#define FC_STORAGE_RAID_NVRAM            FC_STORAGE_PRE"010"
#define FC_STORAGE_RAID_BBU              FC_STORAGE_PRE"011"
#define FC_STORAGE_RAID_BBU_REPLACE      FC_STORAGE_PRE"012"
#define FC_STORAGE_RAID_INIT_ERROR       FC_STORAGE_PRE"013"
#define FC_STORAGE_HDD_FW_UNCONFIGURED_BAD      FC_STORAGE_PRE"014"
#define FC_STORAGE_HDD_FW_FOREIGN        FC_STORAGE_PRE"015"
#define FC_STORAGE_HDD_FW_FAILED         FC_STORAGE_PRE"016"
#define FC_STORAGE_HDD_FW_OFFLINE        FC_STORAGE_PRE"017"
#define FC_STORAGE_LOGICAL_IO_DEGRADED              FC_STORAGE_PRE"018"
#define FC_STORAGE_HDD_IO_DEGRADED                  FC_STORAGE_PRE"019"
#define FC_STORAGE_HDD_INTERMITTENT_DISCONNECTION   FC_STORAGE_PRE"020"
#define FC_STORAGE_LINK_PHY_ERROR                   FC_STORAGE_PRE"021"
#define FC_STORAGE_PHYCIAL_PREFAULT                 FC_STORAGE_PRE"022"
#define FC_STORAGE_HDD_FW_NOT_SUPPORTED             FC_STORAGE_PRE"024"
#define FC_STORAGE_HDD_FW_PREDICTIVE_FAILURE        FC_STORAGE_PRE"025"
#define FC_STORAGE_RAID_BOARD_HARDWARE              FC_STORAGE_PRE"026"
#define FC_STORAGE_RAID_BOARD_CLOCK                 FC_STORAGE_PRE"027"
#define FC_STORAGE_RAID_BOARD_POWER                 FC_STORAGE_PRE"028"
#define FC_STORAGE_RAID_BOARD_CAPACITY              FC_STORAGE_PRE"029"
#define FC_STORAGE_RAID_FLASH                       FC_STORAGE_PRE"030"

/* UPGRADE 模块 82-85 */
#define FC_UPGRADE_PRE                   FC_PRE"82"
#define FC_UPGRADE_UPG_MCU_FAIL          FC_UPGRADE_PRE"001"
#define FC_UPGRADE_UPG_VRD_FAIL          FC_UPGRADE_PRE"002"
#define FC_UPGRADE_UPG_PRR_FAIL          FC_UPGRADE_PRE"003"
#define FC_UPGRADE_VRD_BOM_FAIL          FC_UPGRADE_PRE"004"
#define FC_UPGRADE_UPG_ME_ERR            FC_UPGRADE_PRE"005"
#define FC_UPGRADE_UPG_RETIMER_FAIL      FC_UPGRADE_PRE"006"
#define FC_UPGRADE_UPG_HINIC_FAIL        FC_UPGRADE_PRE"007"
#define FC_UPGRADE_UPG_BMC_FAIL          FC_UPGRADE_PRE"008"


/* user 模块 86 */
#define FC_USER_PRE                      FC_PRE"86"
#define FC_USER_KERBEROS_LOGIN_FAILED    FC_USER_PRE"001"


/* LicenseMgnt 模块 92 */
#define FC_LICENSE_PRE                   FC_PRE"92"
#define FC_LICENSE_LIC_REVOKE            FC_LICENSE_PRE"001"

/* RackMgnt 模块 94 */
#define FC_RACK_MGNT_PRE                 FC_PRE"94"

/* SmmSol 模块 95 */
#define FC_SMM_SOL_PRE                   FC_PRE"95"
#define FC_SMM_SOL_TMOUT                 FC_SMM_SOL_PRE"001"

/* libs 模块 96-99 */
#define FC_LIBS_PRE                      FC_PRE"96"
#define FC_LIBS_GET_REMOTE               FC_LIBS_PRE"001"

/* 网络模块 100-102 */
#define FC_NETCONFIG_PRE                  FC_PRE"100"
#define FC_SSH_SVR_ABNORMAL               FC_NETCONFIG_PRE"001"

/*
   注意:
   故障码规则:
   1、只能增加
   2、不能删除
   3、不能修改(可优化描述，不能改变含义)
*/


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* __FAULT_CODE_H__ */

