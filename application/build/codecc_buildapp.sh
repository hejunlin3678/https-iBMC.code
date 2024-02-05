#!/bin/bash
# This scripts bulid huawei PME BMC.test ignore OS type.
# Copyright © Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.

# children need
#export G_VERSION="1.8.4"
#export G_RELEASE=1
#export G_RPM_SPEC_DIR=$(pwd)

# this file need
G_OS_TYPE=""
G_WORK_DIR=$(pwd)
G_ARCH="$1"  # valid option: "", "arm"
G_STEP="$2"  # valid option: "", "build", "install"
G_MOULDE="$3"
G_STRIP=arm32-linux-gnueabi-strip

#make all scripts to executable.
#find .. -name '*.sh' -type f | xargs chmod u+x

# get utils function
#export G_DEBUG="yes"
#source ./utils.sh

if [ -n "$G_DEBUG" ]; then
    set -x
fi

# adjust rpm version and release number.
#adjust_version;

# create rpm build topdir.
#mkdir -p /root/rpmbuild/BUILD
#mkdir -p /root/rpmbuild/RPMS
#mkdir -p /root/rpmbuild/SOURCES
#mkdir -p /root/rpmbuild/SPECS
#mkdir -p /root/rpmbuild/SRPMS

help_info() {
    echo
    echo "Usage: $(basename $0) [arm] [step]"
    echo
    echo "Example: $(basename $0) arm (1710 version)"
    echo "         $(basename $0) arm build"
	echo "         $(basename $0) arm package"
	echo "         $(basename $0) (x86 version)"
}

build() {
    rm -rf ${G_WORK_DIR}/target >/dev/null 2>&1
    mkdir ${G_WORK_DIR}/target

    cd ${G_WORK_DIR}/target
    if [ "$G_ARCH" == "arm" ]; then
        make_1710_package;
    elif [ "$G_ARCH" == "" ]; then
        make_x86_package;
    else
        echo arch is wrong
        help_info; 
        exit 1
    fi
    
    if [ $G_MOULDE == "user_interface" ] || [ "$G_MOULDE" == "libs" ]; then
	cd ./src/$G_MOULDE
    elif [ $G_MOULDE != "all" ]; then

	cd ./src/apps/$G_MOULDE
    fi
    
    make -j 4 
    #make $3
    #make -j 4
    if [ $? != 0 ]; then
        exit 2
    fi

}


package() {
    rm -f ${G_WORK_DIR}/$1 >/dev/null 2>&1
    rm -rf ${G_WORK_DIR}/target/tmp_root >/dev/null 2>&1
    mkdir -p ${G_WORK_DIR}/target/tmp_root/opt/pme
    
    cd ${G_WORK_DIR}/target
    make install
    
	if [ $? != 0 ]; then
        exit 3
    fi

    date > ${G_WORK_DIR}/target/tmp_root/opt/pme/build_date.txt
    svn info ${G_WORK_DIR}/.. >> ${G_WORK_DIR}/target/tmp_root/opt/pme/app-revision.txt

    # clean svn configure file
    find ${G_WORK_DIR}/target/tmp_root -name '.svn' -type d | xargs rm -rf >/dev/null 2>&1

    # make scripts execuable
    find ${G_WORK_DIR}/target/tmp_root -name '*.sh' -type f | xargs chmod u+x

    # clean svn configure file
    find ${G_WORK_DIR}/target/tmp_root -name 'CMakeLists.txt' -type f | xargs rm -rf >/dev/null 2>&1

    # clean js script's comment and blackspace.
#    cd "$G_WORK_DIR"
#    rm -f *.js
#    for line in $(find ${G_WORK_DIR}/target/tmp_root -name '*.js'); do
#        file_name=$(basename $line)
#        dir=${line%/*}
#        $(${G_WORK_DIR}/../toolchain/js_clean/jsmin_system ${G_WORK_DIR}/../toolchain/js_clean/js_compress "$line" "$file_name")
#        mv "$file_name" "$dir"
#    done
#    cd -
    sh ${G_WORK_DIR}/codecc_buildapp_package.sh "$1" "$2" "$3" "${G_WORK_DIR}"

}


make_package() {
	rm -rf output
	mkdir output
    if [ "$G_STEP" == "" ] || [ "$G_STEP" == "build" ]; then
        build;
    fi

    #if [ "$G_STEP" == "" ] || [ "$G_STEP" == "package" ]; then
     #   if [ "$G_ARCH" == "arm" ]; then
         #   package "${G_WORK_DIR}/output/app_strip.tar.gz" "strip";
         #   package "${G_WORK_DIR}/output/app.tar.gz" "";
      #  else
       #     package "${G_WORK_DIR}/output/app.tar.gz" "";
      #  fi    
   # fi
}

#为便于调试，直接生成一个tar包，可以放到别的x86机器上跑
make_x86_package() {
    # make sure CMAKE_LIBRARY_PATH is right.
    export CMAKE_LIBRARY_PATH=/usr/local/lib
    cmake -DCMAKE_TOOLCHAIN_FILE=../arch/x86/x86-linux.cmake -DCMAKE_INSTALL_PREFIX=${G_WORK_DIR}/target/tmp_root ../..
}

make_1710_package() {
    # make sure CMAKE_LIBRARY_PATH is right.
    export CMAKE_LIBRARY_PATH=${G_WORK_DIR}/../open_source/lib
    source /opt/RTOS/dlsetenv.sh -p armA9le_3.10_ek -v V100R005C00
    cmake -DCMAKE_TOOLCHAIN_FILE=../arch/arm/arm-linux-gnueabi-ultimate.cmake  -DCMAKE_INSTALL_PREFIX=${G_WORK_DIR}/target/tmp_root ../..
}


# this is main
make_package;
    


set +x

exit 0

