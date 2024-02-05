#!/bin/bash
# This file stores common functions in each script and is referenced by $(source common.sh).
# Note: This file cannot contain any execution statements.
# Each variable uses the local declaration to prevent pollution.
# Copyright © Huawei Technologies Co., Ltd. 2021-2023. All rights reserved.

set -e

G_WORK_DIR=$1

delete_recover_ssh_key_sh(){
    # recover_ssh_key.sh 是DTS2019052910349为了R7回退到R5的兼容性保证而引入的，对应git 6209009932de63
    # 对于只有R7的产品，需要删除
    local delete_list=(  # 与application/build/build_CI.sh 中的support_board名称一致 
        "2488hv6"
        "1288hv6_2288hv6_5288v6"
        "2288HV6-16DIMM"
        "xh321v6"
        "Atlas880"
        "Atlas500_3000"
        "Atlas800_9010"
        "bmc_card"
        "RM210"
        "RM210_SECURITY_3V0"
        "RM211"
        "S920XA0"
        "S920XB0"
        "TaiShan2280Ev2"
        "TaiShan2280v2_1711"
		"BM320"
        "BM320_V3"
        "S920X05"
        "S920S03"
        "TaiShan2480v2"
        "TaiShan2480v2Pro"
        "TaiShan2280v2Pro"
        "DA121Cv2"
        "DA122Cv2"
        "SMM"
        "CN221"
        "CM221"
        "CM221S"
        "CN221V2"
        "CN221SV2"
        "DP1210_DP2210_DP4210"
        "Atlas200I_SoC_A1"
        "Atlas800D_G1"
        "CST1020V6"
        "2288xv5_1711"
	    "S902X20"
        "Atlas800D_RM",
        "Atlas900_PoD_A2"
    )
    local board_name=$1
    for i in ${delete_list[@]};do
        if [[ "${board_name}" == "$i" ]];then
            remove_file_name="${G_WORK_DIR}/../rootfs/etc/rc.d/rc.stop/recover_ssh_key.sh"
            if [ -e "${remove_file_name}" ]; then
                rm ${remove_file_name}
            else
                echo "${remove_file_name} not exists!!!"
            fi
            break
        fi
    done
}

delete_recover_ssh_key_sh $2
