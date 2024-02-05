#!/bin/sh

# This scripts insmod product based drivers.
# author: 
# date: Wed Sep 23 14:04:54 CST 2015
# version: 0.8.0-1

KERNEL_VERSION=`uname -r`

##########insmod usb host drvice start#########
insmod /lib/modules/${KERNEL_VERSION}/kernel/drivers/usb/ehci-hcd_1710.ko
insmod /lib/modules/${KERNEL_VERSION}/kernel/drivers/usb/ohci-hcd_1710.ko
##########insmod usb host drvice end#########

##########insmod cachefs driver#########
insmod /lib/modules/${KERNEL_VERSION}/kernel/fs/fscache/fscache.ko
insmod /lib/modules/${KERNEL_VERSION}/kernel/fs/cachefiles/cachefiles.ko
insmod /lib/modules/${KERNEL_VERSION}/kernel/fs/nfs_common/grace.ko

##########insmod nfs driver#########
insmod /lib/modules/${KERNEL_VERSION}/kernel/lib/oid_registry.ko
insmod /lib/modules/${KERNEL_VERSION}/kernel/net/sunrpc/sunrpc.ko
insmod /lib/modules/${KERNEL_VERSION}/kernel/fs/nfs_common/nfs_acl.ko
insmod /lib/modules/${KERNEL_VERSION}/kernel/fs/lockd/lockd.ko
insmod /lib/modules/${KERNEL_VERSION}/kernel/net/dns_resolver/dns_resolver.ko
insmod /lib/modules/${KERNEL_VERSION}/kernel/fs/nfs/nfs.ko
modprobe nfsv3.ko
modprobe nfsv4.ko

##########insmod nls driver#########
insmod /lib/modules/${KERNEL_VERSION}/kernel/fs/nls/nls_cp437.ko
insmod /lib/modules/${KERNEL_VERSION}/kernel/fs/nls/nls_iso8859-1.ko

##########insmod cifs driver#########
insmod /lib/modules/${KERNEL_VERSION}/kernel/crypto/des_generic.ko
insmod /lib/modules/${KERNEL_VERSION}/kernel/crypto/ecb.ko
insmod /lib/modules/${KERNEL_VERSION}/kernel/crypto/hmac.ko
insmod /lib/modules/${KERNEL_VERSION}/kernel/crypto/md4.ko
insmod /lib/modules/${KERNEL_VERSION}/kernel/crypto/sha256_generic.ko
modprobe cifs.ko

##########insmod mctp driver#########
insmod /opt/pme/lib/modules/ko/mctp_drv.ko

##########insmod isofs driver#########
modprobe isofs.ko

##########insmod dm-mod driver#########
insmod /lib/modules/${KERNEL_VERSION}/kernel/drivers/md/dm-mod.ko

##########insmod iptables driver#######
insmod /lib/modules/${KERNEL_VERSION}/kernel/net/netfilter/x_tables.ko
insmod /lib/modules/${KERNEL_VERSION}/kernel/net/netfilter/xt_limit.ko
##########insmod ip6tables driver#######
insmod /lib/modules/${KERNEL_VERSION}/kernel/net/ipv6/netfilter/ip6_tables.ko
insmod /lib/modules/${KERNEL_VERSION}/kernel/net/ipv6/netfilter/ip6table_filter.ko

#########insmode ipv6##################
insmod /lib/modules/${KERNEL_VERSION}/kernel/lib/crc-ccitt.ko
modprobe ipv6
insmod /lib/modules/${KERNEL_VERSION}/kernel/crypto/cryptd.ko
insmod /lib/modules/${KERNEL_VERSION}/kernel/crypto/crypto_simd.ko
insmod /lib/modules/${KERNEL_VERSION}/kernel/lib/crypto/libaes.ko
insmod /lib/modules/${KERNEL_VERSION}/kernel/arch/arm64/crypto/aes-ce-cipher.ko
insmod /lib/modules/${KERNEL_VERSION}/kernel/arch/arm64/crypto/aes-ce-blk.ko
