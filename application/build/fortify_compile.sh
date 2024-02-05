#!/bin/bash
# fortify compile
# Copyright © Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.

BUILD_DIR=${pwd}

export CODEDEX_HOME=/usr2/tools/plugins/CodeDEX/tool/fortify
export PATH=${PATH}:${CODEDEX_HOME}/bin

#add fortify hook
find . -name 'Makefile*' | xargs sed -i 's/gcc/sourceanalyzer -b my_fortify_build gcc/g'

prj="tqe_sh"
exe="./Compile_CI.sh"

sudo chmod +x *
/usr2/tools/plugins/CodeDEX/tool/fortify/bin/sourceanalyzer -b ${prj} -clean
/usr2/tools/plugins/CodeDEX/tool/fortify/bin/sourceanalyzer -b ${prj}  touchless ${exe}
