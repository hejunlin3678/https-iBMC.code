#!/bin/bash
# test ut for kvm
# Copyright © Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.

# global const define
KVM_PATH="$1"
KVM_REPORT="$2"

kvm_test(){
    # 确保ant已安装并运行测试
    cd $KVM_PATH
    if [ -z $ANT_HOME ]
    then
        echo "ANT_HOME DOES NOT EXIST"
        exit 1
    fi
    ant test
    
    # 查找是否有失败的用例
    grep -q "Failure" $KVM_REPORT
    if [ $? -eq 0 ]
    then
        echo ""
        echo "=============================!TEST FAILED!============================="
        echo "See ${KVM_REPORT_INDEX} for details."
        exit 1
    else
        echo ""
        echo "=============================!TEST SUCCESS!============================="
        echo "See ${KVM_REPORT_INDEX} for details."
        exit 0
    fi
}

kvm_test
