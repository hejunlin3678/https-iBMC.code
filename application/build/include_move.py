#!/usr/bin/env python
#coding=utf-8

"""
文件名：include_move.py
功能：消解include目录
版权信息：华为技术有限公司，版本所有(C) 2019-2020
"""

import os
import sys
import shutil
import argparse
import subprocess

from utils.log import Logger
from utils.common import Common
from utils.error_info import EXCEPTION_STAGE
from utils.error_info import EXCEPTION_RESPONSE
from utils.error_info import EXCEPTION_CODE

parser = argparse.ArgumentParser(description = "Excute include_move.py")
parser.add_argument("-o", "--option", help = "Fill in option", default = "")
args = parser.parse_args()

logger = Logger().logger
def set_global():
    """
    设置全局变量
    """
    global g_common
    global g_current_dir
    global g_current_v2r2_trunk
    global g_work_dir_default
    global g_option
    global g_luajit_header
    g_common = Common()
    g_current_dir = g_common.code_path
    g_current_v2r2_trunk = g_common.code_root
    g_work_dir_default = f"{g_common.opensource_vendor}/open_source"
    g_option = args.option
    g_luajit_header = f"{g_common.third_path}/include"
    return

def check_dir(dir_name):
    """
    确认目录是否存在，存在则删除重建，不存在则创建
    """
    if os.path.isdir(dir_name):
        shutil.rmtree(dir_name)
        os.makedirs(dir_name)
    else:
        os.makedirs(dir_name)
    return

def copy_file_or_dir(src_dir, dst_dir):
    subprocess.run(f"cp -a {src_dir} {dst_dir}", shell=True)
    return

def headers_install_patch(dst_file, patch_file):
    """
    简化headers打补丁操作减少重复代码
    """
    path_main_dir = f"{g_current_v2r2_trunk}/application/src/include/open_source"
    subprocess.run(f"patch {path_main_dir}/{dst_file} {g_current_v2r2_trunk}/{patch_file}", shell=True)
    return

def patch_headers():
    """
    针对BMC的app编译进行patch操作
    """
    headers_install_patch("krb5/k5-int.h", "open_source/Kerberos_5/huawei_k5-int.h.patch")
    headers_install_patch("krb5/k5-plugin.h", "open_source/Kerberos_5/huawei_k5-plugin.h.patch")
    headers_install_patch("jmorecfg.h", "open_source/libjpeg/huawei_jmorecfg.h.patch")
    return

def open_source_inc_mv():
    """
    说明：
    部分开源软件头文件在源码包中不存在，在编译后生成，
    故此函数需在一键式源码编译后调用，将源码编译的结果中的头文件取出，
    放至 application/src/include/open_source 中。
    历史遗留问题尚未解决：
    存在冗余头文件，需待深入审视代码中是否用到；
    开源软件之间、与第三方软件之间头文件混合存放；
    存放路径不规范，难以溯源，导致CMake配置的引用关系混乱；
    pme_app.h承载了大部分头文件的引用关系
    """
    curl = f"{g_work_dir_default}/curl"
    openldap = f"{g_work_dir_default}/OpenLDAP"
    openssh = f"{g_work_dir_default}/openssh"
    json = f"{g_work_dir_default}/json-c"
    krb5 = f"{g_work_dir_default}/Kerberos_5"
    net_snmp = f"{g_work_dir_default}/net-snmp"
    sasl = f"{g_work_dir_default}/Cyrus_SASL"
    sqlite = f"{g_work_dir_default}/SQLite"
    linux_pam = f"{g_work_dir_default}/Pam"
    jpeg = f"{g_work_dir_default}/libjpeg"

    # 固定目录及文件，防止被修改
    open_source_dir = (
        'open_source/curl',
        'open_source/json',
        'open_source/krb5',
        'open_source/krb5/gssapi',
        'open_source/krb5/rcache',
        'open_source/net-snmp',
        'open_source/net-snmp/agent',
        'open_source/net-snmp/library',
        'open_source/net-snmp/machine',
        'open_source/net-snmp/system',
        'open_source/raid',
        'open_source/raid/lsi',
        'open_source/sasl',
        'open_source/security',
        'open_source/luajit2'
    )
    # 变量 g_work_dir_default 关联到
    # V2R2_trunk/open_source/build.sh中规定的编译开源软件的决定路径
    if os.path.exists(f"{g_work_dir_default}/headers"):
        shutil.rmtree(f"{g_work_dir_default}/headers")
    os.makedirs(f"{g_work_dir_default}/headers")
    os.chdir(f"{g_work_dir_default}/headers")

    # 在新建的目录headers中，把编译后的头文件复制到一个临时的open_source目录中，
    # 其目录结构在 OPEN_SOURCE_DIR 中描述
    if os.path.isdir("open_source"):
        shutil.rmtree("open_source")
    if os.path.isdir(f"{g_current_v2r2_trunk}/application/src/inc/open_source"):
        shutil.rmtree(f"{g_current_v2r2_trunk}/application/src/inc/open_source")
    os.makedirs("open_source")
    for dir in open_source_dir:
        check_dir(dir)
    
    copy_file_or_dir(f"{curl}/include/curl/*.h", "./open_source/curl")
    copy_file_or_dir(f"{json}/*.h", "./open_source/json")
    copy_file_or_dir(f"{json}/build/json_config.h", "./open_source/json")
    os.remove("open_source/json/math_compat.h")
    os.remove("open_source/json/snprintf_compat.h")
    os.remove("open_source/json/strdup_compat.h")
    os.remove("open_source/json/strerror_override.h")
    os.remove("open_source/json/strerror_override_private.h")
    os.remove("open_source/json/vasprintf_compat.h")
    shutil.copy(f"{krb5}/src/include/socket-utils.h", "./open_source/krb5")
    shutil.copy(f"{krb5}/src/include/profile.h", "./open_source/krb5")
    shutil.copy(f"{krb5}/src/include/port-sockets.h", "./open_source/krb5")
    shutil.copy(f"{krb5}/src/include/osconf.h", "./open_source/krb5")
    shutil.copy(f"{krb5}/src/include/k5-thread.h", "./open_source/krb5")
    shutil.copy(f"{krb5}/src/include/k5-trace.h", "./open_source/krb5")
    shutil.copy(f"{krb5}/src/include/k5-queue.h", "./open_source/krb5")
    shutil.copy(f"{krb5}/src/include/k5-plugin.h", "./open_source/krb5")
    shutil.copy(f"{krb5}/src/include/k5-platform.h", "./open_source/krb5")
    shutil.copy(f"{krb5}/src/include/k5-int-pkinit.h", "./open_source/krb5")
    shutil.copy(f"{krb5}/src/include/k5-int.h", "./open_source/krb5")
    shutil.copy(f"{krb5}/src/include/k5-gmt_mktime.h", "./open_source/krb5")
    shutil.copy(f"{krb5}/src/include/k5-err.h", "./open_source/krb5")
    shutil.copy(f"{krb5}/src/include/k5-buf.h", "./open_source/krb5")
    shutil.copy(f"{krb5}/src/include/k5-base64.h", "./open_source/krb5")
    shutil.copy(f"{krb5}/src/include/autoconf.h", "./open_source/krb5")
    shutil.copy(f"{krb5}/src/include/krb5/plugin.h", "./open_source/krb5")
    shutil.copy(f"{krb5}/src/include/krb5/krb5.h", "./open_source/krb5")
    shutil.copy(f"{krb5}/src/include/krb5/authdata_plugin.h", "./open_source/krb5")
    shutil.copy(f"{krb5}/src/lib/krb5/rcache/memrcache.h", "./open_source/krb5/rcache")
    shutil.copy(f"{krb5}/src/include/gssapi/gssapi_krb5.h", "./open_source/krb5/gssapi")
    shutil.copy(f"{krb5}/src/include/gssapi/gssapi_generic.h", "./open_source/krb5/gssapi")
    shutil.copy(f"{krb5}/src/include/gssapi/gssapi_ext.h", "./open_source/krb5/gssapi")
    shutil.copy(f"{krb5}/src/include/gssapi/gssapi_alloc.h", "./open_source/krb5/gssapi")
    shutil.copy(f"{krb5}/src/include/gssapi/gssapi.h", "./open_source/krb5/gssapi")
    shutil.copy(f"{krb5}/src/lib/krb5/krb/auth_con.h", "./open_source/krb5/gssapi")
    shutil.copy(f"{krb5}/src/lib/gssapi/mechglue/mglueP.h", "./open_source/krb5/gssapi")
    shutil.copy(f"{krb5}/src/lib/gssapi/mechglue/mechglue.h", "./open_source/krb5/gssapi")
    shutil.copy(f"{krb5}/src/lib/gssapi/spnego/gssapiP_spnego.h", "./open_source/krb5/gssapi")
    shutil.copy(f"{krb5}/src/lib/gssapi/spnego/gssapiP_negoex.h", "./open_source/krb5/gssapi")
    shutil.copy(f"{krb5}/src/lib/gssapi/krb5/gssapiP_krb5.h", "./open_source/krb5/gssapi")
    shutil.copy(f"{krb5}/src/lib/gssapi/krb5/gssapi_err_krb5.h", "./open_source/krb5/gssapi")
    shutil.copy(f"{krb5}/src/lib/gssapi/generic/gssapiP_generic.h", "./open_source/krb5/gssapi")
    shutil.copy(f"{krb5}/src/lib/gssapi/generic/gssapi_err_generic.h", "./open_source/krb5/gssapi")
    shutil.copy(f"{linux_pam}/modules/pam_tally2/tallylog.h", "./open_source/security")
    copy_file_or_dir(f"{net_snmp}/include/net-snmp/*", "./open_source/net-snmp")
    shutil.copy(f"{net_snmp}/agent/mibgroup/mib_module_includes.h", "./open_source/net-snmp/agent")
    shutil.copy(f"{net_snmp}/agent/mibgroup/util_funcs.h", "./open_source/net-snmp/agent")
    shutil.rmtree("./open_source/net-snmp/agent/hardware")
    shutil.rmtree("./open_source/net-snmp/data_access")
    os.remove("./open_source/net-snmp/library/README")
    os.remove("./open_source/net-snmp/net-snmp-config.h.in")
    shutil.copy(f"{openldap}/include/lber_types.h", "./open_source")
    shutil.copy(f"{openldap}/include/lber.h", "./open_source")
    shutil.copy(f"{openldap}/include/ldap_cdefs.h", "./open_source")
    shutil.copy(f"{openldap}/include/ldap_features.h", "./open_source")
    shutil.copy(f"{openldap}/include/ldap_schema.h", "./open_source")
    shutil.copy(f"{openldap}/include/ldap.h", "./open_source")
    copy_file_or_dir(f"{sasl}/include/*.h", "./open_source/sasl")
    shutil.copy(f"{sqlite}/sqlite3.h", "./open_source")
    shutil.copy(f"{sqlite}/sqlite3ext.h", "./open_source")
    shutil.copy(f"{jpeg}/jconfig.h", "./open_source")
    shutil.copy(f"{jpeg}/jerror.h", "./open_source")
    shutil.copy(f"{jpeg}/jmorecfg.h", "./open_source")
    shutil.copy(f"{jpeg}/jpeglib.h", "./open_source")
    luajit_list = g_common.py_find_dir("lua.*", g_luajit_header)
    for luajit_file in luajit_list:
        shutil.copy(luajit_file, "./open_source/luajit2")
    shutil.copy(f"{g_luajit_header}/lauxlib.h", "./open_source/luajit2")

    # StoreLib 头文件
    os.chdir(f"{g_work_dir_default}/../vendor")
    os.chdir("StoreLibIR3/versionChangeSet/storelibir3_bmc_rel/")
    os.chdir("storelib_src_rel/fw/fw_sas_mega_api")
    copy_file_or_dir("./*", f"{g_work_dir_default}/headers/open_source/raid/lsi/")

    # 回到目录 StoreLibIT/versionChangeSet/storelib_unf_rel/storelibit_oob_src/app_util/storelib_unf/src/common/include/inc_mpi
    os.chdir(f"{g_work_dir_default}/../vendor")
    os.chdir("StoreLibIT/versionChangeSet/storelib_unf_rel/")
    os.chdir("storelibit_oob_src/app_util/storelib_unf/src/common/include/inc_mpi")
    shutil.copy("mpi2_cnfg.h", f"{g_work_dir_default}/headers/open_source/raid/lsi")
    shutil.copy("mpi2.h", f"{g_work_dir_default}/headers/open_source/raid/lsi")

    os.chdir(f"{g_work_dir_default}/../vendor")
    os.chdir("StoreLibIR3/versionChangeSet/storelibir3_bmc_rel/storelib_src_rel/app_util/common_storeLib_hdr/")
    copy_file_or_dir("./*", f"{g_work_dir_default}/headers/open_source/raid/lsi/")

    os.chdir(f"{g_work_dir_default}/../vendor")
    os.chdir("StoreLib/versionChangeSet/common_storelib_unf_rel/")
    os.chdir("MR_StoreLib_Unified_OOB_SourceCode/app_util/common_storelib_unf/src/oob/include/")
    shutil.copy("scsi-t10.h", f"{g_work_dir_default}/headers/open_source/raid/lsi/")

    os.chdir(f"{g_work_dir_default}/../vendor")
    os.chdir("StoreLib/versionChangeSet/common_storelib_unf_rel/")
    os.chdir("MR_StoreLib_Unified_OOB_SourceCode/fw/fw_sas_mega_api/")
    # 强制复制， 否则权限覆盖报错
    subprocess.run(f"cp -f eventmsg.h {g_work_dir_default}/headers/open_source/raid/lsi/", shell=True)

    # 移动到代码目录中
    os.chdir(f"{g_work_dir_default}/headers")
    g_common.copy_all("./open_source", f"{g_current_v2r2_trunk}/application/src/include/open_source")
    os.chdir(g_current_v2r2_trunk)
    shutil.rmtree(f"{g_work_dir_default}/headers")

    return

def get_cs4343_inc():
    g_common.check_path(f"{g_current_v2r2_trunk}/application/src/include/pme_app/cs4343")
    g_common.copy_all(f"{g_current_v2r2_trunk}/vendor/CS4343_PHY_API/inc",
                      f"{g_current_v2r2_trunk}/application/src/include/pme_app/cs4343")
    g_common.copy_all(f"{g_current_v2r2_trunk}/vendor/CS4343_PHY_API/src",
                      f"{g_current_v2r2_trunk}/application/src/libs/cs4343/src")

if __name__ == "__main__":
    ###### 开始执行 ######
    set_global()
    os.chdir(g_current_v2r2_trunk)
    # 2. ci 目录已经修改到使用子仓中的 include/ci
    # 3. open_source
    logger.info(f"##############move open source inc {g_option} begin ##################")
    if g_option == "from_submodule":
        # 3.1 open_source 头文件从子仓获得
        logger.info("moving from submodule...")
        subprocess.run(f"cp -af {g_current_v2r2_trunk}/tools/others/other_tools/open_source {g_current_v2r2_trunk}/application/src/include/", shell=True)
    elif g_option == "from_source_build":
        # 3.2 一键式源码构建时
        # 与 open_source/build.sh 中的变量一致
        open_source_inc_mv()
        patch_headers()
    else:
        err_msg = f"{EXCEPTION_CODE[701]}{EXCEPTION_STAGE['D']}{EXCEPTION_RESPONSE[4]}"
        logger.error(f"{err_msg}USAGE:python3 {sys.argv[0]} -o from_submodule | from_source_build")
        os._exit(1)

    get_cs4343_inc()
    
    #4. AdaptiveLM: 直接从 CMC 上下载
    logger.info(f"##############move open source inc {g_option} end ##################")

