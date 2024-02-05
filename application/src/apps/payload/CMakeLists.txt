#==============================================================================

## Customizing Section: adjust the following if necessary.
##=============================================================================

aux_source_directory(src PAYLOAD_SRC)
list(SORT PAYLOAD_SRC)
add_executable(payload ${PAYLOAD_SRC})
if(FROM_SOURCE)
    add_dependencies(payload libssh2 jsonc curl sqlite)
endif()
INCLUDE_DIRECTORIES(inc)
INCLUDE_DIRECTORIES(${HUAWEI_PME_BMC_SOURCE_DIR}/src/include)
INCLUDE_DIRECTORIES(${HUAWEI_PME_BMC_SOURCE_DIR}/src/include/open_source)
INCLUDE_DIRECTORIES(${HUAWEI_PME_BMC_SOURCE_DIR}/src/libs/kmc/inc)
INCLUDE_DIRECTORIES(${HUAWEI_PME_BMC_SOURCE_DIR}/src/include/pme_app/kmc)
INCLUDE_DIRECTORIES(${HUAWEI_PME_BMC_SOURCE_DIR}/src/include/pme_app/common)

INCLUDE_DIRECTORIES(${SDK_INC}/libxml2)
INCLUDE_DIRECTORIES(${SDK_INC}/glib-2.0)
INCLUDE_DIRECTORIES(${SDK_LIB}/glib-2.0/include)
INCLUDE_DIRECTORIES(${SDK_INC} ${SDK_INC_PME})
INCLUDE_DIRECTORIES(${HUAWEI_PME_BMC_SOURCE_DIR}/src/include/open_source/json)

link_directories(${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source)

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

TARGET_LINK_LIBRARIES(payload ${GLIB2})
TARGET_LINK_LIBRARIES(payload ${HUAWEI_PME_BMC_SOURCE_DIR}/src/rootfs/usr/lib/libdalib.so)
TARGET_LINK_LIBRARIES(payload ${CURL})

TARGET_LINK_LIBRARIES(payload ${COMMON})
TARGET_LINK_LIBRARIES(payload ${MAINT_DEBUG})
TARGET_LINK_LIBRARIES(payload ${DFLIB})
TARGET_LINK_LIBRARIES(payload ${SECURE})

TARGET_LINK_LIBRARIES(payload ${XML_PARSER})
TARGET_LINK_LIBRARIES(payload ${HASLIB})
TARGET_LINK_LIBRARIES(payload ${DFLBASE})
TARGET_LINK_LIBRARIES(payload ${THRIFT})
TARGET_LINK_LIBRARIES(payload ${MUPARSER})

TARGET_LINK_LIBRARIES(payload ${PFLASH})
TARGET_LINK_LIBRARIES(payload ${DFRPC})
TARGET_LINK_LIBRARIES(payload ${MAINT_DEBUG_COM})
TARGET_LINK_LIBRARIES(payload ${MAINT_DEBUG_MSG})
TARGET_LINK_LIBRARIES(payload ${SSH2})

TARGET_LINK_LIBRARIES(payload ${PERSISTANCE})
TARGET_LINK_LIBRARIES(payload ${MAINT_DEBUG})
TARGET_LINK_LIBRARIES(payload ${IPMI_COMMON})
TARGET_LINK_LIBRARIES(payload ${CRYPTO})
TARGET_LINK_LIBRARIES(payload ${SSL})
TARGET_LINK_LIBRARIES(payload ${DRIVERS})
TARGET_LINK_LIBRARIES(payload ${FILE_MANAGE})
TARGET_LINK_LIBRARIES(payload ${XML2})
TARGET_LINK_LIBRARIES(payload ${HUAWEI_PME_BMC_SOURCE_DIR}/src/rootfs/usr/lib/libarithmetic.so)
TARGET_LINK_LIBRARIES(payload ${PTHREAD})
TARGET_LINK_LIBRARIES(payload ${HUAWEI_PME_BMC_SOURCE_DIR}/src/rootfs/usr/lib/libkmc.so)
TARGET_LINK_LIBRARIES(payload ${HUAWEI_PME_BMC_SOURCE_DIR}/src/rootfs/usr/lib/libxmlparser.so)
TARGET_LINK_LIBRARIES(payload ${JSON})
TARGET_LINK_LIBRARIES(payload ${HUAWEI_PME_BMC_SOURCE_DIR}/src/rootfs/usr/lib/libpme_mctp_rxtx.so)
TARGET_LINK_LIBRARIES(payload ${HUAWEI_PME_BMC_SOURCE_DIR}/src/rootfs/usr/lib/libpcie_mgmt.so)
TARGET_LINK_LIBRARIES(payload ${HUAWEI_PME_BMC_SOURCE_DIR}/src/rootfs/usr/lib/libsmm_public.so)
if(EUREKA)
    install(TARGETS payload RUNTIME DESTINATION /opt/pme/apps)
else()
    install(TARGETS payload RUNTIME DESTINATION opt/bmc/apps/legacy/payload)
endif()


