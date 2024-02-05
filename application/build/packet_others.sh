#!/bin/bash
# Package the packing tool script.
# Copyright © Huawei Technologies Co., Ltd. 2021-2023. All rights reserved.

set -e

G_WORK_DIR=$(pwd)
TEMP_DIR=${G_WORK_DIR}/temp
OUTPUT_DIR=${G_WORK_DIR}/output
PACKET_DIR=${G_WORK_DIR}/output/packet
RELEASE_DIR=ToInner
str_success="\033[32;1m[success]\033[0m"
str_fail="\033[31;1m[Fail]\033[0m"

create_hpm_tools_pkg() {

	echo "create compressed packet for HPM tools ------------> [begin]"
	if [ ! -d ${TEMP_DIR} ]
	then
		mkdir -p ${TEMP_DIR}
	fi
	
	cd ${TEMP_DIR}
	
	if [ -d ${RELEASE_DIR} ]
	then
		rm -fr ${RELEASE_DIR}
	fi
	
	#打包BIOS/CPLD HPM打包用的工具
	mkdir -p ${RELEASE_DIR}/HPMPacketTools
	
	cp -fr ../../../tools/dev_tools/hpmpack_release/common/* ${RELEASE_DIR}/HPMPacketTools/
	
	cd ${RELEASE_DIR}/HPMPacketTools/
	rm -fr *.sh
 
	zip -r HPMPacketTools.zip *
	rm -fr $(ls | grep -v HPMPacketTools.zip)
	
	cd -
	
	#打包KVM需要使用的JAR包
	mkdir -p ${RELEASE_DIR}/JAR
	
	cp -fr ../../src_bin/rootfs/opt/pme/web/htdocs/bmc/pages/jar/vconsole.jar ${RELEASE_DIR}/JAR/	
	
	cd ${RELEASE_DIR}/JAR/
	
	zip -r JAR.zip *
	rm -fr $(ls | grep -v JAR.zip)
	
	cd -
	
	#制作Software目录下的文件夹
	if [ ! -d ${PACKET_DIR} ]
	then
		mkdir -p "${PACKET_DIR}"
	fi
	
	cd "${PACKET_DIR}"
	
	if [ -d "${RELEASE_DIR}" ]
	then
		rm -fr "${RELEASE_DIR}"
	fi
	mkdir ${RELEASE_DIR}
	
	cd -
	mv  "${RELEASE_DIR}" "${PACKET_DIR}/"

	echo -e "create relese directory : ${PACKET_DIR}/${RELEASE_DIR} ------------>  $str_success"
	
	echo
}

python3 download_from_cmc.py
create_hpm_tools_pkg
