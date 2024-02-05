#!/bin/bash

# This scripts will run at start.sh and before start pme.
# author: 
# date: Sat May 24 11:22:48 CST 2014
# version: 0.8.0-1


# global var define
G_COUNT=13
G_FLAG="/var/firstboot2.flag"
#G_SYNC_DIR="/opt/pme/conf/data_dir_sync"

CustomizeSensorFileSrc="/opt/pme/extern/profile/CustomizeSensor.xml"
CustomizeSensorFileDst="/data/opt/pme/conf/oem/profile/CustomizeSensor.xml"


# note: every time you edit function sync_cmd, you must ++G_COUNT.

func1()
{
    if [ "$1" = "-R" ]; then
        # do nothing
        :
    else
        # session timeout limit
        # foovsftpd /etc/vsftpd/vsftpd.conf
        sed -i 's/^.*ClientAliveInterval.*$/ClientAliveInterval 900/' /data/etc/ssh/sshd_config
        sed -i 's/^.*ClientAliveCountMax.*$/ClientAliveCountMax 0/' /data/etc/ssh/sshd_config
    fi
}

func2()
{
    if [ "$1" = "-R" ]; then
        # do nothing
        :
    else
        grep 'dhcp6.client-id' /data/etc/dhclient6.conf >/dev/null 2>&1
        if [ $? != 0 ]; then
            sed -i '1isend dhcp6.client-id = concat(00:03:00, hardware);' /data/etc/dhclient6.conf
        fi
        grep 'dhcp6.client-id' /data/etc/dhclient6_ip.conf >/dev/null 2>&1
        if [ $? != 0 ]; then
            sed -i '8isend dhcp6.client-id = concat(00:03:00, hardware);' /data/etc/dhclient6_ip.conf
        fi
    fi
}

func3()
{
    if [ "$1" = "-R" ]; then
        # do nothing
        :
    else
        if [ ! -f /opt/pme/save/.factory_recover_point.tar.gz ]; then
            mkdir /opt/pme/save -p
            chmod 755 /opt/pme/save
            cp /opt/pme/conf/data_dir_sync2/3/.factory_recover_point.tar.gz /opt/pme/save/ -f
        fi
    fi
}

func4()
{
    if [ "$1" = "-R" ]; then
        # do nothing
        :
    else
        if [ ! -f /data/etc/HOSTNAME ]; then
            echo iBMC > /data/etc/HOSTNAME
        fi
    fi
}

func5()
{
    if [ "$1" = "-R" ]; then
        # do nothing
        :
    else
        if [ ! -f /data/etc/ssdp/ssdp.conf ]; then
            mkdir /data/etc/ssdp -p
            chmod 755 /data/etc/ssdp 
            cp /opt/pme/conf/data_dir_sync2/5/ssdp.conf /data/etc/ssdp/ssdp.conf -f
        fi
    fi
}

func6()
{
    if [ "$1" = "-R" ]; then
        # do nothing
        :
    else
        if [ ! -d /data/opt/pme/conf/oem/profile ]; then
            mkdir -p /data/opt/pme/conf/oem/profile
            chmod 750 /data/opt/pme/conf/oem/profile
        fi
        
        if [ ! -f /data/opt/pme/conf/oem/profile/CustomizeSensor.xml ];then
            touch /data/opt/pme/conf/oem/profile/CustomizeSensor.xml
            chmod 664 /data/opt/pme/conf/oem/profile/CustomizeSensor.xml 
        fi
    fi
}

func7()
{
    if [ "$1" = "-R" ]; then
        # do nothing
        :
    else
        #upload file privilege control
        sed -i "s/^.*\/usr\/libexec\/openssh\/sftp-server.*$/Subsystem sftp internal-sftp -l INFO -u 0077/" /data/etc/ssh/sshd_config
        sed -i "s/^.*internal-sftp -u 0077.*$/Subsystem sftp internal-sftp -l INFO -u 0077/" /data/etc/ssh/sshd_config
    fi
}

func8()
{
    if [ "$1" = "-R" ]; then
        # do nothing
        :
    else
        #change shell
		cp /data/etc/passwd /dev/shm/User_passwd
		sed -i 's/bin\/bash/opt\/pme\/bin\/clp_commands/g' /dev/shm/User_passwd
		sed -i 's/bin\/bash/opt\/pme\/bin\/clp_commands/g' /data/etc/passwd

		/bin/busybox md5sum /dev/shm/User_passwd | awk -F ' ' '{print $1}' > /dev/shm/User_passwd.md5

		cp /dev/shm/User_passwd.md5 /opt/pme/pram/User_passwd.md5
		cp /dev/shm/User_passwd /opt/pme/pram/User_passwd
		
		rm /dev/shm/User_passwd.md5
		rm /dev/shm/User_passwd
		
		# remove pam_shells.so from /data/etc/pam.d/vsftpd
		cp /data/etc/pam.d/vsftpd /dev/shm/User_vsftp
		sed -i '/pam_shells.so/d' /data/etc/pam.d/vsftpd
		sed -i '/pam_shells.so/d' /dev/shm/User_vsftp
		
		/bin/busybox md5sum /dev/shm/User_vsftp | awk -F ' ' '{print $1}' > /dev/shm/User_vsftp.md5
		
		cp /dev/shm/User_vsftp /opt/pme/pram/User_vsftp
		cp /dev/shm/User_vsftp.md5 /opt/pme/save/User_vsftp.md5
		
		rm /dev/shm/User_vsftp.md5
		rm /dev/shm/User_vsftp

		sleep 1
    fi
}

func9()
{
    if [ ! -f /data/etc/remotelog.conf] ; then
        touch /data/etc/remotelog.conf
    fi
}

func10()
{
    if [ "$1" = "-R" ]; then
        # do nothing
        :
    else
        cp /data/etc/ssh/sshd_config /dev/shm/User_sshd_config
		sed -i 's/^#AuthorizedKeysFile.*$/AuthorizedKeysFile .ssh\/authorized_keys\nPubkeyAcceptedKeyTypes=+ssh-dss/' /dev/shm/User_sshd_config
		sed -i 's/PasswordAuthentication no/PasswordAuthentication yes/' /dev/shm/User_sshd_config
		sed -i 's/^#ChallengeResponseAuthentication yes/ChallengeResponseAuthentication no/' /dev/shm/User_sshd_config
		
		# login security banner config
		sed -i 's/^#PrintMotd yes/PrintMotd no/' /dev/shm/User_sshd_config
		sed -i 's/Banner \/etc\/HOSTNAME/Banner \/etc\/motd/' /dev/shm/User_sshd_config
		
		/bin/busybox md5sum /dev/shm/User_sshd_config | awk -F ' ' '{print $1}' > /dev/shm/User_sshd_config.md5
		
		cp /dev/shm/User_sshd_config /opt/pme/pram/User_sshd_config
		cp /dev/shm/User_sshd_config /data/etc/ssh/sshd_config
		cp /dev/shm/User_sshd_config.md5 /opt/pme/pram/User_sshd_config.md5
		
		rm /dev/shm/User_sshd_config.md5
		rm /dev/shm/User_sshd_config
    fi
}

func11()
{
	if [ "$1" = "-R" ]; then
        # do nothing
        :
    else
		cp /data/etc/dhclient.conf /dev/shm/BMC_dhclient.conf 
		sed -i 's/domain-name, domain-name-servers;$/domain-name, domain-name-servers, ntp-servers;/g' /dev/shm/BMC_dhclient.conf 
		/bin/busybox md5sum /dev/shm/BMC_dhclient.conf | awk -F ' ' '{print $1}' > /dev/shm/BMC_dhclient.conf.md5 
		cp /dev/shm/BMC_dhclient.conf /opt/pme/pram/BMC_dhclient.conf 
		cp /dev/shm/BMC_dhclient.conf /data/etc/dhclient.conf 
		cp /dev/shm/BMC_dhclient.conf.md5 /opt/pme/pram/BMC_dhclient.conf.md5 
		rm /dev/shm/BMC_dhclient.conf 
		rm /dev/shm/BMC_dhclient.conf.md5
		
		cp /data/etc/dhclient6.conf /dev/shm/BMC_dhclient6.conf
		cat "/dev/shm/BMC_dhclient6.conf" | grep -i "request dhcp6.sntp-servers;"
		if [ $? == 1 ] ; then
			echo "request dhcp6.sntp-servers;" >> /dev/shm/BMC_dhclient6.conf
			/bin/busybox md5sum /dev/shm/BMC_dhclient6.conf | awk -F ' ' '{print $1}' > /dev/shm/BMC_dhclient6.conf.md5 
			cp /dev/shm/BMC_dhclient6.conf /opt/pme/pram/BMC_dhclient6.conf 
			cp /dev/shm/BMC_dhclient6.conf /data/etc/dhclient6.conf 
			cp /dev/shm/BMC_dhclient6.conf.md5 /opt/pme/pram/BMC_dhclient6.conf.md5 
			rm /dev/shm/BMC_dhclient6.conf 
			rm /dev/shm/BMC_dhclient6.conf.md5
		fi
		
		cp /data/etc/dhclient_ip.conf /dev/shm/BMC_dhclient_ip.conf 
		sed -i 's/request subnet-mask, broadcast-address, time-offset, routers;$/request subnet-mask, broadcast-address, time-offset, routers, ntp-servers;/g' /dev/shm/BMC_dhclient_ip.conf
		/bin/busybox md5sum /dev/shm/BMC_dhclient_ip.conf | awk -F ' ' '{print $1}' > /dev/shm/BMC_dhclient_ip.conf.md5 
		cp /dev/shm/BMC_dhclient_ip.conf /opt/pme/pram/BMC_dhclient_ip.conf 
		cp /dev/shm/BMC_dhclient_ip.conf /data/etc/dhclient_ip.conf 
		cp /dev/shm/BMC_dhclient_ip.conf.md5 /opt/pme/pram/BMC_dhclient_ip.conf.md5 
		rm /dev/shm/BMC_dhclient_ip.conf 
		rm /dev/shm/BMC_dhclient_ip.conf.md5
		
		cp /data/etc/dhclient6_ip.conf /dev/shm/BMC_dhclient6_ip.conf 
		cat "/dev/shm/BMC_dhclient6_ip.conf" | grep -i "request dhcp6.sntp-servers;"
		if [ $? == 1 ] ; then
			echo "request dhcp6.sntp-servers;" >> /dev/shm/BMC_dhclient6_ip.conf
			/bin/busybox md5sum /dev/shm/BMC_dhclient6_ip.conf | awk -F ' ' '{print $1}' > /dev/shm/BMC_dhclient6_ip.conf.md5 
			cp /dev/shm/BMC_dhclient6_ip.conf /opt/pme/pram/BMC_dhclient6_ip.conf 
			cp /dev/shm/BMC_dhclient6_ip.conf /data/etc/dhclient6_ip.conf 
			cp /dev/shm/BMC_dhclient6_ip.conf.md5 /opt/pme/pram/BMC_dhclient6_ip.conf.md5 
			rm /dev/shm/BMC_dhclient6_ip.conf 
			rm /dev/shm/BMC_dhclient6_ip.conf.md5			
		fi
		
		mkdir /data/etc/ntp
        chmod 755 /data/etc/ntp
    fi	
}

#add sha256sum for func8, func10, func11
func12()
{
    if [ "$1" = "-R" ]; then
        # do nothing
        :
    else
        #change shell
		/usr/bin/sha256sum /opt/pme/pram/User_passwd | awk -F ' ' '{print $1}' > /opt/pme/pram/User_passwd.sha256
		
		cp /opt/pme/pram/User_passwd /opt/pme/save/User_passwd.bak
		cp /opt/pme/pram/User_passwd.md5 /opt/pme/save/User_passwd.bak.md5
		cp /opt/pme/pram/User_passwd.sha256 /opt/pme/save/User_passwd.bak.sha256
		cp /opt/pme/pram/User_passwd.md5 /opt/pme/save/User_passwd.md5		
		cp /opt/pme/pram/User_passwd.sha256 /opt/pme/save/User_passwd.sha256
		
		# remove pam_shells.so from /data/etc/pam.d/vsftpd
		/usr/bin/sha256sum /opt/pme/pram/User_vsftp | awk -F ' ' '{print $1}' > /opt/pme/pram/User_vsftp.sha256		
		
		cp /opt/pme/pram/User_vsftp /opt/pme/save/User_vsftp.bak
		cp /opt/pme/pram/User_vsftp.md5 /opt/pme/save/User_vsftp.bak.md5
		cp /opt/pme/pram/User_vsftp.sha256 /opt/pme/save/User_vsftp.bak.sha256
		cp /opt/pme/pram/User_vsftp.md5 /opt/pme/save/User_vsftp.md5
		cp /opt/pme/pram/User_vsftp.sha256 /opt/pme/save/User_vsftp.sha256		
		
		#modify ssh config
		/usr/bin/sha256sum /opt/pme/pram/User_sshd_config | awk -F ' ' '{print $1}' > /opt/pme/pram/User_sshd_config.sha256

		cp /opt/pme/pram/User_sshd_config /opt/pme/save/User_sshd_config.bak
		cp /opt/pme/pram/User_sshd_config.md5 /opt/pme/save/User_sshd_config.bak.md5
		cp /opt/pme/pram/User_sshd_config.sha256 /opt/pme/save/User_sshd_config.bak.sha256
		cp /opt/pme/pram/User_sshd_config.md5 /opt/pme/save/User_sshd_config.md5
		cp /opt/pme/pram/User_sshd_config.sha256 /opt/pme/save/User_sshd_config.sha256		
		
		#modify dhclient config
		/usr/bin/sha256sum /opt/pme/pram/BMC_dhclient.conf | awk -F ' ' '{print $1}' > /opt/pme/pram/BMC_dhclient.conf.sha256
		
		cp /opt/pme/pram/BMC_dhclient.conf /opt/pme/save/BMC_dhclient.conf.bak
		cp /opt/pme/pram/BMC_dhclient.conf.md5 /opt/pme/save/BMC_dhclient.conf.bak.md5
		cp /opt/pme/pram/BMC_dhclient.conf.sha256 /opt/pme/save/BMC_dhclient.conf.bak.sha256
		cp /opt/pme/pram/BMC_dhclient.conf.md5 /opt/pme/save/BMC_dhclient.conf.md5
		cp /opt/pme/pram/BMC_dhclient.conf.sha256 /opt/pme/save/BMC_dhclient.conf.sha256		
		
		/usr/bin/sha256sum /opt/pme/pram/BMC_dhclient6.conf | awk -F ' ' '{print $1}' > /opt/pme/pram/BMC_dhclient6.conf.sha256

		cp /opt/pme/pram/BMC_dhclient6.conf /opt/pme/save/BMC_dhclient6.conf.bak
		cp /opt/pme/pram/BMC_dhclient6.conf.md5 /opt/pme/save/BMC_dhclient6.conf.bak.md5
		cp /opt/pme/pram/BMC_dhclient6.conf.sha256 /opt/pme/save/BMC_dhclient6.conf.bak.sha256
		cp /opt/pme/pram/BMC_dhclient6.conf.md5 /opt/pme/save/BMC_dhclient6.conf.md5
		cp /opt/pme/pram/BMC_dhclient6.conf.sha256 /opt/pme/save/BMC_dhclient6.conf.sha256		
		
		/usr/bin/sha256sum /opt/pme/pram/BMC_dhclient_ip.conf | awk -F ' ' '{print $1}' > /opt/pme/pram/BMC_dhclient_ip.conf.sha256
		
		cp /opt/pme/pram/BMC_dhclient_ip.conf /opt/pme/save/BMC_dhclient_ip.conf.bak
		cp /opt/pme/pram/BMC_dhclient_ip.conf.md5 /opt/pme/save/BMC_dhclient_ip.conf.bak.md5
		cp /opt/pme/pram/BMC_dhclient_ip.conf.sha256 /opt/pme/save/BMC_dhclient_ip.conf.bak.sha256
		cp /opt/pme/pram/BMC_dhclient_ip.conf.md5 /opt/pme/save/BMC_dhclient_ip.conf.md5
		cp /opt/pme/pram/BMC_dhclient_ip.conf.sha256 /opt/pme/save/BMC_dhclient_ip.conf.sha256		
		
		/usr/bin/sha256sum /opt/pme/pram/BMC_dhclient6_ip.conf | awk -F ' ' '{print $1}' > /opt/pme/pram/BMC_dhclient6_ip.conf.sha256 			
		
		cp /opt/pme/pram/BMC_dhclient6_ip.conf /opt/pme/save/BMC_dhclient6_ip.conf.bak
		cp /opt/pme/pram/BMC_dhclient6_ip.conf.md5 /opt/pme/save/BMC_dhclient6_ip.conf.bak.md5
		cp /opt/pme/pram/BMC_dhclient6_ip.conf.sha256 /opt/pme/save/BMC_dhclient6_ip.conf.bak.sha256
		cp /opt/pme/pram/BMC_dhclient6_ip.conf.md5 /opt/pme/save/BMC_dhclient6_ip.conf.md5
		cp /opt/pme/pram/BMC_dhclient6_ip.conf.sha256 /opt/pme/save/BMC_dhclient6_ip.conf.sha256		
		
		sleep 1
    fi
}

func13()
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
		echo -n $md5_value > per_config.ini.md5

		sha256_value=`/usr/bin/sha256sum per_config.ini | awk -F ' ' '{print $1}'`
		echo -n $sha256_value > per_config.ini.sha256

		if [ `pwd`x = "/opt/pme/pram"x ];then
			cp -f per_config.ini /opt/pme/save/per_config.ini
			cp -f per_config.ini.md5 /opt/pme/save/per_config.ini.md5
			cp -f per_config.ini.sha256 /opt/pme/save/per_config.ini.sha256
		fi
		#如果当前是save目录,由框架的文件管理同步到save

		echo y | cp per_config.ini /opt/pme/save/per_config.ini.bak
		echo y | cp per_config.ini.md5 /opt/pme/save/per_config.ini.bak.md5
		echo y | cp per_config.ini.sha256 /opt/pme/save/per_config.ini.bak.sha256

		echo `date "+%Y-%m-%d %H:%M:%S"`" per_config_check INFO: delete property Type of object "$DELETE_OBJS"." >> /var/log/pme/app_debug_log_all
	else
		echo `date "+%Y-%m-%d %H:%M:%S"`" per_config_check INFO: nothing need to be deleted." >> /var/log/pme/app_debug_log_all
	fi
	cd -
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
        chmod 755 /data/opt/pme/conf/oem/profile
    fi

    if [ ! -s $CustomizeSensorFileDst ]; then
        if [ -f $CustomizeSensorFileSrc ]; then
            cp -f $CustomizeSensorFileSrc $CustomizeSensorFileDst
        fi
    fi
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
