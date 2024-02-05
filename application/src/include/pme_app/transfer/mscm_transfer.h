#ifndef __TRANSFER_H__
#define __TRANSFER_H__

#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif
#endif /* __cplusplus */

typedef struct tag_user_info
{
    gchar * user;
    gchar * password;
    gchar * ip;
} user_info;

typedef struct CLIENT_STRUCT
{
    gchar *server;
    gchar *from_mail;
    gchar *domain;
    gchar *user;
    gchar *passwd;
    gint32   port;
    gint32   sockfd;
} CLIENT_STRUCT;

typedef struct MAIL_STRUCT
{
    const gchar *to;
    gchar * cc;
    gchar * subject;
    gchar *body;
    gchar * attachment;
} MAIL_STRUCT;

#define RESPONSE_MAX_LEN 256

extern gint32 transfer_send_mail(CLIENT_STRUCT *client, MAIL_STRUCT *mail, guint8 secrity_mode);

#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif /* __cplusplus */
#endif
