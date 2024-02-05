#!/bin/sh

chmod +x *
############################################
# bash script: 升级加载脚本
############################################
#把项目编码赋值好
project_code=$2

SAVEFILE=$project_code"_save.txt"
VERSIONNAME=$project_code"_version.ini"

# 维修包repair_hpm, 现网包online_hpm, 不校验包noverify_hpm, 默认normal_hpm
IMAGE_TYPE="normal_hpm"

source ./common_transfer_flie.sh

source ./add_temp_user.sh

#参数为 输入文件  开始字节  输出文件
hextochar()
{
    temp=`cat  $1`
    temp=`echo $temp | cut -b $2-100`

    converhex="$temp"
    #清空文件
    echo "" >$3
    for k in $converhex
    do
       TEMP="\\x$k"
       echo -n -e $TEMP >>$3      
    done
}

get_product_version_num()
{
    ipmicmd -k "0f 00 30 93 db 07 00 36 30 00 01 20 00 00 ff ff 00 01 00 07 00" smi 0 > product_version_num.txt
    if [ $? -eq 0 ] ; then
        PRODUCT_VERSION_NUM=`cat product_version_num.txt | awk '{print $NF}'`
    fi
    #返回值ff表示获取产品版本的命令还不支持，属性还没有添加，这个肯定是V3的产品
    if [ "$PRODUCT_VERSION_NUM" == "ff"  -o "$PRODUCT_VERSION_NUM" == "FF" ] ; then
        PRODUCT_VERSION_NUM=03
    fi		
    echo "PRODUCT VERSION : $PRODUCT_VERSION_NUM"
    echo "PRODUCT VERSION : $PRODUCT_VERSION_NUM" >>$SAVEFILE
    rm -f product_version_num.txt
}

#BMC 版本查询
active_bmc_version_query()
{
    run_cmd "f 0 30 90 08 0 1 0 10" "0f 31 00 90 00 80"
    if [ $? != 0 ] ; then
       echo "get BMC version $getversion fail"
       echo "get BMC version $getversion fail" >>$SAVEFILE
       error_quit
    fi

    hextochar temp.txt 18 getversion.txt
    getversion=`cat getversion.txt | grep .` >/dev/null
    searchstr="ActiveBMC=$getversion"  >/dev/null

    grep -wq "$searchstr" $VERSIONNAME >/dev/null
    if [ $? != 0 ] ; then
       echo "BMC version $getversion fail"
       echo "BMC version $getversion fail" >>$SAVEFILE
       return 1
    else
       echo "BMC version:$getversion ok"
       echo "BMC version:$getversion ok" >>$SAVEFILE
       return 0
    fi
}
backup_bmc_version_query()
{
    run_cmd "f 0 30 90 08 0 0c 0 10" "0f 31 00 90 00 80"
    if [ $? != 0 ] ; then
       echo "get backup BMC version $getversion fail"
       echo "get backup BMC version $getversion fail" >>$SAVEFILE
       error_quit
    fi

    hextochar temp.txt 18 getversion.txt
    getversion=`cat getversion.txt | grep .` >/dev/null
    searchstr="BackupBMC=$getversion"  >/dev/null

    grep -wq "$searchstr" $VERSIONNAME >/dev/null
    if [ $? != 0 ] ; then
       echo "backup BMC version $getversion fail"
       echo "backup BMC version $getversion fail" >>$SAVEFILE
       return 1
    else
       echo "backup BMC version:$getversion ok"
       echo "backup BMC version:$getversion ok" >>$SAVEFILE
       return 0
    fi
}

#BMC 版本查询
bmc_version_query()
{
    #Active BMC version query
    active_bmc_version_query
    if [ $? -ne 0 ] ; then		
        return 1
    fi
	
    #Backup BMC version query
    backup_bmc_version_query
    if [ $? -ne 0 ] ; then		
        return 1
    fi
	
    return 0
}
#启动升级 如果发升级命令失败，先发送finsh命令，延时，然后再重发升级命令，重发3次
initiate_bmc_upgrade()
{
	for((i=0;i<3;i++))
	{
		run_cmd "f 0 30 91 db 07 00 06 AA 00 01 00 0E 2F 74 6D 70 2F 69 6D 61 67 65 2E 68 70 6D" "0f 31 00 91 00 db 07 00"
		if [ $? != 0 ] ; then
		   echo "The $i time initiate BMC upgrade fail"
		   
		   #finish upgrade
		   run_cmd "f 0 30 91 db 07 00 06 55" "0f 31 00 91 00 db 07 00"
		   if [ $? != 0 ] ; then
			   echo "The $i time finish BMC upgrade fail"
			   return 1
			else
			   echo "The $i time finish BMC upgrade ok"
			   sleep 1
		   fi
		else
		   echo "initiate BMC upgrade $getversion ok"
		   return 0
		fi
	}
	
	return 1
}

#升级进度查询 连续6次查询不到进度，返回错误
get_bmc_upgrade_status()
{
	error_counter=0
	
    for((i=0;i<600;i++))
	{
		run_cmd "f 0 30 91 db 07 00 06 00" "0f 31 00 91 00 db 07 00"
		if [ $? != 0 ] ; then
			((error_counter++))
			if [ $error_counter -eq 6 ] ; then
				echo "The $i time get BMC upgrade status fail cnt: $error_counter"
				echo "The $i time get BMC upgrade status fail cnt: $error_counter" >>$SAVEFILE
				return 1
			fi
		else
		    error_counter=0
		fi
		
		status=`cat temp.txt |awk -F " " '{print $9}' `
		
		if [ $((16#$status)) -eq 228 ] ; then
		   echo "BMC upgrade status is 100%"
		   echo "BMC upgrade status is 100%" >>$SAVEFILE
		   return 0
		else
		   echo "The $i time get BMC upgrade status is $((16#$status))%"
		fi
		
		sleep 5
	}
	
	echo "get BMC upgrade status overtime" 
	echo "get BMC upgrade status overtime" >>$SAVEFILE
	return 1
}

#传输文件，升级，查询进度
start_upgrade()
{
    file_name=$1
    #查询升级文件是否存在
	#hpm_file_path=${G_WORK_DIR}/image.hpm
	hpm_file_path=${G_WORK_DIR}/$file_name
	if [ ! -f "$hpm_file_path" ] ; then
	   echo "hpm file is not present!"
	   echo "hpm file is not present!" >>$SAVEFILE
	   error_quit
	fi
	
    #传输文件
	send_file "\[fe80::9e7d:a3ff:fe28:6ffa%veth\]" $file_name /tmp/image.hpm "send"
	if [ $? != 0 ] ; then	 
	   echo "send hpm file fail!"
	   echo "send hpm file fail!" >>$SAVEFILE
	   return 1	
	fi
	
	#启动升级
	initiate_bmc_upgrade
	if [ $? != 0 ] ; then
       echo "init bmc upgrade fail" >>$SAVEFILE
	   return 1	
	fi
	
	#查询进度
	get_bmc_upgrade_status
	if [ $? != 0 ] ; then	   
	   return 1	
	fi
	
	return 0
}

#使用bt通道升级bmc
hpm_upgrade_hpmfwupg()
{
    local i
    
	for((i=0;i<$REPEAT;i++))
	{
		inputfile=in 
		rm -fr $inputfile
		mknod $inputfile p 

		exec 8<>$inputfile 

		echo "0" >$inputfile 
		echo "y" >$inputfile 
        #bmc因为要升级2次 防止跨版本场景第二次升级使用处理过的包升级 调用参数修改为image_out.hpm
        ./hpmfwupg upgrade image_out.hpm activate <&8
		if [ $? != 0 ] ; then
			sleep 30
			run_cmd "f 0 06 02" "0f 07 00 02 00"
			if [ $? != 0 ] ; then
				echo "hpm reset BMC fail $i" 
				echo "hpm reset BMC fail $i" >>$SAVEFILE
				error_quit
			fi
			sleep 60
		else
			echo "hpm load BMC ok $i"
			echo "hpm load BMC ok $i"  >>$SAVEFILE
			return 0
		fi
	}
	echo "hpm rest BMC fail $i" 
	echo "hpm rest BMC fail $i" >>$SAVEFILE
	error_quit
}

#hpm加载bmc软件
hpm_upgrade()
{
    file_name=$1
	#安装驱动
	load_driver

	for((k=0;k<$REPEAT;k++))
	{
		start_upgrade $file_name
		if [ $? != 0 ] ; then
			reset_bmc
			if [ $? != 0 ] ; then
				return 1
			fi
		else
			echo "hpm load BMC ok"
            echo "image_type $IMAGE_TYPE"
            #升级维修包起来后不立马重启，否则安全启动校验伙伴根切换到华为根无法启动
            if [ "$IMAGE_TYPE" != "repair_hpm" ] ; then
                echo "Reset BMC to valid the image..."
			    reset_bmc
            fi
			return 0
		fi
	}
	echo "hpm rest BMC fail $k" 
	echo "hpm rest BMC fail $k" >>$SAVEFILE
	return 1
}

#生成GUID
generate_guid()
{
    for((i=0;i<3;i++))
    {
        run_cmd "0f 00 30 90 27 00 47 55 49 44 aa" "0f 31 00 90 00 00"
        if [ $? -ne 0 ] ; then
            echo "please wait for 10 second!"
            sleep 10
        else 
            echo "generate guid ok!"
	    sleep 5
            return 0
        fi
    }
    if [ "$i" -eq 3 ] ; then
        echo "generate guid fail"
        echo "generate guid fail" >>$SAVEFILE
        return 1 
    fi    
}
get_guid_mac_part()
{
  guid_mac_part=`cat temp.txt | xargs | cut -b 16-32 | awk '{print $6 "\x20" $5 "\x20" $4 "\x20" $3 "\x20" $2 "\x20" $1}'`
}
do_check_guid()
{
    run_cmd "0f 00 06 08" "0f 07 00 08 00"   
    if [ $? != 0 ] ; then
        echo "get bmc guid fail"
        echo "get bmc guid fail" >>$SAVEFILE
        return 1        
    fi
    get_guid_mac_part   
    run_cmd "0f 00 0c 02 01 05 00 00" "0f 0d 00 02 00"    
    grep -wq "$guid_mac_part" temp.txt   
    if [ $? != 0 ] ; then
        echo "check guid fail"
        echo "check guid fail" >>$SAVEFILE
        return 1
    else
        echo "check guid ok"
	return 0
    fi    
}
generate_check_guid()
{
    for((var=0;var<3;var++))
    {
        generate_guid
        if [ $? -ne 0 ] ; then
            sleep 2
        else 
           do_check_guid
	   if [ $? -ne 0 ] ; then
               sleep 2
	   else
               echo "generate and check guid ok"
               echo "generate and check guid ok" >>$SAVEFILE
	       break
	   fi
        fi
    }
    if [ "$var" -eq 3 ] ; then
        echo "generate and check guid fail"
        echo "generate and check guid fail" >>$SAVEFILE
        error_quit
    fi
}
M3_no_verify()
{
    run_cmd  "f 0 30 90 21 05"  "0f 31 00 90 00"
    if [ $? != 0 ] ; then
        echo "enable M3 no verify fail!"
        echo "enable M3 no verify fail!" >>$SAVEFILE
        error_quit
    else 
        echo "enable M3 no verify ok!"
        return 0
    fi
}

# 判断是否为新efuse环境
judge_efuse()
{
    echo "start get efuse state"
    # 如果返回值为00，则代表不含有efuse
    run_cmd "0f 00 30 93 db 07 00 5b 2c 00" "0f 31 00 93 00 db 07 00 00"
    if [ $? == 0 ]; then
        echo_fail "the environment has no efuse"
        echo "the environment has no efuse" >>$SAVEFILE
        ok_quit
    else
        run_cmd "0f 00 30 93 db 07 00 5b 2b 00" "0f 31 00 93 00 db 07 00 00"
        if [ $? == 0 ]; then
            echo_fail "the environment is old efuse"
            echo "the environment is old efuse" >>$SAVEFILE
            ok_quit
        else
            echo_success "the environment is new efuse"
        fi
    fi
}
############################################################################
#########################程序开始运行#####################################
############################################################################
#先设置配置  后重启BMC  最后读取校验
dos2unix *.ini

#查看日志文件是否存在
if [ ! -f $SAVEFILE ] ; then
	rm -rf $SAVEFILE
fi 
#清空文件 写入当前时间
echo "" >$SAVEFILE
date >>$SAVEFILE


cp -f pme_dft_video_test.bmp /root

#获取版本信息
get_product_version_num

#开启dft验证
DFT_enable
sleep 1

#防止生产发货版本guid概率重复
generate_check_guid

if [ $1 == image_repairs.hpm ]; then
    #确定升级包类型
    IMAGE_TYPE="repair_hpm"
    judge_efuse
    #发送ipmi命令使能不校验版本
    M3_no_verify
fi	
#加载维修版本
hpm_upgrade image.hpm 
#通过veth升级失败则通过bt升级
if [ $? != 0 ] ; then
    ./modify_hpm_for_bt_upgrade.sh image.hpm image_out.hpm
    if [ $? != 0 ] ; then
        error_quit
    fi
    hpm_upgrade_hpmfwupg
    rm image_out.hpm
fi

sleep 180

ok_quit




