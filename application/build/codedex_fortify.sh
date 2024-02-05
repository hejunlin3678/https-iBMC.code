#!/bin/bash
# codedex fortify
# Copyright © Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.

FORITY_ROOT_DIR=/tmp/fortify
CODECC_FORTIFY_BIN=/usr/CodeDEX/tool/fortify/bin/sourceanalyzer
echo "####################################  fortify build start  ####################################"
cd ${FORITY_ROOT_DIR}
while [ 1 ]
do
   # touch busy
    if [ $? -eq 0 ]; then
        break;
    else
        echo "wait for task schedule......"
        sleep 10
    fi
done
cd ${BUILD_PATH}
${CODECC_FORTIFY_BIN} -b ibmcv2 -clean

date +%s | sha256sum | base64 | head -c 14 > /tmp/random_string
echo -n "#\$" >> /tmp/random_string

if [ "$1" == "all" ]; then
	${CODECC_FORTIFY_BIN}  -b ibmcv2 touchless  ./build.sh arm
elif [ "$1" == "kvm" ]; then
	cd ../src/user_interface/kvm_client/KVM
	${CODECC_FORTIFY_BIN}  -b bmc_java touchless ant -buildfile build_client.xml
elif [ "$1" == "player" ]; then
	cd ../src/user_interface/kvm_client/ScreenKinescopePlayer
	${CODECC_FORTIFY_BIN}  -b bmc_java touchless ant -buildfile build.xml 
else
	${CODECC_FORTIFY_BIN}  -b ibmcv2 touchless  ./codecc_buildapp.sh arm build  $1 
fi
rm -f /tmp/random_string

cd ${FORITY_ROOT_DIR}
zip -r fortify.zip sca6.2/
mkdir ${TARGET_PATH}
cp fortify.zip ${TARGET_PATH}
rm -rf *
echo "####################################  fortify build end  ####################################"
