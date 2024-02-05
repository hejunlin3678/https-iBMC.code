/** @file pstorc_types.h
@copyright Copyright (c) 2021 Microchip Technology Inc. All Rights Reserved.

Description: storageCore types header file

*/

#ifndef INC_PSTORC_TYPES_H_
#define INC_PSTORC_TYPES_H_

#include "psysc_types.h"

#if defined(__cplusplus)
extern "C" {
#endif

/************************//**
 * @addtogroup storcore
 * @{
 ***************************/

/** @addtogroup storc_events Events
 @{ */

#if defined(_MSC_VER)
#pragma pack(push,1)
#else
#pragma pack(1)
#endif

struct _ControllerEvent
{
   SA_DWORD relative_controller_time;
   SA_WORD  event_class_code;
   SA_WORD  event_subclass_code;
   SA_WORD  event_detail_code;
   SA_BYTE  event_specific_data[64];
   char     event_message[80];
   SA_DWORD event_tag;
   SA_BYTE  event_day;
   SA_BYTE  event_month;
   SA_WORD  event_year;
   SA_DWORD event_seconds;
   SA_BYTE  event_filler[346];
} STRUCT_POSTFIX(packed);
typedef struct _ControllerEvent ControllerEvent;
typedef struct _ControllerEvent *PControllerEvent;

struct _PersistentControllerEvent
{
   SA_DWORD relative_controller_time;
   SA_WORD  event_class_code;
   SA_WORD  event_subclass_code;
   SA_WORD  event_detail_code;
   SA_BYTE  event_specific_data[64];
   char     event_message[80];
   SA_DWORD event_tag;
   SA_BYTE  event_day;
   SA_BYTE  event_month;
   SA_WORD  event_year;
   SA_DWORD event_seconds;
   SA_BYTE  event_filler[22];
} STRUCT_POSTFIX(packed);
typedef struct _PersistentControllerEvent PersistentControllerEvent;
typedef struct _PersistentControllerEvent *PPersistentControllerEvent;

struct _PhysicalDriveHotplugEvent
{
   SA_WORD pd_number;
   SA_BYTE configured_drive_flag;
   SA_BYTE spare_drive_flag;
   SA_BYTE big_physical_drive_number;
   SA_BYTE enclosure_bay_number;
   SA_BYTE controller_enclosure_index;
   SA_BYTE enclosure_box_number;
   char    port_id[2];
} STRUCT_POSTFIX(packed);
typedef struct _PhysicalDriveHotplugEvent PhysicalDriveHotplugEvent;
typedef struct _PhysicalDriveHotplugEvent *PPhysicalDriveHotplugEvent;

struct _PhysicalDriveStateChangeEvent
{
   SA_WORD pd_number;
   SA_BYTE failure_reason;
   SA_BYTE configured_drive_flag;
   SA_BYTE spare_drive_flag;
   SA_BYTE big_physical_drive_number;
   SA_BYTE enclosure_bay_number;
   SA_BYTE controller_enclosure_index;
   char    port_id[2];
   SA_BYTE enclosure_box_number;
} STRUCT_POSTFIX(packed);
typedef struct _PhysicalDriveStateChangeEvent PhysicalDriveStateChangeEvent;
typedef struct _PhysicalDriveStateChangeEvent* PPhysicalDriveStateChangeEvent;

struct _LogicalDriveStatusChangeEvent
{
   SA_WORD ld_number;
   SA_BYTE previous_ld_state;
   SA_BYTE new_ld_state;
   SA_BYTE previous_spare_status;
   SA_BYTE new_spare_status;
} STRUCT_POSTFIX(packed);
typedef struct _LogicalDriveStatusChangeEvent LogicalDriveStatusChangeEvent;
typedef struct _LogicalDriveStatusChangeEvent *PLogicalDriveStatusChangeEvent;

#if defined(_MSC_VER)
#pragma pack(pop)
#else
#pragma pack()
#endif

typedef void(*PEVENT_SINK)(void *cda, PControllerEvent, void *param);

/** @} */ /* storc_events */

/** @} */ /* storcore */


#if defined(__cplusplus)
}
#endif

#endif /* INC_PSTORC_TYPES_H_ */

