#!/bin/bash
# ***********************************************************************
# Copyright: (c) Huawei Technologies Co., Ltd. 2022. All rights reserved.
# script for install AdoptOpenJDK/openjdk8-binaries
# version: 8u372b07
# change log:
# ***********************************************************************
set -eux
set -o pipefail

install_dir=$1
FILE_NAME=OpenJDK8U-jdk_x64_linux_hotspot_8u372b07.tar.gz
DIR_NAME=jdk8u372-b07
SEC_FILE=${install_dir}/jre/lib/security/java.security

# check install files
ls ${FILE_NAME}
[ -d "${install_dir}" ] && rm -rf ${install_dir}
mkdir -p ${install_dir}
tar -xzf ${FILE_NAME}
cp -fa ${DIR_NAME}/*  ${install_dir}

# install https certificate
cd  ${install_dir}/jre/lib/security/
wget --no-check-certificate  https://cmc-szver-artifactory.cmc.tools.huawei.com/artifactory/CMC-Release/certificates/HuaweiITRootCA.cer
wget --no-check-certificate  https://cmc-szver-artifactory.cmc.tools.huawei.com/artifactory/CMC-Release/certificates/HWITEnterpriseCA1.cer
${install_dir}/bin/keytool -keystore cacerts -importcert -alias HuaweiITRootCA -file HuaweiITRootCA.cer -storepass changeit -noprompt
${install_dir}/bin/keytool -keystore cacerts -importcert -alias HWITEnterpriseCA1 -file HWITEnterpriseCA1.cer -storepass changeit -noprompt

# Set Environment Variable
sed -i '/JAVA_HOME/d' /etc/profile
sed -i '/JRE_HOME/d'  /etc/profile
echo "export JAVA_HOME=${install_dir}"    >> /etc/profile
echo "export JRE_HOME=${install_dir}/jre" >> /etc/profile
echo 'export CLASSPATH=$JAVA_HOME/lib:$JRE_HOME/lib:$CLASSPATH' >> /etc/profile
echo 'export PATH=$JAVA_HOME/bin:$JRE_HOME/bin:$PATH'           >> /etc/profile
ln -sf ${install_dir}/bin/java /usr/bin