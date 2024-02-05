#!/bin/bash
# ***********************************************************************
# Copyright: (c) Huawei Technologies Co., Ltd. 2022. All rights reserved.
# script for install Apache Ant 1.10.13
# version: 1.10.13
# change log:
# ***********************************************************************
set -eux
set -o pipefail

install_dir=$1
FILE_NAME=apache-ant-1.10.13-bin.tar.gz
DIR_NAME=apache-ant-1.10.13

#check install files
ls ${FILE_NAME}

[ -d "${install_dir}" ] && rm -rf ${install_dir}
mkdir -p ${install_dir}
tar xf ${FILE_NAME}
cp -fr ${DIR_NAME}/* ${install_dir}

#set env
sed -i '/ANT_HOME/d' /etc/profile
echo "export ANT_HOME=${install_dir}" >> /etc/profile
echo 'export CLASSPATH=$ANT_HOME/lib:$CLASSPATH' >> /etc/profile
echo 'export PATH=$ANT_HOME/lib:$ANT_HOME/bin:$PATH' >> /etc/profile

#create link
rm -rf /usr/bin/ant
ln -fs ${install_dir}/bin/ant /usr/bin/ant
ln -fs ${install_dir}/bin/ant /usr/local/bin/ant
ln -fs ${install_dir}/bin/antRun /usr/local/bin/antRun
ln -fs ${install_dir}/bin/antRun.pl /usr/local/bin/antRun.pl
ln -fs ${install_dir}/bin/complete-ant-cmd.pl /usr/local/bin/complete-ant-cmd.pl
ln -fs ${install_dir}/bin/runant.pl /usr/local/bin/runant.pl
ln -fs ${install_dir}/bin/runant.py /usr/local/bin/runant.py