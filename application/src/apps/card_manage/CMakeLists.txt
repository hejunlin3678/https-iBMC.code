#
# Configure file for Cmake project
#
# Author: hucheng (hucheng.hu@huawei.com)
# Date:   2013/07/02

# Description:
# This pme bmc pcie_card source code dir.
#
#
#==============================================================================

## Customizing Section: adjust the following if necessary.
##=============================================================================

AUX_SOURCE_DIRECTORY(src CARD_MANAGE_SRC)
list(SORT CARD_MANAGE_SRC)

if(UT)
    ADD_LIBRARY(card_manage STATIC ${CARD_MANAGE_SRC})
else()
    ADD_EXECUTABLE(card_manage ${CARD_MANAGE_SRC})
endif()

if(FROM_SOURCE)
    add_dependencies(card_manage libssh2 jsonc sqlite)
endif()

INCLUDE_DIRECTORIES(inc)
INCLUDE_DIRECTORIES(${HUAWEI_PME_BMC_SOURCE_DIR}/src/include)
INCLUDE_DIRECTORIES(${HUAWEI_PME_BMC_SOURCE_DIR}/src/include/open_source/net-snmp)
INCLUDE_DIRECTORIES(${HUAWEI_PME_BMC_SOURCE_DIR}/src/libs/arithmetic/inc)
INCLUDE_DIRECTORIES(${HUAWEI_PME_BMC_SOURCE_DIR}/src/libs/dalib/inc)
INCLUDE_DIRECTORIES(${HUAWEI_PME_BMC_SOURCE_DIR}/src/rootfs/usr/lib)
INCLUDE_DIRECTORIES(${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source)

# open source libs
if(CMAKE_CROSSCOMPILING)
    INCLUDE_DIRECTORIES(${HUAWEI_PME_BMC_SOURCE_DIR}/src/include/open_source)
    INCLUDE_DIRECTORIES(${SDK_INC}/glib-2.0)
    INCLUDE_DIRECTORIES(${SDK_LIB}/glib-2.0/include)
    INCLUDE_DIRECTORIES(${SDK_INC} ${SDK_INC_PME})
    INCLUDE_DIRECTORIES(${HUAWEI_PME_BMC_SOURCE_DIR}/src/include/open_source/json)
else()
    # x86 version
    INCLUDE_DIRECTORIES(${SDK_INC_PME})
    INCLUDE_DIRECTORIES(${THIRD_INC}/glib-2.0)
    INCLUDE_DIRECTORIES(${THIRD_LIB}/glib-2.0/include)	
    INCLUDE_DIRECTORIES(${THIRD_INC}/json)
    INCLUDE_DIRECTORIES(${THIRD_INC})
endif()

# open source libs dir
TARGET_LINK_LIBRARIES(card_manage ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libnetsnmpagent.so.40.1.0)
TARGET_LINK_LIBRARIES(card_manage ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libnetsnmphelpers.so.40.1.0) 
TARGET_LINK_LIBRARIES(card_manage ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libnetsnmpmibs.so.40.1.0) 
TARGET_LINK_LIBRARIES(card_manage ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libnetsnmp.so.40.1.0) 
TARGET_LINK_LIBRARIES(card_manage ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libnetsnmptrapd.so.40.1.0) 
TARGET_LINK_LIBRARIES(card_manage ${GLIB2})
TARGET_LINK_LIBRARIES(card_manage ${HUAWEI_PME_BMC_SOURCE_DIR}/src/rootfs/usr/lib/libarithmetic.so.0.9.0)
TARGET_LINK_LIBRARIES(card_manage ${HUAWEI_PME_BMC_SOURCE_DIR}/src/rootfs/usr/lib/libdalib.so.0.9.0)
TARGET_LINK_LIBRARIES(card_manage ${COMMON})
TARGET_LINK_LIBRARIES(card_manage ${DFLIB})
TARGET_LINK_LIBRARIES(card_manage ${SECURE})

TARGET_LINK_LIBRARIES(card_manage ${XML_PARSER})
TARGET_LINK_LIBRARIES(card_manage ${HASLIB})
TARGET_LINK_LIBRARIES(card_manage ${DFLBASE})
TARGET_LINK_LIBRARIES(card_manage ${THRIFT})
TARGET_LINK_LIBRARIES(card_manage ${MUPARSER})
TARGET_LINK_LIBRARIES(card_manage ${DRIVERS})

TARGET_LINK_LIBRARIES(card_manage ${PFLASH})
TARGET_LINK_LIBRARIES(card_manage ${DFRPC})
TARGET_LINK_LIBRARIES(card_manage ${MAINT_DEBUG_COM})
TARGET_LINK_LIBRARIES(card_manage ${MAINT_DEBUG_MSG})
TARGET_LINK_LIBRARIES(card_manage ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libssh2.so.1.0.1) 

TARGET_LINK_LIBRARIES(card_manage ${PERSISTANCE})
TARGET_LINK_LIBRARIES(card_manage ${FILE_MANAGE})
TARGET_LINK_LIBRARIES(card_manage ${MAINT_DEBUG})
TARGET_LINK_LIBRARIES(card_manage ${IPMI_COMMON})
TARGET_LINK_LIBRARIES(card_manage ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libjson-c.so.5.2.0) 
TARGET_LINK_LIBRARIES(card_manage ${DRIVERS})
TARGET_LINK_LIBRARIES(card_manage ${HUAWEI_PME_BMC_SOURCE_DIR}/src/rootfs/usr/lib/libmodbus.so.0.9.0)
TARGET_LINK_LIBRARIES(card_manage ${HUAWEI_PME_BMC_SOURCE_DIR}/src/rootfs/usr/lib/librecord_wave.so.0.9.0)
TARGET_LINK_LIBRARIES(card_manage ${HUAWEI_PME_BMC_SOURCE_DIR}/src/rootfs/usr/lib/libpcie_mgmt.so.0.9.0)
TARGET_LINK_LIBRARIES(card_manage m)
TARGET_LINK_LIBRARIES(card_manage ${HUAWEI_PME_BMC_SOURCE_DIR}/src/rootfs/usr/lib/libsmm_public.so.0.9.0)
TARGET_LINK_LIBRARIES(card_manage ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libpcre.so.1.2.13)
TARGET_LINK_LIBRARIES(card_manage ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libpcrecpp.so.0.0.2) 
TARGET_LINK_LIBRARIES(card_manage ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libpcreposix.so.0.0.7) 
TARGET_LINK_LIBRARIES(card_manage ${HUAWEI_PME_BMC_SOURCE_DIR}/src/rootfs/usr/lib/libuip.so.0) 
TARGET_LINK_LIBRARIES(card_manage ${HUAWEI_PME_BMC_SOURCE_DIR}/src/rootfs/usr/lib/libsqlite3_pme.so.0) 
TARGET_LINK_LIBRARIES(card_manage ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libsqlite3.so.0.8.6) 

if (ITEST_FLAG)
	TARGET_LINK_LIBRARIES(card_manage ${ITEST_LIB})
endif()

# open source lib

if (UT)
    INSTALL(TARGETS card_manage ARCHIVE DESTINATION opt/pme/lib)
else()
    INSTALL(TARGETS card_manage RUNTIME DESTINATION opt/bmc/apps/legacy/card_manage)
endif()

OPTION(FUZZ_COV "" OFF)
OPTION(ASAN_TEST "" OFF)

if (FUZZ_COV)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage -O0")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage -O0")
    ADD_DEFINITIONS(-DENABLE_FUZZ_COV)
endif()

if (ASAN_TEST)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fsanitize-recover=address,all -fno-omit-frame-pointer -fno-stack-protector -O0")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize-recover=address,all -fno-omit-frame-pointer -fno-stack-protector -O0")
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lasan")
    ADD_DEFINITIONS(-DENABLE_ASAN)
endif()