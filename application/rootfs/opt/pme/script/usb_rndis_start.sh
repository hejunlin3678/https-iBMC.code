#!/bin/sh

if [ ! -d "/var/run/configfs/" ];then
  mkdir /var/run/configfs
fi

mount -t configfs none /var/run/configfs

cd /var/run/configfs/usb_gadget
mkdir g.1
cd g.1
echo 0x12d1 > idVendor
echo 0x0006 > idProduct
echo 0x0100 > bcdDevice
echo 0x0200 > bcdUSB
echo 0xef > bDeviceClass
echo 0x04 > bDeviceSubClass
echo 0x01 > bDeviceProtocol

mkdir configs/c.1

echo "1" > os_desc/use
echo 0xcd > os_desc/b_vendor_code
echo "MSFT100" > os_desc/qw_sign

mkdir functions/rndis.usb1
echo "RNDIS" > functions/rndis.usb1/os_desc/interface.rndis/compatible_id
echo "5162001" > functions/rndis.usb1/os_desc/interface.rndis/sub_compatible_id

ln -s functions/rndis.usb1 configs/c.1
ln -s configs/c.1 os_desc

case "$1" in
  usb2-2)
    echo "8a00000.dwc3" > UDC
    ;;
  usb3-0)
    echo "cc00000.dwc3" > UDC
    ;;
  *)
    echo "Usage: /opt/pme/script/usb_rndis_start.sh {usb2-2|usb3-0}"
    exit 1
esac