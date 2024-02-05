/****************************************************************  
 * FILE        : types.h
 * DESCRIPTION : StoreLib data type definitions
 * AUTHOR      : HNR/SB
 * DATE        : 10-25-2004
 ***************************************************************** 
       Copyright 2008-2014 Avago Technologies.  All rights reserved.
 *****************************************************************

 *****************************************************************
 * Revision History:
 * 10-25-2004 : HNR/SB  : Initial Release.
 * 02-01-2005 : SB      : Changed SL_OS_LINUX to __linux__
 * 07-20-2005 : SB      : Added ifndef before every definition
 * 08-04-2005 : SB      : Removed #ifndef before every definition.
 *                        Added SL_LSI_TYPES define S32 & U32.
 *                        This is done to avoid conflict with user
 *                        defined types. To avoid using S32 and U32 in your code
 *                        define SL_LSI_TYPES in your makefile
 * 10-27-2006 : MR      : LSID100060571 Adding Solaris x86 support
 * 11-30-2006 : MR      : LSID100061969 Adding IA64 support by changing U/S 32
 *                        to INT from LONG
 ******************************************************************/
/*
* Data Type Definitions
*/
#ifndef INCLUDE_TYPES
#define INCLUDE_TYPES

typedef unsigned char       U8;
typedef char                S8;

typedef unsigned short      U16;
typedef short               S16;

#ifndef SL_LSI_TYPES
#define SL_LSI_TYPES
typedef unsigned int        U32;
typedef int                 S32;
#endif //SL_LSI_TYPES

#ifdef SL_OS_WINDOWS
typedef unsigned __int64    U64;
typedef __int64             S64;
#elif defined(__linux__) || defined(SL_OS_SOLARIS) || defined(__FreeBSD__) || defined(OS_EFI)
typedef unsigned long long  U64;
typedef long long           S64;
#endif //SL_OS_WINDOWS || __linux__ || SL_OS_SOLARIS

#endif /* INCLUDE_TYPES */
