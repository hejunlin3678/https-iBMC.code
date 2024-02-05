#!/bin/sh
if [ $# == 2 ] ; then
	source ./common.sh 
	init_common $1 $2
	cp ../conver .
	cp ../defaultserverport.ini .	
fi

#清除日志文件
clear_log_file()
{	
	run_cmd "f 0 30 93 DB 07 00 08 02 aa" "0f 31 00 93 00 db 07 00"
   
	if [ $? != 0 ] ; then		
	    echo_fail "Clear log file fail!"
	    echo "Clear log file fail!" >>$SAVEFILE
	    error_quit
	else 
	    echo_success "Clear log file success!"
	    echo "Clear log file success!" >>$SAVEFILE
	    return 0
	fi
}

#清除录像截屏文件
clear_screen_video_file()
{
	run_cmd "f 0 30 93 DB 07 00 08 03 aa" "0f 31 00 93 00 db 07 00"
   
	if [ $? != 0 ] ; then		
	    echo_fail "Clear screen video file fail!"
	    echo "Clear screen video file fail!" >>$SAVEFILE
	    error_quit
	else 
	    echo_success "Clear screen video file success!"
	    echo "Clear screen video file success!" >>$SAVEFILE
	    return 0
	fi
}

#清除功率统计数据
clear_power_statistic_data()
{
	run_cmd "f 0 30 93 DB 07 00 12 00" "0f 31 00 93 00 db 07 00"
   
	if [ $? != 0 ] ; then		
	    echo_fail "Clear power statistic data fail!"
	    echo "Clear power statistic data fail!" >>$SAVEFILE
	    error_quit
	else 
	    echo_success "Clear power statistic data success!"
	    echo "Clear power statistic data success!" >>$SAVEFILE
	    return 0
	fi
}

#清除历史功率数据
clear_history_power_data()
{
	run_cmd "f 0 30 93 DB 07 00 12 01" "0f 31 00 93 00 db 07 00"
   
	if [ $? != 0 ] ; then		
	    echo_fail "Clear history power data fail!"
	    echo "Clear history power data fail!" >>$SAVEFILE
	    error_quit
	else 
	    echo_success "Clear history power data success!"
	    echo "Clear history power data success!" >>$SAVEFILE
	    return 0
	fi
}

main()
{
	#EQUIPMENT_SEPARATION_SUPPORT_STATE为0表示不支持装备分离，保持原有流程，1表示升级白牌包清除日志
	if [ "$EQUIPMENT_SEPARATION_SUPPORT_STATE" == 0 ];then
		clear_power_statistic_data
		sleep 1

		clear_history_power_data
		sleep 1

		clear_screen_video_file
		sleep 1

		clear_log_file
	else
		clear_power_statistic_data
		sleep 1

		clear_history_power_data
		sleep 1

		# start wbd-clearlog-image upgrade
		hpm_upgrade wbd-clearlog-image.hpm
	fi
}

main

