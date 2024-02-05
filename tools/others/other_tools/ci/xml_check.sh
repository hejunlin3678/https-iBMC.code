#!/bin/bash
# Description：xml check
# Copyright © Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.


exit_with_create_flag()
{
	flag_file="/tmp/flag_$1"

	if [ ! -f ${flag_file} ]; then
		echo > ${flag_file}
	fi

	if [ "$2" != "" ]; then
		exit $2
	fi	
}

check_task_flag()
{
	flag_file="/tmp/flag_$1"
	while [ ! -f ${flag_file} ];
	do		
		sleep 1
	done	
	
	rm $flag_file
}

xml_check_for_board()
{
	python localbuild.py xml $1
	exit_with_create_flag $1
}

support_board="rh1288v3 rh2285v3 rh2288v3 rh8100v3 ch220v3
			   ch121v3 cx710 cx712 cx220 rh2288a ch140v3
			   ch242v3 rh1288a x6800 MM810 TaiShan2280_5280 xr320 PangeaV6_Arctic PangeaV6_Atlantic PangeaV6_Atlantic_Smm PangeaV6_Pacific PAC1220V6
			   ENC0210V6"
			   
for one_board in ${support_board[*]};
do
	xml_check_for_board $one_board &
done

for one_board in ${support_board[*]};
do
	check_task_flag $one_board
done

echo "all xml check is done"	
