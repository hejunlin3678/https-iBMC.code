
/****************************************************************  
 * FILE        : ses.h
 * DESCRIPTION : SES Enclosure data structures from SES -2 specs
 ********************************************************************************* 
    Copyright (c) 2014-2016 Avago Technologies
 *********************************************************************************/


#ifndef __SES_H__
#define __SES_H__

#pragma pack(1)
#define SES_DEBUG   21

#define SPC_SEND_DIAG                           0x1D
#define SPC_RCV_DIAG_RESULTS                    0x1C


/*
*   Meanings
*   DPH         -   Diagnostic Page Header
*   Config      -   Configuration
*   Encl        -   Enclosure
*   Cnt         -   Count /Number of 
*/
#define SAS_INVALID_SAS_ADDRESS             0xFFFFFFFFFFFFFFFF /*Invalid SAS Address*/
#define SES_INVALID_INDEX                   0xFF 
#define SES_MONITOR_INTERVAL                0x120 /* Monitor Every 2 Minutes*//* Can be some value from MFCD, MFC.SES_MonitorInterval */
#define SES_MAX_ENCL                        0x20
#define SES_MAX_ENCL_PER_PORT               0x10


#define SL_CHECK_CONDITION      0x02
#define SES_MAX_RETRIES         0x05

#define SES_HARDWARE_ERROR      0x01
#define SES_SERVICES_FAILURE    0x02
#define SES_TRANSFER_FAILURE    0x04
#define SES_RECOVERED_ERROR     0x08
#define SES_DEGRADED            0x10

#define TMO                                 0
#define GET_MSB_16(_val16Bit)                   ((_val16Bit>>8)&0xFF)
#define GET_LSB_16(_val16Bit)                   ((_val16Bit)&0xFF)

#define GET_32HIGH_OF_64(_val64Bit)             ((_val64Bit>>32)&0xFFFFFFFF)
#define GET_32LOW_OF_64(_val64Bit)              ((_val64Bit)&0xFFFFFFFF)
#define MAKE_64_OF_32(_val32Low, _val32High)    ((_val32High<<32)|(_val32Low))


typedef enum _SES_ERROR
{
    SES_ERROR_NONE,
    SES_ERROR_UNSUPPORTED_PORT,
    SES_ERROR_EXCEEDED_PER_PORT_LIMIT, /*Varad*/
    SES_ERROR_EXCEEDED_MAX_SES_DEV_LIMIT, /*Varad*/
    SES_ERROR_DUPLICATE_SES_DEVICE,

}SES_ERROR;


typedef enum _SES_STATE
{
    SES_IS_INACTIVE,
    SES_IS_ACTIVE,
    SES_IS_INITIALIZING
}SES_STATE;


typedef enum _SES_DIAGNOSTIC_PG_OPCODE
{
    SES_DPO_SUPPORTED_DPS                    =0x00,
    SES_DPO_CONFIG_DP                        =0x01,
    SES_DPO_CTRL_DP                          =0x02,
    SES_DPO_STS_DP                           =0x02,
    SES_DPO_HELP_TEXT_DP                     =0x03,
    SES_DPO_STRING_OUT_DP                    =0x04,
    SES_DPO_STRING_IN_DP                     =0x04,
    SES_DPO_THRESHOLD_OUT_DP                 =0x05,
    SES_DPO_THRESHOLD_IN_DP                  =0x05,
    SES_DPO_OBSOLETE_PG                    =0x06,
    SES_DPO_ELMT_DSRPT_DP                    =0x07,
    SES_DPO_SHORT_ENCL_STS_DP                =0x08,
    SES_DPO_ENCL_BUSY_DP                     =0x09,
    SES_DPO_DEVICE_ELMT_STS_DP               =0x0A,
    SES_DPO_SUB_ENCL_HELP_TEXT_DP            =0x0B,
    SES_DPO_SUB_ENCL_STRING_OUT_DP           =0x0C,
    SES_DPO_SUB_ENCL_STRING_IN_DP            =0x0C,
    SES_DPO_SUPPORTED_SES_DPS_DP             =0x0D,
    SES_DPO_RESERVED_START_01                       =0x0E,
    SES_DPO_RESERVED_END_01                         =0x0F,
    SES_DPO_VENDOR_SPECIFIC_PG_START_01           =0x10,
    SES_DPO_VENDOR_SPECIFIC_PG_END_01             =0x1F,
    SES_DPO_RESERVED_START_02                       =0x20,
    SES_DPO_RESERVED_END_02                         =0x2F,
    SES_DPO_RESERVED_START_03                       =0x30,
    SES_DPO_RESERVED_END_03                         =0x3F,
    SES_DPO_RESERVED_START_04                       =0x40,
    SES_DPO_RESERVED_END_04                         =0x7F,
    SES_DPO_VENDOR_SPECIFIC_PG_START_02           =0x80,
    SES_DPO_VENDOR_SPECIFIC_PG_END_02             =0xFF
}SES_DIAGNOSTIC_PG_OPCODE;

typedef enum _SES_ELMT_TYPE_CODE
{
    SES_ELMT_UNSPECIFIED                     =0x00,
    SES_ELMT_DEVICE                          =0x01,
    SES_ELMT_POWER_SUPPLY                    =0x02,
    SES_ELMT_COOLING                         =0x03,
    SES_ELMT_TEMPERATURE_SENSOR              =0x04,
    SES_ELMT_DOOR_LOCK                       =0x05,
    SES_ELMT_AUDIBLE_ALARM                   =0x06,
    SES_ELMT_SES_CONTROLLER_ELECTRONICS      =0x07,
    SES_ELMT_SCC_CONTROLLER_ELECTRONICS      =0x08,
    SES_ELMT_NONVOLATILE_CACHE               =0x09,
    SES_ELMT_INVALID_OPERATION_REASON        =0x0A,
    SES_ELMT_UNINTERRUPTABLE_POWER_SUPPLY    =0x0B,
    SES_ELMT_DISPLAY                         =0x0C,
    SES_ELMT_KEY_PAD_ENTRY                   =0x0D,
    SES_ELMT_ENCLOSURE                       =0x0E,
    SES_ELMT_SCSI_TRANSCEIVER_PORT           =0x0F,
    SES_ELMT_LANGUAGE                        =0x10,
    SES_ELMT_COMMUNICATION_PORT              =0x11,
    SES_ELMT_VOLTAGE_SENSOR                  =0x12,
    SES_ELMT_CURRENT_SENSOR                  =0x13,
    SES_ELMT_SCSI_TARGET_PORT                =0x14,
    SES_ELMT_SCSI_INITIATOR_PORT             =0x15,
    SES_ELMT_SIMPLE_SUB_ENCLOSURE            =0x16,
    SES_ELMT_ARRAY_DEVICE                    =0x17,
    SES_ELMT_RESERVED_START_01               =0x18,
    SES_ELMT_RESERVED_END_01                 =0x7F,
    SES_ELMT_VENDOR_SPECIFIC_START_01        =0x80,
    SES_ELMT_VENDOR_SPECIFIC_END_01          =0xFF

}SES_ELMT_TYPE_CODE;



typedef struct _SPC_VPD83
{
     U64    SAS_Address;
}SPC_VPD83;


typedef struct _SPC_SEND_DIAG_CDB
{
    U8          OpCode;

    U8          UnitOffL:1;
    U8          DevOffL:1;
    U8          SelfTest:1;
    U8          Rsvd01:1;
    U8          PF:1;
    U8          SelfTestCode:3;

    U8          Rsvd02;

    U16         ParameterListLength;

    U8          Ctrl;
} SPC_SEND_DIAG_CDB;

typedef struct _SPC_RCV_DIAG_RESULTS_CDB
{
    U8          OpCode;

    U8          PCV:1;
    U8          Rsvd01:7;

    U8          PgCode;

    U16         AllocLen;

    U8          Ctrl;
} SPC_RCV_DIAG_RESULTS_CDB;

typedef struct _GENERATION_CODE
{
    U32         GenerationCode; /*Generation Code*/
}GENERATION_CODE;

typedef struct _SES_CONFIG_DPH
{
    U8          PgCode;       /*Page Code*/
    U8          SubEnclCnt;     /*Number of Sub-Enclosures*/
    U16         PgLen;     /*Page Length*/
    GENERATION_CODE GenCode; /*Generation Code*/
} SES_CONFIG_DPH;





typedef struct _ENCL_DSRPT_HEADER
{
    U8          EnclServicePrcsCnt         :3; /*NUMBER OF ENCLOSURE SERVICE PROCESSES*/
    U8          Rsvd_1                      :1; /*Reserved*/
    U8          RelEnclServicePrcsID    :3; /*RELATIVE ENCLOSURE SERVICE PROCESS IDENTIFIER*/
    U8          Rsvd_2                      :1; /*Reserved*/
    U8          SubEnclID;                          /*SUBENCLOSURE IDENTIFIER*/
    U8          ElmtTypeCnt;                     /*NUMBER OF ELEMENT TYPES SUPPORTED*/
    U8          EnclDsrptLen;               /*ENCLOSURE DESCRIPTOR LENGTH*/
}ENCL_DSRPT_HEADER;

typedef struct _ENCL_DSRPT
{
    U8          EnclLogicalID[8];
    U8          EnclVendorID[8];
    U8          ProductID[16];
    U8          ProductRevLevel[4];
}ENCL_DSRPT;



typedef struct _TYPE_DSRPT_HEADER
{
    U8          ElmtType;
    U8          PossibleElmtCnt;
    U8          SubEnclID;                          /*SUBENCLOSURE IDENTIFIER*/
    U8          TypeDsrptTextLen;
}TYPE_DSRPT_HEADER;

typedef struct _ENCL_CTRL_DIAG_PG_HDR
{
    U8  PgCode;

    U8  UnRecov:1;
    U8  Crit:1;
    U8  NonCrit:1;
    U8  Info:1;
    U8  Rsvd01:4;

    U16 PgLen;

    GENERATION_CODE GenCode;

}ENCL_CTRL_DIAG_PG_HDR;

typedef struct _ENCL_STS_DIAG_PG_HDR
{
    U8  PgCode;

    U8  UnRecov:1;
    U8  Crit:1;
    U8  NonCrit:1;
    U8  Info:1;
    U8  Invop:1;
    U8  Rsvd01:3;

    U16 PgLen;

    GENERATION_CODE GenCode;

}ENCL_STS_DIAG_PG_HDR;

typedef struct _ENCL_THRESHOLD_IN_PG_HDR
{
    U8  PgCode;

    U8  reserved;

    U16 PgLen;

    GENERATION_CODE GenCode;

}ENCL_THRESHOLD_IN_PG_HDR;

typedef struct _ENCL_THRESHOLD_OUT_PG_HDR
{
    U8  PgCode;

    U8  reserved;

    U16 PgLen;

    GENERATION_CODE GenCode;

}ENCL_THRESHOLD_OUT_PG_HDR;

typedef struct _SES_ELEMENT_THRESHOLD
{
    U8      HighCritical;
    U8      HighWarning;
    U8      LowWarning;
    U8      LowCritical;
}SES_ELEMENT_THRESHOLD;

typedef struct _SES_COMMON_CTRL
{
    U8      Rsvd01:4;
    U8      RstSwap:1;
    U8      Disable:1;
    U8      PredFail:1;
    U8      Select:1;
}SES_COMMON_CTRL;

typedef enum _SES_ELEMENT_STS_CODE
{
    SES_ELMT_STS_UNSUPPORTED,
    SES_ELMT_STS_OK,
    SES_ELMT_STS_CRIT,
    SES_ELMT_STS_NON_CRIT,
    SES_ELMT_STS_UNRECOV,
    SES_ELMT_STS_UN_INSTALLED,
    SES_ELMT_STS_UNKNOWN,
    SES_ELMT_STS_UNAVAIL,
    SES_ELMT_STS_RSVD
}SES_ELEMENT_STS_CODE;

typedef struct _SES_COMMON_STS
{
#if defined(SPARC) || defined(_PPC_STORELIB)
    U8      Rsvd02:1;
    U8      PredFail:1;
    U8      Rsvd:1;
    U8      Swap01:1;
    U8      StsCode:4;
#else
    U8      StsCode:4;
    U8      Swap01:1;
    U8      Rsvd:1;
    U8      PredFail:1;
    U8      Rsvd02:1;
#endif
}SES_COMMON_STS;



typedef struct _SES_ELMT_UNSPECIFIED_CTRL
{
    SES_COMMON_CTRL CmnCtrl;
    U8          Rsvd01[3];
}SES_ELMT_UNSPECIFIED_CTRL;   /*Ctrl for Element Type 0x00*/

typedef struct _SES_ELMT_UNSPECIFIED_STS
{
    SES_COMMON_STS  CmnSts;
    U8          Rsvd01[3];
}SES_ELMT_UNSPECIFIED_STS;    /*Sts for Element Type 0x00*/


typedef struct _SES_ELMT_DEVICE_CTRL
{
    SES_COMMON_CTRL CmnCtrl;

    U8          Rsvd03;

    U8          Rsvd04:1;
    U8          RqstIdent:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/
    U8          RqstRemove:1;       /*The RQST REMOVE (request removal) bit is set to request that the device slot be prepared for the removal of a device. */
    U8          RqstInsert:1;       /*The RQST INSERT (request insert) bit is set to request that the device slot be prepared for the insertion of a device */
    U8          Rsvd05:2;           /* */
    U8          DoNotRemove:1;      /*The DO NOT REMOVE bit is set to request the device not be removed.*/
    U8          Active:1;           /*The ACTIVE may be set to one by the application client to cause a visual indication that the device is active.*/

    U8          Rsvd06:2;
    U8          EnableBypB:1;
    U8          EnableBypA:1;
    U8          DeviceOff:1;
    U8          RqstFault:1;
    U8          Rsvd07:2;
} SES_ELMT_DEVICE_CTRL;    /*Ctrl for Element Type 0x01*/

typedef struct _SES_ELMT_DEVICE_STS
{
    SES_COMMON_STS  CmnSts;

    U8          SlotAddress;

#if defined(SPARC) || defined(_PPC_STORELIB)
    U8          AppClientBypassedA:1;
    U8          DoNotRemove:1;      /*The DO NOT REMOVE bit is set to request the device not be removed.*/
    U8          EnableBypA:1;
    U8          EnableBypB:1;
    U8          ReadyToInsert:1;       /*The RQST INSERT (request insert) bit is set to request that the device slot be prepared for the insertion of a device */
    U8          Remove:1;       /*The RQST REMOVE (request removal) bit is set to request that the device slot be prepared for the removal of a device. */
    U8          Ident:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/
    U8          Report:1;
	
    U8          AppClientBypassedB:1; 
    U8          FaultSensed:1;
    U8          FaultReqstd:1;
    U8          DeviceOff:1;
    U8          BypassedA:1;
    U8          BypassedB:1;
    U8          DeviceBypassedA:1;
    U8          DeviceBypassedB:1;

#else
    U8          Report:1;
    U8          Ident:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/
    U8          Remove:1;       /*The RQST REMOVE (request removal) bit is set to request that the device slot be prepared for the removal of a device. */
    U8          ReadyToInsert:1;       /*The RQST INSERT (request insert) bit is set to request that the device slot be prepared for the insertion of a device */
    U8          EnableBypB:1;
    U8          EnableBypA:1;
    U8          DoNotRemove:1;      /*The DO NOT REMOVE bit is set to request the device not be removed.*/
    U8          AppClientBypassedA:1;

    U8          DeviceBypassedB:1;
    U8          DeviceBypassedA:1;
    U8          BypassedB:1;
    U8          BypassedA:1;
    U8          DeviceOff:1;
    U8          FaultReqstd:1;
    U8          FaultSensed:1;
    U8          AppClientBypassedB:1; 
#endif

}SES_ELMT_DEVICE_STS; /*Sts for Element Type 0x01*/



typedef struct _SES_ELMT_POWER_SUPPLY_CTRL
{
    SES_COMMON_CTRL CmnCtrl;

    U8          Rsvd03:7;           /* */
    U8          RqstIdent:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/

    U8          Rsvd04;

    U8          Rsvd05:4;           /* */
    U8          RqstOn:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/
    U8          RqstFail:1;       /*The RQST REMOVE (request removal) bit is set to request that the device slot be prepared for the removal of a device. */
    U8          Rsvd06:2;

}SES_ELMT_POWER_SUPPLY_CTRL; /*Ctrl for Element Type 0x02*/


typedef struct _SES_ELMT_POWER_SUPPLY_STS
{
    SES_COMMON_STS  CmnSts;

#if defined(SPARC) || defined(_PPC_STORELIB)
    U8          Ident:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/	
    U8          Rsvd03:7;       

    U8          Rsvd05:4;
    U8          DCOverVoltage:1;	
    U8          DCUnderVoltage:1;
    U8          DCOverCurrent:1;
    U8          Rsvd04:1;

    U8          Rsvd06:1;	
    U8          Fail:1;
    U8          RqstedOn:1;
    U8          Off:1;
    U8          OvrTmpFail:1;
    U8          TempWarn:1;
    U8          ACFail:1; 
    U8          DCFail:1;  	
	
#else
    U8          Rsvd03:7;           /* */
    U8          Ident:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/

    U8          Rsvd04:1;
    U8          DCOverCurrent:1;
    U8          DCUnderVoltage:1;
    U8          DCOverVoltage:1;
    U8          Rsvd05:4;

    U8          DCFail:1;           /* */
    U8          ACFail:1;           /* */
    U8          TempWarn:1;
    U8          OvrTmpFail:1;
    U8          Off:1;
    U8          RqstedOn:1;
    U8          Fail:1;
    U8          Rsvd06:1;
#endif

}SES_ELMT_POWER_SUPPLY_STS; /*Sts for Element Type 0x02*/

typedef struct _SES_ELMT_COOLING_ELMT_CTRL
{
    SES_COMMON_CTRL CmnCtrl;

    U8          Rsvd03:7;           /* */
    U8          RqstIdent:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/

    U8          Rsvd04;

    U8          SpeedCode:3;           /* */
    U8          Rsvd05:2;
    U8          RqstOn:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/
    U8          RqstFail:1;       /*The RQST REMOVE (request removal) bit is set to request that the device slot be prepared for the removal of a device. */
    U8          Rsvd06:1;

}SES_ELMT_COOLING_ELMT_CTRL; /*Ctrl for Element Type 0x03*/

typedef struct _SES_ELMT_COOLING_ELMT_STS
{
    SES_COMMON_STS  CmnSts;
	
#if defined(SPARC) || defined(_PPC_STORELIB)
    U8          Ident:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/
    U8          Rsvd01:5;          
    U8          SpeedMSB:2;   
#else
    U8          SpeedMSB:2;           /* */
    U8          Rsvd01:5;           /* */
    U8          Ident:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/
#endif

    U8          SpeedLSB;           /* */
	
#if defined(SPARC) || defined(_PPC_STORELIB)
    U8          Rsvd06:1;
    U8          Fail:1;
    U8          RqstedOn:1;
    U8          Off:1;	
    U8          Rsvd02:1;  
    U8          SpeedCode:3; 
#else	
    U8          SpeedCode:3;           /* */
    U8          Rsvd02:1;           /* */
    U8          Off:1;
    U8          RqstedOn:1;
    U8          Fail:1;
    U8          Rsvd06:1;
#endif
}SES_ELMT_COOLING_ELMT_STS; /*Sts for Element Type 0x03*/


typedef struct _SES_ELMT_TEMP_SNSR_CTRL
{
    SES_COMMON_CTRL CmnCtrl;

    U8          Rsvd03:7;           /* */
    U8          RqstIdent:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/

    U8          Rsvd04;
    U8          Rsvd06;

}SES_ELMT_TEMP_SNSR_CTRL; /*Ctrl for Element Type 0x04*/

typedef struct _SES_ELMT_TEMP_SNSR_STS
{
    SES_COMMON_STS  CmnSts;
#if defined(SPARC) || defined(_PPC_STORELIB)
    U8          Ident:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/
    U8          Rsvd01:7;           /* */
#else
    U8          Rsvd01:7;           /* */
    U8          Ident:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/
#endif
    U8          Temperature;           /* */
#if defined(SPARC) || defined(_PPC_STORELIB)
    U8          Rsvd06:4;
    U8          OTFailure:1;
    U8          OTWarning:1;
    U8          UTFailure:1;           /* */
    U8          UTWarning:1;           /* */
#else
    U8          UTWarning:1;           /* */
    U8          UTFailure:1;           /* */
    U8          OTWarning:1;
    U8          OTFailure:1;
    U8          Rsvd06:4;
#endif
}SES_ELMT_TEMP_SNSR_STS; /*Sts for Element Type 0x04*/


typedef struct _SES_ELMT_DOOR_CTRL
{
    SES_COMMON_CTRL CmnCtrl;

    U8          Rsvd03:7;           /* */
    U8          RqstIdent:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/

    U8          Rsvd04;

    U8          UnLock:1;
    U8          Rsvd06:7;

}SES_ELMT_DOOR_CTRL; /*Ctrl for Element Type 0x05*/

typedef struct _SES_ELMT_DOOR_STS
{
    SES_COMMON_STS  CmnSts;
#if defined(SPARC) || defined(_PPC_STORELIB)
    U8          Ident:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/
    U8          Rsvd01:7;     
#else
    U8          Rsvd01:7;           /* */
    U8          Ident:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/
#endif

    U8          Rsvd02;           /* */
	
#if defined(SPARC) || defined(_PPC_STORELIB)
    U8          Rsvd06:7;
    U8          Unlocked:1;       
#else
    U8          Unlocked:1;           /* */
    U8          Rsvd06:7;
#endif

}SES_ELMT_DOOR_STS; /*Sts for Element Type 0x05*/


typedef struct _SES_ELMT_ALARM_CTRL
{
    SES_COMMON_CTRL CmnCtrl;

    U8          Rsvd03:7;           /* */
    U8          RqstIdent:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/

    U8          Rsvd04;

    U8          UnRecov:1;
    U8          Crit:1;
    U8          NonCrit:1;
    U8          Info:1;
    U8          SetRemind:1;
    U8          Rsvd06:1;
    U8          SetMute:1;
    U8          Rsvd07:1;
}SES_ELMT_ALARM_CTRL; /*Ctrl for Element Type 0x06*/

typedef struct _SES_ELMT_ALARM_STS
{
    SES_COMMON_STS  CmnSts;

#if defined(SPARC) || defined(_PPC_STORELIB)
    U8          Ident:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/
    U8          Rsvd01:7;     
#else
    U8          Rsvd01:7;           /* */
    U8          Ident:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/
#endif

    U8          Rsvd02;           /* */

#if defined(SPARC) || defined(_PPC_STORELIB)
    U8          RqstMute:1;
    U8          Muted:1;
    U8          Rsvd06:1;
    U8          Remind:1;
    U8          Info:1;
    U8          NonCrit:1;
    U8          Crit:1;
    U8          UnRecov:1;
#else
    U8          UnRecov:1;
    U8          Crit:1;
    U8          NonCrit:1;
    U8          Info:1;
    U8          Remind:1;
    U8          Rsvd06:1;
    U8          Muted:1;
    U8          RqstMute:1;
#endif
}SES_ELMT_ALARM_STS; /*Sts for Element Type 0x06*/


typedef struct _SES_ELMT_SCE_CTRL
{
    SES_COMMON_CTRL CmnCtrl;

    U8          Rsvd03:7;           /* */
    U8          RqstIdent:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/

    U8          SelectElmt:1;
    U8          Rsvd04:7;

    U8          Rsvd07;
}SES_ELMT_SCE_CTRL; /*Ctrl for Element Type 0x07*/

typedef struct _SES_ELMT_SCE_STS
{
    SES_COMMON_STS  CmnSts;

#if defined(SPARC) || defined(_PPC_STORELIB)
    U8          Ident:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/
    U8          Rsvd01:7;  
	
    U8          Rsvd02:7;      
    U8          Report:1;
     
#else
    U8          Rsvd01:7;           /* */
    U8          Ident:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/

    U8          Report:1;
    U8          Rsvd02:7;           /* */
#endif
	
    U8          Rsvd06;
}SES_ELMT_SCE_STS; /*Sts for Element Type 0x07*/

typedef struct _SES_ELMT_SCC_EE_CTRL
{
    SES_COMMON_CTRL CmnCtrl;

    U8          Rsvd03:7;           /* */
    U8          RqstIdent:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/

    U8          Rsvd04;
    U8          Rsvd07;
}SES_ELMT_SCC_EE_CTRL; /*Ctrl for Element Type 0x08*/

typedef struct _SES_ELMT_SCC_EE_STS
{
    SES_COMMON_STS  CmnSts;
	
#if defined(SPARC) || defined(_PPC_STORELIB)
    U8          Ident:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/
    U8          Rsvd01:7; 
	
    U8          Rsvd02:7;      
    U8          Report:1;      
#else
    U8          Rsvd01:7;           /* */
    U8          Ident:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/

    U8          Report:1;
    U8          Rsvd02:7;           /* */
#endif

    U8          Rsvd06;
}SES_ELMT_SCC_EE_STS; /*Sts for Element Type 0x08*/

typedef struct _SES_ELMT_NV_CACHING_CTRL
{
    SES_COMMON_CTRL CmnCtrl;

    U8          Rsvd03:7;           /* */
    U8          RqstIdent:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/

    U8          Rsvd04;
    U8          Rsvd07;
}SES_ELMT_NV_CACHING_CTRL; /*Ctrl for Element Type 0x09*/

typedef struct _SES_ELMT_NV_CACHING_STS
{
    SES_COMMON_STS  CmnSts;
	
#if defined(SPARC) || defined(_PPC_STORELIB)
    U8          Ident:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/
    U8          Rsvd01:5;        
    U8          SizeMulti:2;
#else
    U8          SizeMulti:2;
    U8          Rsvd01:5;           /* */
    U8          Ident:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/
#endif

    U8          MSB_NVCacheSize;
    U8          LSB_NVCacheSize;           /* */
}SES_ELMT_NV_CACHING_STS; /*Sts for Element Type 0x09*/

typedef struct _SES_ELMT_INVOP_REASON_CTRL
{
    SES_COMMON_CTRL CmnCtrl;

    U8          Rsvd03;           /* */
    U8          Rsvd04;
    U8          Rsvd07;
}SES_ELMT_INVOP_REASON_CTRL; /*Ctrl for Element Type 0x0A*/

typedef struct _SES_ELMT_INVOP_REASON_STS
{
    SES_COMMON_STS  CmnSts;

#if defined(SPARC) || defined(_PPC_STORELIB)
    U8          InvopType:2;         
    U8          InvopTypeSpec01:6;
#else
    U8          InvopTypeSpec01:6;
    U8          InvopType:2;           /* */
#endif

    U8          InvopTypeSpec02;
    U8          InvopTypeSpec03;           /* */
}SES_ELMT_INVOP_REASON_STS; /*Sts for Element Type 0x0A*/


typedef struct _SES_ELMT_UPS_CTRL
{
    SES_COMMON_CTRL CmnCtrl;

    U8          Rsvd03;

    U8          Rsvd04;

    U8          Rsvd05:7;           /* */
    U8          RqstIdent:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/
}SES_ELMT_UPS_CTRL; /*Ctrl for Element Type 0x0B*/


typedef struct _SES_ELMT_UPS_STS
{
    SES_COMMON_STS  CmnSts;

    U8          BatteryStatus;

#if defined(SPARC) || defined(_PPC_STORELIB)
    U8          ACLow:1;
    U8          ACHigh:1;
    U8          ACQual:1;
    U8          ACFail:1;
    U8          DCFail:1;
    U8          UPSFail:1;
    U8          Warn:1;
    U8          IntfFail:1;
	
    U8          Ident:1;
    U8          Rsvd03:5;
    U8          BattFail:1;
    U8          BPF:1;

#else
    U8          IntfFail:1;
    U8          Warn:1;
    U8          UPSFail:1;
    U8          DCFail:1;
    U8          ACFail:1;
    U8          ACQual:1;
    U8          ACHigh:1;
    U8          ACLow:1;

    U8          BPF:1;
    U8          BattFail:1;
    U8          Rsvd03:5;
    U8          Ident:1;
#endif
}SES_ELMT_UPS_STS; /*Sts for Element Type 0x0B*/


typedef struct _SES_ELMT_DISPLAY_CTRL
{
    SES_COMMON_CTRL CmnCtrl;

    U8          Rsvd03:7;           /* */
    U8          RqstIdent:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/

    U8          Rsvd04;
    U8          Rsvd05;
}SES_ELMT_DISPLAY_CTRL;/*Ctrl for Element Type 0x0C*/



typedef struct _SES_ELMT_DISPLAY_STS
{
    SES_COMMON_STS  CmnSts;

#if defined(SPARC) || defined(_PPC_STORELIB)
    U8          Ident:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/
    U8          Rsvd03:7;        
#else
    U8          Rsvd03:7;           /* */
    U8          Ident:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/
#endif

    U8          Rsvd04;
    U8          Rsvd05;
}SES_ELMT_DISPLAY_STS; /*Sts for Element Type 0x0C*/

typedef struct _SES_ELMT_KEYPAD_ENTRY_CTRL
{
    SES_COMMON_CTRL CmnCtrl;

    U8          Rsvd03:7;           /* */
    U8          RqstIdent:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/

    U8          Rsvd04;
    U8          Rsvd05;
}_SES_ELMT_KEYPAD_ENTRY_CTRL; /*Ctrl for Element Type 0x0D*/



typedef struct _SES_ELMT_KEYPAD_ENTRY_STS
{
    SES_COMMON_STS  CmnSts;

#if defined(SPARC) || defined(_PPC_STORELIB)
    U8          Ident:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/
    U8          Rsvd03:7;        
#else    
    U8          Rsvd03:7;           /* */
    U8          Ident:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/
#endif

    U8          Rsvd04;
    U8          Rsvd05;
}SES_ELMT_KEYPAD_ENTRY_STS; /*Sts for Element Type 0x0D*/


typedef struct _SES_ELMT_ENCL_CTRL
{
    SES_COMMON_CTRL CmnCtrl;

    U8          Rsvd01:7;           /* */
    U8          RqstIdent:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/

    U8          Rsvd02;

    U8          RqstWarning:1;
    U8          RqstFailure:1;
    U8          Rsvd03:6;
}SES_ELMT_ENCL_CTRL; /*Ctrl for Element Type 0x0E*/


typedef struct _SES_ELMT_ENCL_STS
{
    SES_COMMON_STS  CmnSts;

#if defined(SPARC) || defined(_PPC_STORELIB)
	
    U8          Ident:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/
    U8          Rsvd01:7;       
	
    U8          Rsvd02:6;
    U8          FailureIndication:1;
    U8          WarningIndication:1;

    U8          Rsvd03:6;
    U8          FailureRequested:1;
    U8          WarningRequested:1;
	
#else
    U8          Rsvd01:7;           /* */
    U8          Ident:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/

    U8          WarningIndication:1;
    U8          FailureIndication:1;
    U8          Rsvd02:6;

    U8          WarningRequested:1;
    U8          FailureRequested:1;
    U8          Rsvd03:6;
#endif
}SES_ELMT_ENCL_STS; /*Sts for Element Type 0x0E*/


typedef struct _SES_ELMT_SCSI_PORT_CTRL
{
    SES_COMMON_CTRL CmnCtrl;

    U8          Rsvd03:7;           /* */
    U8          RqstIdent:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/

    U8          Rsvd04;

    U8          Rsvd05:4;
    U8          Disable:1;
    U8          Rsvd06:3;    
}SES_ELMT_SCSI_PORT_CTRL; /*Ctrl for Element Type 0x0F*/

typedef struct _SES_ELMT_SCSI_PORT_STS
{
    SES_COMMON_STS  CmnSts;
	
#if defined(SPARC) || defined(_PPC_STORELIB)
	
    U8          Ident:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/
    U8          Rsvd03:7;        
	
    U8          Rsvd04:7;
    U8          Report:1;

    U8          Rsvd06:3;  
    U8          Disable:1;
    U8          Rsvd05:2;
    U8          Lol:1;
    U8          XmitFail:1;
	
#else
    U8          Rsvd03:7;           /* */
    U8          Ident:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/

    U8          Report:1;
    U8          Rsvd04:7;

    U8          XmitFail:1;
    U8          Lol:1;
    U8          Rsvd05:2;
    U8          Disable:1;
    U8          Rsvd06:3;    
#endif
}SES_ELMT_SCSI_PORT_STS; /*Sts for Element Type 0x0F*/


typedef struct _SES_ELMT_LANGUAGE_CTRL
{
    SES_COMMON_CTRL CmnCtrl;

    U8          Rsvd03:7;           /* */
    U8          RqstIdent:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/

    U16         LanguageCode;
}SES_ELMT_LANGUAGE_CTRL;    /*Ctrl for Element Type 0x10*/

typedef struct _SES_ELMT_LANGUAGE_STS
{
    SES_COMMON_STS  CmnSts;

#if defined(SPARC) || defined(_PPC_STORELIB)
    U8          Ident:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/
    U8          Rsvd03:7;  
#else
    U8          Rsvd03:7;           /* */
    U8          Ident:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/
#endif 

    U16         LanguageCode;
}SES_ELMT_LANGUAGE_STS; /*Sts for Element Type 0x10*/

typedef struct _SES_ELMT_COM_PORT_CTRL
{
    SES_COMMON_CTRL CmnCtrl;

    U8          Rsvd03:7;           /* */
    U8          RqstIdent:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/

    U8          Rsvd04;

    U8          Disable:1;
    U8          Rsvd05:7;
}SES_ELMT_COM_PORT_CTRL; /*Ctrl for Element Type 0x11*/

typedef struct _SES_ELMT_COM_PORT_STS
{
    SES_COMMON_STS  CmnSts;

#if defined(SPARC) || defined(_PPC_STORELIB)
    U8          Ident:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/
    U8          Rsvd03:7; 
#else
    U8          Rsvd03:7;           /* */
    U8          Ident:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/
#endif

    U8          Rsvd04;

#if defined(SPARC) || defined(_PPC_STORELIB)
    U8          Rsvd05:7;
    U8          Disabled:1;
#else
    U8          Disabled:1;
    U8          Rsvd05:7;
#endif
}SES_ELMT_COM_PORT_STS;/*Sts for Element Type 0x11*/

typedef struct _SES_ELMT_VOLTAGE_SENSOR_CTRL
{
    SES_COMMON_CTRL CmnCtrl;

    U8          Rsvd03:7;           /* */
    U8          RqstIdent:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/

    U8          Rsvd04;

    U8          Rsvd05;
}SES_ELMT_VOLTAGE_SENSOR_CTRL; /*Ctrl for Element Type 0x12*/


typedef struct _SES_ELMT_VOLTAGE_SENSOR_STS
{
    SES_COMMON_STS  CmnSts;

#if defined(SPARC) || defined(_PPC_STORELIB)
    U8          Ident:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/
    U8          Rsvd03:3;      
    U8          WarnOver:1;
    U8          WarnUnder:1;
    U8          CritOver:1;
    U8          CritUnder:1;
#else
    U8          CritUnder:1;           /* */
    U8          CritOver:1;
    U8          WarnUnder:1;
    U8          WarnOver:1;
    U8          Rsvd03:3;        
    U8          Ident:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/
#endif
    U16         Voltage;
}SES_ELMT_VOLTAGE_SENSOR_STS;/*Sts for Element Type 0x12*/


typedef struct _SES_ELMT_CURRENT_SENSOR_CTRL
{
    SES_COMMON_CTRL CmnCtrl;

    U8          Rsvd03:7;           /* */
    U8          RqstIdent:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/

    U8          Rsvd04;

    U8          Rsvd05;
}SES_ELMT_CURRENT_SENSOR_CTRL; /*Ctrl for Element Type 0x1 3*/

typedef struct _SES_ELMT_CURRENT_SENSOR_STS
{
    SES_COMMON_STS  CmnSts;

    U8          Rsvd03:1;           /* */
    U8          CritOver:1;
    U8          Rsvd04:1;
    U8          WarnOver:1;
    U8          Rsvd05:3;
    U8          RqstIdent:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/

    U16         Current;
}SES_ELMT_CURRENT_SENSOR_STS; /*Sts for Element Type 0x1 3*/

typedef struct _SES_ELMT_SCSI_TARGET_PORT_CTRL
{
    SES_COMMON_CTRL CmnCtrl;

    U8          Rsvd03:7;           /* */
    U8          RqstIdent:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/

    U8          Rsvd04;

    U8          Enable:1;
    U8          Rsvd05:7;    
}SES_ELMT_SCSI_TARGET_PORT_CTRL; /*Ctrl for Element Type 0x14*/

typedef struct _SES_ELMT_SCSI_TARGET_PORT_STS
{
    SES_COMMON_STS  CmnSts;

    U8          Rsvd03:7;           /* */
    U8          Ident:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/

    U8          Report:1;
    U8          Rsvd04:7;

    U8          Enabled:1;
    U8          Rsvd05:7;    
}SES_ELMT_SCSI_TARGET_PORT_STS; /*Sts for Element Type 0x14*/

typedef struct _SES_ELMT_SCSI_INITIATOR_PORT_CTRL
{
    SES_COMMON_CTRL CmnCtrl;

    U8          Rsvd03:7;           /* */
    U8          RqstIdent:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/

    U8          Rsvd04;

    U8          Enable:1;
    U8          Rsvd05:7;    
}SES_ELMT_SCSI_INITIATOR_PORT_CTRL; /*Ctrl for Element Type 0x15*/

typedef struct _SES_ELMT_SCSI_INITIATOR_PORT_STS
{
    SES_COMMON_STS  CmnSts;

    U8          Rsvd03:7;           /* */
    U8          Ident:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/

    U8          Report:1;
    U8          Rsvd04:7;

    U8          Enabled:1;
    U8          Rsvd05:7;    
}SES_ELMT_SCSI_INITIATOR_PORT_STS; /*Sts for Element Type 0x15*/

typedef struct _SES_ELMT_SUBENCLOSURE_CTRL
{
    SES_COMMON_CTRL CmnCtrl;

    U8          Rsvd03:7;           /* */
    U8          RqstIdent:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/

    U8          Rsvd04;

    U8          Rsvd05;    
}SES_ELMT_SUBENCLOSURE_CTRL; /*Ctrl for Element Type 0x16*/

typedef struct _SES_ELMT_SUBENCLOSURE_STS
{
    SES_COMMON_STS  CmnSts;

    U8          Rsvd03:7;           /* */
    U8          Ident:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/

    U8          Rsvd04;

    U8          ShortEnclSts;    
}SES_ELMT_SUBENCLOSURE_STS; /*Sts for Element Type 0x16*/

typedef struct _SES_ELMT_ARRAY_DEVICE_CTRL
{
    SES_COMMON_CTRL CmnCtrl;

    U8          RqstRR_Abort:1;      /*The RQST R/R ABORT (request rebuild/remap aborted) bit is set to request that the rebuild/remap abort indicator be turned on.*/
    U8          RqstRR_Remap:1;      /*The RQST REBUILD/REMAP (request rebuild/remap) bit is set to request that the rebuild/remap indicator be turned on.*/
    U8          RqstInFailedArray:1;
    U8          RqstInCritArray:1;
    U8          RqstConsCheck:1;
    U8          RqstHotspare:1;
    U8          RqstRsvdDevice:1;
    U8          RqstOK:1;

    U8          Rsvd04:1;
    U8          RqstIdent:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/
    U8          RqstRemove:1;       /*The RQST REMOVE (request removal) bit is set to request that the device slot be prepared for the removal of a device. */
    U8          RqstInsert:1;       /*The RQST INSERT (request insert) bit is set to request that the device slot be prepared for the insertion of a device */
    U8          Rsvd05:2;           /* */
    U8          DoNotRemove:1;      /*The DO NOT REMOVE bit is set to request the device not be removed.*/
    U8          Active:1;           /*The ACTIVE may be set to one by the application client to cause a visual indication that the device is active.*/

    U8          Rsvd06:2;
    U8          EnableBypB:1;
    U8          EnableBypA:1;
    U8          DeviceOff:1;
    U8          RqstFault:1;
    U8          Rsvd07:2;
}SES_ELMT_ARRAY_DEVICE_CTRL; /*Ctrl for Element Type 0x17*/

typedef struct _SES_ELMT_ARRAY_DEVICE_STS
{
    SES_COMMON_STS  CmnSts;

    U8          RR_Abort:1;      /*The RQST R/R ABORT (request rebuild/remap aborted) bit is set to request that the rebuild/remap abort indicator be turned on.*/
    U8          RR_Remap:1;      /*The RQST REBUILD/REMAP (request rebuild/remap) bit is set to request that the rebuild/remap indicator be turned on.*/
    U8          InFailedArray:1;
    U8          InCritArray:1;
    U8          ConsCheck:1;
    U8          Hotspare:1;
    U8          RsvdDevice:1;
    U8          OK:1;

    U8          Report:1;
    U8          Ident:1;        /*The RQST IDENT (request identify) bit is set to request that the device slot be identified by a visual indication.*/
    U8          Remove:1;       /*The RQST REMOVE (request removal) bit is set to request that the device slot be prepared for the removal of a device. */
    U8          ReadyToInsert:1;       /*The RQST INSERT (request insert) bit is set to request that the device slot be prepared for the insertion of a device */
    U8          EnclBypassedB:1;
    U8          EnclBypassedA:1;
    U8          DoNotRemove:1;      /*The DO NOT REMOVE bit is set to request the device not be removed.*/
    U8          AppClientBypassedA:1;           /*The ACTIVE may be set to one by the application client to cause a visual indication that the device is active.*/

    U8          DeviceBypassedB:1;
    U8          DeviceBypassedA:1;
    U8          BypassedB:1;
    U8          BypassedA:1;
    U8          DeviceOff:1;
    U8          FaultRequested:1;
    U8          FaultSensed:1;
    U8          AppClientBypassedB:1;           /*The ACTIVE may be set to one by the application client to cause a visual indication that the device is active.*/
}SES_ELMT_ARRAY_DEVICE_STS; /*Sts for Element Type 0x17*/



typedef union _SES_UNION_ELMT_STS
{
    SES_ELMT_UNSPECIFIED_STS            Unspecified;
    SES_ELMT_DEVICE_STS                 DeviceElmt;
    SES_ELMT_POWER_SUPPLY_STS           PS_Elmt;
    SES_ELMT_COOLING_ELMT_STS           CoolingElmt;
    SES_ELMT_TEMP_SNSR_STS              TempSnsrElmt;
    SES_ELMT_DOOR_STS                   DoorElmt;
    SES_ELMT_ALARM_STS                  AlarmElmt;
    SES_ELMT_SCE_STS                    SCE_Elmt;
    SES_ELMT_SCC_EE_STS                 SCC_EE_Elmt;
    SES_ELMT_NV_CACHING_STS             NV_CachingElmt;
    SES_ELMT_INVOP_REASON_STS           InvopElmt;
    SES_ELMT_UPS_STS                    UPS_Elmt;
    SES_ELMT_DISPLAY_STS                DisplayElmt;
    SES_ELMT_KEYPAD_ENTRY_STS           KeypadElmt;
    SES_ELMT_ENCL_STS                   EnclElmt;
    SES_ELMT_SCSI_PORT_STS              SCSI_PortElmt;
    SES_ELMT_LANGUAGE_STS               LangElmt;
    SES_ELMT_COM_PORT_STS               ComportElmt;
    SES_ELMT_VOLTAGE_SENSOR_STS         VoltageSnsrElmt;
    SES_ELMT_CURRENT_SENSOR_STS         CurrentSnsrElmt;
    SES_ELMT_SCSI_TARGET_PORT_STS       TargetPortElmt;
    SES_ELMT_SCSI_INITIATOR_PORT_STS    InitElmt;
    SES_ELMT_SUBENCLOSURE_STS           SubEnclElmt;
    SES_ELMT_ARRAY_DEVICE_STS           ArrDevElmt;
} SES_UNION_ELMT_STS;




typedef struct _SES_PHY_DSRPT
{
    U8          Rsvd03:4;
    U8          DevType:3;
    U8          Rsvd04:1;

    U8          Rsvd05;

    U8          Rsvd06:1;
    U8          SMPInitatorPort:1;
    U8          STPInitatorPort:1;
    U8          SSPInitatorPort:1;
    U8          Rsvd07:4;

    U8          Rsvd08:1;
    U8          SMPTargetPort:1;
    U8          STPTargetPort:1;
    U8          SSPtargetPort:1;
    U8          Rsvd09:4;

    U64         sasAddr;
    U64         asasAddr;
    U8          PhyID;
    U8          Rsvd10[7];
} SES_PHY_DSRPT;

typedef struct _SES_DEV_ELMT_STS_DP_HDR
{
    U8          PgCode;       /*Page Code*/
    U8          Rsvd01;     /*rsvd*/
    U16         PgLen;     /*Page Length*/
    GENERATION_CODE GenCode; /*Generation Code*/
} SES_DEV_ELMT_STS_DP_HDR;


typedef struct _SES_SAS_DEV_ELMT_STS_DSRPT
{
    U8          ProtocolID:4;
    U8          Rsvd01:4;

    U8          DsrptLen;
    
    U8          PhyDsrptCnt;

    U8          NotAllPhys:1;
    U8          Rsvd02:7;

    SES_PHY_DSRPT   PhyDsrpt;

} SES_SAS_DEV_ELMT_STS_DSRPT;





typedef struct _SES_SLOT_DEV_ID_MAP_TABLE
{
    U64     sasAddr;                    /*SAS Address of the SAS Device*/
    U16     DevID;                      /*Device ID assigned by Device Module*/
    U8      EnclID;                     /*Device ID of SES Enclosure where this PD is present*/
    U8      ElmtNum;                    /*Element Number in the Enclosure*/
    U8      EnclIndex;
} SES_SLOT_DEV_ID_MAP_TABLE;


typedef struct _ENCL_SES
{
#define MAX_ARRAY_DEVICE_ELMT               0x21

#define SES_MAX_CONFIG_DATA                 4096    /*Simply Some max number*/
#define SES_MAX_STATUS_DATA                 4096
#define SES_MAX_CTRL_DATA                   4096
#define SES_MAX_DEV_ELMT_DATA               4096
#define SES_SYNC_TIME_MAX_DATA              21

#define SES_CONFIG_PG_READ_IN_PROG          0x00000000000001    /*Config Page read in progress*/
#define SES_CONFIG_PG_READ_DEFER            0x00000000000002    /*Defer Config Page read*/
#define SES_STS_PG_READ_IN_PROG             0x00000000000004    /*Status page read in Progress*/
#define SES_STS_PG_READ_DEFER               0x00000000000008    /*Defer Status page read*/
#define SES_SLOT_SAS_ADDR_MAP_IN_PROG       0x00000000000010    /*Map page read in progress*/
#define SES_SLOT_SAS_ADDR_MAP_DEFER         0x00000000000020    /*defer Map page read*/
#define SES_SYNC_TIME_IN_PROG               0x00000000000040    /*Time sync in progress */
#define SES_SYNC_TIME_DEFER                 0x00000000000080    /*Defer Sync time*/
#define SES_CTRL_WRITE_IN_PROG              0x00000000000100    /*Update Ctrl Page In progress*/
#define SES_CTRL_WRITE_DEFER                0x00000000000200    /*Defer Updating Ctrl Page*/
#define SES_LOCATE_ON_IN_PROG               0x00000000000400    /*SES Locate Om in progress*/
#define SES_LOCATE_OFF_DEFER                0x00000000000800    /*Defer Off of SES Locate*/
#define SES_LOCATE_OFF_IN_PROG              0x00000000001000    /*SES locate Off in progress*/
#define SES_LOCATE_ON_DEFER                 0x00000000002000    /*Defer Locate ON */
#define SES_LED_UPDATE_IN_PROG              0x00000000004000    /*LED Update in progress*/
#define SES_LED_UPDATE_DEFER                0x00000000008000    /*DEfer LED Update*/

    U8          sesType;        /*Attached Enclosure Services or SES Device */ /*SES DEVICE EXTERNAL->1,SES DEVICE INTERNAL->2, ATTACHED SES DEVICE ->3*/ 
    U8          IsActive;       /*1->Active, 0-> Marked as Failed*/
    U8          connectedPort;
    U64         sasAddr;/*SAS Address of SES Device*/
    U64         asasAddr;/*Attached SAS Address*/
    U16         deviceID;       /*Device ID exported to Application*/
    U64         sesStateFlags;/*One bit indicates each of the following
                                    0x01-> Fired RECEIVE DIAGNOSTIC RESULTS, page=0x01, Config
                                    0x02-> Fired RECEIVE DIAGNOSTIC RESULTS, page=0x02, Status
                                    0x04-> Deferred firing.
                                    0x08-> Fired RECEIVE DIAGNOSTIC RESULTS, page=0x10, mapping
                                    */

    U8          FireCtrlData[SES_MAX_CTRL_DATA]; /*This buffer has the Data which is fired to SES*/
    U8          UpdateCtrlData[SES_MAX_CTRL_DATA];/*This has the updated data, in case some one updates states when the FireCtrlData has been fired*/
    U8          *pUpdateCtrlData;
    U8          *pFiredCtrlData;

    U8          SyncTime[SES_SYNC_TIME_MAX_DATA];
    U8          DevElmtStsPg[SES_MAX_DEV_ELMT_DATA];
    U8          ConfigPg[SES_MAX_CONFIG_DATA];
    U8          CachedStsPg[SES_MAX_STATUS_DATA];
    U8          CachedAltStsPg[SES_MAX_STATUS_DATA];
    U8          *psesCmdFiredPg;
    U8          *pHostRqstPg;

    SES_SLOT_DEV_ID_MAP_TABLE ArrDevElmtMapTbl[MAX_ARRAY_DEVICE_ELMT];

} ENCL_SES;



typedef struct _ENCL_SES_LIST
{
#define SAS_MAX_PORTS   4
    U8                  sesEnclCnt;
    U8                  sasPortCnt;                 /*Number of ports on this adapter*/
    U8                  sesPerPort[SAS_MAX_PORTS];

    ENCL_SES          sesDevice[SES_MAX_ENCL];
} ENCL_SES_LIST;
#pragma pack() 

#endif //__SES_H__
