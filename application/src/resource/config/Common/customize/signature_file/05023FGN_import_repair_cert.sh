chmod +x *
############################################
# bash script: 导入维修凭证
############################################
#把项目编码赋值好
project_code=`echo $0 | awk -F "_" '{printf $1}'`
project_code=`echo $project_code | awk -F "/" '{printf $2}'`

SAVEFILE=$project_code"_save.txt"
RCERT_FILE_NAME=partner_rcert.bin
RINFO_FILE_NAME=partner_rinfo.bin
RINFO_SIGN_FILE="$RINFO_FILE_NAME.rsa"

is_file_exist()
{
	if [ ! -e $1 ]
	then
		echo "File $1 does not exist!"
		echo "File $1 does not exist" | tee -a >>$SAVEFILE
		return 1
	else
		return 0
	fi
}

source ./common_transfer_flie.sh

source ./add_temp_user.sh
#生成伙伴根信息
#partner_rinfo.bin partner_rinfo.bin.rsa 检查这两个文件是否存在
is_file_exist $RINFO_FILE_NAME
if [ $? != 0 ] ; then	 
    error_quit	
fi

is_file_exist $RINFO_SIGN_FILE
if [ $? != 0 ] ; then	 
    error_quit	
fi

cp -f $RINFO_FILE_NAME $RCERT_FILE_NAME
cat $RINFO_SIGN_FILE >> $RCERT_FILE_NAME

#查看/tmp/partner_prcert.bin文件是否存在
is_file_exist $RCERT_FILE_NAME
if [ $? != 0 ] ; then	 
    error_quit	
fi
#加载驱动
load_driver

#scp传输文件
send_file "\[fe80::9e7d:a3ff:fe28:6ffa%veth\]" ${G_WORK_DIR}/partner_rcert.bin /tmp/partner_rcert.bin "send"
if [ $? != 0 ] ; then	 
    echo "send file fail!"
    echo "send file fail!" >>$SAVEFILE
    error_quit	
fi

#导入维修凭证
run_cmd "0f 00 30 93 db 07 00 57 00 02 05 00 00 16 2f 74 6d 70 2f 70 61 72 74 6e 65 72 5f 72 63 65 72 74 2e 62 69 6e" "0f 31 00 93 00 db 07 00"
if [ $? != 0 ] ; then
    echo "create import Repair certificate task failed"
    echo "create import Repair certificate task failed" >>$SAVEFILE
    error_quit
fi
#清根动作需要一段时间
sleep 30

echo "ipmicmd import Repair certificate successfully" >>$SAVEFILE

#重启bmc
reset_bmc
if [ $? != 0 ] ; then
   echo "reset failed"
   echo "reset failed" >>$SAVEFILE
   error_quit
fi

#执行导出维修凭证ipmi命令，判断返回值是否是0x81
run_cmd "f 0 30 93 db 07 00 57 00 01 04 00 00 16 2f 74 6d 70 2f 70 61 72 74 6e 65 72 5f 72 69 6e 66 6f 2e 62 69 6e" "0f 31 00 93 81"
if [ $? != 0 ] ; then
    echo "ipmicmd export Repair certificate failed"
    echo "ipmicmd export Repair certificate failed" >>$SAVEFILE
    error_quit
fi

echo "ipmicmd export Repair certificate successfully"
echo "ipmicmd export Repair certificate successfully" >>$SAVEFILE
ok_quit
