#!/bin/bash
# This script get img parma area.
# Copyright © Huawei Technologies Co., Ltd. 2021-2023. All rights reserved.

set -e

PLAT_FILE="$1"
IMAGE="$2"

get_img_parma_area()
{
#结构体头部定义，以下字段为小端
    #Magic Number
    printf '\x0c\xb1\x7a\xda'
    #Structure Version = 1
    printf '\x01\x00\x00\x00'
    #Reserved
    printf '\x00\x00\x00\x00'
    #Revoke Mode：0：传统模式，吊销指定版本号及以下所有版本，非0：定制模式，根据掩码吊销指定版本
    printf '\xff\x00\x00\x00'
    #Secure Version：安全版本号从1开始
    local ver=$(cat ${PLAT_FILE} | grep PMEMinorVer | sed 's,.*<VALUE>\(.*\)</VALUE>.*,\1,')
    if [ $ver -lt 1 -o $ver -gt 99 ]
    then
        exit 1
    fi
    ver=$(printf "%x" ${ver})
    printf "\x${ver}\x00\x00\x00"
#吊销掩码为16字节数组，吊销版本号和前2个字节对应关系举例如下：1:01 00，2:02 00，8:80 00,9:00 01，16:00 80
    #Revoked Ver Mask - BMC
    printf '\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'
    #Reserved
    dd if=/dev/zero bs=1 count=$((4 * 1024 - 36))
}
get_img_parma_area >>${IMAGE}
