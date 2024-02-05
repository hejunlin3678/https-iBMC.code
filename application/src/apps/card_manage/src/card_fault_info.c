

#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <math.h>
#include "pme_app/pme_app.h"
#include "pcie_card.h"
#include "pcie_card_parse_bdf_thread.h"
#include "card_ipmi_msg.h"
#include "get_version.h"
#include "card_manage_module.h"
#include "pcie_card_comm.h"
#include "mezz_card.h"
#include "pme_app/common/debug_log_macro.h"
#include "../../../libs/pcie_mgmt/inc/pcie_mgmt_by_std_smbus.h"

#define SJK1828_CARD_NAME "SJK1828 V2.0" // 需要屏蔽PCIe降带宽维护日志和告警的密码卡名称


TASKID g_pcie_decrease_bandwithd_task_id = 0;
gint32 g_pcie_card_load_completed = 0;

#define MCU_HEART_BEAT_NORMAL   0
#define MCU_HEART_BEAT_ABNORMAL 1
#define MCU_HEART_BEAT_DEBOUNCE_TIMES 5


LOCAL guint8 g_pcie_card_slot_fault_state = 0;
G_LOCK_DEFINE(g_pcie_card_slot_fault_state);

// 定义每类卡的错误码范围
LOCAL FAULT_CODE_RANGE_S fault_code_range_tbl[] =
{
    {0, 0},       // 0 无效值
    {0, 0},       // 1 RAID卡
    {0, 0},       // 2 SSD卡
    {0, 0},       // 3 NIC卡
    {4096, 7581}, // 4 压缩卡, 例如视频压缩卡IVS3800(复用了D-MINI卡的部分故障码)
    {5120, 5138}, // 5 FPGA卡
    {7168, 7600}, // 6 D-mini卡
    {7168, 7196}, // 7 中端FPGA卡，比如FX505
    {8192, 8673}  // 8 SDI卡
};

#define FAULT_CODE_INWORD 2
#define OVER_TEMP_MASK 0x800
#define OVER_CURRENT_MASK 0x1000
typedef struct {
    guint32 fault_code;
    const gchar *event_monitor_obj_name;
    const gchar *event_desc_en;
} FAULT_CODE_DESC_EVENT_MAP_S;

#define SET_BIT(array, index)   (array)[(index) / 32] |= (1UL << ((index) % 32))
#define CLEAR_BIT(array, index) (array)[(index) / 32] &= ~(1UL << ((index) % 32))
#define GET_BIT(array, index)   (((array)[(index) / 32] >> ((index) % 32)) & 1UL)

LOCAL void clear_pcie_card_fault_from_mcu(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info, gboolean sdi_support_mcu);


LOCAL FAULT_CLASS_S g_fault_map_table[] = {
    {7168, 0x08},       {7194, 0x08},

    {7169, 0x10},       {7504, 0x10},       {7505, 0x10},

    {7170, 0x20},

    {7171, 0x40},

    {7172, 0x80},       {7598, 0x80},       {7597, 0x80},

    {4138, 0x100},      {4141, 0x100},      {4144, 0x100},      {4147, 0x100},      {4150, 0x100},      {4153, 0x100},
    {4156, 0x100},      {4159, 0x100},      {4162, 0x100},      {4165, 0x100},      {4168, 0x100},      {4171, 0x100},
    {4174, 0x100},      {4177, 0x100},      {4180, 0x100},      {4183, 0x100},      {4186, 0x100},      {4189, 0x100},
    {4192, 0x100},      {4195, 0x100},      {4198, 0x100},      {4201, 0x100},      {7173, 0x100},      {7174, 0x100},
    {7506, 0x100},      {7509, 0x100},      {7512, 0x100},      {7518, 0x100},      {7594, 0x100},      {7595, 0x100},

    {4136, 0x200},      {4139, 0x200},      {4142, 0x200},      {4145, 0x200},      {4148, 0x200},      {4151, 0x200},
    {4154, 0x200},      {4157, 0x200},      {4160, 0x200},      {4163, 0x200},      {4166, 0x200},      {4169, 0x200},
    {4172, 0x200},      {4175, 0x200},      {4178, 0x200},      {4181, 0x200},      {4184, 0x200},      {4187, 0x200},
    {4190, 0x200},      {4193, 0x200},      {4196, 0x200},      {4199, 0x200},      {7175, 0x200},      {7176, 0x200},
    {7507, 0x200},      {7510, 0x200},      {7513, 0x200},      {7562, 0x200},      {7564, 0x200},      {7566, 0x200},
    {7568, 0x200},      {7570, 0x200},      {7572, 0x200},      {7574, 0x200},      {7576, 0x200},      {7578, 0x200},
    {7580, 0x200},      {7582, 0x200},      {7584, 0x200},      {7586, 0x200},      {7588, 0x200},      {7590, 0x200},
    {7592, 0x200},

    {4137, 0x400},      {4140, 0x400},      {4143, 0x400},      {4146, 0x400},      {4149, 0x400},      {4152, 0x400},
    {4155, 0x400},      {4158, 0x400},      {4161, 0x400},      {4164, 0x400},      {4167, 0x400},      {4170, 0x400},
    {4173, 0x400},      {4176, 0x400},      {4179, 0x400},      {4182, 0x400},      {4185, 0x400},      {4188, 0x400},
    {4191, 0x400},      {4194, 0x400},      {4197, 0x400},      {4200, 0x400},      {7177, 0x400},      {7178, 0x400},
    {7508, 0x400},      {7511, 0x400},      {7514, 0x400},      {7563, 0x400},      {7565, 0x400},      {7567, 0x400},
    {7569, 0x400},      {7571, 0x400},      {7573, 0x400},      {7575, 0x400},      {7577, 0x400},      {7579, 0x400},
    {7581, 0x400},      {7583, 0x400},      {7585, 0x400},      {7587, 0x400},      {7589, 0x400},      {7591, 0x400},
    {7593, 0x400},

    {7179, 0x800},

    {7180, 0x1000},

    {7182, 0x2000},     {7196, 0x2000},     {7199, 0x2000},

    {7184, 0x4000},

    {7185, 0x8000},     {7186, 0x8000},

    {7181, 0x10000},    {7195, 0x10000},    {7187, 0x10000},    {7188, 0x10000},    {7189, 0x10000},
    {7190, 0x10000},    {7191, 0x10000},    {7192, 0x10000},    {7193, 0x10000},    {7600, 0x10000},

    {7500, 0x20000},    {7501, 0x20000},

    {7515, 0x40000},    {7516, 0x40000},

    {7517, 0x80000},

    {7519, 0x100000},

    {7520, 0x200000},   {7521, 0x200000},   {7522, 0x200000},   {7523, 0x200000},   {7524, 0x200000},
    {7525, 0x200000},   {7526, 0x200000},   {7527, 0x200000},

    {7528, 0x400000},   {7529, 0x400000},   {7530, 0x400000},   {7531, 0x400000},   {7532, 0x400000},
    {7533, 0x400000},   {7534, 0x400000},   {7535, 0x400000},

    {7536, 0x800000},   {7537, 0x800000},   {7538, 0x800000},   {7539, 0x800000},   {7540, 0x800000},
    {7541, 0x800000},   {7542, 0x800000},   {7543, 0x800000},

    {7544, 0x1000000},  {7545, 0x1000000},  {7546, 0x1000000},  {7547, 0x1000000},

    {7548, 0x2000000},  {7549, 0x2000000},  {7550, 0x2000000},  {7551, 0x2000000},

    {4120, 0x4000000},  {4121, 0x4000000},  {4122, 0x4000000},  {4123, 0x4000000},  {4124, 0x4000000},
    {4125, 0x4000000},  {4126, 0x4000000},  {4127, 0x4000000},  {4128, 0x4000000},  {4129, 0x4000000},
    {7552, 0x4000000},  {7553, 0x4000000},  {7554, 0x4000000},  {7555, 0x4000000},  {7556, 0x4000000},
    {7557, 0x4000000},  {7558, 0x4000000},  {7559, 0x4000000},

    {7560, 0x8000000},

    {7502, 0x10000000}, {7503, 0x10000000}, {7561, 0x20000000},

    {7596, 0x40000000},

    {7183, 0x80000000},
};
FAULT_CODE_DESC_EVENT_MAP_S g_fault_code_desc_event_table[] = {
    { 8199, "CpuMinorFaultOnPCIeMntr1", "CPU Frequency Reduction." },
    { 8600, "CpuMinorFaultOnPCIeMntr2", "ME Heartbeat Error."      },
    { 8350, "CpuMinorFaultOnPCIeMntr3", "CPU Cache Error."         },
    { 8192, "CpuMajorFaultOnPCIeMntr1", "CPU Bist Error."          },
    { 8351, "CpuMajorFaultOnPCIeMntr2", "CPU NFE IERR."            },
    { 8198, "CpuMajorFaultOnPCIeMntr3", "CPU MCE/AER Error."       },
    { 8200, "CpuMajorFaultOnPCIeMntr4", "CPU Core Isolation."      },
    { 8201, "CpuMajorFaultOnPCIeMntr5", "CPU Margin Fail."         },
    { 8352, "CpuMajorFaultOnPCIeMntr6", "Cache Timeout."           },
    { 8353, "CpuMajorFaultOnPCIeMntr7", "Memory Request Timeout."  },
    { 8196, "CpuCriticalFaultOnPCIeMntr1", "CPU Fatal Error."      },
    { 8197, "CpuCriticalFaultOnPCIeMntr2", "CPU FE IERR."          },
    { 8262, "CpuCriticalFaultOnPCIeMntr3", "CPU Overheat Shutdown." },

    { 8203, "MemoryMinorFaultOnPCIeMntr1", "Memory self-check failure."  },
    { 8214, "MemoryMinorFaultOnPCIeMntr2", "Memory Isolation."           },
    { 8204, "MemoryMajorFaultOnPCIeMntr1", "Memory UCE."                 },
    { 8206, "MemoryMajorFaultOnPCIeMntr2", "Memory Channel A Isolation." },
    { 8207, "MemoryMajorFaultOnPCIeMntr3", "Memory Channel B Isolation." },
    { 8208, "MemoryMajorFaultOnPCIeMntr4", "Memory Channel C Isolation." },
    { 8209, "MemoryMajorFaultOnPCIeMntr5", "Memory Channel D Isolation." },
    { 8210, "MemoryMajorFaultOnPCIeMntr6", "Memory Initialization Error."},
    { 8213, "MemoryMajorFaultOnPCIeMntr7", "Memory Pre-failure."         },

    { 8226, "DiskMinorFaultOnPCIeMntr1", "M.2 Smart Status Error."         },
    { 8227, "DiskMinorFaultOnPCIeMntr2", "M.2 Smart Information Get Fail." },
    { 8216, "DiskMajorFaultOnPCIeMntr1", "M.2 A Not present."              },
    { 8217, "DiskMajorFaultOnPCIeMntr2", "M.2 B Not present."              },
    { 8218, "DiskMajorFaultOnPCIeMntr3", "M.2 A Scan Fail."                },
    { 8219, "DiskMajorFaultOnPCIeMntr4", "M.2 B Scan Fail."                },
    { 8220, "DiskMajorFaultOnPCIeMntr5", "M.2 Life Warning."               },
    { 8223, "DiskMajorFaultOnPCIeMntr6", "M.2 Raid Status Error."          },
    { 8666, "DiskMajorFaultOnPCIeMntr7", "M.2 A OverCurrent."          },
    { 8667, "DiskMajorFaultOnPCIeMntr8", "M.2 B OverCurrent."          },

    { 8231, "NetCardMinorFaultOnPCIeMntr1", "Network Port Link Fail."                                              },
    { 8233, "NetCardMinorFaultOnPCIeMntr2", "The negotiated working rate of the network port is incorrect."        },
    { 8236, "NetCardMinorFaultOnPCIeMntr3", "Abnormal optical power."                                              },
    { 8604, "NetCardMinorFaultOnPCIeMntr4", "The SmartNic RTC abnormal." },
    { 8228, "NetCardMajorFaultOnPCIeMntr1", "NIC health status abnormal."                                          },
    { 8230, "NetCardMajorFaultOnPCIeMntr2", "Abnormal optical module Status."                                      },
    { 8232, "NetCardMajorFaultOnPCIeMntr3", "The Serdes bit error rate of the network port exceeds the threshold." },
    { 8234, "NetCardMajorFaultOnPCIeMntr4", "The Network mac address get fail."                                    },
    { 8235, "NetCardMajorFaultOnPCIeMntr5", "MPU heartbeat lost."                                                  },
    { 8402, "NetCardMajorFaultOnPCIeMntr6", "NIC BOM gets fail."      },
    { 8401, "NetCardMajorFaultOnPCIeMntr7", "NIC Elabel gets fail."   },
    { 8606, "NetCardMajorFaultOnPCIeMntr8", "NIC Flash life warning." },
    { 8602, "NetCardMajorFaultOnPCIeMntr9", "SmartNIC has critical RAS error." },
    { 8668, "NetCardMajorFaultOnPCIeMntr10", "Optical module 1 overcurrent." },
    { 8669, "NetCardMajorFaultOnPCIeMntr11", "Optical module 2 overcurrent." },
    { 8354, "NetCardCriticalFaultOnPCIeMntr1", "NIC chip Error."                                                   },
    { 8603, "NetCardCriticalFaultOnPCIeMntr2", "SmartNIC has fatal RAS error." },

    { 8239, "StartingMinorFaultOnPCIeMntr1", "Watchdog timeout alarm during BIOS loading." },
    { 8243, "StartingMinorFaultOnPCIeMntr2", "Watchdog timeout alarm during OS loading."   },
    { 8607, "StartingMinorFaultOnPCIeMntr3",
        "NIC Firmware Verify Fail." },
    { 8240, "StartingMajorFaultOnPCIeMntr1", "BIOS Startup Fail."                          },
    { 8241, "StartingMajorFaultOnPCIeMntr2", "Watchdog timeout alarm during PXE."          },
    { 8242, "StartingMajorFaultOnPCIeMntr3", "OS Verify Fail."                             },
    { 8244, "StartingMajorFaultOnPCIeMntr4", "OS Startup Fail."                            },
    { 8400, "StartingMajorFaultOnPCIeMntr5", "BIOS Firmware upgrade fail."                 },
    { 8671, "StartingMajorFaultOnPCIeMntr6", "Failed to obtain the hard disk boot option." },
    { 8672, "StartingMajorFaultOnPCIeMntr7", "The certificate is about to expire."         },
    { 8673, "StartingMajorFaultOnPCIeMntr8", "SPU Heartbeat lost."                         },

    { 8605, "McuMinorFaultOnPCIeMntr1", "CPLD abnormal heartbeat"            },
    { 8359, "McuMinorFaultOnPCIeMntr2", "MCU Firmware(Single Partition) integrity check fail." },
    { 8246, "McuMajorFaultOnPCIeMntr1", "MCU SRAM abnormal."                 },
    { 8247, "McuMajorFaultOnPCIeMntr2", "MCU Flash abnormal."                },
    { 8248, "McuMajorFaultOnPCIeMntr3", "MCU RTC abnormal."                  },
    { 8390, "McuMajorFaultOnPCIeMntr4", "MCU Firmware upgrade fail."         },
    { 8391, "McuMajorFaultOnPCIeMntr5", "MCU Firmware failed to take effect." },
    { 8245, "McuMajorFaultOnPCIeMntr6", "MCU Firmware integrity check fail." },
    { 8358, "McuMajorFaultOnPCIeMntr7", "VRD Firmware upgrade fail."          },
    { 8357, "McuMajorFaultOnPCIeMntr8", "VRD inspect fail."                   },
    { 8389, "McuMajorFaultOnPCIeMntr9", "VRD inspect inconsistent data."      },
    { 8610, "McuMajorFaultOnPCIeMntr10", "VRD inspect abnormal."              },
    { 8670, "McuMajorFaultOnPCIeMntr11", "VRD OverCurrent."                   },

    { 8250, "TemperatureMajorFaultOnPCIe1", "The CPU temperature of SmartNIC too high."     },
    { 8251, "TemperatureMajorFaultOnPCIe2", "The CPU temperature of SmartNIC get fail."     },
    { 8252, "TemperatureMajorFaultOnPCIe3", "The temperature of optical module 1 abnormal." },
    { 8253, "TemperatureMajorFaultOnPCIe4", "The temperature of optical module 1 get fail." },
    { 8254, "TemperatureMajorFaultOnPCIe5", "The temperature of optical module 2 abnormal." },
    { 8255, "TemperatureMajorFaultOnPCIe6", "The temperature of optical module 2 get fail." },
    { 8256, "TemperatureMajorFaultOnPCIe7", "The temperature of Air outlet 1 abnormal."     },
    { 8257, "TemperatureMajorFaultOnPCIe8", "The temperature of Air outlet 1 get fail."     },
    { 8258, "TemperatureMajorFaultOnPCIe9", "The temperature of Air outlet 2 abnormal."     },
    { 8259, "TemperatureMajorFaultOnPCIe10", "The temperature of Air outlet 2 get fail."     },
    { 8355, "TemperatureMajorFaultOnPCIe11", "The temperature of Air inlet 1 abnormal."      },
    { 8356, "TemperatureMajorFaultOnPCIe12", "The temperature of Air inlet 1 get fail."      },
    { 8260, "TemperatureMajorFaultOnPCIe13", "The temperature of NIC abnormal."              },
    { 8261, "TemperatureMajorFaultOnPCIe14", "The temperature of NIC get fail."              },

    { 8264, "CableMinorFaultOnPCIeMntr", "NCSI cable Not Present." },

    { 8266, "VoltageMajorFaultOnPCIeMntr1", "Voltage PVCCANA_CPU exceeds the overvoltage threshold."        },
    { 8267, "VoltageMajorFaultOnPCIeMntr2", "Voltage PVCCANA_CPU lower than the undervoltage threshol."     },
    { 8268, "VoltageMajorFaultOnPCIeMntr3", "Voltage PVCCANA_CPU get fail."                                 },
    { 8269, "VoltageMajorFaultOnPCIeMntr4", "Voltage P1V05 exceeds the overvoltage threshold."              },
    { 8270, "VoltageMajorFaultOnPCIeMntr5", "Voltage P1V05 lower than the undervoltage threshol."           },
    { 8271, "VoltageMajorFaultOnPCIeMntr6", "Voltage P1V05 get fail."                                       },
    { 8272, "VoltageMajorFaultOnPCIeMntr7", "Voltage PVDDQ_AB_CPU exceeds the overvoltage threshold."       },
    { 8273, "VoltageMajorFaultOnPCIeMntr8", "Voltage PVDDQ_AB_CPU lower than the undervoltage threshol."    },
    { 8274, "VoltageMajorFaultOnPCIeMntr9", "Voltage PVDDQ_AB_CPU get fail."                                },
    { 8275, "VoltageMajorFaultOnPCIeMntr10", "Voltage PVNN_PCH exceeds the overvoltage threshold."           },
    { 8276, "VoltageMajorFaultOnPCIeMntr11", "Voltage PVNN_PCH lower than the undervoltage threshol."        },
    { 8277, "VoltageMajorFaultOnPCIeMntr12", "Voltage PVNN_PCH get fail."                                    },
    { 8278, "VoltageMajorFaultOnPCIeMntr13", "Voltage P3V3_AUX exceeds the overvoltage threshold."           },
    { 8279, "VoltageMajorFaultOnPCIeMntr14", "Voltage P3V3_AUX lower than the undervoltage threshol."        },
    { 8280, "VoltageMajorFaultOnPCIeMntr15", "Voltage P3V3_AUX get fail."                                    },
    { 8281, "VoltageMajorFaultOnPCIeMntr16", "Voltage PVDDQ_EF_CPU exceeds the overvoltage threshold."       },
    { 8282, "VoltageMajorFaultOnPCIeMntr17", "Voltage PVDDQ_EF_CPU lower than the undervoltage threshol."    },
    { 8283, "VoltageMajorFaultOnPCIeMntr18", "Voltage PVDDQ_EF_CPU get fail."                                },
    { 8284, "VoltageMajorFaultOnPCIeMntr19", "Voltage PVPP_CPU_AB exceeds the overvoltage threshold."        },
    { 8285, "VoltageMajorFaultOnPCIeMntr20", "Voltage PVPP_CPU_AB lower than the undervoltage threshol."     },
    { 8286, "VoltageMajorFaultOnPCIeMntr21", "Voltage PVPP_CPU_AB get fail."                                 },
    { 8287, "VoltageMajorFaultOnPCIeMntr22", "Voltage PVPP_CPU_EF exceeds the overvoltage threshold."        },
    { 8288, "VoltageMajorFaultOnPCIeMntr23", "Voltage PVPP_CPU_EF lower than the undervoltage threshol."     },
    { 8289, "VoltageMajorFaultOnPCIeMntr24", "Voltage PVPP_CPU_EF get fail."                                 },
    { 8290, "VoltageMajorFaultOnPCIeMntr25", "Voltage P3V3_VCC exceeds the overvoltage threshold."           },
    { 8291, "VoltageMajorFaultOnPCIeMntr26", "Voltage P3V3_VCC lower than the undervoltage threshol."        },
    { 8292, "VoltageMajorFaultOnPCIeMntr27", "Voltage P3V3_VCC get fail."                                    },
    { 8293, "VoltageMajorFaultOnPCIeMntr28", "Voltage PVTT_CPU_AB exceeds the overvoltage threshold."        },
    { 8294, "VoltageMajorFaultOnPCIeMntr29", "Voltage PVTT_CPU_AB lower than the undervoltage threshol."     },
    { 8295, "VoltageMajorFaultOnPCIeMntr30", "Voltage PVTT_CPU_AB get fail."                                 },
    { 8296, "VoltageMajorFaultOnPCIeMntr31", "Voltage PVTT_CPU_EF exceeds the overvoltage threshold."        },
    { 8297, "VoltageMajorFaultOnPCIeMntr32", "Voltage PVTT_CPU_EF lower than the undervoltage threshol."     },
    { 8298, "VoltageMajorFaultOnPCIeMntr33", "Voltage PVTT_CPU_EF get fail."                                 },
    { 8299, "VoltageMajorFaultOnPCIeMntr34", "Voltage P12V exceeds the overvoltage threshold."               },
    { 8300, "VoltageMajorFaultOnPCIeMntr35", "Voltage P12V lower than the undervoltage threshol."            },
    { 8301, "VoltageMajorFaultOnPCIeMntr36", "Voltage P12V get fail."                                        },
    { 8302, "VoltageMajorFaultOnPCIeMntr37", "Voltage V_VCC_VRD_MOS exceeds the overvoltage threshold."      },
    { 8303, "VoltageMajorFaultOnPCIeMntr38", "Voltage V_VCC_VRD_MOS lower than the undervoltage threshol."   },
    { 8304, "VoltageMajorFaultOnPCIeMntr39", "Voltage V_VCC_VRD_MOS get fail."                               },
    { 8305, "VoltageMajorFaultOnPCIeMntr40", "Voltage P3V3_STBY exceeds the overvoltage threshold."          },
    { 8306, "VoltageMajorFaultOnPCIeMntr41", "Voltage P3V3_STBY lower than the undervoltage threshol."       },
    { 8307, "VoltageMajorFaultOnPCIeMntr42", "Voltage P3V3_STBY get fail."                                   },
    { 8308, "VoltageMajorFaultOnPCIeMntr43", "Voltage PVCCIN_CPU exceeds the overvoltage threshold."         },
    { 8309, "VoltageMajorFaultOnPCIeMntr44", "Voltage PVCCIN_CPU lower than the undervoltage threshol."      },
    { 8310, "VoltageMajorFaultOnPCIeMntr45", "Voltage PVCCIN_CPU get fail."                                  },
    { 8311, "VoltageMajorFaultOnPCIeMntr46", "Voltage P1V8_AUX exceeds the overvoltage threshold."           },
    { 8312, "VoltageMajorFaultOnPCIeMntr47", "Voltage P1V8_AUX lower than the undervoltage threshol."        },
    { 8313, "VoltageMajorFaultOnPCIeMntr48", "Voltage P1V8_AUX get fail."                                    },
    { 8314, "VoltageMajorFaultOnPCIeMntr49", "Voltage V_1V8_DVDD exceeds the overvoltage threshold."         },
    { 8315, "VoltageMajorFaultOnPCIeMntr50", "Voltage V_1V8_DVDD lower than the undervoltage threshol."      },
    { 8316, "VoltageMajorFaultOnPCIeMntr51", "Voltage V_1V8_DVDD get fail."                                  },
    { 8317, "VoltageMajorFaultOnPCIeMntr52", "Voltage V_0V8_AVDD_NIC exceeds the overvoltage threshold."     },
    { 8318, "VoltageMajorFaultOnPCIeMntr53", "Voltage V_0V8_AVDD_NIC lower than the undervoltage threshol."  },
    { 8319, "VoltageMajorFaultOnPCIeMntr54", "Voltage V_0V8_AVDD_NIC get fail."                              },
    { 8320, "VoltageMajorFaultOnPCIeMntr55", "Voltage V_VCC_3V3 exceeds the overvoltage threshold."          },
    { 8321, "VoltageMajorFaultOnPCIeMntr56", "Voltage V_VCC_3V3 lower than the undervoltage threshol."       },
    { 8322, "VoltageMajorFaultOnPCIeMntr57", "Voltage V_VCC_3V3 get fail."                                   },
    { 8323, "VoltageMajorFaultOnPCIeMntr58", "Voltage V_0V75_DVDD_NIC exceeds the overvoltage threshold."    },
    { 8324, "VoltageMajorFaultOnPCIeMntr59", "Voltage V_0V75_DVDD_NIC lower than the undervoltage threshol." },
    { 8325, "VoltageMajorFaultOnPCIeMntr60", "Voltage V_0V75_DVDD_NIC get fail."                             },
    { 8326, "VoltageMajorFaultOnPCIeMntr61", "Voltage V_VCC_5V0 exceeds the overvoltage threshold."          },
    { 8327, "VoltageMajorFaultOnPCIeMntr62", "Voltage V_VCC_5V0 lower than the undervoltage threshol."       },
    { 8328, "VoltageMajorFaultOnPCIeMntr63", "Voltage V_VCC_5V0 get fail."                                   },
    { 8329, "VoltageMajorFaultOnPCIeMntr64", "Voltage V_1V2_AVDD_NIC exceeds the overvoltage threshold."     },
    { 8330, "VoltageMajorFaultOnPCIeMntr65", "Voltage V_1V2_AVDD_NIC lower than the undervoltage threshol."  },
    { 8331, "VoltageMajorFaultOnPCIeMntr66", "Voltage V_1V2_AVDD_NIC get fail."                              },
    { 8332, "VoltageMajorFaultOnPCIeMntr67", "Voltage V_VCC_12V0 exceeds the overvoltage threshold."         },
    { 8333, "VoltageMajorFaultOnPCIeMntr68", "Voltage V_VCC_12V0 lower than the undervoltage threshol."      },
    { 8334, "VoltageMajorFaultOnPCIeMntr69", "Voltage V_VCC_12V0 get fail."                                  },
    { 8360, "VoltageMajorFaultOnPCIeMntr70", "Power Good PG_P12V_R_PC11 Abnormal."                           },
    { 8361, "VoltageMajorFaultOnPCIeMntr71", "Power Good PG_NIC_VRD Abnormal."                               },
    { 8362, "VoltageMajorFaultOnPCIeMntr72", "Power Good PWRGD_P5V_DRMOS_PF12 Abnormal."                     },
    { 8363, "VoltageMajorFaultOnPCIeMntr73", "Power Good PWRGD_P3V3_VCC_R_PF11  Abnormal."                   },
    { 8364, "VoltageMajorFaultOnPCIeMntr74", "Power Good PG_P3V3_VRD_R_PG9 Abnormal."                        },
    { 8365, "VoltageMajorFaultOnPCIeMntr75", "Power Good PG_P3V3_AUX_R_PD9 Abnormal."                        },
    { 8366, "VoltageMajorFaultOnPCIeMntr76", "Power Good PG_P1V8_AUX_R_PI10 Abnormal."                       },
    { 8367, "VoltageMajorFaultOnPCIeMntr77", "Power Good PWRGD_PVNN_PCH_R2_PE6 Abnormal."                    },
    { 8368, "VoltageMajorFaultOnPCIeMntr78", "Power Good PWRGD_P1V05_CPU_PH4 Abnormal."                      },
    { 8369, "VoltageMajorFaultOnPCIeMntr79", "Power Good PG_SLPS45_PB6 Abnormal."                            },
    { 8370, "VoltageMajorFaultOnPCIeMntr80", "Power Good PG_PVTT_CPU_AB_R_PI9 Abnormal."                     },
    { 8371, "VoltageMajorFaultOnPCIeMntr81", "Power Good PG_PVTT_CPU_EF_R_PE8 Abnormal."                     },
    { 8372, "VoltageMajorFaultOnPCIeMntr82", "Power Good PG_PVPP_CPU_AB_R2_PG1 Abnormal."                    },
    { 8373, "VoltageMajorFaultOnPCIeMntr83", "Power Good PG_CPUVTT_PE7 Abnormal."                            },
    { 8374, "VoltageMajorFaultOnPCIeMntr84", "Power Good PG_CPUVTT_PF15 Abnormal."                           },
    { 8375, "VoltageMajorFaultOnPCIeMntr85", "Power Good PWRGD_PVDDQ_EF_R2_PG0 PG Abnormal."                 },
    { 8376, "VoltageMajorFaultOnPCIeMntr86", "Power Good PWRGD_PVCCANA_CPU_R2_PF13 Abnormal."                },
    { 8377, "VoltageMajorFaultOnPCIeMntr87", "Power Good PG_PVCCIN_CPU_R_PH5 Abnormal."                      },
    { 8378, "VoltageMajorFaultOnPCIeMntr88", "Power Good PG_CPU_OUTLVC_PI8 Abnormal."                        },
    { 8379, "VoltageMajorFaultOnPCIeMntr89", "Power Good PG_PLTRST_PD12 Abnormal."                           },
    { 8380, "VoltageMajorFaultOnPCIeMntr90", "SDI Power up fail."                                            },
    { 8608, "VoltageMajorFaultOnPCIeMntr91",
        "Abnormal power down." },

    { 8501, "VoltageMajorFaultOnPCIeMntr92",
        "Voltage V_5V0_VRD_MOS_ADC exceeds the overvoltage threshold."   },
    { 8502, "VoltageMajorFaultOnPCIeMntr93",
        "Voltage V_5V0_VRD_MOS_ADC lower than the overvoltage threshold."},
    { 8503, "VoltageMajorFaultOnPCIeMntr94",
        "Voltage V_5V0_VRD_MOS_ADC get fail."                            },
    { 8504, "VoltageMajorFaultOnPCIeMntr95",
        "Voltage V_5V0_DRMOS_ADC exceeds the overvoltage threshold."     },
    { 8505, "VoltageMajorFaultOnPCIeMntr96",
        "Voltage V_5V0_DRMOS_ADC lower than the overvoltage threshold."  },
    { 8506, "VoltageMajorFaultOnPCIeMntr97",
        "Voltage V_5V0_DRMOS_ADC get fail."                              },
    { 8507, "VoltageMajorFaultOnPCIeMntr98",
        "Voltage V_2V5_VPP_AB_ADC exceeds the overvoltage threshold."    },
    { 8508, "VoltageMajorFaultOnPCIeMntr99",
        "Voltage V_2V5_VPP_AB_ADC lower than the overvoltage threshold." },
    { 8509, "VoltageMajorFaultOnPCIeMntr100",
        "Voltage V_2V5_VPP_AB_ADC get fail."                             },
    { 8510, "VoltageMajorFaultOnPCIeMntr101",
        "Voltage V_VCC_12V0_ADC exceeds the overvoltage threshold."      },
    { 8511, "VoltageMajorFaultOnPCIeMntr102",
        "Voltage V_VCC_12V0_ADC lower than the overvoltage threshold."   },
    { 8512, "VoltageMajorFaultOnPCIeMntr103",
        "Voltage V_VCC_12V0_ADC get fail."                               },
    { 8513, "VoltageMajorFaultOnPCIeMntr104",
        "Voltage V_VCC_12V_CABLE_ADC exceeds the overvoltage threshold." },
    { 8514, "VoltageMajorFaultOnPCIeMntr105",
        "Voltage V_VCC_12V_CABLE_ADC lower than the overvoltage threshold."  },
    { 8515, "VoltageMajorFaultOnPCIeMntr106",
        "Voltage V_VCC_12V_CABLE_ADC get fail."                              },
    { 8516, "VoltageMajorFaultOnPCIeMntr107",
        "Voltage V_VCC_12V_GOLD_ADC exceeds the overvoltage threshold."      },
    { 8517, "VoltageMajorFaultOnPCIeMntr108",
        "Voltage V_VCC_12V_GOLD_ADC lower than the overvoltage threshold."   },
    { 8518, "VoltageMajorFaultOnPCIeMntr109",
        "Voltage V_VCC_12V_GOLD_ADC get fail."                               },
    { 8519, "VoltageMajorFaultOnPCIeMntr110",
        "Voltage V_VCC_3V3_ADC exceeds the overvoltage threshold."           },
    { 8520, "VoltageMajorFaultOnPCIeMntr111",
        "Voltage V_VCC_3V3_ADC lower than the overvoltage threshold."        },
    { 8521, "VoltageMajorFaultOnPCIeMntr112",
        "Voltage V_VCC_3V3_ADC get fail."                                    },
    { 8522, "VoltageMajorFaultOnPCIeMntr113",
        "Voltage V_0V75_DVDD_FIX exceeds the overvoltage threshold."         },
    { 8523, "VoltageMajorFaultOnPCIeMntr114",
        "Voltage V_0V75_DVDD_FIX lower than the overvoltage threshold."      },
    { 8524, "VoltageMajorFaultOnPCIeMntr115",
        "Voltage V_0V75_DVDD_FIX get fail."                                  },
    { 8525, "VoltageMajorFaultOnPCIeMntr116",
        "Voltage V_0V75_DVDD_AVS exceeds the overvoltage threshold."         },
    { 8526, "VoltageMajorFaultOnPCIeMntr117",
        "Voltage V_0V75_DVDD_AVS lower than the overvoltage threshold."      },
    { 8527, "VoltageMajorFaultOnPCIeMntr118",
        "Voltage V_0V75_DVDD_AVS get fail."                                  },
    { 8528, "VoltageMajorFaultOnPCIeMntr119",
        "Voltage V_2V5_VPP_CD_ADC exceeds the overvoltage threshold."        },
    { 8529, "VoltageMajorFaultOnPCIeMntr120",
        "Voltage V_2V5_VPP_CD_ADC ower than the overvoltage threshold."      },
    { 8530, "VoltageMajorFaultOnPCIeMntr121",
        "Voltage V_2V5_VPP_CD_ADC get fail."                                 },
    { 8531, "VoltageMajorFaultOnPCIeMntr122",
        "Voltage V_2V5_VPP_B_ADC exceeds the overvoltage threshold."         },
    { 8532, "VoltageMajorFaultOnPCIeMntr123",
        "Voltage V_2V5_VPP_B_ADCC lower than the overvoltage threshold."     },
    { 8533, "VoltageMajorFaultOnPCIeMntr124",
        "Voltage V_2V5_VPP_B_ADC get fail."                                  },
    { 8534, "VoltageMajorFaultOnPCIeMntr125",
        "Voltage V_0V75_AVDD_SDS32 exceeds the overvoltage threshold."       },
    { 8535, "VoltageMajorFaultOnPCIeMntr126",
        "Voltage V_0V75_AVDD_SDS32 lower than the overvoltage threshold."    },
    { 8536, "VoltageMajorFaultOnPCIeMntr127",
        "Voltage V_0V75_AVDD_SDS32 get fail."                                },
    { 8537, "VoltageMajorFaultOnPCIeMntr128",
        "Voltage V_0V8_DVDD_DDR exceeds the overvoltage threshold."          },
    { 8538, "VoltageMajorFaultOnPCIeMntr129",
        "Voltage V_0V8_DVDD_DDR lower than the overvoltage threshold."       },
    { 8539, "VoltageMajorFaultOnPCIeMntr130",
        "Voltage V_0V8_DVDD_DDR get fail."                                   },
    { 8540, "VoltageMajorFaultOnPCIeMntr131",
        "Voltage V_1V2_AVDD_SDS_FIX exceeds the overvoltage threshold."      },
    { 8541, "VoltageMajorFaultOnPCIeMntr132",
        "Voltage V_1V2_AVDD_SDS_FIX lower than the overvoltage threshold."   },
    { 8542, "VoltageMajorFaultOnPCIeMntr133",
        "Voltage V_1V2_AVDD_SDS_FIX get fail."                               },
    { 8543, "VoltageMajorFaultOnPCIeMntr134",
        "Voltage V_0V8_AVDD_SDS60_FIX exceeds the overvoltage threshold."    },
    { 8544, "VoltageMajorFaultOnPCIeMntr135",
        "Voltage V_0V8_AVDD_SDS60_FIX lower than the overvoltage threshold." },
    { 8545, "VoltageMajorFaultOnPCIeMntr136",
        "Voltage V_0V8_AVDD_SDS60_FIX get fail."                             },
    { 8546, "VoltageMajorFaultOnPCIeMntr137",
        "Voltage V_1V2_CPU_DDR_VDDQ_CD exceeds the overvoltage threshold."   },
    { 8547, "VoltageMajorFaultOnPCIeMntr138",
        "Voltage V_1V2_CPU_DDR_VDDQ_CD lower than the overvoltage threshold."},
    { 8548, "VoltageMajorFaultOnPCIeMntr139",
        "Voltage V_1V2_CPU_DDR_VDDQ_CD get fail."                            },
    { 8549, "VoltageMajorFaultOnPCIeMntr140",
        "Voltage V_1V2_CPU_DDR_VDDQ_AB exceeds the overvoltage threshold."   },
    { 8550, "VoltageMajorFaultOnPCIeMntr141",
        "Voltage V_1V2_CPU_DDR_VDDQ_AB lower than the overvoltage threshold."},
    { 8551, "VoltageMajorFaultOnPCIeMntr142",
        "Voltage V_1V2_CPU_DDR_VDDQ_AB get fail."                            },
    { 8552, "VoltageMajorFaultOnPCIeMntr143",
        "Voltage V_1V2_CPU_DDR_VDDQ exceeds the overvoltage threshold."      },
    { 8553, "VoltageMajorFaultOnPCIeMntr144",
        "Voltage V_1V2_CPU_DDR_VDDQ lower than the overvoltage threshold."   },
    { 8554, "VoltageMajorFaultOnPCIeMntr145",
        "Voltage V_1V2_CPU_DDR_VDDQ get fail."                               },
    { 8555, "VoltageMajorFaultOnPCIeMntr146",
        "Voltage V_1V8_DVDD_1823E exceeds the overvoltage threshold."        },
    { 8556, "VoltageMajorFaultOnPCIeMntr147",
        "Voltage V_1V8_DVDD_1823E lower than the overvoltage threshold."     },
    { 8557, "VoltageMajorFaultOnPCIeMntr148",
        "Voltage V_1V8_DVDD_1823E get fail."                                 },
    { 8560, "VoltageMajorFaultOnPCIeMntr149",
        "Power Good PG_V_VCC_12V_CABLE Abnormal."                            },
    { 8561, "VoltageMajorFaultOnPCIeMntr150",
        "Power Good PG_V_VCC_12V_GOLD Abnormal."                             },
    { 8562, "VoltageMajorFaultOnPCIeMntr151",
        "Power Good PG_V_VCC_3V3 Abnormal."                                  },
    { 8563, "VoltageMajorFaultOnPCIeMntr152",
        "Power Good PG_V_3V3_VRD Abnormal."                                  },
    { 8564, "VoltageMajorFaultOnPCIeMntr153",
        "Power Good PG_V_VCC_1V8 Abnormal."                                  },
    { 8565, "VoltageMajorFaultOnPCIeMntr154",
        "Power Good PG_V_1V2_VDDQ_AB Abnormal."                              },
    { 8566, "VoltageMajorFaultOnPCIeMntr155",
        "Power Good PG_V_1V2_CPU_HVCC Abnormal."                             },
    { 8567, "VoltageMajorFaultOnPCIeMntr156",
        "Power Good PG_V_1V2_VDDQ_CD Abnormal."                              },
    { 8568, "VoltageMajorFaultOnPCIeMntr157",
        "Power Good PG_V_1V2_VDDQ Abnormal."                                 },
    { 8569, "VoltageMajorFaultOnPCIeMntr158",
        "Power Good PG_V_VCC_5V0 Abnormal."                                  },
    { 8570, "VoltageMajorFaultOnPCIeMntr159",
        "Power Good PG_V_0V8_DVDD_DDR Abnormal."                             },
    { 8571, "VoltageMajorFaultOnPCIeMntr160",
        "Power Good PG_V_0V75_DVDD_AVS Abnormal."                            },
    { 8572, "VoltageMajorFaultOnPCIeMntr161",
        "Power Good PG_V_2V5_VPP_AB Abnormal."                               },
    { 8573, "VoltageMajorFaultOnPCIeMntr162",
        "Power Good PG_V_2V5_VPP_CD Abnormal."                               },
    { 8574, "VoltageMajorFaultOnPCIeMntr163",
        "Power Good PG_V_0V75_DVDD_FIX Abnormal."                            },
    { 8575, "VoltageMajorFaultOnPCIeMntr164",
        "Power Good PG_V_0V6_VTT_AB Abnormal."                               },
    { 8576, "VoltageMajorFaultOnPCIeMntr165",
        "Power Good PG_V_0V6_VTT_CD Abnormal."                               },
    { 8577, "VoltageMajorFaultOnPCIeMntr166",
        "Power Good PG_AVDD_SDS_FIX_1V2 Abnormal."                           },
    { 8612, "VoltageMajorFaultOnPCIeMntr167",
        "Power Good PG_VCC_1V8_EFUSE Abnormal."                              },
    { 8613, "VoltageMajorFaultOnPCIeMntr168",
        "Power Good PG_V_VCC_1V1_WX1860 Abnormal."                           },
    { 8650, "VoltageMajorFaultOnPCIeMntr169",
        "V_VCC_3V3 power-on timeout."                                        },
    { 8651, "VoltageMajorFaultOnPCIeMntr170",
        "V_VCC_5V0 power-on timeout."                                        },
    { 8652, "VoltageMajorFaultOnPCIeMntr171",
        "V_VRD_3V3 power-on timeout."                                        },
    { 8653, "VoltageMajorFaultOnPCIeMntr172",
        "V_VCC_1V1_WX1860 power-on timeout."                                 },
    { 8654, "VoltageMajorFaultOnPCIeMntr173",
        "V_VCC_1V8 power-on timeout."                                        },
    { 8655, "VoltageMajorFaultOnPCIeMntr174",
        "V_2V5_VPP_AB power-on timeout."                                     },
    { 8656, "VoltageMajorFaultOnPCIeMntr175",
        "V_2V5_VPP_CD power-on timeout."                                     },
    { 8657, "VoltageMajorFaultOnPCIeMntr176",
        "V_0V8_DVDD_DDR power-on timeout."                                   },
    { 8658, "VoltageMajorFaultOnPCIeMntr177",
        "V_0V75_DVDD_AVS power-on timeout."                                  },
    { 8659, "VoltageMajorFaultOnPCIeMntr178",
        "V_0V75_DVDD_FIX power-on timeout."                                  },
    { 8660, "VoltageMajorFaultOnPCIeMntr179",
        "V_1V2_AVDD_SDS_FIX power-on timeout."                               },
    { 8661, "VoltageMajorFaultOnPCIeMntr180",
        "V_1V2_CPU_DDR_VDDQ_AB power-on timeout."                            },
    { 8662, "VoltageMajorFaultOnPCIeMntr181",
        "V_1V2_CPU_DDR_VDDQ_CD power-on timeout."                            },
    { 8663, "VoltageMajorFaultOnPCIeMntr182",
        "VCC_1V8_EFUSE power-on timeout."                                    },
    { 8664, "VoltageMajorFaultOnPCIeMntr183",
        "V_VTT_AB_0V6 power-on timeout."                                     },
    { 8665, "VoltageMajorFaultOnPCIeMntr184",
        "V_VTT_CD_0V6 power-on timeout."                                     },

    { 8341, "PowerConsumMajorFaultOnPCIe1", "The power consumption of SmartNIC exceeds threshold." },
    { 8342, "PowerConsumMajorFaultOnPCIe2", "The power consumption of SmartNIC gets fail."         },

    { 8348, "NetworkMinorFaultOnPCIeMntr1", "NIC bandwidth abnormal." },
    { 8349, "NetworkMinorFaultOnPCIeMntr2", "NIC speed abnormal."     },
    { 8347, "NetworkMajorFaultOnPCIeMntr", "NIC Not Present."        },
};


void recover_saved_value(OBJ_HANDLE obj_handle, const char *pro_name, DF_OPTIONS op)
{
    GVariant *property_value = NULL;
    gint32 ret;

    ret = dfl_get_saved_value_v2(obj_handle, pro_name, &property_value, op);
    if (ret == DFL_OK) {
        (void)dfl_set_property_value(obj_handle, pro_name, property_value, op);
        g_variant_unref(property_value);
        property_value = NULL;
    }

    return;
}


LOCAL void pcie_card_set_fault_state_by_fault_code_fpga(guint16 fault_code, guint8 *fault_state)
{
    switch (fault_code) {
            // PCIe标卡内存故障告警
        case 5121:
        case 5122:
        case 5123:
            (*fault_state) = (*fault_state) | 0x02;
            break;

            // 标准PCIE卡MCE/AER故障
            
            // case 5124:
            
            // case 5125:
        case 5126:
            (*fault_state) = (*fault_state) | 0x04;
            break;

            // PCIe标卡固件初始化异常告警
        case 5127:
            (*fault_state) = (*fault_state) | 0x08;
            break;

        default:

            // PCIe标卡硬件故障告警
            if (((fault_code >= 5128) && (fault_code <= 5138))) {
                (*fault_state) = (*fault_state) | 0x01;
            }
    }

    return;
}

LOCAL void pcie_card_confirm_fault_state_by_fault_code(guint16 fault_code, guint32 *fault_state)
{
    if (fault_state == NULL) {
        return;
    }
    size_t fault_map_length = G_N_ELEMENTS(g_fault_map_table);
    // 为了减小圈复杂度，方便故障码拓展，修改成查表形式
    for (size_t i = 0; i < fault_map_length; i++) {
        if (fault_code == g_fault_map_table[i].fault_code) {
            (*fault_state) = (*fault_state) | g_fault_map_table[i].fault_class_mask;
            return;
        }
    }

    debug_log(DLOG_INFO, "%s: invalid fault code: %d", __FUNCTION__, fault_code);
    return;
}

gint32 net_card_recover_device_status(OBJ_HANDLE obj_handle, gpointer user_data)
{
    gint32 ret = 0;
    guint8 default_value = 0;
    GVariant *property_data = NULL;
    if (user_data == NULL) {
        return RET_OK;
    }
    default_value = *(guint8 *)user_data;

    property_data = g_variant_new_byte(default_value);

    ret = dfl_set_property_value(obj_handle, PROPERTY_FDM_FAULT_STATE, property_data, DF_SAVE_TEMPORARY);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Set NET card FDM fault state default value failed: %d.", ret);
    }

    g_variant_unref(property_data);

    
    ret = dal_set_property_value_byte(obj_handle, PROPERTY_PFA_EVENT, default_value, DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set PFAEvent default value failed: %d.", ret);
    }

    

    return RET_OK;
}

void card_manage_update_fault_state(guint8 fault_state)
{
    gint32 ret = 0;
    OBJ_HANDLE warning_handle = 0;
    GSList *input_list = NULL;

    input_list = g_slist_append(input_list, g_variant_new_byte(fault_state));

    (void)dal_get_object_handle_nth(CLASS_WARNING, 0, &warning_handle);
    ret = dfl_call_class_method(warning_handle, METHOD_SETPCIESLOT_FAULTSTATE, NULL, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: dfl_call_class_method for fault state failed, ret is %d", __FUNCTION__, ret);
        return;
    }

    return;
}

gint32 card_manage_set_fdm_fault_state(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    GVariant *property_data = NULL;
    guint8 state = 0;
    gint32 ret = RET_OK;

    if (input_list == NULL) {
        return RET_ERR;
    }

    property_data = (GVariant *)g_slist_nth_data(input_list, 0);
    state = g_variant_get_byte(property_data);
    if ((state != 0) && (state != 1)) {
        debug_log(DLOG_ERROR, "Invalid parameter.");
        return RET_ERR;
    }

    ret = dfl_set_property_value(object_handle, PROPERTY_FDM_FAULT_STATE, property_data, DF_SAVE_TEMPORARY);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Set FDM fault state property failed: %d.", ret);
    }

    return ret;
}


LOCAL gint32 set_card_fault_state(OBJ_HANDLE object_handle, guint8 fault_state)
{
    _cleanup_gvariant_unref_ GVariant *value = NULL;
    gint32 ret = dfl_get_property_value(object_handle, PROPERTY_PCIE_CARD_FAULT_STATE, &value);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get %s fault state fail %d.", __FUNCTION__, dfl_get_object_name(object_handle), ret);
        return RET_ERR;
    }

    const gchar *type = g_variant_get_type_string(value);
    if (type == NULL) {
        debug_log(DLOG_ERROR, "%s : get type fail", __FUNCTION__);
        return RET_ERR;
    }

    switch (type[0]) {
        case G_VARIANT_CLASS_BYTE:
            ret = dal_set_property_value_byte(object_handle, PROPERTY_PCIE_CARD_FAULT_STATE, fault_state, DF_NONE);
            break;
        case G_VARIANT_CLASS_INT32:
            ret = dal_set_property_value_int32(object_handle, PROPERTY_PCIE_CARD_FAULT_STATE, fault_state, DF_NONE);
            break;
        case G_VARIANT_CLASS_UINT32:
            ret = dal_set_property_value_uint32(object_handle, PROPERTY_PCIE_CARD_FAULT_STATE, fault_state, DF_NONE);
            break;
        default:
            debug_log(DLOG_ERROR, "%s : %s fault state type %s is not support.", __FUNCTION__,
                      dfl_get_object_name(object_handle), type);
            return RET_ERR;
    }

    return ret;
}


gint32 card_manage_set_pcie_error_info(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret = 0;
    guint8 fault_state = 0;
    const gchar *fault_str = NULL;

    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "%s:input param null.", __FUNCTION__);
        return RET_ERR;
    }

    fault_state = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    fault_str = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 1), NULL);

    ret = set_card_fault_state(object_handle, fault_state);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, Set net card (%s) fault state failed, ret = %d.", __FUNCTION__,
            dfl_get_object_name(object_handle), ret);
        return RET_ERR;
    }

    if (strlen(fault_str) == 0) {
        ret = dal_set_property_value_string(object_handle, PROPERTY_PCIE_CARD_FAULT_CODE, "NA", DF_NONE);
    } else {
        ret = dal_set_property_value_string(object_handle, PROPERTY_PCIE_CARD_FAULT_CODE, fault_str, DF_NONE);
    }
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, Set net card (%s) fault code failed, ret = %d.", __FUNCTION__,
            dfl_get_object_name(object_handle), ret);
        return RET_ERR;
    }

    return RET_OK;
}

void ocp_card_update_health_info(OBJ_HANDLE obj_handle)
{
    
    recover_saved_value(obj_handle, PROPERTY_FDM_FAULT_STATE, DF_SAVE_TEMPORARY);

    
    recover_saved_value(obj_handle, PROPERTY_PFA_EVENT, DF_SAVE_TEMPORARY);

    return;
}


gint32 ocp_card_recover_device_status(OBJ_HANDLE object_handle, gpointer user_data)
{
    gint32 ret;
    guint8 default_value = 0;

    ret = dal_set_property_value_byte(object_handle, PROPERTY_FDM_FAULT_STATE, default_value, DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set OCP card FDM fault state default value failed: %d.", ret);
    }

    ret = dal_set_property_value_byte(object_handle, PROPERTY_PFA_EVENT, default_value, DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set OCP card PFAEvent state default value failed: %d.", ret);
    }

    ret = dal_set_property_value_byte(object_handle, PROPERTY_PCIECARD_DECREASE_BANDWIDTH, default_value,
        DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set pcie card DecreasedBandWidth default value failed, ret = %d", ret);
    }

    return RET_OK;
}

LOCAL gint32 pcie_card_set_fault_code_property(OBJ_HANDLE obj_handle, gchar *fault_code_str, guint16 length)
{
    gint32 ret;
    if (fault_code_str == NULL) {
        return RET_ERR;
    }
    if (strlen(fault_code_str) == 0) {
        (void)strncpy_s(fault_code_str, length, "NA", strlen("NA"));
    }
    ret = dal_set_property_value_string(obj_handle, PROPERTY_PCIE_CARD_FAULT_CODE, fault_code_str, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s (%d): Update pcie card fault code failed!", __FUNCTION__, __LINE__);
        return ret;
    }
    return RET_OK;
}

void pcie_card_update_fault_code_by_smbus(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    errno_t securec_rv = EOK;
    guint32 i = 0;
    gint32 ret = RET_OK;
    guint8 *buf = NULL;
    guint8 health_state = 0;
    guint32 buf_len = 0;
    guint16 fault_code = 0;
    gchar fault_code_str[FAULT_CODE_LEN] = {0};
    gchar fault_code_formated[FAULT_CODE_LEN] = {0};

    if (pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "%s (%d): input param error", __FUNCTION__, __LINE__);
        return;
    }

    
    if (pcie_card_mgnt_ready() != TRUE && !is_ocp3_card(obj_handle)) {
        (void)dal_set_property_value_byte(obj_handle, PROPERTY_PCIE_CARD_FAULT_STATE, 0, DF_NONE);
        return;
    }

    // 防止读不到时刷日志，采用debug级别
    ret = pm_get_health_state(pm_chip_info, &health_state, KEY_HEALTH_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s (%d): Get pcie card health state failed! ret = %d", __FUNCTION__, __LINE__, ret);
        return;
    }

    if (health_state == 0) {
        ret = dal_set_property_value_byte(obj_handle, PROPERTY_PCIE_CARD_FAULT_STATE, 0, DF_NONE);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s (%d): Update pcie card fault state failed!", __FUNCTION__, __LINE__);
            return;
        }
        return;
    }

    ret = pm_get_fault_code(pm_chip_info, &buf, &buf_len);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s (%d): Get pcie card fault code failed! ret = %d", __FUNCTION__, __LINE__, ret);
        return;
    }

    // 格式化输出故障码
    for (i = 0; i < buf_len - 1; i += 2) {
        fault_code = buf[i] | (buf[i + 1] << 8);
        if (fault_code == 0) {
            break;
        }

        (void)memset_s(fault_code_formated, sizeof(fault_code_formated), 0, sizeof(fault_code_formated));

        if (strlen(fault_code_str) == 0) {
            (void)snprintf_s(fault_code_formated, sizeof(fault_code_formated), sizeof(fault_code_formated) - 1, "%u",
                fault_code);
        } else {
            (void)snprintf_s(fault_code_formated, sizeof(fault_code_formated), sizeof(fault_code_formated) - 1, ", %u",
                fault_code);
        }

        securec_rv =
            strncat_s(fault_code_str, sizeof(fault_code_str), fault_code_formated, strlen(fault_code_formated));
        if (securec_rv != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d", __FUNCTION__, securec_rv);
        }

        debug_log(DLOG_DEBUG, "%s (%d): index: %d, str_fault: %s, fault code: %d!", __FUNCTION__, __LINE__, i,
            fault_code_str, fault_code);
    }

    ret = pcie_card_set_fault_code_property(obj_handle, fault_code_str, FAULT_CODE_LEN);
    if (ret != RET_OK) {
        g_free(buf);
        return;
    }

    ret = dal_set_property_value_byte(obj_handle, PROPERTY_PCIE_CARD_FAULT_STATE, health_state, DF_NONE);
    if (ret != RET_OK) {
        g_free(buf);
        debug_log(DLOG_ERROR, "%s (%d): Update pcie card fault state failed!", __FUNCTION__, __LINE__);
        return;
    }
    g_free(buf);

    return;
}

LOCAL gint32 pcie_card_check_if_ignore_fault_code(guint16 fault_code)
{
    gint32 codeflag = RET_ERR;
    switch (fault_code) {
        // 固件要求屏蔽5125、5124、5134、5120故障码
        case 5120:
        case 5124:
        case 5125:
        case 5134:
            codeflag = RET_OK;
            break;

        default:
            break;
    }

    return codeflag;
}

LOCAL void pcie_card_set_fault_state_by_fault_code_accesslerate(guint16 fault_code, guint8 *fault_state)
{
    switch (fault_code) {
        case 4096:
            (*fault_state) = (*fault_state) | 0x01;
            break;

        case 4097:
        case 4098:
            (*fault_state) = (*fault_state) | 0x02;
            break;

        case 4099:
        case 4100:
            (*fault_state) = (*fault_state) | 0x04;
            break;

        case 4101:
            (*fault_state) = (*fault_state) | 0x08;
            break;

        default:
            if (fault_code >= 4102 && fault_code <= 4112) {
                (*fault_state) = (*fault_state) | 0x10;
            }
    }

    return;
}


void pcie_card_update_fault_code(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    errno_t securec_rv = EOK;
#define CODE_LENGTH 16
#define FAULT_RETRY_TIME 60
    guint32 i = 0;
    gint32 ret = RET_OK;
    guint8 *buf = NULL;
    guint8 health_state = 0;
    guint32 buf_len = 0;
    guint8 fault_state = 0;
    guint8 old_fault_state = 0;
    guint16 fault_code = 0;
    gchar *str_fault = NULL;
    gchar code_buf[CODE_LENGTH] = {0};

    guint16 fault_code_min = 0;
    guint16 fault_code_max = 0;

    if (pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__);
        return;
    }

    // 防止读不到是刷日志，采用debug级别
    ret = pm_get_health_state(pm_chip_info, &health_state, KEY_HEALTH_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: Get pcie card health state failed", __FUNCTION__);
        return;
    }

    if (health_state == 0) {
        (void)memset_s(pm_chip_info->data_region_table.fault_retry_time,
            sizeof(pm_chip_info->data_region_table.fault_retry_time), 0,
            sizeof(pm_chip_info->data_region_table.fault_retry_time));

        (void)dal_set_property_value_string(obj_handle, PROPERTY_ACCELERATE_CARD_FAULT_CODE, "NA", DF_NONE);
        (void)dal_set_property_value_byte(obj_handle, PROPERTY_ACCELERATE_CARD_FAULT_STATE, 0, DF_NONE);
        return;
    }

    if (pm_chip_info->card_type < (sizeof(fault_code_range_tbl) / sizeof(FAULT_CODE_RANGE_S))) {
        fault_code_min = fault_code_range_tbl[pm_chip_info->card_type].fault_code_min;
        fault_code_max = fault_code_range_tbl[pm_chip_info->card_type].fault_code_max;
    }

    ret = pm_get_fault_code(pm_chip_info, &buf, &buf_len);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get pcie card fault code failed", __FUNCTION__);
        return;
    }

    str_fault = (gchar *)g_malloc0(STR_FAULT_CODE_LEN * sizeof(gchar));
    if (str_fault == NULL) {
        g_free(buf);
        debug_log(DLOG_ERROR, "%s: malloc failed", __FUNCTION__);
        return;
    }

    for (i = 0; i < buf_len - 1; i += 2) {
        fault_code = buf[i] | (buf[i + 1] << 8);
        
        // 固件要求屏蔽5125、5124、5134、5120故障码
        if (pcie_card_check_if_ignore_fault_code(fault_code) == RET_OK) {
            
            debug_log(DLOG_DEBUG, "%s: read fault code: %d", __FUNCTION__, fault_code);
            continue;
        }

        (void)memset_s(code_buf, CODE_LENGTH, 0, CODE_LENGTH);

        if (strlen(str_fault) == 0) {
            (void)snprintf_s(code_buf, CODE_LENGTH, CODE_LENGTH - 1, "%u", fault_code);
        } else {
            (void)snprintf_s(code_buf, CODE_LENGTH, CODE_LENGTH - 1, ", %u", fault_code);
        }

        if ((securec_rv = strncat_s(str_fault, STR_FAULT_CODE_LEN, code_buf, STR_FAULT_CODE_LEN - 1)) != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, return %d", __FUNCTION__, securec_rv);
        }

        
        if (fault_code < fault_code_min || fault_code > fault_code_max) {
            debug_log(DLOG_ERROR, "%s: fault code %u exceed range [%u, %u]", __FUNCTION__, fault_code, fault_code_min,
                fault_code_max);
            goto EXIT;
        }

        (pm_chip_info->data_region_table.fault_retry_time[fault_code - fault_code_min])++;

        if (pm_chip_info->data_region_table.fault_retry_time[fault_code - fault_code_min] < FAULT_RETRY_TIME) {
            continue;
        } else {
            // 防止一直加反转
            pm_chip_info->data_region_table.fault_retry_time[fault_code - fault_code_min] = FAULT_RETRY_TIME;
        }

        
        if (pm_chip_info->card_type == FPGA_ACCELERATE_CARD) {
            pcie_card_set_fault_state_by_fault_code_fpga(fault_code, &fault_state);
        } else if (pm_chip_info->card_type == COMPRESSION_CARD) {
            pcie_card_set_fault_state_by_fault_code_accesslerate(fault_code, &fault_state);
        }
    }

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_ACCELERATE_CARD_FAULT_STATE, &old_fault_state);

    debug_log(DLOG_DEBUG, "%s: str_fault: %s, fault state: %d, old state: %d", __FUNCTION__, str_fault, fault_state,
        old_fault_state);

    if (old_fault_state != fault_state) {
        (void)dal_set_property_value_string(obj_handle, PROPERTY_ACCELERATE_CARD_FAULT_CODE, str_fault, DF_NONE);
        (void)dal_set_property_value_byte(obj_handle, PROPERTY_ACCELERATE_CARD_FAULT_STATE, fault_state, DF_NONE);
    }

EXIT:
    g_free(str_fault);
    g_free(buf);
    return;
}


void pcie_card_set_pcieslot_fault_state(guint8 fault_state)
{
    G_LOCK(g_pcie_card_slot_fault_state);
    g_pcie_card_slot_fault_state = fault_state;
    G_UNLOCK(g_pcie_card_slot_fault_state);
}

guint8 pcie_card_get_pcieslot_fault_state()
{
    return g_pcie_card_slot_fault_state;
}

LOCAL void set_mcu_heartbeat_state(OBJ_HANDLE obj_handle, gint32 ret)
{
    static guint32 mcu_heartbeat_debounce = 0; // mcu心跳检测防抖
    guint32 heartbeat_state = MCU_HEART_BEAT_NORMAL;

    if (is_sdi_support_mcu(obj_handle) == FALSE) {
        return;
    }

    if (ret != RET_OK) { // 心跳异常
        mcu_heartbeat_debounce++;
        if (mcu_heartbeat_debounce < MCU_HEART_BEAT_DEBOUNCE_TIMES) {
            return; // 防抖次数不到,不更新
        }
        heartbeat_state = MCU_HEART_BEAT_ABNORMAL;
    } else {  // 心跳正常
        heartbeat_state = MCU_HEART_BEAT_NORMAL;
    }
    
    mcu_heartbeat_debounce = 0; // 防抖次数到或者心跳正常,清零

    guint32 fault_state = 0;
    if (dal_get_property_value_uint32(obj_handle, PROPERTY_PCIE_CARD_FAULT_STATE, &fault_state) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get pcie card fault state failed", __FUNCTION__);
        return;
    }
    guint32 ori_heartbeat_state = ((fault_state >> 31) & 0x1);
    if (ori_heartbeat_state == heartbeat_state) {
        return;
    }
    if (heartbeat_state == MCU_HEART_BEAT_NORMAL) {
        fault_state &= (~(1u << 31)); // 31-清零bit32
    } else {
        fault_state |= (1u << 31); // 31-置位bit32
    }
    (void)dal_set_property_value_uint32(obj_handle, PROPERTY_PCIE_CARD_FAULT_STATE, fault_state, DF_NONE);

    return;
}


gint32 update_fault_state(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info, guint8 *health_state,
    guint8 **fault_code_buf, guint32 *fault_code_buf_len)
{
    PROTOCOL_HEAD_PARA head_para = { 0 };
    gint32 ret = 0;

    if (pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "%s: pm_chip_info is null", __FUNCTION__);
        return RET_ERR;
    }

    if (health_state == NULL || fault_code_buf == NULL || fault_code_buf_len == NULL) {
        debug_log(DLOG_ERROR, "%s: input para error", __FUNCTION__);
        return RET_ERR;
    }

    if (PCIE_MGNT_PROTOCOL_STD_SMBUS == pm_chip_info->inf_protocol) {
        (void)memset_s(&head_para, sizeof(PROTOCOL_HEAD_PARA), 0, sizeof(PROTOCOL_HEAD_PARA));
        head_para.opcode = REGISTER_KEY_HEALTH;
        ret = pm_get_fixed_len_data_by_std_smbus(pm_chip_info, (guint8 *)&head_para, health_state, KEY_HEALTH_LEN);
        set_mcu_heartbeat_state(obj_handle, ret);
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "%s: get pcie card health state failed, ret = %d", __FUNCTION__, ret);
            return ret;
        }

        if (*health_state != 0) {
            (void)memset_s(&head_para, sizeof(PROTOCOL_HEAD_PARA), 0, sizeof(PROTOCOL_HEAD_PARA));
            head_para.opcode = REGISTER_KEY_ERROR_CODE;
            ret = pm_get_random_len_data_by_std_smbus(pm_chip_info, (guint8 *)&head_para, fault_code_buf,
                fault_code_buf_len);
            if (ret != RET_OK || (*fault_code_buf) == NULL) {
                debug_log(DLOG_DEBUG, "%s: get pcie card fault code failed, ret = %d", __FUNCTION__, ret);
                return ret;
            }
        }
    } else if (PCIE_MGNT_PROTOCOL_SMBUS == pm_chip_info->inf_protocol) {
        ret = pm_get_health_state(pm_chip_info, health_state, KEY_HEALTH_LEN);
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "%s (%d): Get pcie card health state failed! ret = %d", __FUNCTION__, __LINE__,
                ret);
            return ret;
        }

        if (*health_state != 0) {
            ret = pm_get_fault_code(pm_chip_info, fault_code_buf, fault_code_buf_len);
            if (ret != RET_OK || (*fault_code_buf) == NULL) {
                debug_log(DLOG_DEBUG, "%s (%d): Get pcie card fault code failed! ret = %d", __FUNCTION__, __LINE__,
                    ret);
                return ret;
            }
        }
    }

    return RET_OK;
}

void pcie_card_clear_fault_code(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    gint32 ret = 0;
    gchar slave_card_class_name[CLASSE_NAME_LEN] = {0};
    OBJ_HANDLE slave_obj_handle = 0;

    (void)dal_get_property_value_string(obj_handle, PROPERTY_PCIE_CARD_SLAVE_CARD, slave_card_class_name,
        sizeof(slave_card_class_name));

    if (strlen(slave_card_class_name) != 0) {
        ret = dal_get_specific_object_position(obj_handle, slave_card_class_name, &slave_obj_handle);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s (%d): Get pcie card slave card object handle failed!", __FUNCTION__, __LINE__);
            return;
        }

        (void)memset_s(pm_chip_info->data_region_table.fault_retry_time,
            sizeof(pm_chip_info->data_region_table.fault_retry_time), 0,
            sizeof(pm_chip_info->data_region_table.fault_retry_time));

        ret = dal_set_property_value_string(slave_obj_handle, PROPERTY_ACCELERATE_CARD_FAULT_CODE, "NA", DF_NONE);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s (%d): Update pcie card fault code failed!", __FUNCTION__, __LINE__);
            return;
        }

        ret = dal_set_property_value_byte(slave_obj_handle, PROPERTY_ACCELERATE_CARD_FAULT_STATE, 0, DF_NONE);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s (%d): Update pcie card fault state failed!", __FUNCTION__, __LINE__);
            return;
        }
    }

    return;
}


void pcie_card_get_info_fail(OBJ_HANDLE obj_handle)
{
    gint32 ret = 0;
    gchar slave_card_class_name[CLASSE_NAME_LEN] = {0};
    OBJ_HANDLE slave_obj_handle = 0;
    guint8 fault_state = 0;

    (void)dal_get_property_value_string(obj_handle, PROPERTY_PCIE_CARD_SLAVE_CARD, slave_card_class_name,
        sizeof(slave_card_class_name));

    if (strlen(slave_card_class_name) != 0) {
        ret = dal_get_specific_object_position(obj_handle, slave_card_class_name, &slave_obj_handle);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s (%d): Get pcie card slave card object handle failed!", __FUNCTION__, __LINE__);
            return;
        }

        ret = dal_get_property_value_byte(obj_handle, PROPERTY_ACCELERATE_CARD_FAULT_STATE, &fault_state);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s (%d): Get pcie card fault state failed!", __FUNCTION__, __LINE__);
            return;
        }

        fault_state = fault_state | 0x80;

        ret = dal_set_property_value_byte(slave_obj_handle, PROPERTY_ACCELERATE_CARD_FAULT_STATE, fault_state, DF_NONE);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s (%d): Update pcie card fault state failed!", __FUNCTION__, __LINE__);
            return;
        }
    }

    return;
}


LOCAL void update_pcieslot_fault_state(OBJ_HANDLE object_handle, PM_CHIP_INFO *pm_chip_info, guint32 fault_state_mask)
{
    guint8 card_fault_flag = 0;
    guint32 fault_state = 0;

    if (object_handle == 0 || pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "%s: obj handle is null", __FUNCTION__);
        return;
    }

    (void)dal_get_property_value_uint32(object_handle, PROPERTY_PCIE_CARD_FAULT_STATE, &fault_state);

    // 更新系统健康状态
    card_fault_flag = (fault_state & fault_state_mask) ? 1 : 0;
    pm_chip_info->pcieslot_fault_state = card_fault_flag;

    return;
}


void clear_pcie_card_fault(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    gint32 ret = 0;

    if (obj_handle == 0) {
        debug_log(DLOG_ERROR, "%s: object handle is invalid", __FUNCTION__);
        return;
    }

    ret = dal_set_property_value_string(obj_handle, PROPERTY_PCIE_CARD_FAULT_CODE, "Error code: NA", DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: clear pcie card fault code failed", __FUNCTION__);
        return;
    }

    if (pm_chip_info == NULL) {
        ret = dal_set_property_value_uint32(obj_handle, PROPERTY_PCIE_CARD_FAULT_STATE, 0, DF_NONE);
    } else {
        ret = dal_set_property_value_uint32(obj_handle, PROPERTY_PCIE_CARD_FAULT_STATE,
            pm_chip_info->fault_state_from_sms, DF_NONE);
        clear_pcie_card_fault_from_mcu(obj_handle, pm_chip_info, is_sdi_support_mcu(obj_handle));
    }

    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: update pcie card fault state failed", __FUNCTION__);
    }

    return;
}


gint32 update_fault_code(OBJ_HANDLE object_handle, PM_CHIP_INFO *pm_chip_info, guint8 *buf, guint32 buf_len,
    guint32 fault_state)
{
#define FAULT_CODE_INWORD 2
    errno_t securec_rv = EOK;
    guint32 code_index = 0;
    guint16 fault_code = 0;
    gchar fault_code_buf[FAULT_CODE_LEN] = {0};
    gchar fault_code_str[STR_FAULT_CODE_LEN] = {0};
    guint16 fault_code_min = 0;
    guint16 fault_code_max = 0;
    guint32 old_fault_state = 0;

    if (pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "%s: parameter is null", __FUNCTION__);
        return RET_ERR;
    }

    if (pm_chip_info->card_type < (sizeof(fault_code_range_tbl) / sizeof(FAULT_CODE_RANGE_S))) {
        fault_code_min = fault_code_range_tbl[pm_chip_info->card_type].fault_code_min;
        fault_code_max = fault_code_range_tbl[pm_chip_info->card_type].fault_code_max;
    }

    if (buf_len < FAULT_CODE_INWORD) {
        debug_log(DLOG_ERROR, "%s: fault code buffer len %d is invalid", __FUNCTION__, buf_len);
        return RET_ERR;
    }

    (void)memset_s(fault_code_str, sizeof(fault_code_str), 0, sizeof(fault_code_str));

    // 格式化输出故障码,解析电压相关的故障码
    for (code_index = 0; code_index < buf_len - 1; code_index += FAULT_CODE_INWORD) {
        fault_code = buf[code_index] | (buf[code_index + 1] << 8);
        if (fault_code == 0) {
            break;
        }

        if (fault_code < fault_code_min || fault_code > fault_code_max) {
            debug_log(DLOG_DEBUG, "%s: fault code %u excced range [%u, %u]", __FUNCTION__, fault_code, fault_code_min,
                fault_code_max);
            continue;
        }

        (void)memset_s(fault_code_buf, sizeof(fault_code_buf), 0, sizeof(fault_code_buf));

        if (strlen(fault_code_str) == 0) {
            // 兼容所有告警描述，复用动态参数，因此进行格式为Error code:***
            (void)snprintf_s(fault_code_buf, sizeof(fault_code_buf), sizeof(fault_code_buf) - 1, "Error code: %u",
                fault_code);
        } else {
            (void)snprintf_s(fault_code_buf, sizeof(fault_code_buf), sizeof(fault_code_buf) - 1, ", %u", fault_code);
        }

        securec_rv = strncat_s(fault_code_str, STR_FAULT_CODE_LEN, fault_code_buf, strlen(fault_code_buf));
        if (securec_rv != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, return %d", __FUNCTION__, securec_rv);
            return RET_ERR;
        }

        // 故障码解析
        pcie_card_confirm_fault_state_by_fault_code(fault_code, &fault_state);
    }

    fault_state |= pm_chip_info->fault_state_from_sms;
    (void)dal_get_property_value_uint32(object_handle, PROPERTY_PCIE_CARD_FAULT_STATE, &old_fault_state);
    debug_log(DLOG_DEBUG, "%s: fault code: %s, fault state from OOB: 0x%x, fault state from sms: 0x%x, old state: 0x%x",
        __FUNCTION__, fault_code_str, fault_state, pm_chip_info->fault_state_from_sms, old_fault_state);

    // 故障状态更新，上报新的故障码
    if (old_fault_state != fault_state) {
        if (strlen(fault_code_str) == 0) {
            (void)dal_set_property_value_string(object_handle, PROPERTY_PCIE_CARD_FAULT_CODE, "Error code: NA",
                DF_NONE);
        } else {
            (void)dal_set_property_value_string(object_handle, PROPERTY_PCIE_CARD_FAULT_CODE, fault_code_str, DF_NONE);
        }

        (void)dal_set_property_value_uint32(object_handle, PROPERTY_PCIE_CARD_FAULT_STATE, fault_state, DF_NONE);
    }

    return RET_OK;
}

// 检查故障码fault_code是否在表中
LOCAL FAULT_CODE_DESC_EVENT_MAP_S *get_fault_code_desc_event_from_table(guint16 fault_code)
{
    size_t front = 0;
    size_t rear = sizeof(g_fault_code_desc_event_table) / sizeof(g_fault_code_desc_event_table[0]) - 1;
    while (front < rear) {
        if (g_fault_code_desc_event_table[front].fault_code == fault_code) {
            return &g_fault_code_desc_event_table[front];
        }
        if (g_fault_code_desc_event_table[rear].fault_code == fault_code) {
            return &g_fault_code_desc_event_table[rear];
        }
        front++;
        rear--;
    }
    return (g_fault_code_desc_event_table[front].fault_code ==
        fault_code) ? &g_fault_code_desc_event_table[front] : NULL;
}

// 上报告警函数，根据current_state状态, des_param 为描述信息的数组
LOCAL gint32 post_event_on_event_monitor(const gchar *obj_name, guint8 current_state, const gchar *des_param[],
    gint32 des_param_num)
{
    gint32 ret = MODULE_OK;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
 
    debug_log(DLOG_INFO, "%s: input parameters are [%s], state[%u].", __FUNCTION__, obj_name, current_state);

    ret = dal_get_object_handle_nth(CLASS_EVENT_CONFIGURATION, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "[%s]Get %s obj_handle failed.", __FUNCTION__, CLASS_EVENT_CONFIGURATION);
        return ret;
    }

    input_list = g_slist_append(input_list, g_variant_new_string(obj_name));
    input_list = g_slist_append(input_list, g_variant_new_byte(current_state));
    input_list = g_slist_append(input_list, g_variant_new_strv((const gchar * const *)des_param, des_param_num));

    ret = dfl_call_class_method(obj_handle, METHOD_EVENT_CONFIGURATION_POST_EVENT_PARA, NULL, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
 
    if (ret != DFL_OK) {
        debug_log(DLOG_INFO, "[%s]event method (%s) return failed, ret=%d.", __FUNCTION__,
            METHOD_EVENT_CONFIGURATION_POST_EVENT, ret);
        return ret;
    }
    return ret;
}

LOCAL gint32 get_slod_id_card_desc(OBJ_HANDLE obj_handle, gchar *slot_id_str, gint32 slot_id_len,
    gchar *card_desc, gint32 card_desc_len)
{
    guchar slot_id = MAX_SLOT_COUNT + 1; // 赋值超过最大值,没获取到时,可以及时退出
    (void)dal_get_property_value_byte(obj_handle, PROPETRY_PCIECARD_SLOT, &slot_id);
    if (slot_id >= MAX_SLOT_COUNT) {
        debug_log(DLOG_ERROR, "%s: obj[%s] slot_id [%u] is out of range.",
            __FUNCTION__, dfl_get_object_name(obj_handle), slot_id);
        return RET_ERR;
    }
    (void)snprintf_s(slot_id_str, slot_id_len, slot_id_len - 1, "%u", slot_id);

    gchar tmp[MAX_PCIE_CARD_DESC_LEN] = {0};
    (void)dal_get_property_value_string(obj_handle, PROPERTY_PCIECARD_DESC, tmp, sizeof(tmp));
    gint32 ret = snprintf_s(card_desc, card_desc_len, card_desc_len - 1, "(%s)", tmp);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: obj[%s] snprintf_s fail, ret = %d.",
            __FUNCTION__, dfl_get_object_name(obj_handle), ret);
    }

    return RET_OK;
}
LOCAL gint32 get_slave_card_handle(OBJ_HANDLE obj_handle, OBJ_HANDLE *slave_obj_handle)
{
    gchar slave_card_class_name[MAX_NAME_SIZE] = {0};
    // 获取slave card对象
    (void)dal_get_property_value_string(obj_handle, PROPERTY_PCIE_CARD_SLAVE_CARD, slave_card_class_name,
        sizeof(slave_card_class_name));

    if (strlen(slave_card_class_name) == 0) {
        debug_log(DLOG_ERROR, "%s: get pcie card slave card class name failed", __FUNCTION__);
        return RET_ERR;
    }
    gint32 ret = dal_get_specific_object_position(obj_handle, slave_card_class_name, slave_obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get pcie card slave card object handle failed", __FUNCTION__);
    }
    return ret;
}

// 重启后, 第一次进入时,获取prev_bitmap
LOCAL void get_prev_fault_code(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    // 数组最后一位的最高位,用来标记是否已经取过数据了,仅重启后取一次
    if (GET_BIT(pm_chip_info->prev_bitmap, (MAX_FAULT_CODE_NUM - 1)) == 1) {
        return;
    }

    OBJ_HANDLE slave_obj_handle = 0;
    if (get_slave_card_handle(obj_handle, &slave_obj_handle) != RET_OK) {
        return;
    }

    GVariant *property_value = NULL;
    gint32 ret = dfl_get_property_value(slave_obj_handle, PROPERTY_SDI_FAULT_CODE_BITMAP, &property_value);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s:get %s of %s failed.", __FUNCTION__,
            PROPERTY_SDI_FAULT_CODE_BITMAP, dfl_get_object_name(slave_obj_handle));
        return;
    }
    gsize n_elements = 0;
    const guint32 *code_bitmap = (const guint32 *)g_variant_get_fixed_array(property_value,
        &n_elements, sizeof(guint32));
    guint32 bitmap_len = (guint32)n_elements;

    if (code_bitmap == NULL) {
        g_variant_unref(property_value);
        debug_log(DLOG_ERROR, "%s: %s of %s array is NULL.", __FUNCTION__,
            PROPERTY_SDI_FAULT_CODE_BITMAP, dfl_get_object_name(slave_obj_handle));
        return;
    }

    gint32 securec_rv = memcpy_s((guint32*)(pm_chip_info->prev_bitmap), sizeof(pm_chip_info->prev_bitmap),
        (const void *)code_bitmap, bitmap_len * sizeof(guint32));
    if (securec_rv != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, securec_rv = %d\n", __FUNCTION__, securec_rv);
        g_variant_unref(property_value);
        return;
    }

    SET_BIT(pm_chip_info->prev_bitmap, (MAX_FAULT_CODE_NUM - 1)); // 标记已取过持久化的值

    g_variant_unref(property_value);
    return;
}

// 更新code_bitmap到"FaultCodeBitMap"中
LOCAL void update_fault_code_bitmap(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info, gboolean need_update)
{
    if (need_update == FALSE) {
        return;
    }
    size_t bitmap_len = sizeof(pm_chip_info->prev_bitmap) / sizeof(guint32);
    OBJ_HANDLE slave_obj_handle = 0;
    if (get_slave_card_handle(obj_handle, &slave_obj_handle) != RET_OK) {
        return;
    }

    CLEAR_BIT(pm_chip_info->prev_bitmap, (MAX_FAULT_CODE_NUM - 1)); // 不持久化该bit位

    GVariant *property_value =
        g_variant_new_fixed_array(G_VARIANT_TYPE_UINT32, (gconstpointer)(pm_chip_info->prev_bitmap),
            bitmap_len, sizeof(guint32));
    gint32 ret = dfl_set_property_value(slave_obj_handle, PROPERTY_SDI_FAULT_CODE_BITMAP, property_value, DF_SAVE);
    g_variant_unref(property_value);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s:set %s of %s error,ret=%d.", __FUNCTION__,
            PROPERTY_SDI_FAULT_CODE_BITMAP, dfl_get_object_name(slave_obj_handle), ret);
    }

    SET_BIT(pm_chip_info->prev_bitmap, (MAX_FAULT_CODE_NUM - 1)); // 标记已取过值
    return;
}

// 检查前后2次故障码列表变化情况,有更新时上报告警
LOCAL void chk_bitmap_and_post_event(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info,
    FAULT_CODE_RANGE_S *fault_code_range, guint32 *cur_bitmap)
{
    gchar slot_id_str[8] = {0};
    gchar card_desc[MAX_PCIE_CARD_DESC_LEN] = {0};
    if (get_slod_id_card_desc(obj_handle, slot_id_str, sizeof(slot_id_str), card_desc, sizeof(card_desc)) != RET_OK) {
        return;
    }
    // 首次进入标志
    gboolean fisrt_flag = (GET_BIT(pm_chip_info->prev_bitmap, (MAX_FAULT_CODE_NUM - 1)) == 0) ? TRUE : FALSE;
    get_prev_fault_code(obj_handle, pm_chip_info);
    
    gboolean need_update = FALSE;
    for (guint32 i = 0; i < (MAX_FAULT_CODE_NUM >> 5); i++) { // 右移5,计算bitmap数组长度
        // 首次进入,只有等于0不处理,其余都要处理
        if ((fisrt_flag == FALSE && cur_bitmap[i] == pm_chip_info->prev_bitmap[i]) ||
            (fisrt_flag == TRUE && cur_bitmap[i] == pm_chip_info->prev_bitmap[i] && cur_bitmap[i] == 0)) {
            debug_log(DLOG_INFO, "%s: bitmap[%d]: prev[%u],cur[%u]", __FUNCTION__, i,
                pm_chip_info->prev_bitmap[i], cur_bitmap[i]);
            continue;
        }
        guint32 base = (i << 5); // 左移5,bit位基数
        if ((fault_code_range->fault_code_min + base) > fault_code_range->fault_code_max) {
            debug_log(DLOG_INFO, "%s: fault_code [%u] out of max range[%u]",
                __FUNCTION__, base, fault_code_range->fault_code_max);
            break;
        }

        for (guint32 bit = 0; bit < 32; bit++) { // 每个uint32 有 32bit
            guint8 cur = GET_BIT(cur_bitmap, (base + bit));
            guint8 prev = GET_BIT(pm_chip_info->prev_bitmap, (base + bit));
            // 首次进入,只有等于0不处理,其余都要处理
            if ((fisrt_flag == FALSE && cur == prev) || (fisrt_flag == TRUE && cur == prev && cur == 0)) {
                continue;
            }
            // 当前bitmap中存在,则上报告警; 在上一次bitmap中存在,则消除告警
            guint8 state = (cur == 1) ? EVENT_OBJ_STATE_ASSERT : EVENT_OBJ_STATE_DEASSERT;
            guint16 fault_code = fault_code_range->fault_code_min + base + bit;
            debug_log(DLOG_INFO, "%s: fault_code is [%u]...", __FUNCTION__, fault_code);

            FAULT_CODE_DESC_EVENT_MAP_S *fault_code_desc_event = get_fault_code_desc_event_from_table(fault_code);
            if (fault_code_desc_event == NULL) {
                debug_log(DLOG_ERROR, "%s: fault code[%u] is not supported.", __FUNCTION__, fault_code);
                continue;
            }

            gchar fault_code_str[FAULT_CODE_LEN] = {0};
            (void)snprintf_s(fault_code_str, sizeof(fault_code_str),
                sizeof(fault_code_str) - 1, "Error Code: %u", fault_code);

            const gchar *buf[] = {slot_id_str, card_desc, fault_code_desc_event->event_desc_en, fault_code_str};

            debug_log(DLOG_INFO, "Add_info: %s %s %s ........", slot_id_str, card_desc, fault_code_str);
            post_event_on_event_monitor(fault_code_desc_event->event_monitor_obj_name,
                state, buf, sizeof(buf) / sizeof(buf[0]));
            need_update = TRUE;
        }
        pm_chip_info->prev_bitmap[i] = cur_bitmap[i];
    }
    update_fault_code_bitmap(obj_handle, pm_chip_info, need_update);

    return;
}

LOCAL FAULT_CODE_RANGE_S *get_fault_code_range(guint8 card_type)
{
    if (card_type < (sizeof(fault_code_range_tbl) / sizeof(FAULT_CODE_RANGE_S))) {
        return &fault_code_range_tbl[card_type];
    }
    debug_log(DLOG_DEBUG, "%s: card_type[%x]", __FUNCTION__, card_type);

    return NULL;
}

// 更新从SDI5.1获取到的故障码，并上报告警
LOCAL gint32 update_fault_code_from_mcu(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info, guint8 *buf, guint32 buf_len)
{
    guint32 cur_bitmap[MAX_FAULT_CODE_NUM >> 5] = {0};   // 右移5,需要的uint32数组长度

    if (buf_len < FAULT_CODE_INWORD) {
        debug_log(DLOG_ERROR, "%s: input parameters are invalid.", __FUNCTION__);
        return RET_ERR;
    }

    debug_log(DLOG_DEBUG, "%s: fault code len is %u", __FUNCTION__, buf_len);

    FAULT_CODE_RANGE_S *fault_code_range = get_fault_code_range(pm_chip_info->card_type);
    if (fault_code_range == NULL) {
        return RET_ERR;
    }

    if (fault_code_range->fault_code_max - fault_code_range->fault_code_min >= MAX_FAULT_CODE_NUM) {
        debug_log(DLOG_DEBUG, "%s: MAX_FAULT_CODE_NUM is too short [%u].", __FUNCTION__, MAX_FAULT_CODE_NUM);
        return RET_ERR;
    }

    debug_log(DLOG_INFO, "%s: begin to parse fault_code...", __FUNCTION__);

    // 解析故障码,设置bitmap
    for (guint32 code_index = 0; code_index < buf_len - 1; code_index += FAULT_CODE_INWORD) {
        guint16 fault_code = buf[code_index] | (buf[code_index + 1] << 8);
        if (fault_code == 0) {
            break;
        }

        if (fault_code < fault_code_range->fault_code_min || fault_code > fault_code_range->fault_code_max) {
            debug_log(DLOG_DEBUG, "%s: fault code %u excced range [%u, %u]", __FUNCTION__, fault_code,
                fault_code_range->fault_code_min, fault_code_range->fault_code_max);
            continue;
        }
        debug_log(DLOG_INFO, "%s: fault_code %u...", __FUNCTION__, fault_code);

        guint32 index = fault_code - fault_code_range->fault_code_min; // 在bitmap中的位置
        SET_BIT(cur_bitmap, index);
    }

    chk_bitmap_and_post_event(obj_handle, pm_chip_info, fault_code_range, cur_bitmap);

    return RET_OK;
}

// 清除mcu的所有告警, pm_chip_info由调用者保障合法性,函数中不重复检查
LOCAL void clear_pcie_card_fault_from_mcu(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info, gboolean sdi_support_mcu)
{
    if (obj_handle == 0 || sdi_support_mcu == FALSE) {
        debug_log(DLOG_DEBUG, "%s: object handle is invalid or not support_mcu[%d]", __FUNCTION__, sdi_support_mcu);
        return;
    }

    guint32 cur_bitmap[MAX_FAULT_CODE_NUM >> 5] = {0};   // 右移5,需要的uint32数组长度
    FAULT_CODE_RANGE_S *fault_code_range = get_fault_code_range(pm_chip_info->card_type);
    if (fault_code_range == NULL) {
        return;
    }
    chk_bitmap_and_post_event(obj_handle, pm_chip_info, fault_code_range, cur_bitmap);

    return;
}


void pcie_card_update_fault_code_by_std_smbus(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    guint32 health_state = 0;
    guint8 *buf = NULL;
    guint32 buf_len = 0;
    PROTOCOL_HEAD_PARA head_para = { 0 };

    if (pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__);
        return;
    }

    
    if (pcie_card_mgnt_ready() != TRUE && is_sdi_support_mcu(obj_handle) == FALSE) {
        clear_pcie_card_fault(obj_handle, NULL);
        pm_chip_info->fault_state_from_sms = 0;
        return;
    }

    (void)memset_s(&head_para, sizeof(PROTOCOL_HEAD_PARA), 0, sizeof(PROTOCOL_HEAD_PARA));
    head_para.opcode = REGISTER_KEY_HEALTH;
    gint32 ret =
        pm_get_fixed_len_data_by_std_smbus(pm_chip_info, (guint8 *)&head_para, (guint8 *)&health_state, KEY_HEALTH_LEN);
    set_mcu_heartbeat_state(obj_handle, ret);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get pcie card health state failed! ret = %d", __FUNCTION__, ret);
        return;
    }

    if (health_state == 0) {
        clear_pcie_card_fault(obj_handle, pm_chip_info);
        return;
    }

    // PCIE卡的故障码获取及解析
    (void)memset_s(&head_para, sizeof(PROTOCOL_HEAD_PARA), 0, sizeof(PROTOCOL_HEAD_PARA));
    head_para.opcode = REGISTER_KEY_ERROR_CODE;
    if ((ret = pm_get_random_len_data_by_std_smbus(pm_chip_info, (guint8 *)&head_para, &buf, &buf_len)) != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get pcie card fault code failed! ret = %d", __FUNCTION__, ret);
        return;
    }
    if (buf == NULL) {
        debug_log(DLOG_DEBUG, "%s: card obj(%s) faul code is NULL", __FUNCTION__, dfl_get_object_name(obj_handle));
        return;
    }
    if (is_sdi_support_mcu(obj_handle) == TRUE) { // 支持mcu的sdi
        ret = update_fault_code_from_mcu(obj_handle, pm_chip_info, buf, buf_len);
    } else {
        ret = update_fault_code(obj_handle, pm_chip_info, buf, buf_len, health_state);
    }
    g_free(buf);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: update pcie card fault code failed", __FUNCTION__);
        return;
    }
    // 判断PCIE卡异常状态:过温、过流
    update_pcieslot_fault_state(obj_handle, pm_chip_info, OVER_TEMP_MASK | OVER_CURRENT_MASK);

    return;
}


void pcie_card_update_info_fail(OBJ_HANDLE obj_handle)
{
    gint32 ret = 0;
    guint32 fault_state = 0;

    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_PCIE_CARD_FAULT_STATE, &fault_state);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get pcie card fault state failed", __FUNCTION__);
        return;
    }

    fault_state = fault_state | 0x4;

    ret = dal_set_property_value_uint32(obj_handle, PROPERTY_PCIE_CARD_FAULT_STATE, fault_state, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: update pcie card fault state failed", __FUNCTION__);
        return;
    }

    return;
}


gint32 pcie_card_set_device_status(OBJ_HANDLE handle, gpointer data)
{
    gint32 ret = 0;
    guint8 bus = 0;
    guint8 device = 0;
    guint8 funcition = 0;
    guint8 *device_status = NULL;
    OBJ_HANDLE obj_handle = 0;
    guchar out_val = 0;
    DEVICE_STATUS_S *device_status_param = (DEVICE_STATUS_S *)data;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;

    if (device_status_param == NULL || device_status_param->device_status == NULL ||
        device_status_param->length > DEVICE_STATUS_LEN) {
        debug_log(DLOG_ERROR, "Invalid input param!");
        return RET_ERR;
    }
    device_status = device_status_param->device_status;

    
    property_name_list = g_slist_append(property_name_list, PROPERTY_PCIE_BUS);
    property_name_list = g_slist_append(property_name_list, PROPERTY_PCIE_DEVICE);
    property_name_list = g_slist_append(property_name_list, PROPERTY_PCIE_FUNCTION);

    ret = dfl_multiget_property_value(handle, property_name_list, &property_value);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get PcieAddrInfo failed.");
        g_slist_free(property_name_list);
        return RET_ERR;
    }

    bus = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 0));
    device = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 1));
    funcition = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 2));

    debug_log(DLOG_INFO, "Get PcieAddrInfo success bus=%d,device=%d,funcition=%d", bus, device, funcition);

    g_slist_free(property_name_list);
    g_slist_free_full(property_value, (GDestroyNotify)g_variant_unref);

    if ((bus != device_status[1]) || (device != device_status[2]) || (funcition != device_status[3])) {
        return RET_OK;
    }

    debug_log(DLOG_INFO, "Find the error pcie slot: bus=%d,device=%d,funcition=%d", bus, device, funcition);

    ret = dal_get_property_value_byte(handle, PROPERTY_PCIE_SLOT_ID, &out_val);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get PROPERTY_PCIE_SLOT_ID failed");
        return RET_ERR;
    }

    ret = dal_get_specific_object_byte(CLASS_PCIECARD_NAME, PROPETRY_PCIECARD_SLOT, out_val, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get specific_object pciecard failed");
        return RET_ERR;
    }

    ret = pcie_card_set_obj_property(obj_handle, PROPERTY_PCIECARD_FATALERROR, (const void *)(&device_status[6]),
        PCIE_CARD_P_TYPE_BYTE, DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set pcie card FATALERROR failed!");
        return RET_ERR;
    }

    debug_log(DLOG_INFO, "set PROPERTY_PCIECARD_FATALERROR successfully");
    return RET_OK;
}

LOCAL gint32 pcie_card_set_sysresource_status_legacy(guint8 *device_status, guint32 length)
{
    gint32 ret = 0;
    guint32 property_value = 0;
    GSList *input_list = NULL;
    OBJ_HANDLE obj_handle = 0;

    if (length < DEVICE_STATUS_LEN) {
        return RET_ERR;
    }
    ret = dal_get_object_handle_nth(BMC_PRODUCT_NAME_APP, 0, &obj_handle);
    if (ret != RET_OK) {
        return ret;
    }

    if (PCIE_DEV_STAT_TYPE17 == device_status[0]) {
        property_value = 0x01; // bit0置一表示MMIO resoure not enough
    } else if (PCIE_DEV_STAT_TYPE18 == device_status[0]) {
        property_value = 0x02; // bit1置一表示legacy IO resoure not enough
    } else if (PCIE_DEV_STAT_TYPE19 == device_status[0]) {
        property_value = 0x04; // bit2置一表示legacy oprom resoure not enough
    }
    
    if (device_status[DEVICE_STATUS_LEN - 1] >= 0x80) {
        property_value |= 0x80;
    }
    
    input_list = g_slist_append(input_list, g_variant_new_uint32(property_value));
    ret = dfl_call_class_method(obj_handle, METHOD_SET_SYSRESOURE_STATE, NULL, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: Set failed, ret = %d!", __FUNCTION__, ret);
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL gint32 pcie_card_set_mainboard_pcie_fatal_error(guint8 device_status)
{
    gint32 ret = 0;
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;

    ret = dfl_get_object_list(CLASS_MAINBOARD_NAME, &obj_list);
    if (ret != DFL_OK || g_slist_length(obj_list) == 0) {
        debug_log(DLOG_ERROR, "Get MainBoard object list failed!");
        return RET_ERR;
    }

    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        ret = pcie_card_set_obj_property((OBJ_HANDLE)obj_note->data, PROPERTY_MAINBOARD_PCIEFATALERROR, &device_status,
            PCIE_CARD_P_TYPE_BYTE, DF_SAVE_TEMPORARY);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Set MainBoard(%s) %s(%d) failed!", dfl_get_object_name((OBJ_HANDLE)obj_note->data),
                PROPERTY_MAINBOARD_PCIEFATALERROR, device_status);
            g_slist_free(obj_list);
            return RET_ERR;
        }
    }

    g_slist_free(obj_list);

    return RET_OK;
}

LOCAL gint32 set_pcie_or_ocp_bandwidth_warnning(const char *class_name, PCIE_DEC_BANDWIDTH_MSG *msg)
{
    GSList *obj_list = NULL;
    OBJ_HANDLE ref_component_handle = 0;
    gchar device_name[32] = {0};
    gchar name[PROP_VAL_LENGTH] = {0};

    gint32 ret = dfl_get_object_list(class_name, &obj_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s: get %s obj list failed, ret = %d.", __FUNCTION__, class_name, ret);
        return RET_ERR;
    }

    for (GSList *obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        guint8 pciecard_bus = 0;
        guint8 pciecard_device = 0;
        guint8 pciecard_function = 0;
        guint8 pciecard_segment = 0;

        OBJ_HANDLE pciecard_handle = (OBJ_HANDLE)obj_node->data;
        (void)dal_get_property_value_byte(pciecard_handle, PROPERTY_PCIECARD_BUS_NUM, &pciecard_bus);
        (void)dal_get_property_value_byte(pciecard_handle, PROPERTY_PCIECARD_DEV_NUM, &pciecard_device);
        (void)dal_get_property_value_byte(pciecard_handle, PROPERTY_PCIECARD_FUN_NUM, &pciecard_function);
        (void)dal_get_property_value_byte(pciecard_handle, PROPERTY_PCIECARD_SEGMENT, &pciecard_segment);
        if (pciecard_bus == msg->bus && pciecard_device == msg->device && pciecard_function == msg->function) {
            
            if (dal_check_board_special_me_type() == RET_OK && pciecard_segment != msg->segment) {
                continue;
            }
            debug_log(DLOG_ERROR, "%s:find decreased pcie bandwidth object(%s).", __FUNCTION__,
                      dfl_get_object_name(pciecard_handle));
            (void)dfl_get_referenced_object(pciecard_handle, PROPERTY_REF_COMPONENT, &ref_component_handle);
            (void)dal_get_property_value_string(ref_component_handle, PROPERTY_COMPONENT_DEVICE_NAME, device_name,
                sizeof(device_name) - 1);
            (void)dal_get_property_value_string(pciecard_handle, PROPERTY_CARD_NAME, name, sizeof(name) - 1);
            if (strcmp(name, SJK1828_CARD_NAME) == 0) { // 检测到是SJK1828卡时，屏蔽维护日志和告警
                g_slist_free(obj_list);
                return RET_OK;
            }
            maintenance_log_v2(MLOG_ERROR, FC_CARD_MNG_PCIE_ERR,
                "BIOS, PCIe, %s, %s, segment(0x%x) Bus(0x%x) Device(0x%x) Function(0x%x) MaxLink(X%u) NegoLink(X%u).",
                PCIE_BANDWIDTH_ERROR, device_name, msg->segment, msg->bus,
                msg->device, msg->function, msg->max_link_bandwidth, msg->cur_link_bankwidth);
            
            (void)dal_set_property_value_byte(pciecard_handle, PROPERTY_PCIECARD_DECREASE_BANDWIDTH,
                msg->event_dir, DF_SAVE_TEMPORARY);
            g_slist_free(obj_list);
            return RET_OK;
        }
    }

    g_slist_free(obj_list);
    return RET_ERR;
}


LOCAL void set_pcie_bandwidth_task(void *param)
{
#define OS_STATE_DEFAULT_VALUE 0
#define OS_RESET_OR_POWER_OFF_STATE 1
#define TASK_TIME_OUT_30_MINUTES_LIMIT (30 * 60)
#define RETRY_WAIT_3_MINUTES_LIMIT (3 * 60)
#define QUEUE_RECV_MSG_TIMEOUT_2_SEC 2000000

    gint32 ret = RET_ERR;
    gint32 task_time_count = 0;
    guint8 power_state = 0;

    PCIE_DEC_BANDWIDTH_MSG *pcie_bandwidth_msg = NULL;
    guint8 wait_pcieacrd_load_flag = 0;
    guint32 retry_cnt = 0;
    guint32 card_obj_cnt_pre = 0;
    guint32 pciecard_obj_cnt_cur = 0;
    guint32 ocpcard_obj_cnt_cur = 0;

    (void)prctl(PR_SET_NAME, (gulong) "PcieBandwidth");

    for (; task_time_count < TASK_TIME_OUT_30_MINUTES_LIMIT; task_time_count += 2) {
        
        (void)card_manage_get_os_reset_flag(&power_state);

        if (power_state == OS_RESET_OR_POWER_OFF_STATE) {
            debug_log(DLOG_INFO, "%s: set pcie lanewith task exit for os reset!", __FUNCTION__);

            
            while (vos_queue_receive(g_pcie_dec_bandwidth_msg_queue, (gpointer *)&pcie_bandwidth_msg, 100) == RET_OK) {
                g_free(pcie_bandwidth_msg);
                pcie_bandwidth_msg = NULL;
            }

            card_manage_update_os_reset_flag(OS_STATE_DEFAULT_VALUE);
            g_pcie_card_load_completed = FALSE;
            g_pcie_decrease_bandwithd_task_id = 0;
            return;
        }

        
        if (g_pcie_card_load_completed != TRUE) {
            vos_task_delay(2000);
            continue;
        }

        
        ret = vos_queue_receive(g_pcie_dec_bandwidth_msg_queue, (gpointer *)&pcie_bandwidth_msg,
            QUEUE_RECV_MSG_TIMEOUT_2_SEC);
        if (ret != RET_OK || pcie_bandwidth_msg == NULL) {
            continue;
        }

        
        if (wait_pcieacrd_load_flag == 0) {
            (void)dfl_get_object_count(CLASS_PCIECARD_NAME, &pciecard_obj_cnt_cur);
            (void)dfl_get_object_count(CLASS_OCP_CARD, &ocpcard_obj_cnt_cur);
            retry_cnt = 0;

            while ((ocpcard_obj_cnt_cur + pciecard_obj_cnt_cur == 0) && (retry_cnt < RETRY_WAIT_3_MINUTES_LIMIT)) {
                vos_task_delay(3000);
                (void)dfl_get_object_count(CLASS_PCIECARD_NAME, &pciecard_obj_cnt_cur);
                (void)dfl_get_object_count(CLASS_OCP_CARD, &ocpcard_obj_cnt_cur);
                retry_cnt += 3;
            }

            if (retry_cnt >= RETRY_WAIT_3_MINUTES_LIMIT) {
                g_free(pcie_bandwidth_msg);
                pcie_bandwidth_msg = NULL;
                debug_log(DLOG_ERROR, "%s:Get PcieCard object failed for time out!", __FUNCTION__);
                continue;
            }

            // 间隔3秒，查询3次，连续3次查询BMC当前加载的PCIe卡数量不变，以此表示PCIe卡加载完成
            retry_cnt = 0;

            while (retry_cnt < 3) {
                vos_task_delay(3000);
                (void)dfl_get_object_count(CLASS_PCIECARD_NAME, &pciecard_obj_cnt_cur);
                (void)dfl_get_object_count(CLASS_OCP_CARD, &ocpcard_obj_cnt_cur);

                if (card_obj_cnt_pre != ocpcard_obj_cnt_cur + pciecard_obj_cnt_cur) {
                    card_obj_cnt_pre = ocpcard_obj_cnt_cur + pciecard_obj_cnt_cur;
                    retry_cnt = 0;
                    continue;
                }

                retry_cnt++;
            }

            wait_pcieacrd_load_flag = 1;
        }

        if (set_pcie_or_ocp_bandwidth_warnning(CLASS_PCIECARD_NAME, pcie_bandwidth_msg) != RET_OK &&
            set_pcie_or_ocp_bandwidth_warnning(CLASS_OCP_CARD, pcie_bandwidth_msg) != RET_OK) {
            maintenance_log_v2(MLOG_ERROR, FC_CARD_MNG_PCIE_ERR,
                "BIOS, PCIe, %s, Bus(0x%x) Device(0x%x) Function(0x%x) MaxLink(X%u) NegoLink(X%u).",
                PCIE_BANDWIDTH_ERROR, pcie_bandwidth_msg->bus, pcie_bandwidth_msg->device, pcie_bandwidth_msg->function,
                pcie_bandwidth_msg->max_link_bandwidth, pcie_bandwidth_msg->cur_link_bankwidth);
        }

        // 释放消息堆内存
        g_free(pcie_bandwidth_msg);
        pcie_bandwidth_msg = NULL;
    }
    g_pcie_decrease_bandwithd_task_id = 0;
    return;
}


LOCAL gint32 pcie_card_set_bandwidth_decrease_status(const guint8 *device_status, guint32 length)
{
#define OS_STATE_DEFAULT_VALUE 0

    gint32 ret = RET_ERR;
    PCIE_DEC_BANDWIDTH_MSG *msg = NULL;

    if (device_status == NULL || length != DEVICE_STATUS_LEN) {
        debug_log(DLOG_ERROR, "%s Invalid Parameter!", __FUNCTION__);
        return RET_ERR;
    }

    
    if (g_pcie_decrease_bandwithd_task_id == 0) {
        
        card_manage_update_os_reset_flag(OS_STATE_DEFAULT_VALUE);

        ret = vos_task_create(&g_pcie_decrease_bandwithd_task_id, PCIE_BANDWIDTH_DECREASE_WARNING_TASK_NAME,
            set_pcie_bandwidth_task, NULL, DEFAULT_PRIORITY);
        
        if (ret == RET_ERR) {
            g_pcie_decrease_bandwithd_task_id = 0;
            debug_log(DLOG_ERROR, "%s create task failed,ret = %d.", __FUNCTION__, ret);
            goto PROCESS_ERROR_DO_RECORD;
        }
    }

    msg = (PCIE_DEC_BANDWIDTH_MSG *)g_malloc0(sizeof(PCIE_DEC_BANDWIDTH_MSG));
    if (msg == NULL) {
        debug_log(DLOG_ERROR, "%s malloc failed,ret = %d.", __FUNCTION__, ret);
        goto PROCESS_ERROR_DO_RECORD;
    }

    (void)memcpy_s((gchar *)msg, sizeof(PCIE_DEC_BANDWIDTH_MSG), (const void *)&device_status[1],
        sizeof(PCIE_DEC_BANDWIDTH_MSG));
    ret = vos_queue_send(g_pcie_dec_bandwidth_msg_queue, (void *)msg);
    if (ret != RET_OK) {
        g_free(msg);
        debug_log(DLOG_ERROR, "%s send pcie bandwidth decrease msg failed,ret = %d.", __FUNCTION__, ret);
        goto PROCESS_ERROR_DO_RECORD;
    }
    return RET_OK;

PROCESS_ERROR_DO_RECORD:
    maintenance_log_v2(MLOG_ERROR, FC_CARD_MNG_PCIE_ERR,
        "BIOS, PCIe, %s, Bus(0x%x) Device(0x%x) Function(0x%x) MaxLink(X%u) NegoLink(X%u).\r\n", PCIE_BANDWIDTH_ERROR,
        device_status[1], device_status[2], device_status[3], device_status[4], device_status[5]);
    return RET_OK;
}

gint32 pcie_card_recover_device_status(OBJ_HANDLE obj_handle, gpointer user_data)
{
    gint32 ret = 0;
    guint8 default_value = 0;
    guchar slot_id = 0;

    
    ret = pcie_card_set_obj_property(obj_handle, PROPERTY_PCIECARD_FATALERROR, &default_value, PCIE_CARD_P_TYPE_BYTE,
        DF_SAVE_TEMPORARY);
    
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set pcie card fatal error default value failed!");
    }

    
    ret = pcie_card_set_obj_property(obj_handle, PROPERTY_FDM_FAULT_STATE, &default_value, PCIE_CARD_P_TYPE_BYTE,
        DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set pcie card FDM fault state default value failed: %d.", ret);
    }

    

    
    ret = pcie_card_set_obj_property(obj_handle, PROPERTY_PFA_EVENT, &default_value, PCIE_CARD_P_TYPE_BYTE,
        DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set pcie card PFAEvent default value failed: %d.", ret);
    }

    

    
    ret =
        dal_set_property_value_byte(obj_handle, PROPERTY_PCIECARD_DECREASE_BANDWIDTH, default_value, DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set pcie card DecreasedBandWidth default value failed, ret = %d", ret);
    }
    

    (void)dal_set_property_value_byte(obj_handle, PROPERTY_PCIECARD_FALT_BY_BIOS, 0x00, DF_SAVE_TEMPORARY);

    (void)dal_get_property_value_byte(obj_handle, PROPETRY_PCIECARD_SLOT, &slot_id);
    if (slot_id > MAX_PCIE_CARD) {
        debug_log(DLOG_ERROR, "[%s] Array subscript out of range.", __FUNCTION__);
        return RET_ERR;
    }
    g_pcie_status_by_bios_group.status[slot_id - 1] = 0x00;
    per_save((guint8 *)&g_pcie_status_by_bios_group);

    return RET_OK;
}

void pcie_card_set_device_status_default_value(void)
{
    gint32 ret = 0;
    guint8 default_state = 0;

    ret = dfl_foreach_object(CLASS_PCIECARD_NAME, pcie_card_recover_device_status, NULL, NULL);
    if (ret != DFL_OK) {
        if (ERRCODE_OBJECT_NOT_EXIST != ret) {
            debug_log(DLOG_ERROR, "Recover pcie card default value failed!");
        }
    }

    
    ret = pcie_card_set_mainboard_pcie_fatal_error(default_state);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Recover mainboard pcie default value failed!");
    }

    

    return;
}


LOCAL gint32 pcie_alarm_generate_delete_status_confirm(guint8 alarm_event_type, guint8 alarm_event_dir)
{
    guint32 bit = 0;
    gint32 set_value = 0;
    const guint8 event_array[] = {
        PCIE_DEV_STAT_TYPE0,
        PCIE_DEV_STAT_TYPE1,
        PCIE_DEV_STAT_TYPE2,
        PCIE_DEV_STAT_TYPE3,
        PCIE_DEV_STAT_TYPE4,
        PCIE_DEV_STAT_TYPE5,
        PCIE_DEV_STAT_TYPE6,
        PCIE_DEV_STAT_TYPE7,
        PCIE_DEV_STAT_TYPE8,
        PCIE_DEV_STAT_TYPE9,
        PCIE_DEV_STAT_TYPE10,
        PCIE_DEV_STAT_TYPE11,
        PCIE_DEV_STAT_TYPE12,
        PCIE_DEV_STAT_TYPE13,
        PCIE_DEV_STAT_TYPE14,
        PCIE_DEV_STAT_TYPE15,
        PCIE_DEV_STAT_TYPE16,
        PCIE_DEV_STAT_TYPE17,
        PCIE_DEV_STAT_TYPE18,
        PCIE_DEV_STAT_TYPE19
    };
    for (bit = 0; bit < (sizeof(event_array) / sizeof(guint8)); bit++) {
        if (event_array[bit] == alarm_event_type) {
            break;
        }
    }
    if (bit >= (sizeof(event_array) / sizeof(guint8))) {
        return RET_ERR;
    }

    // alarm_event_dir为0消除告警，为1则生成告警
    if (alarm_event_dir == 1) {
        g_pcie_alarm_state |= (guint32)(1 << bit);
    } else if (alarm_event_dir == 0) {
        g_pcie_alarm_state &= ~((guint32)(1 << bit));
    }
    if (g_pcie_alarm_state != 0) {
        set_value = 1;
    }
    per_save((guint8 *)&g_pcie_alarm_state);
    return set_value;
}

LOCAL gint32 __pcie_card_set_mainboard_pcie_disabled(void)
{
    gint32 ret;
    OBJ_HANDLE mainboard_handle = 0;
    const guint32 disabled_mask = 0xffffffff;

    ret = dal_get_object_handle_nth(CLASS_MAINBOARD_NAME, 0, &mainboard_handle);
    if (ret != RET_OK) {
        return RET_ERR;
    }
    debug_log(DLOG_ERROR, "%s: Set PCIe slot disabled mask command.", __FUNCTION__);
    return dal_set_property_value_uint32(mainboard_handle, PROPERTY_MAINBOARD_PCIE_SLOT_DISABLED_MASK, disabled_mask,
        DF_SAVE_TEMPORARY);
}

LOCAL gint32 verify_manufacture_id(const guint32 imana, guint32 *manufactureid)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;

    
    ret = dal_get_object_handle_nth(BMC_OBJECT_NAME, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get BMC object handle failed. ret = %d", __FUNCTION__, ret);
        return ret;
    }

    ret = dal_get_property_value_uint32(obj_handle, BMC_MANU_ID_NAME, manufactureid);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get BMC manufacturer Id failed. ret = %d", __FUNCTION__, ret);
        return ret;
    }

    if ((*manufactureid) != imana) {
        return RET_ERR;
    }
    return RET_OK;
}


gint32 pcie_card_ipmi_set_device_status(const void *msg_data, gpointer user_data)
{
    gint32 ret = 0;
    const guint8 *req_data = NULL;
    guint8 resp_data[255] = {0};
    guint32 manufactureid = 0;
    guint32 imana = 0;
    guint8 event_dir = 0;
    gint32 set_value = 0;
    guint8 device_status[DEVICE_STATUS_LEN] = {0};
    guint8 req_data_len;
    DEVICE_STATUS_S device_status_param;

    
    if (msg_data == NULL) {
        debug_log(DLOG_ERROR, "%s: msg_data is NULL", __FUNCTION__);
        return RET_ERR;
    }

    
    req_data = get_ipmi_src_data(msg_data);
    req_data_len = get_ipmi_src_data_len(msg_data);
    if (req_data == NULL) {
        debug_log(DLOG_ERROR, "%s: get ipmi src data fail", __FUNCTION__);
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    
    imana = MAKE_DWORD(0, req_data[2], req_data[1], req_data[0]);
    ret = verify_manufacture_id(imana, &manufactureid);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: manufactureid(%d) is invalid", __FUNCTION__, imana);
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    event_dir = !(req_data[5] & 0x80); // 兼容以1作为触发告警的方案
    device_status[0] = req_data[6];    // event_type
    device_status[1] = req_data[7];    // bus_id
    device_status[2] = req_data[8];    // device_id
    device_status[3] = req_data[9];    // function_id
    
    if (req_data_len >= 11) {
        device_status[4] = req_data[10];
    }
    if (req_data_len >= 12) {
        device_status[5] = req_data[11];
    }
    device_status[6] = event_dir;
    
    device_status[7] = req_data[5];
    
    debug_log(DLOG_ERROR, "Pcie fault(%d) %s, Bus Num(%d), Device Num(%d), Function Num(%d)", device_status[0],
        (event_dir == 1) ? "Asserted" : "Deasserted", device_status[1], device_status[2], device_status[3]);

    
    switch (device_status[0]) {
        case PCIE_DEV_STAT_TYPE0:
        case PCIE_DEV_STAT_TYPE1:
        case PCIE_DEV_STAT_TYPE2:
        case PCIE_DEV_STAT_TYPE3:
        case PCIE_DEV_STAT_TYPE4:
        case PCIE_DEV_STAT_TYPE5:
        case PCIE_DEV_STAT_TYPE6:
        case PCIE_DEV_STAT_TYPE7:
        case PCIE_DEV_STAT_TYPE8:
        case PCIE_DEV_STAT_TYPE9:
        case PCIE_DEV_STAT_TYPE10:
        case PCIE_DEV_STAT_TYPE11:
        case PCIE_DEV_STAT_TYPE12:
        case PCIE_DEV_STAT_TYPE13:
        case PCIE_DEV_STAT_TYPE14:
        case PCIE_DEV_STAT_TYPE15:
            pcie_set_device_status_to_log(device_status, sizeof(device_status));
            
            set_value = pcie_alarm_generate_delete_status_confirm(device_status[0], event_dir);
            if (set_value == RET_ERR) {
                ipmi_operation_log(msg_data, "Set PCIeCard status failed");
                return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
            }
            
            // 先设置主板PcieFatalError
            ret = pcie_card_set_mainboard_pcie_fatal_error(set_value);
            if (ret != RET_OK) {
                ipmi_operation_log(msg_data, "Set PCIeCard status failed");
                return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
            }

            // 新需求修改 AR1C44AFBA-4CF8-48f6-8F20-625F4DF3464A：遍历所有CLASS_PCIE_ADDR_INFO对象，定位到告警槽位
            device_status_param.device_status = device_status;
            device_status_param.length = DEVICE_STATUS_LEN;
            ret = dfl_foreach_object(CLASS_PCIE_ADDR_INFO, pcie_card_set_device_status, &device_status_param, NULL);
            if (ret != RET_OK) {
                ipmi_operation_log(msg_data, "Set PCIeCard status failed");
                return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
            }
            break;
        case PCIE_DEV_STAT_TYPE17:
        case PCIE_DEV_STAT_TYPE18:
        case PCIE_DEV_STAT_TYPE19:
            pcie_set_device_status_to_log(device_status, sizeof(device_status));
            
            set_value = pcie_alarm_generate_delete_status_confirm(device_status[0], event_dir);
            if (set_value == RET_ERR) {
                ipmi_operation_log(msg_data, "Set PCIeCard status failed");
                return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
            }
            
            // 先设置主板PcieFatalError BMC_PRODUCT_NAME_APP
            ret = pcie_card_set_mainboard_pcie_fatal_error(set_value);
            if (ret != RET_OK) {
                ipmi_operation_log(msg_data, "Set PCIeCard status failed");
                return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
            }
            ret = pcie_card_set_sysresource_status_legacy(device_status, DEVICE_STATUS_LEN);
            if (ret != RET_OK) {
                ipmi_operation_log(msg_data, "Set PCIeCard status failed");
                return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
            }

            break;

            
            
        case PCIE_DEV_STAT_TYPE16:
            device_status[7] = req_data[5] & 0x1F; // device_status第7字节0x1F用于获取BIOS上报0-4字节(req_data[5])segment识别指定PCIe卡
            (void)pcie_card_set_bandwidth_decrease_status(device_status, DEVICE_STATUS_LEN);
            break;
            
        case PCIE_DEV_STAT_TYPE20:
            pcie_set_device_status_to_log(device_status, sizeof(device_status));
            break;
        case PCIE_DEV_STAT_TYPE21:
            if (__pcie_card_set_mainboard_pcie_disabled() != RET_OK) {
                ipmi_operation_log(msg_data, "Set PCIeCard status failed");
                return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
            }
            break;
        default:
            debug_log(DLOG_ERROR, "%s: invalid device_status(%d)", __FUNCTION__, device_status[0]);
            ipmi_operation_log(msg_data, "Set PCIeCard status failed");
            return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
    }

    resp_data[0] = COMP_CODE_SUCCESS;
    resp_data[1] = LONGB0(manufactureid);
    resp_data[2] = LONGB1(manufactureid);
    resp_data[3] = LONGB2(manufactureid);
    ipmi_operation_log(msg_data, "Set PCIeCard status successfully");
    return ipmi_send_response(msg_data, 4, resp_data);
}
void reset_os_update_resources_state(void)
{
    const guint8 event_dir = 0; // 恢复告警
    gint32 set_value;
    OBJ_HANDLE mainboard_handle = 0;
    gint32 ret = 0;
    guint8 device_status[DEVICE_STATUS_LEN] = {0};
    
    device_status[0] = PCIE_DEV_STAT_TYPE19;
    device_status[DEVICE_STATUS_LEN - 1] = RESET_RESOURCES_LEGACY;
    
    set_value = pcie_alarm_generate_delete_status_confirm(PCIE_DEV_STAT_TYPE19, event_dir);
    if (set_value != RET_OK) {
        debug_log(DLOG_INFO, "pcie_alarm_generate_delete_status_confirm:set_value %d", set_value);
    }

    // 先设置主板PcieFatalError BMC_PRODUCT_NAME_APP
    ret = pcie_card_set_mainboard_pcie_fatal_error(set_value);
    debug_log(DLOG_INFO, "pcie_card_set_mainboard_pcie_fatal_error:ret %d", ret);
    ret = pcie_card_set_sysresource_status_legacy(device_status, DEVICE_STATUS_LEN);
    debug_log(DLOG_INFO, "pcie_card_set_sysresource_status_legacy:ret %d", ret);

    (void)dal_get_object_handle_nth(CLASS_MAINBOARD_NAME, 0, &mainboard_handle);
    set_value = dal_set_property_value_uint32(mainboard_handle, PROPERTY_MAINBOARD_PCIE_SLOT_DISABLED_MASK, 0,
        DF_SAVE_TEMPORARY);
    if (set_value != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Set PCIe slot disabled mask failed!", __FUNCTION__);
    }
    return;
}


LOCAL void get_pcie_dev_present_status(guint8 device_type, guint32 *present_status)
{
    OBJ_HANDLE obj_handle;
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    guint8 component_type;
    guint8 component_present;
    guint8 component_virtual_flag;
    guint8 component_slot;
    guint8 component_group_id;
    guint8 const MAX_DEVICE_SLOT = 32; // 当前最大上报槽位是11

    *present_status = 0;

    if (dfl_get_object_list(CLASS_COMPONENT, &obj_list) != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get %s object list failed.", __FUNCTION__, CLASS_COMPONENT);
        return;
    }

    for (obj_note = obj_list; obj_note != NULL; obj_note = obj_note->next) {
        component_type = 0xff;
        component_present = 0;
        component_virtual_flag = 1;
        component_slot = 0;
        component_group_id = 0;

        obj_handle = (OBJ_HANDLE)obj_note->data;
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &component_type);
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_VIRTUALFLAG, &component_virtual_flag);
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_GROUPID, &component_group_id);

        if ((component_type == device_type) && (component_virtual_flag == 0) && (component_group_id == 0xff)) {
            (void)dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_DEVICENUM, &component_slot);
            (void)dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_PRESENCE, &component_present);
            if ((component_slot >= 0x1) && (component_slot <= MAX_DEVICE_SLOT) && (component_present == 1)) {
                *present_status |= 1 << (component_slot - 1);
            }
        }
    }

    g_slist_free(obj_list);
}


gint32 ipmi_get_pcie_dev_present_status(const void *msg_data, gpointer user_data)
{
#define IPMI_DEVICE_PCIE_CARD 1
#define IPMI_DEVICE_OCP_CARD  2

    const guint8 *req_data = NULL;
    guint8 resp_data[255] = {0};
    guint32 manufactureid = 0;
    guint32 imana;
    OBJ_HANDLE obj_handle = 0;

    if (msg_data == NULL) {
        debug_log(DLOG_ERROR, "%s: msg_data is NULL", __FUNCTION__);
        return RET_ERR;
    }

    req_data = get_ipmi_src_data(msg_data);
    if (req_data == NULL) {
        debug_log(DLOG_ERROR, "%s: get ipmi src data fail", __FUNCTION__);
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    if (dfl_get_object_handle(BMC_OBJECT_NAME, &obj_handle) != DFL_OK) {
        debug_log(DLOG_ERROR, "get object handle failed");
        return ipmi_send_simple_response(msg_data, COMP_CODE_CANNOT_RESPONSE);
    }

    (void)dal_get_property_value_uint32(obj_handle, BMC_MANU_ID_NAME, &manufactureid);
    imana = MAKE_DWORD(0, req_data[2], req_data[1], req_data[0]);
    if (imana != manufactureid) {
        debug_log(DLOG_ERROR, "%s: manufactureid(%d) is invalid", __FUNCTION__, imana);
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    // 字节4表示卡type
    guint8 device_type = req_data[4];
    guint32 device_present_status = 0;

    switch (device_type) {
        case IPMI_DEVICE_PCIE_CARD:
            get_pcie_dev_present_status(COMPONENT_TYPE_PCIE_CARD, &device_present_status);
            break;
        case IPMI_DEVICE_OCP_CARD:
            get_pcie_dev_present_status(COMPONENT_TYPE_OCP, &device_present_status);
            break;
        default:
            debug_log(DLOG_ERROR, "%s: invalid device type(%d)", __FUNCTION__, device_type);
            return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    guint8 len = 0;
    resp_data[len++] = COMP_CODE_SUCCESS;
    resp_data[len++] = LONGB0(manufactureid);
    resp_data[len++] = LONGB1(manufactureid);
    resp_data[len++] = LONGB2(manufactureid);
    resp_data[len++] = LONGB0(device_present_status);
    resp_data[len++] = LONGB1(device_present_status);
    resp_data[len++] = LONGB2(device_present_status);
    resp_data[len++] = LONGB3(device_present_status);
    return ipmi_send_response(msg_data, len, resp_data);
}
