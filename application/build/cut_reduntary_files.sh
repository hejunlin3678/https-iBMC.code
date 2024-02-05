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


G_RELATIVE_DIR="../../" # 代码仓目录相对当前脚本的路径,以符号'.'(dot)开头
G_CURRENT_DIR=$(dirname $(realpath $0)) # 当前文件路径, 比$(pwd)更稳定
G_CODE_BASE=$(realpath ${G_CURRENT_DIR}/${G_RELATIVE_DIR}) # 代码仓路径
G_BOARD_NAME=""
G_BOARD_RESOURCE=( # 单板资源文件目录,其下文件夹名与单板名一致
    "application/src/resource/board"
    "application/src_bin/rootfs/opt/pme/conf/vrd_mgnt"
)
source "${G_CODE_BASE}/application/build/delete_item_config.sh" 
################################################################################
function help_info (){
# 裁剪当前单板无用的文件
# 思路：
# 1. 获得输入参数单板名$1
# 2. 删除公共工具等；根据单板名，删除特性、资源文件等
# 3. 自动删除无效的CMake语句
# 4. 验证：
#   4.0 准备[拆包工具](http://3ms.huawei.com/km/blogs/details/7783237?l=zh-cn#preview_attachment_box_7783237)与BeyondCompare
#   4.1 执行裁剪脚本前，出包A, 执行裁剪脚本后，出包B
#   4.2 将A与B用拆包工具分别解包，得到a与b
#   4.3 比较a与b的内容，要求一致
    echo "usage: $(basename $0) [board_name]"}
    echo "support_list:"
    echo "    ${G_SUPPORTED_LIST[@]}"
    return 0
}
# Description : 判断参数1字符数组array是否包含参数2字符val
# Notes：example $(contains ${array[@]} ${val}) == "y"
#        支持array、val中包含空格的情况
function contains (){
    local n_of_parameter=$#
    local target=${!n_of_parameter} # 最后一个参数是目标值
    for (( i=1;i < $n_of_parameter;i++ )) {
        if [[ "${!i}" == "${target}" ]]; then
            echo "y"
            return 0
        fi
    }
    echo "n"
    return 1
}
# Description : 判断单板是否支持自动裁剪
function check_board_name (){
    local board_name="$1"
    if [[ "${board_name}" == "" ]];then
        G_BOARD_NAME="TaiShan2280v2" # default
        return 0
    fi
    if [[ $(contains "${G_SUPPORTED_LIST[@]}" "${board_name}") == "n" ]];then
        echo "wrong n_of_parameter! board name $board_name is not in support_list!"
        help_info
        exit 1
    fi
    return 0
}
# Description : 裁剪各单板公共冗余文件
function cut_common (){
    for file in ${delete_allboard_file[@]};do
        echo $(find ${file} -type f)
        rm -rvf ${file}
    done;
    return 0
}
# Description : 裁剪指定单板资源文件
function cut_board_resource (){
    local board_name="$1"
    local files=$(find ${G_BOARD_RESOURCE[0]} -maxdepth 1 -type d|xargs -I {} basename "{}")
    local while_list=(
        "web_online_help"
        "board"
        "${board_name}"
    )
    for file in ${files[@]};do
        if [[ $(contains "${while_list[@]}" "${file}") == "n" ]];then
            echo $(find ${G_BOARD_RESOURCE[0]}/${file} -type f)
            rm -rvf "${G_BOARD_RESOURCE[0]}/${file}"
        fi
    done
    files=$(find ${G_BOARD_RESOURCE[1]} -maxdepth 1 -type d|xargs -I {} basename "{}")
    for file in ${files[@]};do
        if [[ ${board_name} != ${file} && ${file} != "vrd_mgnt" ]];then
            echo $(find ${G_BOARD_RESOURCE[1]}/${file} -type f)
            rm -rvf "${G_BOARD_RESOURCE[1]}/${file}"
        fi
    done
    return 0
}

# Description : 裁剪指定单板文件
function cut_board (){
    local board_name="$1"
    if [[ "${board_name}" == "TaiShan2280v2" ]];then
        for file in ${delete_taishan2280v2_file[@]};do
            echo $(find ${file} -type f)
            rm -rvf ${file}
        done;
    else
        echo "error"
    fi
    cut_board_resource ${board_name}
    return 0
}
################################################################################
echo "cut redundant files begin..."
cd "${G_CODE_BASE}"
G_BOARD_NAME=$1
check_board_name "${G_BOARD_NAME}"
cut_common
cut_board "${G_BOARD_NAME}"

sh ${G_CURRENT_DIR}/cut_reduntary_files_tail.sh ${G_CODE_BASE} ${G_BOARD_NAME}

echo "cut redundant files done."
exit 0