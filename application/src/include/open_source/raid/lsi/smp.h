
/****************************************************************  
 * FILE        : smp.h
 * DESCRIPTION : This file provides the SMP functions and data struct definitions as per SAS 1 spec
 ********************************************************************************* 
    Copyright (c) 2014-2016 Avago Technologies
**********************************************************************************/


#ifndef __SMP_H__
#define __SMP_H__

#include "types.h"      //data type definitions

#pragma pack(1)

#ifdef _OOB_INTERFACE_
	#define SL_SMP_MAX_RESPONSE_BUFFER_LEN_SAS_1 76 //Length of Response buffer (1024 bytes of data + 4 bytes header + 4 bytes CRC)
#else
	#define SL_SMP_MAX_RESPONSE_BUFFER_LEN_SAS_1 1032 //Length of Response buffer (1024 bytes of data + 4 bytes header + 4 bytes CRC)
#endif

#define SL_SMP_REQUEST_FRAME_TYPE						0x40
#define SL_SMP_RESPONSE_FRAME_TYPE						0x41
#define SL_SMP_FUNCTION_CODE_REPORT_GENERAL				0x00
#define SL_SMP_FUNCTION_CODE_DISCOVER					0x10
#define SL_SMP_FUNCTION_CODE_REPORT_ROUTE_INFO			0x13
#define SL_SMP_FUNCTION_CODE_CONFIGURE_ROUTE_INFO		0X90
#define SL_SMP_FUNCTION_CODE_REPORT_MANUFACTURER_INFO	0x01
#define SL_SMP_FUNCTION_CODE_REPORT_PHY_ERROR_LOG		0x11
#define SL_SMP_FUNCTION_CODE_REPORT_PHY_SATA			0x12
#define SL_SMP_FUNCTION_CODE_REQUEST_PHY_CONTROL		0x91
#define SL_SMP_FUNCTION_CODE_READ_GPIO_REGISTER			0x02
#define SL_SMP_FUNCTION_CODE_WRITE_GPIO_REGISTER		0x82

#define SL_GPIO_REGISTER_TYPE_GPIO_CFG					0x00
#define SL_GPIO_REGISTER_TYPE_GPIO_RX					0x01
#define SL_GPIO_REGISTER_TYPE_GPIO_RX_GP				0x02
#define SL_GPIO_REGISTER_TYPE_GPIO_TX					0x03
#define SL_GPIO_REGISTER_TYPE_GPIO_TX_GP				0x04

#define SL_GPIO_REGISTER_INDEX_GPIO_CFG_0				0x00
#define SL_GPIO_REGISTER_INDEX_GPIO_CFG_1				0x01

//SMP DATA STRUCTURES

/**--**/
typedef struct _SL_SMP_REQUEST_REPORT_GENERAL_T
{
	U8 smpFrameType;
	U8 function;
	U8 allocated_response_length;
	U8 reserved[1];
}SL_SMP_REQUEST_REPORT_GENERAL_T;

#define SL_SMP_REQUEST_REPORT_GENERAL_S sizeof(SL_SMP_REQUEST_REPORT_GENERAL_T)

typedef struct _SL_SMP_RESPONSE_REPORT_GENERAL_T
{
	U8 smpFrameType;
	U8 function;
	U8 functionResult;
	U8 reserved;
	U16 expChangeCount;
	U16 expRouteIndex;
	U8 reserved1;
	U8 numPhy;
	struct{
#if defined(SPARC) || defined(_PPC_STORELIB)
		U8 reserved:6;
		U8 configTag:1;
		U8 configurableRouteTable:1;
#else
		U8 configurableRouteTable:1;
		U8 configTag:1;
		U8 reserved:6;
#endif
	}configBitMap;
	U8 reserved2[17];
	U8 crc[4];
}SL_SMP_RESPONSE_REPORT_GENERAL_T;

#define SL_SMP_RESPONSE_REPORT_GENERAL_S sizeof(SL_SMP_RESPONSE_REPORT_GENERAL_T)

/**--**/

typedef struct _SL_SMP_REQUEST_REPORT_MANUFACTURER_INFO_T
{
	U8 smpFrameType;
	U8 function;
	U8 reserved[2];
}SL_SMP_REQUEST_REPORT_MANUFACTURER_INFO_T;

#define SL_SMP_REQUEST_REPORT_MANUFACTURER_INFO_S sizeof(SL_SMP_REQUEST_REPORT_MANUFACTURER_INFO_T)

typedef struct _SL_SMP_RESPONSE_REPORT_MANUFACTURER_INFO_T
{
	U8 smpFrameType;
	U8 function;
	U8 functionResult;
	U8 reserved[9];
	U8 vendorID[8];
	U8 productID[16];
	U32 productRevision;
	U8 vendorSpecificInfo[20];
	U8 crc[4];
}SL_SMP_RESPONSE_REPORT_MANUFACTURER_INFO_T;

#define SL_SMP_RESPONSE_REPORT_MANUFACTURER_INFO_S sizeof(SL_SMP_RESPONSE_REPORT_MANUFACTURER_INFO_T)

/**--**/
typedef struct _SL_SMP_REQUEST_READ_GPIO_T
{
	U8 smpFrameType;
	U8 function;
	U8 registerType;
	U8 registerIndex;
	U8 registerCount;
	U8 reserved[3];
}SL_SMP_REQUEST_READ_GPIO_T;

#define SL_SMP_REQUEST_READ_GPIO_S sizeof(SL_SMP_REQUEST_READ_GPIO_T)

typedef struct _SL_SMP_RESPONSE_READ_GPIO_T
{
	U8 smpFrameType;
	U8 function;
	U8 functionResult;
	U8 reserved;
	U16 readData[1];
	U8 crc[4];
}SL_SMP_RESPONSE_READ_GPIO_T;

#define SL_SMP_RESPONSE_READ_GPIO_S sizeof(SL_SMP_RESPONSE_READ_GPIO_T)
/**--**/

typedef struct _SL_SMP_REQUEST_DISCOVER_T
{
	U8 smpFrameType;
	U8 function;
	U8 allocated_response_length;
	U8 ignored[6];
	U8 phyID;
	U8 ignored1;
	U8 reserved2;
}SL_SMP_REQUEST_DISCOVER_T;

#define SL_SMP_REQUEST_DISCOVER_S sizeof(SL_SMP_REQUEST_DISCOVER_T)

typedef struct _SL_SMP_RESPONSE_DISCOVER_T
{
	U8 smpFrameType;
	U8 function;
	U8 functionResult;
	U8 ignored[6];
	U8 phyID;
	U8 reserved[2];
	struct
	{
#if defined(SPARC) || defined(_PPC_STORELIB)
		U8 attachedDeviceType:4;
		U8 ignored1:4;
#else
		U8 ignored1:4;
		U8 attachedDeviceType:4;
#endif
	}attachedDevType;
	struct
	{
#if defined(SPARC) || defined(_PPC_STORELIB)
		U8 reserved:4;
		U8 linkRate:4;
#else
		U8 linkRate:4;
		U8 reserved:4;
#endif
	}physicalLinkRate;
	struct
	{
#if defined(SPARC) || defined(_PPC_STORELIB)
		U8 reserved:4;
		U8 SSPInitiator:1;
		U8 STPInitiator:1;
		U8 SMPInitiator:1;
		U8 SATAHost:1;
#else
		U8 SATAHost:1;
		U8 SMPInitiator:1;
		U8 STPInitiator:1;
		U8 SSPInitiator:1;
		U8 reserved:4;
#endif
	}attachedInitiatorBitMap;
	struct
	{
#if defined(SPARC) || defined(_PPC_STORELIB)
		U8 SATAPortSelector:1;
		U8 attachedDeviceType:3;
		U8 SSPTarget:1;
		U8 STPTarget:1;
		U8 SMPTarget:1;
		U8 SATADevice:1;
#else
		U8 SATADevice:1;
		U8 SMPTarget:1;
		U8 STPTarget:1;
		U8 SSPTarget:1;
		U8 attachedDeviceType:3;
		U8 SATAPortSelector:1;
#endif
	}attachedTargetBitMap;
	U64 SASAddress;
	U64 attachedSASAddress;
	U8	attachedPhyId;
	U8 reserved2[7];
	struct
	{
#if defined(SPARC) || defined(_PPC_STORELIB)
		U8 minProgrammedPhysicalLinkRate:4;
		U8 minHardwarePhysicalLinkRate:4;
#else
		U8 minHardwarePhysicalLinkRate:4;
		U8 minProgrammedPhysicalLinkRate:4;
#endif
	}minimumPhysicalLinkRate;
	struct
	{
#if defined(SPARC) || defined(_PPC_STORELIB)
		U8 maxProgrammedPhysicalLinkRate:4;
		U8 maxHardwarePhysicalLinkRate:4;
#else
		U8 maxHardwarePhysicalLinkRate:4;
		U8 maxProgrammedPhysicalLinkRate:4;
#endif
	}maximumPhysicalLinkRate;
	U8 phyChangeCount;
	struct
	{
#if defined(SPARC) || defined(_PPC_STORELIB)
		U8 virtualPhy:1;
		U8 reserved:3;
		U8 partialPathwayTimeoutValue:4;
#else
		U8 partialPathwayTimeoutValue:4;
		U8 reserved:3;
		U8 virtualPhy:1;
#endif
	}timeoutBitMap;
	struct
	{
#if defined(SPARC) || defined(_PPC_STORELIB)
		U8 reserved:4;
		U8 routingAttribute:4;
#else
        U8 routingAttribute:4;
		U8 reserved:4;
#endif
	}routingAttribute;
	U8 reserved4[5];
	U8 vendorSpecificInfo[2];
	U8 crc[4];
}SL_SMP_RESPONSE_DISCOVER_T;

#define SL_SMP_RESPONSE_DISCOVER_S sizeof(SL_SMP_RESPONSE_DISCOVER_T)

/**--**/

typedef struct _SL_SMP_REQUEST_REPORT_PHY_ERROR_LOG_T
{
	U8 smpFrameType;
	U8 function;
	U8 reserved[7];
	U8 phyID;
	U8 reserved2[2];
}SL_SMP_REQUEST_REPORT_PHY_ERROR_LOG_T;

#define SL_SMP_REQUEST_REPORT_PHY_ERROR_LOG_S sizeof(SL_SMP_REQUEST_REPORT_PHY_ERROR_LOG_T)

typedef struct _SL_SMP_RESPONSE_REPORT_PHY_ERROR_LOG_T
{
	U8 smpFrameType;
	U8 function;
	U8 functionResult;
	U8 ignored[6];
	U8 phyID;
	U8 reserved[2];
	U32 invalidDwordCount;
	U32 runningDisparityErrorCount;
	U32 lossDwordSyncCount;
	U32 phyResetProblemCount;
	U8 crc[4];
}SL_SMP_RESPONSE_REPORT_PHY_ERROR_LOG_T;

#define SL_SMP_RESPONSE_REPORT_PHY_ERROR_LOG_S sizeof(SL_SMP_RESPONSE_REPORT_PHY_ERROR_LOG_T)

/**--**/

typedef struct _SL_SMP_REQUEST_REPORT_PHY_SATA_T
{
	U8 smpFrameTye;
	U8 function;
	U8 ignored[7];
	U8 phyID;
	U8 reserved[2];
}SL_SMP_REQUEST_REPORT_PHY_SATA_T;

#define SL_SMP_REQUEST_REPORT_PHY_SATA_S sizeof(SL_SMP_REQUEST_REPORT_PHY_SATA_T)

typedef struct _SL_SMP_RESPONSE_REPORT_PHY_SATA_T
{
	U8 smpFrameType;
	U8 function;
	U8 functionResult;
	U8 ignored[6];
	U8 phyID;
	U8 reserved;
	struct
	{
#if defined(SPARC) || defined(_PPC_STORELIB)
		U8 reserved:6;
		U8 affiliationSupported:1;
		U8 affiliationValid:1;
#else
		U8 affiliationValid:1;
		U8 affiliationSupported:1;
		U8 reserved:6;
#endif
	}affiliationBitMap;
	U8 reserved1[4];
	U64 stpSASAddr;
	U8 regDeviceHostFIS[20];
	U8 reserved2[4];
	U64 affiliatedSTPinitiatorSASAddr;
	U8 crc[4];
}SL_SMP_RESPONSE_REPORT_PHY_SATA_T;

#define SL_SMP_RESPONSE_REPORT_PHY_SATA_S sizeof(SL_SMP_RESPONSE_REPORT_PHY_SATA_T)

/**--**/

typedef struct _SL_SMP_REQUEST_REPORT_ROUTE_INFO_T
{
	U8 smpFrameType;
	U8 function;
	U8 reserved[4];
	U16 expanderRouteIndex;
	U8 reserved1;
	U8 phyID;
	U8 ignored;
	U8 reserved2;
}SL_SMP_REQUEST_REPORT_ROUTE_INFO_T;

#define SL_SMP_REQUEST_REPORT_ROUTE_INFO_S sizeof(SL_SMP_REQUEST_REPORT_ROUTE_INFO_T)

typedef struct _SL_SMP_RESPONSE_REPORT_ROUTE_INFO_T
{
	U8 smpFrameType;
	U8 function;
	U8 functionResult;
	U8 reserved[3];
	U16 expanderRouteIndex;
	U8 reserved1;
	U8 phyID;
	U8 ignored[2];
	struct
	{
#if defined(SPARC) || defined(_PPC_STORELIB)
		U8 isDisabled:1;
		U8 reserved:7;
#else
		U8 reserved:7;
		U8 isDisabled:1;
#endif
	}expanderRouteEntry;
	U8 ignored1[3];
	U64 routedSASAddr;
	U8 ignored2[16];
	U8 crc[4];
}SL_SMP_RESPONSE_REPORT_ROUTE_INFO_T;

#define SL_SMP_RESPONSE_REPORT_ROUTE_INFO_S sizeof(SL_SMP_RESPONSE_REPORT_ROUTE_INFO_T)

/**--**/
typedef struct _SL_SMP_REQUEST_WRITE_GPIO_T
{
	U8 smpFrameType;
	U8 function;
	U8 registerType;
	U8 registerIndex;
	U8 registerCount;
	U8 reserved[3];
	U16 writeData[1];
}SL_SMP_REQUEST_WRITE_GPIO_T;

#define SL_SMP_REQUEST_WRITE_GPIO_S sizeof(SL_SMP_REQUEST_WRITE_GPIO_T)

typedef struct _SL_SMP_RESPONSE_WRITE_GPIO_T
{
	U8 smpFrameType;
	U8 function;
	U8 functionResult;
	U8 reserved;
	U8 crc[4];
}SL_SMP_RESPONSE_WRITE_GPIO_T;

#define SL_SMP_RESPONSE_WRITE_GPIO_S sizeof(SL_SMP_RESPONSE_WRITE_GPIO_T)

/**--**/

typedef struct _SL_SMP_REQUEST_CONFIGURE_ROUTE_INFO_T
{
	U8 smpFrameType;
	U8 function;
	U8 reserved[4];
	U16 expanderRouteIndex;
	U8 reserved1;
	U8 phyID;
	U8 reserved2[2];
	struct
	{
#if defined(SPARC) || defined(_PPC_STORELIB)
		
		U8 isDisabled:1;
		U8 reserved:7;
#else
		U8 reserved:7;
		U8 isDisabled:1;
#endif
	}expanderRouteEntry;
	U8 ignored[3];
	U64 routedSASAddr;
	U8 ignored2[16];
}SL_SMP_REQUEST_CONFIGURE_ROUTE_INFO_T;

#define SL_SMP_REQUEST_CONFIGURE_ROUTE_INFO_S sizeof(SL_SMP_REQUEST_CONFIGURE_ROUTE_INFO_T)

typedef struct _SL_SMP_RESPONSE_CONFIGURE_ROUTE_INFO_T
{
	U8 smpFrameType;
	U8 function;
	U8 functionResult;
	U8 reserved;
	U8 crc[4];
}SL_SMP_RESPONSE_CONFIGURE_ROUTE_INFO_T;

#define SL_SMP_RESPONSE_CONFIGURE_ROUTE_INFO_S sizeof(SL_SMP_RESPONSE_CONFIGURE_ROUTE_INFO_T)

/**--**/

typedef struct _SL_SMP_REQUEST_PHY_CONTROL_T
{
	U8 smpFrameType;
	U8 function;
	U8 reserved[4];
	U16 expanderRouteIndex;
	U8 reserved1;
	U8 phyID;
	U8 phyOperation;
	struct
	{
#if defined(SPARC) || defined(_PPC_STORELIB)
		U8 reserved:7;
		U8 updateTimeoutValue:1;
#else
		U8 updateTimeoutValue:1;
		U8 reserved:7;
#endif
	}updateTimeoutValueBitMap;
	U8 ignored1[20];
	struct
	{
#if defined(SPARC) || defined(_PPC_STORELIB)
		U8 progMinPhysicalLinkRate:4;
		U8 ignored:4;
#else
		U8 ignored:4;
		U8 progMinPhysicalLinkRate:4;
#endif
	}progMinPhysicalLinkRate;
	struct
	{
#if defined(SPARC) || defined(_PPC_STORELIB)
		U8 progMaxPhysicalLinkRate:4;
		U8 ignored:4;
#else
		U8 ignored:4;
		U8 progMaxPhysicalLinkRate:4;
#endif
	}progMaxPhysicalLinkRate;
	U8 ignored2[2];
	struct
	{
#if defined(SPARC) || defined(_PPC_STORELIB)
		U8 partialPathwayTimeoutValue:4;
		U8 ignored:4;
#else
		U8 partialPathwayTimeoutValue:4;
		U8 reserved:4;
#endif
	}partialPathwayTimeout;
	U8 reserved2[3];
}SL_SMP_REQUEST_PHY_CONTROL_T;

#define SL_SMP_REQUEST_PHY_CONTROL_S sizeof(SL_SMP_REQUEST_PHY_CONTROL_T)

typedef struct _SL_SMP_RESPONSE_PHY_CONTROL_T
{
	U8 smpFrameType;
	U8 function;
	U8 functionResult;
	U8 reserved;
	U8 crc[4];
}SL_SMP_RESPONSE_PHY_CONTROL_T;

#define SL_SMP_RESPONSE_PHY_CONTROL_S sizeof(SL_SMP_RESPONSE_PHY_CONTROL_T)

#pragma pack() //@@@ Add all data structures above this inside pragma pack decl @@@@
#endif //__SMP_H__
