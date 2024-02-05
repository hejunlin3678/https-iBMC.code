/* ******************************************************************************
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 *
 * -------------------------------------------------------------------------------
 *
 * util_file_operator.h
 *
 * Project Code: DOPAI
 * Module Name: FileOperator
 * Date Created: 2021-08-5
 * Author: wutao
 * Description: 文件权限定义
 *
 * -------------------------------------------------------------------------------
 * Modification History
 * DATE          NAME                       DESCRIPTION
 * -----------------------------------------------------------------------------
 *
 * ***************************************************************************** */

#ifndef FILE_OPERATOR_H
#define FILE_OPERATOR_H
#ifdef __cplusplus
extern "C" {
#endif
// 定义文件权限
#define PERMISSIONS_IRUSR (0x0100)  /**< 文件所有者可读权限 */
#define PERMISSIONS_IWUSR (0x0080)  /**< 文件所有者可写权限 */
#define PERMISSIONS_IXUSR (0x0040)  /**< 文件所有者可执行权限 */
#define PERMISSIONS_IRWUSR (PERMISSIONS_IRUSR | PERMISSIONS_IWUSR)  /**< 文件所有者读写权限 */
#define PERMISSIONS_IRWXUSR (PERMISSIONS_IRUSR | PERMISSIONS_IWUSR | PERMISSIONS_IXUSR) /**< 文件所有者读写执行权限 */

// 如下为用户组权限定
#define PERMISSIONS_IRGRP (PERMISSIONS_IRUSR >> 3)
#define PERMISSIONS_IWGRP (PERMISSIONS_IWUSR >> 3)
#define PERMISSIONS_IXGRP (PERMISSIONS_IXUSR >> 3)
#define PERMISSIONS_IRWGRP (PERMISSIONS_IRWUSR >> 3)
#define PERMISSIONS_IRWXGRP (PERMISSIONS_IRWXUSR >> 3)

// 如下为其他用户权限定义
#define PERMISSIONS_IROTH (PERMISSIONS_IRGRP >> 3)
#define PERMISSIONS_IWOTH (PERMISSIONS_IWGRP >> 3)
#define PERMISSIONS_IXOTH (PERMISSIONS_IXGRP >> 3)
#define PERMISSIONS_IRWOTH (PERMISSIONS_IRWGRP >> 3)
#define PERMISSIONS_IRWXOTH (PERMISSIONS_IRWXGRP >> 3)

#ifdef __cplusplus
}
#endif
#endif /* FILE_OPERATOR_H */
