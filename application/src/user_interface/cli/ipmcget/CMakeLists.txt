#==============================================================================

## Customizing Section: adjust the following if necessary.
##=============================================================================

AUX_SOURCE_DIRECTORY(src IPMCGET_SRC)
list(SORT IPMCGET_SRC)
ADD_EXECUTABLE(ipmcget ${IPMCGET_SRC})  
if(FROM_SOURCE)
    add_dependencies(ipmcget libssh2 sqlite)
endif()

INCLUDE_DIRECTORIES(../inc)
INCLUDE_DIRECTORIES(../inc/ipmcget)
INCLUDE_DIRECTORIES(${HUAWEI_PME_BMC_SOURCE_DIR}/src/include)
INCLUDE_DIRECTORIES(${HUAWEI_PME_BMC_SOURCE_DIR}/src/include/open_source)
INCLUDE_DIRECTORIES(${HUAWEI_PME_BMC_SOURCE_DIR}/src/include/pme_app/uip)
INCLUDE_DIRECTORIES(${HUAWEI_PME_BMC_SOURCE_DIR}/src/include/pme_app/property_method)
INCLUDE_DIRECTORIES(${SDK_INC_PME}/pme/debug)
INCLUDE_DIRECTORIES(${SDK_INC_PME})
INCLUDE_DIRECTORIES(${SDK_INC_PME}/pme/common)
INCLUDE_DIRECTORIES(${SDK_INC_PME}/pme/dflib)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIE")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIE")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pie ")

INCLUDE_DIRECTORIES(${SDK_INC})
INCLUDE_DIRECTORIES(${HUAWEI_PME_BMC_SOURCE_DIR}/src/include/open_source)
INCLUDE_DIRECTORIES(${SDK_INC}/glib-2.0)
INCLUDE_DIRECTORIES(${SDK_LIB}/glib-2.0/include)

# dalib的被动依赖
TARGET_LINK_LIBRARIES(ipmcget ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libnetsnmpagent.so)
TARGET_LINK_LIBRARIES(ipmcget ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libnetsnmphelpers.so) 
TARGET_LINK_LIBRARIES(ipmcget ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libnetsnmpmibs.so) 
TARGET_LINK_LIBRARIES(ipmcget ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libnetsnmp.so) 
TARGET_LINK_LIBRARIES(ipmcget ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libnetsnmptrapd.so) 
TARGET_LINK_LIBRARIES(ipmcget ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libpcre.so)
TARGET_LINK_LIBRARIES(ipmcget ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libpcrecpp.so)
TARGET_LINK_LIBRARIES(ipmcget ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libpcreposix.so)
TARGET_LINK_LIBRARIES(ipmcget ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libsqlite3.so)
TARGET_LINK_LIBRARIES(ipmcget ${HUAWEI_PME_BMC_SOURCE_DIR}/src/rootfs/usr/lib/libuip.so)
TARGET_LINK_LIBRARIES(ipmcget ${HUAWEI_PME_BMC_SOURCE_DIR}/src/rootfs/usr/lib/libsqlite3_pme.so)

TARGET_LINK_LIBRARIES(ipmcget ${GLIB2})
TARGET_LINK_LIBRARIES(ipmcget ipmccommon)
TARGET_LINK_LIBRARIES(ipmcget ${COMMON})
TARGET_LINK_LIBRARIES(ipmcget ${DFLIB})
TARGET_LINK_LIBRARIES(ipmcget ${SECURE})

TARGET_LINK_LIBRARIES(ipmcget ${XML_PARSER})
TARGET_LINK_LIBRARIES(ipmcget ${HASLIB})
TARGET_LINK_LIBRARIES(ipmcget ${DFLBASE})
TARGET_LINK_LIBRARIES(ipmcget ${THRIFT})
TARGET_LINK_LIBRARIES(ipmcget ${MUPARSER})
TARGET_LINK_LIBRARIES(ipmcget ${PFLASH})
TARGET_LINK_LIBRARIES(ipmcget ${DFRPC})
TARGET_LINK_LIBRARIES(ipmcget ${MAINT_DEBUG_COM})
TARGET_LINK_LIBRARIES(ipmcget ${MAINT_DEBUG_MSG})
TARGET_LINK_LIBRARIES(ipmcget ${SSH2})
TARGET_LINK_LIBRARIES(ipmcget ${SSL})
TARGET_LINK_LIBRARIES(ipmcget ${CRYPTO})
TARGET_LINK_LIBRARIES(ipmcget ${PERSISTANCE})
TARGET_LINK_LIBRARIES(ipmcget ${DRIVERS})
TARGET_LINK_LIBRARIES(ipmcget ${FILE_MANAGE})

TARGET_LINK_LIBRARIES(ipmcget ${MAINT_DEBUG})
TARGET_LINK_LIBRARIES(ipmcget ${HUAWEI_PME_BMC_SOURCE_DIR}/src/rootfs/usr/lib/libdalib.so)
TARGET_LINK_LIBRARIES(ipmcget ${HUAWEI_PME_BMC_SOURCE_DIR}/src/rootfs/usr/lib/libsml_base.so)
TARGET_LINK_LIBRARIES(ipmcget ${HUAWEI_PME_BMC_SOURCE_DIR}/src/rootfs/usr/lib/libsmm_public.so)
TARGET_LINK_LIBRARIES(ipmcget ${M})

INSTALL(TARGETS ipmcget RUNTIME DESTINATION opt/pme/bin)