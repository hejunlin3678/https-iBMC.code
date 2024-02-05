/** @file pstorc_events.h
@copyright Copyright (c) 2021 Microchip Technology Inc. All Rights Reserved.

Description: storageCore events header file
   Defines structures and functions to retrieve controller events

*/

#ifndef PSTORC_EVENTS_H_
#define PSTORC_EVENTS_H_

/************************//**
 * @addtogroup storcore
 * @{
 ***************************/

/************************************//**
 * @addtogroup storc_events Events
 * @brief UART messaging/events.
 * @details
 * @{
 ***************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/** Get the next controller event.
 * This function is recommended only if the [event queue](@ref storc_events) has been stopped.
 * @attention The caller must release the memory at the returned address using @ref SA_free.
 * @warning Each call will remove one event from the controller buffer, which will cause the event to not
 * be processed by any registered sink.
 * @param[in] cda                  Controller data area.
 * @param[in] log_index            Log consumer index to controller event buffer.
 * @param[in] get_serial_log_data  If @ref kFalse, get next non-serial log event (filter serial log events from return).
 * @return Address to allocated controller event or NULL if the controller reported no new events.
 *
 * __Example__
 * @code
 * PControllerEvent event = NULL;
 * if ((event = SA_GetNextControllerEvent(cda, 0, kFalse)) != NULL) //=> Get next Non-serial log event (if any).
 * {
 *     // Process event...
 *     SA_free(event);
 * }
 * else if ((event = SA_GetNextControllerEvent(cda, 0, kTrue)) != NULL) //=> Get next [possibly serial log] event (if any).
 * {
 *     printf("Controller only had a serial log event\n");
 *     // Process event...
 *     SA_free(event);
 * }
 * else
 * {
 *     printf("Controller currently has no events to report\n");
 * }
 * @endcode
 */
PControllerEvent SA_GetNextControllerEvent(PCDA cda, SA_BYTE log_index, SA_BOOL get_serial_log_data);

#if !defined(__UEFI)

/** @name Register Event Sink
 @{ */
/** Register an event handler (or sink) to callback with events.
 * @pre The [event queue has been started](@ref SA_StartEventQueue).
 * @attention Stopping the event queue will cause the registered event handler to be 'forgotten'.
 * @attention The user must register an event handler/sink for each CDA.
 * @param[in] cda      Controller data area to monitor for events.
 * @param[in] handler  Callback function when an event is queued (can be NULL to unregister any event sink).
 * @param[in] param    Address that will be given when the event handler is invoked (can be NULL).
 * @return [Register Event Sink Returns](@ref storc_events).
 *
 * __Example__
 * @code
 *
 * int id;
 *
 * void MyEventHandler(PCDA cda, PControllerEvent event, void * param)
 * {
 *    // handle event...
 * }
 *
 * if (SA_StartEventQueue(cda) == kStartEventQueueSuccess)
 * {
 *    if (SA_RegisterEventSink(cda, MyEventHandler, &id) != kRegisterEventSinkSuccess)
 *    {
 *       fprintf(stderr, "Error: Failed to register event handler\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_RegisterEventSink(PCDA cda, PEVENT_SINK handler, void *param);
   /** Successfully register event sink.
    * @outputof SA_RegisterEventSink. */
   #define kRegisterEventSinkSuccess      0
   /** The event sink cannot be registered because the event queue has not been started.
    * @outputof SA_RegisterEventSink. */
   #define kRegisterEventSinkNotStarted   1
/** @} */

/** @name Register Serial Log Event Sink
 @{ */
/** Register an serial log handler (or sink) to callback with serial log events.
 * @pre The [event queue has been started](@ref SA_StartEventQueue).
 * @attention Stopping the event queue will cause the registered serial log handler to be 'forgotten'.
 * @attention The user must register a serial log handler/sink for each CDA.
 * @param[in] cda      Controller data area to monitor for serial log events.
 * @param[in] handler  Callback function when a serial log event is queued (can be NULL to unregister a sink).
 * @param[in] param    Address that will be given when the event handler is invoked (can be NULL).
 * @return [Register Serial Log Sink Event Returns](@ref storc_events).
 *
 * __Example__
 * @code
 *
 * int id;
 *
 * void MySerialLogHandler(PCDA cda, PControllerEvent event, void * param)
 * {
 *    // handle serial log event...
 * }
 *
 * if (SA_StartEventQueue(cda) == kStartEventQueueSuccess)
 * {
 *    if (SA_RegisterSerialLogSink(cda, MySerialLogHandler, &id) != kRegisterSerialLogSinkSuccess)
 *    {
 *       fprintf(stderr, "Error: Failed to register event handler\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_RegisterSerialLogSink(PCDA cda, PEVENT_SINK handler, void *param);
   /** Successfully register serial log event sink.
    * @outputof SA_RegisterSerialLogSink. */
   #define kRegisterSerialLogSinkSuccess      0
   /** The serial log event sink cannot be registered because the event queue has not been started.
    * @outputof SA_RegisterSerialLogSink. */
   #define kRegisterSerialLogSinkNotStarted   1
/** @} */

/** @name Set Event Queue Refresh Period
 @{ */
/** Set the refresh period (in milliseconds) of the event queue monitor.
 * @pre The [event queue has been started](@ref SA_StartEventQueue).
 * @attention Stopping the event queue will cause any set refresh period to be forgotten.
 * @attention The user can set a different event monitor refresh period for each CDA.
 * @param[in] cda                     Controller data area.
 * @param[in] period_in_milliseconds  Desired refresh period (in milliseconds).
 * @return [Set Event Queue Refresh Period Returns](@ref storc_events).
 *
 * __Example__
 * @code
 * if (SA_StartEventQueue(cda) == kStartEventQueueSuccess)
 * {
 *    if (SA_SetEventQueueRefreshPeriod(cda, 0) == kRegisterEventSinkSuccess)
 *    {
 *       fprintf(stderr, "Successfully removed any waiting by the event monitor.\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_SetEventQueueRefreshPeriod(PCDA cda, SA_DWORD period_in_milliseconds);
   /** Successfully set the event queue refresh period.
    * @outputof SA_SetEventQueueRefreshPeriod. */
   #define kSetEventQueueRefreshPeriodSuccess               0
   /** The event queue refresh period cannot be set because the event queue has not been started.
    * @outputof SA_SetEventQueueRefreshPeriod. */
   #define kSetEventQueueRefreshPeriodEventQueueNotStarted  1
/** @} */

/** @name Start Event Queue
 @{ */
/** Start the event queue threads for the given controller.
 * @attention A CDA's event queue is automically started (if possible) when calling @ref SA_InitializeCDA.
 * @pre The [event queue has been started](@ref SA_StartEventQueue).
 * @post Clients may set any event sink (@ref SA_RegisterEventSink) or set the refresh period (@ref SA_SetEventQueueRefreshPeriod).
 * @post Clients __must__ stop the event queue (using @ref SA_StopEventQueue) before exiting the process.
 *
 * @param[in] cda        Controller data area of controller to monitor/report events.
 * @param[in] log_index  [0,3] - Log consumer index to controller event buffer.
 * @return [Start Event Queue Returns](@ref storc_events).
 *
 * __Example__
 * @code
 * if (SA_StartEventQueue(cda, 0) != kStartEventQueueSuccess)
 * {
 *    fprintf(stderr, "Failed to start event queue.\n");
 * }
 * @endcode
 */
SA_BYTE SA_StartEventQueue(PCDA cda, SA_BYTE log_index);
   /** Successfully started the event monitor.
    * @outputof SA_StartEventQueue. */
   #define kStartEventQueueSuccess        0
   /** Invalid log index.
    * @outputof SA_StartEventQueue. */
   #define kStartEventInvalidLogIndex     1
   /** Failed to start the event queue because it is already running.
    * @outputof SA_StartEventQueue. */
   #define kStartEventQueueAlreadyRunning 2
   /** Failed to start the event queue monitor.
    * @outputof SA_StartEventQueue. */
   #define kStartEventQueueFailed         3
/** @} */

/** @name Stop Event Queue
 @{ */
/** Stop the event queue threads for the given controller.
 * @pre The event queue is running (or paused).
 * @post Clients can re-start the event queue (using @ref SA_StartEventQueue) and re-register their sink.
 * @attention Any event sink or refresh period set for this event queue will be forgotten.
 * @attention Any outstanding events in the queue are dropped.
 *
 * @param[in] cda  Controller data area of controller to stop event monitoring.
 * @return [Stop Event Queue Returns](@ref storc_events).
 *
 * __Example__
 * @code
 * if (SA_StartEventQueue(cda) == kStartEventQueueSuccess)
 * {
 *    if (SA_StopEventQueue(cda) != kStopEventQueueSuccess)
 *    {
 *       fprintf(stderr, "Failed to stop event queue.\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_StopEventQueue(PCDA cda);
   /** Successfully stopped the event monitor.
    * @outputof SA_StopEventQueue. */
   #define kStopEventQueueSuccess         0
   /** Failed to stop the event queue because it has not been started.
    * @outputof SA_StopEventQueue. */
   #define kStopEventQueueNotStarted      1
   /** Failed to stop the event queue monitor.
    * @outputof SA_StopEventQueue. */
   #define kStopEventQueueFailed          2
/** @} */

/** @name Pause Event Queue
 @{ */
/** Pause the event queue threads for the given controller.
 * @pre The event queue is running.
 * @post Clients can resume the event queue (using @ref SA_ResumeEventQueue).
 * @post The event queue does not have to be resumed to stop.
 * @attention New events will not be reported until the event queue resumes (@ref SA_ResumeEventQueue).
 *
 * @param[in] cda  Controller data area of controller to pause event monitoring.
 * @return [Pause Event Queue Returns](@ref storc_events).
 *
 * __Example__
 * @code
 * if (SA_StartEventQueue(cda) == kStartEventQueueSuccess)
 * {
 *    if (SA_PauseEventQueue(cda) != kPauseEventQueueSuccess)
 *    {
 *       fprintf(stderr, "Failed to pause event queue.\n");
 *    }
 *    else if (SA_ResumeEventQueue(cda) != kResumeEventQueueSuccess)
 *    {
 *       fprintf(stderr, "Failed to resume event queue.\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_PauseEventQueue(PCDA cda);
   /** Successfully paused the event monitor.
    * @outputof SA_PauseEventQueue. */
   #define kPauseEventQueueSuccess           0
   /** Failed to pause the event queue because it has not been started.
    * @outputof SA_PauseEventQueue. */
   #define kPauseEventQueueNotStarted        1
   /** Failed to pause the event queue because it is already paused.
    * @outputof SA_PauseEventQueue. */
   #define kPauseEventQueueEventQueuePaused  2
/** @} */

/** @name Resume Event Queue
 @{ */
/** Resume a paused event queue for the given controller.
 * @pre The event queue is running but paused.
 * @param[in] cda  Controller data area of controller to resume paused event monitoring.
 * @return [Resume Event Queue Returns](@ref storc_events).
 *
 * __Example__
 * @code
 * if (SA_StartEventQueue(cda) == kStartEventQueueSuccess)
 * {
 *    if (SA_PauseEventQueue(cda) != kPauseEventQueueSuccess)
 *    {
 *       fprintf(stderr, "Failed to pause event queue.\n");
 *    }
 *    else if (SA_ResumeEventQueue(cda) != kResumeEventQueueSuccess)
 *    {
 *       fprintf(stderr, "Failed to resume event queue.\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_ResumeEventQueue(PCDA cda);
   /** Successfully resume the event monitor.
    * @outputof SA_ResumeEventQueue. */
   #define kResumeEventQueueSuccess            0
   /** Failed to resume the event queue because it has not been started.
    * @outputof SA_ResumeEventQueue. */
   #define kResumeEventQueueNotStarted         1
   /** Failed to resume the event queue because it is already running.
    * @outputof SA_ResumeEventQueue. */
   #define kResumeEventQueueEventQueueRunning  2
/** @} */

#endif /* !__UEFI */

/** @defgroup storc_event_codes Event Codes
 * @details
 * | Class Code                    | Subclass Code                              | Detail Code |
 * | ----------------------------- | ------------------------------------------ | ----------- |
 * | @ref kEventClassHardwareError |                                            | |
 * |                               | @ref kEventSubClassControllerCache         | |
 * ||||
 * | @ref kEventClassEnvironment   |                                            | |
 * |                               | @ref kEventSubClassControllerPower         | |
 * |                               |                                            | @ref kEventDetailPowerMode |
 * ||||
 * | @ref kEventClassPhysicalDrive |                                            | |
 * |                               | @ref kEventSubClassPhysicalDriveStatus     | |
 * ||||
 * | @ref kEventClassLogicalDrive  |                                            | |
 * |                               | @ref kEventSubClassLogicalDriveStatus      | |
 * ||||
 * | @ref kEventClassEncryption    |                                            | |
 * |                               | @ref kEventSubClassEncryptionToggle        | |
 * |                               | @ref kEventSubClassEncryptionChange        | |
 * ||||
 *
 * @{ */

/** @name Serial Log Event Codes
 * @{ */
#define kEventClassSerialLog          11
/** @} */
/** @name Hardware Error/Failure Event Codes
 * @{ */
#define kEventClassHardwareError      2
   #define kEventSubClassControllerCache            4
/** @} */
/** @name Environment Event Codes
 * @{ */
#define kEventClassEnvironment        3
   #define kEventSubClassControllerPower            5
      #define kEventDetailPowerMode                              0
/** @} */
/** @name Logical Drive Event Codes
 * @{ */
#define kEventClassPhysicalDrive      4
   #define kEventSubClassPhysicalDriveStatus        0
/** @} */
/** @name Logical Drive Event Codes
 * @{ */
#define kEventClassLogicalDrive       5
   #define kEventSubClassLogicalDriveStatus         0
/** @} */
/** @name Encryption Event Codes
 * @{ */
#define kEventClassEncryption         14
   #define kEventSubClassEncryptionToggle           0
   #define kEventSubClassEncryptionChange           1
/** @} */

/** @} */ /* event_codes */
#if defined(__cplusplus)
}
#endif

/** @} */ /* storc_events */

/** @} */ /* storcore */

#endif /* PSTORC_EVENTS_H_ */
