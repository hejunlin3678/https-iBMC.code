#!/bin/bash
# codedex coverity
# Copyright © Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.

echo "####################################  coverity build start  ####################################"
CODECC_COVERITY_BIN=/usr/CodeDEX/tool/coverity/bin/cov-build

cd ${BUILD_PATH}
	rm -rf coverity_result
	mkdir coverity_result

	date +%s | sha256sum | base64 | head -c 14 > /tmp/random_string
        echo -n "#\$" >> /tmp/random_string
        if [ "$1" == "kvm" ]; then
		cd ../src/user_interface/kvm_client/KVM
        	${CODECC_COVERITY_BIN} --dir ${BUILD_PATH}/coverity_result ant -buildfile build_client.xml
	elif [ "$1" == "player" ]; then
		cd ../src/user_interface/kvm_client/ScreenKinescopePlayer
        	${CODECC_COVERITY_BIN} --dir ${BUILD_PATH}/coverity_result ant -buildfile build.xml
	else
		${CODECC_COVERITY_BIN} --dir ./coverity_result  ./codecc_buildapp.sh arm build $1
	fi
	rm -f /tmp/random_string

	cd ${BUILD_PATH}/coverity_result
	zip -r coverity.zip *
	cp coverity.zip ${TARGET_PATH}


echo "####################################  coverity build end  ####################################"
