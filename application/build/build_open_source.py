#!/usr/bin/env python
# coding: utf-8

"""
功 能：build_open_source，该脚本主要用来全量源码编译中编译开源软件及第三方软件
版权信息：华为技术有限公司，版本所有(C) 2019-2020
-a：指定位数 arch32(32位) 或 arch64(64位)
-l：是否进行下载(指定则使用本地主干代码构建)
-i：是否使用本地下载包(指定则使用本地下载包(未指定-l情况下指定-i，-i不生效))
-p：指定编译模块，具体编译模块请看V2R2_trunk/open_source/的build.sh与build64.sh
"""

import argparse
import shutil
import rtos
import os
import subprocess
from utils.common import Common
from utils.log import Logger
from utils.error_info import EXCEPTION_STAGE
from utils.error_info import EXCEPTION_RESPONSE
from utils.error_info import EXCEPTION_CODE



# parse option
parser = argparse.ArgumentParser(description = "used for parse build_open_source command")
parser.add_argument('-a', '--arch', default = 'all', choices= ["all", "arch32", "arch64"],
                    help= "Select the part to build")
parser.add_argument('-l', '--local', action='store_true',
                    help= "Do not execute upload!")
parser.add_argument('-i', '--inner', action='store_true',
                    help= "Used with --local to imcomplement not download")
parser.add_argument('-p', '--part_name', default = 'all', choices= [
    "json",
    "net_snmp",
    "openldap",
    "sqlite",
    "libjpeg",
    "libedit",
    "curl",
    "openssh",
    "storelib",
    "storelib3108",
    "storelibir3",
    "storelibit",
    "ntp",
    "syslog_ng",
    "krb5",
    "lldpd",
    "nginx",
    "luajit2"], help= "Due to dependencies, the following open-source software constructs mappings: linux_pam->OpenSSH,libssh2->curl,sasl->openldap"
)
args = parser.parse_args()

logger = Logger().logger

def set_global():
    global g_code_root
    global g_work_dir_default
    global g_flag_arm64
    global g_part_name
    global g_flag_arch
    global g_flag_opensource_only
    global g_flag_bin_update
    global g_version
    global g_open_source_dir
    global g_common
    global g_open_source_env_dir

    g_common = Common()
    g_code_root = g_common.code_root
    g_work_dir_default = g_common.opensource_vendor
    g_part_name = args.part_name
    g_flag_arch = args.arch
    g_version = "208.5.0"
    g_open_source_dir = g_common.open_source_path
    g_open_source_env_dir = f"{g_common.temp_path}/open_source_env_dir"

    if args.inner:
        g_flag_opensource_only = False
    else:
        g_flag_opensource_only = True

    if args.local:
        g_flag_bin_update = False
    else:
        g_flag_bin_update = True


def build():
    global g_rtos
    global g_open_source_bin_dir
    logger.info("set env begin...")

    if g_flag_opensource_only:
        os.chdir(g_common.code_path)
        # 下载开源软件包与RTOS
        subprocess.run("python3 download_from_cmc.py -o", shell=True)
        rtos_dir = g_common.rtos_path
        rtos_source = f"{g_common.temp_path}/build_tools/rtos-sdk"
        rtos_arm64_dir = g_common.rtos_arm64_path
        rtos_arm64_source = f"{g_common.temp_path}/build_tools/rtos-sdk-arm64"
        subprocess.run(f"rm {rtos_dir}/*.rpm", shell=True)
        subprocess.run(f"rm {rtos_arm64_dir}/*.rpm", shell=True)
        subprocess.run(f"yes y | cp -ai {rtos_source}/* {rtos_dir}", shell=True)
        subprocess.run(f"yes y | cp -ai {rtos_arm64_source}/* {rtos_arm64_dir}", shell=True)

    subprocess.run(f"find {g_common.code_root}/open_source -name 'configure' | xargs chmod +x", shell=True)

    subprocess.run(f"find {g_common.code_root}/open_source -name 'configure' | xargs chmod +x", shell=True)

    if g_flag_arm64 :
        g_rtos = rtos.Rtos(True)

    else:
        g_rtos = rtos.Rtos(False)

    g_rtos.install_rtos()

    if os.path.isdir(g_open_source_env_dir):
        g_common.read_file_set_env(f"{g_open_source_env_dir}/open_source_env.txt")
    else:
        err_msg = f"{EXCEPTION_CODE[600]}{EXCEPTION_STAGE['D']}{EXCEPTION_RESPONSE[1]}"
        logger.error(f"{err_msg}Set env failed!")
        os._exit(1)

    logger.info("Copy file to home, please wait for a minute!")
    g_open_source_bin_dir = g_rtos.g_open_source_bin_dir
    if not os.path.exists(g_rtos.g_libstdcpp):
        g_common.run_command(f"cp {g_rtos.g_lib_path}/libstdc++.*  {g_rtos.g_libstdcpp_install_path}/", "cp libstdc++* ERR!")

    if not os.path.isdir(g_work_dir_default):
        os.makedirs(g_work_dir_default)
    os.chdir(g_work_dir_default)
    g_common.remove_path("open_source")
    g_common.remove_path("vendor")
    shutil.copytree(os.path.join(g_code_root, "open_source"),
                    os.path.join(g_work_dir_default, "open_source"), symlinks=True)
    g_common.copy_all(os.path.join(g_common.temp_path, "open_source"),
                             os.path.join(g_work_dir_default, "open_source"))
    shutil.copytree(os.path.join(g_code_root, "vendor"),
                    os.path.join(g_work_dir_default, "vendor"))
    g_common.copy_all(os.path.join(g_common.temp_path, "vendor"),
                             os.path.join(g_work_dir_default, "vendor"))

    os.chdir(os.path.join(g_work_dir_default, "vendor/StoreLibIR3"))
    libir3 = os.listdir(os.path.join(g_work_dir_default, "vendor/StoreLibIR3"))
    g_common.del_head(libir3, "SAS3_")

    os.chdir(os.path.join(g_work_dir_default, "vendor/StoreLib"))
    libir = os.listdir(os.path.join(g_work_dir_default, "vendor/StoreLib"))
    g_common.del_head(libir, "MR_")

    os.chdir(g_common.code_path)
    logger.info("sh real_build_open_source.sh begin...")
    subprocess.run(f"sh real_build_open_source.sh {g_version} {g_rtos.g_cpu} {g_rtos.g_kernel} \
    {g_rtos.g_cross_compile_install_path} {g_work_dir_default} {g_rtos.g_arg_flag_arm64} {g_part_name}", shell=True)
    logger.info("sh real_build_open_source.sh end!")
    os.chdir(g_common.code_path)

    logger.info("set env end! ")


def update():
    # 将二进制复制到src_bin
    logger.info("update bin begin...")
    os.chdir(os.path.join(g_code_root, "application/src_bin"))
    g_common.remove_path("open_source")
    g_common.remove_path("vendor")

    # 更新到BMC 代码目录用以编译
    g_common.copy_all(os.path.join(g_work_dir_default, "output"), os.path.join(g_code_root, "application/src_bin") )
    g_common.remove_path("third_party_groupware")
    if not os.path.exists("third_party_groupware"):
        os.rename("vendor", "third_party_groupware")

    # 处理syslog软链接
    if os.path.isdir(os.path.join(g_open_source_bin_dir, "syslog-ng")) and not g_flag_arm64:
        os.chdir(os.path.join(g_open_source_bin_dir, "syslog-ng"))
        g_common.copy("libevtlog-3.36.so.0.0.0", "libevtlog.so")
        g_common.copy("libsecret-storage.so.0.0.0", "libsecret-storage.so")
        g_common.copy("libsyslog-ng-3.36.so.0.0.0", "libsyslog-ng.so")

    # 更新二进制库上传用目录
    subprocess.run(f"cp -ar {g_open_source_dir}/* {g_common.temp_path}/bin_cmc/application/src_bin/open_source", shell=True)
    subprocess.run(f"cp -ar {g_common.third_party_groupware_path}/* {g_common.temp_path}/bin_cmc/application/src_bin/third_party_groupware", shell=True)

    if g_flag_bin_update:
        logger.info("python3 upload_to_cmc.py begin...")
        os.chdir(g_common.code_path)
        g_common.run_py_command("upload_to_cmc.py", "upload_to_cmc ERR!")
        logger.info("python3 upload_to_cmc.py end!")

    logger.info("update bin end!")

if __name__ == "__main__":
    set_global()

    if g_flag_arch == "all":
        # 编译32位开源软件
        g_flag_arm64 = False
        build()

        # 编译64位开源软件
        g_flag_arm64 = True
        g_flag_opensource_only = False
        build()
        update()
        os._exit(0)
    else:
        if g_flag_arch == "arch64":
            g_flag_arm64 = True

        elif g_flag_arch == "arch32":
            g_flag_arm64 = False

        build()
        update()


