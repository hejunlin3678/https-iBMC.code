#!/bin/bash
# build with sdk
# Copyright © Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.

cd ../../framework/build
bash -x ./update_sdk_for_app.sh
cd -
bash -x ./build.sh

