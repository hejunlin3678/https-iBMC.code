/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description:  Contains FIPS module mode set/get
 * Project Code: IPSI
 * Module Name: ipsi_fips_crypto.h
 * Create: 2020-03-26
 * Author: VPP
 */

#ifndef IPSI_FIPS_CRYPTO_H
#define IPSI_FIPS_CRYPTO_H

#include "ipsi_types.h"
#include "sec_sys.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IPSI_FIPS_MODE 1
#define IPSI_FIPS_COMPATIBLE_MODE 2

/*
Func Name:  IPSI_FIPS_CRYPT_module_set_mode
*/
/**
 * @defgroup IPSI_FIPS_CRYPT_module_set_mode
 * @ingroup FIPS_Function
 * @par Prototype
 * @code
 * SEC_INT IPSI_FIPS_CRYPT_module_set_mode(SEC_INT iMode);
 * @endcode
 *
 * @par Purpose
 * This function is used to mode of library.
 *
 * @par Description
 * This function is used to to mode of library.
 *
 * @param[in] iMode (Data Input) IPSI_FIPS_MODE - FIPS mode(strict mode) ,
 * IPSI_FIPS_COMPATIBLE_MODE - FIPS-compatible mode
 *
 * @retval SEC_INT (Status Output) On successful return [SEC_TRUE|N/A]
 * @retval SEC_INT (Status Output) On failure [SEC_FALSE|N/A]
 *
 * @par Required Header File
 * ipsi_fips_crypto.h
 *
 * @par Note
 * - NA
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_INT IPSI_FIPS_CRYPT_module_set_mode(SEC_INT iMode);

/*
Func Name:  IPSI_FIPS_CRYPT_module_get_mode
*/
/**
 * @defgroup IPSI_FIPS_CRYPT_module_get_mode
 * @ingroup FIPS_Function
 * @par Prototype
 * @code
 * SEC_INT IPSI_FIPS_CRYPT_module_get_mode(void);
 * @endcode
 *
 * @par Purpose
 * This function is used to get the FIPS module mode.
 *
 * @par Description
 * IPSI_FIPS_CRYPT_module_get_mode is used to get the FIPS module mode.
 *
 * @par Parameters
 * N/A
 *
 * @retval SEC_INT (Status Output) Mode in set by application [N/A]
 *
 * @par Required Header File
 * ipsi_fips_crypto.h
 *
 * @par Note
 * N/A
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_INT IPSI_FIPS_CRYPT_module_get_mode(void);

#ifdef __cplusplus
}
#endif /* end of __cplusplus */

#endif /* end of _IPSI_FIPS_CRYPTO_H_ */
