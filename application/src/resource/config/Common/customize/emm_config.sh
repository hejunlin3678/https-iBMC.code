#!/bin/bash
#********************************************************************
#  FileName	: config.sh
#  Author		:      
#  history		: first create
#  Date		    : 2014/09/23
#  Description  :
#                 $1 --- customset.ini
#                 $2 --- serial.ini
#                 $3 --- $project_code
#********************************************************************

chmod +x *
#把项目编码赋值好
project_code=$3
#命令失败时的重试次数 和间隔 
REPEAT=6
REPEAT_TIME=1

SAVEFILE=$project_code"_save.txt"
VERSIONNAME=$project_code"_version.ini"
CONFIGFILE=config.ini

USER_FINISH_FLAG=1 #判断用户定制化是否完成的标志，0表示部分配置缺失，1表示未尝试定制化，2表示定制化完成
USENM_N=""
USECRDT_N=""
USEPTY_N=""
USECRDTVP_N=""
USEITF_N=""

#如果FRU有修改，则增加，后面统一更新fru
FRU_CHANGE=0

echo_fail()
{
	echo -e "\033[31;1m$1\033[0m"
}

echo_success()
{
	echo -e "\033[32;1m$1\033[0m"
}

error_quit()
{
	echo_fail "Run customize config command fail"
	echo "Run customize config command fail" >>$SAVEFILE
	exit 1
}

ok_quit()
{
	echo_success "Run all customize config command sucess"
	echo "Run all customize config command sucess" >>$SAVEFILE
	exit 0
}

#参数为 输入文件  开始字节  输出文件
hextochar()
{
    temp=`cat  $1`
    temp=`echo $temp | cut -b $2-120`

    converhex="$temp"

    #清空文件
    >$3

    for k in $converhex
    do
       TEMP="\\x$k"
       echo -n -e $TEMP >>$3      
    done
}

function try_add_user() 
{
	### 6,如果用户名，用户密码均已获取，则添加用户
	if [ "$USENM_N" != "" ] && [ "$USECRDT_N" != "" ] && [ "$USECRDTVP_N" != "" ]&& [ "$MINUSECRDTVP_N" != "" ] && [ "$USEITF_N" != "" ]&& [ "$USEPTY_N" != "" ];then
		./smmuseradd "$USENM_N" "$USECRDT_N" enabled $USEPTY_N $USECRDTVP_N $MINUSECRDTVP_N $USEITF_N
		if [ $? -ne 0 ];then
			echo "Run command ./smmuseradd failed"
			error_quit
		fi
		
		USENM_N=""
		USECRDT_N=""
		USEPTY_N=""
		USECRDTVP_N=""
		USEITF_N=""
		MINUSECRDTVP_N=""
		USER_FINISH_FLAG=2
	fi
}


#********************************************************************
# Name		: for_proc_config_file
# Function  : 循环处理配置文件中的每一行
# Descrip   : 
#             $PROCFILE每一行标准格式为：{smm,NULL,ipmode,static}
#                      注意：逗号后面不能跟空格，否则解析失败  
#             
#********************************************************************
function for_proc_config_file()
{
	while read -r row
	do
		echo $row

		### 1,保存用户名    可能带特殊字符，需要首先判断
		echo $row | grep -q "useradd"
		if [ $? -eq 0 ] ; then
			# 去掉useradd和前面的字符
			USENM_N=${row#*useradd}
			# 去掉','和前面的字符
			USENM_N=${USENM_N#*,}
			# 去掉最后一个}以及后面的字符
			USENM_N=${USENM_N%\}*}
			USER_FINISH_FLAG=0
			try_add_user
			continue
		fi

		### 2,保存用户密码 可能带特殊字符，需要首先判断
		echo $row | grep -q "userpasswd"
		if [ $? -eq 0 ] ; then
			# 去掉userpasswd和前面的字符
			USECRDT_N=${row#*userpasswd}
			# 去掉','和前面的字符
			USECRDT_N=${USECRDT_N#*,}
			# 去掉最后一个}以及后面的字符
			USECRDT_N=${USECRDT_N%\}*}
			USER_FINISH_FLAG=0
			try_add_user
			continue
		fi

		# 过滤空行
		echo $row | grep -q '[\,\{\}a-zA-Z]'
		if [ $? -ne 0 ] ; then
			continue
		fi

		#######################################################
		#####################去除开始的{ ####################
		row=${row#*\{}
		echo $row

		#####################去除末尾的} ####################
		row=${row%\}*}
		echo $row

		#######################################################

		######################解析出每行的参数##############
		location=`echo $row | awk -F "," '{print $1}'`
		echo $location
		targets=`echo $row | awk -F "," '{print $2}'`
		echo $targets
		dataitems=`echo $row | awk -F "," '{print $3}'`
		echo $dataitems
		value1=`echo $row | awk -F "," '{print $4}'`
		echo $value1
		value2=`echo $row | awk -F "," '{print $5}'`
		echo $value2
		value3=`echo $row | awk -F "," '{print $6}'`
		echo $value3
		value4=`echo $row | awk -F "," '{print $7}'`
		echo $value4
		value5=`echo $row | awk -F "," '{print $8}'`
		echo $value5

		###############默认操作dataitem和value不能为空########
		if [ "$dataitems" = "NULL" ] || [ "$dataitems" = "null" ] || [ "$value1" = "NULL" ] || [ "$value1" = "null" ] ; then
			echo_fail "dataitems or value can not be null!"
			echo "dataitems or value can not be null!" >>$SAVEFILE
			return 1
		fi
		
		###############创建用户##############################
		### 保存用户权限
		if [ "$dataitems" = "userpropertychg" ] ;then
			USEPTY_N=$value1
			USER_FINISH_FLAG=0
			try_add_user
			continue
		fi
		### 保存用户密码有效期
		if [ "$dataitems" = "passwdvalidperiod" ] ;then
			USECRDTVP_N=$value1
			MINUSECRDTVP_N=$value2
			USER_FINISH_FLAG=0
			try_add_user
			continue
		fi
		### 保存用户的接口访问权限
		if [ "$dataitems" = "userintfauth" ] ;then
			USEITF_N=$value1
			USER_FINISH_FLAG=0
			try_add_user
			continue
		fi

		######################判断如何执行命令################
		if [ "$location" = "NULL" ] || [ "$location" = "null" ] ; then
			echo "location is null!"
			if [ "$targets" = "NULL" ] || [ "$targets" = "null" ] ; then
				echo "targets is null!"
				cmdserial=" -d  $dataitems -v $value1 $value2 $value3 $value4 $value5"
			else
				cmdserial=" -t $targets -d  $dataitems -v $value1 $value2 $value3 $value4 $value5"
			fi
		else
			echo "location is not null!"
			if [ "$targets" = "NULL" ] || [ "$targets" = "null" ] ; then
				echo "targets is null!"
				cmdserial=" -l $location -d  $dataitems -v $value1 $value2 $value3 $value4 $value5"
			else
				cmdserial=" -l $location -t $targets -d  $dataitems -v $value1 $value2 $value3 $value4 $value5"
			fi
		fi

		#####################执行命令并判断执行结果###############
		error_flag=1
		for i in `seq $REPEAT`
		do
			echo Y | /opt/pme/bin/ipmcset $cmdserial >temp_tmp.txt
			grep -E -i "Success|Succeed|ok" temp_tmp.txt > /dev/null
			if [ $? -eq 0 ] ; then
				echo_success "Run command /opt/pme/bin/ipmcset $cmdserial success"
				echo "Run command /opt/pme/bin/ipmcset $cmdserial success" >>$SAVEFILE
				sleep 2
				error_flag=0
				break
			else
				echo_fail "Run command /opt/pme/bin/ipmcset $cmdserial fail count $i"
				echo "Run command /opt/pme/bin/ipmcset $cmdserial fail count $i" >>$SAVEFILE
				sleep $REPEAT_TIME
				error_flag=1
			fi
		done
		if [ $error_flag -eq 1 ] ; then
			echo_fail "Run command /opt/pme/bin/ipmcset $cmdserial fail total count $i"
			echo "Run command /opt/pme/bin/ipmcset $cmdserial fail total count $i" >>$SAVEFILE
			return 1
		fi
	done < $CONFIGFILE

	# 不允许用户配置缺失, 但是允许不配置或者配置完成
	if [ $USER_FINISH_FLAG -eq 0 ] ;then
		echo "Incomplete user configuration."
		return 1;
	fi

}


############################################################################
#########################程序开始运行#####################################
############################################################################

#先设置配置   最后读取校验
chmod 777 *

#整理配置文件 合二为一
# customset.ini serial.ini" 
cat $1 >$CONFIGFILE
echo "" >>$CONFIGFILE
cat $2 >>$CONFIGFILE


#清空文件
echo "" >$SAVEFILE

###################################################################################
###################新的定制化格式 {smm,NULL,ipmode,static}######################
###################################################################################


######################循环执行定制化配置文件中的每一项########################
for_proc_config_file
if [ $? -eq 0 ] ; then		
	ok_quit
else
	error_quit
fi


