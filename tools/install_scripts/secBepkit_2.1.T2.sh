#!/bin/bash
# ***********************************************************************
# Copyright: (c) Huawei Technologies Co., Ltd. 2022. All rights reserved.
# script for install BepKit-2.1.T2
# version: 1.0.0
# change log:
# ***********************************************************************
set -eux
set -o pipefail

# solve the install error
install_dir=$1
# [ -z ${install_dir} ] && install_dir=/opt/buildtools/secbepkit-2.1.0
install_dir=/opt/buildtools/secBepkit-2.1.T2
[ -d ${install_dir} ] && rm -rf ${install_dir}
mkdir -p ${install_dir}
rm -rf ${install_dir}

# check install files
ls secBepkit-2.1.T2-1.suse.x86_64.rpm Rebuild_Maven_Plugin.zip
# clear

[ -f /usr/lib/bep_env.so ] && rm -fr /usr/lib/bep_env.so
[ -f /usr/lib64/bep_env.so ] && rm -fr /usr/lib64/bep_env.so


#install
rpm -ivh secBepkit-2.1.T2-1.suse.x86_64.rpm --force --nodeps

ln -fs ${install_dir}/bep_env.sh /usr/local/bin/bep_env.sh
unzip -o Rebuild_Maven_Plugin.zip -d ${install_dir}

#set env
sed -i '/SECBEPKIT_HOME/d' /etc/profile
echo "export BEP_ENV_PATH=${install_dir}" >> /etc/profile
echo 'export PATH=$BEP_ENV_PATH:$PATH' >> /etc/profile
echo "export BEPHOME=${install_dir}" >> /etc/profile
echo 'export PATH=$BEPHOME:$PATH' >> /etc/profile

export BEP_PATH=${install_dir}
cd $BEP_PATH
chmod +x ./bep_env.sh
sed -i 's/BEP_INOLIST=.*/BEP_INOLIST=gs gzip cpio/g' bep_env.conf
sed -i 's/BEP_UIDLIST=.*/BEP_UIDLIST=ar zip gzip/g' bep_env.conf
sed -i 's/BEP_TIMELIST=.*/BEP_TIMELIST=cc1 cc1plus ar strip lzma tar zip gzip xz bzip2 mksquashfs cpio java ant mkfs.jffs2 guile aarch64-linux-gnu-ar bash mkimage mkfs.cramfs hpmimage hpmimage_ext4 jar jarsigner/g' bep_env.conf
sed -i 's/BEP_GTDLIST=.*/BEP_GTDLIST=g++ label_bin gs iscsi-name aarch64-linux-gnu-cpp arm-linux-gnueabi-gcc mips-wrs-linux-gcc createbin tar doxygen cc1 jar java jarsigner/g' bep_env.conf
sed -i 's/BEP_CGTLIST=.*/BEP_CGTLIST=date zip python mkfs.jffs2 tar gzip aarch64-linux-gnu-ar aarch64-linux-gnu-ld aarch64-linux-gnu-objcopy aarch64-linux-gnu-objdump bash sh file ld mkimage mkfs.cramfs hpmimage hpmimage_ext4 java jar jarsigner/g' bep_env.conf
sed -i 's/BEP_FILE_TIME_LIST=.*/BEP_FILE_TIME_LIST=cc1 cc1plus ar strip lzma tar zip gzip mkfs.jffs2 cpio aarch64-linux-gnu-ar aarch64-linux-gnu-gcc aarch64-linux-gnu-ld aarch64-linux-gnu-objcopy aarch64-linux-gnu-objdump arm-linux-gnueabi-gcc arm-linux-gnueabi-g++ bash sh as awk cat chmod cp file grep ld mkimage mkfs.cramfs hpmimage hpmimage_ext4 jar java jarsigner/g' bep_env.conf
sed -i 's/BEP_DIRLIST=.*/BEP_DIRLIST=ls make gmake qmake dmake pmake emake cmake remake depmod find tar zip mksquashfs jar java jarsigner/g' bep_env.conf

#check bep_env.sh
bep_env.sh -v
