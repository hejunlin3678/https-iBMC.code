#!/bin/bash
# ***********************************************************************
# Copyright: (c) Huawei Technologies Co., Ltd. 2022. All rights reserved.
# script for install RTOS_208.5.3.B023
# version: 
# change log:
# ***********************************************************************
set -eux
set -o pipefail
 
# solve the rtos install error
set +u
 
RTOS_VER=208.5.3
RTOS_TIMESTAMP=20230821010651
 
RTOS_INS_DIR=/opt/RTOS/${RTOS_VER%.*}.0
RTOS_RPM_POSTFIX=${RTOS_VER}-${RTOS_TIMESTAMP}.i586.rpm
RTOS_ARM64_PROC=arm64le_5.10_ek_preempt_pro
ls -l *.rpm
 
install_dir=$1
[ -z ${install_dir} ] && install_dir=/opt/buildtools/${RTOS_VER}
[ -d ${install_dir} ] && rm -rf ${install_dir}
mkdir -p ${install_dir}
 
#Transmission & Access 
rpm -ivh rtos_base-${RTOS_RPM_POSTFIX}
rpm -ivh rtos_lkp-${RTOS_RPM_POSTFIX}
rpm -ivh rtos_userspace_sysroots-${RTOS_ARM64_PROC}-${RTOS_RPM_POSTFIX}
rpm -ivh rtos_kernelspace_sysroots-${RTOS_ARM64_PROC}-${RTOS_RPM_POSTFIX}
rpm -ivh rtos_libc_sysroots-${RTOS_ARM64_PROC}-${RTOS_RPM_POSTFIX}
 
cp -rf rtos_*.rpm ${install_dir}/
 
source ${RTOS_INS_DIR}/dlsetenv.sh -p ${RTOS_ARM64_PROC} --sdk-path=/opt/hcc_arm64le