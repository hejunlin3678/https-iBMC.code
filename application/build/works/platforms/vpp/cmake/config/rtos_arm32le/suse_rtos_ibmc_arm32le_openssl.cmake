option(OPENSSL "openssl" ON)

set(PDT_NAME "IBMC")
set(PLT_NAME "SUSE12_SP5_ARM32LE_IBMC")

include(${CMAKE_CURRENT_LIST_DIR}/../openssl/openssl_${PDT_NAME}.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/suse_rtos_ibmc_arm32le_options.cmake)

# 自研crypto_ex库配置
set(LIBNAME_CRYPTO_EX "crypto_ex")
set(COMPILE_OPTIONS_CRYPTO_EX ${CC_ALL_OPTIONS})
set(COMPILE_MACRO_CRYPTO_EX "-DVPP_OPENSSL_VERSION_3_0 -DOPENSSL_SUPPRESS_DEPRECATED -DOPENSSL_NO_FILENAMES -D_VSB_CONFIG_FILE=lib/h/config/vsbConfig.h")

# 自研ssl_ex库配置
set(LIBNAME_SSL_EX "ssl_ex")
set(COMPILE_OPTIONS_SSL_EX ${CC_ALL_OPTIONS})
set(COMPILE_MACRO_SSL_EX "-DVPP_OPENSSL_VERSION_3_0 -DOPENSSL_SUPPRESS_DEPRECATED -DOPENSSL_NO_FILENAMES -D_VSB_CONFIG_FILE=lib/h/config/vsbConfig.h")

# 配置链接选项
set(LDFLAGS ${SHARED_LNK_FLAGS})

# openssl开关配置
set(ENABLE_ASM "y")
set(ENV_OPT "--prefix=/usr --prefix=/usr --openssldir=/usr/lib/ssl-3 --libdir=/usr/lib enable-ui-console enable-tls-sigalg-sha1")
set(OPTION_SHARED "y" CACHE STRING "overwrite configure before" FORCE)
set(LIB_LEGACY "y")
set(LIB_ALL "y" CACHE STRING "overwrite configure before" FORCE)
set(LIBTYPE_SHARED ${OPTION_SHARED})

# 删除公共编译选项中开源openssl原本就存在的选项, 避免重复编译选项
foreach(flag ${CC_OPENSOURCE_OPTIONS})
    set(_CC_OPENSOURCE_OPTIONS "${_CC_OPENSOURCE_OPTIONS} ${flag}")
endforeach()
string(REGEX REPLACE "-O2|-fPIC" "" _CC_OPENSOURCE_OPTIONS ${_CC_OPENSOURCE_OPTIONS})

# 开源部分编译选项目标系统, 编译选项, 链接选项, 编译宏设置
set(COMPILE_TARGET "linux-armv4") # 目标系统,在开源根目录下执行 perl Configure LIST 查看可选的目标系统
set(CNF_CFLAGS ${_CC_OPENSOURCE_OPTIONS})
set(CNF_CPPDEFINES "CODE_BRANCH_FLAGS=${CODE_BRANCH_FLAGS}") # 开源特殊编译宏，开源openssl使用CPPDEFINES添加宏定义时， 不加 /D 或 -D , openssl Configure脚本会自动添加-D
set(CNF_CPPINCLUDES "")
set(CNF_LDFLAGS "${LDFLAGS}")
set(CNF_LDLIBS "")

set(VPP_OPENSSL_VERSION_3_0 on)

# RENAME_ENABLE 是否修改库名称, 不需要修改的话设为"n", 最终库名为: ${RENAME_***_PREFIX}${RENAME_LIBNAME_***}${RENAME_***_SUFFIX}
# 若需要修改库名称RENAME_ENABLE 设为"y", 前缀可以为空字符串, 库名和文件后缀名不能为空
set(RENAME_ENABLE "y")
set(OSSL_RENAME_EXT "y")
set(RENAME_STATIC_PREFIX "lib")
set(RENAME_SHARED_PREFIX "lib")
set(RENAME_LIBNAME_CRYPTO "crypto_ossl")
set(RENAME_LIBNAME_SSL "ssl_ossl")
set(RENAME_STATIC_SUFFIX ".a")
set(RENAME_SHARED_SUFFIX ".so")