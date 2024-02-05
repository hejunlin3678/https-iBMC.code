/******************************************************************************

                  版权所有 (C), 2001-2013, 华为技术有限公司

******************************************************************************
  文 件 名   : gpio_usr_api_ker.h
  版 本 号   : 初稿
  作    者   : w00225984
  生成日期   : 2013年8月14日
  最近修改   :
  功能描述   : gpio用户层使用到的代码
  函数列表   :
  修改历史   :
  1.日    期   : 2013年8月14日
    作    者   : w00225984
    修改内容   : 创建文件

******************************************************************************/

#define GPIO_DEV_NAME "/dev/gpio"

#define GPIO_MAX_GROUP_NUM_USR 5
#define GPIO_GROUP_MAX_PIN_NUM_USR 32
#define GPIO_GROUP5_MAX_PIN_NUM_USR 20

#define GPIO_CMD_MAGIC 'g'
#define GPIO_CMD_SET_REUSE _IOW(GPIO_CMD_MAGIC, 0, int)  //设置管脚复用
#define GPIO_CMD_GET_REUSE _IOR(GPIO_CMD_MAGIC, 1, int)  //获取管脚复用关系
#define GPIO_CMD_SETDIR _IOW(GPIO_CMD_MAGIC, 2, int)  //设置管脚方向
#define GPIO_CMD_GETDIR _IOR(GPIO_CMD_MAGIC, 3, int)  //获取管脚方向
#define GPIO_CMD_SETLEVEL _IOW(GPIO_CMD_MAGIC, 4, int)  //设置电平状态
#define GPIO_CMD_GETLEVEL _IOR(GPIO_CMD_MAGIC, 5, int)  //获取电平状态

#define GPIO_CMD_INT_CONFIG      _IOW(GPIO_CMD_MAGIC, 6, int)    
#define GPIO_CMD_GET_INT_EVENT   _IOR(GPIO_CMD_MAGIC, 7, int)  //获取电平状态
#define GPIO_CMD_OUTPUT_GETLEVEL _IOR(GPIO_CMD_MAGIC, 8, int)  //获取输出模式时的管脚状态

typedef struct
{
    unsigned int gpio_value[GPIO_MAX_GROUP_NUM_USR]; //GPIO的值,第一组到第四组有32个管脚，第五组有20个管脚
    unsigned int gpio_mask[GPIO_MAX_GROUP_NUM_USR]; //GPIO需要操作的管脚号，第一组到第四组有32个管脚，第五组有20个管脚
} GPIO_INFO;

typedef struct GPIO_INT_SET_STRU
{
    unsigned int gpio_int_num; /*外部中断数，值范围:[0,6],对于外部中断1~7*/
    unsigned int int_level;    /*中断极性 0=下降沿有效 1=上升沿有效*/
} GPIO_INT_SET_S;

typedef struct GPIO_INT_STRU
{
    unsigned int gpio_int_num; /*外部中断数，值范围:[0,6],对于外部中断1~7*/
    unsigned int timeout;       /*超时时间，值范围:[0,0xffffffff];0xffffffff表示不超时，永远等待,直到中断产生*/
    unsigned int value;         /*获取到的事件值，0:没有中断产生，1:产生中断*/
    unsigned int rsv;           /*保留*/
} GPIO_INT_S;

