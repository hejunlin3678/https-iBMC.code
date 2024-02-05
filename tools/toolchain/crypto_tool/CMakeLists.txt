#
# Configure file for Cmake project
#
# Author: hucheng (hucheng.hu@huawei.com)
# Date:   2013/07/02

# Description:
# This crypto tool source code dir.
#
#
#==============================================================================

## Customizing Section: adjust the following if necessary.
##=============================================================================
AUX_SOURCE_DIRECTORY(src CRYPTO_SRC)
ADD_EXECUTABLE(crypto_tool_ext4 ${CRYPTO_SRC})
SET(LIB /usr/lib64/libcrypto.so)

INCLUDE_DIRECTORIES(inc)

TARGET_LINK_LIBRARIES(crypto_tool_ext4 ${LIB})

INSTALL(TARGETS crypto_tool_ext4 RUNTIME DESTINATION bin)

