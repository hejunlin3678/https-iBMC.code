#!/bin/sh
/sbin/udhcpc -i usb0 -s /usr/share/udhcpc/default.script -p /var/run/dhclient_usb.pid >/dev/null 2>&1
exit 0