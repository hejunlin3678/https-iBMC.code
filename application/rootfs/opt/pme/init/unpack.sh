
EXTERN_PATH="/opt/pme/extern"
DATA_EXTERN_PATH="/data/opt/pme/extern"

#get package name

board_id=`cat /proc/sys_info/board_id | awk -F 'x' '{printf $2}'`
if [ ${#board_id} == 1 ]; then
    board_id="0"$board_id
fi

platform_id=`cat /proc/sys_info/platform_id| awk -F 'x' '{printf $2}'`
if [ $platform_id == 0 ]; then
    platform_id=""
fi
package_key="_"$platform_id$board_id

echo `date` "Platform ID:"$platform_id";Board ID:"$board_id

PACKAGE_NAME=$EXTERN_PATH"/"`ls $EXTERN_PATH |grep -E "$package_key[_\.].*tar\.gz$"`

#check sha256

CHECKSUM_FILE=$PACKAGE_NAME".sha256"
CHECK_SUCCESS=1
for line in `cat $CHECKSUM_FILE`
do
        eval $(echo  $line | awk -F ':' '{printf("file_name=%s;sha256_value=%s",$1,$2)}' )

        calc_value=`/usr/bin/openssl sha256 $DATA_EXTERN_PATH"/"$file_name | awk -F ' ' '{print $2}'`

        if [ "$calc_value"x != "$sha256_value"x ]; then
           CHECK_SUCCESS=0
	   echo $line" "$calc_value
           break;
        fi
done

#unpack
mkdir -p $DATA_EXTERN_PATH
chmod 755 DATA_EXTERN_PATH
if [ $CHECK_SUCCESS -eq 0 ]; then
    tar -xzf $PACKAGE_NAME -C $DATA_EXTERN_PATH
fi

#mount

/bin/mount -t ext4 -o nosuid,nodev,noexec -rw $DATA_EXTERN_PATH"/profile.img" $EXTERN_PATH"/profile"
/bin/mount -t ext4 -o nosuid,nodev,noexec -rw $DATA_EXTERN_PATH"/web.img" $EXTERN_PATH"/web" 
/bin/chown 98:98 $EXTERN_PATH"/web" -R
/bin/chmod 750 $EXTERN_PATH"/web" 
/bin/mount -o remount,nosuid,nodev,noexec -r $EXTERN_PATH"/web"
/bin/chmod 750 $EXTERN_PATH"/profile" 
/bin/mount -o remount,nosuid,nodev,noexec -r $EXTERN_PATH"/profile"
echo `date` unpack $PACKAGE_NAME successful
