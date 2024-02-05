#!/bin/sh

if [ ! -f /etc/ssh/ssh_config ]; then
	touch /etc/ssh/ssh_config
	echo "Host *" >> /etc/ssh/ssh_config
	echo "    StrictHostKeyChecking no" >> /etc/ssh/ssh_config
	echo "    UserKnownHostsFile /dev/null" >> /etc/ssh/ssh_config
fi

chmod 710 /etc/ssh/
chmod 640 /etc/ssh/ssh_config
chmod 666 /dev/tty  >/dev/null 2>&1
