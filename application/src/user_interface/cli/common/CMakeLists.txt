#==============================================================================

## Customizing Section: adjust the following if necessary.
##=============================================================================

AUX_SOURCE_DIRECTORY(. IPMCCOMMON_SRC)
AUX_SOURCE_DIRECTORY(./ipmcget IPMCCOMMON_SRC)
AUX_SOURCE_DIRECTORY(./ipmcset IPMCCOMMON_SRC)
list(SORT IPMCCOMMON_SRC)
ADD_LIBRARY(ipmccommon SHARED ${IPMCCOMMON_SRC})

INCLUDE_DIRECTORIES(../inc)
INCLUDE_DIRECTORIES(../inc/ipmcset)
INCLUDE_DIRECTORIES(../inc/ipmcget)
INCLUDE_DIRECTORIES(${HUAWEI_PME_BMC_SOURCE_DIR}/src/include)
INCLUDE_DIRECTORIES(${HUAWEI_PME_BMC_SOURCE_DIR}/src/include/open_source)
INCLUDE_DIRECTORIES(${HUAWEI_PME_BMC_SOURCE_DIR}/src/include/pme_app/uip)
INCLUDE_DIRECTORIES(${HUAWEI_PME_BMC_SOURCE_DIR}/src/include/pme_app/property_method)
INCLUDE_DIRECTORIES(${SDK_INC_PME}/pme/debug)
if(FROM_SOURCE)
    add_dependencies(ipmccommon sqlite)
endif()

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIE")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIE")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pie ")

INCLUDE_DIRECTORIES(${SDK_INC})
INCLUDE_DIRECTORIES(${SDK_INC_PME})
INCLUDE_DIRECTORIES(${HUAWEI_PME_BMC_SOURCE_DIR}/src/include/open_source)
INCLUDE_DIRECTORIES(${SDK_INC}/glib-2.0)
INCLUDE_DIRECTORIES(${SDK_LIB}/glib-2.0/include)

# dalib的被动依赖
TARGET_LINK_LIBRARIES(payload ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libnetsnmpagent.so)
TARGET_LINK_LIBRARIES(payload ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libnetsnmphelpers.so) 
TARGET_LINK_LIBRARIES(payload ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libnetsnmpmibs.so) 
TARGET_LINK_LIBRARIES(payload ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libnetsnmp.so) 
TARGET_LINK_LIBRARIES(payload ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libnetsnmptrapd.so) 
TARGET_LINK_LIBRARIES(payload ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libpcre.so)
TARGET_LINK_LIBRARIES(payload ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libpcrecpp.so)
TARGET_LINK_LIBRARIES(payload ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libpcreposix.so)
TARGET_LINK_LIBRARIES(payload ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libsqlite3.so)
TARGET_LINK_LIBRARIES(payload ${HUAWEI_PME_BMC_SOURCE_DIR}/src/rootfs/usr/lib/libuip.so)
TARGET_LINK_LIBRARIES(payload ${HUAWEI_PME_BMC_SOURCE_DIR}/src/rootfs/usr/lib/libsqlite3_pme.so)

TARGET_LINK_LIBRARIES(ipmccommon ${GLIB2})
TARGET_LINK_LIBRARIES(payload ${HUAWEI_PME_BMC_SOURCE_DIR}/src/rootfs/usr/lib/libdalib.so)

INSTALL(TARGETS ipmccommon LIBRARY DESTINATION usr/lib)