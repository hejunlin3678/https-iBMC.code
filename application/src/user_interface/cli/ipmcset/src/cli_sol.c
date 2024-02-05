#include <termios.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "pme_app/uip/uip.h"
#include "ipmcsethelp.h"
#include "ipmcsetcommand.h"
#include "cli_sol.h"

LOCAL void sol_info_init(void);
LOCAL gint32 sol_open(void);
LOCAL void sol_close(void);
LOCAL gint32 cli_recv_sol(gchar *recv_buf, guint32 *odd_ment);
LOCAL gint32 cli_send_sol(guchar msg_type, guchar *data, guint8 data_len);
LOCAL gint32 cli_sol_activate(guchar com, gint32 mode);
LOCAL void sol_printf(const gchar *str);

struct cli_sol_info SOL_INFO = {
    .fd = -1,
    .init = sol_info_init,
    .open = sol_open,
    .close = sol_close,
    .recv_sol = cli_recv_sol,
    .send_sol = cli_send_sol,
    .activate_sol = cli_sol_activate,
};

static struct termios _saved_tio;
static gint32 _in_raw_mode = 0;

LOCAL void leave_raw_mode(void)
{
    if (!_in_raw_mode) {
        return;
    }

    if (tcsetattr(fileno(stdin), TCSADRAIN, &_saved_tio) != -1) {
        _in_raw_mode = 0;
    }
}

LOCAL void enter_raw_mode(void)
{
    struct termios tio;

    if (tcgetattr(fileno(stdin), &tio) == -1) {
        return;
    }

    _saved_tio = tio;
    tio.c_iflag |= IGNPAR;
    tio.c_iflag &= ~(ISTRIP | INLCR | IGNCR | ICRNL | IXON | IXANY | IXOFF);
    tio.c_lflag &= ~(ISIG | ICANON | ECHO | ECHOE | ECHOK | ECHONL);
    tio.c_lflag &= ~IEXTEN;
    tio.c_oflag &= ~OPOST;
    tio.c_cc[VMIN] = 1;
    tio.c_cc[VTIME] = 0;

    if (tcsetattr(fileno(stdin), TCSADRAIN, &tio) != -1) {
        _in_raw_mode = 1;
    }
}


void sol_info_init(void)
{
    SOL_INFO.key = 0;                    // 1.初始化密钥，无加密
    SOL_INFO.ucAlg = ENCRYPT_ARITH_NONE; // 2.初始化加密算法
}


LOCAL gint32 connect_socket(void)
{
    gint32 ret;
    OBJ_HANDLE obj_handle;
    gchar ip_addr[MAX_IP_LENGTH + 1] = "127.0.0.1";
    struct sockaddr_in servaddr;

    if (SOL_INFO.fd != -1) {
        (void)close(SOL_INFO.fd);
        SOL_INFO.fd = -1;
    }
    ret = dfl_get_object_handle(INNER_ETHGROUP_OBJ_NAME, &obj_handle);
    
    if (ret == RET_OK) {
        (void)memset_s(ip_addr, sizeof(ip_addr), 0, sizeof(ip_addr));
        ret = dal_get_property_value_string(obj_handle, ETH_GROUP_ATTRIBUTE_IP_ADDR, ip_addr, sizeof(ip_addr));
        if (ret != RET_OK) {
            return ret;
        }
    }
    (void)memset_s(&servaddr, sizeof(servaddr), 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    (void)inet_pton(AF_INET, ip_addr, (void *)&servaddr.sin_addr.s_addr);
    servaddr.sin_port = htons(SOL_MODULE_TCP_PORT);

    SOL_INFO.fd = socket(AF_INET, SOCK_STREAM, 0);

    if (SOL_INFO.fd == -1) {
        return ret;
    }

    if ((connect(SOL_INFO.fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)) {
        return RET_ERR;
    }
    return RET_OK;
}


gint32 sol_open(void)
{
    errno_t safe_fun_ret;
    gint32 ret;
    guchar data[MAX_CLI_DATA_LEN] = {0};
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    OBJ_HANDLE obj_handle = 0;
    guint8 data_len = 0;
    GSList *input_list = NULL;
    
    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }
    ret = dal_get_object_handle_nth(CLASS_NAME_SOL_MANAGEMENT, 0, &obj_handle);
    if (ret != RET_OK) {
        return ret;
    }

    input_list = g_slist_append(input_list, g_variant_new_int32(SOL_INFO.mode));
    ret = uip_call_class_method_with_handle("CLI", (const gchar *)username, (const gchar *)ip,
        CLASS_NAME_SOL_MANAGEMENT, obj_handle, METHOD_OPEN_SOL, input_list, NULL);
    uip_free_gvariant_list(input_list);

    switch (ret) {
        case RET_OK:
            break;
        case ERR_SOL_SESSION_DISABLED:
            g_printf("\rSOL is disabled!\r\n");
            return ret;
        case ERR_SOL_SESSION_OPEN_BY_RMCP:
            g_printf("\rConnect is busy now!\r\n");
            return ret;
        case ERR_SOL_SESSION_REACHED_MAX:
            g_printf("\rThe maximum number of SOL sessions has been reached!\r\n");
            return ret;
        case ERR_SOL_SESSION_IN_DEDICATED_MODE:
            g_printf("\rCurrent SOL session is in private mode!\r\n");
            return ret;
        case ERR_SOL_SESSION_IN_SHARED_MODE:
            g_printf("\rCurrent SOL session is in shared mode!\r\n");
            return ret;
        default:
            return ret;
    }
    
    ret = connect_socket();
    if (ret != RET_OK) {
        if (SOL_INFO.fd != -1) {
            (void)close(SOL_INFO.fd);
            SOL_INFO.fd = -1;
        }
        return RET_ERR;
    }
    

    
    data[data_len] = SOL_INFO.com_num;
    data_len++;
    
    data[data_len] = (guchar)strlen(username) + 1;
    data_len++;
    
    safe_fun_ret = memcpy_s(data + data_len, MAX_CLI_DATA_LEN - data_len, username, strlen(username));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
    data_len += (guchar)strlen(username) + 1;
    
    data[data_len] = (guchar)strlen(ip) + 1;
    data_len++;
    
    safe_fun_ret = memcpy_s(data + data_len, MAX_CLI_DATA_LEN - data_len, ip, strlen(ip));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
    data_len += strlen(ip) + 1;
    ret = SOL_INFO.send_sol(SOL_OPEN_CONNECT, data, data_len);
    if (ret != RET_OK) {
        if (SOL_INFO.fd != -1) {
            (void)close(SOL_INFO.fd);
            SOL_INFO.fd = -1;
        }
        return RET_ERR;
    }

    
    SOL_INFO.is_connect = 1;
    return RET_OK;
}

void sol_close(void)
{
    
    if (SOL_INFO.fd != -1) {
        (void)SOL_INFO.send_sol(SOL_CLOSE_CONNECT, NULL, 0);
        
        (void)close(SOL_INFO.fd);
        SOL_INFO.fd = -1;
    }
    
    SOL_INFO.is_connect = 0;
}


LOCAL gint32 send_msg(gint32 fd, const void *pData, guint32 ulDataLen)
{
    gint32 ret = 0;
    guint32 ulSentLen = 0;
    gint32 times = 0;
    if (fd < 0 || pData == NULL || ulDataLen == 0) {
        return -1;
    }

    while (!(ulSentLen == ulDataLen)) {
        ret = send(fd, (const guchar *)pData + ulSentLen, ulDataLen - ulSentLen, 0);
        if (ret < 0 || ret > ((gint32)ulDataLen - (gint32)ulSentLen)) {
            if (times < MAX_SOL_RETRY) {
                times++;
                continue;
            } else {
                return -1;
            }
        }
        times = 0;
        ulSentLen += ret;
    }
    return ulSentLen;
}

gint32 cli_send_sol(guchar msg_type, guchar *data, guint8 data_len)
{
    void *pNewSendData = NULL;
    guint16 sendlen;
    guint8 payLoadLen = data_len;
    PSOL_MSG pSolCmd;
    PSOL_MSG_HEAD pSolCmdHead;
    guchar *pSolPayLoad = NULL;
    
    if (SOL_INFO.fd < 0) {
        return RET_ERR;
    }
    
    
    sendlen = (guint16)(sizeof(SOL_MSG_HEAD)) + (guint16)(payLoadLen);
    
    
    pNewSendData = (void *)g_malloc0(sendlen);
    if (pNewSendData == NULL) {
        return RET_ERR;
    }

    pSolCmd = (PSOL_MSG)pNewSendData;
    pSolCmdHead = (PSOL_MSG_HEAD)pNewSendData;
    pSolPayLoad = (guchar *)pNewSendData + sizeof(SOL_MSG_HEAD);

    
    pSolCmdHead->head = htons(SOL_MSG_ID);
    pSolCmdHead->msg_type = msg_type;
    pSolCmdHead->blade = 0;
    pSolCmdHead->com = 0;
    pSolCmdHead->req = 0;
    pSolCmdHead->ucAlg = 0; 
    pSolCmdHead->datalen = htons(payLoadLen);

    if (sendlen > sizeof(SOL_MSG_HEAD)) {
        if (memcpy_s((pSolPayLoad), sendlen - sizeof(SOL_MSG_HEAD), data, payLoadLen) != EOK) {
            g_free(pNewSendData);
            debug_log(DLOG_ERROR, "%s: memcpy_s fail.", __FUNCTION__);
            return RET_ERR;
        }
    }

    if (send_msg(SOL_INFO.fd, (void *)pSolCmd, sendlen) != sendlen) {
        g_free(pNewSendData);
        return RET_ERR;
    }

    g_free(pNewSendData);
    return RET_OK;
}


LOCAL gint32 check_sol_timeout(void)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    gint32 timeout = -1;
    gulong end;

    
    end = vos_get_cur_time_stamp();
    
    
    if ((end - SOL_INFO.last_user_input_time) < SECOND_PER_MIN) {
        return RET_OK;
    }
    

    ret = dal_get_object_handle_nth(CLASS_NAME_SOL_MANAGEMENT, 0, &obj_handle);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    ret = dal_get_property_value_int32(obj_handle, PROPERTY_SOL_MANAGEMENT_TIMEOUT, &timeout);
    if (ret != RET_OK) {
        return RET_ERR;
    }
    
    if (timeout == 0) {
        return RET_OK;
    }

    
    if ((end - SOL_INFO.last_user_input_time) > (timeout * SECOND_PER_MIN)) {
        sol_printf("\r\nSOL session is timeout.");
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 check_heartbeat(void)
{
    guint8 heartbeat_error = 0;
    gulong end;

    
    end = vos_get_cur_time_stamp();
    
    if ((end - SOL_INFO.heart_time) > SOL_HEART_INTERVAL_SEC) {
        
        if (SOL_INFO.heart_beat == 0) {
            heartbeat_error = 1;
        } else {
            SOL_INFO.heart_beat = 0;
        }
        
        (void)SOL_INFO.send_sol(SOL_HEART_BEAT, NULL, 0);

        SOL_INFO.heart_time = vos_get_cur_time_stamp();
        
        if (heartbeat_error) {
            return VOS_ERR_TIMEOUT;
        }
    }
    return RET_OK;
}


LOCAL gint32 process_user_input(gchar *input, guint32 len, gint32 *escape_pending)
{
    gint32 ret = 0;
    gchar data[MAX_CLI_DATA_LEN] = {0};
    gint32 length = 0;
    gchar ch;
    guint32 i;

    if ((input == NULL) || (len == 0) || (escape_pending == NULL)) {
        return RET_ERR;
    }

    for (i = 0; i < len; ++i) {
        ch = input[i];
        if (*escape_pending) {
            *escape_pending = 0;
            switch (ch) {
                
                case 40:
                    g_printf("Esc( [Close SOL]\r\n");
                    ret = 1;
                    break;
                default:
                    if (ch != SOL_ESCAPE_CHARACTER_DEFAULT)
                        data[length++] = SOL_ESCAPE_CHARACTER_DEFAULT;
                    data[length++] = ch;
            }
        } else {
            if (ch == SOL_ESCAPE_CHARACTER_DEFAULT) {
                *escape_pending = 1;
                continue;
            }

            data[length++] = ch;
        }
    }

    if (length) {
        ret = SOL_INFO.send_sol(SOL_SEND_DATA, (guchar *)data, length);
    }

    return ret;
}

LOCAL gint32 process_user_data(gint32 *escape_pending)
{
    gint32 ret;
    gint32 num;
    gchar  buffer[MAX_CLI_DATA_LEN] = {0};

    
    SOL_INFO.last_user_input_time = vos_get_cur_time_stamp();
    
    memset_s(buffer, MAX_CLI_DATA_LEN, 0, MAX_CLI_DATA_LEN);
    num = read(fileno(stdin), (guchar *)buffer, MAX_CLI_DATA_LEN);
    if (num > 0) {
        
        ret = process_user_input(buffer, num, escape_pending);
        
    } else {
        ret = RET_ERR;
    }
    return ret;
}

LOCAL PSOL_MSG_HEAD get_msg_head(gchar *msg)
{
    PSOL_MSG_HEAD head = (PSOL_MSG_HEAD)msg;
    if (msg == NULL) {
        return NULL;
    }

    head->head = ntohs(head->head);
    head->datalen = ntohs(head->datalen);

    return head;
}

LOCAL void output(guchar *rsp, gint32 data_len)
{
    gint32 i = 0;
    if (rsp) {
        for (i = 0; i < data_len; ++i) {
            putc(rsp[i], stdout);
        }
        fflush(stdout);
    }
}

LOCAL void sol_printf(const gchar *str)
{
    errno_t safe_fun_ret;
    guchar sol_tip[MAX_CLI_DATA_LEN] = {0};
    gint32 sol_tip_len;
    sol_tip_len = strlen(str);
    safe_fun_ret = strncpy_s((gchar *)sol_tip, MAX_CLI_DATA_LEN, str, sol_tip_len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
    output(sol_tip, sol_tip_len);
}

LOCAL void output_sol_data(PSOL_MSG pSOLCmd, guint16 poadlen)
{
    errno_t safe_fun_ret;
    guchar *dataHead = NULL;
    guchar pDataBuf[MAX_CLI_DATA_LEN] = {0};
    if (poadlen == 0 || pSOLCmd == NULL) {
        return;
    }

    // 有效负载头指针
    dataHead = (guchar *)pSOLCmd + sizeof(SOL_MSG_HEAD);
    // 初始化，复制数据
    safe_fun_ret = memcpy_s(pDataBuf, MAX_CLI_DATA_LEN, dataHead, poadlen);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        return;
    }
    output(pDataBuf, poadlen);
    return;
}


LOCAL gint32 process_sol_message(gchar *begin, gint32 len, gchar **end)
{
    gint32 ret = 0;
    gint32 i = 0;
    PSOL_MSG_HEAD pHdr;

    if (begin == NULL || end == NULL || *end == NULL || len <= 0) {
        return -1;
    }

    
    if ((guint32)len < sizeof(SOL_MSG_HEAD)) {
        return 1;
    }

    pHdr = get_msg_head(begin);
    if (pHdr == NULL) {
        
        for (i = 1; i < len - 1; i++) {
            if (((guchar)(*(begin + i)) == 0xfe) && ((guchar)(*(begin + i + 1)) == 0xf6)) {
                begin = begin + i;
                pHdr = get_msg_head(begin);
                break;
            }
        }
        
        if ((len - i) < (gint32)(sizeof(SOL_MSG_HEAD))) {
            *end = begin + len - i;
            return 1;
        }

        
        if (pHdr == NULL) {
            return -1;
        }
    }
    
    if ((guint32)len < (pHdr->datalen + sizeof(SOL_MSG_HEAD))) {
        *end = begin + len;
        return 1;
    }

    
    PSOL_MSG pMsg = (PSOL_MSG)(begin);
    switch (pMsg->msg_head.msg_type) {
        case SOL_OPEN_CONNECT:
            break;

        case SOL_CLOSE_CONNECT:
            SOL_INFO.close();
            break;

        case SOL_HEART_BEAT:
            SOL_INFO.heart_beat = 1;
            break;

        case SOL_SEND_DATA:
            
            output_sol_data(pMsg, pMsg->msg_head.datalen);
            break;
        case SOL_IS_BUSY:
            SOL_INFO.close();
            break;
        default:
            
            for (i = 1; i < len; i++) {
                if ((guchar)(*(begin + i)) == 0xfe) {
                    *end = begin + i;
                    return ret;
                }
            }
            break;
    }
    if (pHdr != NULL) {
        *end = begin + sizeof(SOL_MSG_HEAD) + pHdr->datalen;
    }

    return ret;
}

gint32 cli_recv_sol(gchar *recv_buf, guint32 *odd_ment)
{
    errno_t safe_fun_ret = EOK;
    gint32 readbytes;
    gint32 iret = 0;
    gint32 org_odd_ment;
    gchar *sendpos = recv_buf;

    if ((recv_buf == NULL) || (odd_ment == NULL)) {
        return RET_ERR;
    }
    org_odd_ment = *odd_ment;
    if (*odd_ment > MAX_SOL_DATA_LEN) {
        (*odd_ment = 0);
        return RET_OK;
    }

    readbytes = recv(SOL_INFO.fd, (gchar *)recv_buf + org_odd_ment, MAX_SOL_DATA_LEN - org_odd_ment, 0);
    
    if (readbytes <= 0) {
        (void)close(SOL_INFO.fd);
        SOL_INFO.fd = -1;
        g_printf("\rReceive SOL message failed.\r\n");
        return RET_ERR;
    }

    
    readbytes = org_odd_ment + readbytes;
    if (readbytes > MAX_SOL_DATA_LEN) {
        readbytes = MAX_SOL_DATA_LEN;
    }

    *odd_ment = 0;
    
    while (iret != -1) {
        iret = process_sol_message(sendpos, (readbytes - (sendpos - recv_buf)), &sendpos);
        if (iret == 1) {
            if (readbytes < (sendpos - recv_buf)) {
                debug_log(DLOG_ERROR, "%s,readbytes = %d,oddment=%d.", __FUNCTION__, readbytes, *odd_ment);
                return RET_ERR;
            }
            *odd_ment = readbytes - (sendpos - recv_buf);
            if (*odd_ment > MAX_SOL_DATA_LEN) {
                return RET_ERR;
            }
            safe_fun_ret = memmove_s(recv_buf, MAX_SOL_DATA_LEN, sendpos, *odd_ment);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
                return RET_ERR;
            }
            break;
        }
    }
    return RET_OK;
}

LOCAL void task_start(void)
{
    gint32 ret = 0, tmp_fd;
    fd_set read_fds;
    struct timeval tv = { 0, 0 };
    gint32 retrySol = 0;
    gchar recvbuf[MAX_SOL_DATA_LEN] = {0};
    guint32 odd_ment = 0;
    gulong temp_tv = 0;
    gint32 escape_pending = 0;
    guchar escape_key = SOL_ESCAPE_CHARACTER_DEFAULT;

    SOL_INFO.heart_time = vos_get_cur_time_stamp();

    
    SOL_INFO.last_user_input_time = SOL_INFO.heart_time;

    
    enter_raw_mode();
    
    if (SOL_INFO.mode == 0) {
        sol_printf("\r[Connect SOL successfully! Use 'Esc(' to exit.]\r\nWarning! The SOL session is in shared mode, "
            "the operation can be viewed on another terminal.\r\n");
    } else {
        sol_printf("\r[Connect SOL successfully! Use 'Esc(' to exit.]\r\n");
    }

    while (SOL_INFO.is_connect) {
        
        ret = check_sol_timeout();
        if (ret != RET_OK) {
            SOL_INFO.close();
            continue;
        }

        
        ret = check_heartbeat();
        
        if (ret == VOS_ERR_TIMEOUT) {
            
            if (retrySol >= MAX_SOL_RETRY) {
                SOL_INFO.close();
                continue;
            } else {
                retrySol++;
            }
        }
        
        else if (SOL_INFO.heart_beat == 1) {
            retrySol = 0;
        }
        tmp_fd = SOL_INFO.fd;
        FD_ZERO(&read_fds);
        FD_SET(0, &read_fds);
        FD_SET(tmp_fd, &read_fds);
        
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        ret = select(tmp_fd + 1, &read_fds, NULL, NULL, &tv);
        if (ret < 0) {
            
            SOL_INFO.close();
            continue;
        } else if (ret == 0) {
            
            
            if (escape_pending) {
                escape_pending = 0;
                (void)SOL_INFO.send_sol(SOL_SEND_DATA, &escape_key, 1);
            }
            continue;
        }

        
        if (FD_ISSET(0, &read_fds)) {
            if (process_user_data(&escape_pending) != RET_OK) {
                SOL_INFO.close();
            }
        }
        
        else if (tmp_fd != -1 && FD_ISSET(tmp_fd, &read_fds)) {
            if (SOL_INFO.recv_sol(recvbuf, &odd_ment) != RET_OK) {
                SOL_INFO.close();
            }

            temp_tv = vos_get_cur_time_stamp();
            
            if (escape_pending && ((temp_tv - SOL_INFO.last_user_input_time) >= 1)) {
                escape_pending = 0;
                (void)SOL_INFO.send_sol(SOL_SEND_DATA, &escape_key, 1);
            }
        }
        
        
        else {
            SOL_INFO.close();
        }
    }
    
    leave_raw_mode();
    return;
}

gint32 cli_sol_activate(guchar com, gint32 mode)
{
    gint32 ret = RET_OK;
    
    SOL_INFO.init();

    SOL_INFO.com_num = com; // 串口id
    SOL_INFO.mode = mode;   // 用户连接模式:0 共享，1独占
    
    if (!SOL_INFO.is_connect) {
        ret = SOL_INFO.open();
        if (ret != RET_OK) {
            return ret;
        }
        
        task_start();
    }
    return RET_OK;
}
