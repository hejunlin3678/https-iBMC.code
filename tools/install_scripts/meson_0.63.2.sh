#!/bin/bash
# ***********************************************************************
# Copyright: (c) Huawei Technologies Co., Ltd. 2021. All rights reserved.
# script for install meson-0.63.2
# version: 1.0.0
# change log:
# ***********************************************************************
set -eux
set -o pipefail
 
install_dir=$1
# check install files
ls meson-0.63.2.tar.gz
# clear
[ -d "${install_dir}" ] && rm -rf ${install_dir}
mkdir -p ${install_dir}

which python
#install
tar -zxvf  meson-0.63.2.tar.gz
cd meson-0.63.2/
python setup.py install

Python_Path=`which python`
if [ -L $Python_Path ]; then
    paths=`readlink -f ${Python_Path}`
    ln -sf $(dirname ${paths})/{{toolName}} /usr/local/bin/{{toolName}}
else
    ln -sf $(dirname ${Python_Path})/{{toolName}} /usr/local/bin/{{toolName}}
fi


#link
python_dir=$(dirname $(which python|xargs ls -l|awk '{print $NF}'|sed 's/python$//g'))
ln -sf ${python_dir}/meson /usr/local/bin/meson
