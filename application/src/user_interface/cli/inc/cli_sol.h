#ifndef __CLI_SOL_H__
#define __CLI_SOL_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 


#define SOL_MSG_ID 0xFEF6
#define MAX_SOL_DATA_LEN 1024
#define MAX_CLI_DATA_LEN 200
#define MAX_IP_LENGTH 32


#define SOL_MODULE_TCP_PORT 8210  
#define SOL_OPEN_CONNECT 0x01     
#define SOL_CLOSE_CONNECT 0x02    
#define SOL_HEART_BEAT 0x03       
#define SOL_SEND_DATA 0x04        
#define SOL_IS_BUSY 0x05          
#define SOL_GET_VER 0x06          
#define SOL_KEY_ERROR 0x07        
#define SOL_HEART_INTERVAL_SEC 20 
#define MAX_SOL_RETRY 3           

#define SOL_IS_CONNECT 1
#define SOL_IS_CLOSE 0


#define SOL_ESCAPE_CHARACTER_DEFAULT 27

#pragma pack(1)


#define ENCRYPT_ARITH_NONE 0 


// struct cli_sol_crypt_alg
// {
//    guchar ucAlg;      
//    guchar ucKeyLen;   
//    guchar ucIVLen;    
//    guchar ucAlignLen; 
//    guchar bIsSupport; 
//    /*加密函数*/
//    EncryptData_CALLBACK EncryptData;
//    /*解密函数*/
//    DecryptData_CALLBACK DecryptData;
// };


typedef struct cli_sol_head {
    guint16 head;    
    guchar msg_type; 
    guchar blade;
    guchar com;
    guchar req;
    guchar ucAlg; 
    guint16 datalen;
} SOL_MSG_HEAD, *PSOL_MSG_HEAD;



typedef struct cli_sol_msg {
    SOL_MSG_HEAD msg_head;
    guchar *msg_payload;
} SOL_MSG, *PSOL_MSG;
#pragma pack()

struct cli_sol_info {
    gint32 fd;                   
    guchar com_num;              
    guchar is_connect;           
    gint32 mode;                 
    guchar heart_beat;           
    gulong heart_time;           
    gulong last_user_input_time; 

    guint32 key;  
    guchar ucAlg; 

    void (*init)(void);
    gint32 (*open)(void);
    void (*close)(void);
    gint32 (*recv_sol)(gchar *, guint32 *);
    gint32 (*send_sol)(guchar, guchar *, guint8);
    gint32 (*activate_sol)(guchar, gint32);
};
extern struct cli_sol_info SOL_INFO;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 


#endif 
