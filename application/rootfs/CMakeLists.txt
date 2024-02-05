#
# Configure file for Cmake project
#
# Author: hucheng (hucheng.hu@huawei.com)
# Date:   2013/07/02

# Description:
# This pme bmc rootfs top dir.
#
#
#==============================================================================

## Customizing Section: adjust the following if necessary.
##=============================================================================


INSTALL(DIRECTORY ./ DESTINATION ./)
INSTALL(DIRECTORY data/home/ DESTINATION data/home)
INSTALL(DIRECTORY data/ DESTINATION data)
#INSTALL(DIRECTORY etc/ DESTINATION etc)
INSTALL(DIRECTORY opt/ DESTINATION opt)
INSTALL(DIRECTORY usr/ DESTINATION usr)
INSTALL(DIRECTORY lib/ DESTINATION lib)
INSTALL(DIRECTORY sbin/ DESTINATION sbin)
INSTALL(DIRECTORY bin/ DESTINATION bin)
