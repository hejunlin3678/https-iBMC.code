#
# Configure file for Cmake project
#
# Author: hucheng (hucheng.hu@huawei.com)
# Date:   2013/07/03

# Description:
# This is HUAWEI Server PME BMC project top dir.
#
#
#==============================================================================


ADD_SUBDIRECTORY(apps)
#ADD_SUBDIRECTORY(user_interface)
ADD_SUBDIRECTORY(rootfs)
if(ARM64_HI1711_ENABLED)
  INSTALL(DIRECTORY lib_open_source/ DESTINATION usr/lib64)
else()
  INSTALL(DIRECTORY lib_open_source/ DESTINATION usr/lib)
endif()
