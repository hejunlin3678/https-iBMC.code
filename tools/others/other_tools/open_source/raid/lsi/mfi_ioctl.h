/****************************************************************
 * FILE        : MFI_IOCTL.H
 * MODULE      : SAS
 * DESCRIPTION : SAS IOCTL Header File
 * AUTHOR      : Parag Maharana
 * DATE        : 10-25-2004
 *****************************************************************
    Copyright (c) 2004 LSI Logic Corporation.
 *****************************************************************

 *****************************************************************
 * Revision History:
 * 10-25-2004 : Parag Maharana : Initial Release.
 * 11-11-2004 : Parag Maharana : Updated PCI_INFORMATION,
 *                               DOMAIN defined by controlCode.
 * 12-20-2004 : Parag Maharana : Removed Driver Ioctl details
 *                               MFI_IOCTL packed to 93 bytes
 * 01-13-2005 : PRM/SB/SP/GS   : SenseDataBuffer Added
 * 02-09-2005 : PRM/SB/HR/GS   : Adding SGL frame to IOCTL
 * 02-22-2012 : Subhankar      : Support for #pragma pack for Solaris
 ******************************************************************/


#ifndef INCLUDE_MFI_IOCTL
#define INCLUDE_MFI_IOCTL

#include "mfi.h"
#include "mr.h"
#include "mr_drv.h"

#ifdef  __cplusplus
extern "C" {
#endif


/****************************************************************************
*
*        I O C T L   H E A D E R   I N F O R M A T I O N
*
****************************************************************************/


typedef union _MFI_IOCTL_HEADER
{
    //
    //Windows HEADER is same as SRB_IO_CONTROL
    //
    struct
    {
        U32 headerLength;
        U8  signature[8];
        U32 timeout;
        U32 controlCode;    //0: Send to Firmware, 1~60 : reserved for CSMI, MR_DRIVER_IOCTL_WINDOWS : Handle by Driver
        U32 returnCode;
        U32 length;

    }win;

    //
    //LINUX, NETWARE, SOLARIS and all other OS HEADER
    //
    struct
    {
        U16 version;
        U16 controllerId;
        U8  signature[8];
        U32 reserved;
        U32 controlCode;  //0: Send to Firmware,  MR_DRIVER_IOCTL_OTHERS : Handle by Driver
        U32 reserved2[2];
    }other;

    U8   headerByte[28];
    U32  headerDword[7];

}MFI_IOCTL_HEADER;

/****************************************************************************
*
*        I O C T L   S G L     F R A M E
*
****************************************************************************/

typedef union _MFI_IOCTL_SGL_FRAME
{
    MFI_SGE     sge[5];
    MFI_SGE_32  sge32[8];
    MFI_SGE_64  sge64[5];
    U8          b[MFI_FRAME_SIZE];  /* standard frame is 64 bytes */
    U16         w[32];              /* word access to structure */
    U32         d[16];              /* double word access to the frame */
    U64         l[8];               /* large access */

} MFI_IOCTL_SGL_FRAME, *PMFI_IOCTL_SGL_FRAME;


/****************************************************************************
*
*        S A S  I O C T L   F R A M E
*
****************************************************************************/



/****************************************************************************
* Every IOCTL will be consisting of a header, mfi frame, sgl frame and      *
* additional databuffer. ControlCode of header will be used for Domain      *
* specific (e.g. if controlCode is zero then it is firmware domain call,    *
* otherwise driver domain call.                                             *
* Actual opcode will be part of MFI Frame.                                  *
****************************************************************************/
#define MR_MAX_SENSE_LENGTH 32

#ifdef PRAGMA_PACK_PUSH_UNSUPPORTED
    #pragma pack(1)
#else
    #pragma pack(push, 1)
#endif

typedef struct _MFI_IOCTL
{
    MFI_IOCTL_HEADER    ioctlHeader;                          //IOCTL HEADER INFORMATION
    MFI_CMD_FRAMES      mfiFrames;                            //64 bytes MFI FRAME
    MFI_IOCTL_SGL_FRAME sglFrame;                             //64 bytes SGL FRAME
    U8                  senseDataBuffer[MR_MAX_SENSE_LENGTH]; //32bytes Sense buffer
    U8                  dataBuffer[1];                        //Addition Data Buffer length is specified in MFI frame
}MFI_IOCTL, *PMFI_IOCTL;

#ifdef PRAGMA_PACK_PUSH_UNSUPPORTED
    #pragma pack()
#else
    #pragma pack(pop)
#endif

#ifdef  __cplusplus
}
#endif

#endif //INCLUDE_MFI_IOCTL
