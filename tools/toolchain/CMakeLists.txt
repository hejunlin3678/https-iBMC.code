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

## Customizing Section: adjust the following if necessary.
##=============================================================================
PROJECT(HUAWEI_BMC_TOOLCHAIN C)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

## global setup
#enable_language(CXX)
# turn on this, you can use else()
SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS ON)

# don't add -g and remove -O3 FLAG, for security.
SET(CMAKE_C_FLAGS "-pipe -Wall -Wstrict-prototypes -Wpointer-arith -Wformat=2 -Wformat-security -Wformat-nonliteral -Wno-format-y2k -Wcast-qual -Wcast-align -Wno-strict-aliasing -Wno-unused-value -fexceptions -fstack-protector ${COMPILE_DEFINITIONS} ")
#SET(CMAKE_CXX_FLAGS "-pipe -Wall -g -Wpointer-arith -Wformat=2 -Wformat-security -Wformat-nonliteral -Wno-format-y2k -Wcast-qual -Wcast-align -Wno-strict-aliasing -Wno-unused-value -DHAVE_PTHREAD_H -DHAVE_NETINET_IN_H -fexceptions -fstack-protector ${COMPILE_DEFINITIONS} ")


if(CMAKE_CROSSCOMPILING)
    ADD_DEFINITIONS( -DARM_PLATFORM)
    #ADD_DEFINITIONS( -D_USE_FIX_MEMORY)
else()
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -O3")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
endif()
ADD_DEFINITIONS( -D_GNU_SOURCE)
ADD_DEFINITIONS( -D_REENTRANT)
#ADD_DEFINITIONS( -DRPC_THREAD_POOL_COUNT=5)
#ADD_DEFINITIONS( -D__PME_LOCALHOST__)
#ADD_DEFINITIONS( -DBD_LITTLE_ENDIAN)
#ADD_DEFINITIONS( -DPME_DEBUG=1)

# compile MACRO define
# debug ctrl
#if(UT)
#    ADD_DEFINITIONS( -DUNIT_TEST=1)
#	SET(LIB_TYPE_STATIC ON)#静态库，否则无法动弹打桩
#	SET(GCOV ONX)
#	SET(IGNORE_LOCAL ON)#在UT工程中不能有静态函数，因为静态函数无法动弹打桩
#	MESSAGE(STATUS "IGNORE_LOCAL = "${IGNORE_LOCAL})
#endif()

if(IGNORE_LOCAL)
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLOCAL")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLOCAL")
else()
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wmissing-prototypes -Wmissing-declarations -DLOCAL=static")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wmissing-declarations -DLOCAL=static")
endif()

#if(GCOV)
#	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
#	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
#endif()

ADD_SUBDIRECTORY(crypto_tool)

MESSAGE(STATUS "This is BINARY dir "${HUAWEI_BMC_TOOLCHAINBINARY_DIR})
MESSAGE(STATUS "This is SOURCE dir "${HUAWEI_BMC_TOOLCHAINSOURCE_DIR})

