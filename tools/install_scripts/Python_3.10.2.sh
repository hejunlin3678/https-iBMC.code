#!/bin/bash
# ***********************************************************************
# Copyright: (c) Huawei Technologies Co., Ltd. 2022. All rights reserved.
# script for install python-3.10.2
# version: 3.10.2
# change log:
# ***********************************************************************
set -eux
set -o pipefail

install_dir=$1
FILE_NAME=Python-3.10.2.tgz
DIR_NAME=Python-3.10.2

# check install files
ls ${FILE_NAME}

# clear
[ -d "${install_dir}" ] && rm -rf ${install_dir}
mkdir -p ${install_dir}


# check or install denpendence
rpm -q xz-devel libffi48-devel  sqlite3-devel

tar -xf ${FILE_NAME}
cd ${DIR_NAME}/

# install openssl 1.1.1
wget --no-check-certificate \
    https://cmc-hgh-artifactory.cmc.tools.huawei.com/artifactory/opensource_general/OpenSSL/1.1.1n/package/openssl-1.1.1n.tar.gz
tar -xf openssl-1.1.1n.tar.gz
pushd openssl-1.1.1n
./config --prefix=/usr/local/openssl
make -j$(nproc)
make install
echo /usr/local/openssl/lib > /etc/ld.so.conf.d/openssl.conf
ldconfig -v
popd

cat >>Modules/Setup 2>/dev/null <<EOF
OPENSSL=/usr/local/openssl/
_ssl _ssl.c \\
    -I\$(OPENSSL)/include -L\$(OPENSSL)/lib \\
    -lssl -lcrypto
_hashlib _hashopenssl.c \\
    -I\$(OPENSSL)/include -L\$(OPENSSL)/lib \\
    -lcrypto
EOF

cat Modules/Setup | grep ssl
# enable sqlite3
./configure --enable-loadable-sqlite-extensions --prefix=${install_dir} --with-ensurepip=install
make -j$(nproc)
make install

rm -rf /usr/bin/python3
ln -sf ${install_dir}/bin/python3.10 /usr/bin/python
ln -sf ${install_dir}/bin/python3.10 /usr/bin/python3
ln -sf ${install_dir}/bin/python3.10 /usr/local/bin/python3
ln -sf ${install_dir}/bin/python3.10 /usr/local/bin/python
echo "export PATH=${install_dir}/bin:\$PATH" >>/etc/profile
