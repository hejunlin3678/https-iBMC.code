#!/bin/bash
times=0
IP="$1"
FILE_NAME="$2"
FILE_SIZE=0
LOCAL_FILE_NAME=`basename $FILE_NAME`
CHECK_SUM_HMM_FILE_NAME="/opt/pme/pram/hmm_checksum.txt" 
result=0
sleep 1

/usr/bin/tftp -g -r "$FILE_NAME" "$IP" -l "$LOCAL_FILE_NAME" -b 2048
result=$?
while true ; do	
    FILE_SIZE=`ls -l $LOCAL_FILE_NAME | awk '{print $5}'`
    if [ $FILE_SIZE -ne 0 ] ; then
        break
    fi
    
    sleep 1	
    
	/usr/bin/tftp -g -r "$FILE_NAME" "$IP" -l "$LOCAL_FILE_NAME" -b 2048
	result=$?	
	times=`expr $times + 1`
	if [ $times -ge 4 ]; then
			
			exit 1
    fi    
done
if [ $result -ne 0 ]; then
	echo $result
	exit 1
fi
/usr/bin/openssl sha256 -out $CHECK_SUM_HMM_FILE_NAME -r "$LOCAL_FILE_NAME"
echo $result
exit 0
