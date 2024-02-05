#!/bin/bash
# This is for package GPP and write address into the Top header
# Copyright © Huawei Technologies Co., Ltd. 2021-2023. All rights reserved.

set -e

##############################################
#GPP_Top_header的大小为512B，放置在最后512字节：
#Hi1711_uBoot.bin的大小为2M，放置在最后-3M的位置
#sub_img_header的大小小于1M，放置在最后-4M的位置，
#sub_img_header包括（defaultCA.der, cms文件，crl文件，2kb对齐）
#rootfs.img 的大小可变，放置在GPP区最前面0M。
#############################################
gpp_max_size=384
rootfs_max_size=$(echo "$gpp_max_size-4"|bc)
board_name=$1
rw_flag=$2
echo $board_name
rootfs_img_file_name="rootfs_$board_name.img"
sub_img_header_file_name=emmc_sub_header
HUAWEI_ROOTCA=defaultCA.der
sign_cms=rootfs_$board_name.img.cms
sign_cms_crl=cms.crl
uboot_file_name=Hi1711_boot_4096.bin
gpp_top_header_file_name=emmc_top_header

rm -rf GPP_$board_name.bin GPP_temp_$board_name.bin GPP_rootfs_$board_name.bin

if [ $rw_flag == 1 ] ; then
    echo "$board_name emmc gpp need rw version"
    [ -e "${rootfs_img_file_name}" ] && sudo rm -rf "${rootfs_img_file_name}"
    [ -e "${sign_cms}" ] && sudo rm -rf "${sign_cms}"
    [ -e "${uboot_file_name}" ] && sudo rm -rf "${uboot_file_name}"
    sudo mv rootfs_rw_${board_name}.img "$rootfs_img_file_name"
    sudo mv rootfs_rw_${board_name}.img.cms "$sign_cms"
    sudo mv Hi1711_boot_4096_debug.bin "$uboot_file_name"
fi


########--Gpp_Top_Header --####################
if  [ ! -n "$gpp_top_header_file_name" ] ;then
    echo "The config file $gpp_top_header_file_name doesn't exist!"
    exit -1
else
    echo "the Gpp topheader file is $gpp_top_header_file_name"
    gpp_top_header_file_size=$(ls -l $gpp_top_header_file_name | awk '{print $5}')
    if [ "$gpp_top_header_file_size" != "512" ]; then
        echo "the size of $gpp_top_header_file_size is not 512!"
        exit -1
    fi
    dd if=/dev/zero of=./Top_header_padding_$board_name bs=512 count=2047  ##填充最后为1M，大小为1M-512
    cat Top_header_padding_$board_name $gpp_top_header_file_name > GPP_temp_$board_name.bin
    cp GPP_temp_$board_name.bin GPP_$board_name.bin -rf
fi


########--Uboot_FW_2M--##########################
if  [ ! -n "$uboot_file_name" ] ;then
    echo "The config file doesn't include $uboot_file_name!"
    exit -1
else
    echo "the Gpp uboot file is $uboot_file_name"
    uboot_file_size=$(ls -l $uboot_file_name | awk '{print $5}')
    if [ "$uboot_file_size" != "2097152" ]; then
        echo "the size of $uboot_file_name is not 2M!"
        exit -1
    fi
    cat $uboot_file_name GPP_$board_name.bin > GPP_temp_$board_name.bin 
    cp GPP_temp_$board_name.bin GPP_$board_name.bin -rf
fi


###################################################################
#Rootfs sub Header的整个bin的大小，采用2kb对齐
#文件的顺序: sub_img_header、huawei_RootCA、sign_cms、sign_cms_crl。
#文件采用2kb对齐，后面采用全零进行填充，作为客户签名的预留区
########--rootfs_sub_img_header--###############################
if  [ ! -n "$sub_img_header_file_name" ] ;then
    echo "The $sub_img_header_file_name is not exist!"
    exit -1
else
    echo "The rootfs sub header file is $sub_img_header_file_name"
    rootfs_sub_header_file_size=$(ls -l $sub_img_header_file_name | awk '{print $5}')
    rootfs_sub_header_size=$(echo "(($rootfs_sub_header_file_size+2*1024-1)/(2*1024))*2*1024"|bc)  ##保证2kb对齐
    echo "the size of rootfs header is  $rootfs_sub_header_size."
    dd if=/dev/zero of=./rootfs_header_$board_name.bin bs=1M count=1
    dd if=./$sub_img_header_file_name of=./rootfs_header_$board_name.bin  conv=notrunc
fi

################--huawei_RootCA--################################
if  [ ! -n "$HUAWEI_ROOTCA" ] ;then
    echo "The $HUAWEI_ROOTCA is not exist!"
    exit -1
else
    echo "The huawei Root CA file is $HUAWEI_ROOTCA"
    huawei_rootca_file_size=$(ls -l $HUAWEI_ROOTCA | awk '{print $5}')
    huawei_rootca_size=$(echo "(($huawei_rootca_file_size+2*1024-1)/(2*1024))*2*1024"|bc)  ##保证2kb对齐
    echo "the size of Root CA is  $huawei_rootca_size."
    dd if=./$HUAWEI_ROOTCA of=./rootfs_header_$board_name.bin  bs=1 seek=$rootfs_sub_header_size conv=notrunc
fi

################--sign_cms--################################
if  [ ! -n "$sign_cms" ] ;then
    echo "The $sign_cms is not exist!"
    exit -1
else
    echo "The sign cms file is $sign_cms."
    sign_cms_file_size=$(ls -l $sign_cms | awk '{print $5}')
    sign_cms_size=$(echo "(($sign_cms_file_size+2*1024-1)/(2*1024))*2*1024"|bc)  ##保证2kb对齐
    echo "the size of sign cms is  $sign_cms_size."
    sign_cms_start_site=$(echo "$rootfs_sub_header_size+$huawei_rootca_size"|bc)
    dd if=./$sign_cms of=./rootfs_header_$board_name.bin  bs=1 seek=$sign_cms_start_site conv=notrunc
fi

################--sign_crl--################################
if  [ ! -n "$sign_cms_crl" ] ;then
    echo "The $sign_cms_crl is not exist!"
    exit -1
else
    echo "The sign cms crl file is $sign_cms_crl."
    sign_cms_crl_file_size=$(ls -l $sign_cms_crl | awk '{print $5}')
    sign_cms_crl_size=$(echo "(($sign_cms_crl_file_size+2*1024-1)/(2*1024))*2*1024"|bc)  ##保证2kb对齐
    echo "the size of sign cms is  $sign_cms_crl_size."
    sign_crl_start_site=$(echo "$sign_cms_start_site+$sign_cms_size"|bc)
    dd if=./$sign_cms_crl of=./rootfs_header_$board_name.bin  bs=1 seek=$sign_crl_start_site conv=notrunc
fi

cat rootfs_header_$board_name.bin GPP_$board_name.bin > GPP_temp_$board_name.bin

##########################################################
#rootfs image的整个bin,最大为380M。不到380M的部分，使用零填充。
########--rootfs_1288hv5.img--#########
if  [ ! -n "$rootfs_img_file_name" ] ;then
    echo "The config file doesn't include $rootfs_img_file_name!"
    exit -1
else
    echo "the rootfs image file is $rootfs_img_file_name"
    rootfs_file_size=$(ls -l $rootfs_img_file_name | awk '{print $5}')
    dd if=/dev/zero of=./Gpp_padding_$board_name bs=1M count=$rootfs_max_size
    dd if=./$rootfs_img_file_name  of=./Gpp_padding_$board_name bs=1M count=380 conv=notrunc
    cat Gpp_padding_$board_name GPP_temp_$board_name.bin > GPP_rootfs_$board_name.bin

fi
