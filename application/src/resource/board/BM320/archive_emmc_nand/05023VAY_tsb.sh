#!/bin/bash

# -----------------------------------------------------------------------------
# Name          : error_quit
# Function      : Failed exit
# -----------------------------------------------------------------------------
error_quit()
{
	echo "test..........................fail"
	echo "fail" >>save.txt
	echo "fail" >result.txt
	exit 1
}

# -----------------------------------------------------------------------------
# Name           : ok _quit
# Function      : success exit
# -----------------------------------------------------------------------------
ok_quit()
{
	echo "test..........................pass"
	echo "pass" >>save.txt
	echo "pass" >result.txt
	exit 0
}

#****************************************************************
# remove String blanks
#****************************************************************
string_blank_remove()
{
	local char="$1"
	local _out_file="$2"
	rm -f ${_out_file}

	echo $char | sed 's/ //g' >> ${_out_file}

	return 0
}

# -----------------------------------------------------------------------------
# Clear license before test.
#
# Usage
#    clear_license_by_bmc
#
# Options and/or Parameters
#    NA.
#
# Exit Status
#    0 : Clear license pass.
#    1 : Clear license fail.
#
# Remarks
#    NA
# -----------------------------------------------------------------------------
get_request_lic_by_bmc()
{
	for i in {1..10}
	do
		ipmitool raw 0x30 0x90 0xdb 0x07 0x00 0x68 0x00 0x00 0x00 0x00 >start_request_ret.txt
		### 00 means clear pass , d5 means license not load
		grep -E "db 07 00 00 00" start_request_ret.txt
		if [ $? -ne 0 ]; then
			echo "start request fail, send command: 0x30 0x90 0xdb 0x07 0x00 0x68 0x00 0x00 0x00 0x00, return value: $(cat start_request_ret.txt | tr -d "\n\r")" | tee save.txt
			sleep 5
			continue
		fi
		break
	done

	# 判断是否超时
	if [ $i -eq 10 ]
	then
		echo "Command timed out after 10 retries."
		return 1
	fi

	local loop_count=0
	local read_offset=0
	local read_len=""
	local return_tmp=""
	local over_flg=""
	local return_data=""
	local data_tmp=""
	local request_lic_success=0

	while true
		do
		if [ $read_offset -eq 0 ] ; then

			sleep 5
		fi

		ipmitool raw 0x30 0x90 0xdb 0x07 0x00 0x68 0x01 0x00 0x${read_offset} 0x00 >request_lic_ret.txt
		grep -wq "db 07 00" request_lic_ret.txt
		if [ $? -eq 0 ] ; then
			return_tmp=$(cat request_lic_ret.txt)
			over_flg=$(echo ${return_tmp} | awk '{print $4}')
			read_len=$(echo ${return_tmp} | awk '{print $5}')
			data_tmp=$(echo ${return_tmp} | awk -F "db 07 00 ${over_flg} ${read_len} " '{print $2}')
			return_data="${return_data} ${data_tmp}"
			if [ "$((0x$over_flg & 0x1))" = "1" ] ; then
				let offset_hex=16#$read_offset+16#$read_len
				read_offset=`printf "%x" $offset_hex`
			#### the highest bit "1" means data is over
			elif [ "$((0x$over_flg & 0xff))" = "0" ] ; then
				break
			fi

		else
			if [ $loop_count -gt 5 ]; then
				request_lic_success=1
				break
			fi
		fi
		loop_count=$(($loop_count+1))
	done

	if [ $request_lic_success -ne 0 ]; then
		echo "get request lic fail, return value: $(cat request_lic_ret.txt | tr -d "\n\r")" | tee save.txt
		return 1
	fi
	string_blank_remove "${return_data}" request_lic.txt

	return 0
}

send_request_lic()
{
	local request_lic=$(cat request_lic.txt)
	local send_request="{\"license_req\":\""${request_lic}"\"}"

	echo $send_request

	curl -H "Content-type: application/json" -X POST -d $send_request $local_create_server >license_result.txt
    grep -E "200" license_result.txt
	if [ $? -ne 0 ]; then
		echo "send_request_lic fail" | tee save.txt
		return 1
	fi

	return 0
}

verify_actice_code()
{
	active_code=$(cat license_result.txt | awk -F "\"activecode\":\"" '{print $2}' | awk -F "\"" '{print $1}')
    local active_code_part1=${active_code: 0: 372}
    local active_code_part2=${active_code: 372}
	active_code_part1=$(echo ${active_code_part1} | sed 's/../&\ 0x/g')
	active_code_part2=$(echo ${active_code_part2} | sed 's/../&\ 0x/g')
    active_code_part1=0x${active_code_part1: 0: -3}
    active_code_part2=0x${active_code_part2: 0: -3}

	ipmitool raw 0x30 0x90 0xdb 0x07 0x00 0x68 0x02 0x80 0x00 0xba $active_code_part1 >send_activecode_ret.txt
	grep -E "db 07 00 00 00" send_activecode_ret.txt
	if [ $? -ne 0 ]; then
		echo "send verify active code part1 fail, return value: $(cat send_activecode_ret.txt | tr -d "\n\r")" | tee save.txt
		return 1
	fi

	ipmitool raw 0x30 0x90 0xdb 0x07 0x00 0x68 0x02 0x00 0xba 0x4e $active_code_part2 >send_activecode_ret.txt
	grep -E "db 07 00 00 00" send_activecode_ret.txt
	if [ $? -ne 0 ]; then
		echo "send verify active code part2 fail, return value: $(cat send_activecode_ret.txt | tr -d "\n\r")" | tee save.txt
		return 1
	fi

	sleep 10

	ipmitool raw 0x30 0x90 0xdb 0x07 0x00 0x68 0x03 0x00 0x00 0x00 > verify_result.txt
	grep -E "db 07 00 00 01 03" verify_result.txt
	if [ $? -ne 0 ]; then
		echo "get active result from bmc fail, 0x3 return value: $(cat verify_result.txt | tr -d "\n\r")" | tee save.txt
		return 1
	fi
    return 0
}

sendback_active_result()
{
	local send_request="{\"activecode\":\""${active_code}"\",\"resultcode\":200,\"resultmsg\":0}"

	echo $send_request

	curl -H "Content-type: application/json" -X POST -d $send_request $local_send_server >active_result.txt
    grep -E "200" active_result.txt
	if [ $? -ne 0 ]; then
		echo "sendback_active_result fail" | tee save.txt
		return 1
	fi

	return 0
}

# -----------------------------------------------------------------------------
# Initial for test.
#
# Usage
#    initial
#
# Options and/or Parameters
#    NA.
#
# Exit Status
#    0 : Initial test pass.
#    1 : Initial test fail.
#
# Remarks
#    NA
# -----------------------------------------------------------------------------
initial()
{
	licensecode=${1}
	barcode=${2}
	server_addr=${3}
	local_import_server=http://$server_addr/api/license/import
	local_create_server=http://$server_addr/api/license/create
    local_send_server=http://$server_addr/api/license/result

	local send_request="{\"licensecode\":\""${licensecode}"\",\"barcode\":\""${barcode}"\"}"

	echo $send_request

	curl -H "Content-type: application/json" -X POST -d $send_request $local_import_server >init_result.txt
    grep -E "200" init_result.txt
	if [ $? -ne 0 ]; then
		echo "send_import_code fail" | tee save.txt
		return 1
	fi

	return 0
}

# -----------------------------------------------------------------------------
# Main test function.
#
# Usage
#    main
#
# Options and/or Parameters
#    NA.
#
# Exit Status
#    0 : NA.
#    1 : NA.
#
# Remarks
#    NA
# -----------------------------------------------------------------------------
main()
{
	### check input
	if [ $# -ne 3 ] ; then
		return 1
	fi

	### initial test environment
	initial ${1} ${2} ${3}
	if [ $? -ne 0 ] ; then
		return 1
	fi

	### start get request license
	get_request_lic_by_bmc
	if [ $? -ne 0 ] ; then
		return 1
	fi

	### send request license
	send_request_lic
    if [ $? -ne 0 ] ; then
		return 1
	fi

	### verify_actice_code
	verify_actice_code
    if [ $? -ne 0 ] ; then
		return 1
	fi
    sendback_active_result
    if [ $? -ne 0 ] ; then
		return 1
	fi
    return 0
}

main $1 $2 $3
if [ $? -ne 0 ]; then
	error_quit
fi
ok_quit