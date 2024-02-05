# this one is important
SET(CMAKE_SYSTEM_NAME Linux)
#this one not so much
SET(CMAKE_SYSTEM_VERSION 1)

# use, i.e. don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  TRUE)
SET(CMAKE_SKIP_RPATH TRUE)
SET(CMAKE_SKIP_INSTALL_RPATH TRUE)
# when building, don't use the install RPATH already
# (but later on when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)


# specify the cross compiler
SET(CMAKE_C_COMPILER  aarch64-target-linux-gnu-gcc)
SET(CMAKE_CXX_COMPILER  aarch64-target-linux-gnu-g++)

# where is the target environment 
SET(CMAKE_FIND_ROOT_PATH  /opt/RTOS/208.5.0/arm64le_5.10_ek_preempt_pro/sdk)

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
# set this var BOTH, because of src/lib_open_source is need.
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# global var
# cross compile libs dir
SET(SDK_LIB "/opt/RTOS/208.5.0/arm64le_5.10_ek_preempt_pro/sdk/usr/lib64")
# cross compile include dir
SET(SDK_INC "/opt/RTOS/208.5.0/arm64le_5.10_ek_preempt_pro/sdk/usr/include")
# PME libs dir
SET(SDK_LIB_PME "${HUAWEI_PME_BMC_SOURCE_DIR}/sdk/PME/V100R001C10/lib")
# PME include dir
SET(SDK_INC_PME "${HUAWEI_PME_BMC_SOURCE_DIR}/sdk/PME/V100R001C10/include")

# global lib
SET(PTHREAD "${CMAKE_FIND_ROOT_PATH}/usr/lib64/libpthread.so")
SET(GLIB2 "${SDK_LIB}/libglib-2.0.so")
SET(GTHREAD2 "${SDK_LIB}/libgthread-2.0.so")
SET(RT "${CMAKE_FIND_ROOT_PATH}/usr/lib64/librt.so")
SET(TCL8 "${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libtcl8.6.so")
SET(M "${CMAKE_FIND_ROOT_PATH}/usr/lib64/libm.so")
SET(DL "${CMAKE_FIND_ROOT_PATH}/usr/lib64/libdl.so")
SET(XML2 "${CMAKE_FIND_ROOT_PATH}/usr/lib64/libxml2.so")
SET(JPEG "${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libjpeg.so")
SET(LIBEDIT "${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libedit.so")
SET(NETSNMP "${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libnetsnmp.so")
SET(NETSNMP_STATIC "${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libnetsnmp.a")
SET(NETSNMPTRAPD "${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libnetsnmptrapd.so")
SET(NETSNMPTRAPD_STATIC "${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libnetsnmptrapd.a")
SET(NETSNMPAGENT "${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libnetsnmpagent.so")
SET(NETSNMPHELPERS "${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libnetsnmphelpers.so")
SET(NETSNMPMIBS "${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libnetsnmpmibs.so")
SET(PCRE_LIB            "${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libpcre.so")
SET(PCRECPP_LIB         "${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libpcrecpp.so")
SET(PCREPOSIX_LIB       "${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libpcreposix.so")
SET(CRYPT "${CMAKE_FIND_ROOT_PATH}/usr/lib64/libcrypt.so")
SET(LDAP "${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libldap.so")
SET(PAM "${CMAKE_FIND_ROOT_PATH}/lib64/libpam.so")
SET(PAM_MISC "${CMAKE_FIND_ROOT_PATH}/lib64/libpam_misc.so")
SET(SQLITE3 "${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libsqlite3.so")
SET(BZ2 "${CMAKE_FIND_ROOT_PATH}/usr/lib64/libbz2.so")
SET(Z "${CMAKE_FIND_ROOT_PATH}/usr/lib64/libz.so")
SET(SSL "${CMAKE_FIND_ROOT_PATH}/usr/lib64/libssl.so")
SET(CRYPTO "${CMAKE_FIND_ROOT_PATH}/usr/lib64/libcrypto.so")
SET(JSON "${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libjson.so")
SET(RESOLV "${CMAKE_FIND_ROOT_PATH}/usr/lib64/libresolv.so")
SET(CIM_FW "${HUAWEI_PME_BMC_SOURCE_DIR}/rootfs/opt/pme/lib/libcim_fw.so")
SET(CIMPLE "${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libcimplecmpiadap.so")
SET(SSH2 "${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libssh2.so.1")
SET(CURL "${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libcurl.so")
#SET(MUPARSER "${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libmuparser.so")
SET(LBER "${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/liblber.so")
SET(COMMON "${SDK_LIB_PME}/libcommon.so")
SET(FILE_MANAGE "${SDK_LIB_PME}/libfile_manage.so")
SET(DFLIB "${SDK_LIB_PME}/libdflib.so")
SET(DFLBASE "${SDK_LIB_PME}/libdflbase.so")
SET(PFLASH "${SDK_LIB_PME}/libpflash_drv.so")
SET(XML_PARSER "${SDK_LIB_PME}/libxml_parser.so")
SET(HASLIB "${SDK_LIB_PME}/libhaslib.so")
SET(THRIFT "${SDK_LIB_PME}/libthrift.so")
SET(MUPARSER "${SDK_LIB_PME}/libmuparser.so")
SET(DFRPC "${SDK_LIB_PME}/libdfrpc.so")
SET(FILE_MANAGE "${SDK_LIB_PME}/libfile_manage.so")
SET(PERSISTANCE "${SDK_LIB_PME}/libpersistance.so")
SET(IPMI_COMMON "${SDK_LIB_PME}/libipmi_common.so")
SET(MAINT_DEBUG "${SDK_LIB_PME}/libmaint_debug.so")
SET(MAINT_DEBUG_COM "${SDK_LIB_PME}/libmaint_debug_com.so")
SET(MAINT_DEBUG_MSG "${SDK_LIB_PME}/libmaint_debug_msg.so")
SET(DRIVERS "${SDK_LIB_PME}/libdrivers.so")
SET(PME_CRYPTO "${SDK_LIB_PME}/libpme_crypto.so")
SET(SASL2 "${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libsasl2.so")
SET(SECURE "${SDK_LIB_PME}/libsecure.so")
SET(PAM_TALLY2 "${CMAKE_FIND_ROOT_PATH}/lib64/security/pam_tally2.so")
SET(LSW_SMM "${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libunilsw_smm.so")
SET(ADAPTIVE_LM "${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/liblic.so")
SET(KRB5_FUNC "${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libkrb5.so")
SET(KRB5_CRYPTO "${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libk5crypto.so")
SET(KRB5_SUPPORT "${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libkrb5support.so")
SET(KRB5_COMERR "${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libcom_err.so")
SET(KRB5_GSSAPI "${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libgssapi_krb5.so")
SET(LUAJIT "${HUAWEI_PME_BMC_SOURCE_DIR}/src/lib_open_source/libluajit.so")
SET(ARM64_HI1711_ENABLED 1)
ADD_DEFINITIONS(-DARM64_HI1711_ENABLED)
