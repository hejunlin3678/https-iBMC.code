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
PROJECT(HUAWEI_PME_BMC C)
if(MAKE_UT_ENABLE)
    CMAKE_MINIMUM_REQUIRED(VERSION 3.3)
else()
    CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
endif()
if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE "Release")
   message(STATUS "No CMAKE_BUILD_TYPE selected, defaulting to ${CMAKE_BUILD_TYPE}")
endif()
message(STATUS "CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE_TOOLCHAIN_FILE is ${CMAKE_TOOLCHAIN_FILE}")

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    add_definitions(-D_DEBUG)
elseif(CMAKE_BUILD_TYPE MATCHES "Release")
    add_definitions(-DNDEBUG)
    add_definitions(-DBMC_RELEASE)
elseif(CMAKE_BUILD_TYPE MATCHES "DevTest")
    add_definitions(-D_DEBUG)
    add_definitions(-DUNIT_TEST=1)
    add_definitions(-DUT=1)
    set(GCOV ON)
    set(IGNORE_LOCAL ON)#在UT工程中不能有静态函数，因为静态函数无法动态打桩
    set(LIB_TYPE_STATIC ON)#静态库，否则无法动态打桩
    set(MAKE_UT_ENABLE ON)
    set(UT ON)
else()
    message(STATUS "unknown CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
endif()


option(FROM_SOURCE "" ON)
if(FROM_SOURCE)
    include(${CMAKE_CURRENT_SOURCE_DIR}/../open_source/libjpeg/jpeg.cmake)
    include(${CMAKE_CURRENT_SOURCE_DIR}/../open_source/Kerberos_5/krb5.cmake)
    include(${CMAKE_CURRENT_SOURCE_DIR}/../open_source/net-snmp/netsnmp.cmake)
    include(${CMAKE_CURRENT_SOURCE_DIR}/../open_source/Cyrus_SASL/sasl.cmake)
    include(${CMAKE_CURRENT_SOURCE_DIR}/../open_source/NTP-The_Network_Time_Protocol/ntp.cmake)
    include(${CMAKE_CURRENT_SOURCE_DIR}/../open_source/OpenLDAP/openldap.cmake)
    include(${CMAKE_CURRENT_SOURCE_DIR}/../open_source/OpenSSH/openssh.cmake)
    include(${CMAKE_CURRENT_SOURCE_DIR}/../open_source/Pam/pam.cmake)
    include(${CMAKE_CURRENT_SOURCE_DIR}/../open_source/curl/curl.cmake)
    include(${CMAKE_CURRENT_SOURCE_DIR}/../open_source/SQLite/sqlite.cmake)
    include(${CMAKE_CURRENT_SOURCE_DIR}/../open_source/json-c/jsonc.cmake)
    include(${CMAKE_CURRENT_SOURCE_DIR}/../open_source/libssh2/libssh2.cmake)
    include(${CMAKE_CURRENT_SOURCE_DIR}/../open_source/lldpd/lldpd.cmake)
    include(${CMAKE_CURRENT_SOURCE_DIR}/../open_source/luajit2/luajit2.cmake)
    include(${CMAKE_CURRENT_SOURCE_DIR}/../open_source/nginx/nginx.cmake)
    include(${CMAKE_CURRENT_SOURCE_DIR}/../open_source/syslog-ng/syslog.cmake)
    include(${CMAKE_CURRENT_SOURCE_DIR}/../open_source/Editline_library-libedit/libedit.cmake)
    include(${CMAKE_CURRENT_SOURCE_DIR}/../vendor/StoreLib/storelib.cmake)
    include(${CMAKE_CURRENT_SOURCE_DIR}/../vendor/StoreLib3108/storelib.cmake)
    include(${CMAKE_CURRENT_SOURCE_DIR}/../vendor/StoreLibIR3/storelibir3.cmake)
    include(${CMAKE_CURRENT_SOURCE_DIR}/../vendor/StoreLibIT/storelibit.cmake)
endif()
## global setup
enable_language(CXX)
# turn on this, you can use else()
SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS ON)

SET(CMAKE_C_FLAGS "-fPIC -fPIE -pie -Wall -Wstrict-prototypes -Wpointer-arith -Wformat=2 -Wformat-security -Wformat-nonliteral -Wno-format-y2k -Wcast-qual -Wcast-align -fexceptions ${COMPILE_DEFINITIONS} -fgnu89-inline ")
SET(CMAKE_CXX_FLAGS "-fPIC -fPIE -pie -Wall -Wpointer-arith -Wformat=2 -Wformat-security -Wformat-nonliteral -Wno-format-y2k -Wcast-qual -Wcast-align -DHAVE_PTHREAD_H -DHAVE_NETINET_IN_H -fexceptions ${COMPILE_DEFINITIONS} ")
SET(CMAKE_EXE_LINKER_FLAGS " -Wl,-z,relro,-z,now,-z,noexecstack ")
if(MAKE_FUZZ_ENABLE)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fsanitize=leak -fsanitize=undefined -fsanitize-recover=address,all -fno-omit-frame-pointer -fno-stack-protector")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=leak -fsanitize=undefined -fsanitize-recover=address,all -fno-omit-frame-pointer -fno-stack-protector -D_GLIBCXX_USE_CXX11_ABI=0")
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lasan")
else()
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pipe")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe")            
endif()
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_EXE_LINKER_FLAGS}")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_EXE_LINKER_FLAGS}")

if(CMAKE_CROSSCOMPILING)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fstack-protector-strong")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstack-protector-strong")
    SET(CS4343_C_FLAFS "-fPIC -fPIE -pie -Wall -pipe -fexceptions ${COMPILE_DEFINITIONS} -Wl,-z,relro,-z,now,-z,noexecstack -fstack-protector-strong")
    SET(CS4343_CXX_FLAGS "-fPIC -fPIE -pie -Wall -pipe -DHAVE_PTHREAD_H -DHAVE_NETINET_IN_H -fexceptions ${COMPILE_DEFINITIONS} -Wl,-z,relro,-z,now,-z,noexecstack -fstack-protector-strong")
    SET(SNMP_C_FLAFS "-Wl,-z,relro,-z,now,-z,noexecstack -fstack-protector-strong -pipe -Wall -Wmissing-prototypes -Wmissing-declarations -Wstrict-prototypes -Wpointer-arith -Wformat=2 -Wformat-security -Wformat-nonliteral -Wno-format-y2k -Wcast-qual -Wcast-align -D_GNU_SOURCE -D_REENTRANT -fexceptions -DBD_LITTLE_ENDIAN -DSQLITE_THREADSAFE=1 ")
    ADD_DEFINITIONS( -DARM_PLATFORM)
    ADD_DEFINITIONS( -DSECUREC_SUPPORT_FORMAT_WARNING=1)
    ADD_DEFINITIONS( -DPME_SUPPORT_FORMAT_WARNING)
    INCLUDE_DIRECTORIES(${SDK_INC})
endif()
if (CMAKE_NO_PME)
    ADD_DEFINITIONS( -DNO_PME_BUILD)
endif()
ADD_DEFINITIONS( -D_GNU_SOURCE)
ADD_DEFINITIONS( -D_REENTRANT)
ADD_DEFINITIONS( -DRPC_THREAD_POOL_COUNT=5)
ADD_DEFINITIONS( -D__PME_LOCALHOST__)
ADD_DEFINITIONS( -DBD_LITTLE_ENDIAN)
ADD_DEFINITIONS( -DCMSCBB_SUPPORT_RSAPSS)

# compile MACRO define
# debug ctrl
if(MAKE_UT_ENABLE)
    SET(LIB_TYPE_STATIC ON)#静态库，否则无法动态打桩
    SET(IGNORE_LOCAL ON)#在UT工程中不能有静态函数，因为静态函数无法动态打桩
    SET(UT ON)
    SET(CMAKE_C_FLAGS "-O0 ${CMAKE_C_FLAGS} -std=c99 -DDT_COMPILE_GCC")
    SET(CMAKE_CXX_FLAGS "-O0 ${CMAKE_CXX_FLAGS} -DDT_COMPILE_GCC")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-builtin-macro-redefined -D'__FILE__=\"$(subst $(realpath ${CMAKE_SOURCE_DIR})/,,$(abspath $<))\"'")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-builtin-macro-redefined -D'__FILE__=\"$(subst $(realpath ${CMAKE_SOURCE_DIR})/,,$(abspath $<))\"'")
    # 如果为64位编译系统编译32位程序，则加上-m32选项
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        MESSAGE(STATUS "++++build dtcenter 32bit library by 64bit compiler, add -m32 for gcc flags")
        add_definitions(-m32)
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -m32")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -m32")
    endif (CMAKE_SIZEOF_VOID_P EQUAL 8)
    
    MESSAGE(STATUS "using pme_framework in ${G_DT_FRAMEWORK_BIN_PATH} for llt")
    ADD_DEFINITIONS(-DG_DT_FRAMEWORK_BIN_PATH=\"${G_DT_FRAMEWORK_BIN_PATH}\") # need \"

    MESSAGE(STATUS "using storage sml lsi in ${LSI_SML_LIB_PATH} for llt")
    ADD_DEFINITIONS(-DLSI_SML_LIB_PATH=\"${LSI_SML_LIB_PATH}\") # need \"

    MESSAGE(STATUS "using xmls in ${G_DT_XML_BOARD_DIR} for llt")
    ADD_DEFINITIONS(-DG_DT_XML_BOARD_DIR=\"${G_DT_XML_BOARD_DIR}\")

    MESSAGE(STATUS "take the dir as temproot dir when running tests")
    ADD_DEFINITIONS(-DG_DT_RUNTIME_TEMPROOT_DIR=\"${G_DT_RUNTIME_TEMPROOT_DIR}\")

else()
    SET(CMAKE_C_FLAGS "-O2 ${CMAKE_C_FLAGS} -std=c99")
    SET(CMAKE_CXX_FLAGS "-O2 ${CMAKE_CXX_FLAGS}")
endif()

if(IGNORE_LOCAL)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLOCAL")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLOCAL")
else()
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wmissing-prototypes -Wmissing-declarations -DLOCAL=static")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wmissing-declarations -DLOCAL=static")
endif()

#用于区分合作伙伴模式出包版本
if(PARTNER_COMPILE_VERSION STREQUAL "online")
	ADD_DEFINITIONS(-DCMS_ONLINE_SIGN_FLAG)
elseif(PARTNER_COMPILE_VERSION STREQUAL "repair")
	ADD_DEFINITIONS(-DCMS_REPAIR_SIGN_FLAG)
elseif(PARTNER_COMPILE_VERSION STREQUAL "noverify")
	ADD_DEFINITIONS(-DCMS_NOVERIFY_SIGN_FLAG)
endif()

if(GCOV)
    if(UT)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
    else()
        message(STATUS "enable arm_gcov for black-box test")
        set(ARM_GCOV ON)
        add_compile_definitions(ARM_GCOV=1)
    endif()
endif()
if(ARM64_HI1711_ENABLED)
    # 1711安全编译选项开启非安全函数替换检测，分布式存储增加，AR20201119130337
    ADD_DEFINITIONS( -D_FORTIFY_SOURCE=2)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wno-unused-result")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wno-unused-result")
endif()

if(DFT_ENABLED)
    ADD_DEFINITIONS(-DDFT_ENABLED)
endif()

#用于TaiShan2480v2Pro机型做PID调速特殊处理,注意要放在加载src目录之前
if(PIDSPEED_REGULATION)
    ADD_DEFINITIONS(-DPIDSPEED_REGULATION)
endif()
ADD_SUBDIRECTORY(src)
# ADD_SUBDIRECTORY(infected/libs/file_hook)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
MESSAGE(STATUS "This is BINARY dir ${HUAWEI_PME_BMC_BINARY_DIR}")
MESSAGE(STATUS "This is SOURCE dir ${HUAWEI_PME_BMC_SOURCE_DIR}")

