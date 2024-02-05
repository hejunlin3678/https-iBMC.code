/******************************************************************************

                  版权所有 (C), 2004-2014, 华为技术有限公司

******************************************************************************
  文 件 名   : endian.h
  版 本 号   : 初稿
  作    者   : songtongling
  生成日期   : 2004年4月15日
  最近修改   :
  功能描述   : 处理大小字序变换
  函数列表   :
  修改历史   :
  1.日    期   : 2004年4月15日
    作    者   : songtongling
    修改内容   : 创建文件

******************************************************************************/
#ifndef __BDENDIAN_H__
#define __BDENDIAN_H__

#ifndef BD_BIG_ENDIAN
 #ifndef BD_LITTLE_ENDIAN
  #error you must define the macro BD_BIG_ENDIAN or BD_LITTLE_ENDIAN in your makefile or project settings.
 #endif
#endif

#ifdef BD_BIG_ENDIAN
 #ifdef BD_LITTLE_ENDIAN
  #error you have defined both BD_BIG_ENDIAN and BD_LITTLE_ENDIAN, only one of them need be defined.
 #endif
#endif

#ifdef BD_BIG_ENDIAN

 #define long_by_big_endian(x) x
 #define short_by_big_endian(x) x

#else

/// 将变量转换为long类型，该变量是以大字序方式存储的
 #define long_by_big_endian(l) (long)((l >> 24) | ((l >> 8) & 0xff00) | ((l << 8) & 0xff0000) | (l << 24))

/// 将变量转换为short类型，该变量是以大字序方式存储的
 #define short_by_big_endian(s) (short)((s >> 8) | (s << 8))

#endif

#ifdef BD_LITTLE_ENDIAN

 #define long_by_little_endian(x) x
 #define short_by_little_endian(x) x

#else

/// 已知该变量是以小字序方式存储的，将变量转换为本地CPU支持的long类型
 #define long_by_little_endian(l) (long)((l >> 24) | ((l >> 8) & 0xff00) | ((l << 8) & 0xff0000) | (l << 24))

/// 已知该变量是以小字序方式存储的，将变量转换为本地CPU支持的short类型
 #define short_by_little_endian(s) (short)((s >> 8) | (s << 8))

#endif

/// 将 本地的long 型变量转换为 big endian 存储格式
#define long_to_big_endian long_by_big_endian

/// 将 本地的short 型变量转换为 big endian 存储格式
#define short_to_big_endian short_by_big_endian

/// 将 本地的long 型变量转换为 little endian 存储格式
#define long_to_little_endian long_by_little_endian

/// 将 本地的short 型变量转换为 little endian 存储格式
#define short_to_little_endian short_by_little_endian

#endif
