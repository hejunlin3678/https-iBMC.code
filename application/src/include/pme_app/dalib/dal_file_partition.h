/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: 独立文件分区相关公共接口
 * Author: x00454924
 * Notes: 通过将虚拟镜像文件绑定到循环设备,然后将循环设备挂载到文件系统的方式实现独立文件分区
 * History: 2022-02-08 x00454924 创建文件
 */
#ifndef DAL_FILE_PARTITION_H
#define DAL_FILE_PARTITION_H

#ifdef ARM64_HI1711_ENABLED
#include "pme/pme.h"
#include "pme_app/pme_app.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */
gint32 dal_mount_partition_image(const gchar* image_path, const gchar* mount_dir);
gint32 dal_umount_partition_image(const gchar* image_path, const gchar* mount_dir);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif
#endif  /* DAL_FILE_PARTITION_H */