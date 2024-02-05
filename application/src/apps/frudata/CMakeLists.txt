#==============================================================================

## Customizing Section: adjust the following if necessary.
##=============================================================================

aux_source_directory(src FRU_DATA_SRC)
aux_source_directory(src/component FRU_DATA_SRC)
aux_source_directory(src/elabel FRU_DATA_SRC)
aux_source_directory(src/intf FRU_DATA_SRC)
aux_source_directory(src/media FRU_DATA_SRC)
list(SORT FRU_DATA_SRC)
add_executable(frudata ${FRU_DATA_SRC})

if(FROM_SOURCE)
    add_dependencies(frudata libssh2 curl jsonc sqlite)
endif()
    
include_directories(inc)
include_directories(${HUAWEI_PME_BMC_SOURCE_DIR}/src/include)
include_directories(${HUAWEI_PME_BMC_SOURCE_DIR}/src/include/open_source)
include_directories(${HUAWEI_PME_BMC_SOURCE_DIR}/src/libs/kmc/inc)
include_directories(${HUAWEI_PME_BMC_SOURCE_DIR}/src/include/pme_app/kmc)
include_directories(${HUAWEI_PME_BMC_SOURCE_DIR}/src/include/pme_app/common)

include_directories(${SDK_INC}/libxml2)
include_directories(${SDK_INC}/glib-2.0)
include_directories(${SDK_LIB}/glib-2.0/include)
include_directories(${SDK_INC} ${SDK_INC_PME})
include_directories(${HUAWEI_PME_BMC_SOURCE_DIR}/src/include/open_source/json)

link_directories(${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source)
# dalib的被动依赖
TARGET_LINK_LIBRARIES(frudata ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libnetsnmpagent.so)
TARGET_LINK_LIBRARIES(frudata ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libnetsnmphelpers.so) 
TARGET_LINK_LIBRARIES(frudata ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libnetsnmpmibs.so) 
TARGET_LINK_LIBRARIES(frudata ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libnetsnmp.so) 
TARGET_LINK_LIBRARIES(frudata ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libnetsnmptrapd.so) 
TARGET_LINK_LIBRARIES(frudata ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libpcre.so)
TARGET_LINK_LIBRARIES(frudata ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libpcrecpp.so)
TARGET_LINK_LIBRARIES(frudata ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libpcreposix.so)
TARGET_LINK_LIBRARIES(frudata ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libsqlite3.so)
TARGET_LINK_LIBRARIES(frudata ${HUAWEI_PME_BMC_SOURCE_DIR}/src/rootfs/usr/lib/libuip.so)
TARGET_LINK_LIBRARIES(frudata ${HUAWEI_PME_BMC_SOURCE_DIR}/src/rootfs/usr/lib/libsqlite3_pme.so)

TARGET_LINK_LIBRARIES(frudata ${GLIB2})
TARGET_LINK_LIBRARIES(frudata ${HUAWEI_PME_BMC_SOURCE_DIR}/src/rootfs/usr/lib/libdalib.so)
TARGET_LINK_LIBRARIES(frudata ${CURL})

# common lib
TARGET_LINK_LIBRARIES(frudata ${COMMON})
TARGET_LINK_LIBRARIES(frudata ${MAINT_DEBUG})
TARGET_LINK_LIBRARIES(frudata ${DFLIB})
TARGET_LINK_LIBRARIES(frudata ${SECURE})

TARGET_LINK_LIBRARIES(frudata ${XML_PARSER})
TARGET_LINK_LIBRARIES(frudata ${HASLIB})
TARGET_LINK_LIBRARIES(frudata ${DFLBASE})
TARGET_LINK_LIBRARIES(frudata ${THRIFT})
TARGET_LINK_LIBRARIES(frudata ${MUPARSER})

TARGET_LINK_LIBRARIES(frudata ${PFLASH})
TARGET_LINK_LIBRARIES(frudata ${DFRPC})
TARGET_LINK_LIBRARIES(frudata ${MAINT_DEBUG_COM})
TARGET_LINK_LIBRARIES(frudata ${MAINT_DEBUG_MSG})
TARGET_LINK_LIBRARIES(frudata ${SSH2})

TARGET_LINK_LIBRARIES(frudata ${PERSISTANCE})
TARGET_LINK_LIBRARIES(frudata ${MAINT_DEBUG})
TARGET_LINK_LIBRARIES(frudata ${IPMI_COMMON})
TARGET_LINK_LIBRARIES(frudata ${CRYPTO})
TARGET_LINK_LIBRARIES(frudata ${SSL})
TARGET_LINK_LIBRARIES(frudata ${DRIVERS})
TARGET_LINK_LIBRARIES(frudata ${FILE_MANAGE})
TARGET_LINK_LIBRARIES(frudata ${XML2})
TARGET_LINK_LIBRARIES(frudata ${HUAWEI_PME_BMC_SOURCE_DIR}/src/rootfs/usr/lib/libarithmetic.so)
TARGET_LINK_LIBRARIES(frudata ${PTHREAD})
TARGET_LINK_LIBRARIES(frudata ${HUAWEI_PME_BMC_SOURCE_DIR}/src/rootfs/usr/lib/libkmc.so)

TARGET_LINK_LIBRARIES(frudata ${HUAWEI_PME_BMC_SOURCE_DIR}/src/rootfs/usr/lib/libxmlparser.so)
TARGET_LINK_LIBRARIES(frudata ${JSON})
TARGET_LINK_LIBRARIES(frudata ${HUAWEI_PME_BMC_SOURCE_DIR}/src/rootfs/usr/lib/libpme_mctp_rxtx.so)
TARGET_LINK_LIBRARIES(frudata ${HUAWEI_PME_BMC_SOURCE_DIR}/src/rootfs/usr/lib/libpcie_mgmt.so)
TARGET_LINK_LIBRARIES(frudata ${HUAWEI_PME_BMC_SOURCE_DIR}/src/rootfs/usr/lib/libsmm_public.so)
if(EUREKA)
    install(TARGETS frudata RUNTIME DESTINATION /opt/pme/apps)
else()
    install(TARGETS frudata RUNTIME DESTINATION opt/bmc/apps/legacy/frudata)
endif()
