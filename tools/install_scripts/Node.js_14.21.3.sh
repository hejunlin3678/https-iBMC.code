#!/bin/bash
# ***********************************************************************
# Copyright: (c) Huawei Technologies Co., Ltd. 2022. All rights reserved.
# script for install Node.js_14.21.3
# version: 14.21.3
# change log:
# ***********************************************************************
set -eux
set -o pipefail
 
install_dir=$1
FILE_NAME=node-v14.21.3-linux-x64.tar.gz
DIR_NAME=node-v14.21.3-linux-x64

# check install files
ls ${FILE_NAME}
# clear
[ -d "${install_dir}" ] && rm -rf ${install_dir}

mkdir -p ${install_dir}

tar xf ${FILE_NAME}
cp -fr ${DIR_NAME}/* ${install_dir}

sed -i '/NODE_HOME/d' /etc/profile
echo "export NODE_HOME=${install_dir}" >> /etc/profile
echo 'export PATH=$NODE_HOME/bin:$PATH' >> /etc/profile

#create link
ln -sf ${install_dir}/bin/* /bin

#config npm
npm config rm proxy 
npm config rm http-proxy 
npm config rm https-proxy 
npm config set no-proxy .huawei.com 
npm config set strict-ssl false
npm config set registry https://cmc.centralrepo.rnd.huawei.com/artifactory/api/npm/npm-central-repo/
npm config set @cloud:registry https://cmc.centralrepo.rnd.huawei.com/artifactory/api/npm/product_npm/
npm config list -json

CACHE_DIR=/usr1/jenkins/workspace/compile/ibmc_cache
mkdir -p ${CACHE_DIR}/.npm
npm config set cache "${CACHE_DIR}/.npm"

#install angularcli
export NG_CLI_ANALYTICS=ci
export NODE_HOME=${install_dir}
export PATH=$NODE_HOME/bin:$PATH
npm install -g @angular/cli@14.2.0 --unsafe-perm=true --allow-root
npm install uglify-js -g