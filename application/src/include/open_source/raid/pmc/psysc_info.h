/** @file psysc_info.h
@copyright Copyright (c) 2021 Microchip Technology Inc. All Rights Reserved.

Description: sysCore device info header file

*/

#ifndef PSYSC_INFO_H
#define PSYSC_INFO_H

/** @addtogroup syscore
 * @{
 */

#if defined(__cplusplus)
extern "C" {
#endif

/** @defgroup syscore_info Info
 * @brief
 * @details
 * @{
 */

/** @name Device descriptor properties
 * @{ */
#define kDeviceInfoRawDescriptor       0xFF  /* Get the raw device device descriptor. */
#define kDeviceInfoBaseConfigNode      0   /* Optional: Base configuration node (Variable length). Generally used internally by sysCore. */
#define kDeviceInfoOSDeviceNode        1   /* Mandatory: Device node (system dependent). */
#define kDeviceInfoProtocolType        2   /* Mandatory: Protocol type (one of [Device protocol types](@ref syscore_info)). */
#define kDeviceInfoProtocolAddress     3   /* Mandatory: Protocol Address (kProtocolAddressLength bytes). */
#define kDeviceInfoControllerIndex     4   /* Optional: Controller index (SA_WORD). Used by external impl of sysCore. */
#define kDeviceInfoDeviceType          5   /* Mandatory: Device type (SA_WORD) (one of [Device descriptor device types](@ref syscore_info)). */
#define kDeviceInfoIndex               6   /* Mandatory: Device index (SA_WORD). Set internally. */

#define kDeviceInfoDriverName          16  /* Optional: Device driver's name (system dependent).*/
#define kDeviceInfoDriverVersion       17  /* Optional: Device driver's version (system dependent).*/
#define kDeviceInfoDriverBypassSupport 18  /* Optional: Driver IO Bypass support (SA_WORD): 0 - No, 1 - Yes(disabled), 2 - Yes(enabled) */
#define kDeviceInfoControllerStatus    19  /* Optional: Controller Status (SA_DWORD): 0: OK, 1<<16: Failed, 2<<16: Offline, 3<<16 Poweroff, 4<<16 Unknown */
#define kDeviceInfoControllerDescription 20 /* Optional: Controller Description (variable length) */

#define kDeviceInfoVolumeOSName        32  /* Optional: Volume OS name (variable) */
#define kDeviceInfoVolumeUniqueId      33  /* Optional: Volume unique id (variable). First character encodes type: 'S' for string, 'B' for binary */
#define kDeviceInfoVolumeBusType       34  /* Optional: Volume bus type (SA_WORD) (one of [Device volume types](@ref syscore_info)). */
#define kDeviceInfoVolumeSerialNumber  35  /* Optional: Volume serial number (variable) */
#define kDeviceInfoVolumeVendorModel   36  /* Optional: Volume vendor and model (variable) */
#define kDeviceInfoVolumeInUse         37  /* Optional: Volume in use (YES, NO) */

#define kDeviceInfoPCIIdentifiers      64 /* Optional: PCI identifier (64-bit): (Vendor << 48) + (Device << 32) + (SSVendor << 16) + SSID */
#define kDeviceInfoPCILocation         65 /* Mandatory: PCI Location (4 bytes) (Domain:Bus:Device:Function) */
#define kDeviceInfoPCISlotNumberFormat 66 /* Optional: Single char (S:Simple slot number, T:Blade slot+slot number, M:MultiBlade RootPort + SlotNumber + Cabinet Number + Blade Info + MezzInfo ) */
#define kDeviceInfoPCISlotNumber       67 /* Optional: Controller PCI slot number as reported by OS (SA_DWORD). */
#define kDeviceInfoPCIDeviceClass      68 /* Optional: PCI Device class (in hex string) ( CCSS[PP] ) CC = Class, SS = Subclass, PP = Programming interface */

#define kDeviceInfoCSMIParentProtocolAddress      130 /* Optional: Parent phy identifier (1 byte) + filler(3) + Controller number (4 bytes) + SAS Address (8 bytes) : (kProtocolAddressLength) */
#define kDeviceInfoCSMIControllerSerialNumber     131 /* Optional: CSMI Controller serial number (variable length) */
#define kDeviceInfoCSMIControllerFirmwareVersion  132 /* Optional: CSMI Controler firmware version (SA_QWORD) */
#define kDeviceInfoCSMIControllerOptionROMVersion 133 /* Optional: CSMICOntroller Option ROM version (SA_QWORD) */

#define kDeviceInfoReserved            254  /* Reserved/internal for external sysCore implementations. */
/** @} */

#define kProtocolAddressLength         16

/** @name Device protocol types
 * @{ */
#define kProtocolTypeCISS              0x0000
#define kProtocolTypeSIS               0x0001
#define kProtocolTypePQI               0x0002
#define kProtocolTypeCSMI              0x0003
#define kProtocolTypeNativeSCSI        0x0004
#define kProtocolTypeNativeHPSA        0x0005
#define kProtocolTypeNativePQI         0x0006
#define kProtocolTypeUnknown           0xFFFF
 /** @} */

/** @name Device volume types
 * @{ */
#define kVolumeBusTypeATA    1
#define kVolumeBusTypeSCSI   2
#define kVolumeBusTypeSCSIPT 3
#define kVolumeBusTypeRAID   4
#define kVolumeBusTypeNVME   5
#define kVolumeBusTypeOther  6
/** @} */

/** @name Device descriptor device types
 * @{ */
#define kDeviceTypeArrayController      1
#define kDeviceTypePhysicalDrive        2
#define kDeviceTypeTapeDrive            3
#define kDeviceTypeSEP                  4
#define kDeviceTypeExpander             5
#define kDeviceTypeSGPIO                6
#define kDeviceTypeOtherPhysicalDevice  7
#define kDeviceTypeExternalController   8
#define kDeviceTypeLocalLogicalDrive    9
#define kDeviceTypeRemoteLogicalDrive  10
#define kDeviceTypeStorageVolume       11
#define kDeviceTypeCSMIHostBusAdapter  12
#define kDeviceTypeNVMEHostBusAdapter  13
#define kDeviceTypeOtherHostBusAdapter 14
#define kDeviceTypeUnknown             99
/** @} */

/** Get the size needed to read the property
 * @param[in] device_descriptor  Controller, physical device, or other handle.
 * @param[in] property_id        Standard id
 * @return The size in bytes of the buffer needed to read the system property
*/
SA_WORD SC_GetDeviceSystemPropertySize(DeviceDescriptor device_descriptor, SA_BYTE property_id);

/** Get a system property for a device, given a device handle
 * @param[in]  device_descriptor     Controller, physical device, or other handle.
 * @param[in]  property_id           Standard id
 * @param[out] property_buffer       Memory buffer that receives the property value on success
 * @param[in]  property_buffer_size  Size of the memory buffer provided for results
 * @return [Get Device System Property Returns]
 */
SA_BYTE SC_GetDeviceSystemProperty(DeviceDescriptor device_descriptor, SA_BYTE property_id, void *property_buffer, SA_DWORD property_buffer_size);
   /** @name Get Device System Property Returns
    * @{ */
   #define kGetDevicePropertyOK                  0  /**< @outputof SC_GetDeviceSystemProperty */
   #define kGetDevicePropertyNoValue             1  /**< @outputof SC_GetDeviceSystemProperty */
   #define kGetDevicePropertyInvalidPropertySize 2  /**< @outputof SC_GetDeviceSystemProperty */
   /** @} */

/** Change a system property for a device, given a device handle.
 * @warning Changing sysCore device descriptor properties can cause unexpected
 * behavior if incorrect data written to the descriptor. This API should only be
 * used by clients that know what they are doing.
 *
 * @param[in]  device_descriptor     Controller, physical device, or other handle.
 * @param[in]  property_id           ID for the (existing) target property.
 * @param[out] property_buffer       Memory buffer that has the new property value
 * @param[in]  property_buffer_size  Size of the memory buffer provided for results (must match current property size).
 * @return [Change Device System Property Returns](@ref syscore_info).
 */
SA_BYTE SC_ChangeDeviceSystemProperty(DeviceDescriptor device_descriptor, SA_BYTE property_id, const void *property_buffer, SA_DWORD property_buffer_size);
   /** @name Change Device System Property Returns
    * @{ */
   #define kChangeDevicePropertyOK                  0  /**< @outputof SC_ChangeDeviceSystemProperty */
   #define kChangeDevicePropertyNoValue             1  /**< @outputof SC_ChangeDeviceSystemProperty */
   #define kChangeDevicePropertyInvalidPropertySize 2  /**< @outputof SC_ChangeDeviceSystemProperty */
   /** @} */

struct _StorageVolumeInformation
{
   SA_WORD wVolumeInformationFlags;
   SA_QWORD qwUsableSizeInBytes;
   SA_QWORD qwLogicalBlockSize;
   SA_QWORD qwPhysicalBlockSize;
   SA_QWORD qwNumCylinders;
   SA_QWORD qwNumHeads;
   SA_QWORD qwNumSectors;
   SA_BYTE bSCSIBus;
   SA_BYTE bSCSITarget;
   SA_BYTE bSCSILUN;
};
typedef struct _StorageVolumeInformation StorageVolumeInformation;
typedef struct _StorageVolumeInformation *PStorageVolumeInformation;

#define kVolumeInfoFlagUsableSizeValid    0x1
#define kVolumeInfoFlagLogicalBlockSizeValid 0x02
#define kVolumeInfoFlagPhysicalBlockSizeValid 0x04
#define kVolumeInfoFlagNumCylindersValid  0x8
#define kVolumeInfoFlagNumHeadsValid      0x10
#define kVolumeInfoFlagNumSectorsValid    0x20
#define kVolumeInfoFlagSCSIBusValid       0x40
#define kVolumeInfoFlagSCSITargetValid    0x80
#define kVolumeInfoFlagSCSILUNValid       0x100


/* Controller Reset */
#define HW_IOP_RESET          0x01
#define HW_SOFT_RESET         0x02
#define IOP_HWSOFT_RESET     (HW_IOP_RESET | HW_SOFT_RESET)

struct ControllerResetRequest
{
   SA_BYTE reset_type;
};


/** Get information for a storage volume
 * @param[in]  device_descriptor  Volume handle.
 * @param[out] volume_information Structure where the information will be populated
 * @return kTrue if the information was populated, kFalse otherwise
 */
SA_BOOL SC_GetStorageVolumeInformation(DeviceDescriptor device_descriptor, PStorageVolumeInformation volume_information);

/** @} */ /* syscore_info */

#if defined(__cplusplus)
}    /* extern "C" { */
#endif

/** @} */

#endif /* PSYSC_COMMANDS_H */

