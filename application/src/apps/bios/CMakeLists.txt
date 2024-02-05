#==============================================================================

## Customizing Section: adjust the following if necessary.
##=============================================================================

AUX_SOURCE_DIRECTORY(src BIOS_SRC)
AUX_SOURCE_DIRECTORY(src/clp BIOS_SRC)
AUX_SOURCE_DIRECTORY(src/clp/clp_2xx BIOS_SRC)
AUX_SOURCE_DIRECTORY(src/clp/clp_3xx_9xx BIOS_SRC)
AUX_SOURCE_DIRECTORY(src/clp/clp_5xx BIOS_SRC)
AUX_SOURCE_DIRECTORY(src/clp/clp_lom BIOS_SRC)
list(SORT BIOS_SRC)
ADD_EXECUTABLE(bios ${BIOS_SRC})
if(FROM_SOURCE)
    add_dependencies(bios libssh2 sqlite)
endif()
INCLUDE_DIRECTORIES(inc)
INCLUDE_DIRECTORIES(./inc/clp)
INCLUDE_DIRECTORIES(./inc/clp/clp_2xx)
INCLUDE_DIRECTORIES(./inc/clp/clp_3xx_9xx)
INCLUDE_DIRECTORIES(./inc/clp/clp_5xx)
INCLUDE_DIRECTORIES(./inc/clp/clp_lom)
INCLUDE_DIRECTORIES(${HUAWEI_PME_BMC_SOURCE_DIR}/src/include)
INCLUDE_DIRECTORIES(${HUAWEI_PME_BMC_SOURCE_DIR}/../platforms/kmc/include)
INCLUDE_DIRECTORIES(${HUAWEI_PME_BMC_SOURCE_DIR}/src/include/pme_app/kmc)

# turn on this, you can use else()
SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS ON)

INCLUDE_DIRECTORIES(${HUAWEI_PME_BMC_SOURCE_DIR}/src/include/open_source)
INCLUDE_DIRECTORIES(${SDK_INC}/glib-2.0)
INCLUDE_DIRECTORIES(${SDK_LIB}/glib-2.0/include)
INCLUDE_DIRECTORIES(${SDK_INC} ${SDK_INC_PME})

# dalib的被动依赖
TARGET_LINK_LIBRARIES(bios ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libnetsnmpagent.so)
TARGET_LINK_LIBRARIES(bios ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libnetsnmphelpers.so) 
TARGET_LINK_LIBRARIES(bios ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libnetsnmpmibs.so) 
TARGET_LINK_LIBRARIES(bios ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libnetsnmp.so) 
TARGET_LINK_LIBRARIES(bios ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libnetsnmptrapd.so) 
TARGET_LINK_LIBRARIES(bios ${HUAWEI_PME_BMC_SOURCE_DIR}/src/rootfs/usr/lib/libarithmetic.so)
TARGET_LINK_LIBRARIES(bios ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libpcre.so)
TARGET_LINK_LIBRARIES(bios ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libpcrecpp.so)
TARGET_LINK_LIBRARIES(bios ${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libpcreposix.so) 

TARGET_LINK_LIBRARIES(bios ${GLIB2})
TARGET_LINK_LIBRARIES(bios ${JSON})
TARGET_LINK_LIBRARIES(bios ${HUAWEI_PME_BMC_SOURCE_DIR}/src/rootfs/usr/lib/libdalib.so)
TARGET_LINK_LIBRARIES(bios ${HUAWEI_PME_BMC_SOURCE_DIR}/src/rootfs/usr/lib/libxmlparser.so)

TARGET_LINK_LIBRARIES(bios ${COMMON})
TARGET_LINK_LIBRARIES(bios ${DFLIB})
TARGET_LINK_LIBRARIES(bios ${SECURE})

TARGET_LINK_LIBRARIES(bios ${XML_PARSER})
TARGET_LINK_LIBRARIES(bios ${HASLIB})
TARGET_LINK_LIBRARIES(bios ${DFLBASE})
TARGET_LINK_LIBRARIES(bios ${THRIFT})
TARGET_LINK_LIBRARIES(bios ${MUPARSER})

TARGET_LINK_LIBRARIES(bios ${PFLASH})
TARGET_LINK_LIBRARIES(bios ${DFRPC})
TARGET_LINK_LIBRARIES(bios ${MAINT_DEBUG_COM})
TARGET_LINK_LIBRARIES(bios ${MAINT_DEBUG_MSG})
TARGET_LINK_LIBRARIES(bios ${SSH2})
TARGET_LINK_LIBRARIES(bios ${CRYPTO})

TARGET_LINK_LIBRARIES(bios ${FILE_MANAGE})
TARGET_LINK_LIBRARIES(bios ${PERSISTANCE})
TARGET_LINK_LIBRARIES(bios ${IPMI_COMMON})
TARGET_LINK_LIBRARIES(bios ${MAINT_DEBUG})
TARGET_LINK_LIBRARIES(bios ${DRIVERS})
TARGET_LINK_LIBRARIES(bios ${CRYPTO})
TARGET_LINK_LIBRARIES(bios ${HUAWEI_PME_BMC_SOURCE_DIR}/src/rootfs/usr/lib/libkmc.so)

INSTALL(TARGETS bios RUNTIME DESTINATION opt/bmc/apps/legacy/bios)