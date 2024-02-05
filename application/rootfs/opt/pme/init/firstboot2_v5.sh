#!/bin/bash

# This scripts will run at start.sh and before start pme.
# author: 
# date: Sat May 24 11:22:48 CST 2014
# version: 0.8.0-1


# global var define
G_COUNT=32
G_FLAG="/var/firstboot2.flag"

CustomizeSensorFileSrc="/opt/pme/extern/profile/CustomizeSensor.xml"
CustomizeSensorFileDst="/data/opt/pme/conf/oem/profile/CustomizeSensor.xml"
PSUCustomizeSensorFileSrc="/opt/pme/extern/profile/PSUCustomizeSensor.xml"
PSUCustomizeSensorFileDst="/data/opt/pme/conf/oem/profile/PSUCustomizeSensor.xml"
MAINTENANCE_LOG_FILE="/opt/pme/pram/md_so_maintenance_log"

# note: every time you edit function sync_cmd, you must ++G_COUNT.

func1()
{
	echo 0 >> /dev/null
}

func2()
{
	echo 0 >> /dev/null
}

func3()
{
	echo 0 >> /dev/null
}

func4()
{
	echo 0 >> /dev/null
}

func5()
{
	echo 0 >> /dev/null
}

func6()
{
	echo 0 >> /dev/null
}

func7()
{
	echo 0 >> /dev/null
}

func8()
{
	echo 0 >> /dev/null
}

func9()
{
	echo 0 >> /dev/null
}

func10()
{
	echo 0 >> /dev/null
}

func11()
{
	echo 0 >> /dev/null
}

func12()
{
	echo 0 >> /dev/null
}

func13()
{
    if [ ! -f "/data/opt/pme/save/.factory_recover_point.tar.gz" ]; then
		cp -rf /opt/pme/conf/data_dir_sync2/3/.factory_recover_point.tar.gz /data/opt/pme/save/.factory_recover_point.tar.gz
		return 0
    fi
	
	rm -rf /dev/shm/recover_tmp
	mkdir -p /dev/shm/recover_tmp
	tar -xzf /data/opt/pme/save/.factory_recover_point.tar.gz -C /dev/shm/recover_tmp
	cat /dev/shm/recover_tmp/User_IPMI | grep "2:root:"
    if [ $? -eq 0 ]; then
		rm -rf /data/opt/pme/save/.factory_recover_point.tar.gz
		rm -rf /opt/pme/pram/.factory_recover_point.tar.gz
		cp -rf /opt/pme/conf/data_dir_sync2/3/.factory_recover_point.tar.gz /data/opt/pme/save/.factory_recover_point.tar.gz
    fi
	
	rm -rf /dev/shm/recover_tmp
}

func14()
{
	#遍历/var/log/pme/的所有文件
	for log_name in `ls /var/log/pme/`
	do
		#查看文件大小如果文件大于1.2M 则删除掉文件，防止文件过大导致一键信息收集失败
		file_length=`ls -l /var/log/pme/$log_name | awk  '{print $5}'`
		if [ $file_length -gt 1258291 ];then
			rm -f /var/log/pme/$log_name
		fi
	done
}

func15()
{
    if [ ! -d /var/log/storage ]; then
        mkdir -p /var/log/storage
        chmod 755 /var/log/storage
    fi
}

func16()
{
    if [ "$1" = "-R" ]; then
        # do nothing
        :
    else
		cp /data/etc/dhclient.conf /dev/shm/BMC_dhclient.conf 
		sed -i 's/domain-name, domain-name-servers;$/domain-name, domain-name-servers, ntp-servers;/g' /dev/shm/BMC_dhclient.conf 
		cp /dev/shm/BMC_dhclient.conf /opt/pme/pram/BMC_dhclient.conf 
		cp /dev/shm/BMC_dhclient.conf /data/etc/dhclient.conf 
        if [ `uname -m` != "aarch64" ]; then 
            /bin/busybox md5sum /dev/shm/BMC_dhclient.conf | awk -F ' ' '{print $1}' > /dev/shm/BMC_dhclient.conf.md5 
            cp /dev/shm/BMC_dhclient.conf.md5 /opt/pme/pram/BMC_dhclient.conf.md5 
            rm /dev/shm/BMC_dhclient.conf.md5
        fi
        rm /dev/shm/BMC_dhclient.conf 
		
		cp /data/etc/dhclient6.conf /dev/shm/BMC_dhclient6.conf
		cat "/dev/shm/BMC_dhclient6.conf" | grep -i "request dhcp6.sntp-servers;"
		if [ $? == 1 ] ; then
			echo "request dhcp6.sntp-servers;" >> /dev/shm/BMC_dhclient6.conf
			cp /dev/shm/BMC_dhclient6.conf /opt/pme/pram/BMC_dhclient6.conf 
			cp /dev/shm/BMC_dhclient6.conf /data/etc/dhclient6.conf 
            if [ `uname -m` != "aarch64" ]; then 
                /bin/busybox md5sum /dev/shm/BMC_dhclient6.conf | awk -F ' ' '{print $1}' > /dev/shm/BMC_dhclient6.conf.md5 
                cp /dev/shm/BMC_dhclient6.conf.md5 /opt/pme/pram/BMC_dhclient6.conf.md5 
                rm /dev/shm/BMC_dhclient6.conf.md5
            fi
			rm /dev/shm/BMC_dhclient6.conf 
		fi
		
		cp /data/etc/dhclient_ip.conf /dev/shm/BMC_dhclient_ip.conf 
		sed -i 's/request subnet-mask, broadcast-address, time-offset, routers;$/request subnet-mask, broadcast-address, time-offset, routers, ntp-servers;/g' /dev/shm/BMC_dhclient_ip.conf
		cp /dev/shm/BMC_dhclient_ip.conf /opt/pme/pram/BMC_dhclient_ip.conf 
		cp /dev/shm/BMC_dhclient_ip.conf /data/etc/dhclient_ip.conf 
        if [ `uname -m` != "aarch64" ]; then 
            /bin/busybox md5sum /dev/shm/BMC_dhclient_ip.conf | awk -F ' ' '{print $1}' > /dev/shm/BMC_dhclient_ip.conf.md5 
            cp /dev/shm/BMC_dhclient_ip.conf.md5 /opt/pme/pram/BMC_dhclient_ip.conf.md5 
            rm /dev/shm/BMC_dhclient_ip.conf.md5
        fi
		rm /dev/shm/BMC_dhclient_ip.conf 
		
		cp /data/etc/dhclient6_ip.conf /dev/shm/BMC_dhclient6_ip.conf 
		cat "/dev/shm/BMC_dhclient6_ip.conf" | grep -i "request dhcp6.sntp-servers;"
		if [ $? == 1 ] ; then
			echo "request dhcp6.sntp-servers;" >> /dev/shm/BMC_dhclient6_ip.conf
			cp /dev/shm/BMC_dhclient6_ip.conf /opt/pme/pram/BMC_dhclient6_ip.conf 
			cp /dev/shm/BMC_dhclient6_ip.conf /data/etc/dhclient6_ip.conf 
            if [ `uname -m` != "aarch64" ]; then 
                /bin/busybox md5sum /dev/shm/BMC_dhclient6_ip.conf | awk -F ' ' '{print $1}' > /dev/shm/BMC_dhclient6_ip.conf.md5 
                cp /dev/shm/BMC_dhclient6_ip.conf.md5 /opt/pme/pram/BMC_dhclient6_ip.conf.md5 
                rm /dev/shm/BMC_dhclient6_ip.conf.md5
            fi
			rm /dev/shm/BMC_dhclient6_ip.conf 			
		fi
		
		#calculate sh256 of dhclient config file
		/usr/bin/sha256sum /opt/pme/pram/BMC_dhclient.conf | awk -F ' ' '{print $1}' > /opt/pme/pram/BMC_dhclient.conf.sha256
		
		cp /opt/pme/pram/BMC_dhclient.conf /opt/pme/save/BMC_dhclient.conf.bak
		cp /opt/pme/pram/BMC_dhclient.conf.sha256 /opt/pme/save/BMC_dhclient.conf.bak.sha256
		cp /opt/pme/pram/BMC_dhclient.conf.sha256 /opt/pme/save/BMC_dhclient.conf.sha256
        if [ `uname -m` != "aarch64" ]; then 
            cp /opt/pme/pram/BMC_dhclient.conf.md5 /opt/pme/save/BMC_dhclient.conf.bak.md5
            cp /opt/pme/pram/BMC_dhclient.conf.md5 /opt/pme/save/BMC_dhclient.conf.md5
        fi		
		
		/usr/bin/sha256sum /opt/pme/pram/BMC_dhclient6.conf | awk -F ' ' '{print $1}' > /opt/pme/pram/BMC_dhclient6.conf.sha256
		cp /opt/pme/pram/BMC_dhclient6.conf /opt/pme/save/BMC_dhclient6.conf.bak
		cp /opt/pme/pram/BMC_dhclient6.conf.sha256 /opt/pme/save/BMC_dhclient6.conf.bak.sha256
		cp /opt/pme/pram/BMC_dhclient6.conf.sha256 /opt/pme/save/BMC_dhclient6.conf.sha256	
        if [ `uname -m` != "aarch64" ]; then 
            cp /opt/pme/pram/BMC_dhclient6.conf.md5 /opt/pme/save/BMC_dhclient6.conf.bak.md5
            cp /opt/pme/pram/BMC_dhclient6.conf.md5 /opt/pme/save/BMC_dhclient6.conf.md5
        fi	
		
		/usr/bin/sha256sum /opt/pme/pram/BMC_dhclient_ip.conf | awk -F ' ' '{print $1}' > /opt/pme/pram/BMC_dhclient_ip.conf.sha256
		cp /opt/pme/pram/BMC_dhclient_ip.conf /opt/pme/save/BMC_dhclient_ip.conf.bak
		cp /opt/pme/pram/BMC_dhclient_ip.conf.sha256 /opt/pme/save/BMC_dhclient_ip.conf.bak.sha256
		cp /opt/pme/pram/BMC_dhclient_ip.conf.sha256 /opt/pme/save/BMC_dhclient_ip.conf.sha256	
        if [ `uname -m` != "aarch64" ]; then 
            cp /opt/pme/pram/BMC_dhclient_ip.conf.md5 /opt/pme/save/BMC_dhclient_ip.conf.bak.md5
            cp /opt/pme/pram/BMC_dhclient_ip.conf.md5 /opt/pme/save/BMC_dhclient_ip.conf.md5
        fi	
		
		/usr/bin/sha256sum /opt/pme/pram/BMC_dhclient6_ip.conf | awk -F ' ' '{print $1}' > /opt/pme/pram/BMC_dhclient6_ip.conf.sha256 			
		cp /opt/pme/pram/BMC_dhclient6_ip.conf /opt/pme/save/BMC_dhclient6_ip.conf.bak
		cp /opt/pme/pram/BMC_dhclient6_ip.conf.sha256 /opt/pme/save/BMC_dhclient6_ip.conf.bak.sha256
		cp /opt/pme/pram/BMC_dhclient6_ip.conf.sha256 /opt/pme/save/BMC_dhclient6_ip.conf.sha256
        if [ `uname -m` != "aarch64" ]; then 
            cp /opt/pme/pram/BMC_dhclient6_ip.conf.md5 /opt/pme/save/BMC_dhclient6_ip.conf.bak.md5
            cp /opt/pme/pram/BMC_dhclient6_ip.conf.md5 /opt/pme/save/BMC_dhclient6_ip.conf.md5
        fi
        

		mkdir /data/etc/ntp
        chmod 755 /data/etc/ntp
    fi	
}

func17()
{
	#更改目录权限
    if [ ! -d /data/etc/license ]; then
        mkdir -p /data/etc/license
        chmod 755 /data/etc/license
    fi
}

func18()
{

	if [ -f /opt/pme/pram/per_config.ini ];then
		cd /opt/pme/pram/
	else
		cd /opt/pme/save/
	fi

	eval $(awk 'BEGIN{del_obj="";del_line=""} /^\[.+\]/{obj_name=$0} obj_name!~/^\[Eth.+\]|^\[veth\]/&&/^Type=..;y;/{del_obj=sprintf("%s%s",del_obj,obj_name);del_line=sprintf("%s%dd;",del_line,NR)} END{printf("DELETE_OBJS=\"%s\";DELETE_LINES=\"%s\"",del_obj,del_line)}' ./per_config.ini)

	if [ -n "$DELETE_LINES" ];then                                              
		sed -i "${DELETE_LINES}" per_config.ini

		md5_value=`/usr/bin/md5sum per_config.ini | awk -F ' ' '{print $1}'`
		
        if [ `uname -m` != "aarch64" ]; then 
            echo -n $md5_value > per_config.ini.md5
        fi

		sha256_value=`/usr/bin/sha256sum per_config.ini | awk -F ' ' '{print $1}'`
		echo -n $sha256_value > per_config.ini.sha256

		if [ `pwd`x = "/opt/pme/pram"x ];then
			cp -f per_config.ini /opt/pme/save/per_config.ini
            if [ `uname -m` != "aarch64" ]; then 
                cp -f per_config.ini.md5 /opt/pme/save/per_config.ini.md5
            fi
			cp -f per_config.ini.sha256 /opt/pme/save/per_config.ini.sha256
		fi
		#如果当前是save目录,由框架的文件管理同步到save

		echo y | cp per_config.ini /opt/pme/save/per_config.ini.bak
        if [ `uname -m` != "aarch64" ]; then 
            echo y | cp per_config.ini.md5 /opt/pme/save/per_config.ini.bak.md5
        fi
		echo y | cp per_config.ini.sha256 /opt/pme/save/per_config.ini.bak.sha256

		echo `date "+%Y-%m-%d %H:%M:%S"`" per_config_check INFO: delete property Type of object "$DELETE_OBJS"." >> /var/log/pme/app_debug_log_all
	else
		echo `date "+%Y-%m-%d %H:%M:%S"`" per_config_check INFO: nothing need to be deleted." >> /var/log/pme/app_debug_log_all
	fi
	cd -
}

func19()
{
	echo 0 >> /dev/null
}

func20()
{
    echo `date "+%Y-%m-%d %H:%M:%S"`" remove data acquisition database begin." >> /var/log/pme/app_debug_log_all
    # 删除数据采集功能的数据库文件
    rm -f /data/opt/pme/dataacquisition/data_acq.db
    rm -f /data/opt/pme/dataacquisition/data_acq_another.db
    echo `date "+%Y-%m-%d %H:%M:%S"`" remove data acquisition database end." >> /var/log/pme/app_debug_log_all
}

APACHE_SSL_CIPHER_SUITE_FILE=/data/opt/pme/web/conf/extra/httpd-ssl-ciphersuite.conf
func21()
{
    cat $APACHE_SSL_CIPHER_SUITE_FILE | grep "SSLCipherSuite" > /dev/null 2>&1
    if [ $? != 0 ] ; then
        echo "SSLCipherSuite ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES256-GCM-SHA384:DHE-RSA-AES256-GCM-SHA384:ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES128-GCM-SHA256:DHE-RSA-AES128-GCM-SHA256:ECDHE-RSA-CHACHA20-POLY1305:DHE-RSA-AES128-CCM:DHE-RSA-AES256-CCM:DHE-RSA-CHACHA20-POLY1305:ECDHE-ECDSA-AES128-CCM:ECDHE-ECDSA-AES256-CCM:ECDHE-ECDSA-CHACHA20-POLY1305:TLS_CHACHA20_POLY1305_SHA256:TLS_AES_256_GCM_SHA384:TLS_AES_128_GCM_SHA256" > $APACHE_SSL_CIPHER_SUITE_FILE
    fi
}

func22()
{
    echo 0 >> /dev/null
}

func23()
{
    chmod 640 /data/var/dhcp/dhclient.leases
}

func24()
{
    chmod 640 $G_FLAG
}

func25()
{
    echo 0 >> /dev/null
}

func26()
{
    echo 0 >> /dev/null
}

func27()
{
    if [ -f /opt/pme/pram/per_config.ini ];then
        cd /opt/pme/pram/
    else
        cd /opt/pme/save/
    fi
	
    file='per_config.ini'
    TLSVers=$(grep -o -E TLSVersion=[0-9a-fA-F]{4} ${file})
    tls=0xFCFF
    tls=$(printf "%X" ${tls})
    tls=${TLSVers:0:11}${tls}
    sed -i "s/TLSVersion=[0-9a-fA-F]*/${tls}/g" ${file}

    md5_value=`/usr/bin/md5sum per_config.ini | awk -F ' ' '{print $1}'`
    if [ `uname -m` != "aarch64" ]; then 
        echo -n $md5_value > per_config.ini.md5
    fi

    sha256_value=`/usr/bin/sha256sum per_config.ini | awk -F ' ' '{print $1}'`
    echo -n $sha256_value > per_config.ini.sha256

    if [ `pwd`x = "/opt/pme/pram"x ];then
        cp -f per_config.ini /opt/pme/save/per_config.ini
        if [ `uname -m` != "aarch64" ]; then 
            cp -f per_config.ini.md5 /opt/pme/save/per_config.ini.md5
        fi
        cp -f per_config.ini.sha256 /opt/pme/save/per_config.ini.sha256
    fi
    #如果当前是save目录,由框架的文件管理同步到save
    echo y | cp per_config.ini /opt/pme/save/per_config.ini.bak
    if [ `uname -m` != "aarch64" ]; then 
        echo y | cp per_config.ini.md5 /opt/pme/save/per_config.ini.bak.md5
    fi
    echo y | cp per_config.ini.sha256 /opt/pme/save/per_config.ini.bak.sha256

    echo `date "+%Y-%m-%d %H:%M:%S"`" INFO : Enable TLS1.3, Disable TLS1.0/TLS1.1 Successfully." >> $MAINTENANCE_LOG_FILE
    cd -
}

func28()
{
    currPwd=$(pwd)
	if [ -f /opt/pme/pram/per_reset.ini ];then
		cd /opt/pme/pram/
	else
		cd /opt/pme/save/
	fi

    if grep -q "\[Payload\]" per_reset.ini
    then
        echo "[Payload] is exist!"
        NewStr=`cat per_reset.ini | grep -A 26 "\[BMC\]" | grep "^Watchdog2\>" | sed 's/[ ]*$//g' | sed 's/^[ ]*//g'`
        OldStr=`cat per_reset.ini | grep -A 26 "\[Payload\]" | grep "^Watchdog2\>" | sed 's/[ ]*$//g' | sed 's/^[ ]*//g'`
        echo "OldStr: "${OldStr}
        echo "NewStr: "${NewStr}
        if [ ${OldStr} == "Watchdog2=0000000000000000" ];then
	        echo "[Payload] is NULL!"
            sed -i 's/'"${OldStr}"'/'"${NewStr}"'/g' per_reset.ini
			echo `date "+%Y-%m-%d %H:%M:%S"`" per_reset INFO: copy wdt2 property Type of object " >> /var/log/pme/app_debug_log_all
		else
		    echo `date "+%Y-%m-%d %H:%M:%S"`" per_reset INFO: nothing need to be copy." >> /var/log/pme/app_debug_log_all
        fi
    else
        NewStr=`cat per_reset.ini | grep -A 26 "\[BMC\]" | grep "^Watchdog2\>" | sed 's/[ ]*$//g' | sed 's/^[ ]*//g'`
	    echo -e "\n[Payload]" >> ./per_reset.ini
	    echo ${NewStr}>> ./per_reset.ini
		echo `date "+%Y-%m-%d %H:%M:%S"`" per_reset INFO: add wdt2 property Type of object " >> /var/log/pme/app_debug_log_all
    fi
    if [ `uname -m` != "aarch64" ]; then 
	    md5_value=`/usr/bin/md5sum per_reset.ini | awk -F ' ' '{print $1}'`
	    echo -n $md5_value > per_reset.ini.md5
    fi
	sha256_value=`/usr/bin/sha256sum per_reset.ini | awk -F ' ' '{print $1}'`
	echo -n $sha256_value > per_reset.ini.sha256

	if [ `pwd`x = "/opt/pme/pram"x ];then
		cp -f per_reset.ini /opt/pme/save/per_reset.ini
		if [ `uname -m` != "aarch64" ]; then 
            cp -f per_reset.ini.md5 /opt/pme/save/per_reset.ini.md5
		fi
        cp -f per_reset.ini.sha256 /opt/pme/save/per_reset.ini.sha256
	fi
    #如果当前是save目录,由框架的文件管理同步到save
	echo y | cp per_reset.ini /opt/pme/save/per_reset.ini.bak
    if [ `uname -m` != "aarch64" ]; then 
	    echo y | cp per_reset.ini.md5 /opt/pme/save/per_reset.ini.bak.md5
	fi
    echo y | cp per_reset.ini.sha256 /opt/pme/save/per_reset.ini.bak.sha256	
	cd $currPwd
}

func29()
{
    chmod 755 /data/var/log
    chmod 755 /data/var/log/pme
    # 90M以上的日志文件保留最后1000行
    find /data/var/log/pme -size +92160k | while read name;do newname=${name}_bak;tail -1000 ${name} > ${newname};cat ${newname} > ${name}; rm -f ${newname};done
}

# 自定义协议中或者标准协议允许时必须支持DH自定义域参数,长度4096
func30()
{
    # SSH协议 DH参数 支持4096以及以上长度，由于在data分区，因此启动的时候强制写一次
    if [ -f /opt/pme/extern/moduli ]; then
        cp -f /opt/pme/extern/moduli /data/etc/ssh/moduli
        echo `date "+%Y-%m-%d %H:%M:%S"`" INFO : Enable SSH DH 4096 Successfully." >> /var/log/pme/app_debug_log_all
    fi
}

# 自定义弱密码字典
func31()
{
    if [ -f /opt/pme/extern/weakdictionary ]; then
        cp -f /opt/pme/extern/weakdictionary /dev/shm/User_weakdictionary
        /usr/bin/sha256sum /dev/shm/User_weakdictionary | awk -F ' ' '{print $1}' > /dev/shm/User_weakdictionary.sha256
        cp /dev/shm/User_weakdictionary /opt/pme/pram/User_weakdictionary
        cp /dev/shm/User_weakdictionary.sha256 /opt/pme/pram/User_weakdictionary.sha256
        cp /opt/pme/pram/User_weakdictionary /opt/pme/save/User_weakdictionary.bak
        cp /opt/pme/pram/User_weakdictionary.sha256 /opt/pme/save/User_weakdictionary.bak.sha256
        cp /opt/pme/pram/User_weakdictionary.sha256 /opt/pme/save/User_weakdictionary.sha256
        rm /dev/shm/User_weakdictionary
        rm /dev/shm/User_weakdictionary.sha256
        echo `date "+%Y-%m-%d %H:%M:%S"`" INFO : Enable customized weak dictionary Successfully." >> /var/log/pme/app_debug_log_all
    fi
}

func32()
{
    sed -i 's/MaxStartups 10:30:100/MaxStartups 10:30:12/g' /data/etc/ssh/sshd_config
    cp /data/etc/ssh/sshd_config /dev/shm/User_sshd_config 
    sed -i '/^RSAAuthentication /d' /dev/shm/User_sshd_config
    sed -i '/^# For this to work you will also need host keys in /d' /dev/shm/User_sshd_config
    sed -i '/^RhostsRSAAuthentication /d' /dev/shm/User_sshd_config

    /usr/bin/sha256sum /dev/shm/User_sshd_config | awk -F ' ' '{print $1}' > /dev/shm/User_sshd_config.sha256 	
    cp /dev/shm/User_sshd_config /opt/pme/pram/User_sshd_config
    cp /dev/shm/User_sshd_config.sha256 /opt/pme/pram/User_sshd_config.sha256
    cp /opt/pme/pram/User_sshd_config /opt/pme/save/User_sshd_config.bak
    cp /opt/pme/pram/User_sshd_config.sha256 /opt/pme/save/User_sshd_config.bak.sha256
    cp /opt/pme/pram/User_sshd_config.sha256 /opt/pme/save/User_sshd_config.sha256
    if [ `uname -m` != "aarch64" ]; then 
        /usr/bin/md5sum /dev/shm/User_sshd_config| awk -F ' ' '{print $1}' > /dev/shm/User_sshd_config.md5
        cp /dev/shm/User_sshd_config.md5 /opt/pme/pram/User_sshd_config.md5
        cp /opt/pme/pram/User_sshd_config.md5 /opt/pme/save/User_sshd_config.bak.md5
        cp /opt/pme/pram/User_sshd_config.md5 /opt/pme/save/User_sshd_config.md5
        rm /dev/shm/User_sshd_config.md5
    fi		

    rm /dev/shm/User_sshd_config
    rm /dev/shm/User_sshd_config.sha256	
}

SSL_CIPHER_SUITE_FILE=/data/opt/pme/web/conf/nginx_ssl_ciphersuite.conf
check_ssl_ciphersuite_conf()
{
    if [ ! -f $SSL_CIPHER_SUITE_FILE ] ; then
        touch $SSL_CIPHER_SUITE_FILE
    fi
    cat $SSL_CIPHER_SUITE_FILE | grep "ssl_ciphers" > /dev/null 2>&1
    if [ $? != 0 ] ; then
        echo "ssl_ciphers ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES256-GCM-SHA384:DHE-RSA-AES256-GCM-SHA384:ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES128-GCM-SHA256:DHE-RSA-AES128-GCM-SHA256:ECDHE-RSA-CHACHA20-POLY1305:DHE-RSA-AES128-CCM:DHE-RSA-AES256-CCM:DHE-RSA-CHACHA20-POLY1305:ECDHE-ECDSA-AES128-CCM:ECDHE-ECDSA-AES256-CCM:ECDHE-ECDSA-CHACHA20-POLY1305:TLS_CHACHA20_POLY1305_SHA256:TLS_AES_256_GCM_SHA384:TLS_AES_128_GCM_SHA256;" > $SSL_CIPHER_SUITE_FILE
    fi
}

web_cfg()
{
    rm -rf /dev/shm/web
    mkdir -p /dev/shm/web/logs
    mkdir -p /dev/shm/web/proxy_temp
    mkdir -p /dev/shm/web/client_body_temp
    chmod 755 -R /dev/shm/web
    chown 98:98 -R /dev/shm/web
}

sync_cmd()
{
    if [ ! -f "$G_FLAG" ]; then
        echo 0 > "$G_FLAG"
    fi

    local count=$G_COUNT
    local flag=$(cat "$G_FLAG")

    if [ $flag -lt $count ]; then
		while [ $flag -lt $count ]; do
			let flag=flag+1
            func${flag} ""  
        done
    fi
}

update_flag()
{
    echo "$G_COUNT" > "$G_FLAG"
}

default_sensor_config()
{
    if [ ! -d /data/opt/pme/conf/oem/profile ]; then
        mkdir -p /data/opt/pme/conf/oem/profile
        chmod 750 /data/opt/pme/conf/oem/profile
    fi

    if [ ! -s $CustomizeSensorFileDst ]; then
        if [ -f $CustomizeSensorFileSrc ]; then
            cp -f $CustomizeSensorFileSrc $CustomizeSensorFileDst
        fi
    fi
	
	if [ ! -s $PSUCustomizeSensorFileDst ]; then
        if [ -f $PSUCustomizeSensorFileSrc ]; then
            cp -f $PSUCustomizeSensorFileSrc $PSUCustomizeSensorFileDst
        fi
    fi

    chmod 640 /data/opt/pme/conf/oem/profile/CustomizeSensor.xml
    chmod 640 /data/opt/pme/conf/oem/profile/PSUCustomizeSensor.xml
}

if [ -s "$G_FLAG" ] && (flag=`cat "$G_FLAG"` && test -n "$flag") && [ "$G_COUNT" = `cat "$G_FLAG"` ]; then
    # do nothing
    :
else
    sync_cmd;
    update_flag;
fi

default_sensor_config;

check_ssl_ciphersuite_conf;

web_cfg;

exit 0
