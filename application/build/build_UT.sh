#!/usr/bin/env bash
# This scripts bulid huawei PME BMC.test runs on x86 platform.
# Copyright © Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.

# 1. Export common global variable
cd "$(dirname "$(realpath $0)")" || exit 1
G_CURRENT_DIR=$(pwd)
G_RELATIVE_DIR="../../"
G_CODE_BASE=$(realpath ${G_CURRENT_DIR}/${G_RELATIVE_DIR}) # 代码仓路径
G_OPEN_PM_DIR="${G_CODE_BASE}/tools/others/dev_tools/llt/platform"
G_TEST_TREE_DIR="${G_CODE_BASE}/test_tree"
G_OPEN_SDK_DIR="${G_CODE_BASE}/application/sdk"
G_CMAKE_TOOL_CHAIN="${G_CODE_BASE}/application/build/arch/x86/x86-linux.cmake"
G_DT_FRAMEWORK_BIN_PATH="/opt/pme/sbin/dfm" # 编译环境上的dfm框架路径,当前框架暂时使用绝对路径
LSI_SML_LIB_PATH="${G_CODE_BASE}/test_tree/src/libs/smlib/sml_lsi/libsml_lsi.so" # 编译环境上的存储smllsi库路径 
# static_insert_stub tool 
STATIC_INSERT_DIR="${G_CODE_BASE}/tools/others/dev_tools/llt/static_insert_stub"

# test will run in G_DT_RUNTIME_TEMPROOT_DIR and take it as root dir
G_DT_RUNTIME_TEMPROOT_DIR="${G_TEST_TREE_DIR}/runtime_root"
G_DT_XML_BOARD_DIR="${G_TEST_TREE_DIR}/XMLs"

# 支持DT门禁的模块
G_SUPPORTED_APPS=("storage_mgnt" "ipmi" "user" "sensor_alarm" "kvm_vmm" "upgrade" "ui_manage" "rack_mgnt" "diagnose" \
                  "msm" "lsw_center" "lsw_main" "security_policy")
# 触发DT阈值的代码行数(预留功能，当前设置为0，即涉及修改就触发DT)
G_DT_THRESHOLD=0
# git diff输出显示的宽度，保证文件路径输出完整，防止字符匹配失败
G_WIDTH=400
# 需要运行DT的app列表文件（便于多个脚本共享）
G_DT_APPS_FILE="dt_apps.txt"
# 无app需要进行DT测试时标识符
G_APP_NONE="None"
# git差异比较(主线分支的前一个节点和当前最新节点)
G_SINCE="HEAD~"
G_UNTIL="HEAD"
# 脚本名称
G_SCRIPT_NAME="$0"

# 2. Define local functions and variables
rebuild_link_to_libs() {
    # 软链接映射字典，用于重建软链接, key=symbol link, value = target file
    declare -A LINK_MAPS
    LINK_MAPS=(
        ["libbz2.so.1.0"]="libbz2.so.1.0.5"
        ["libbz2.so.1"]="libbz2.so.1.0.5"
        ["libbz2.so"]="libbz2.so.1.0.5"
        ["libcim_fw.so.0"]="libcim_fw.so.0.9.0"
        ["libcim_fw.so"]="libcim_fw.so.0.9.0"
        ["libcom_err.so.3"]="libcom_err.so.3.0"
        ["libcom_err.so"]="libcom_err.so.3.0"
        ["libcrypt.so"]="libcrypt.so.1" 
        ["libcrypto.so"]="libcrypto.so.1.0.0"
        ["libcrypto.so.0.9.8"]="libcrypto.so.1.0.0"
        ["libcurl.so.4"]="libcurl.so.4.6.0"
        ["libcurl.so"]="libcurl.so.4.6.0"
        ["libdataacq.so.0"]="libdataacq.so.0.9.0"
        ["libdataacq.so"]="libdataacq.so.0.9.0"
        ["libdlmalloc.so.0"]="libdlmalloc.so.0.9.0"
        ["libdlmalloc.so"]="libdlmalloc.so.0.9.0"
        ["libfilehook.so.0"]="libfilehook.so.0.9.0"
        ["libfilehook.so"]="libfilehook.so.0.9.0"
        ["libglib-2.0.so.0"]="libglib-2.0.so.0.6800.1"
        ["libglib-2.0.so"]="libglib-2.0.so.0.6800.1"
        ["libgssapi_krb5.so"]="libgssapi_krb5.so.2"
        ["libgssapi_krb5.so.2"]="libgssapi_krb5.so.2.2"
        ["libgssrpc.so.4"]="libgssrpc.so.4.2"
        ["libgssrpc.so"]="libgssrpc.so.4"
        ["libhistory.so.5"]="libhistory.so.5.2"
        ["libhistory.so"]="libhistory.so.5.2"
        ["libjpeg.so.8"]="libjpeg.so.8.4.0"
        ["libjpeg.so"]="libjpeg.so.8"
        ["libjson-c.so.5"]="libjson-c.so.5.2.0"
        ["libjson-c.so"]="libjson-c.so.5.2.0"
        ["libk5crypto.so.3"]="libk5crypto.so.3.1"
        ["libk5crypto.so"]="libk5crypto.so.3"
        ["libkadm5clnt_mit.so.12"]="libkadm5clnt_mit.so.12.0"
        ["libkadm5clnt_mit.so"]="libkadm5clnt_mit.so.12"
        ["libkadm5clnt.so"]="libkadm5clnt_mit.so"
        ["libkadm5srv_mit.so.12"]="libkadm5srv_mit.so.12.0"
        ["libkadm5srv_mit.so"]="libkadm5srv_mit.so.12"
        ["libkadm5srv.so"]="libkadm5srv_mit.so"
        ["libkdb5.so.10"]="libkdb5.so.10.0"
        ["libkdb5.so"]="libkdb5.so.10"
        ["libkrad.so.0"]="libkrad.so.0.0"
        ["libkrad.so"]="libkrad.so.0"
        ["libkrb5.so.3"]="libkrb5.so.3.3"
        ["libkrb5.so"]="libkrb5.so.3"
        ["libkrb5support.so.0"]="libkrb5support.so.0.1"
        ["libkrb5support.so"]="libkrb5support.so.0"
        ["libldap-2.4.so.2"]="libldap-2.4.so.2.4.2"
        ["libldap.so"]="libldap-2.4.so.2"
        ["libmodbus.so.0"]="libmodbus.so.0.9.0"
        ["libmodbus.so"]="libmodbus.so.0"
        ["libnetsnmp.so.35"]="libnetsnmp.so.35.0.0"
        ["libnetsnmp.so"]="libnetsnmp.so.35"
        ["libnetsnmpagent.so.35"]="libnetsnmpagent.so.35.0.0"
        ["libnetsnmpagent.so"]="libnetsnmpagent.so.35"
        ["libnetsnmphelpers.so.35"]="libnetsnmphelpers.so.35.0.0"
        ["libnetsnmphelpers.so"]="libnetsnmphelpers.so.35"
        ["libnetsnmpmibs.so.35"]="libnetsnmpmibs.so.35.0.0"
        ["libnetsnmpmibs.so"]="libnetsnmpmibs.so.35"
        ["libnetsnmptrapd.so.35"]="libnetsnmptrapd.so.35.0.0"
        ["libnetsnmptrapd.so"]="libnetsnmptrapd.so.35"
        ["libnvme_vpd.so.0"]="libnvme_vpd.so.0.9.0"
        ["libnvme_vpd.so"]="libnvme_vpd.so.0.9.0"
        ["libpam_misc.so.0"]="libpam_misc.so.0.82.1"
        ["libpam_misc.so"]="libpam_misc.so.0.82.1"
        ["libpam_pme.so.0"]="libpam_pme.so.0.9.0"
        ["libpam_pme.so"]="libpam_pme.so.0.9.0"
        ["libpam_tally2.so.0"]="libpam_tally2.so.0.83.1"
        ["libpam_tally2.so"]="libpam_tally2.so.0.83.1"
        ["libpam.so.0"]="libpam.so.0.84.1"
        ["libpam.so"]="libpam.so.0.84.1"
        ["libpamc.so.0"]="libpamc.so.0.82.1"
        ["libpamc.so"]="libpamc.so.0.82.1"
        ["libpcie_mgmt.so.0"]="libpcie_mgmt.so.0.9.0"
        ["libpcie_mgmt.so"]="libpcie_mgmt.so.0.9.0"
        ["libpme_crypto.so.0"]="libpme_crypto.so.0.9.0"
        ["libpme_crypto.so"]="libpme_crypto.so.0.9.0"
        ["libsasl2.so.3"]="libsasl2.so.3.0.0"
        ["libsasl2.so"]="libsasl2.so.3.0.0"
        ["libsecure.so.0"]="libsecure.so.0.9.0"
        ["libsecure.so"]="libsecure.so.0.9.0"
        ["libsmbios.so.0"]="libsmbios.so.0.9.0"
        ["libsmbios.so"]="libsmbios.so.0.9.0"
        ["libsnmp_extern.so.0"]="libsnmp_extern.so.0.9.0"
        ["libsnmp_extern.so"]="libsnmp_extern.so.0.9.0"
        ["libsqlite3.so.0"]="libsqlite3.so.0.8.6"
        ["libsqlite3.so"]="libsqlite3.so.0.8.6"
        ["libssh2.so.1"]="libssh2.so.1.0.1"
        ["libssl_kvm.so"]="libssl.so.1.0.0"
        ["libssl.so"]="libssl.so.1.1"   #
        ["libssl.so.0.9.8"]="libssl.so.1.0.0" # TODO: rebuild pme/lib/libldap.so to fix it.
        ["libtestlib.so.0"]="libtestlib.so.0.9.0"
        ["libtestlib.so"]="libtestlib.so.0.9.0"
        ["libthreadutil.so.6"]="libthreadutil.so.6.0.4"
        ["libthreadutil.so"]="libthreadutil.so.6.0.4"
        ["libthrift_c_glib.so.0"]="libthrift_c_glib.so.0.0.0"
        ["libthrift_c_glib.so"]="libthrift_c_glib.so.0.0.0"
        ["libthrift.so"]="libthrift-0.9.0.so"
        ["libthriftz.so"]="libthriftz-0.9.0.so"
        ["libtransfer.so.0"]="libtransfer.so.0.9.0"
        ["libtransfer.so"]="libtransfer.so.0.9.0"
        ["libverto.so.0"]="libverto.so.0.0"
        ["libverto.so"]="libverto.so.0.0"
        ["libxml2.so.2"]="libxml2.so.2.7.7"
        ["libxml2.so"]="libxml2.so.2.7.7"
        ["libxmlparser.so.0"]="libxmlparser.so.0.9.0"
        ["libxmlparser.so"]="libxmlparser.so.0.9.0"
        ["libz.so.1"]="libz.so.1.2.8"
        ["libz.so"]="libz.so.1.2.8"
    )
    # 进入tools/others/dev_tools/llt/pme/lib/下重建软链接
    cd ${G_CODE_BASE}/tools/others/dev_tools/llt/pme/lib/ || exit 1
    for link in "${!LINK_MAPS[@]}"; do
        rm -f $link
        ln -s ${LINK_MAPS[$link]} $link
    done
    # 返回工作目录
    cd - || exit 1
}
init_pme_sdk() {
    # 建立必要的目录
    local dirs=("${G_OPEN_SDK_DIR}") 
    for i in "${dirs[@]}"; do
        if [[ -d $i ]]; then
            rm -r $i
        fi
        mkdir -p $i
    done

    # 文件准备
    tar -zxf "${G_OPEN_PM_DIR}/PME_SDK_x86_32.tar.gz" -C "${G_OPEN_SDK_DIR}"

    # 重建软链接
    rebuild_link_to_libs
}

function init_site_ai() {
    python3 download_from_cmc.py -b ${BOARD} -l
    site_ai_pkg_name=$(ls "${G_CODE_BASE}/temp/platforms/site_ai")
    site_data_pkg_name=$(ls "${G_CODE_BASE}/temp/platforms/site_data")
    site_ai_dir_name="${G_CODE_BASE}/temp/platforms/site_ai/${site_ai_pkg_name%.tar.gz}"
    site_data_dir_name="${G_CODE_BASE}/temp/platforms/site_data/${site_data_pkg_name%.tar.gz}"
    site_ai_ut_lib_service_dst_dir="${G_CODE_BASE}/application/src_bin/platforms/site_ai_ut"
    site_ai_ut_lib_llt_dst_dir="${G_CODE_BASE}/tools/others/dev_tools/llt/pme/lib"

    mkdir -p "${site_ai_dir_name}"
    mkdir -p "${site_data_dir_name}"

    tar -zxf "${G_CODE_BASE}/temp/platforms/site_ai/${site_ai_pkg_name}" -C "${site_ai_dir_name}"
    tar -zxf "${G_CODE_BASE}/temp/platforms/site_data/${site_data_pkg_name}" -C "${site_data_dir_name}"

    site_ai_lib_dir="${G_CODE_BASE}/application/src_bin/platforms/site_ai"
    site_data_lib_dir="${G_CODE_BASE}/application/src_bin/platforms/site_data"
    site_ai_include_dir="${G_CODE_BASE}/platforms/site_ai/include"
    site_data_include_dir="${G_CODE_BASE}/platforms/site_data/include"

    mkdir -p "${site_ai_lib_dir}"
    mkdir -p "${site_data_lib_dir}"
    mkdir -p "${site_ai_include_dir}"
    mkdir -p "${site_data_include_dir}"
    mkdir -p "${site_ai_ut_lib_service_dst_dir}"

    cp -f "${site_ai_dir_name}/tools/cml_converter/fbs/CML.fbs" "${site_ai_lib_dir}/CML.fbs"
    cp -rf "${site_ai_dir_name}/src/cml/rtos_arm32a55le/include/C" "${site_ai_include_dir}/cml"
    cp -rf "${site_data_dir_name}/src/cdf/rtos_arm32a55le/include/C" "${site_data_include_dir}/cdf"

    cp -f "${site_data_dir_name}/src/cdf/rtos_arm32a55le/lib/libsitedata_cdf.so" "${site_ai_ut_lib_service_dst_dir}/libsitedata_cdf.so"
    cp -f "${site_ai_dir_name}/src/cml/rtos_arm32a55le/lib/libsiteai_cml.so" "${site_ai_ut_lib_service_dst_dir}/libsiteai_cml.so"
    cp -f "${site_ai_dir_name}/src/cml/rtos_arm32a55le/lib/libsiteai_util.so" "${site_ai_ut_lib_service_dst_dir}/libsiteai_util.so"
    cp -f "${site_data_dir_name}/src/cdf/rtos_arm32a55le/lib/libsitedata_cdf.so" "${site_ai_ut_lib_llt_dst_dir}/libsitedata_cdf.so"
    cp -f "${site_ai_dir_name}/src/cml/rtos_arm32a55le/lib/libsiteai_cml.so" "${site_ai_ut_lib_llt_dst_dir}/libsiteai_cml.so"
    cp -f "${site_ai_dir_name}/src/cml/rtos_arm32a55le/lib/libsiteai_util.so" "${site_ai_ut_lib_llt_dst_dir}/libsiteai_util.so"
}

init_test_tree() {
    if [[ -d "${G_TEST_TREE_DIR}" ]]; then
        echo "clear ${G_TEST_TREE_DIR}"
        rm -rf ${G_TEST_TREE_DIR} >/dev/null 2>&1
    fi
    mkdir ${G_TEST_TREE_DIR}
}
init_test_runtime() {
    dirs=("/tmp/ramfs" "/opt/pme" "/data/etc/ssh/") # DT frame_work
    for i in ${dirs[@]}
    do
        if [[ -d $i ]];then
            rm -r $i
        fi
        mkdir -p $i
    done
    cp -a ${G_OPEN_SDK_DIR}/PME/V100R001C10/x86/* /opt/pme/

    # prepare runtime xml
    echo "init XMLs in ${G_TEST_TREE_DIR}"
    bash ${G_CURRENT_DIR}/unit_test/xml.sh ${BOARD}

    # 为平台资源打补丁
    bash ${G_CURRENT_DIR}/patch_for_platforms.sh
}
init_open_source_inc() {
    python3 include_move.py -o from_submodule 
}
init_env() {
    export LD_LIBRARY_PATH="/usr/local/pme/lib/"
    export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:${G_OPEN_SDK_DIR}/PME/V100R001C10/x86/lib/"
    export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:${G_TEST_TREE_DIR}/unit_test/opt/pme/lib/"

    init_open_source_inc
    init_pme_sdk
    init_test_tree
    init_test_runtime
    init_site_ai
}

insert_coverage_stub() {
    cd ${STATIC_INSERT_DIR}/func_monitor || exit 1
    if [[ $APP == "" ]]; then
        python func_monitor_insert.py all
    else
        python func_monitor_insert.py $APP
    fi
}

del_coverage_stub() {
    cd ${STATIC_INSERT_DIR}/func_monitor || exit 1
    if [[ $APP == "" ]]; then
        python func_monitor_del.py all
    else
        python func_monitor_del.py $APP
    fi
}

build() {
    # init
    if [[ $ENABLE_CACHE != 1 ]]; then
        init_env
    fi

    if [[ $ENABLE_COVERAG_STUB == 1 ]]; then
        del_coverage_stub
        insert_coverage_stub
    fi
    cmake_config || exit 1
    (make -j"$(nproc)" && make install) || exit 1
    if [[ $ENABLE_COVERAG_STUB == 1 ]]; then
        del_coverage_stub
    fi
}

cmake_config() {
    local cmake_dir="${G_CODE_BASE}/application"
    local cmake_para="-DCMAKE_TOOLCHAIN_FILE=${G_CMAKE_TOOL_CHAIN} -DCMAKE_BUILD_TYPE=DevTest"
    # make sure CMAKE_LIBRARY_PATH is right.
    export CMAKE_LIBRARY_PATH="/usr/local/lib"
    cmake_para="${cmake_para} -DCMAKE_INSTALL_PREFIX=${G_TEST_TREE_DIR}/unit_test"
    cmake_para="${cmake_para} -DMAKE_UT_ENABLE=on"
    cmake_para="${cmake_para} -DMAKE_UT_MODULES=${APP}"
    cmake_para="${cmake_para} -DG_DT_FRAMEWORK_BIN_PATH=${G_DT_FRAMEWORK_BIN_PATH}"
    
    # will inject to c++ code
    cmake_para="${cmake_para} -DG_DT_XML_BOARD_DIR=${G_DT_XML_BOARD_DIR}"
    cmake_para="${cmake_para} -DG_DT_RUNTIME_TEMPROOT_DIR=${G_DT_RUNTIME_TEMPROOT_DIR}"

    if [[ $ENABLE_FUZZ == 1 ]]; then
        cmake_para="${cmake_para} -DMAKE_FUZZ_ENABLE=on"
    fi

    if [[ $ENABLE_VERBOSE == 1 ]]; then
        cmake_para="${cmake_para} -DCMAKE_VERBOSE_MAKEFILE=on"
    fi

    if [[ $ENABLE_COVERAG_STUB == 1 ]]; then
        cmake_para="${cmake_para} -DUT_COVERAGE_STUB_ENABLE=ON"
    fi
    if [[ $G_FROM_SOURCE == "" ]]; then
        cmake_para="${cmake_para} -DFROM_SOURCE=false"
    fi
    cd ${G_TEST_TREE_DIR} || exit 1
    cmake ${cmake_para} ${cmake_dir}
}

analysis_commit()
{
    # 计算前后修改的文件，进而获取涉及的模块
    echo "Analysis modified apps from commits..."
    local test_app=$(git diff --stat=$G_WIDTH ${G_SINCE} ${G_UNTIL} | awk -v supported_apps="${G_SUPPORTED_APPS[*]}"    \
                                                -v dt_threshold="${G_DT_THRESHOLD}"                                     \
                                                'BEGIN {                                                                \
                                                    split(supported_apps, apps, " ");                                   \
                                                }                                                                       \
                                                {                                                                       \
                                                    if (match($1, /application\/src\/libs/)) {                          \
                                                        flag=1;                                                         \
                                                        exit;                                                           \
                                                    } else {                                                            \
                                                        match($1, /application\/src\/apps\/(.*)\/(inc|src)/, app);      \
                                                        for (idx in apps) {                                             \
                                                            if (app[1] == apps[idx]) {                                  \
                                                                changes[app[1]]=changes[app[1]]+$3;                     \
                                                            }                                                           \
                                                        }                                                               \
                                                    }                                                                   \
                                                }                                                                       \
                                                END {                                                                   \
                                                    if (flag == 1) {                                                    \
                                                        for (i in apps) {                                               \
                                                            print apps[i];                                              \
                                                        }                                                               \
                                                        exit;                                                           \
                                                    } else {                                                            \
                                                        for (modified_app in changes) {                                 \
                                                            if (changes[modified_app] >= dt_threshold) {                \
                                                                print modified_app;                                     \
                                                            }                                                           \
                                                        }                                                               \
                                                    }                                                                   \
                                                }')                                                        
    # 没有匹配的app时，test_app为空("")
    if [[ -z ${test_app} ]]; then
        APP=${G_APP_NONE}
        # 将None写入到文件
        echo ${APP} > ${G_DT_APPS_FILE}
    else
        echo -e "Apps to be tested:\n${test_app[@]}"
        # 写入到文件
        echo ${test_app[@]} > ${G_DT_APPS_FILE}

        # 循环拼接成cmake变量形式
        for app in ${test_app[@]}
        do
            APP=${app}";"${APP}
        done
    fi
}

usage() {
    echo "Usage:[ -a | --app APP ]"
    echo "      [ -b | --board BOARD ]"
    echo "      [ -c | --enable-cache ]"
    echo "      [ -v | --enable-verbose ]"
    echo "      [ --enable-fuzz ]"
    echo "      [ --enable-coverage-stub ]"
    exit 2
}
################################################################################
# 3. Run main function
# Set some default values:
BOARD="2288hv5"
ENABLE_CACHE=0
ENABLE_FUZZ=0
ENABLE_VERBOSE=0
ENABLE_COVERAG_STUB=0

PARSED_ARGUMENTS=$(getopt --options a:b:cv --longoptions app:,board:,enable-cache,enable-fuzz,enable-verbose,enable-coverage-stub -- "$@")
VALID_ARGUMENTS=$?
if [ "$VALID_ARGUMENTS" != "0" ]; then
    usage
fi

echo "PARSED_ARGUMENTS is $PARSED_ARGUMENTS"
eval set -- "$PARSED_ARGUMENTS"
while true; do
    case "$1" in
    -a | --app)
        input_app=$2
        shift 2
        ;;
    -b | --board)
        BOARD=$2
        shift 2
        ;;
    -c | --enable-cache)
        ENABLE_CACHE=1
        shift
        ;;
    -v | --enable-verbose)
        ENABLE_VERBOSE=1
        shift
        ;;
    --enable-fuzz)
        ENABLE_FUZZ=1
        shift
        ;;
    --enable-coverage-stub)
        ENABLE_COVERAG_STUB=1
        shift
        ;;
    --)
        shift
        break
        ;;
    *)
        echo "Unexpected option: $1 - this should not happen."
        usage
        ;;
    esac
done

# 打印当前使能DT门禁的app和输入app名称
echo "DT Supported Apps: ${G_SUPPORTED_APPS[@]}"
echo "input_app = ${input_app}"

# 1.输入参数为auto或空时，通过提交的commit差异来分析获得变更的模块
# 2.IDE容器场景需要用户通过rsync_exclude.list使能.git文件的同步，以便能够通过git分析差异
if [[ ${input_app} == "auto" || ${input_app} == "" ]]; then
    analysis_commit ${input_app}
    if [[ ${APP} == ${G_APP_NONE} ]]; then
        # 没有app需要测试时，清理test_tree路径，防止缓存引起test_UT.sh和test_lcov.sh运行异常
        init_test_tree
        echo "No apps need to be built, ${G_SCRIPT_NAME} exit normally"
        exit 0
    fi
else
    APP=${input_app}
    echo ${APP} > ${G_DT_APPS_FILE}
fi

echo "APP            : $APP"
echo "BOARD          : $BOARD"
echo "ENABLE_CACHE   : $ENABLE_CACHE"
echo "ENABLE_FUZZ    : $ENABLE_FUZZ"
echo "ENABLE_VERBOSE : $ENABLE_VERBOSE"
echo "ENABLE_COVERAG_STUB : $ENABLE_COVERAG_STUB"
echo "Parameters remaining are: $*"
if [[ $ENABLE_VERBOSE == 1 ]]; then
    set -x
fi

build
