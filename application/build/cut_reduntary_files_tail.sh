#!/bin/bash
# Cut out redundant files irrelevant to boards.
# Copyright (c) Huawei Technologies Co., Ltd. 2012-2020. All rights reserved. 
# Description: 裁剪单板无关的冗余文件, 包括公共工具、单板特性资源及代码、裁剪后的冗余
#              cmake语句
# Author: z00518714 
# Create: 2020-04-07 
# Notes: 
# History: 2020-04-07 z00518714 适配单板$(TaiShan2280v2)
################################################################################

G_CODE_BASE="$1"
G_BOARD_NAME="$2"
source "${G_CODE_BASE}/application/build/delete_item_config.sh" 
################################################################################
# Description: 各单板的特殊操作.临时为之,尽量改为自动化脚本,不推荐更新/添加此函数.
function special_opertations (){
    local board_name=${G_BOARD_NAME}
    cd ${G_CODE_BASE}
    if [[ $board_name == "TaiShan2280v2" ]];then
        sed -i "/upgrade_lcd/d" "./application/src/apps/upgrade/CMakeLists.txt"
        sed -i "/UPGRADE_LCD_SRC/d" "./application/src/apps/upgrade/CMakeLists.txt"
        sed -i "/upgrade_12/d" "./application/src/apps/upgrade/CMakeLists.txt"
        sed -i "/cs4343/d" "./application/src/libs/CMakeLists.txt"
    fi
    return 0
}
# Description: 自动删除不存在目录的cmake语句，其他裁剪完成后调用
# Note: 1,仅支持cmake语句严格为ADD_SUBDIRECTORY(\w+)的形式
#!      2,注意会自动把不存在目录的cmake语句删除，若目录移动后再编译需注意！
#!      3,注意，cmake语句直接影响编译结果，故major_dir的范围要考虑2中的情况，避免自动删除
#!           掉需要的目录。更好的实践方式是对整体编译过程中的目录移动进行整改。
function cut_cmakelists (){
    local pattern="ADD_SUBDIRECTORY"
    local major_dir="./application/src/apps" # 暂时只检查此处
    local sub_dir=""
    cd ${G_CODE_BASE}
    for i in $(find $major_dir -type f -name CMakeLists.txt);do
        sub_dir=$(grep $pattern "$i"|grep -E -o "\(\w+\)"| grep -E -o "\w+")
        if [[ $sub_dir == "" ]];then
            continue
        fi
        for j in $sub_dir;do 
            if [[ ! -d "$(dirname $i)/$j" ]];then
                echo "delete:$i ADD_SUBDIRECTORY($j)" 
                sed -i "s/ADD_SUBDIRECTORY($j)//g" $i
            fi
        done
    done
    return 0
}
################################################################################
cut_cmakelists
special_opertations "${G_BOARD_NAME}"