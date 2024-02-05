#!/bin/sh

if [ $# != 3 ]; then
	echo "argc is not correct!"
	exit 1
fi

SERVICE_TYPE="$1"
SERVICE_FILE="$2"
SERVICE_STATE="$3"

SERVICE_BASE_NAME=`basename ${SERVICE_FILE}`
TMP_FILE=/opt/pme/pram/${SERVICE_BASE_NAME}

case $SERVICE_TYPE in
	1)  
	    #ftp
	    sed "s/disable[ \t]\{1,\}=[ ]*[a-z]*/disable\t= ${SERVICE_STATE}/" ${SERVICE_FILE} > $TMP_FILE
	    ;;	
	2)  
	    #ssh do nothing
		exit 0
	    ;;	
	9)
	    #http do nothing
		exit 0
	    ;;		
	11)
	    #https do nothing
		exit 0
	    ;;	
	*)
	    echo "set severice($SERVICE_TYPE) $SERVICE_STATE fail"
		exit 2
	;;
    esac

#比较SHA256
SHA256_BAK=`/usr/bin/sha256sum $TMP_FILE | awk '{print $1," "}'`
SHA256=`/usr/bin/sha256sum $SERVICE_FILE | awk '{print $1," "}'`

chmod 644 $TMP_FILE # default permission of TMP_FILE is 666, should be changed to 644

if [ "$SHA256_BAK" != "$SHA256" ]; then
	cp -rf $TMP_FILE ${SERVICE_FILE}
	sync
	rm -rf $TMP_FILE
else
	#debug
	echo "severcie($SERVICE_TYPE)(file:${SERVICE_FILE}) set state $SERVICE_STATE: file sha256sum is same" > /dev/null
	rm -rf $TMP_FILE

fi

exit 0
