#******************  default info cann't be changed  ****************
TuName=`echo $0 | awk -F "/" '{print $NF}' | awk -F "." '{print $1}'`			# Get script name
SoftID=`echo $TuName | awk -F "_" '{print $1}'`									# Get 0502XXXX softcode
CheckFile="${SoftID}_check.sh"													# Script to verify version
LoadFile="${SoftID}_load.sh"													# Script to load firmware
IniFile="${SoftID}_version.ini"													# File to config version
SaveFile="${SoftID}_save.txt"													# File to save info

#*******************************  end  ******************************


#********************************************************************
# Name			: get_fw_version
# Function      : Get local soft version with FRU Device ID & IC position
# IPMI CMD		: f 0 30 90 08 fru xx 00 10
# FRU_ID		: 00=Mother board	01=Raid card	02=HDD backplane			
# IC_POSITION	: 0=PCB version
#				  1=main firmware version	7=Hardware version
#				  2=CPLD version			8=bootloader version /uboot/bootrom
#				  3=FPGA version			9=hpm image version
#				  4=FRU data version		10=USB flash device firmware version
#				  5=SDR version				11=other firmware version
#				  6=BIOS firmware version	0xFF=All version"
#********************************************************************
get_fw_version()
{
	local output_file=$1
	local fruid=$2
	local ic_offset=$3
	local softname=$4
	local ipmicode=""
	
	local i=0	
	while true
	do
		begin_hex=`printf "%x" $[32*i]`
		end_hex=`printf "%x" $[32+32*i]`
		((i++))
		
		ipmicmd -k "f 0 30 90 08 $fruid $ic_offset $begin_hex $end_hex" smi 0 > temp.txt
		grep -wq "0f 31 00 90 00" temp.txt										# Get target soft version info
		if [ $? -ne 0 ]; then													# The version info is vaild?
			echo "`cat temp.txt` Get local version fail" | tee -a $SaveFile		# Save ipmi return code to upload when fail
			return 1	
		fi	
		
		grep -wq "0f 31 00 90 00 00" temp.txt									# Return code not end, get vaild bytes from the 7th to the end
		if [ $? -eq 0 ]; then
			ipmicode="$ipmicode`cat temp.txt | tr -d "\r\n" | awk -F "0f 31 00 90 00 00 " '{printf $2}'`"
			continue
		fi
		
		grep -wq "0f 31 00 90 00 80" temp.txt									# Return code is finish, get vaild bytes from the 7th to the end
		if [ $? -eq 0 ]; then
			ipmicode="$ipmicode`cat temp.txt | tr -d "\r\n" | awk -F "0f 31 00 90 00 80 " '{printf $2}'`"
			break
		fi
	done
			
	# Conver code return from BMC
	bytes=`echo $ipmicode | awk '{for(i=1;i<=NF;i++) printf("\\\\x%s",$i);}'`	# Change string to char arry
	echo -e $bytes | tr -d "\r\n" > $output_file								# Hex conver to ascii

	if [ ! -s $output_file ] ; then												# Version info include unvisiable nor invaild charactor
		echo "Get local version is null" | tee -a $SaveFile
		return 1
	fi
	Local_FW_Ver=`cat getversion.txt`	
	Expect_FW_Ver=`cat $IniFile | grep "$softname" | awk -F "=" '{print $2}'`
	# ***** Version Compare *****
	if [ "$Expect_FW_Ver" != "$Local_FW_Ver" ] ; then
		echo "$softname expect:$Expect_FW_Ver, actual:$Local_FW_Ver error." | tee -a $SaveFile
		return 1
	else
		echo "$softname:$Local_FW_Ver ok. " | tee -a $SaveFile
		return 0
	fi
	
	return 0
}


#********************************************************************
# Name			: quit
# Function      : Return code
#********************************************************************
error_quit()
{
	echo "[40;31m $TuName ...................... FAIL[0m" 						# Return nor 0 if fail
    exit 1
}

ok_quit()
{
	echo "[40;32m $TuName ...................... PASS[0m" 						# Return 0 if pass
	exit 0
}



#####################################################################
#main
#####################################################################
rm -rf $SaveFile
chmod +x *
printf "\nCopy pme_dft_video_test to /root ok\n"
cp -f pme_dft_video_test.bmp /root

printf "\n******************** Version Test Start **********************\n"
# ***** Check file associated exist?  ***** 
if [ ! -f $IniFile ]; then
	echo "$IniFile not found error. " | tee $SaveFile
	error_quit
fi

# ***** Enable DFT *****
ipmicmd -k "f 0 30 90 20 db 07 00 01" smi 0 > enable_info.txt
grep -wq "0f 31 00 90 00" enable_info.txt
if [ $? -ne 0 ] ; then
	echo "`cat enable_info.txt` Enable DFT error. " | tee $SaveFile
	error_quit
fi

errorflag=0
# "\n********************** Active BMC Version Query ***************************\n " 
get_fw_version "getversion.txt"	0 1 ActiveBMC
if [ $? -ne 0 ] ; then		
	((errorflag++))
fi	

# "\n********************** Backup BMC Version Query ***************************\n " 
get_fw_version "getversion.txt"	0 0c BackupBMC
if [ $? -ne 0 ] ; then		
	((errorflag++))	
fi	

if [ $errorflag -gt 0 ] ; then
	error_quit
fi
ok_quit
