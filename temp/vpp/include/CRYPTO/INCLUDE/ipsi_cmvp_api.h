/**
 * ***************************************************************************
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description:
          提供标准中密码模块对外接口定义
 * File Name: ipsi_cmvp_api.h
 * Create: 2022-05-08
 * Author:
 *****************************************************************************
 */
#ifndef IPSI_CMVP_API_H
#define IPSI_CMVP_API_H

#include "sec_sys.h"
#include "ipsi_cmvp_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ipsi_cmvp_api
 * This provides API for CMVP Function.
 */
/**
 * @defgroup IPSI_CMVP_InitSetKey
 * @ingroup ipsi_cmvp_api
 * @par Prototype
 * @code
 * IPSI_CMVP_InitSetKey(const SEC_UCHAR *key, SEC_UINT32 keyLen)
 * @endcode
 *
 * @par Purpose
 * Call this API to setting default key for library integrity checking
 *
 * @par Description
 * This interface must be called before IPSI_CMVP_Init.
 *
 * @par Parameters
 * @param key    [in]  Key used for integrity check when HMAC verification is used.
 * @param keyLen [in]  Key length.
 *
 * @retval SEC_INT32 (Status Output) On successful return. [SEC_SUCCESS|N/A]
 * @retval SEC_INT32 (Status Output) On failure. [SEC_ERROR|N/A]
 *
 * @par Required Header File
 * ipsi_cmvp_api.h
 *
 * @par Note
 * This API is exposed for the products which are going for CMVP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_INT32 IPSI_CMVP_InitSetKey(const SEC_UCHAR *key, SEC_UINT32 keyLen);

/**
 * @defgroup ipsi_cmvp_api
 * This provides API for CMVP Function.
 */
/**
 * @defgroup IPSI_CMVP_InitSetMode
 * @ingroup ipsi_cmvp_api
 * @par Prototype
 * @code
 * IPSI_CMVP_InitSetMode(IPSI_CMVP_CryptoMode workMode)
 * @endcode
 *
 * @par Purpose
 * Call this API to setting default mode of crypto library
 *
 * @par Description
 * This interface must be called before IPSI_CMVP_Init.
 *
 * @par Parameters
 * @param workMode [in] standard mode enum.
 *
 * @retval SEC_INT32 (Status Output) On successful return. [SEC_SUCCESS|N/A]
 * @retval SEC_INT32 (Status Output) On failure. [SEC_ERROR|N/A]
 *
 * @par Required Header File
 * ipsi_cmvp_api.h
 *
 * @par Note
 * This API is exposed for the products which are going for CMVP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_INT32 IPSI_CMVP_InitSetMode(IPSI_CMVP_CryptoMode workMode);

/**
 * @defgroup ipsi_cmvp_api
 * This provides API for CMVP Function.
 */
/**
 * @defgroup IPSI_CMVP_InitSetIntegrityChk
 * @ingroup ipsi_cmvp_api
 * @par Prototype
 * @code
 * IPSI_CMVP_InitSetIntegrityChk(IPSI_CMVP_IntegrityChkSwitch sw)
 * @endcode
 *
 * @par Purpose
 * Call this API to enable/disable crypto library integrity check.
 *
 * @par Description
 * This interface must be called before IPSI_CMVP_Init.
 *
 * @par Parameters
 * @param sw [in] Integrity check switch.
 *
 * @retval SEC_INT32 (Status Output) On successful return. [SEC_SUCCESS|N/A]
 * @retval SEC_INT32 (Status Output) On failure. [SEC_ERROR|N/A]
 *
 * @par Required Header File
 * ipsi_cmvp_api.h
 *
 * @par Note
 * This API is exposed for the products which are going for CMVP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_INT32 IPSI_CMVP_InitSetIntegrityChk(IPSI_CMVP_IntegrityChkSwitch sw);

/**
 * @defgroup ipsi_cmvp_api
 * This provides API for CMVP Function.
 */
/**
 * @defgroup IPSI_CMVP_Init
 * @ingroup ipsi_cmvp_api
 * @par Prototype
 * @code
 * SEC_INT32 IPSI_CMVP_Init(SEC_VOID)
 * @endcode
 *
 * @par Purpose
 * Initializing the CMVP Module
 *
 * @par Description
 * This interface must be invoked before being used.
 * This initialization interface does not trigger integrity check.
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_INT32 (Status Output) On successful return. [SEC_SUCCESS|N/A]
 * @retval SEC_INT32 (Status Output) On failure. [SEC_ERROR|N/A]
 *
 * @par Required Header File
 * ipsi_cmvp_api.h
 *
 * @par Note
 * This API is exposed for the products which are going for CMVP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_INT32 IPSI_CMVP_Init(SEC_VOID);

/**
 * @defgroup IPSI_CMVP_DeInit
 * @ingroup ipsi_cmvp_api
 * @par Prototype
 * @code
 * SEC_VOID IPSI_CMVP_DeInit(SEC_VOID)
 * @endcode
 *
 * @par Purpose
 * Deinitialize the CMVP Module
 *
 * @par Description
 * When the automatic startup function is disabled, you need to manually invoke the deinitialization function.
 *
 * @par Parameters
 * N/A
 *
 * @par Required Header File
 * ipsi_cmvp_api.h
 *
 * @par Note
 * This API is exposed for the products which are going for CMVP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_VOID IPSI_CMVP_DeInit(SEC_VOID);

/*
Func Name:  IPSI_CMVP_ModeSet
*/
/**
 * @defgroup IPSI_CMVP_ModeSet
 * @ingroup ipsi_cmvp_api
 * @par Prototype
 * @code
 * SEC_INT32 IPSI_CMVP_ModeSet(IPSI_CMVP_CryptoMode mode, const SEC_VOID *auth)
 * @endcode
 *
 * @par Purpose
 * Switch the working mode of the password module by calling IPSI_CMVP_ModeSet.
 *
 * @par Description
 * After the SO library is loaded, the IPSI_CMVP_ModeSet interface should be invoked to switch the mode immediately.
 * After cryptographic algorithm is used, the IPSI_CMVP_ModeSet interface is invoked to switch the mode. In this case,
 * the switching may fail.
 * When the password module is loaded, the system checks the integrity of the SO library. If the verification fails,
 * the mode switchover fails. In this scenario, the installation file is damaged.
 * If the mode switching fails, the password module cannot be switched or an error occurs.
 * The CO needs to re-initialize the password module by restarting the password module and then switch the mode.
 *
 * @param[in] mode ISO19790 and other standards approval modes
 * @param[in] auth Reserved parameter. This parameter is not used currently. You can set this parameter to NULL.
 *
 * @retval SEC_INT32 (Status Output) On successful return. [SEC_SUCCESS|N/A]
 * @retval SEC_INT32 (Status Output) On failure. [SEC_ERROR|N/A]
 *
 * @par Required Header File
 * ipsi_cmvp_api.h
 *
 * @par Note
 * This API is exposed for the products or applications which are going for CMVP/CPP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_INT32 IPSI_CMVP_ModeSet(IPSI_CMVP_CryptoMode mode, const SEC_VOID *auth);

/*
Func Name:  IPSI_CMVP_ModeGet
*/
/**
 * @defgroup IPSI_CMVP_ModeGet
 * @ingroup ipsi_cmvp_api
 * @par Prototype
 * @code
 * IPSI_CMVP_CryptoMode IPSI_CMVP_ModeGet(SEC_VOID)
 * @endcode
 *
 * @par Purpose
 * Obtains the working mode of the password module.
 *
 * @par Description
 * Obtains the current working mode of the cryptographic module.
 * This function can be invoked in both approved and unapproved modes.
 * Thread security is ensured when multithreading is enabled.
 * The user needs to confirm the working mode of the password module.
 *
 * @par Parameters
 * N/A
 *
 * @retval IPSI_CMVP_CryptoMode Return to Non-Approved Mode. [IPSI_CMVP_MODE_NONAPPROVED|N/A]
 * @retval IPSI_CMVP_CryptoMode Return to ISO19790 mode. [IPSI_CMVP_MODE_ISO19790|N/A]
 *
 * @par Required Header File
 * ipsi_cmvp_api.h
 *
 * @par Note
 * This API is exposed for the products or applications which are going for CMVP/CPP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL IPSI_CMVP_CryptoMode IPSI_CMVP_ModeGet(SEC_VOID);

/*
Func Name:  IPSI_CMVP_ErrorGet
*/
/**
 * @defgroup IPSI_CMVP_ErrorGet
 * @ingroup ipsi_cmvp_api
 * @par Prototype
 * @code
 * SEC_INT32 IPSI_CMVP_ErrorGet(SEC_VOID)
 * @endcode
 *
 * @par Purpose
 * Obtains the error status of the password module.
 *
 * @par Description
 * Error in obtaining the password module. This function can be invoked in both approved and unapproved modes.
 * Thread security is ensured when multiple threads are enabled.
 * When a user fails to use the password module in approval mode, this interface can be invoked to view the error code
 * of the password module.
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_INT32 (Status Output) On successful return. [SEC_SUCCESS|N/A]
 * @retval SEC_INT32 (Status Output) On failure. [IPSI_CMVP_Errno|N/A]
 *
 * @par Required Header File
 * ipsi_cmvp_api.h
 *
 * @par Note
 * This API is exposed for the products or applications which are going for CMVP/CPP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_INT32 IPSI_CMVP_ErrorGet(SEC_VOID);

/*
Func Name:  IPSI_CMVP_IsSupportMultiThread
*/
/**
 * @defgroup IPSI_CMVP_IsSupportMultiThread
 * @ingroup ipsi_cmvp_api
 * @par Prototype
 * @code
 * SEC_BOOL IPSI_CMVP_IsSupportMultiThread(SEC_VOID)
 * @endcode
 *
 * @par Purpose
 * Gets the CMVP support status of the cryptographic module for multithreaded services
 *
 * @par Description
 * The interface itself is not thread-safe.
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_BOOL Support multi thread. [SEC_TRUE|N/A]
 * @retval SEC_BOOL Not support multi thread. [SEC_FALSE|N/A]
 *
 * @par Required Header File
 * ipsi_cmvp_api.h
 *
 * @par Note
 * This API is exposed for the products or applications which are going for CMVP/CPP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_BOOL IPSI_CMVP_IsSupportMultiThread(SEC_VOID);

/*
Func Name:  IPSI_CMVP_EnableMultiThreadSupport
*/
/**
 * @defgroup IPSI_CMVP_EnableMultiThreadSupport
 * @ingroup ipsi_cmvp_api
 * @par Prototype
 * @code
 * SEC_INT32 IPSI_CMVP_EnableMultiThreadSupport(SEC_VOID)
 * @endcode
 *
 * @par Purpose
 * Enable the CMVP multi-thread service support capability of the cryptographic module.
 *
 * @par Description
 * The interface itself is not thread-safe.
 * When the password module is in approved working mode, multiple threads are required to invoke the approved algorithm
 * or operate the working mode of the password module.
 * Enable multi-threading must be invoked before multi-threading is created. You are advised to invoke this interface
 * immediately after the password module is loaded.
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_INT32 (Status Output) On successful return. [SEC_SUCCESS|N/A]
 * @retval SEC_INT32 (Status Output) On failure. [IPSI_CMVP_Errno|N/A]
 *
 * @par Required Header File
 * ipsi_cmvp_api.h
 *
 * @par Note
 * This API is exposed for the products or applications which are going for CMVP/CPP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_INT32 IPSI_CMVP_EnableMultiThreadSupport(SEC_VOID);

/*
Func Name:  IPSI_CMVP_GetVersion
*/
/**
 * @defgroup IPSI_CMVP_GetVersion
 * @ingroup ipsi_cmvp_api
 * @par Prototype
 * @code
 * const SEC_CHAR *IPSI_CMVP_GetVersion(SEC_VOID)
 * @endcode
 *
 * @par Purpose
 * Obtains the version information of the password module.
 *
 * @par Description
 * Obtains the version information of the password module.
 *
 * @par Parameters
 * N/A
 *
 * @retval const SEC_CHAR return CMVP Version. [const SEC_CHAR*|N/A]
 *
 * @par Required Header File
 * ipsi_cmvp_api.h
 *
 * @par Note
 * This API is exposed for the products or applications which are going for CMVP/CPP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL const SEC_CHAR *IPSI_CMVP_GetVersion(SEC_VOID);

/*
Func Name:  IPSI_CMVP_RegServiceIndicatorFunc
*/
/**
 * @defgroup IPSI_CMVP_RegServiceIndicatorFunc
 * @ingroup ipsi_cmvp_api
 * @par Prototype
 * @code
 * SEC_INT32 IPSI_CMVP_RegServiceIndicatorFunc(IPSI_CMVP_SrvIndicFun srvIndic)
 * @endcode
 *
 * @par Purpose
 * Register service indication output function hook.
 *
 * @par Description
 * The service indication hook needs to be set before the standard mode is switched. The registration hook is
 * thread-safe.
 *
 * @param[in] srvIndic Service indication output function hook.
 *
 * @retval SEC_INT32 (Status Output) On successful return. [SEC_SUCCESS|N/A]
 * @retval SEC_INT32 (Status Output) On failure. [SEC_ERROR|N/A]
 *
 * @par Required Header File
 * ipsi_cmvp_api.h
 *
 * @par Note
 * This API is exposed for the products or applications which are going for CMVP/CPP certification.
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_INT32 IPSI_CMVP_RegServiceIndicatorFunc(IPSI_CMVP_SrvIndicFun srvIndic);

#ifdef __cplusplus
}
#endif /* end of __cplusplus */

#endif // IPSI_CMVP_API_H
