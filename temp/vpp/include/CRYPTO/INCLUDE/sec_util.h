/**
 * ****************************************************************************
 * *
 * Copyright (c) Huawei Technologies Co., Ltd. 2005-2019. All rights reserved.
 * Description: Contains various crypto utility functions
 * ALL RIGHTS RESERVED                                *
 * *
 * Project Code:         iPsi
 * Filename:sec_util.h
 * Module Name:          Crypto Module                                           *
 * *
 * Create:  15-07-2005
 * Author:               Sanjay Vasudevan                                        *
 * Descrption:           Contains various crypto utility functions               *
 * Modification History*
 * DATE              NAME                        DESCRIPTION                     *
 * --------------------------------------------------------------------------    *
 * YYYY-MM-DD                                                                  *
 * 2007-09-18     Jayant Bansal 72052         Cleanup                         *
 * *
 * *****************************************************************************
 */
#ifndef SEC_UTIL_H
#define SEC_UTIL_H

#include "sec_sys.h"

#ifdef __cplusplus
extern "C" {
#endif

/* This is Version 1 Release 5 of Crypto lib */
#ifndef VPP_CRYPTO_VERSION
#define VPP_CRYPTO_VERSION ""
#endif
#define CRYPTO_VERSION VPP_CRYPTO_VERSION

/**
 * @defgroup sec_common
 * This section contains the sec_common Functions and Enums
 */
/**
 * @defgroup sec_utilEnum
 * @ingroup sec_util
 * This section contains the sec_common Enum
 */
/**
 * @defgroup SEC_CRYPTO_TYPE_E
 * @ingroup Enums
 * @par Description
 * Indicates the type of crypto implementaion type.
 * @par Prototype
 * @code
 * typedef enum
 * {
 * 	SEC_SOFTWARE = 0,
 * 	SEC_MAX
 * } SEC_CRYPTO_TYPE_E;
 *
 * @endcode
 *
 * @datastruct SEC_SOFTWARE=0 Software implemenation type.
 */
typedef enum {
    SEC_SOFTWARE = 0,
    SEC_MAX
} SEC_CRYPTO_TYPE_E;

/* This data structure is defined for API's future extensibility */
/**
 * @defgroup IPSI_AUX_PAR_S
 * @ingroup Structures
 * @par Description
 * This data structure is defined for API's future extensibility.
 * @par Prototype
 * @code
 * typedef struct stAuxPar
 * {
 * SEC_UINT32 ulId;
 * SEC_VOID *pData;
 * }IPSI_AUX_PAR_S;
 * @endcode
 *
 * @datastruct ulId ID or value related to feature.
 * @datastruct pData Byte stream of data.
 *
 */
typedef struct stAuxPar {
    SEC_UINT32 ulId;
    SEC_VOID *pData;
} IPSI_AUX_PAR_S;

/**
 * @defgroup sec_util
 * This section contains the sec_util Functions.
 */
/**
 * @defgroup sec_utilFunctions
 * @ingroup sec_util
 * This section contains the sec_util Functions.
 */
/*
    Func Name:  SEC_getCryptoVersion
*/
/**
 * @defgroup SEC_getCryptoVersion
 * @ingroup sec_utilFunctions
 * @par Prototype
 * @code
 * const SEC_CHAR* SEC_getCryptoVersion
 * @endcode
 *
 * @par Purpose
 * This is used to get crypto library version.
 *
 * @par Description
 * SEC_getCryptoVersion function returns the text which specifies the version of crypto library being used.
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_CHAR* The version of Crypto library [1.3|N/A]
 *
 * @par Required Header File
 * sec_util.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL const SEC_CHAR *SEC_getCryptoVersion(void);

/*
    Func Name:  SEC_displayCryptoVersion
*/
/**
 * @defgroup  SEC_displayCryptoVersion
 * @ingroup sec_utilFunctions
 * @par Prototype
 * @code
 * SEC_VOID SEC_displayCryptoVersion()
 * @endcode
 *
 * @par Purpose
 * This is used to print crypto library version.
 *
 * @par Description
 * SEC_displayCryptoVersion function prints the text which specifies the version of crypto library being used.
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_VOID This function does not return any value [N/A|N/A]
 *
 * @par Required Header File
 * sec_util.h
 *
 * @par Note
 * \n
 * This function internally uses printf to print the Crypto Version to the console. Users are advised NOT to
 * use this function in C++ based adapter codes as it may have a conflict with std::cout.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_VOID SEC_displayCryptoVersion(void);

/*
    Func Name:  SEC_setDebugSwitch
*/
/**
 * @defgroup SEC_setDebugSwitch
 * @ingroup  sec_utilFunctions
 * @par Prototype
 * @code
 * SEC_VOID SEC_setDebugSwitch (SEC_UINT32 ulSecDebugSwitch);
 * @endcode
 *
 * @par Purpose
 * This is used to set the dbg level for crypto library.
 *
 * @par Description
 * The user can set the dbg level for crypto library using SEC_setDebugSwitch function. Calling this function with
 * SEC_DEBUG_ON, enables logging of messages which are of type dbg, warning, error and fatal. Calling this function
 * with SEC_DEBUG_OFF disables dbg and warning messages and only errors and fatal messages are logged.
 *
 * @param[in] ulSecDebugSwitch The dbg switch [N/A]
 *
 * @retval SEC_VOID This function does not return any value [N/A|N/A]
 *
 * @par Required Header File
 * sec_util.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_VOID SEC_setDebugSwitch(SEC_UINT32 ulSecDebugSwitch);

/*
    Func Name:  SEC_getDebugSwitch
*/
/**
 * @defgroup SEC_getDebugSwitch
 * @ingroup  sec_utilFunctions
 * @par Prototype
 * @code
 * SEC_UINT32 SEC_getDebugSwitch()
 * @endcode
 *
 * @par Purpose
 * This is used to get the dbg level for crypto library.
 *
 * @par Description
 * SEC_getDebugSwitch function returns the dbg level that has been set currently. The function returns SEC_DEBUG_ON
 * or SEC_DEBUG_OFF.
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_UINT32 The dbg level [1|N/A]
 *
 * @par Required Header File
 * sec_util.h
 *
 * @par Note
 * \n
 * N/A
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_UINT32 SEC_getDebugSwitch(void);

/*
    Func Name:  SEC_showDebugSwitch
*/
/**
 * @defgroup  SEC_showDebugSwitch
 * @ingroup  sec_utilFunctions
 * @par Prototype
 * @code
 * SEC_VOID SEC_showDebugSwitch()
 * @endcode
 *
 * @par Purpose
 * This is used to display whether dbg level is ON or OFF.
 *
 * @par Description
 * SEC_showDebugSwitch function displays a text which specifies if the dbg level has been set or reset. If debugging
 * is set, "Dbg Switch is ON" is displayed else "Dbg Switch is OFF" is displayed.
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_VOID This function does not return any value [N/A|N/A]
 *
 * @par Required Header File
 * sec_util.h
 *
 * @par Note
 * \n
 * This function internally uses printf to print the Dbg Switch Status to the console.
 * Users are advised NOT to use this function in C++ based adapter codes as it may
 * have a conflict with std::cout.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_VOID SEC_showDebugSwitch(void);

#ifdef __cplusplus
}
#endif

#endif
