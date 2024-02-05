#!/bin/sh

if [ ! -d "/var/run/configfs/usb_gadget" ]; then
  exit 0
fi

echo "" > /var/run/configfs/usb_gadget/g.1/UDC
rm /var/run/configfs/usb_gadget/g.1/os_desc/c.1
rm /var/run/configfs/usb_gadget/g.1/configs/c.1/rndis.usb1
rmdir /var/run/configfs/usb_gadget/g.1/functions/rndis.usb1
rmdir /var/run/configfs/usb_gadget/g.1/configs/c.1
rmdir /var/run/configfs/usb_gadget/g.1
umount /var/run/configfs
rmdir /var/run/configfs