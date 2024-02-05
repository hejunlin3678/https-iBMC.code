#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "smm_sol_client.h"

#define SOL_MAX_DATA_LENGTH 256
#define SOL_NULL_SOCKET (-1)
#define RET_OK 0
#define RET_ERROR (-1)
#define VOS_TRUE 1
#define VOS_FALSE 0
#define SMM_MAX_INFO_LEN 64
#define STDIN 0
#define STDOUT 1
// Local function
LOCAL gint32 process_com_info(gint32 sock, guint32 blade_number);
LOCAL gint32 process_activate_sol(gint32 sock, guint32 blade_number, guint32 com_number, gchar *user_name,
    gchar *user_ip);
LOCAL gint32 connect_smm_sol(void);
LOCAL void recv_and_display(gint32 sock);
LOCAL void read_and_send(gpointer data);
LOCAL gint32 set_terminal_attr(void);
LOCAL void reset_terminal_attr(void);
// Local variant
LOCAL struct termios g_old_terminal_attr; // Terminal attribute
LOCAL gboolean g_running_flag;            // Whether continue to run


gint32 smm_sol_start_process(enum smm_sol_request_type request, guint32 blade_number, guint32 com_number,
    gchar *user_name, gchar *user_ip)
{
    gint32 ret;
    gint32 sock;
    sock = connect_smm_sol();
    if (sock == SOL_NULL_SOCKET) {
        return RET_ERROR;
    }
    ret = send(sock, &request, sizeof(request), 0);
    if (ret <= 0) {
        close(sock);
        return RET_ERROR;
    }
    g_running_flag = VOS_TRUE;
    if (set_terminal_attr() == RET_ERROR) {
        g_printf("Set terminal attribute failed\r\n");
        close(sock);
        return RET_ERROR;
    }
    switch (request) {
        case COM_INFO:
            ret = process_com_info(sock, blade_number);
            break;
        case ACTIVATE_SOL:
            ret = process_activate_sol(sock, blade_number, com_number, user_name, user_ip);
            break;
        default:
            printf("Wrong request %d\r\n", request);
            ret = RET_ERROR;
            break;
    }
    reset_terminal_attr();
    (void)close(sock);
    return ret;
}

LOCAL gint32 process_com_info(gint32 sock, guint32 blade_number)
{
    gint32 ret;
    ret = send(sock, (gpointer)&blade_number, sizeof(blade_number), 0);
    if (ret <= 0) {
        // Send failed
        printf("Connect to smm sol failed, Send data failed\r\n");
        return RET_ERROR;
    }
    recv_and_display(sock);
    return RET_OK;
}

LOCAL gint32 process_activate_sol(gint32 sock, guint32 blade_number, guint32 com_number, gchar *user_name,
    gchar *user_ip)
{
    gint32 ret;
    gulong read_task_id = 0;
    guint32 user_name_length;
    guint32 user_ip_length;
    if (user_name == NULL || user_ip == NULL) {
        return RET_ERROR;
    }
    user_name_length = strlen(user_name);
    user_ip_length = strlen(user_ip);
    // User name
    ret = send(sock, (gpointer)&user_name_length, sizeof(guint32), 0);
    if (ret <= 0) {
        printf("Connect to smm sol error, Send data failed.\r\n");
        return RET_ERROR;
    }
    ret = send(sock, (gpointer)user_name, user_name_length, 0);
    if (ret <= 0) {
        printf("Connect to smm sol error, Send data failed.\r\n");
        return RET_ERROR;
    }
    // User ip
    ret = send(sock, (gpointer)&user_ip_length, sizeof(guint32), 0);
    if (ret <= 0) {
        printf("Connect to smm sol error, Send data failed.\r\n");
        return RET_ERROR;
    }
    ret = send(sock, (gpointer)user_ip, user_ip_length, 0);
    if (ret <= 0) {
        printf("Connect to smm sol error, Send data failed.\r\n");
        return RET_ERROR;
    }
    // Blade number
    ret = send(sock, (gpointer)&blade_number, sizeof(guint32), 0);
    if (ret <= 0) {
        printf("Connect to smm sol error, Send data failed.\r\n");
        return RET_ERROR;
    }
    // Com number
    ret = send(sock, (gpointer)&com_number, sizeof(guint32), 0);
    if (ret <= 0) {
        printf("Connect to smm sol error, Send data failed.\r\n");
        return RET_ERROR;
    }
    ret = vos_task_create(&read_task_id, "SmmSolClientSend", (TASK_ENTRY)read_and_send, INT32_TO_POINTER(sock),
        DEFAULT_PRIORITY);
    if (ret != RET_OK) {
        printf("\r\nConnect to smm sol error, Create task failed.\r\n");
        return RET_ERROR;
    }
    recv_and_display(sock);
    return RET_OK;
}

LOCAL gint32 connect_smm_sol(void)
{
    gint32 sock = SOL_NULL_SOCKET;
    gint32 ret;
    struct sockaddr_in addr;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == SOL_NULL_SOCKET) {
        // Create socket failed
        printf("Can not connection to smm sol, create socket failed\r\n");
        return SOL_NULL_SOCKET;
    }
    memset_s(&addr, sizeof(addr), 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(0x7f000001);
    addr.sin_port = htons(1101);
    ret = connect(sock, (struct sockaddr *)&addr, sizeof(addr));
    if (ret != 0) {
        // Connect failed
        printf("Connect to smm_sol failed, err code %d\r\n", errno);
        (void)close(sock);
        return SOL_NULL_SOCKET;
    }
    return sock;
}

LOCAL void recv_and_display(gint32 sock)
{
    gint32 ret;
    gchar buffer[SOL_MAX_DATA_LENGTH + 1];
    while (VOS_TRUE) {
        memset_s(buffer, SOL_MAX_DATA_LENGTH + 1, 0, SOL_MAX_DATA_LENGTH + 1);
        ret = recv(sock, buffer, SOL_MAX_DATA_LENGTH, 0);
        if (ret <= 0) {
            g_running_flag = VOS_FALSE;
            break;
        }
        ret = write(STDOUT, buffer, ret);
        if (ret <= 0) {
            g_running_flag = VOS_FALSE;
            break;
        }
    }
    return;
}

LOCAL void read_and_send(gpointer data)
{
    gint32 sock = POINTER_TO_INT32(data);
    gchar buffer[1];
    gint32 ret;
    (void)send(sock, "\n", 1, 0); 
    while (g_running_flag) {
        ret = read(STDIN, buffer, 1);
        if (ret <= 0) {
            break;
        }
        ret = send(sock, buffer, 1, 0);
        if (ret <= 0) {
            break;
        }
    }
    return;
}

LOCAL gint32 set_terminal_attr(void)
{
    struct termios newt_terminal_attr;
    (void)tcgetattr(STDIN_FILENO, &g_old_terminal_attr);
    newt_terminal_attr = g_old_terminal_attr;
    newt_terminal_attr.c_lflag &= ~(ICANON | ECHO | ISIG);
    if (tcsetattr(STDIN_FILENO, TCSANOW, &newt_terminal_attr) != 0) {
        return RET_ERROR;
    }
    return RET_OK;
}

LOCAL void reset_terminal_attr(void)
{
    (void)tcsetattr(STDIN_FILENO, TCSANOW, &g_old_terminal_attr);
}