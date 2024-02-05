#!/usr/bin/env python
# coding: utf-8
# Copyright © Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.

import json
import os
import signal
import sys
import time
import logging
import re
import stat
import subprocess
import shutil
import tarfile
import zipfile
import traceback
import yaml
from .log import Logger
from .error_info import EXCEPTION_STAGE
from .error_info import EXCEPTION_RESPONSE
from .error_info import EXCEPTION_CODE
import xml.etree.ElementTree as ET
import random
from string import Template

class Common :
    """
    功能描述：公共类定义全局变量,公共函数
    接口：无
    """
    # 脚本目录
    code_path = os.path.join(os.path.dirname(__file__), "..")
    code_path = os.path.realpath(code_path)
    pme_dir = os.path.realpath(f"{code_path}/..")

    # V2R2_trunk目录
    code_root = os.path.realpath(os.path.join(code_path, '../..'))

    #jenkins工作区并发数
    jenkins_executors = code_root.split('@')
    if len(jenkins_executors) < 2:
        jenkins_executors = '1'
    else:
        jenkins_executors = str(code_root.split('@')[1].split('/')[0])

    # 非root用户及组的配置
    non_root_user = "huawei"
    non_root_group = "user"
    non_root_ug = f"{non_root_user}:{non_root_group}"

    #loop设备
    loop = str("loop" + jenkins_executors)
    loop_dev = "/dev/"+loop

    # install目录
    output_path = os.path.join(code_root, 'output')

    # build目录
    temp_path = os.path.join(code_root, 'temp')
    work_out = os.path.join(temp_path, 'output')
    ci_package = os.path.join(work_out, 'CI_Package')
    open_code_root = os.path.join(temp_path, 'build_open', 'V2R2_trunk')

    # pme全量构建目录
    pme_full_src_path = os.path.join(temp_path, "full_source")

    # tools目录
    tools_path = os.path.join(code_root, 'tools')

    # 二进制目录
    internal_release_path = os.path.join(code_root, 'internal_release')
    bin_cmc_path = os.path.join(code_root, 'bin_cmc')

    # PME目录
    platform_v5_path = os.path.join(code_root, 'application/platform_v5')
    platform_v6_path = os.path.join(code_root, 'application/platform_v6')
    platform_path = os.path.join(code_root, 'application/platform')

    # AdaptiveLM目录
    adaptivelv_path = os.path.join(bin_cmc_path, 'application/src_bin/platforms/AdaptiveLM')
    adaptivelv_path_arm64 = os.path.join(bin_cmc_path, 'application/src_bin/platforms/arm64/AdaptiveLM')

    # FusioniRM目录
    asset_locator_path = os.path.join(bin_cmc_path, 'application/src_bin/rootfs/opt/pme/conf/asset_locator')

    # cmake目录
    cmake_path = os.path.join(bin_cmc_path, 'tools/build_tools/cmake')

    # java-linux-64目录
    java_linux_path = os.path.join(bin_cmc_path, 'tools/build_tools/java-linux-64')

    # python目录
    python_path = os.path.join(bin_cmc_path, 'tools/build_tools/python')

    # fpod_hinic_dir目录
    fpod_hinic_dir = os.path.join(bin_cmc_path, 'application/src/drivers/1822')

    # BiSheng CPU and RTOS目录
    rtos_path = os.path.join(code_root, 'tools/build_tools/rtos-sdk')
    rtos_arm64_path = os.path.join(code_root, 'tools/build_tools/rtos-sdk-arm64')

    # kvm_client目录
    kvm_client_linux = os.path.join(code_root, 'vendor/bin/kvm_client/jre_linux')
    kvm_client_mac = os.path.join(code_root, 'vendor/bin/kvm_client/jre_mac')
    kvm_client_windows = os.path.join(code_root, 'vendor/bin/kvm_client/jre_windows')

    # v3 component目录
    manifest_path = os.path.join(code_root, "../manifest")
    v3_component_dir = os.path.join(manifest_path, "temp/debug/tmp_root")

    # src_bin相关目录
    src_bin = os.path.join(code_root, 'application/src_bin')
    lsw_path = os.path.join(src_bin, 'lsw')
    open_source_path = os.path.join(src_bin, 'open_source')
    third_party_groupware_path = os.path.join(src_bin, 'third_party_groupware')

    # 前端资源相关目录
    data = os.path.join(code_root, 'application/rootfs/data')
    src_data_web = os.path.join(data, 'opt/pme/web')

    # rootfs目录
    rootfs = os.path.join(code_root, 'application/rootfs/opt/pme')
    src_bin_rootfs = os.path.join(src_bin, 'rootfs/opt/pme')

    # kvm相关jar包目录
    kvm_jar_path = os.path.join(src_bin_rootfs, "web/htdocs/bmc/pages/jar")

    # lib_open_source目录
    lib_open_source = os.path.join(code_root, "application/src/lib_open_source")

    # OEM 定制化修改, 代码的根目录
    # OEM 目录结构应保证如下:
    # ./OEM_Code
    # ├── build                 OEM的build_tree
    # │   └── V2R2_trunk        本代码目录
    # └── output                OEM的install_tree
    oem_code_root =  os.path.realpath(os.path.join(code_root, '../..'))
    oem_config_path = os.path.realpath(os.path.join(temp_path, './OEM'))

    # 定义DFT_ENABLED编译宏编译的app压缩包
    app_strip = os.path.join(work_out, "app_strip.tar.gz")

    # 未定义DFT_ENABLED编译宏编译的app压缩包
    release_app_strip = os.path.join(work_out, "app_release_strip.tar.gz")

    # language.hmp 目录
    language_dir = os.path.join(temp_path, 'language')

    # 打包路径目录
    gpp_rootfs_bin_build_dir = os.path.join(temp_path, 'gpp_rootfs_bin_build')
    gpp_bin_build_dir = os.path.join(temp_path, 'gpp_bin_build')
    hpm_build_dir = os.path.join(temp_path, 'hpm_build')

    # log集中目录
    log_manage_dic = {
        "tiny_log": f"{temp_path}/log/tiny_log",
        "download_log": f"{temp_path}/log/download_log",
        "npm_log": f"{temp_path}/log/npm_log",
        "open_source_log": f"{temp_path}/log/open_source_log",
        "packet_log": f"{temp_path}/log/packet_log",
        "cmcDown_log": f"{temp_path}/log/cmcDown_log"
    }

    # 实例化日志对象
    logger = Logger().logger

    compability_enable = 0
    security_enhanced_compatible_board_v1 = 0
    security_enhanced_compatible_board_v2 = 0
    security_enhanced_compatible_board_v3 = 0
    security_enhanced_compatible_board_v4 = 0
    security_enhanced_compatible_board_v5 = 0
    board_with_ebmc_list = 0

    print_cnt = 0

    opensource_vendor = "/home/public/workspace/application/"
    third_path = "/home/3rd"

    rtos_version = "208.5.0"
    rtos_dlsetenv = f"/opt/RTOS/{rtos_version}/dlsetenv.sh"
    revision = ""

    def get_hcc_path(self, board_name = "", version = ""):
        if version == "":
            version = self.get_board_version(board_name)
        if version == "V6":
            return "/opt/hcc_arm64le"
        else:
            return "/opt/hcc_arm32le"

    def get_rtos_path(self, board_name = "", version = ""):
        if version == "":
            version = self.get_board_version(board_name)
        if version == "V6":
            return f"/opt/RTOS/{self.rtos_version}/arm64le_5.10_ek_preempt_pro"
        else:
            return f"/opt/RTOS/{self.rtos_version}/arm32A9le_5.10_ek_preempt_pro"

    def __init__(self):
        self.create_log_content()
        # PSS验签功能初始化
        self.sign_pkcs_board_list = set(map(lambda x:x.strip(), open(os.path.join(Common.code_path, 'package_sign/sign_pkcs_board_list'))))
        self.sign_pss_board_list = set(map(lambda x:x.strip(), open(os.path.join(Common.code_path, 'package_sign/sign_pss_board_list'))))
        # PSS签名需要下载crl文件，这里先准备好，放在crl_path
        self.crl_path = os.path.join(Common.code_path, 'package_sign/crldata.crl')
        if not os.path.exists(self.crl_path):
            p1 = subprocess.run(f'wget --retry-connrefused --waitretry=1 -t 3 -O {self.crl_path} "https://download.huawei.com/dl/download.do?actionFlag=download&nid=PKI1100000074&partNo=3001&mid=SUP_PKI"', shell=True)
            p1.check_returncode()

    def get_support_dft_separation_flag(self, board_name):
        """
        功能描述：判断单板是否支持准备分离
        参数：单板名称
        """
        compile_board_lists = ["1288hv6_2288hv6_5288v6", "RM210", "RM210_SECURITY_3V0", "RM211", "S920S03", "TaiShan2480v2Pro", "SMM", "CM221", "CN221", "CN221V2", "CN221SV2", "CM221S",
                                "PangeaV6_Atlantic", "PangeaV6_Atlantic_Smm", "PangeaV6_Pacific", "PangeaV6_Arctic", "TaiShan2280v2Pro", "DA121Cv2", "DA122Cv2", "DP1210_DP2210_DP4210", "BM320", "BM320_V3", "Atlas200I_SoC_A1",
                                "CST1020V6", "CST1220V6", "CST0210V6", "CST0220V6", "CST0221V6", "SMM0001V6", "PAC1220V6", "S902X20", "Atlas900_PoD_A2", "Atlas800D_G1", "Atlas800D_RM"]
        support_dft_separation = False
        if board_name in compile_board_lists:
            support_dft_separation = True
        return support_dft_separation
    
    def get_support_dft_version_separation_flag(self, board_name):
        """
        功能描述：判断单板是否支持装备包版本分离
        参数：单板名称
        """
        compile_board_lists = ["RM210", "RM210_SECURITY_3V0", "RM211", "1288hv6_2288hv6_5288v6", "S920S03", "TaiShan2480v2", "TaiShan2480v2Pro", "DA121Cv2", "DA122Cv2", "PAC1220V6"]
        support_dft_version_separation = False
        if board_name in compile_board_lists:
            support_dft_version_separation = True
        return support_dft_version_separation

    def get_PIDspeed_regulation_flag(self, board_name):
        """
        功能描述：判断单板是否支持个性化PID调速设置
        参数：单板名称
        """
        board_lists = ["TaiShan2480v2Pro"]
        PIDspeed_regulation = False
        if board_name in board_lists:
            PIDspeed_regulation = True
        return PIDspeed_regulation

    def get_sign_algorithm_type(self, board_name):
        """
        功能描述：签名算法的配置，目前只有两种：PKCS，PSS
        参数：单板名称
        """
        if board_name in self.sign_pss_board_list:
            return 'PSS'
        elif board_name in self.sign_pkcs_board_list:
            return 'PKCS'
        else:
            # 默认使用PSS
            return 'PSS'

    def create_sign_filelist(self, board_name, tmp_name = ""):
        """
        功能描述：0502包生成签名目录xml文件
        参数：单板名称,临时文件名称,在sign_hpm_pkg.sh中使用
        """
        if tmp_name == "":
            file_log = f"{self.work_out}/{board_name}_vercfg.xml"
        else:
            file_log = f"{self.work_out}/{board_name}_{tmp_name}_vercfg.xml"
 
        if os.path.exists(file_log):
            os.remove(file_log)
        with open(file_log, "w") as file_log_file:
            file_log_file.write("<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n")
            file_log_file.write("<Product>\n")
            file_log_file.write("<Type>IVS</Type>\n")
            file_log_file.write("<Version>1.0</Version>\n")
            file_log_file.close()
 
        file_name_list = self.list_all_file(".*", ".", 1)
        file_size_list = []
        for f in file_name_list:
            file_size_list.append(os.stat(f, follow_symlinks=False).st_size)
 
        file_list_len = len(file_size_list)
        for i in range(file_list_len):
            if file_size_list[i] == 0:
                continue
            if os.path.isdir(file_name_list[i]):
                continue
            with open(file_log, "a") as file_log_file:
                file_log_file.write("<File>\n")
                file_log_file.write(f"<FilePath>{file_name_list[i]}</FilePath>\n")
                sha256 = self.get_shell_command_result(f"sha256sum {file_name_list[i]} | awk '{{print $1}}'")
                file_log_file.write(f"<SHAValue>{sha256}</SHAValue>\n")
                file_log_file.write("</File>\n")
                file_log_file.close()
        with open(file_log, "a") as file_log_file:
            file_log_file.write("</Product>\n")
            file_log_file.close()
        return

    def get_support_security_policy(self, board_name):
        """
        功能描述：判断单板是否支持安全检查功能
        参数：单板名称
        """
        compile_board_lists = ["BM320", "TaiShan2280Mv2", "DA122Cv2", "Atlas200I_SoC_A1", "Atlas800D_G1", "Atlas800D_RM"]
        support_security_policy = False
        if board_name in compile_board_lists:
            support_security_policy = True
        return support_security_policy

    def copy_pss_crl(self, dest_path):
        self.logger.info(f"################PSS cp {self.crl_path} {dest_path}")
        p1 = subprocess.run(f"cp {self.crl_path} {dest_path}", shell=True)
        p1.check_returncode()

    def is_pss_uboot(self, board_name):
        """
        功能描述：判断单板是否用pss签名证书uboot 过渡接口 过渡完成后与 get_sign_algorithm_type 保持一致
        参数：单板名称
        """
        board_list = ["BM320", "BM320_V3", "DA122Cv2", "Atlas200I_SoC_A1", "2288xv5_1711", "TaiShan2280Mv2", "TaiShan2280v2_1711", 
            "TaiShan2480v2Pro", "TaiShan2480v2", "S920X05", "S920S03", "TaiShan2280v2Pro", "TaiShan5290v2", "TaiShan2280Ev2", "Atlas800D_G1", "Atlas800D_RM"]
        if board_name in board_list:
            return True
        else:
            return False

    def get_env_para(self, board_name, board_version):
        """
        功能描述：判断单板：是否需要关闭 sha1 加密、是否在已上网产品列表中、是否在无需强制修改密码产品列表中
        参数：单板名称，单板类型
        """
        if not board_name:
            self.logger.info("no input")
            self.compability_enable = 1
            self.security_enhanced_compatible_board_v1 = 1
            self.security_enhanced_compatible_board_v2 = 1
            self.security_enhanced_compatible_board_v4 = 1
        if board_version == "V6":
            self.security_enhanced_compatible_board_v3 = 1
        else:
            if board_version == "V5":
                ret = self.file_find_text(board_name, f"{self.code_path}/compatible_board.list", True)
            if len(ret):
                self.logger.info("in board list")
                self.compability_enable = 1
            else:
                self.logger.info("not in board list")
                self.compability_enable = 0

        # 判断单板是否需要关闭 sha1 加密
        ret = self.file_find_text(board_name, f"{self.code_path}/security_enhanced_compatible_board_list", True)
        if len(ret):
            self.logger.info("in security_enhanced_compatible_board_list")
            self.security_enhanced_compatible_board_v1 = 1
        else:
            self.logger.info("not in security_enhanced_compatible_board_list")
            self.security_enhanced_compatible_board_v1 = 0

        # 判断单板是否在已上网产品列表中
        ret = self.file_find_text(board_name, f"{self.code_path}/security_enhanced_online_compatible_board_list", True)
        if len(ret):
            self.logger.info("in security_enhanced_online_compatible_board_list")
            self.security_enhanced_compatible_board_v2 = 1
        else:
            self.logger.info("not in security_enhanced_online_compatible_board_list")
            self.security_enhanced_compatible_board_v2 = 0

        # 判断单板是否在无需强制修改密码产品列表中
        ret = self.file_find_text(board_name, f"{self.code_path}/security_enhanced_password_no_reset_board_list", True)
        if len(ret):
            self.logger.info("in security_enhanced_password_no_reset_board_list")
            self.security_enhanced_compatible_board_v3 = 1
        else:
            self.logger.info("not in security_enhanced_password_no_reset_board_list")
            self.security_enhanced_compatible_board_v3 = 0
        
        # 判断单板是否在不允许管理员手动删除日志产品列表中
        ret = self.file_find_text(board_name, f"{self.code_path}/security_enhanced_log_delete_board_list", True)
        if len(ret):
            self.logger.info("in security_enhanced_log_delete_board_list")
            self.security_enhanced_compatible_board_v4 = 1
        else:
            self.logger.info("not in security_enhanced_log_delete_board_list")
            self.security_enhanced_compatible_board_v4 = 0

        # 判断单板是否为安全红线3.0机型
        ret = self.file_find_text(board_name, f"{self.code_path}/security_enhance_3v0_board_list", True)
        if len(ret):
            self.logger.info("in security_enhance_3v0_board_list")
            self.security_enhanced_compatible_board_v5 = 1
        else:
            self.logger.info("not in security_enhance_3v0_board_list")
            self.security_enhanced_compatible_board_v5 = 0
        
        # 判断单板是否在IPMI命令冲突的产品列表中
        ret = self.file_find_text(board_name, f"{self.code_path}/board_with_ebmc_list", True)
        if len(ret):
            self.logger.info("in board_with_ebmc_list")
            self.board_with_ebmc_list = 1
        else:
            self.logger.info("not in board_with_ebmc_list")
            self.board_with_ebmc_list = 0
        return

    def run_command(self, cmd, errinfo = "run command failed", raise_exp = False):
        """
        功能描述：执行shell脚本
        参数：cmd shell脚本 ，errinfo 自定义错误信息
        返回值：ret shell运行结果
        """
        ret = subprocess.run(cmd, shell=True, capture_output=True)
        if ret.returncode and raise_exp:
            self.logger.error(f"run command({cmd}) failed, command output:")
            self.logger.error(ret.stderr.decode())
            raise Exception
        return ret


    def get_shell_command_result(self, cmd, error_print=False):
        try:
            ret = subprocess.getstatusoutput(cmd)
            if ret[0] == 1:
                raise Exception(f"{EXCEPTION_CODE[610]}{EXCEPTION_STAGE['C']}{EXCEPTION_RESPONSE[1]}")
        except Exception as e:
            if error_print:
                self.logger.error(e.__str__())
        finally:
            return ret[1]


    def remove_path(self, path):
        """
        功能描述：删除指定的目录
        参数：path 要删除的目录
        返回值：无
        """
        if os.path.isdir(path):
            shutil.rmtree(path, ignore_errors=True)
    

    def remove_file(self, file_name):
        """
        功能描述：删除指定的文件
        参数：file_name 要删除的文件
        返回值：无
        """
        if os.path.exists(file_name):
            os.remove(file_name)


    def del_evn(self, path):
        """
        功能描述：删除环境变量
        参数：path 要删除环境变量
        返回值：无
        """
        if os.environ.get(path):
            del os.environ[path]

    def check_path(self, path):
        """
        功能描述：检查目录是否存在，没有就创建
        参数：path 要检查的目录
        返回值：无
        """
        if not os.path.exists(path) and not os.path.islink(path):
            os.makedirs(path, exist_ok=True)

    def copy_all(self, source_path, target_path):
        """
        功能描述：将 source_path 目录下所有文件全部复制到 target_path 下
                 如果要复制 source_path 文件夹，需指定 target_path 下同名文件夹
        参数：source_path 源目录 target_path 目标目录
        返回值：无
        """
        if os.path.exists(source_path):
            for root, dirs, files in os.walk(source_path, topdown=True):
                for dir in dirs:
                    dest_dir_t = f"{target_path}/{root[len(source_path) + 1:]}/{dir}"
                    self.check_path(dest_dir_t)
                for file in files:
                    src_file = os.path.join(root, file)
                    dest_dir = os.path.join(target_path, root[len(source_path) + 1:])
                    self.check_path(dest_dir)
                    try:
                        dest_file = os.path.join(dest_dir, os.path.basename(src_file))
                        if os.path.isfile(dest_file):
                            os.remove(dest_file)
                        if os.path.islink(src_file):
                            shutil.copy2(src_file, dest_dir, follow_symlinks = False)
                        else:
                            shutil.copy2(src_file, dest_dir)
                    except Exception as e:
                        self.logger.info(f"copy file {src_file} to {dest_dir} failed")

    def move_all(self, source_path, target_path):
        """
        功能描述：将 source_path 目录下所有文件全部移动到 target_path 下
                 如果要复制 source_path 文件夹，需指定 target_path 下同名文件夹
        参数：source_path 源目录 target_path 目标目录
        返回值：无
        """
        if os.path.exists(source_path):
            for root, dirs, files in os.walk(source_path, topdown=True):
                for dir in dirs:
                    dest_dir_t = f"{target_path}/{root[len(source_path) + 1:]}/{dir}"
                    self.check_path(dest_dir_t)
                for file in files:
                    src_file = os.path.join(root, file)
                    # 确保目标文件夹已存在，不存在则创建
                    dest_dir = os.path.join(target_path, root[len(source_path) + 1:])
                    self.check_path(dest_dir)
                    # 确保目标文件不存在
                    dest_file = os.path.join(dest_dir, file)
                    if os.path.isfile(dest_file):
                        os.remove(dest_file)
                    if os.path.islink(src_file):
                        shutil.copy(src_file, dest_dir, follow_symlinks = False)
                    else:
                        shutil.move(src_file, dest_dir)
            shutil.rmtree(source_path)

    def copy(self, source, target):
        """
        功能描述：拷贝文件优化，如果存在目标文件先删除后拷贝
        参数：source 源文件 target 目标文件
        返回值：无
        """
        if os.path.exists(source):
            if os.path.exists(target):
                os.remove(target)
            shutil.copy(source, target)

    def copy_if_exist(self, src, dst):
        """
        功能描述：如果文件存在， 则复制文件
        参数：src 源文件 dst 目标文件
        返回值：无
        """
        if os.path.exists(src):
            shutil.copy(src, dst)
        else:
            self.logger.info(f"This file is not exist: {src}")
            self.logger.info("Please check directory V2R2_trunk/application/src/resource/board/[board_name]")
    
    def copy_file_or_dir(self, src_dir, dst_dir):
        '''
        功能描述：复制文件或者目录
        参数：src_dir 源文件（目录） dst_dir 目标文件（目录）
        返回值：无
        '''
        if not os.path.isdir(dst_dir):
            os.makedirs(dst_dir, exist_ok=True)
        subprocess.run(f"cp -af {src_dir} {dst_dir}", shell=True)
        return

    def run_py_command(self, py_file, errinfo, args=''):
        """
        功能描述：使用python3 执行python脚本
        参数：py_file python脚本 ，errinfo 自定义错误信息 ,args 脚本入参
        返回值：ret python运行结果
        """
        cmd = "python3 {} {}".format(py_file, args)
        return self.run_command(cmd, errinfo)

    def untar_to_dir(self, targz_source, dst_dir=".", numeric_owner=True):
        """
        功能描述：解压tar包到指定目录
        参数：targz_source tar包 ，dst_dir 目标目录
        返回值：无
        """
        try:
            tar_temp = tarfile.open(targz_source)
            tar_temp.extractall(dst_dir, numeric_owner=numeric_owner)
            tar_temp.close()
        except Exception as e:
            self.logger.error(f"{EXCEPTION_CODE[634]}{EXCEPTION_STAGE['C']}{EXCEPTION_RESPONSE[1]}{e.__str__()}")

    def tar_to_dir(self, src_dir, dst_dir):
        """
        功能描述：压缩指定目录到目标tar包
        参数：src_dir 源目录 ，dst_dir 目标tar名前缀
        返回值：无
        """
        try:
            tar_temp = tarfile.open(f"{dst_dir}.tar.gz", "w:gz")
            tar_temp.add(src_dir)
            tar_temp.close()
        except Exception as e:
            self.logger.error(f"EXCEPTION_CODE[634]{EXCEPTION_STAGE['P']}{EXCEPTION_RESPONSE[1]}{e.__str__()}")

    def unzip_to_dir(self, zip_source, dst_dir):
        """
        功能描述：解压zip包到指定目录
        参数：zip_source zip包 ，dst_dir 解压目录
        返回值：无
        """
        try:
            zip_temp = zipfile.ZipFile(zip_source)
            zip_temp.extractall(dst_dir)
            zip_temp.close()
        except Exception as e:
            self.logger.error(f"EXCEPTION_CODE[634]{EXCEPTION_STAGE['P']}{EXCEPTION_RESPONSE[1]}{e.__str__()}")

    def del_head(self, libir, head):
        """
        功能描述：去除指定的文件名头
        参数：libir 文件列表 ，head 要删除的文件名头
        返回值：无
        """
        for f in libir:
            if head in f:
                if not os.path.exists(f[len(head):]):
                    os.rename(f, f[len(head):])

    def get_file_info(self, file):
        """
        功能描述：获取文件中 ‘=’ 左右两边值 返回字典
        参数：file 文件
        返回值：键值 字典
        """
        info = {}
        if os.path.exists(file):
            with open(rf'{file}') as somefile:
                lines = somefile.readlines()
                for line in lines:
                    if "=" in line:
                        result = line.strip('\n').split('=', 1)
                        info[result[0]] = result[1]
        return info

    def get_first_line(self,file):
        """
        功能描述：获取文件第一行 去除换行符
        参数：file 文件
        返回值：str 字符串
        """
        if os.path.exists(file):
            with open(rf'{file}') as somefile:
                # 取第一行，去换行符
                return somefile.readline().strip('\n')

    def get_xml_value(self, file, element, name, subelement):
        """
        功能描述：获取xml文件 指定的element对应属性name的subelement值
        参数：element 元素 subelement子元素
        返回值：str 字符串
        """
        value = ""
        if os.path.exists(file):
            tree = ET.parse(file)
            for result in tree.iter(tag=element):
                if result.get('NAME') == name:
                    value = result.findtext(subelement)
        return value

    def set_xml_value(self, file, element, name, value):
        """
        功能描述：获取xml文件 修改指定的element对应属性的值
        参数：element 元素 subelement子元素
        返回值：str 字符串
        """
        if os.path.exists(file):
            tree = ET.parse(file)
            for result in tree.iter(tag=element):
                if result.get('NAME') == name:
                    for item in result:
                        item.text = value
            tree.write(file)

    def set_xml_subelement_value(self, file, element, name, subelement, value):
        """
        功能描述：修改xml文件 指定的element对应属性name的subelement值
        参数：element 元素 subelement子元素 value 目标修改值
        """

        if os.path.exists(file):
            tree = ET.parse(file)
            for result in tree.iter(tag=element):
                if result.get('NAME') == name:
                    subele = result.find(subelement)
                    subele.text = value
                    tree.write(file)
                    return

    def py_chmod(self, src_file, mode):
        """
        功能描述：修改文件或目录权限，不支持shell选项
        参数：src_file 要修改权限的文件或目录 mode 目标权限
        返回值：无
        """
        try:
            RD, WD, XD = 4, 2, 1
            BNS = [RD, WD, XD]
            MDS = [
                [stat.S_IRUSR, stat.S_IRGRP, stat.S_IROTH],
                [stat.S_IWUSR, stat.S_IWGRP, stat.S_IWOTH],
                [stat.S_IXUSR, stat.S_IXGRP, stat.S_IXOTH]
            ]
            if isinstance(mode, int):
                mode = str(mode)
            if not re.match("^[0-7]{1,3}$", mode):
                raise Exception("mode does not conform to ^[0-7]{1,3}$ pattern")
            mode = "{0:0>3}".format(mode)
            mode_num = 0
            for midx, m in enumerate(mode):
                for bnidx, bn in enumerate(BNS):
                    if (int(m) & bn) > 0:
                        mode_num += MDS[bnidx][midx]
            os.chmod(src_file, mode_num)
        except Exception as e:
            self.logger.error(f"{EXCEPTION_CODE[631]}{EXCEPTION_STAGE['D']}{EXCEPTION_RESPONSE[1]}{e.__str__()}")
        return


    def perm_add(self, src_file, obj, perm_str):
        """
        功能描述：给某个文件增加某个权限
        参数：src_file  要修改的文件或目录
              obj       要添加权限的对象，u:user g:group o:other
              perm_str  权限字符串，最大长度3，r w x
        返回值：无
        """
        try:
            perm_list = ["r", "w", "x"]
            obj_list = ["u", "g", "o"]
            perm_dic = {
                "u": {"r": stat.S_IREAD, "w": stat.S_IWRITE, "x": stat.S_IEXEC},
                "g": {"r": stat.S_IRGRP, "w": stat.S_IWGRP, "x": stat.S_IXGRP},
                "o": {"r": stat.S_IROTH, "w": stat.S_IWOTH, "x": stat.S_IXOTH}
            }
            if len(perm_str) > 3:
                raise Exception(f"{EXCEPTION_CODE[631]}{EXCEPTION_STAGE['D']}{EXCEPTION_RESPONSE[1]}{e.__str__()}")
            for perm in perm_str:
                if perm in perm_list and obj in obj_list:
                    os.chmod(src_file, os.stat(src_file).st_mode | perm_dic[obj][perm])
        except Exception as e:
            self.logger.error(f"{EXCEPTION_CODE[631]}{EXCEPTION_STAGE['D']}{EXCEPTION_RESPONSE[1]}{e.__str__()}")
        return


    def get_board_version(self, board):
        """
        功能描述：根据板子名称获取是V5 or V6
        参数：board 板子名
        返回值：str V5 or V6
        """
        try:
            version = ""
            file = os.path.join(self.code_path, "board.json")
            if os.path.exists(file):
                with open(rf'{file}') as jsonfile:
                    boards = json.load(jsonfile)
                    boards_v3 = boards['V3']
                    boards_v5 = boards['V5']
                    boards_v6 = boards['V6']
                    if board in boards_v3['BoardName']:
                        version = 'V3'
                    elif board in boards_v5['BoardName']:
                        version = 'V5'
                    elif board in boards_v6['BoardName']:
                        version = 'V6'

            if version == "":
                raise Exception(EXCEPTION_CODE[602])
        except Exception as e:
            self.logger.error(f"{e.__str__()}{EXCEPTION_STAGE['D']}{EXCEPTION_RESPONSE[1]}, board:{board}")
            raise Exception(EXCEPTION_CODE[602])
        return version


    def get_product_arch(self, board_name):
        """
        功能描述：依据板名返回板子类型
        参数：board_name 板名
        返回值：产品架构类型 x86 或者 arm
        """
        arch_type_list = ["x86", "arm"]
        file = f"{self.code_path}/product_arch.json"
        if os.path.exists(file):
            with open(rf'{file}') as jsonfile:
                product_arch_dic = json.load(jsonfile)
                for arch_type in arch_type_list:
                    if board_name in product_arch_dic[arch_type]:
                        return arch_type
        self.logger.info("--->>>This product is not in configure json file, use default arch type<<<---")
        self.logger.info("--->>>Please check V2R2_trunk/application/build/product_arch.json<<<---")
        return arch_type_list[0]


    def delete_for_matched(self, path, name):
        """
        功能描述：在给定的路径下把匹配name的文件删除
        参数：path路径 name 要删除文件名含有的文件 *代表删除所有
        返回值：
        """
        files = os.listdir(path)
        for f in files:
            if name == "*" or name in f:
                subprocess.run(f"LD_PRELOAD= sudo LD_PRELOAD=${{LD_PRELOAD}} -E rm -rf {path}/{f}", shell=True)
    
    def rm_symbol_link(self, *symbol_link_name):
        """
        功能描述：删除一个多个指定的符号链接
        参数：指定的一个或多个符号链接
        返回值：无
        """
        for symbol_link in symbol_link_name:
            if os.path.islink(symbol_link):
                os.unlink(symbol_link)

    def read_file_set_env(self, file_name):
        """
        功能描述：读取存储子脚本环境变量的文件，并在当前脚本设置
        参数：存储子脚本环境变量的文件名
        返回值：无
        """
        try:
            with open(file_name, mode="r") as fp:
                for line in fp:
                    if '=' in line:
                        (key, value) = line.strip('\n').split('=', 1)
                        os.environ[key] = value
        except Exception as e:
            self.logger.error(f"{EXCEPTION_CODE[401]}{EXCEPTION_STAGE['C']}{EXCEPTION_RESPONSE[1]}{e.__str__()}")


    def path_env_revise(self):
        '''
        环境变量先后顺序修正
        '''
        hcc_path_list = ['/opt/hcc_arm32le/bin', '/opt/hcc_arm64le/bin']
        path_env = os.getenv('PATH')
        self.logger.debug(path_env)
        path_env_list = path_env.split(':')
        for hcc_path in hcc_path_list:
                if hcc_path in path_env_list:
                        path_env_list.remove(hcc_path)
                        path_env_list.append(hcc_path)
        path_env = path_env_list[0]
        path_list_len = len(path_env_list)
        for i in range(1, path_list_len):
                path_env = f"{path_env}:{path_env_list[i]}"
        os.environ['PATH'] = path_env
        self.logger.info(path_env)
        return


    def re_match_list(self, regular_exp, origin_list):
        """
        功能描述：在 origin_list 列表中，找出符合正则表达式的值，并返回列表
        参数：  regular_exp: 正则表达式
                origin_list: 被查询列表
        返回值：匹配正则表达式的列表
        """
        match_list = []
        for element in origin_list:
            ret = re.fullmatch(regular_exp, element)
            if ret != None:
                match_list.append(element)
        return match_list

    def py_find_dir(self, regular_exp, dir_path, recursive=False):
        """
        功能描述：在 dir_path 列表中，找出符合正则表达式的值路径，并返回列表
        参数：  regular_exp: 正则表达式
                dir_path: 需要查找的路径
                recursive： 是否递归查询(查询所有子文件夹)
        返回值：无
        """
        dir_match_list = []
        dir_list = os.listdir(dir_path)
        for element in dir_list:
            if recursive and os.path.isdir(f"{dir_path}/{element}"):
                dir_match_list += self.py_find_dir(regular_exp, f"{dir_path}/{element}", recursive)
            else:
                ret = re.fullmatch(regular_exp, element)
                if ret != None:
                    dir_match_list.append(f"{dir_path}/{element}")
        return dir_match_list

    def py_find_directory(self, regular_exp, dir_path, recursive=False):
        """
        功能描述：在 dir_path 列表中，找出符合正则表达式的值的文件夹路径。
        参数：  regular_exp: 正则表达式
                dir_path: 需要查找的路径
                recursive： 是否递归查询(查询所有子文件夹)
        返回值：列表，仅包含文件夹路径，不包含文件。
        """
        dir_match_list = []
        dir_list = os.listdir(dir_path)
        for element in dir_list:
            if os.path.isdir(f"{dir_path}/{element}"):
                # 下面都是文件夹
                ret = re.fullmatch(regular_exp, element)
                if ret != None:
                    dir_match_list.append(f"{dir_path}/{element}")
                if recursive:
                    dir_match_list += self.py_find_directory(regular_exp, f"{dir_path}/{element}", recursive)
        return dir_match_list

    def list_all_file(self, regular_exp, dir_path, recursive=False):
        """
        功能描述：在 dir_path 列表中，找出符合正则表达式的值路径，并返回列表
        参数：  regular_exp: 正则表达式
                dir_path: 需要查找的路径
                recursive： 是否递归查询(查询所有子文件夹)
        返回值：无
        """
        dir_match_list = []
        dir_list = os.listdir(dir_path)
        for element in dir_list:
            if recursive and os.path.isdir(f"{dir_path}/{element}"):
                dir_match_list.append(f"{dir_path}/{element}")
                dir_match_list += self.list_all_file(regular_exp, f"{dir_path}/{element}", recursive)
            else:
                ret = re.fullmatch(regular_exp, element)
                if ret != None:
                    dir_match_list.append(f"{dir_path}/{element}")
        return dir_match_list


    def file_find_text(self, src_text, file_name, full_match=False):
        """
        功能描述：在 file_name 文件中，查找是否有 src_text 文本
        参数：   src_text: 需要查找的文本
                file_name: 被查询文件名(必须是文本格式)
        返回值：含有文本的列表
        """
        try:
            text_list = []
            if not src_text:
                return text_list
            with open(file_name, mode='r') as fp:
                for line in fp:
                    text_list.append(line.strip('\n'))
            if full_match == False:
                ret = self.re_match_list(f".*{src_text}.*", text_list)
            else:
                ret = self.re_match_list(f"{src_text}", text_list)
        except FileNotFoundError as e:
            self.logger.error(f"{EXCEPTION_CODE[503]}{EXCEPTION_STAGE['D']}{EXCEPTION_RESPONSE[1]}{e.__str__()}")
        return ret


    def py_sed(self, src_file, regular_exp, instead_text, def_line="l"):
        print(f"Instead file: {src_file} -- regular_exp: {regular_exp}")
        try:
            shutil.copy(src_file, f"{src_file}_bak")
            with open(src_file, "r", encoding="utf-8") as fp_r:
                lines = fp_r.readlines()
            with open(src_file, "w", encoding="utf-8") as fp_w:
                line_num = 0
                if def_line == "l":
                    for line in lines:
                        ret = re.search(regular_exp, line)
                        if ret is None:
                            fp_w.write(line)
                            line_num = line_num + 1
                        else:
                            print(f"string: {ret.group(0)}")
                            line = line.replace(ret.group(0), instead_text)
                            fp_w.write(line)
                            line_num = line_num + 1
                            break
                    for i in range(line_num, len(lines)):
                        fp_w.write(lines[i])
                elif def_line == "g":
                    for line in lines:
                        ret = re.search(regular_exp, line)
                        if ret is not None:
                            line = line.replace(ret.group(0), instead_text)
                        fp_w.write(line)
            os.remove(f"{src_file}_bak")
        except TypeError:
            os.rename(f"{src_file}_bak", src_file)
            self.logger.error(f"{EXCEPTION_CODE[630]}{EXCEPTION_STAGE['D']}{EXCEPTION_RESPONSE[1]}{e.__str__()}")
        return


    def create_log_content(self):
        """
        新建日志目录，对日志进行集中管理
        """
        for key in self.log_manage_dic.keys():
            self.check_path(self.log_manage_dic.get(key))
        return


    def progress_print(self, task_list = []):
        """
        构建等待可视化
        """
        if len(task_list) == 0:
                self.print_cnt = 0
                return
        if False in task_list:
                sys.stdout.write("#")
                sys.stdout.flush()
                self.print_cnt += 1
                if self.print_cnt >= 80:
                        sys.stdout.write("doing\n")
                        sys.stdout.flush()
                        self.print_cnt = 0
                time.sleep(1)

    def ver_num_assemb(self, platform_file, archive_file=""):
        ver_num = ""
        if "v6" in platform_file:
            major_ver = self.get_xml_value(platform_file, 'PROPERTY', 'PMEMajorVer', 'VALUE')
            minor_ver = self.get_xml_value(platform_file, 'PROPERTY', 'PMEMinorVer', 'VALUE').zfill(2)
            release_major_ver = self.get_xml_value(platform_file, 'PROPERTY', 'ReleaseMajorVer', 'VALUE').zfill(2)
            release_minor_ver = self.get_xml_value(platform_file, 'PROPERTY', 'ReleaseMinorVer', 'VALUE').zfill(2)
            if archive_file == "":
                ver_num = f"{major_ver}.{minor_ver}.{release_major_ver}.{release_minor_ver}"
            else:
                tosupport_supply_name = self.get_file_info(archive_file).get("TOSUPPORT_SUPPLY_NAME", "")
                ver_num = f"{major_ver}.{minor_ver}.{release_major_ver}.{release_minor_ver}_{tosupport_supply_name}"
        else:
            major_ver = self.get_xml_value(platform_file, 'PROPERTY', 'PMEMajorVer', 'VALUE')
            minor_ver = self.get_xml_value(platform_file, 'PROPERTY', 'PMEMinorVer', 'VALUE')
            ver_num = f"{major_ver}.{minor_ver}"
        return ver_num

    @classmethod
    def general_permission_conf(cls, configure_file="", configure_field="", data={}):
        bep_preload = f"LD_PRELOAD= sudo LD_PRELOAD=${{LD_PRELOAD}} -E "
        pass

    @classmethod
    def get_json_dic(self, file_name, property_name="", product_arch="", board_name=""):
        with open(file_name, "r") as fp:
            json_dic = json.load(fp)
            if property_name != "":
                property_list = json_dic.get(property_name)
                property_list_len = len(property_list)
                for i in range(property_list_len):
                    if "src_tree" in property_list[i]:
                        property_list[i] = property_list[i].replace("src_tree", self.code_root)
                    if "build_tree" in property_list[i]:
                        property_list[i] = property_list[i].replace("build_tree", self.temp_path)
                    if product_arch != "" and "product_arch" in property_list[i]:
                        property_list[i] = property_list[i].replace("product_arch", product_arch)
                    if board_name != "" and "board_name" in property_list[i]:
                        property_list[i] = property_list[i].replace("board_name", board_name)
            fp.close()
            return json_dic


    def file_cat(self, file_name="", file_path=""):
        if file_name == "" and file_path == "":
            self.logger.error(self.logger.error(f"{EXCEPTION_CODE[701]}{EXCEPTION_STAGE['D']}{EXCEPTION_RESPONSE[1]}"))

        if file_path == "":
            self.logger.info(file_name)
            with open(file_name, "r") as fp:
                for line in fp.readlines():
                    sys.stdout.write(line)
                fp.close()
        else:
            file_list = self.py_find_dir(".*\.log", file_path)
            for cat_file in file_list:
                self.logger.info(cat_file)
                with open(cat_file, "r") as fp:
                    for line in fp.readlines():
                        sys.stdout.write(line)
                    fp.close()
        return


    def term(self):
        os.killpg(os.getpgid(os.getpid()), signal.SIGKILL)


    def get_loop(self):
        while True:
            num = random.randint(0, 8)
            self.loop_dev = f"/dev/loop{num}"
            if not os.path.exists(self.loop_dev):
                subprocess.run(f"LD_PRELOAD= sudo LD_PRELOAD=${{LD_PRELOAD}} -E mknod -m 0660 {self.loop_dev} b 7 {num} ", shell=True)
                ret = subprocess.run(f"LD_PRELOAD= sudo LD_PRELOAD=${{LD_PRELOAD}} -E losetup -a |grep loop{num} ", shell=True)
                if ret.returncode:
                    return
                subprocess.run(f"LD_PRELOAD= sudo LD_PRELOAD=${{LD_PRELOAD}} -E rm -f {self.loop_dev}", shell = True)
                continue
        


    def get_img_flag(self):
        while True:
            flag = self.re_match_list("img.flag", os.listdir(self.code_root))
            if not len(flag):
                self.logger.info("img.flag 文件不存在，继续执行...")
                try:
                    os.mknod(f"{self.code_root}/img.flag")
                except Exception as e:
                    self.logger.info("img.flag 已存在")
                    continue
                break
            else:
                time.sleep(0.1)

    def create_image_and_mount_datafs(self, bs_count, img_path, mnt_datafs):
        """
        创建指定大小的文件并格式化为ext4分区，随后挂载到self.mnt_datafs目录
        """

        # 检查是否有img锁
        self.get_img_flag()
        self.logger.info("img.flag 锁定")
        self.get_loop()
        subprocess.run(f"LD_PRELOAD= sudo LD_PRELOAD=${{LD_PRELOAD}} -E losetup -d {self.loop_dev}", shell=True)
        subprocess.run(f"LD_PRELOAD= sudo LD_PRELOAD=${{LD_PRELOAD}} -E dd if=/dev/zero of={img_path} bs=4096 count={bs_count}", shell=True)
        subprocess.run(f"LD_PRELOAD= sudo LD_PRELOAD=${{LD_PRELOAD}} -E losetup {self.loop_dev} {img_path}", shell=True)
        subprocess.run(f"LD_PRELOAD= sudo LD_PRELOAD=${{LD_PRELOAD}} -E mkfs.ext4 -O ^64bit {self.loop_dev}", shell=True)
        ret = subprocess.run(f"LD_PRELOAD= sudo LD_PRELOAD=${{LD_PRELOAD}} -E mount -t ext4 {self.loop_dev} {mnt_datafs}", shell=True)
        subprocess.run(f"LD_PRELOAD= sudo LD_PRELOAD=${{LD_PRELOAD}} -E chown {self.non_root_ug} {mnt_datafs}", shell=True)
        if (ret.returncode != 0):
            err_msg = f"{EXCEPTION_CODE[412]}{EXCEPTION_STAGE['P']}{EXCEPTION_RESPONSE[3]}"
            self.logger.error(f"{err_msg}mount {img_path} failed")
            subprocess.run(f"LD_PRELOAD= sudo LD_PRELOAD=${{LD_PRELOAD}} -E umount {mnt_datafs}", shell=True)
            subprocess.run(f"LD_PRELOAD= sudo LD_PRELOAD=${{LD_PRELOAD}} -E losetup -d {self.loop_dev}", shell=True)
            subprocess.run(f"rm -f {self.loop_dev}", shell=True)
            os.remove(f"{self.code_root}/img.flag")
            self.logger.info("img.flag 解除")
            os._exit(1)

    def umount_datafs(self, mnt_datafs):
        subprocess.run(f"LD_PRELOAD= sudo LD_PRELOAD=${{LD_PRELOAD}} -E umount {mnt_datafs}", shell=True)
        subprocess.run(f"LD_PRELOAD= sudo LD_PRELOAD=${{LD_PRELOAD}} -E losetup -d {self.loop_dev}", shell=True)
        subprocess.run(f"LD_PRELOAD= sudo LD_PRELOAD=${{LD_PRELOAD}} -E rm -f {self.loop_dev}", shell=True)
        os.remove(f"{self.code_root}/img.flag")
        self.logger.info("img.flag 解除")

    def get_archive_config(self, board_name):
        # archive_emmc_nand 此目录下一个archive.ini文件
        resource_dir = f"{self.code_root}/application/src/resource"
        archive_file = f"{resource_dir}/board/{board_name}/archive_emmc_nand/archive.ini"
        archive_dic = {
            "support_model_name": self.get_file_info(archive_file).get("SUPPORT_MODEL", ""),
            "togdp_dir": self.get_file_info(archive_file).get("TOGDP_PATH", ""),
            "togdp_dir_bcus": self.get_file_info(archive_file).get("TOGDP_PATH_BCUS", ""),
            "togdp_dir_code": self.get_file_info(archive_file).get('TOGDP_PATH_EFUSE_CODE', ""),
            "togdp_dir_eco_ft": self.get_file_info(archive_file).get("TOGDP_PATH_ECO_FT", ""),
            "togdp_dir_fcus": self.get_file_info(archive_file).get("TOGDP_PATH_FCUS", ""),
            "togdp_dir_ccus": self.get_file_info(archive_file).get("TOGDP_PATH_CCUS", ""),
            "togdp_dir_ipmi_enable": self.get_file_info(archive_file).get("TOGDP_PATH_IPMI_ENABLE", ""),
            "togdp_dir_ipmi_disable": self.get_file_info(archive_file).get("TOGDP_PATH_IPMI_DISABLE", ""),
            "togdp_dir_liquid_cooling": self.get_file_info(archive_file).get("TOGDP_PATH_LIQUID_COOLING", ""),
            "togdp_dir_lite": self.get_file_info(archive_file).get("TOGDP_PATH_LITE", ""),
            "togdp_dir_sipmc": self.get_file_info(archive_file).get("TOGDP_PATH_SIPMC", ""),
            "togdp_dir_sp": self.get_file_info(archive_file).get("TOGDP_PATH_SP", ""),
            "togdp_name": self.get_file_info(archive_file).get("TOGDP_NAME", ""),
            "togdp_name_bcus": self.get_file_info(archive_file).get("TOGDP_NAME_BCUS", ""),
            "togdp_name_code": self.get_file_info(archive_file).get('TOGDP_NAME_EFUSE_CODE', ""),
            "togdp_name_delivery": self.get_file_info(archive_file).get("TOGDP_NAME_DELIVERY", ""),
            "togdp_name_eco_ft": self.get_file_info(archive_file).get("TOGDP_NAME_ECO_FT", ""),
            "togdp_name_fcus": self.get_file_info(archive_file).get("TOGDP_NAME_FCUS", ""),
            "togdp_name_ccus": self.get_file_info(archive_file).get("TOGDP_NAME_CCUS", ""),
            "togdp_name_ipmi_enable": self.get_file_info(archive_file).get("TOGDP_NAME_IPMI_ENABLE", ""),
            "togdp_name_ipmi_disable": self.get_file_info(archive_file).get("TOGDP_NAME_IPMI_DISABLE", ""),
            "togdp_name_liquid_cooling": self.get_file_info(archive_file).get("TOGDP_NAME_LIQUID_COOLING", ""),
            "togdp_name_lite": self.get_file_info(archive_file).get("TOGDP_NAME_LITE", ""),
            "togdp_name_sipmc": self.get_file_info(archive_file).get("TOGDP_NAME_SIPMC", ""),
            "togdp_name_sp": self.get_file_info(archive_file).get("TOGDP_NAME_SP", ""),
            "togdp_path_delivery": self.get_file_info(archive_file).get("TOGDP_PATH_DELIVERY", ""),
            "togdp_path_ibma": self.get_file_info(archive_file).get("TOGDP_PATH_iBMA", ""),
            "tosupport_dir": self.get_file_info(archive_file).get("TOSUPPORT_PATH", ""),
            "tosupport_hpm_name": self.get_file_info(archive_file).get("TOSUPPORT_HPM_NAME", ""),
            "tosupport_hpm_name_s920": self.get_file_info(archive_file).get("TOSUPPORT_S920_HPM_NAME", ""),
            "tosupport_lp_name": self.get_file_info(archive_file).get("TOSUPPORT_LP_NAME", ""),
            "tosupport_lp_name_s920": self.get_file_info(archive_file).get("TOSUPPORT_S920_LP_NAME", ""),
            "tosupport_lp_path": self.get_file_info(archive_file).get("TOSUPPORT_LP_PATH", ""),
            "tosupport_mib_name": self.get_file_info(archive_file).get("TOSUPPORT_MIB_NAME", ""),
            "tosupport_mib_name_s920": self.get_file_info(archive_file).get("TOSUPPORT_S920_MIB_NAME", ""),
            "tosupport_mib_path": self.get_file_info(archive_file).get("TOSUPPORT_MIB_PATH", ""),
            "tosupport_model_name_s920": self.get_file_info(archive_file).get("SUPPORT_S920_MODEL", ""),
            "tosupport_name": self.get_file_info(archive_file).get("TOSUPPORT_NAME", ""),
            "tosupport_name_s920": self.get_file_info(archive_file).get("TOSUPPORT_S920_NAME", ""),
            "tosupport_supply_name": self.get_file_info(archive_file).get("TOSUPPORT_SUPPLY_NAME", ""),
            "tosupport_supply_S920_name": self.get_file_info(archive_file).get("TOSUPPORT_SUPPLY_S920_NAME", ""),
            "tosupport_qz_name_s920": self.get_file_info(archive_file).get("TOSUPPORT_S920_QZ_NAME", ""),
            "tosupport_qz_hpm_name_s920": self.get_file_info(archive_file).get("TOSUPPORT_S920_QZ_HPM_NAME", ""),
            "tosupport_qz_mib_name_s920": self.get_file_info(archive_file).get("TOSUPPORT_S920_QZ_MIB_NAME", ""),
            "tosupport_qz_model_name_s920": self.get_file_info(archive_file).get("SUPPORT_S920_QZ_MODEL", ""),
            "togdp_dir_leakdetect_abs_alarm": self.get_file_info(archive_file).get("TOGDP_PATH_LEAKDETECT_ABS_ALARM", ""),
            "togdp_name_leakdetect_abs_alarm": self.get_file_info(archive_file).get("TOGDP_NAME_LEAKDETECT_ABS_ALARM", ""),
            "tosupport_name_pro": self.get_file_info(archive_file).get("TOSUPPORT_PRO_NAME", ""),
            "tosupport_lp_name_pro": self.get_file_info(archive_file).get("TOSUPPORT_PRO_LP_NAME", ""),
            "tosupport_mib_name_pro": self.get_file_info(archive_file).get("TOSUPPORT_PRO_MIB_NAME", ""),
            "tosupport_hpm_name_pro": self.get_file_info(archive_file).get("TOSUPPORT_PRO_HPM_NAME", ""),
            "tosupport_model_name_pro": self.get_file_info(archive_file).get("SUPPORT_PRO_MODEL", ""),
            "tosupport_name_k": self.get_file_info(archive_file).get("TOSUPPORT_K_NAME", ""),
            "tosupport_lp_name_k": self.get_file_info(archive_file).get("TOSUPPORT_K_LP_NAME", ""),
            "tosupport_mib_name_k": self.get_file_info(archive_file).get("TOSUPPORT_K_MIB_NAME", ""),
            "tosupport_hpm_name_k": self.get_file_info(archive_file).get("TOSUPPORT_K_HPM_NAME", ""),
            "tosupport_model_name_k": self.get_file_info(archive_file).get("SUPPORT_K_MODEL", ""),
            "togdp_dir_fw_revs": self.get_file_info(archive_file).get("TOGDP_PATH_FW_REVS", ""),
            "togdp_name_fw_revs": self.get_file_info(archive_file).get("TOGDP_NAME_FW_REVS", ""),
            "togdp_dir_tbj": self.get_file_info(archive_file).get("TOGDP_PATH_TBJ", ""),
            "togdp_name_tbj": self.get_file_info(archive_file).get("TOGDP_NAME_TBJ", ""),
            "togdp_dir_hmf": self.get_file_info(archive_file).get("TOGDP_PATH_HMF", ""),
            "togdp_name_hmf": self.get_file_info(archive_file).get("TOGDP_NAME_HMF", ""),
            "togdp_dir_lcu": self.get_file_info(archive_file).get("TOGDP_PATH_LCU", ""),
            "togdp_name_lcu": self.get_file_info(archive_file).get("TOGDP_NAME_LCU", ""),
            "togdp_dir_hqe": self.get_file_info(archive_file).get("TOGDP_PATH_HQE", ""),
            "togdp_name_hqe": self.get_file_info(archive_file).get("TOGDP_NAME_HQE", ""),
            "togdp_dir_hqh": self.get_file_info(archive_file).get("TOGDP_PATH_HQH", ""),
            "togdp_name_hqh": self.get_file_info(archive_file).get("TOGDP_NAME_HQH", ""),
            "togdp_dir_script": self.get_file_info(archive_file).get("TOGDP_PATH_SCRIPT", ""),
            "togdp_name_script": self.get_file_info(archive_file).get("TOGDP_NAME_SCRIPT", ""),
            "togdp_dir_efuse": self.get_file_info(archive_file).get("TOGDP_PATH_EFUSE", ""),
            "togdp_name_efuse": self.get_file_info(archive_file).get("TOGDP_NAME_EFUSE", ""),
            "togdp_dir_efuse_partner_tcm": self.get_file_info(archive_file).get("TOGDP_PATH_EFUSE_PARTNER_TCM", ""),
            "togdp_name_efuse_partner_tcm": self.get_file_info(archive_file).get("TOGDP_NAME_EFUSE_PARTNER_TCM", ""),
            "tosupport_pro_name_s920": self.get_file_info(archive_file).get("TOSUPPORT_S920_PRO_NAME", ""),
            "tosupport_pro_hpm_name_s920": self.get_file_info(archive_file).get("TOSUPPORT_S920_PRO_HPM_NAME", ""),
            "tosupport_pro_mib_name_s920": self.get_file_info(archive_file).get("TOSUPPORT_S920_PRO_MIB_NAME", ""),
            "tosupport_pro_model_name_s920": self.get_file_info(archive_file).get("SUPPORT_S920_PRO_MODEL", ""),
            "togdp_ht_dir_fw_revs": self.get_file_info(archive_file).get("TOGDP_HT_PATH_FW_REVS", ""),
            "togdp_ht_name_fw_revs": self.get_file_info(archive_file).get("TOGDP_HT_NAME_FW_REVS", "")
        }
        archive_dic.update({
            "delivery_0502_name": archive_dic.get("togdp_path_delivery", "").split("/")[-1],
            "togdp_0502": archive_dic.get("togdp_dir", "").split('/')[-1],
            "togdp_0502_name": archive_dic.get("togdp_dir", "").split("/")[-1],
            "togdp_bcus_0502": archive_dic.get("togdp_dir_bcus", "").split("/")[-1],
            "togdp_code_efuse": archive_dic.get("togdp_dir_code", "").split("/")[-1],
            "togdp_eco_ft_0502": archive_dic.get("togdp_dir_eco_ft", "").split("/")[-1],
            "togdp_fcus_0502": archive_dic.get("togdp_dir_fcus", "").split("/")[-1],
            "togdp_ccus_0502": archive_dic.get("togdp_dir_ccus", "").split("/")[-1],
            "togdp_ipmi_enable_0502": archive_dic.get("togdp_dir_ipmi_enable", "").split('/')[-1],
            "togdp_ipmi_disable_0502": archive_dic.get("togdp_dir_ipmi_disable", "").split('/')[-1],
            "togdp_liquid_cooling_0502": archive_dic.get("togdp_dir_liquid_cooling", "").split('/')[-1],
            "togdp_lite_0502": archive_dic.get("togdp_dir_lite", "").split('/')[-1],
            "togdp_sipmc_0502": archive_dic.get("togdp_dir_sipmc", "").split("/")[-1],
            "togdp_sp_0502": archive_dic.get("togdp_dir_sp", "").split("/")[-1],
            "togdp_leakdetect_abs_alarm_0502": archive_dic.get("togdp_dir_leakdetect_abs_alarm", "").split("/")[-1],
            "togdp_fw_revs_0502": archive_dic.get("togdp_dir_fw_revs", "").split("/")[-1],
            "togdp_tbj_0502": archive_dic.get("togdp_dir_tbj", "").split("/")[-1],
            "togdp_hmf_0502": archive_dic.get("togdp_dir_hmf", "").split("/")[-1],
            "togdp_lcu_0502": archive_dic.get("togdp_dir_lcu", "").split("/")[-1],
            "togdp_hqe_0502": archive_dic.get("togdp_dir_hqe", "").split("/")[-1],
            "togdp_hqh_0502": archive_dic.get("togdp_dir_hqh", "").split("/")[-1],
            "togdp_efuse_0502": archive_dic.get("togdp_dir_efuse", "").split("/")[-1],
            "togdp_efuse_partner_tcm_0502": archive_dic.get("togdp_dir_efuse_partner_tcm", "").split("/")[-1],
            "togdp_ht_fw_revs_0502": archive_dic.get("togdp_ht_dir_fw_revs", "").split("/")[-1]
        })

        # archive_uboot 此目录下有另一个archive.ini文件
        archive_file = f"{resource_dir}/board/{board_name}/archive_uboot/archive.ini"
        if os.path.exists(archive_file):
            archive_dic.update({
                "togdp_uboot_dir": self.get_file_info(archive_file).get("TOGDP_PATH", ""),
                "togdp_uboot_name": self.get_file_info(archive_file).get("TOGDP_NAME", "")
            })
        else:
            self.logger.info("archive_uboot config does not support")
            
        if archive_dic['tosupport_supply_S920_name'] != "":
            archive_dic['tosupport_supply_S920_name'] = f"_{archive_dic['tosupport_supply_S920_name']}"

        if archive_dic['tosupport_supply_name'] != "":
            archive_dic['tosupport_supply_name'] = f"_{archive_dic['tosupport_supply_name']}"

        return archive_dic

    def get_cb_archive_config(self, cb_flag_archive_ini):
        archive_file = f"{cb_flag_archive_ini}"
        archive_dic = {
            "cb_flag_togdp_dir": self.get_file_info(archive_file).get("CB_FLAG_TOGDP_PATH", ""),
            "cb_flag_togdp_name": self.get_file_info(archive_file).get("CB_FLAG_TOGDP_NAME", ""),
            "cb_flag_tosupport_name": self.get_file_info(archive_file).get("CB_FLAG_TOSUPPORT_NAME", ""),
            "cb_flag_tosupport_mib_dir": self.get_file_info(archive_file).get("CB_FLAG_TOSUPPORT_MIB_PATH", ""),
            "cb_flag_tosupport_mib_name": self.get_file_info(archive_file).get("CB_FLAG_TOSUPPORT_MIB_NAME", ""),
            "cb_flag_tosupport_lp_dir": self.get_file_info(archive_file).get("CB_FLAG_TOSUPPORT_LP_PATH", ""),
            "cb_flag_tosupport_lp_name": self.get_file_info(archive_file).get("CB_FLAG_TOSUPPORT_LP_NAME", ""),
            "cb_flag_support_model_name": self.get_file_info(archive_file).get("CB_FLAG_SUPPORT_MODEL", ""),
            "cb_flag_tosupport_name_hpm": self.get_file_info(archive_file).get("CB_FLAG_TOSUPPORT_HPM_NAME", ""),
            "cb_flag_togdp_dir_delivery": self.get_file_info(archive_file).get("CB_FLAG_TOGDP_PATH_DELIVERY", ""),
            "cb_flag_togdp_name_delivery": self.get_file_info(archive_file).get("CB_FLAG_TOGDP_NAME_DELIVERY", ""),
        }
        archive_dic.update({
            "cb_flag_sw_code": archive_dic.get("cb_flag_togdp_dir", "").split("/")[-1],
            "cb_flag_sw_delivery_code": archive_dic.get("cb_flag_togdp_dir_delivery", "").split("/")[-1],
        })
        return archive_dic


    def extract_ver_num(self, ar_ver_file, board_name, readonly=False, dft_flag=False):
        if self.get_board_version(board_name) == "V5":
            platform_file = f"{self.code_root}/application/src/resource/pme_profile/platform.xml"
        else:
            platform_file = f"{self.code_root}/application/src/resource/pme_profile/platform_v6.xml"
        if readonly:
            if dft_flag:
                major_ver = self.get_xml_value(platform_file, 'PROPERTY', 'PMEMajorVer', 'VALUE')
                minor_ver = self.get_xml_value(platform_file, 'PROPERTY', 'PMEMinorVer', 'VALUE').zfill(2)
                release_major_ver = self.get_xml_value(platform_file, 'PROPERTY', 'ReleaseMajorVer', 'VALUE').zfill(2)
                release_minor_ver = self.get_xml_value(platform_file, 'PROPERTY', 'ReleaseMinorVer', 'VALUE').zfill(2)
                dft_minor_ver = str(int(release_minor_ver) + 1).zfill(2)
                ver_num = f"{major_ver}.{minor_ver}.{release_major_ver}.{dft_minor_ver}"
            else:
                ver_num = self.ver_num_assemb(platform_file)
        else:
            shutil.copy(ar_ver_file, "tmp_version.ini")
            if dft_flag:
                major_ver = self.get_xml_value(platform_file, 'PROPERTY', 'PMEMajorVer', 'VALUE')
                minor_ver = self.get_xml_value(platform_file, 'PROPERTY', 'PMEMinorVer', 'VALUE').zfill(2)
                release_major_ver = self.get_xml_value(platform_file, 'PROPERTY', 'ReleaseMajorVer', 'VALUE').zfill(2)
                release_minor_ver = self.get_xml_value(platform_file, 'PROPERTY', 'ReleaseMinorVer', 'VALUE').zfill(2)
                dft_minor_ver = str(int(release_minor_ver) + 1).zfill(2)
                ver_num = f"{major_ver}.{minor_ver}.{release_major_ver}.{dft_minor_ver}"
            else:
                ver_num = self.ver_num_assemb(platform_file)
            if self.get_board_version(board_name) == "V5":
                self.py_sed("tmp_version.ini", "(?<=archive_version=).*", ver_num.replace(".", "", 1))
            else:
                self.py_sed("tmp_version.ini", "(?<=archive_version=).*", ver_num)
            self.py_sed("tmp_version.ini", "(?<=[Aa]ctive[Bb][Mm][Cc]=).*", ver_num)
            self.py_sed("tmp_version.ini", "(?<=[Bb]ackup[Bb][Mm][Cc]=).*", ver_num)
            # 此处之前的正则有问题，现已修正，需咨询板主是否要使用正确的替换逻辑
            if board_name == "2288hv5":
                self.py_sed("tmp_version.ini", "(?<=Active_AuxVersion=).*", "1")
                self.py_sed("tmp_version.ini", "(?<=Backup_AuxVersion=).*", "1")
            shutil.copy("tmp_version.ini", ar_ver_file)
            os.remove("tmp_version.ini")
        if self.get_board_version(board_name) == "V5":
            ver_num = ver_num.replace(".", "", 1)
        return ver_num

    def get_dft_tools(self, release_dir, board_name):
        dft_tools_dir = f"{release_dir}/dft_tools"
        customize_dir = f"{release_dir}/dft_tools/config/Common/customize"
        resource_dir = f"{self.code_root}/application/src/resource"
        product_arch = self.get_product_arch(board_name)
        if board_name == "S920":
            product_arch = "x86"

        self.remove_path(dft_tools_dir)
        self.check_path(dft_tools_dir)
        self.copy_all(f"{self.internal_release_path}/dft_tools",
                        dft_tools_dir)
        self.copy_all(f"{resource_dir}/config/Common/customize",
                        customize_dir)

        if self.get_support_dft_separation_flag(board_name):
            shutil.copy(f"{self.code_root}/tools/others/release/wbd-packet/clearlog-packet/wbd-clearlog-image.hpm",
                        f"{customize_dir}/wbd-clearlog-image.hpm")

        customize_file_tuple = (
            f"{customize_dir}/config.sh",
            f"{customize_dir}/common_factory_restore.sh",
            f"{customize_dir}/white_branding.sh",
            f"{customize_dir}/bmc_upload_cmes.sh",
            f"{customize_dir}/{product_arch}/conver",
            f"{customize_dir}/verify.sh",
            f"{customize_dir}/dftimage_v5.hpm",
            f"{customize_dir}/dftdisableimage_v5.hpm",
            f"{customize_dir}/clear_wbd_config_v5.hpm",
            f"{customize_dir}/clear_wbd_config_3v0.hpm",
            f"{customize_dir}/defaultserverport.ini",
            f"{customize_dir}/function",
            f"{dft_tools_dir}/hpmfwupg/{product_arch}/hpmfwupg_v5",
            f"{resource_dir}/board/{board_name}/version.xml"
        )
        for check_file in customize_file_tuple:
            if not os.path.exists(check_file):
                logger.error(f"{check_file} file is not exist!")
                os._exit(1)

    def board_in_list(self, board, board_list):
        if board in board_list:
            return True
        return False

    def is_rw_emmc(self, board):
        """
        是否需要制作rw版本的烧片包
        """
        board_list = ("RM210", "RM211", "RM210_SECURITY_3V0", "PangeaV6_Atlantic", "PangeaV6_Atlantic_Smm", "PangeaV6_Pacific", "PangeaV6_Arctic", "PAC1220V6", "CST1020V6", "CST1220V6", "CST0220V6", "CST0221V6", "CST0210V6", "SMM0001V6", "ENC0210V6")
        return self.board_in_list(board, board_list)

    def is_rw_to_gdp_pkg(self, board):
        """
        是否需要制作rw版本的to_gdp hpm包
        """
        board_list = ("PangeaV6_Atlantic", "PangeaV6_Atlantic_Smm", "PangeaV6_Pacific", "PangeaV6_Arctic", "PAC1220V6", "CST1020V6", "CST1220V6", "CST0220V6", "CST0221V6", "CST0210V6", "SMM0001V6")
        return self.board_in_list(board, board_list)

    def no_need_support_pkg(self, board):
        """
        是否不需要制作发货包(to support package)
        """
        board_list = ("PangeaV6_Atlantic", "PangeaV6_Atlantic_Smm", "PangeaV6_Pacific", "PangeaV6_Arctic", "CST1020V6", "CST1220V6", "CST0220V6", "CST0221V6", "CST0210V6", "SMM0001V6", "PAC1220V6", "ENC0210V6")
        return self.board_in_list(board, board_list)
    def is_pangea_storage_board(self, board):
        """
        盘古存储单板
        """
        board_list = ("CST0221V6", "CST0210V6", "SMM0001V6", "ENC0210V6")
        return self.board_in_list(board, board_list)
    def need_update_version_xml(self, board):
        """
        需要更新version.xml
        """
        board_list = ("CST1020V6", "CST1220V6", "CST0220V6", "CST0221V6", "CST0210V6", "SMM0001V6", "PangeaV6_Arctic","PangeaV6_Atlantic", "PangeaV6_Atlantic_Smm", "PangeaV6_Pacific", "PAC1220V6", "ENC0210V6")
        return self.board_in_list(board, board_list)
    def need_update_pangea_alarm_suggestions(self, board):
        """
        需要更新为盘古的告警处理建议
        """
        board_list = ("PangeaV6_Atlantic", "PangeaV6_Atlantic_Smm", "PangeaV6_Pacific", "PangeaV6_Arctic", "CST0210V6", "SMM0001V6", "CST0220V6", "CST0221V6", "PAC1220V6", "ENC0210V6")
        return self.board_in_list(board, board_list)

    def yaml_load_template(self, yaml_name: str, template: dict):
        with open(yaml_name, "r") as fp:
            yaml_string = fp.read()
            yaml_string = Template(yaml_string)
            yaml_conf = yaml_string.safe_substitute(template)
            return yaml.full_load(yaml_conf)