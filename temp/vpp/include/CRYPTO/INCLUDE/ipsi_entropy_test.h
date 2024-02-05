/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: Contains various crypto interface functions
 * ALL RIGHTS RESERVED
 * Project Code: iPSI
 * Module Name: ipsi_entropy_test.h
 * Create:  2019-11-10
 * Author:Boga Sandeep
 * Descrption: Contains Repetation and Adaptive test APIs
 * ----------------------------------------------------------------------
 * Modification History
 * DATE        NAME             DESCRIPTION
 * --------------------------------------------------------------------
 */
#ifndef IPSI_ENTROPY_TEST_H
#define IPSI_ENTROPY_TEST_H

#include "sec_sys.h"

#ifdef __cplusplus
extern "C" {
#endif

/* entropy sample buffer size */
#define ENTROPY_SAMPLE_BUFFER_SIZE 512

/* Prototype of structure which store config for Repetition Health Test */
typedef struct stRepHealthTest ST_REPHEALTHTEST;

/* Prototype of structure which store config for Adaptive Heath Test */
typedef struct stAdaptiveHealthTest ST_ADAPTIVEHEALTHTEST;

/* This data structure is defined to store entropy sample */
/**
 * @defgroup ENTROPY_SAMPLE_S
 * @ingroup Structures
 * @par Description
 * This data structure is defined to store entropy sample.
 * @par Prototype
 * @code
 * typedef struct stEntropySample
 * {
      unsigned char ucSample[ENTROPY_SAMPLE_BUFFER_SIZE];
      unsigned int uiSampleSize;
 * }ENTROPY_SAMPLE_S;

 * @endcode
 *
 * @datastruct ucSample Buffer to store entropy sample.
 * @datastruct uiSampleSize Size of the entropy sample.
 *
 */
typedef struct stEntropySample {
    SEC_UCHAR ucSample[ENTROPY_SAMPLE_BUFFER_SIZE];
    SEC_UINT uiSampleSize;
} ENTROPY_SAMPLE_S;

/*
    Func Name:  CRYPT_configRepetitionCountTest
*/
/**
 * @defgroup CRYPT_configRepetitionCountTest
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * struct stRepHealthTest* CRYPT_configRepetitionCountTest(unsigned int uiCutoff, ENTROPY_SAMPLE_S *pstSample);
 *
 * @par Purpose
 * This function is used to configure the Repetitive Health test.
 *
 * @par Description
 * This function is used to configure the Repetitive Health test by taking cutoff value and sample data as input.
 *
 * @param[in] uiCutoff Cutoff value after which test should failed [N/A]
 * @param[in] pstSample initial sample for repetitive test [N/A]
 *
 * @retval ST_REPHEALTHTEST*  for invalid inputs and malloc failure [SEC_NULL|N/A]
 * @retval ST_REPHEALTHTEST*  pointer to struct stRepHealthTest [ST_REPHEALTHTEST|N/A]
 *
 * @par Required Header File
 * ipsi_entropy_test.h
 *
 * @par Note
 * uiCutoff value should not be zero. which will casue API to fail.
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL ST_REPHEALTHTEST *CRYPT_configRepetitionCountTest(SEC_UINT uiCutoff, ENTROPY_SAMPLE_S *pstSample);

/*
    Func Name:  CRYPT_doRepetitionCountTest
*/
/**
 * @defgroup CRYPT_doRepetitionCountTest
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * int CRYPT_doRepetitionCountTest(struct stRepHealthTest *pstRepHT, ENTROPY_SAMPLE_S *pstSample);
 *
 * @par Purpose
 * This function is used to  check the current sample for Repetition Health test.
 *
 * @par Description
 * This function is used to  check the current sample for Repetition Health test.
 *
 * @param[in] pstRepHT structure pointer returned by CRYPT_configRepHT [N/A]
 * @param[in] pstSample  sample to check [N/A]
 *
 * @retval SEC_INT  If Repetative healthTest is success [SEC_SUCCESS|N/A]
 * @retval SEC_INT  If invald arguments are passed [SEC_ERR_INVALID_ARG|N/A]
 * @retval SEC_INT  If sample size is not same with the previous sample [SEC_ERR_INVALID_ENTROPY_SAMPLE_SIZE|N/A]
 * @retval SEC_INT  If Repetative healthTest fails  [SEC_ERR_REP_HEALTH_TEST_FAIL|N/A]
 *
 * @par Required Header File
 * ipsi_entropy_test.h
 *
 * @par Note
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_INT CRYPT_doRepetitionCountTest(ST_REPHEALTHTEST *pstRepHT, ENTROPY_SAMPLE_S *pstSample);

/*
    Func Name:  CRYPT_freeRepCountTestConfig
*/
/**
 * @defgroup CRYPT_freeRepCountTestConfig
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * void CRYPT_freeRepCountTestConfig(struct stRepHealthTest** dpstRepHT);
 *
 * @par Purpose
 * This function is used to Free the config pointer return by CRYPT_configRepHT.
 *
 * @par Description
 * This function is used to Free the config pointer return by CRYPT_configRepHT.
 *
 * @param[in] dpstRepHT double pointer of stRepHealthTest to free [N/A]
 *
 * @retval SEC_VOID  This function does not return any value[N/A|N/A]
 *
 * @par Required Header File
 * ipsi_entropy_test.h
 *
 * @par Note
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_VOID CRYPT_freeRepCountTestConfig(ST_REPHEALTHTEST **dpstRepHT);

/*
    Func Name:  CRYPT_configAdaptiveProportionTest
*/
/**
 * @defgroup CRYPT_configAdaptiveProportionTest
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * struct stAdaptiveHealthTest * CRYPT_configAdaptiveProportionTest(unsigned int uiCutoff, unsigned int uiWindowSize,
 * ENTROPY_SAMPLE_S *pstSample);
 *
 * @par Purpose
 * This function is used to configure the Adaptive Health test.
 *
 * @par Description
 * This function is used to configure the Adaptive Health test by taking cutoff value, WindowSize and sample data as
 * input.
 *
 * @param[in] uiCutoff Cutoff value after which test should failed [N/A]
 * @param[in] uiWindowSize Window Size [N/A]
 * @param[in] pstSample initial sample  [N/A]
 *
 * @retval ST_REPHEALTHTEST*  for invalid inputs and malloc failure [SEC_NULL|N/A]
 * @retval ST_REPHEALTHTEST*  pointer to struct stRepHealthTest [ST_REPHEALTHTEST|N/A]
 *
 * @par Required Header File
 * ipsi_entropy_test.h
 *
 * @par Note
 * uiCutoff and uiWindowSize should not be zero, which will lead to API failure.
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL ST_ADAPTIVEHEALTHTEST *CRYPT_configAdaptiveProportionTest(SEC_UINT uiCutoff, SEC_UINT uiWindowSize,
    ENTROPY_SAMPLE_S *pstSample);
/*
    Func Name:  CRYPT_doAdaptiveProportionTest
*/
/**
 * @defgroup CRYPT_doAdaptiveProportionTest
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * int CRYPT_doAdaptiveProportionTest(struct stAdaptiveHealthTest *pstAdaptiveHT , ENTROPY_SAMPLE_S *pstSample);
 *
 * @par Purpose
 * This function is used to  check the current sample for adaptive Health test.
 *
 * @par Description
 * This function is used to  check the current sample for adaptive Health test.
 *
 * @param[in] pstAdaptiveHT structure pointer returned CRYPT_configAdaptiveHT [N/A]
 * @param[in] pstSample  sample to check [N/A]
 *
 * @retval SEC_INT  If Repetative healthTest is success [SEC_SUCCESS|N/A]
 * @retval SEC_INT  If invald arguments are passed [SEC_ERR_INVALID_ARG|N/A]
 * @retval SEC_INT  If sample size is not same with the previous sample [SEC_ERR_INVALID_ENTROPY_SAMPLE_SIZE|N/A]
 * @retval SEC_INT  If Repetative healthTest fails  [SEC_ERR_ADAP_HEALTH_TEST_FAIL|N/A]
 *
 * @par Required Header File
 * ipsi_entropy_test.h
 *
 * @par Note
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_INT CRYPT_doAdaptiveProportionTest(ST_ADAPTIVEHEALTHTEST *pstAdaptiveHT, ENTROPY_SAMPLE_S *pstSample);

/*
    Func Name:  CRYPT_freeAdapProportionTestConfig
*/
/**
 * @defgroup CRYPT_freeAdapProportionTestConfig
 * @ingroup HLF High Level Functions
 * @par Prototype
 * @code
 * void CRYPT_doAdaptiveProportionTest(struct stAdaptiveHealthTest** dpstRepHT);
 *
 * @par Purpose
 * This function is used to Free the config pointer return by CRYPT_configAdaptiveHT.
 *
 * @par Description
 * This function is used to Free the config pointer return by CRYPT_configAdaptiveHT.
 *
 * @param[in] dpstAdaptiveHT double pointer of stAdaptiveHealthTest to free [N/A]
 *
 * @retval SEC_VOID  This function does not return any value[N/A|N/A]
 *
 * @par Required Header File
 * ipsi_entropy_test.h
 *
 * @par Note
 *
 * @par Related Topics
 * N/A
 */
CRYPTOLINKDLL SEC_VOID CRYPT_freeAdapProportionTestConfig(ST_ADAPTIVEHEALTHTEST **dpstAdaptiveHT);

#ifdef __cplusplus
}
#endif

#endif /* IPSI_ENTROPY_TEST_H */
