echo "Remove drivers"
/sbin/rmmod cdev_veth_drv.ko >/dev/null 2>&1
/sbin/rmmod bmc_veth_drv.ko >/dev/null 2>&1
/sbin/rmmod bmc_cdev_drv.ko >/dev/null 2>&1
/sbin/rmmod bmc_edma_drv.ko >/dev/null 2>&1
/sbin/rmmod bmc_hisport_drv.ko >/dev/null 2>&1