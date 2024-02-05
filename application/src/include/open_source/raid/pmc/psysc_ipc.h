/** @file psysc_ipc.h

@copyright Copyright (c) 2021 Microchip Technology Inc. All Rights Reserved.

Description: sysCore interprocess communication header

*/

#ifndef PSYSC_IPC_H
#define PSYSC_IPC_H

/** @addtogroup syscore
 * @{
 */

/** @defgroup syscore_ipc Inter Thread Communication
 * @brief
 * @details
 * @{
 */

#if defined(__cplusplus)
extern "C" {
#endif

#ifndef SC_DISABLE_MESSAGE_QUEUE

#ifndef DOXYGEN_SHOULD_SKIP_THIS
typedef void* PMESSAGE_QUEUE;
#endif

/** @defgroup syscore_ipc_message_queues Message Queues
 * @{
 */

/** @name Create/Send/Receive/Delete A Message Queue
 *
 * Example:
 * @code
 * PMESSAGE_QUEUE mq = 0;
 * SA_UINT32 max_message_count = 10;
 * SA_UINT32 message_len = 100;
 * char message[message_len];
 * if(SC_MessageQueueCreateSafe(&mq, queue_name, queue_name_size, max_message_count, message_len))
 * {
 *    if(SC_MessageQueueSend(mq, "my msg...", message_len, kMessageQueueSendWait))
 *    {
 *       if(SC_MessageQueueReceive(mq, message, message_len, kMessageQueueReceiveWait))
 *       {
 *          fprintf("Message Received: %s\n", message);
 *       }
 *    }
 *    SC_MessageQueueDelete(mq)
 * }
 * else
 * {
 *    fprintf(stderr, "Failed to create message queue\n");
 * }
 * @endcode
 * @{ */
/** Create a message queue for sending and receiving messages (Safe).
 * (see [Create/Send/Receive/Destroy A Message Queue](@ref syscore_ipc_message_queues))
 * @post Before message queue leaves scope, the user should cleanup and free the
 * message queue using @ref SC_MessageQueueDelete.
 * @param[in] mq                 Message queue handle.
 * @param[in] queue_name         Name of the message queue.
 * @param[in] queue_name_size    Size of the name message queue buffer.
 * @param[in] max_message_count  Maximum numebr of messages the queue will hold.
 * @param[in] message_len        Maximum message size in bytes.
 * @return @ref kTrue/@ref kFalse if command was successful/unsuccessful.
 */
 SA_BOOL SC_MessageQueueCreateSafe(PMESSAGE_QUEUE *mq,
                               const char *queue_name,
                               size_t queue_name_size,
                               SA_UINT32 max_message_count,
                               SA_UINT32 message_len);

/** Create a message queue for sending and receiving messages.
 * @deprecated Use @ref SC_MessageQueueCreateSafe instead.
 * @attention This is an unsafe function which is a security risk. Please use the safe version of this function (@ref SC_MessageQueueCreateSafe).
 * (see [Create/Send/Receive/Destroy A Message Queue](@ref syscore_ipc_message_queues))
 * @post Before message queue leaves scope, the user should cleanup and free the
 * message queue using @ref SC_MessageQueueDelete.
 * @param[in] mq                 Message queue handle.
 * @param[in] queue_name         Name of the message queue.
 * @param[in] max_message_count  Maximum numebr of messages the queue will hold.
 * @param[in] message_len        Maximum message size in bytes.
 * @return @ref kTrue/@ref kFalse if command was successful/unsuccessful.
 */
 SA_BOOL SC_MessageQueueCreate(PMESSAGE_QUEUE *mq,
                               const char *queue_name,
                               SA_UINT32 max_message_count,
                               SA_UINT32 message_len);

/** Send a message.
 * (see [Create/Send/Receive/Destroy A Message Queue](@ref syscore_ipc_message_queues))
 * @pre User must create a message queue using @ref SC_MessageQueueCreateSafe before
 * using the message queue.
 * @param[in] mq           Message queue handle.
 * @param[in] message      The message to be sent through to message queue.
 * @param[in] message_len  The length of the message to be sent.
 * @param[in] flags        Flags that control the operation of the call.
 * @return @ref kTrue/@ref kFalse if command was successful/unsuccessful.
 */
 SA_BOOL SC_MessageQueueSend(PMESSAGE_QUEUE mq, const void *message, SA_UINT32 message_len, SA_UINT32 flags);
 /** Flag indicating that @ref SC_MessageQueueSend will block waiting to send the message. */
#define kMessageQueueSendWait        0x00000000
 /** Flag indicating that @ref SC_MessageQueueSend will not block waiting to send the message. */
#define kMessageQueueSendNoWait      0x00000001

/** Recieve a message.
 * (see [Create/Send/Receive/Destroy A Message Queue](@ref syscore_ipc_message_queues))
 * @pre User must create a message queue using @ref SC_MessageQueueCreateSafe before
 * using the message queue.
 * @param[in]  mq          Message queue handle.
 * @param[out] message     Message received from the message queue.
 * @param[in] message_len  The max length of the message to be received.
 * @param[in] flags        Flags that control the operation of the call.
 * @return @ref kTrue/@ref kFalse if command was successful/unsuccessful.
 */
 SA_BOOL SC_MessageQueueReceive(PMESSAGE_QUEUE mq, void *message, SA_UINT32 message_len, SA_UINT32 flags);
 /** Flag indicating that @ref SC_MessageQueueReceive will block waiting for the message. */
#define kMessageQueueReceiveWait        0x00000000
 /** Flag indicating that @ref SC_MessageQueueReceive will not block waiting for the message. */
#define kMessageQueueReceiveNoWait      0x00000001

/** Delete the message queue.
 * (see [Create/Send/Receive/Destroy A Message Queue](@ref syscore_ipc_message_queues))
 * @post Upon successful deletion mq will be invalid and should not be used in an undefined state.
 * @param[in] mq  Message queue handle.
 * @return @ref kTrue/@ref kFalse if command was successful/unsuccessful.
 */
 SA_BOOL SC_MessageQueueDelete(PMESSAGE_QUEUE mq);
 /** @} */

/** @} */ /* syscore_ipc_message_queues */

#endif /* SC_DISABLE_MESSAGE_QUEUE */

#if defined(__cplusplus)
}    /* extern "C" { */
#endif

/** @} */ /* syscore_ipc */

/** @} */ /* syscore */

#endif /* PSYSC_IPC_H */


