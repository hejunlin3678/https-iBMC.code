/*******************************************************************************
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 *
 * -------------------------------------------------------------------------------
 *
 * cml_algorithm_loss.h
 *
 * Project Code: DOPAI
 * Module Name: machine_learning
 * Date Created: 2021-07-26
 * Author:
 * Description: Algorithm's loss functions
 *
 * -------------------------------------------------------------------------------
 * Modification History
 * DATE          NAME                       DESCRIPTION
 * -----------------------------------------------------------------------------
 *
 * ******************************************************************************/
#ifndef DOPAI_ALGORITHM_LOSS_H
#define DOPAI_ALGORITHM_LOSS_H
/** @defgroup CML_Algorithm 算法Loss Function */
#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @ingroup CML_Algorithm
 */
enum DOPAI_CML_LossFunction {
    ExponentialLoss = 0,            /* *< ExponentialLoss */
    BinomialDeviance = 1,           /* *< BinomialDeviance */
    MultinomialDeviance = 2,        /* *< MultinomialDeviance */
    BinaryLogistic = 3,             /* *< BinaryLogistic */
    BinaryLogitraw = 4,             /* *< BinaryLogitraw */
    MultiSoftmax = 5,               /* *< MultiSoftmax */
    Binary = 6,                     /* *< Binary */
    MultiClass = 7,                 /* *< MultiClass */
    MeanSquaredError = 8,           /* *< MeanSquaredError */
    MeanAbsoluteError = 9,          /* *< MeanAbsoluteError */
    MeanAbsPercentageError = 10,    /* *< MeanAbsPercentageError */
    Logistic = 11,                  /* *< Logistic */
    SquaredError = 12,              /* *< SquaredError */
    LeastAbsoluteError = 13,        /* *< LeastAbsoluteError */
    Huber = 14,                     /* *< Huber */
    MIN = ExponentialLoss,          /* *< 最小值 */
    MAX = Huber,                    /* *< 最大值 */
    Invalid = -1,                   /* *< 无效值 */
};

#ifdef __cplusplus
}
#endif

#endif