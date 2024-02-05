#!/bin/bash
result=0
sleep 1
CHECK_SUM_BIOS_FILE_NAME="/opt/pme/pram/bios_checksum.txt" 
FILE_NAME="$1"
LOCAL_FILE_NAME=`basename $FILE_NAME`

/usr/bin/tftp -p -r "$3" "$2" -l "$1"
result=$?
times=0
while true ; do
	if [ $result -eq 0 ] ; then
		break
	fi
	sleep 1
	/usr/bin/tftp -p -r "$3" "$2" -l "$1"
	result=$?
	times=`expr $times + 1`
	if [ $times -ge 4 ]; then
		echo $result
		exit 1
    	fi
done
/usr/bin/openssl sha256 -out $CHECK_SUM_BIOS_FILE_NAME -r "$LOCAL_FILE_NAME"
echo $result
exit 0