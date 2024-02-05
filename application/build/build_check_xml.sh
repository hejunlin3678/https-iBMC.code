#!/bin/bash
# Used during MR building and invokes the Python script, read invoked python file to obtain details.
# Copyright © Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.

cd ../src/include/ci

result=0   

python super_xml.py
result=$?

exit $result


