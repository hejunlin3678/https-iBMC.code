#!/bin/sh

/usr/sbin/logrotate /etc/logrotate.conf
EXITVALUE=$?
if [ $EXITVALUE != 0 ]; then
    rm -rf /var/lib/logrotate/logrotate.status
    mkdir -p /var/lib/logrotate
fi
chmod 750 /var/lib/logrotate > /dev/null
exit 0