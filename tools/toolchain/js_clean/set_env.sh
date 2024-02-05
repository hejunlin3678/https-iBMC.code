#!/bin/bash
# This script compile js_compress program
# Copyright © Huawei Technologies Co., Ltd. 2021-2023. All rights reserved.

gcc -Wall js_compress.c -o js_compress
gcc -Wall jsmin_system.c -o jsmin_system

exit 0
