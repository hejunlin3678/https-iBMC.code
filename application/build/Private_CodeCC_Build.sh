#!/bin/bash
# This scripts bulid huawei PME BMC.
# Copyright © Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.

echo "#################################update farmework end###################################"
#compile application; 
echo "###############################compile application begin################################"
bash -x ./set_env_for_compile.sh

export MODULE_NAME=iBMCV2
export CODECC_TOOL=/usr1/code/iBMC_V2R1Trunk_PrivateBuild/code/current/CI_Config/CI/script/codecc_scan
export CODECC_LOG=/usr1/codecc.log
export CODECC_RESULT_PATH=/usr1/code/iBMC_V2R1Trunk_PrivateBuild/code/current/BMC_PME_Code/build/output
CUR_PATH=$(cd $(dirname $BASH_SOURCE) && pwd)
B_MAKEFILE=$CUR_PATH/${MODULE_NAME}Make
RE_MAKEFILE=$CUR_PATH/${MODULE_NAME}Makefile
rm -rf $CUR_PATH/${MODULE_NAME}Make
rm -rf $CUR_PATH/${MODULE_NAME}Makefile

#./build_java.sh
echo "	cd $PWD  && ./codecc_buildapp.sh arm build $1" > $B_MAKEFILE

bcache -m $RE_MAKEFILE $B_MAKEFILE
pushd ${CODECC_TOOL}
source ./env.sh
bash -x ./codecc_api.sh ${MODULE_NAME} $RE_MAKEFILE $CODECC_LOG
#exit 0
export PATH=$PATH:${CODECC_TOOL}
codecc_client -s ${CODECC_SERVER_IP} -p /root -w 600
if [ 0 -ne $? ];then
	exit 1
fi
RESULT_LOG=$(cat /tmp/codecc_client)
mkdir -p $CODECC_RESULT_PATH
mkdir -p $cilogpath/
echo "<a href=\"http://${ICP_MASTER_IP}/ci-master/buildresults/${ICP_PROJECT}/${RESULT_LOG}\">${ICP_PROJECT}</a>" > ${CODECC_RESULT_PATH}/ResultURL.html

echo "###############################compile application end##################################"
