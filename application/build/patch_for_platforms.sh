#!/bin/bash
# This scripts bulid huawei PME BMC.
# Copyright © Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.

set -e

# this file need
CURRENT_DIR=$(dirname $(realpath $0))
CURRENT_V2R2_TRUNK=$(realpath ${CURRENT_DIR}/../../)
PLATFORMS_DIR=${CURRENT_V2R2_TRUNK}/platforms
APPLICATION_LIBS_DIR=${CURRENT_V2R2_TRUNK}/application/src/libs
#make all scripts to executable.

patch_for_kmc() {
	echo "patch for kmc"
    cd ${CURRENT_V2R2_TRUNK}
	# 增加日志打印patch
    patch -N ${PLATFORMS_DIR}/kmc/src/sdp/sdp_file.c ${APPLICATION_LIBS_DIR}/kmc/sdp_filec.patch
    patch -N ${PLATFORMS_DIR}/kmc/src/sdp/sdp_file.h ${APPLICATION_LIBS_DIR}/kmc/sdp_fileh.patch
	patch -N ${PLATFORMS_DIR}/kmc/src/common/wsecv2_util.h ${APPLICATION_LIBS_DIR}/kmc/wsecv2_utilh.patch
	
}

patch_for_platforms() {

	# kmc增加patch
	patch_for_kmc
}


# 为所有的平台组件打patch
patch_for_platforms

exit 0

