#!/bin/sh

# This script is customized by application user.

rm -rf /data/var/coredump/core-*-*
sysctl -w kernel.core_pattern=/data/var/coredump/core-%e-%s

#update ssh host key
bash /opt/pme/script/update_ssh_host_key.sh >/dev/null 2>&1 &

if [ ! -d /data/opt/pme/web/custom ]; then
    mkdir -p /data/opt/pme/web/custom
fi
chmod 750 /data/opt/pme/web/custom

#language file overwrite
if [ -d /data/opt/pme/web/htdocs/bmc/resources/i18n ]; then
    rm -r /data/opt/pme/web/htdocs/bmc/resources/i18n
fi

if [ -d /data/opt/pme/web/htdocs/bmc/resources/widget ]; then
    rm -r /data/opt/pme/web/htdocs/bmc/resources/widget
fi

mkdir -p /data/opt/pme/web/htdocs/bmc/resources/i18n
chmod 755 /data/opt/pme/web/htdocs/bmc/resources/i18n

if [ -d /opt/pme/web/htdocs/bmc/resources/update/i18n ]; then
    cp -raf /opt/pme/web/htdocs/bmc/resources/update/* /data/opt/pme/web/htdocs/bmc/resources/
    chmod 755 /data/opt/pme/web/htdocs/bmc/resources/widget
    chmod 755 /data/opt/pme/web/htdocs/bmc/resources/widget/i18n
fi

if [ -d /opt/pme/web/htdocs/update/i18n ]; then
    cp -raf /opt/pme/web/htdocs/update/i18n/* /data/opt/pme/web/htdocs/bmc/resources/i18n/
    find /data/opt/pme/web/htdocs/bmc/resources/i18n -type f -exec chmod 440 {} \;
    find /data/opt/pme/web/htdocs/bmc/resources/i18n -type d -exec chmod 550 {} \;
fi

if [ ! -d /data/opt/pme/web/htdocs/theme/custom/img ]; then
    mkdir -p /data/opt/pme/web/htdocs/theme/custom/img
    chmod 755 /data/opt/pme/web/htdocs/theme/custom/img
fi

mkdir /data/opt/pme/web/conf -p
#firstboot2 script
bash /opt/pme/init/firstboot2.sh >/dev/null 2>&1

# check config files
bash /opt/pme/script/check_config_file.sh >/dev/null 2>&1

# modify /tmp permission
chgrp opetator /tmp
chmod 1777 /tmp

mkdir /tmp/web >/dev/null 2>&1
chmod 1777 /tmp/web

mkdir /dev/shm/web >/dev/null 2>&1
chmod 1770 /dev/shm/web

# kerberos SSO depended
if [ ! -d /data/var/tmp ]; then
    mkdir /data/var/tmp >/dev/null 2>&1
    chmod 755 /data/var/tmp
fi

# modify /dev/urandom permission
chmod o+r /dev/urandom >/dev/null 2>&1

# modify web access/error log permission
chmod 700 /data/opt/pme/web/logs >/dev/null 2>&1
chmod 600 /opt/pme/web/logs/access_log* >/dev/null 2>&1
chmod 600 /opt/pme/web/logs/error_log* >/dev/null 2>&1

# modify web permission
chmod 750 /usr/local/web >/dev/null 2>&1
chmod 750 /usr/local/web/modules >/dev/null 2>&1
chmod 750 /usr/local/web/bin >/dev/null 2>&1
chmod 400 /usr/local/web/modules/* >/dev/null 2>&1
chmod 755 /opt/pme/web >/dev/null 2>&1
chmod 500 /opt/pme/web/bin/* >/dev/null 2>&1
chown 98:98 /data/opt/pme/web -R >dev/null 2>&1
chown 98:200 /tmp >dev/null 2>&1
chown 98:98 /tmp/web -R >dev/null 2>&1
chown 98:103 /dev/shm/web -R >dev/null 2>&1


#为了兼容V5老Web data区i18n权限为root,防止回退老Web，web i18n文件访问失败
ls -l /opt/pme/web/htdocs/i18n | grep apache
if [ $? -ne 0 ]; then
    ls -l /opt/pme/web/htdocs/i18n | grep 98
    if [ $? -ne 0 ]; then
        chown root:root /data/opt/pme/web/htdocs/bmc/resources/i18n    
    fi   
fi

#remove redundant file /var/spool/cron/crond.log 
if [ -f /var/spool/cron/crond.log ]; then
    rm -f /var/spool/cron/crond.log
fi

# monitor task event and try to restart
bash /opt/pme/script/task_monitor.sh >/dev/null 2>&1 &

# manual oom kill 
bash /opt/pme/script/manual_oom_killer.sh >/dev/null 2>&1 &

# monitor cpu
bash /opt/pme/script/cpu_monitor.sh >/dev/null 2>&1 &

# remove fsync_per.conf, since this file will rebuild in pme
if [ -f /opt/pme/pram/fsync_reg.ini ]; then
    rm -f /opt/pme/pram/fsync_reg.ini
fi

if [ -f /data/var/log/pme/LSI_RAID_Controller_Log ]; then
    chmod 600 /data/var/log/pme/LSI_RAID_Controller_Log* >/dev/null 2>&1
fi

if [ -f /data/var/log/pme/PD_SMART_INFO_C0 ]; then
    chmod 600 /data/var/log/pme/PD_SMART_INFO_C* >/dev/null 2>&1
fi

# modify /opt/pme/pram permission
chmod 1777 /opt/pme/pram >/dev/null 2>&1

# modify log directory permission
chmod 1777 /data/var/log/storage >/dev/null 2>&1

if [ ! -f /data/var/log/lastlog ]; then
    touch /data/var/log/lastlog    
fi
chmod 600 /data/var/log/lastlog >/dev/null 2>&1

# modify log file owner
chown ipmi_user /opt/pme/pram/md_so_ipmi_debug_log* >/dev/null 2>&1
chown ipmi_user /data/var/log/pme/ipmi_debug_log* >/dev/null 2>&1
chown kvm_user /opt/pme/pram/md_so_kvm_vmm_debug_log* >/dev/null 2>&1
chown kvm_user  /data/var/log/pme/kvm_vmm_debug_log* >/dev/null 2>&1

chown 98:98 /data/opt/pme/web/htdocs/favicon.ico >/dev/null 2>&1
chown 98:98 /data/opt/pme/web/htdocs/theme/custom/img/code1.png >/dev/null 2>&1
chown 98:98 /data/opt/pme/web/htdocs/bmc/resources/images/cmn/logo.jpg >/dev/null 2>&1

# modify log file permission
chmod 440 /data/opt/pme/web/htdocs/favicon.ico >/dev/null 2>&1
chmod 644 /data/opt/pme/web/htdocs/theme/custom/img/code1.png >/dev/null 2>&1
chmod 440 /data/opt/pme/web/htdocs/bmc/resources/images/cmn/logo.jpg >/dev/null 2>&1


# modify configure file permission
chown 98:98 /data/opt/pme/web/conf -R
chmod 600 /data/opt/pme/web/conf/* -R
chmod 400 /data/opt/pme/web/conf/ssl.key/server.pfx
if [ ! -d /data/opt/pme/web/conf/ssl.key ]; then
    mkdir /data/opt/pme/web/conf/ssl.key
fi
chmod 755 /data/opt/pme/web/conf -R
chmod 640 /data/etc/weakdictionary

chmod 750 /data/etc/pam.d >/dev/null 2>&1
chmod 700 /data/etc/ntp   >/dev/null 2>&1

chmod 750 /data/opt/pme/conf/smbios >/dev/null 2>&1
chmod 750 /data/opt/pme/conf/bios >/dev/null 2>&1
chmod 750 /data/opt/pme/conf/oem >/dev/null 2>&1
chmod 750 /data/opt/pme/conf/oem/profile >/dev/null 2>&1

# icsl 
chmod 640 /data/opt/pme/web/conf/httpd-port.conf >/dev/null 2>&1
chmod 640 /data/opt/pme/web/conf/extra/httpd-ssl-ciphersuite.conf >/dev/null 2>&1
chmod 750 /data/opt/pme/web/conf >/dev/null 2>&1
chmod 750 /data/opt/pme/web/conf/extra >/dev/null 2>&1
chmod 750 /data/opt/pme/web/htdocs/bmc/resources/images/cmn  >/dev/null 2>&1
chmod 750 /data/opt/pme/web/htdocs/bmc/resources/images  >/dev/null 2>&1
# change /dev/* permission
chmod 600 /dev/*       >dev/null 2>&1
chmod 600 /dev/pts/*   >dev/null 2>&1
chmod 600 /dev/bus/usb/001/*       >dev/null 2>&1
chmod 600 /dev/bus/usb/002/*       >dev/null 2>&1

chmod 1777 /dev/bus     >dev/null 2>&1
chmod 1777 /dev/pts     >dev/null 2>&1
chmod 1777 /dev/shm     >dev/null 2>&1

chmod 666 /dev/null    >dev/null 2>&1
chmod 666 /dev/random  >dev/null 2>&1
chmod 666 /dev/urandom >dev/null 2>&1
chmod 666 /dev/zero    >dev/null 2>&1
chmod 666 /dev/log     >dev/null 2>&1
chmod 666 /dev/sec     >dev/null 2>&1
chmod 666 /dev/tty     >dev/null 2>&1
cnt=3
while ( [ $cnt -gt 0 ] )
do
	if [ -c "/dev/trng" ]; then
		chmod 644 /dev/trng
		break
	fi
	cnt=$(expr $cnt - 1)
	sleep 1
done

if [ -f /opt/pme/script/enable_local_ssh.sh ]; then
    bash /opt/pme/script/enable_local_ssh.sh >/dev/null 2>&1 &
fi

if [ -f /opt/pme/script/bak_third_party_files.sh ]; then
    bash /opt/pme/script/bak_third_party_files.sh >/dev/null 2>&1 &
fi

# creat root .ssh directory
if [ ! -d /data/etc/.ssh ]; then
    /bin/mkdir /data/etc/.ssh
    /bin/chmod 755 /data/etc/.ssh
fi

chmod 755 /data
chmod 640 /opt/pme/conf/data_dir_sync2/3/.factory_recover_point_v5/User_IPMI >dev/null 2>&1
chmod 640 /opt/pme/conf/data_dir_sync2/3/.factory_recover_point_v5/User_weakdictionary >dev/null 2>&1
chmod 640 /opt/pme/conf/data_dir_sync2/3/.factory_recover_point/User_IPMI >dev/null 2>&1

rm /data/opt/pme/nginx -rf
mkdir /data/opt/pme/nginx/logs -p
mkdir /data/opt/pme/nginx/client_body_temp -p
mkdir /data/opt/pme/nginx/fastcgi_temp -p
mkdir /data/opt/pme/nginx/proxy_temp -p
mkdir /data/opt/pme/nginx/scgi_temp -p
mkdir /data/opt/pme/nginx/uwsgi_temp -p

echo "2" > /proc/sys/net/ipv4/conf/all/arp_announce
echo "350264" > /proc/sys/net/ipv4/icmp_ratemask

# modify database file and directory permission
if [ ! -f /opt/pme/save/eo.db ]; then
    touch /opt/pme/save/eo.db
    chmod 644 /opt/pme/save/eo.db
fi

if [ ! -f /opt/pme/save/fdm_history.db ]; then
    touch /opt/pme/save/fdm_history.db
    chmod 644 /opt/pme/save/fdm_history.db
fi

chmod 0750 /data/opt/pme/save/backup >dev/null 2>&1
chmod 0750 /data/opt/pme/save/md5 >dev/null 2>&1
chmod 0750 /data/opt/pme/save/md5/backup >dev/null 2>&1
chmod 0750 /data/opt/pme/save/md5/master >dev/null 2>&1
chmod 0750 /data/var/coredump
chmod 0750 /data/opt/pme/save/hash >dev/null 2>&1
chmod 0750 /data/opt/pme/save/hash/backup >dev/null 2>&1
chmod 0750 /data/opt/pme/save/hash/master >dev/null 2>&1
#modify SP directory permission
chmod 0750 /data/sp >dev/null 2>&1
chmod 0750 /data/sp_update -R >dev/null 2>&1

if [ `uname -m`x == "aarch64x" ]; then 
    chmod 0755 /opt/pme/init/link_emmc_devs >dev/null 2>&1
    /opt/pme/init/link_emmc_devs
else
    bash /opt/pme/init/check_uboot_version.sh > /dev/null
fi


exit 0

