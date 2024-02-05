
#ifndef ERROR_CODE_H
#define ERROR_CODE_H


#define UP_PP_ERROR_1 21
#define UP_PP_ERROR_2 22
#define UP_PP_ERROR_3 23
#define UP_PP_ERROR_4 24
#define UP_PP_ERROR_5 25
#define UP_PP_ERROR_6 26
#define UP_PP_ERROR_7 27
#define UP_PP_ERROR_8 28
#define UP_PP_ERROR_9 29
#define UP_PP_ID_FAIL 30 
#define UP_PP_ERROR_10 31
#define UP_PP_NODE_ID_FAIL 32   


#define UP_PC_CONF_1 41
#define UP_PC_CONF_2 42
#define UP_PC_CONF_3 43
#define UP_PC_CONF_4 44
#define UP_PC_CONF_5 45
#define UP_PC_CONF_6 46
#define UP_PC_CONF_7 47
#define UP_PC_CONF_8 48
#define UP_PC_CONF_9 49


#define UP_DFT_MODE_1 61
#define UP_DFT_MODE_2 62
#define UP_DFT_MODE_3 63
#define UP_DFT_MODE_4 64
#define UP_DFT_MODE_5 65
#define UP_DFT_MODE_6 66  


#define UP_PC_RAID_1 80
#define UP_PC_RAID_2 81
#define UP_PC_RAID_3 82
#define UP_PC_RAID_4 83
#define UP_PC_RAID_5 84
#define UP_PC_RAID_6 85
#define UP_PC_RAID_7 86
#define UP_PC_RAID_8 87


#define UP_PC_IPMC_1 91
#define UP_PC_IPMC_2 92
#define UP_PC_IPMC_3 93
#define UP_PC_IPMC_4 94
#define UP_PC_IPMC_5 95
#define UP_PC_IPMC_6 96
#define UP_PC_IPMC_7 97
#define UP_PC_IPMC_8 98


#define UP_PC_BIOS_1  101
#define UP_PC_BIOS_2  102
#define UP_PC_BIOS_3  103
#define UP_PC_BIOS_4  104
#define UP_PC_BIOS_5  105
#define UP_PC_BIOS_6  106
#define UP_PC_BIOS_7  107
#define UP_PC_BIOS_8  108
#define UP_PC_BIOS_9  109
#define UP_PC_BIOS_10 110
#define UP_PC_BIOS_11 111
#define UP_PC_BIOS_12 112
#define UP_PC_BIOS_13 113
#define UP_PC_BIOS_14 114
#define UP_PC_BIOS_15 115
#define UP_PC_BIOS_16 116
#define UP_PC_BIOS_17 117
#define UP_PC_BIOS_18 118
#define UP_PC_BIOS_19 119
#define UP_PC_BIOS_20 120
#define UP_PC_BIOS_21 121
#define UP_PC_BIOS_22 122 
#define UP_PC_BIOS_23 123 


#define UP_PC_DATA_1 131
#define UP_PC_DATA_2 132
#define UP_PC_DATA_3 133
#define UP_PC_DATA_4 134
#define UP_PC_DATA_5 135


#define UP_PC_CPLD_11 140
#define UP_PC_CPLD_1  141
#define UP_PC_CPLD_2  142
#define UP_PC_CPLD_3  143
#define UP_PC_CPLD_4  144
#define UP_PC_CPLD_5  145
#define UP_PC_CPLD_6  146
#define UP_PC_CPLD_7  147
#define UP_PC_CPLD_8  148
#define UP_PC_CPLD_9  149
#define UP_PC_CPLD_10 150


#define UP_PC_CFG_1 151
#define UP_PC_CFG_2 152
#define UP_PC_CFG_3 153
#define UP_PC_CFG_4 154
#define UP_PC_CFG_5 155


#define UP_PC_LCD_1  161
#define UP_PC_LCD_2  162
#define UP_PC_LCD_3  163
#define UP_PC_LCD_4  164
#define UP_PC_LCD_5  165 
#define UP_PC_LCD_6  166 
#define UP_PC_LCD_7  167 
#define UP_PC_LCD_8  168 
#define UP_PC_LCD_9  169 
#define UP_PC_LCD_10 170 
#define UP_PC_LCD_11 171 
#define UP_PC_LCD_12 172 
#define UP_PC_LCD_13 173 
#define UP_PC_LCD_14 174 
#define UP_PC_LCD_15 175 
#define UP_PC_LCD_16 176 
#define UP_PC_LCD_17 177 
#define UP_PC_LCD_18 178 
#define UP_PC_LCD_19 179 
#define UP_PC_LCD_20 180 


#define UP_PC_FAN_1 181
#define UP_PC_FAN_2 182
#define UP_PC_FAN_3 183
#define UP_PC_FAN_4 184
#define UP_PC_FAN_5 185
#define UP_PC_FAN_6 186
#define UP_PC_FAN_7 187


#define UP_PC_WBD_1 191
#define UP_PC_WBD_2 192
#define UP_PC_WBD_3 193
#define UP_PC_WBD_4 194
#define UP_PC_WBD_5 195
#define UP_PC_WBD_6 196 // 语言包、白牌包版本不匹配


#define UP_POWER_SUPPLY_1  201 
#define UP_POWER_SUPPLY_2  202 
#define UP_POWER_SUPPLY_3  203 
#define UP_POWER_SUPPLY_4  204 
#define UP_POWER_SUPPLY_5  205 
#define UP_POWER_SUPPLY_6  206 
#define UP_POWER_SUPPLY_7  207 
#define UP_POWER_SUPPLY_8  208 
#define UP_POWER_SUPPLY_9  209 
#define UP_POWER_SUPPLY_10 210 
#define UP_POWER_SUPPLY_11 211 
#define UP_POWER_SUPPLY_12 212 
#define UP_POWER_SUPPLY_13 213 


#ifdef ARM64_HI1711_ENABLED
#define UP_EFUSE_INFO_ERR_1 220 // 参数输入错误
#define UP_EFUSE_INFO_ERR_2 221 // 升级目录不存在
#define UP_EFUSE_INFO_ERR_3 222 // HPM包中解析tar文件失败
#define UP_EFUSE_INFO_ERR_4 223 // 打开tar文件失败
#define UP_EFUSE_INFO_ERR_5 224 // 解压tar文件失败
#define UP_EFUSE_INFO_ERR_6 225 // 打开efuse的bin文件失败
#define UP_EFUSE_INFO_ERR_7 226 // 烧写efuse信息失败
#endif


#define UP_PC_RETIMER_PARAM_ERR   231 
#define UP_PC_RETIMER_PREPARE_ERR 232 
#define UP_PC_RETIMER_FILE_ERR    233 
#define UP_PC_RETIMER_PROCESS_ERR 234 
#define UP_PC_RETIMER_NODEV       235 


#define UP_PC_MCU_PARAM_ERR   241 
#define UP_PC_MCU_PREPARE_ERR 242 
#define UP_PC_MCU_FILE_ERR    243 
#define UP_PC_MCU_PROCESS_ERR 244 
#define UP_PC_MCU_NOCARD      245 
#define UP_PC_MCU_FW_NOT_COMPATIBLE 246   
#define UP_PC_READ_FW_FILE_ERR      247   


#define UP_PRR_FW_1 251 
#define UP_PRR_FW_2 252 


#define UP_PC_CARD_XML_1 261 


#define UP_PC_CPLD_12 271
#define UP_PC_CPLD_13 272
#define UP_PC_CPLD_14 273


#define UP_PC_FRUD_PARAM_ERR   281 
#define UP_PC_FRUD_FILE_ERR    282 
#define UP_PC_FRUD_PROCESS_ERR 283 


#define UP_VSMM_SECONDARY_ERR  301   


#define UP_PC_CPLD_RAID_1 205
#define UP_PC_CPLD_RAID_2 206
#define UP_PC_CPLD_BY_I2C_1 205
#define UP_PC_CPLD_BY_I2C_2 206

#define UP_SMM_STANDBY_ERR_1 214      // EM备板不支持升级机箱部件(FAN/PEM/LCD)
#define UP_EM_EXTRA_UPGRADE_ERR_1 215 // 本地BMC升级成功，extra操作中对板BMC升级失败
#define UP_EM_EXTRA_CHECK_ERR_1 216   // 升级前的额外检查失败
#define UP_SMM_STATE_NOT_SUPPORT 217  // 当前状态不支持升级（cooling模块暂停失败）
#define UP_SHELF_FAN_ERR_1 218        // 风扇升级错误
#define UP_SHELF_FAN_ERR_2 219        // 风扇升级部分成功


#define UP_PC_VRD_PARAM_ERR 290             
#define UP_PC_VRD_HANDLE_ERR 291            
#define UP_PC_VRD_SYNC_IN_UPGRADE_ERR 292   
#define UP_PC_VRD_CPLD_STATUS_ERR 293       
#define UP_PC_VRD_CPLD_IN_UPGRADE_ERR 294   
#define UP_PC_VRD_UPGRADE_TASK_ERR 295      
#define UP_PC_VRD_INTERNAL_ERR 296          
#define UP_PC_VRD_FILE_PRO_ERR 297          

#endif 