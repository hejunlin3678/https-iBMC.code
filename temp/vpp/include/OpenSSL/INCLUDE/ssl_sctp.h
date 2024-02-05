/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 *
 * Filename      : ssl_sctp.h
 * Author        : n00900453
 *
 * Description   : This is the interface of ssl_sctp module.
 * Create: 2019/07/15
 * Version       : 1.0
*/
#ifndef SSL_SCTP_H
#define SSL_SCTP_H

#include <openssl/ssl.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SSL_EX_SUPPORT_DTLS_OVER_VPP_SCTP

#define BIO_SCTP_CTRL_DRY_NOTIFY 2000
#define BIO_SCTP_CTRL_SET_SSLOBJECT 2001

/* In the SCTP DRY event callback, product will invoke this */
#define BIO_SCTP_DRY_NOTIFY(b) (BIO_ctrl(b, BIO_SCTP_CTRL_DRY_NOTIFY, 0, NULL))

typedef int (*sctp_write_socket_fp)(unsigned int assoc_id, unsigned short stream_id, const char *argp, int argi,
                                    void *pvPara);

typedef int (*sctp_read_socket_fp)(unsigned int assoc_id, char *argp, int argi, void *pvPara);

typedef unsigned int (*sctp_get_active_shared_key_fp)(unsigned int assoc_id, unsigned short *shared_key_id,
                                                      void *aux_data);

typedef unsigned int (*sctp_config_shared_key_fp)(unsigned int assoc_id, unsigned short shared_key_id,
                                                  unsigned int shared_key_len, void *shared_key,
                                                  unsigned short priority, void *aux_data);

typedef unsigned int (*sctp_activate_shared_key_fp)(unsigned int assoc_id, unsigned short shared_key_id,
                                                    void *aux_data);

typedef unsigned int (*sctp_deactivate_shared_key_fp)(unsigned int assoc_id, unsigned short shared_key_id,
                                                      void *aux_data);

typedef unsigned int (*sctp_delete_shared_key_fp)(unsigned int assoc_id, unsigned short shared_key_id, void *aux_data);

typedef unsigned int (*sctp_get_su_sl_num_fp)(unsigned int assoc_id, unsigned int *sl_num);

typedef void (*sctp_activate_key_when_dry_fp)(unsigned int assoc_id, void *pvPara);

/*
    config_value = 1; Enable Dry event option
    config_value = 0; Disable Dry event option
*/
typedef unsigned int (*sctp_config_dry_event_option_fp)(unsigned int assoc_id, unsigned char config_value);

typedef struct BIO_SCTP_CB_ST {
    sctp_write_socket_fp pf_sctp_write_socket_callback_func;
    sctp_read_socket_fp pf_sctp_read_socket_callback_func;
    sctp_get_active_shared_key_fp pf_sctp_get_active_shared_key_callback_func;
    sctp_config_shared_key_fp pf_sctp_config_shared_key_callback_func;
    sctp_activate_shared_key_fp pf_sctp_activate_shared_key_callback_func;
    sctp_deactivate_shared_key_fp pf_sctp_deactivate_shared_key_callback_func;
    sctp_delete_shared_key_fp pf_sctp_delete_shared_key_callback_func;
    sctp_get_su_sl_num_fp pf_sctp_get_su_sl_num_callback_func;
    sctp_config_dry_event_option_fp pf_sctp_config_dry_event_option_callback_func;
} BIO_SCTP_CB;

/**
 * Func Name:  BIO_s_dgram_set_sctp_cb
 * @defgroup BIO_s_dgram_set_sctp_cb
 * @ingroup sslObjectFunctions
 * @par Prototype
 * @code
 * int BIO_s_dgram_set_sctp_cb(const BIO_SCTP_CB *callback_para, const void *pvPara);
 * @endcode
 *
 * @par Purpose
 * This is used to register the SCTP socket/Auth interfaces to DTLS. This step is mandatory for DTLS support over VPP
 * SCTP. If this is not called by product then the BIO_f_sctp() interface will failure.
 *
 * @par Description
 * callback_para structure with callback pointers which needs to be registered by product
 *
 * @retval 1 On success
 * @retval 0 On failure
 *
 * @par Required Header File
 * \n
 * N/A
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * \n
 * N/A
 *
 */
int BIO_s_dgram_set_sctp_cb(const BIO_SCTP_CB *callback_para);

/**
 * Func Name:  BIO_s_datagram_sctp_ex
 * @defgroup BIO_s_datagram_sctp_ex
 * @ingroup sslObjectFunctions
 * @par Prototype
 * @code
 * BIO_METHOD *BIO_s_datagram_sctp_ex(void)
 * @endcode
 *
 * @par Purpose
 * This is used to create a new BIO for VPP SCTP. BIO callbacks are registerd to custom functions to support DTLS over
 * VPP SCTP
 *
 * @par Description
 * BIO_s_datagram_sctp_ex function registers custom callback functions to support DTLS over VPP SCTP
 *
 *
 * @retval a valid pointer On success
 * @retval NULL On failure
 *
 * @par Required Header File
 * openssl/ssl.h
 * openssl/bio.h
 *
 * @par Note
 * It is user reponsibility to call the BIO_meth_free() to free the BIO_METHOD memory allocated by
 * BIO_s_datagram_sctp_ex().
 *
 * @par Related Topics
 * BIO_new.
 * BIO_s_datagram_sctp
 *
 */
BIO_METHOD *BIO_s_datagram_sctp_ex(void);

/**
 * Func Name:  SSL_dtls_dry_notify_config
 * @defgroup SSL_dtls_dry_notify_config
 * @ingroup sslObjectFunctions
 * @par Prototype
 * @code
 * void SSL_dtls_dry_notify_config(BIO *b, void* context, void *buf);
 * @endcode
 *
 * @par Purpose
 * This is the callback funtion which enables the SCTP dry event option. PDT need to set this callback for a given BIO
 * as below: BIO_dgram_sctp_notification_cb((BIO*)bio, SSL_dtls_dry_notify_config, NULL);
 *
 * @par Description
 * b : BIO pointer which is created using BIO_new
 * context : context pointer, currently not used.
 * buf: buffer pointer, currently not used.
 * @retval void
 *
 * @par Required Header File
 * openssl/ssl.h
 * openssl/bio.h
 *
 * @par Note
 * N/A
 *
 * @par Related Topics
 * BIO_new.
 * BIO_dgram_sctp_notification_cb
 *
 */
void SSL_dtls_dry_notify_config(BIO *b, void *context, void *buf);

/**
 * Func Name:  BIO_dgram_sctp_notification_cb_ex
 * @defgroup BIO_dgram_sctp_notification_cb_ex
 * @ingroup sslObjectFunctions
 * @par Prototype
 * @code
 *  int BIO_dgram_sctp_notification_cb_ex(BIO *b,
 *                                  void (*handle_notifications) (BIO *bio, void *context, void *buf),
 *                                  void *context);
 * @endcode
 *
 * @par Purpose
 * This is the same as the openssl BIO_dgram_sctp_notification_cb. But as the structure 'bio_dgram_sctp_data' is
 * modified to support vpp-sctp BIO_dgram_sctp_notification_cb_ex is adapted and exposed. User need to call as beloe:
 * BIO_dgram_sctp_notification_cb_ex((BIO*)bio, ssl_ex_dtls_dry_notify_config, NULL);
 *
 * @par Description
 * b : BIO pointer which is created using BIO_new
 * handle_notifications : ssl_ex_dtls_dry_notify_config function needs to be passed by user application.
 * buf: buffer pointer, currently not used.
 * @retval 0 upon success. -1 upon faislure
 *
 * @par Required Header File
 * openssl/ssl.h
 * openssl/bio.h
 *
 * @par Note
 * N/A
 *
 * @par Related Topics
 * BIO_new.
 * BIO_dgram_sctp_notification_cb
 * ssl_ex_dtls_dry_notify_config
 *
 */
int BIO_dgram_sctp_notification_cb_ex(BIO *b, void (*handle_notifications)(BIO *bio, void *context, void *buf),
                                      void *context);

#endif

#ifdef __cplusplus
}
#endif
#endif
