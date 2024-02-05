#!/bin/sh
crond_count=0
drop_caches_count=0


#  Judge process exists
pidproc(){
    pid=`ps -e | grep $1 |awk '{print $1}'`
}

startproc(){
pidproc $1
if [ "$pid" == "" ]; then
    if [ "$1" == "crond" ]; then
        /etc/init.d/crond restart &
    else
        $2$1 &
    fi
fi
}          


#disable log of drop_caches in kernel 4.4
echo 4 > /proc/sys/vm/drop_caches

while true
do
sleep 60

#drop_caches
drop_caches_count=$((drop_caches_count+1))
if [ $drop_caches_count -ge 1 ]; then
	echo 3 > /proc/sys/vm/drop_caches
    drop_caches_count=0
fi

#crond
crond_count=$((crond_count+1))
if [ $crond_count -ge 10 ]; then
    startproc crond NULL
    crond_count=0
fi


done
