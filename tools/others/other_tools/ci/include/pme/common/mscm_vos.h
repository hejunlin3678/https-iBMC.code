#ifndef _IPMC_VOS_H_
#define _IPMC_VOS_H_

#include <glib.h>

/*公共库头文件，APP和框架只需包含mscm_vos.h*/
#include "securec.h"
#include "mscm_arithmetic.h"
#include "mscm_tar.h"
#include "mscm_macro.h"
#include "mscm_endian.h"
#include "mscm_err.h"

#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif/* __cpluscplus */
#endif /* __cpluscplus */

#ifdef __PCLINT__
#ifdef TRUE
#undef TRUE
#define TRUE 1
#endif
#endif

#define TASKID gulong
#define DEFAULT_PRIORITY 1

/* Error level definition for ErrorID */
#define URGENT     0x0000
#define SEVERE     0x4000
#define GENERAL    0x8000
#define INDICATING 0xC000

enum VOS_MID     /*   VOS MODULE ID definition */
{
    VOS_MID_TASK  = 0,       // Task
    VOS_MID_EVENT = 1,   // Event
    VOS_MID_SEMA4 = 2,   // Semaphore
    VOS_MID_MEM   = 3,       // Memory Management
    VOS_MID_MSGQ  = 4,       // Message Queue
    VOS_MID_QUEUE = 5,   // Queue
    VOS_MID_TIMER = 9,   // System Timer
    VOS_MID_BUTT
};

enum vosError
{
    VOS_OK                   = 0,
    VOS_ERR                  = (-1),
    VOS_ERR_OBJ_ID_ERROR     = -2,
    VOS_ERR_NOT_ENOUGH_MEM   = -3,
    VOS_ERR_OBJ_UNAVAILABLE  = -4,
    VOS_ERR_TIMEOUT          = -5,
    VOS_ERR_ILLEGAL_PRIORITY = -6,
    VOS_ERR_OPFAIL           = -7,
    VOS_ERR_NOTSUPPORT       = -8
};

enum infoType
{
    TASK_INFO,
    QUEUE_INFO,
    SEM4_INFO,
    TIMER_INFO,
};

/*************
queue interface
 **************/
#define QUEID gulong
#define QUE_WAIT_FOREVER 0xffffffff


#define QUEUE_WRITE_FULL   1
#define QUEUE_READ_TIMEOUT 25

#define VOS_MODULE_QUEUE             (-(VOS_MID_QUEUE << 8))

#define VOS_ERRNO_QUEUE_READ_TIMEOUT (VOS_MODULE_QUEUE | QUEUE_READ_TIMEOUT)

#define VOS_ERRNO_QUEUE_FULL         (VOS_MODULE_QUEUE | QUEUE_WRITE_FULL)

#define QUE_PRIVATE_MSG_TYPE 1UL

#define INVALID_QUEUE_ID 0xFFFFFFFF

#define QUE_IPMI_IPC_MSG_TYPE 1

/* message queue management */
enum VOS_QUEUE_ErrorType
{
    QUEUE_BADARGUMENT     = 0,
    QUEUE_NOFREEQUEUECB   = 1,
    QUEUE_CANTCREATEQUEUE = 2,
    QUEUE_NOSUCHQUEUE = 3,
    QUEUE_NOMSG = 4,
    QUEUE_CANTREADQUEUE   = 5,
    QUEUE_QUEUECOUNTERROR = 6,
    QUEUE_MEMALLOC = 7,
    QUEUE_CANTGETTASKID = 8,
    QUEUE_BLOCKERROR = 9,
    QUEUE_CANTWRITEQUEUE = 10,
    QUEUE_CANTDELQUEUE     = 11,
    QUEUE_QUEUENUMERROR    = 12,
    QUEUE_CANTGETMSGNUM    = 13,
    QUEUE_QUEUELENERROR    = 14,
    QUEUE_CANTGETQUEUEID   = 15,
    QUEUE_CANTCREATESIGNAL = 16,

    QUEUE_ERR_EXTENDING = 256,
    QUEUE_ERR_BUTT = 0x4000
};

typedef enum tag_num_type_e
{
    NUM_TPYE_CHAR,
    NUM_TPYE_UCHAR,
    NUM_TPYE_SHORT,
    NUM_TPYE_USHORT,
    NUM_TPYE_INT32,
    NUM_TPYE_UINT32,
}TAG_NUM_TYPE;

#define VOS_ERRNO_QUEUE_BADARGUMENT        (VOS_MODULE_QUEUE | QUEUE_BADARGUMENT)
#define VOS_ERRNO_QUEUE_NOFREEQUEUECB      (VOS_MODULE_QUEUE | QUEUE_NOFREEQUEUECB)
#define VOS_ERRNO_QUEUE_CANTCREATEQUEUE    (VOS_MODULE_QUEUE | QUEUE_CANTCREATEQUEUE)
#define VOS_ERRNO_QUEUE_NOSUCHQUEUE        (VOS_MODULE_QUEUE | QUEUE_NOSUCHQUEUE)
#define VOS_ERRNO_QUEUE_NOMSG              (VOS_MODULE_QUEUE | QUEUE_NOMSG)
#define VOS_ERRNO_QUEUE_CANTREADQUEUE      (VOS_MODULE_QUEUE | QUEUE_CANTREADQUEUE)
#define VOS_ERRNO_QUEUE_QUEUECOUNTERROR    (VOS_MODULE_QUEUE | QUEUE_QUEUECOUNTERROR)
#define VOS_ERRNO_QUEUE_MEMALLOC           (VOS_MODULE_QUEUE | QUEUE_MEMALLOC)
#define VOS_ERRNO_QUEUE_CANTGETTASKID      (VOS_MODULE_QUEUE | QUEUE_CANTGETTASKID)
#define VOS_ERRNO_QUEUE_BLOCKERROR         (VOS_MODULE_QUEUE | QUEUE_BLOCKERROR)
#define VOS_ERRNO_QUEUE_CANTWRITEQUEUE     (VOS_MODULE_QUEUE | QUEUE_CANTWRITEQUEUE)
#define VOS_ERRNO_QUEUE_CANTDELQUEUE       (VOS_MODULE_QUEUE | QUEUE_CANTDELQUEUE)
#define VOS_ERRNO_QUEUE_QUEUENUMERROR      (VOS_MODULE_QUEUE | QUEUE_QUEUENUMERROR)
#define VOS_ERRNO_QUEUE_CANTGETMSGNUM      (VOS_MODULE_QUEUE | QUEUE_CANTGETMSGNUM)
#define VOS_ERRNO_QUEUE_QUEUELENERROR      (VOS_MODULE_QUEUE | QUEUE_QUEUELENERROR)
#define VOS_ERRNO_QUEUE_CANTGETQUEUEID     (VOS_MODULE_QUEUE | QUEUE_CANTGETQUEUEID)

// Level 0 : Urgent

/* Level 0 : Urgent */
#define VOS_ERRID_QUEUE_NOFREEQUEUECB (URGENT | QUEUE_NOFREEQUEUECB)

// Level 1 : Severe
#define VOS_ERRID_QUEUE_CANTCREATEQUEUE (SEVERE | QUEUE_CANTCREATEQUEUE)
#define VOS_ERRID_QUEUE_NOSUCHQUEUE (SEVERE | QUEUE_NOSUCHQUEUE)
#define VOS_ERRID_QUEUE_CANTREADQUEUE (SEVERE | QUEUE_CANTREADQUEUE)
#define VOS_ERRID_QUEUE_QUEUECOUNTERROR (SEVERE | QUEUE_QUEUECOUNTERROR)
#define VOS_ERRID_QUEUE_MEMALLOC (SEVERE | QUEUE_MEMALLOC)
#define VOS_ERRID_QUEUE_CANTGETTASKID (SEVERE | QUEUE_CANTGETTASKID)
#define VOS_ERRID_QUEUE_BLOCKERROR (SEVERE | QUEUE_BLOCKERROR)
#define VOS_ERRID_QUEUE_CANTWRITEQUEUE (SEVERE | QUEUE_CANTWRITEQUEUE)
#define VOS_ERRID_QUEUE_CANTDELQUEUE (SEVERE | QUEUE_CANTDELQUEUE)
#define VOS_ERRID_QUEUE_QUEUENUMERROR (SEVERE | QUEUE_QUEUENUMERROR)
#define VOS_ERRID_QUEUE_CANTGETMSGNUM (SEVERE | QUEUE_CANTGETMSGNUM)
#define VOS_ERRID_QUEUE_QUEUELENERROR (SEVERE | QUEUE_QUEUELENERROR)
#define VOS_ERRID_QUEUE_CANTGETQUEUEID (SEVERE | QUEUE_CANTGETQUEUEID)
#define VOS_ERRNO_QUEUE_SIGNALCREATE (SEVERE | QUEUE_CANTCREATESIGNAL)

// Level 2 : General
#define VOS_ERRID_QUEUE_BADARGUMENT (GENERAL | QUEUE_BADARGUMENT)
#define VOS_ERRID_QUEUE_NOMSG (GENERAL | QUEUE_NOMSG)

/*timer interface*/
#define    TIMER_MODULE (VOS_MID_TIMER << 8)

/*
 **********************************************************************************
 *                                              ERROR CODES
 **********************************************************************************
 */

#define    TIMER_OK 0
#define    TIMER_TYPE_ERR (TIMER_MODULE | 1)
#define    TIMER_PARA_ERR (TIMER_MODULE | 2)

#define    TIMER_TIMEOUT (TIMER_MODULE | 3)
#define    TIMER_NOT_EXIST (TIMER_MODULE | 4)

#define    TIMER_EMPTY (TIMER_MODULE | 5)

#define    TIMER_DEL_ERR (TIMER_MODULE | 6)

#define    TIMER_NO_MORE_CB (TIMER_MODULE | 7)

#define    TIMER_PEND_TOOMANY_TASK (TIMER_MODULE | 8)

#define    TIMER_SCHEDULE_FAIL (TIMER_MODULE | 9)

#define    TEMP_TIMER 0
#define    INVALID_TIMER_NUM 0

#define   IGNORE_LENGTH -1

//安全漏洞 : 过滤敏感字符
#define EXCLUDE_CHARS "= #&'\"<>()%+\\;$|/"


/*操作事件类型定义*/
#define IMANA_LOG_DETAILMSG_SIZE 512
#define IMANA_LOG_SECURITY    1
#define IMANA_LOG_OPERATION   2

/****************
sem4 interface
 *****************/
#define  SEMID gulong
#define SEM_WAIT_FOREVER 0UL

enum VOS_Sema4_ErrorType
{
    SEMA4_OBJTFULL = 0,
    SEMA4_OBJNF = 1,
    SEMA4_NOMEM4CB = 2, /* No sufficient memory for CB.*/
    SEMA4_TIMEOUT = 3,
    SEMA4_CANTP   = 4,
    SEMA4_CANTV   = 5,
    SEMA4_CANTDEL = 6,

    SEMA4_ID_ERROR = 8,
    SEMA4_OBJ_UNAVAILABLE  = 9,
    SEMA4_ILLEGAL_PRIORITY = 10,
    SEMA4_OPFAIL = 11,
    SEMA4_NOTSUPPORT = 12,

    SEMA4_P_IDERR = 16,  /* In P sem Id err                                  */
    SEMA4_P_NOTACTIVE = 17,  /* In P sem isn't active                            */
    SEMA4_P_TYPEERR = 19,  /* In P  sem type err                               */
    SEMA4_P_CANOTP  = 20,                  /* In P  P operation err                            */
    SEMA4_P_TIMEOUT = 21,  /* In P  P operation time out                       */

    SEMA4_ERR_EXTENDING = 0x100,  /* Above is error code at exit point */

    SEMA4_ERR_BUTT = 0x4000  /* Above is error code during processing */
};


#define VOS_SEMA4_BIN_FULL 1
#define VOS_SEMA4_BIN_EMPTY 0

#define VOS_SEMA4_MUT_FULL 1
#define VOS_SEMA4_MUT_EMPTY 0

#define VOS_SEMA4_FIFO 0x00000001
#define VOS_SEMA4_PRIOR 0x00000002
#define VOS_SEMA4_DELETE_SAFE 0x00000004
#define VOS_SEMA4_INVERSION_SAFE 0x00000008
#define VOS_SEMA4_GLOBAL 0x00000010

#define  VOS_MODULE_SEMA4 ((VOS_MID_SEMA4 << 8))

#define NULL_TID (pthread_t) 0L
#define SEM_TYPE_ERR (VOS_MODULE_SEMA4 | SEMA4_P_TYPEERR)

#define VOS_ERRNO_SEMA4_OBJTFULL (VOS_MODULE_SEMA4 | SEMA4_OBJTFULL)
#define VOS_ERRNO_SEMA4_OBJNF (VOS_MODULE_SEMA4 | SEMA4_OBJNF)
#define VOS_ERRNO_SEMA4_NOMEM4CB (VOS_MODULE_SEMA4 | SEMA4_NOMEM4CB)
#define VOS_ERRNO_SEMA4_TIMEOUT (VOS_MODULE_SEMA4 | SEMA4_TIMEOUT)
#define VOS_ERRNO_SEMA4_CANTP (VOS_MODULE_SEMA4 | SEMA4_CANTP)
#define VOS_ERRNO_SEMA4_CANTV (VOS_MODULE_SEMA4 | SEMA4_CANTV)
#define VOS_ERRNO_SEMA4_CANTDEL (VOS_MODULE_SEMA4 | SEMA4_CANTDEL)
#define VOS_ERRNO_SEMA4_TYPEERR (VOS_MODULE_SEMA4 | SEMA4_P_TYPEERR)
#define VOS_ERRNO_SEMA4_ID_ERROR (VOS_MODULE_SEMA4 | SEMA4_ID_ERROR)
#define VOS_ERRNO_SEMA4_OBJ_UNAVAILABLE (VOS_MODULE_SEMA4 | SEMA4_OBJ_UNAVAILABLE)
#define VOS_ERRNO_SEMA4_ILLEGAL_PRIORITY (VOS_MODULE_SEMA4 | SEMA4_ILLEGAL_PRIORITY)
#define VOS_ERRNO_SEMA4_OPFAIL (VOS_MODULE_SEMA4 | SEMA4_OPFAIL)
#define VOS_ERRNO_SEMA4_NOTSUPPORT (VOS_MODULE_SEMA4 | SEMA4_NOTSUPPORT)

/* Error ID definition */
/* Level 0:Urgent */
#define VOS_ERRID_SEMA4_OBJTFULL (URGENT | SEMA4_OBJTFULL)
#define VOS_ERRID_SEMA4_NOMEM4CB (URGENT | SEMA4_NOMEM4CB)

/* Level 1:Severe */
#define VOS_ERRID_SEMA4_OBJNF (SEVERE | SEMA4_OBJNF)
#define VOS_ERRID_SEMA4_TIMEOUT (SEVERE | SEMA4_TIMEOUT)
#define VOS_ERRID_SEMA4_CANTP (SEVERE | SEMA4_CANTP)
#define VOS_ERRID_SEMA4_CANTV (SEVERE | SEMA4_CANTV)
#define VOS_ERRID_SEMA4_CANTDEL (SEVERE | SEMA4_CANTDEL)

/*************************************************************
Data Structure defination
 **************************************************************/

/*************
task interface
 **************/

typedef void *(*_LINUX_TASK_ENTRY)( void * );
typedef void (*TASK_ENTRY)(void*);

typedef struct tagSysInfo
{
    gchar          name[32];
    gulong id;
    guchar type;
    guchar used;
} SYS_INFO_T, *PSYS_INFO_T;

/****************
timer interface
 *****************/
typedef    gushort TIMERID;
typedef  void (*TIMERFUN)(gulong arg);

typedef struct timer_cb
{
    gulong delay; // 单位: ms
    gulong initTime;
    TIMERFUN      pfn;
    gulong arg;

    // gulong      taskid;
    TIMERID *ptid;    // 指向队列ID变量的地址
} TIMER_CB, *PTIMER_CB;

#define BUFFER_SIZE 8192

typedef struct tag_que_msg
{
    glong mtype;
    gchar buffer[BUFFER_SIZE + 1];
} QUE_MSG_S;


typedef struct tag_rtc_time
{
    guchar  ucSecond;    //秒 0~59
    guchar  ucMinutes;   //分钟0~59
    guchar  ucHours;     //小时0~23
    guchar  ucDay;       //星期1~7
    guchar  ucDate;      //日1~31、30，28润月润年自动计算
    guchar  ucMonth;     //月1~12
    gushort wYear;
} RTC_TIME_S;


/*begin uboot env*/
#ifdef __CALL_TRACE__
 #define _CALL_TRACE() g_printf("call %s\n", __FUNCTION__)
#else
 #define _CALL_TRACE()
#endif

#ifdef IPMC_DEBUG
 #define IPMC_DBG(fmt, args...) g_printf(fmt, ## args)
#else
 #define IPMC_DBG(fmt, args...) do { } while (0)
#endif


#ifndef MAP_FAILED
#define MAP_FAILED (-1)
#endif

#define  INTF_OK 0
#define  INTF_EFAULT -1         /* Bad address */
#define  INTF_EINVAL -2         /* Invalid argument */
#define  INTF_ENODEV -3         /* No such device */
#define  INTF_EBUSY -4      /* Device or resource busy */
#define  INTF_EIO -5        /* I/O error */
#define  INTF_ENOMEM -6     /* Out of memory */
#define  INTF_EACCES -7        /* Permission denied */
#define  INTF_EROFS -8         /* Read-only file system */
#define  INTF_ESTATUS -9       /* status error */
#define  INTF_EINTERFACE -10    /*interface not suppprt*/
#define  INTF_ETIMEOUT -11      /*interface not suppprt*/
#define  INTF_EIN_PROG -12      /*in progress*/
#define  INTF_ECHIP -13         /*Chip error*/
#define  INTF_VOLATILE -14    /*value not stabilization*/
#define  INTF_ECRC -15    /*Data crc check error*/


/* 现在设备名已经变化 */
#define DEV_ENV_NAME "/dev/mtdblock5"

/* 由128K修改为8K,与uboot中保持一致 */
#define MTD_SIZE 0x2000   /* 8K */
#define ENV_SIZE (MTD_SIZE - sizeof(unsigned long))

// 默认队列权限
 #define DEFAULT_IPC_PRIV   (0666)
 //受限队列权限
#define DISTRICT_IPC_PRIV   0


struct environment_s
{
    gulong crc;                          /* CRC32 over data bytes */
    guchar data[ENV_SIZE];       /* Environment data */
};

extern gint32 get_uboot_env(const guchar *name, guchar *val, gint32 length);
extern gint32 set_uboot_env(const guchar *name, const guchar *val);

extern gint32             print_uboot_env(const guchar *name);
extern gint32             set_uboot_baudrate(const guchar *val);
extern gint32             get_uboot_baudrate(guchar *val, gint32 length);
extern gint32             set_uboot_bootdelay(const guchar *val);
extern gint32             get_uboot_bootdelay(guchar *val, gint32 length);
extern gint32             set_uboot_bootorder(const guchar *val);
extern gint32             get_uboot_bootorder(guchar *val, gint32 length);
extern gint32             set_uboot_bootvalidflag(gint32 index, const guchar *val);
extern gint32             get_uboot_bootvalidflag(gint32 index, guchar *val, gint32 length);
extern gint32             set_uboot_bootretrynum(const guchar *val);
extern gint32             get_uboot_bootretrynum(guchar *val, gint32 length);
extern gint32             set_uboot_ethaddr(const guchar *val);
extern gint32             get_uboot_ethaddr(guchar *val, gint32 length);
extern gint32             set_uboot_ipaddr(const guchar *val);
extern gint32             get_uboot_ipaddr(guchar *val, gint32 length);
extern gint32             set_uboot_serverip(const guchar *val);
extern gint32             get_uboot_serverip(guchar *val, gint32 length);
extern gint32             set_uboot_gatewayip(const guchar *val);
extern gint32             get_uboot_gatewayip(guchar *val, gint32 length);
extern gint32             set_uboot_netmask(const guchar *val);
extern gint32             get_uboot_netmask(guchar *val, gint32 length);
extern gint32             get_uboot_version(guchar *val, gint32 length);
extern gint32             set_uboot_imageversion(gint32 bootorder, const guchar *val);
extern gint32             get_uboot_imageversion(gint32 bootorder, guchar *val, gint32 length);
extern gint32             get_uboot_neverboot(guchar *val, gint32 length);
extern gint32             set_uboot_neverboot(guchar *val);
extern gint32             get_uboot_ddrtest(guchar *val, gint32 length);
extern gint32             set_uboot_ddrtest(guchar *val);
/*end uboot env*/

/*删除调试信息节点*/
extern void     vos_delete_sys_info(PSYS_INFO_T pInfo, gint32 size, gulong id,
                                                               gint32 silence);
extern RTC_TIME_S vos_gettime_bysecond(gushort from_year, guint32 s);

/*************
task interface
 **************/

/*获取以微妙数表示的系统时间*/
#ifndef WIN32
extern guint64 vos_get_sys_us_count(void);
#else
extern unsigned __int64 vos_get_sys_us_count(void);
#endif

/*获取文件所在目录*/
extern gint32 vos_get_file_dir(const gchar *filename, gchar *filepath, gint32 path_len);
extern gint32 vos_get_file_name(const gchar *filepath, gchar *filename, gint32 name_len);

/*将源文件拷贝到目标文件*/
extern gint32 vos_file_copy(const gchar *desfilepath, const gchar *sourcepath);

/*强制删除指定目录*/
extern gint32 vos_rm_filepath(const gchar* path);
extern gulong vos_get_free_ram(void);
extern void   vos_imana_log(gint32 event_type, gchar *username, gchar *client, gchar *target, gchar *msg_detail);
extern glong  vos_get_file_length(const gchar *filename);
extern gint32 vos_get_file_accessible(const gchar *filename);
extern gulong vos_get_cur_time_stamp(void);
extern glong vos_get_total_free_mem(void);

/*设置当前时间*/
extern void   vos_set_cur_time_stamp(gulong timestamp);

/*获取系统运行以来的jeffies数*/
extern gulong  vos_tick_get(void);

/* 获取随机数 */
extern guint32 vos_get_random(void);

/* 获取指定长度的随机数 */
extern gint32 vos_get_random_array(guchar *buffer, gint32 length);


/*进程实例互斥接口*/
extern void vos_process_mutex(const gchar* process_name, const gchar* mutex_file_name);

/*进程同步锁接口*/
extern gint32   vos_process_sync_lock(const gchar* sync_file_name);
extern void vos_process_sync_unlock(gint32 sync_file_fd);

/*使任务进入休眠状态一段时间*/
extern void vos_task_delay(gulong millisecond);

/*创建任务*/
extern gint32   vos_task_create(gulong *pulTaskID,const gchar* puchName,
                                               TASK_ENTRY pfnStartRoutine,
                                               void *pArg,
                                               gint32 SchedPriority);

/*删除任务*/
extern gint32   vos_task_delete(gulong ulTaskID );

/*判断任务是否存在*/
extern gint32   vos_task_exist(gulong ulTaskId, const gchar* cszName);

/*获取任务调试信息块地址*/
PSYS_INFO_T     vos_get_task_info(gint32 *info_size);

/*************
queue interface
 **************/

/*创建队列并打开*/
extern gint32 vos_process_queue_create(gulong *qid, const gchar* qname, gulong item_size,
                                                            gulong qlength);

/*删除队列*/
extern gint32                       vos_process_queue_delete(gulong ulQueueID );

/*向队列发送消息*/
gint32                              vos_process_queue_send(gulong ulqid,QUE_MSG_S* send_buf, gulong msgsize);


/*从队列收取消息*/
extern gint32                       vos_process_queue_receive(gulong ulqid, QUE_MSG_S* send_buf, gulong msgsize,
                                                             gulong ulTimeOut);

/*设置队列存取模式*/
extern gint32 vos_process_queue_setmode(gulong ulqid, guint16 mode);




/*获取队列调试信息块地址*/
PSYS_INFO_T                                vos_get_queue_info(gint32 *info_size);

/*线程间消息队列*/
extern gint32 vos_queue_create(GAsyncQueue **queue);
extern gint32 vos_queue_delete(GAsyncQueue **queue);
extern gint32 vos_queue_get_msg_num( GAsyncQueue *queue,gint32 *msg_num);
extern gint32 vos_queue_send( GAsyncQueue *queue, gpointer data);
extern gint32 vos_queue_receive(GAsyncQueue *queue,gpointer* data, guint64 timeout);

/****************
timer interface
 *****************/

/*创建定时器*/
extern gint32                      vos_timer_create(TIMERID *ptid, TIMERFUN callback, gulong time,
                                                            gulong arg);

/*删除定时器*/
extern gint32                      vos_timer_delete(TIMERID tid);

/*重设定时器超时时间*/
extern gint32                      vos_timer_restart(TIMERID tid);

/*已字符串方式查询定时器信息*/
extern void                                vos_show_timer_info( gchar* print_buffer);

/***************
sem4 interface
 ****************/

/*二元信号量创建*/
extern gint32                       vos_thread_binary_sem4_create(gulong * Sm_ID, gchar Sm_Name[4],
                                                                  gulong Sm_Init);

/*计数信号量创建*/
extern gint32                       vos_thread_counter_sem4_create(gulong * Sm_ID, gchar Sm_Name[4],
                                                                   gulong Sm_Init);

/*删除信号量，不区分信号量类型*/
extern gint32                       vos_thread_sem4_delete( gulong Sm_ID );

/*互斥信号量创建*/
extern gint32                       vos_thread_mutex_sem4_create(gulong * Sm_ID, gchar Sm_Name[4]);

/*获得信号量，不区分类型*/
extern gint32                       vos_thread_sem4_p( gulong Sm_ID, gulong ulTimeOutInMillSec );

/*获得只读锁*/
extern gint32                       vos_thread_readonly_sem4_p(gulong Sm_ID);

/*释放信号量，不区分类型*/
extern gint32                       vos_thread_sem4_v( gulong Sm_ID );

/*获得读写锁*/
extern gint32                       vos_thread_writeonly_sem4_p(gulong Sm_ID);

/*获得异步锁*/
extern gint32                       vos_thread_async_sem4_p(gulong Sm_ID);

/*获得异步只读锁*/
extern gint32                       vos_thread_async_readonly_sem4_p(gulong Sm_ID);

/*获得异步只写锁*/
extern gint32                       vos_thread_async_writeonly_sem4_p(gulong Sm_ID);

extern gint32 vos_process_sem_getinfo(gint32 *semmsl, gint32 *semmns, gint32 *semopm, gint32 *semmni);
extern gint32 vos_process_sem_create(const gchar* fname, int members);
extern gint32 vos_process_sem_get(gchar* fname);
extern gint32 vos_process_sem_setval(gint32 sem_set_id, gint32 max_members, gint32 value);
extern gint32 vos_process_sem_destroy(gint32 sem_set_id );
extern gint32 vos_process_sem_p(gint32 sem_set_id, int members );
extern gint32 vos_process_sem_mp(gint32 sem_set_id, int members, int optimes);
extern gint32 vos_process_sem_v(gint32 sem_set_id, int members );
extern gint32 vos_process_sem_mv(gint32 sem_set_id, int members, int optimes);
extern gint32 vos_process_sem_timeout_p(gint32 sem_set_id, int members , struct timespec *timeout);
extern gint32 vos_process_sem_timeout_mp(gint32 sem_set_id, int members, int optimes, struct timespec *timeout);
/*获得信号量调试信息块地址*/
PSYS_INFO_T                                vos_get_sem4_info(gint32 *info_size);

/* 递归创建文件夹 */
extern gint32 vos_mkdir_recursive(const gchar* path);


/*计算累加和函数*/
extern guchar vos_calc_check_sum(const guchar* pData, guint length);

/*校验累加和函数*/
extern guchar vos_verify_check_sum(const guchar* pData, guint length);

/*检查文件夹是否存在,不存在就创建一个*/
extern gint32 vos_check_dir(const gchar * dirpath);
extern gint32 vos_check_incorrect_char(const gchar * srcstr, const gchar * exclude, gint32 srclen );
extern gint32 vos_ipv6_addr_valid_check(const guchar * ipv6Str);
extern gint32 vos_ipv4_addr_valid_check(const guchar * ipv4);
extern guint32 vos_get_random_range(guint32 from, guint to);

/* 库函数封装 */
extern gchar *vos_fgets(gchar *s, gint32 size, FILE *stream);
extern gint vos_system(const gchar * cmdstring);
extern gint32 vos_str2int(const gchar* src_string, gint base,
                                void *num, TAG_NUM_TYPE num_type );
extern gint vos_system_s(const gchar*path,  gchar *cmdstring[] );
extern gchar *vos_fgets_s(gchar *s, gint size, FILE *stream);

#ifdef __cplusplus
 #if __cplusplus
}
 #endif/* __cpluscplus */
#endif /* __cpluscplus */

#endif /*_IPMC_VOS_H_*/
