#!/bin/sh

#check and restore xinetd  files
remove_space()
{
	local covert_string="$1"
	covert_string=`echo "$covert_string" | sed 's/^[ ]*//g' | sed 's/[ ]*$//g'`
	echo "$covert_string"
}

check_config_file()
{
	if [ ! -f $1 ]; then
		echo "file not exist"
		return 1
	fi

	last_line=`tail -1 $1`	
	last_line=$(remove_space "$last_line")
	
	if [ "$last_line" = "$2" ]; then
	    echo "file not corrupt"
		return 0
	else
		return 1
	fi
}

#  check services file  
check_services_file()
{
	cat $1 | grep "^ftp[ \t]\{1,\}[0-9]\{1,\}/tcp"
	if [ $? != 0 ]; then
		return 1
	fi
	
	cat $1 | grep "^ssh[ \t]\{1,\}[0-9]\{1,\}/tcp"	
	if [ $? != 0 ]; then
		return 1
	fi
		
	return 0
}

SERVICES_PORT_FILE=/data/etc/services
check_services_file $SERVICES_PORT_FILE
if [ $? != 0 ] ; then
	echo "copy $SERVICES_PORT_FILE"
	cp -rf /etc/services_bak $SERVICES_PORT_FILE
fi

#check factory recover point
if [ ! -f /opt/pme/save/.factory_recover_point.tar.gz ]; then
    mkdir /opt/pme/save -p
    chmod 755 /opt/pme/save
    cp /opt/pme/conf/data_dir_sync2/3/.factory_recover_point.tar.gz /opt/pme/save/ -f
fi

exit 0

