#!/bin/bash
# This script is used to process WEB JS files, enable some funcs or not
# Copyright © Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.

set -e

G_COMPATIBILITY_ENABLED=$1
G_SECURITY_ENHANCED_COMPATIBLE_BOARD_V3=$2

if [ ${G_COMPATIBILITY_ENABLED} == 1 ]; then
    echo "enable KVM encryption config"
    sed -i '/_kvm_encryption_config = false/c\    this._kvm_encryption_config = true;' ./web/webapp_ibmc/src/virtualControl/h5/security.js
fi

if [ ${G_SECURITY_ENHANCED_COMPATIBLE_BOARD_V3} == 0 ]; then
    echo "enable KVM encryption config"
    sed -i '/passwordNeedReset = false/c\            $scope.passwordNeedReset = true;' ./web/webapp_ibmc/src/common/controllers/loginCtrl.js
fi

exit 0
