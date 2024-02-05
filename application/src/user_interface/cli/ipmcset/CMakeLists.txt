#==============================================================================

## Customizing Section: adjust the following if necessary.
##=============================================================================

AUX_SOURCE_DIRECTORY(src IPMCSET_SRC)
list(SORT IPMCSET_SRC)
ADD_EXECUTABLE(ipmcset ${IPMCSET_SRC})
if(FROM_SOURCE)
    add_dependencies(ipmcset libssh2 sqlite)
endif()

INCLUDE_DIRECTORIES(../inc)
INCLUDE_DIRECTORIES(../inc/ipmcset)
INCLUDE_DIRECTORIES(${HUAWEI_PME_BMC_SOURCE_DIR}/src/include)
INCLUDE_DIRECTORIES(${HUAWEI_PME_BMC_SOURCE_DIR}/src/include/open_source)
INCLUDE_DIRECTORIES(${HUAWEI_PME_BMC_SOURCE_DIR}/src/include/pme_app/uip)
INCLUDE_DIRECTORIES(${HUAWEI_PME_BMC_SOURCE_DIR}/src/include/pme_app/property_method)
INCLUDE_DIRECTORIES(${SDK_INC_PME}/pme/debug)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIE")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIE")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pie ")

INCLUDE_DIRECTORIES(${SDK_INC})
INCLUDE_DIRECTORIES(${SDK_INC_PME})
INCLUDE_DIRECTORIES(${HUAWEI_PME_BMC_SOURCE_DIR}/src/include/open_source)
INCLUDE_DIRECTORIES(${SDK_INC}/glib-2.0)
INCLUDE_DIRECTORIES(${SDK_LIB}/glib-2.0/include)

# dalib的被动依赖
TARGET_LINK_LIBRARIES(ipmcset ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libnetsnmpagent.so)
TARGET_LINK_LIBRARIES(ipmcset ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libnetsnmphelpers.so) 
TARGET_LINK_LIBRARIES(ipmcset ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libnetsnmpmibs.so) 
TARGET_LINK_LIBRARIES(ipmcset ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libnetsnmp.so) 
TARGET_LINK_LIBRARIES(ipmcset ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libnetsnmptrapd.so) 
TARGET_LINK_LIBRARIES(ipmcset ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libpcre.so)
TARGET_LINK_LIBRARIES(ipmcset ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libpcrecpp.so)
TARGET_LINK_LIBRARIES(ipmcset ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libpcreposix.so)
TARGET_LINK_LIBRARIES(ipmcset ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libsqlite3.so)
TARGET_LINK_LIBRARIES(ipmcset ${HUAWEI_PME_BMC_SOURCE_DIR}/src/rootfs/usr/lib/libuip.so)
TARGET_LINK_LIBRARIES(ipmcset ${HUAWEI_PME_BMC_SOURCE_DIR}/src/rootfs/usr/lib/libsqlite3_pme.so)

TARGET_LINK_LIBRARIES(ipmcset ${GLIB2})
TARGET_LINK_LIBRARIES(ipmcset ${HUAWEI_PME_BMC_SOURCE_DIR}/src/rootfs/usr/lib/libdalib.so)
TARGET_LINK_LIBRARIES(ipmcset ipmccommon)
TARGET_LINK_LIBRARIES(ipmcset ${COMMON})
TARGET_LINK_LIBRARIES(ipmcset ${IPMI_COMMON})
TARGET_LINK_LIBRARIES(ipmcset ${DFLIB})
TARGET_LINK_LIBRARIES(ipmcset ${SECURE})

TARGET_LINK_LIBRARIES(ipmcset ${XML_PARSER})
TARGET_LINK_LIBRARIES(ipmcset ${HASLIB})
TARGET_LINK_LIBRARIES(ipmcset ${DFLBASE})
TARGET_LINK_LIBRARIES(ipmcset ${THRIFT})
TARGET_LINK_LIBRARIES(ipmcset ${MUPARSER})
TARGET_LINK_LIBRARIES(ipmcset ${PFLASH})
TARGET_LINK_LIBRARIES(ipmcset ${DFRPC})
TARGET_LINK_LIBRARIES(ipmcset ${MAINT_DEBUG_COM})
TARGET_LINK_LIBRARIES(ipmcset ${MAINT_DEBUG_MSG})
TARGET_LINK_LIBRARIES(ipmcset ${SSH2})
TARGET_LINK_LIBRARIES(ipmcset ${SSL})
TARGET_LINK_LIBRARIES(ipmcset ${CRYPTO})
TARGET_LINK_LIBRARIES(ipmcset ${FILE_MANAGE})
TARGET_LINK_LIBRARIES(ipmcset ${DRIVERS})
TARGET_LINK_LIBRARIES(ipmcset ${PERSISTANCE})

TARGET_LINK_LIBRARIES(ipmcset ${MAINT_DEBUG})
TARGET_LINK_LIBRARIES(ipmcset ${M})
TARGET_LINK_LIBRARIES(ipmcset ${HUAWEI_PME_BMC_SOURCE_DIR}/src/rootfs/usr/lib/libsml_base.so)
TARGET_LINK_LIBRARIES(ipmcset ${HUAWEI_PME_BMC_SOURCE_DIR}/src/rootfs/usr/lib/libsmm_public.so)

INSTALL(TARGETS ipmcset RUNTIME DESTINATION opt/pme/bin)