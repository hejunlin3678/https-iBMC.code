#ifndef __MSCM_SMBIOS_H__
#define __MSCM_SMBIOS_H__
#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif
#endif /* __cplusplus */

typedef void* SMBIOS_HANDLE;

/**********************openBios与closeBios配对使用************************/
SMBIOS_HANDLE  smbios_opensmBios(const gchar *pathName);              //入口函数
void smbios_closesmBios(SMBIOS_HANDLE closesmBiosHandle);

/******************processor information (function)*****************/
gint32  smbios_getProcessorInfoNum(SMBIOS_HANDLE smBiosHandle);
gint32  smbios_getProcessorSocketDesignation(SMBIOS_HANDLE smBiosHandle, gint32 num, void *buf, gint32 size);
gint32  smbios_getProcessorManufacturer(SMBIOS_HANDLE smBiosHandle, gint32 num, void *buf, gint32 size);
gint32  smbios_getProcessorVersion(SMBIOS_HANDLE smBiosHandle, gint32 num, void *buf, gint32 size);
gint32  smbios_getProcessorSN(SMBIOS_HANDLE smBiosHandle, gint32 num, void *buf, gint32 size);
gint32  smbios_getProcessorAssetTag(SMBIOS_HANDLE smBiosHandle, gint32 num, void *buf, gint32 size);
gint32  smbios_getProcessorPartNumber(SMBIOS_HANDLE smBiosHandle, gint32 num, void *buf, gint32 size);
gint32  smbios_getProcessorType(SMBIOS_HANDLE smBiosHandle, gint32 num, void *buf, gint32 size);
gint32  smbios_getProcessorFamily(SMBIOS_HANDLE smBiosHandle, gint32 num, void *buf, gint32 size);
gint32  smbios_getProcessorVlotage(SMBIOS_HANDLE smBiosHandle, gint32 num, void *buf, gint32 size);
gint32  smbios_getProcessorStatus(SMBIOS_HANDLE smBiosHandle, gint32 num, void *buf, gint32 size);
gint32  smbios_getProcessorUpgrade(SMBIOS_HANDLE smBiosHandle, gint32 num, void *buf, gint32 size);
gint32  smbios_getProcessorExternalClock(SMBIOS_HANDLE smBiosHandle, gint32 num, void *buf, gint32 size );
gint32  smbios_getProcessorCurrentSpeed(SMBIOS_HANDLE smBiosHandle, gint32 num, void *buf, gint32 size);
gint32  smbios_getProcessorMaxSpeed(SMBIOS_HANDLE smBiosHandle, gint32 num, void *buf, gint32 size);
gint32 smbios_getProcessor_id(SMBIOS_HANDLE smBiosHandle, gint32 num, void *buf, gint32 size);
gint32 smbios_getProcessorCoreCount(SMBIOS_HANDLE smBiosHandle, gint32 num, guint16 *value);
gint32 smbios_getProcessorThreadCount(SMBIOS_HANDLE smBiosHandle, gint32 num, guint16 *value);
gint32 smbios_getProcessorMemoryTec(SMBIOS_HANDLE smBiosHandle, gint32 num, void *buf, gint32 size);
gint32 smbios_getProcessorCache(SMBIOS_HANDLE smBiosHandle, gint32 num, guint32 *value);


/******************memory module  information (function)*****************/
gint32  smbios_getMemoryModuleNum(SMBIOS_HANDLE smBiosHandle );
gint32  smbios_getMemoryModuleSocketDesignation(SMBIOS_HANDLE smBiosHandle, gint32 num, void *buf, gint32 size);
gint32  smbios_getMemoryModuleCurrentSpeed(SMBIOS_HANDLE smBiosHandle, gint32 num, void *buf, gint32 size);
gint32  smbios_getMemoryModuleCurrentMemoryType(SMBIOS_HANDLE smBiosHandle, gint32 num, void *buf, gint32 size);
gint32  smbios_getMemoryModuleInstalledSize(SMBIOS_HANDLE smBiosHandle, gint32 num, void *buf, gint32 size);
gint32  smbios_getMemoryModuleEnabledSize(SMBIOS_HANDLE smBiosHandle, gint32 num, void *buf, gint32 size);

/*******************Memory Device (function)***************************/
gint32  smbios_getMemoryDeviceNum(SMBIOS_HANDLE smBiosHandle);
gint32  smbios_getMemoryDeviceLocator(SMBIOS_HANDLE smBiosHandle, gint32 num, void *buf, gint32 size);
gint32  smbios_getMemoryDeviceBankLocator(SMBIOS_HANDLE smBiosHandle, gint32 num, void *buf, gint32 size);
gint32  smbios_getMemoryDeviceManufacturer(SMBIOS_HANDLE smBiosHandle, gint32 num, void *buf, gint32 size);
gint32  smbios_getMemoryDeviceSN(SMBIOS_HANDLE smBiosHandle, gint32 num, void *buf, gint32 size);
gint32  smbios_getMemoryOriginalPartNum(SMBIOS_HANDLE smBiosHandle, gint32 num, void *buf, gint32 size);
gint32  smbios_getMemoryDeviceAssetTag(SMBIOS_HANDLE smBiosHandle, gint32 num, void *buf, gint32 size);
gint32  smbios_getMemoryDevicePartNumber(SMBIOS_HANDLE smBiosHandle, gint32 num, void *buf, gint32 size);
gint32  smbios_getMemoryTotalWidth(SMBIOS_HANDLE smBiosHandle, gint32 num, void *buf, gint32 size);
gint32  smbios_getMemoryDataWidth(SMBIOS_HANDLE smBiosHandle, gint32 num, void *buf, gint32 size);

gint32  smbios_getMemoryDeviceSize(SMBIOS_HANDLE smBiosHandle, gint32 num, void *buf, gint32 size);
gint32  smbios_getMemoryDeviceMemoryType(SMBIOS_HANDLE smBiosHandle, gint32 num, void *buf, gint32 size);
gint32  smbios_getMemoryDeviceTypeDetail(SMBIOS_HANDLE smBiosHandle, gint32 num, void *buf, gint32 size);
gint32  smbios_getMemoryDeviceSpeed(SMBIOS_HANDLE smBiosHandle, gint32 num, void *buf, gint32 size);
gint32 smbios_getMemoryDeviceConfigMemClkSpeed(SMBIOS_HANDLE smBiosHandle, gint32 num, void *buf, gint32 size);
gint32 smbios_getMemorySN(SMBIOS_HANDLE smBiosHandle, gint32 num, void *buf, gint32 size);
gint32 smbios_getMemoryManufacturerID(SMBIOS_HANDLE smBiosHandle, gint32 num, gchar *buf, gint32 size);
gint32 smbios_getMemoryManufacturingDate(SMBIOS_HANDLE smBiosHandle, gint32 num, gchar *buf, gint32 size);
gint32 smbios_getMemoryType(SMBIOS_HANDLE smBiosHandle, gint32 num, void *buf, gint32 size);
gint32 smbios_getMemoryMinimumVoltage(SMBIOS_HANDLE smBiosHandle, gint32 num, guint16 *value);
gint32 smbios_getMemoryRank(SMBIOS_HANDLE smBiosHandle, gint32 num, guint8 *value);
gint32 smbios_getMemoryBitWidth(SMBIOS_HANDLE smBiosHandle, gint32 num, guint16 *value);
gint32 smbios_getMemoryTec(SMBIOS_HANDLE smBiosHandle, gint32 num, void *buf, gint32 size);
gint32 smbios_getMemoryModuleType(SMBIOS_HANDLE smBiosHandle, gint32 num, void *buf, gint32 size);

void smbios_getMemoryState(SMBIOS_HANDLE smBiosHandle, gint32 memory_num);
/* BEGIN: Added by gwx455466, 2017/7/4 17:48:50   问题单号:AR-0000276588-006-001 */
gint32 smbios_getMemoryDeviceBomNum(SMBIOS_HANDLE smBiosHandle, gint32 num, char *buf, gint32 size);
/* END:   Added by gwx455466, 2017/7/4 17:48:56 */
/* BEGIN: Added by gwx455466, 2018/9/22 14:58:44   问题单号:AR.SR.SFEA02130917.011.002 */
gint32 smbios_getMemoryTechnology2(SMBIOS_HANDLE smBiosHandle, gint32 num, void *buf, gint32 size);
/* END:   Added by gwx455466, 2018/9/22 14:58:48 */
#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif /* __cplusplus */
#endif //__MSCM_SMBIOs_H__
