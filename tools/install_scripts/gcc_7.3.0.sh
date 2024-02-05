#!/bin/bash
# ***********************************************************************
# Copyright: (c) Huawei Technologies Co., Ltd. 2019. All rights reserved.
# script for install gcc-7.3.0
# version: 7.3.0
# change log:
# ***********************************************************************
set -eux
set -o pipefail

install_dir=$1
# check install files
ls gmp-6.2.1.tar.xz mpc-1.2.1.tar.gz mpfr-4.1.0.tar.gz gcc-7.3.0.tar.gz Backport-CVE-2019-15847-CVE-2019-15847-c.patch Backport-CVE-2018-12886-CVE-2018-12886-c.patch
# clear
[ -d "${install_dir}" ] && rm -rf ${install_dir}
mkdir -p ${install_dir}

tmp_cpus=$(grep -w processor /proc/cpuinfo|wc -l)

# install gmp
gmp_install_dir="${install_dir}/gmp-6.2.1"
[ -d "${gmp_install_dir}" ] && rm -rf ${gmp_install_dir}
mkdir -p ${gmp_install_dir}
tar xf gmp-6.2.1.tar.xz
cd gmp-6.2.1
./configure --prefix=${gmp_install_dir}
make -j ${tmp_cpus}
make install

# install mpfr
mpfr_install_dir="${install_dir}/mpfr-4.1.0"
[ -d "${mpfr_install_dir}" ] && rm -rf ${mpfr_install_dir}
mkdir -p ${mpfr_install_dir}
cd ..
tar xf mpfr-4.1.0.tar.gz
cd mpfr-4.1.0
./configure --prefix=${mpfr_install_dir} --with-gmp=${gmp_install_dir}
make -j ${tmp_cpus}
make install

# install mpc
mpc_install_dir="${install_dir}/mpc-1.2.1"
[ -d "${mpc_install_dir}" ] && rm -rf ${mpc_install_dir}
mkdir -p ${mpc_install_dir}
cd ..
tar xf mpc-1.2.1.tar.gz
cd mpc-1.2.1
./configure --prefix=${mpc_install_dir} --with-gmp=${gmp_install_dir} --with-mpfr=${mpfr_install_dir}
make -j ${tmp_cpus}
make install
cd ..

# install gcc
[ -d "gcc-7.3.0" ] && rm -rf gcc-7.3.0
tar -xzf gcc-7.3.0.tar.gz
cp *.patch gcc-7.3.0
cd gcc-7.3.0

# install patch
patch -p1 -f < Backport-CVE-2019-15847-CVE-2019-15847-c.patch
patch -p1 -f < Backport-CVE-2018-12886-CVE-2018-12886-c.patch
# do not need libsanitizer
sed -i '/target-libsanitizer \\/d' configure.ac
sed -i '/target-libsanitizer \\/d' configure
./configure --prefix=${install_dir} --with-mpc=${mpc_install_dir} --with-gmp=${gmp_install_dir} --with-mpfr=${mpfr_install_dir} --enable-checking=release

echo "${mpc_install_dir}/lib" >> /etc/ld.so.conf.d/libc.conf
echo "${gmp_install_dir}/lib" >> /etc/ld.so.conf.d/libc.conf
echo "${mpfr_install_dir}/lib" >> /etc/ld.so.conf.d/libc.conf
ldconfig
make -j ${tmp_cpus} VERBOSE=1
make install

#link
ln -sf ${install_dir}/bin/gcc  ${install_dir}/bin/cc

ln -sf ${install_dir}/bin/* /usr/local/bin
# Environment Variable
sed -i '/GCC_HOME/d' /etc/profile
echo "export GCC_HOME=${install_dir}" >> /etc/profile
echo "export PATH=\$GCC_HOME/bin:\$PATH" >> /etc/profile
update-alternatives --install /usr/bin/gcc gcc /usr/local/bin/gcc 99
update-alternatives --install /usr/bin/g++ g++ /usr/local/bin/g++ 99
cat >>/etc/ld.so.conf <<EOF
${install_dir}/lib
${install_dir}/lib64
${install_dir}/libexec
EOF
cat >>/etc/profile <<EOF
export PATH=${install_dir}/bin:\$PATH
EOF
