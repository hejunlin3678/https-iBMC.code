#!/bin/sh

# This scripts will run at end of /etc/rc.d/rc.local.
# author: 
# date: Thu Sep  5 17:28:33 CST 2013
# version: 0.8.0-1
if [ -f /opt/pme/init/unpack.sh ]; then
    sh /opt/pme/init/unpack.sh
fi
# insert product based drivers.
if [ -f /opt/pme/extern/insmod_driver.sh ]; then
    source /opt/pme/extern/insmod_driver.sh
elif [ -f /opt/pme/init/insmod_driver.sh ]; then 
    sh /opt/pme/init/insmod_driver.sh
else
    #do_nothing
    :
fi

# start pme
echo 8192 > /proc/sys/net/ipv4/tcp_max_tw_buckets

# dump global var.
echo 0x21 > /proc/self/coredump_filter

# make sure the coredump file directory is exist
if [ ! -d /var/coredump ]; then
	mkdir /var/coredump
    chmod 755 /var/coredump
fi
sysctl -w kernel.core_pattern=/data/var/coredump/core-%e-`awk 'NR==1{print}' /opt/pme/build_date.txt | sed 's/[ :]//g'`

# turn off sysrq at user space.
echo 0 > /proc/sys/kernel/sysrq

# firstboot adapt
if [ -f /opt/pme/init/firstboot.sh ]; then
    /opt/pme/init/firstboot.sh
fi

# eco_ft
if [ -f /opt/pme/init/start_ft.sh ]; then
    /opt/pme/init/start_ft.sh
fi

#  /data dir operate end

# do something before start pme begin
if [ -f /opt/pme/init/start_customization.sh ]; then
    sh /opt/pme/init/start_customization.sh >/dev/null 2>&1
fi
# do something before start pme end

busybox ipcs -m
# turn on process monitor
export ENABLE_MODULE_AUTO_RESTART=1
export MAX_RESTART_LIMIT=1000
# turn on APP register and init monitor
export ENABLE_APP_INIT_TIMEOUT=1
export APP_INIT_TIMEOUT=240
export APP_REG_TIMEOUT=40

# config krb path
export KRB5_CONFIG=/data/opt/pme/conf/ssl/kerberos.conf
export KRB5_KTNAME=/data/opt/pme/conf/ssl/kerberos.keytab

/opt/pme/sbin/start_pme.sh start &

/usr/sbin/logrotate /etc/logrotate.conf > /dev/null 2>&1 &

/bin/chmod 640 /var/lib/logrotate.status

/bin/chmod 500 /opt/pme/script/syslog_restart_with_tz.sh
