#!/bin/bash
# This scripts bulid huawei PME BMC.test ignore OS type.
# Copyright © Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.

# children need
#export G_VERSION="1.8.4"
#export G_RELEASE=1
#export G_RPM_SPEC_DIR=$(pwd)

# this file need
G_WORK_DIR="$4"
G_ARCH="$1"  # valid option: "", "arm"
G_STEP="$2"  # valid option: "", "build", "install"
G_MOULDE="$3"
G_STRIP=arm32-linux-gnueabi-strip



package() {

    if [ "$G_ARCH" == "arm" ]; then
        # make open source libs ready
        cd ${G_WORK_DIR}/target/tmp_root/usr
        pwd
        tar -zxvf lib.tar.gz
        rm -f ./lib.tar.gz
	    if [ "$2" == "strip" ]; then
            cd ${G_WORK_DIR}/target/tmp_root
            sudo $G_STRIP ./lib/lib*
            sudo $G_STRIP ./usr/lib/lib*
            sudo $G_STRIP ./bin/*
            sudo $G_STRIP ./usr/bin/*
            sudo $G_STRIP ./sbin/*
            sudo $G_STRIP ./usr/sbin/*
            sudo $G_STRIP ./usr/local/web/modules/*
            sudo $G_STRIP ./usr/local/web/bin/*
            sudo $G_STRIP ./opt/pme/apps/*
            sudo $G_STRIP ./opt/pme/sbin/*
            sudo $G_STRIP ./opt/pme/bin/*
            sudo $G_STRIP ./opt/pme/lib/*
        fi
        # make file symbol link
        cd ${G_WORK_DIR}/target/tmp_root
        pwd
        cp ${G_WORK_DIR}/../rootfs/get_file_link.sh .
        sh ./get_file_link.sh
        rm -f ./get_file_link.sh

        cd ${G_WORK_DIR}/target/tmp_root


        # make them execable
        sudo chmod a+x ./bin/*
        sudo chmod a+x ./usr/bin/*
        sudo chmod a+x ./sbin/*
        sudo chmod a+x ./usr/sbin/*
        sudo chmod a+x ./etc/init.d/*
        sudo chmod a+x ./etc/rc.d/*
        sudo chmod a+x ./etc/sysconfig/network-scripts/*
        sudo chmod a+x ./usr/local/web/bin/*
        sudo chmod a+x ./opt/pme/bin/*
        sudo chmod a+x ./usr/script/*
        sudo chmod a+x ./usr/libexec/openssh/*
        
        #安全红线整改：包含加密口令的文件
        sudo chown 98:98 ./data/var/log/pme/record.xml #修改为apache属主和分组
        sudo chmod 666 ./data/var/log/pme/record.xml
        sudo chmod 644 ./data/etc/passwd
        sudo chmod 600 ./data/etc/shadow
        sudo chmod 600 ./data/etc/ipmi
        sudo chmod 600 ./data/etc/group
        sudo chmod 600 ./data/etc/snmp/snmpd.conf
        sudo chmod 750 ./data/etc/snmp
        sudo chmod 0400 ./opt/pme/extern/app.list
        sudo chmod 700 ./opt/pme/upgrade
        sudo chmod 400 ./opt/pme/upgrade/*
        sudo chmod 400 ./opt/pme/web/conf/ssl.key/A-server.pfx
        sudo chmod 400 ./opt/pme/web/conf/ssl.key/A-server-ECC.pfx
        sudo chmod 0600 ./data/etc/ssh/sshd_config
		chmod 400 ./opt/pme/conf/ssl/piv.conf
        sudo chmod 500 ./opt/pme/conf/data_dir_sync
        sudo chmod 550 ./etc/pam.d
        sudo chmod 550 ./opt/pme/web/conf
        sudo chmod 750 ./data/opt/pme/web/conf
        sudo chmod 600 ./opt/pme/web/htdocs/bmc/pages/jar/*.jar
    elif [ "$G_ARCH" == "" ]; then
        echo x86 no lib no file symbol link
    else
        echo arch is wrong
        exit 1
    fi

    cd ${G_WORK_DIR}/target/tmp_root
    tar --format=gnu -czvf $1 .

}

package
