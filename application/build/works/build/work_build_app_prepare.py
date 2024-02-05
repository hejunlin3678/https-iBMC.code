#!/usr/bin/env python
# coding=utf-8

"""
文件名：work_build_app_prepare.py
功能：从work_build_app.py中拆出，用于完成公共环境准备，以实现多个work_build_app.py并发运行
版权信息：华为技术有限公司，版本所有(C) 2019-2020
可配参数：only_app: 为true时仅编译app不打包，可以不用下载开源源码
"""

import os
import time
import shutil
import sys

import argparse
import subprocess

cwd=os.path.dirname(__file__)
sys.path.append(f"{cwd}/../")
sys.path.append(f"{cwd}/../../")
from works.work import Work
from utils.config import Config
from utils.common import Common
from utils.error_info import EXCEPTION_STAGE
from utils.error_info import EXCEPTION_RESPONSE
from utils.error_info import EXCEPTION_CODE

class WorkBuildAppPrepare(Work):
    '''
    '''
    # 可配置项，如果仅编译app，不需要复制一些文件
    only_app = False

    def set_global(self):
        return

    def set_only_app(self, only_app):
        self.only_app = only_app

    def xml_formula_check(self):
        self.logger.info("XML formula syntax check started")
        xml_dir = f"{self.common.pme_dir}/src/resource"

        formula_status = 0
        formula_check_result = ""
        tmp_result = self.common.get_shell_command_result(
            f"grep -Enr '\|\|\S' `find '{xml_dir}' -name '*\.xml'` | grep '\"Formula\"' 2>&1")
        formula_check_result = f"{formula_check_result}\n{tmp_result}"
        tmp_result = self.common.get_shell_command_result(
            f"grep -Enr '\S\|\|' `find '{xml_dir}' -name '*\.xml'` | grep '\"Formula\"' 2>&1")
        formula_check_result = f"{formula_check_result}\n{tmp_result}"
        tmp_result = self.common.get_shell_command_result(
            f"grep -Enr '&amp;&amp;\S' `find '{xml_dir}' -name '*\.xml'` | grep '\"Formula\"' 2>&1")
        formula_check_result = f"{formula_check_result}\n{tmp_result}"
        tmp_result = self.common.get_shell_command_result(
            f"grep -Enr '\S&amp;&amp;' `find '{xml_dir}' -name '*\.xml'` | grep '\"Formula\"' 2>&1")
        formula_check_result = f"{formula_check_result}\n{tmp_result}"
        formula_fail_result = self.common.get_shell_command_result(
            f"echo -e '{formula_check_result}' | grep -v '^$' | sort | uniq")

        if len(formula_fail_result):
            self.logger.info("XML formula syntax check failed!")
            self.logger.info(
                "XML Formula should have spaces before and after '&amp;&amp; or || '")
            self.logger.info("Incorret formulas are: ")
            self.logger.info(formula_fail_result)
            formula_status = 1
        self.logger.info("XML formula syntax check ended")

        if self.config.board_version == "V5" and formula_status:
            err_msg = f"{EXCEPTION_CODE[601]}{EXCEPTION_STAGE['D']}{EXCEPTION_RESPONSE[2]}"
            self.logger.error(f"{err_msg}XML formula syntax check failed!")
            os._exit(formula_status)
        return

    def copy_rootfs(self):
        os.chdir(f"{self.common.code_path}/../src")
        self.common.remove_path("rootfs")
        self.logger.info("copy rootfs...")
        self.common.copy_file_or_dir(f"{self.common.code_path}/../rootfs", ".")

        # 添加 pme xml
        os.chdir(f"{self.common.code_path}/../src/rootfs/opt/pme/conf/profile")
        self.common.copy_file_or_dir(f"{self.common.code_path}/../src/resource/pme_profile/*", ".")
        # 打包前处理 web 文件
        os.chdir(f"{self.common.code_path}/../src/rootfs/opt/pme")
        self.common.remove_path("web")
        self.logger.info("copy web...")
        self.common.copy_file_or_dir(f"{self.common.code_path}/../src/web", ".")
        self.logger.info("copy source web done")
        self.logger.info("copy rootfs done")
        return


    def add_weakdictionary(self):
        weak_password = ["Huawei12#$"]
        weak_dictionary = []
        with open(f"{self.common.code_path}/../src/rootfs/data/etc/weakdictionary", mode="r+") as fp:
            for line in fp:
                weak_dictionary.append(line.strip('\n'))
            fp.close()
        fp = open(f"{self.common.code_path}/../src/rootfs/data/etc/weakdictionary", mode="a")
        for passwd in weak_password:
            if passwd not in weak_dictionary:
                fp.write(f"{passwd}\n")
        fp.close()
        return

    def update_event_lang_template_version(self):
        """
        根据 platform_5.xml 更新 event_lang_template_v2.xml 的版本号
        """
        os.chdir(self.common.code_path)
        subprocess.run("sh update_event_lang_template_version.sh", shell=True)
        return

    def move_rtl8382(self):
        os.chdir(f"{self.common.code_path}/../../vendor/MSWITCH-8380M-SOFTWARE")
        rtl8382 = "eUSDK-release-v1.1.4-svn98922"
        rtl_8382_sdk_dir = f"{self.common.code_path}/../../vendor/MSWITCH-8380M-SOFTWARE/rtl8382_sdk"
        if not os.path.exists(f"{rtl_8382_sdk_dir}"):
            os.mkdir(f"{rtl_8382_sdk_dir}")
        subprocess.run(f"cp -rf {self.common.code_path}/../../vendor/MSWITCH-8380M-SOFTWARE/{rtl8382}/{rtl8382}/{rtl8382}/* {rtl_8382_sdk_dir}", shell=True)
        subprocess.run("find . -type f -print0 | xargs -0 dos2unix", shell=True)
        if not os.path.exists(f"{self.common.code_path}/../../application/src/apps/lsw_main/inc/sdk_inc"):
            os.mkdir(f"{self.common.code_path}/../../application/src/apps/lsw_main/inc/sdk_inc")
        subprocess.run(f"cp -vrf {rtl_8382_sdk_dir}/include/* {self.common.code_path}/../../application/src/apps/lsw_main/inc/sdk_inc", shell=True)

    def move_rtl8367(self):
        os.chdir(f"{self.common.code_path}/../../vendor/Realtek_RTL8367RB_Firmware")
        rtl8367_pack = "Realtek_Unmanaged_Switch_API_V1.3.12_20171124\(huawei\)"
        subprocess.run(f"ls -l .", shell=True)
        rtl_8367_sdk_dir = f"{self.common.code_path}/../../vendor/Realtek_RTL8367RB_Firmware/rtl8367_sdk"
        if not os.path.exists(f"{rtl_8367_sdk_dir}"):
            os.mkdir(f"{rtl_8367_sdk_dir}")
        subprocess.run(f"cp -rf {self.common.code_path}/../../vendor/Realtek_RTL8367RB_Firmware/{rtl8367_pack}/API_Source {rtl_8367_sdk_dir}", shell=True)
        os.chdir(f"{rtl_8367_sdk_dir}")
        subprocess.run("find . -type f -print0 | xargs -0 dos2unix", shell=True)
        subprocess.run(f"ls -l .", shell=True)
        if not os.path.exists(f"{self.common.code_path}/../../application/src/apps/lsw_main/inc/8367_sdk"):
            os.mkdir(f"{self.common.code_path}/../../application/src/apps/lsw_main/inc/8367_sdk")
        subprocess.run(f"cp -vrf {rtl_8367_sdk_dir}/API_Source/*.h {self.common.code_path}/../../application/src/apps/lsw_main/inc/8367_sdk", shell=True)
        subprocess.run(f"rm -rf {rtl_8367_sdk_dir}", shell=True)

    def move_sf2507(self):
        os.chdir(f"{self.common.code_path}/../../vendor/HUYANG010_HY010-4304-0001")
        sf2507_pack = "SDK-SRC-ESC-2.1.15"
        subprocess.run(f"ls -l .", shell=True)
        sf2507_sdk_dir = f"{self.common.code_path}/../../vendor/HUYANG010_HY010-4304-0001/sf2507_sdk"
        if not os.path.exists(f"{sf2507_sdk_dir}"):
            os.mkdir(f"{sf2507_sdk_dir}")
        subprocess.run(f"cp -rf {self.common.code_path}/../../vendor/HUYANG010_HY010-4304-0001/{sf2507_pack}/API_Source {sf2507_sdk_dir}", shell=True)
        subprocess.run(f"cp -rf {self.common.code_path}/../../vendor/HUYANG010_HY010-4304-0001/{sf2507_pack}/workaround {sf2507_sdk_dir}", shell=True)
        os.chdir(f"{sf2507_sdk_dir}")
        subprocess.run("find . -type f -print0 | xargs -0 dos2unix", shell=True)
        subprocess.run(f"ls -l .", shell=True)
        if not os.path.exists(f"{self.common.code_path}/../../application/src/apps/lsw_main/inc/sf2507_sdk"):
            os.mkdir(f"{self.common.code_path}/../../application/src/apps/lsw_main/inc/sf2507_sdk")
        subprocess.run(f"cp -vrf {sf2507_sdk_dir}/API_Source/*.h {self.common.code_path}/../../application/src/apps/lsw_main/inc/sf2507_sdk", shell=True)
        subprocess.run(f"cp -vrf {sf2507_sdk_dir}/workaround/*.h {self.common.code_path}/../../application/src/apps/lsw_main/inc/sf2507_sdk", shell=True)
        subprocess.run(f"rm -rf {sf2507_sdk_dir}", shell=True)

    def generate_conf(self):
        self.logger.info("generate OpenSSH conf: ")
        os.chdir(f"{self.common.code_path}/../../open_source/OpenSSH")
        subprocess.run("chmod +x generate_conf.sh", shell=True)
        subprocess.run("sh generate_conf.sh", shell=True)
        return
    
    def get_sdk(self):
        self.logger.info(f"*************************** use {self.config.board_version} SDK #################################")
        os.chdir(f"{self.common.pme_dir}/{self.config.platform}")
        shutil.rmtree(f"{self.common.pme_dir}/sdk", ignore_errors=True)
        os.makedirs(f"{self.common.pme_dir}/sdk", exist_ok=True)
        self.run_command(f"tar -zxf PME_SDK.tar.gz -C {self.common.pme_dir}/sdk")
        if self.config.get_flag_need_v3() == "V3_MACA":
            self.logger.info(f"*************************** use iBMC V3 Component #################################")
            # 用iBMC V3构建的so文件来替换PME的so文件
            pme_lib_dest_path = self.common.get_shell_command_result(f"find {self.common.pme_dir}/sdk -type d -name lib")
            self.common.remove_path(pme_lib_dest_path)
            self.common.copy_file_or_dir(f"{self.common.manifest_output_path}/opt/bmc/lib/*", pme_lib_dest_path)

            # 把iBMC V3构建的libmuparser库移动到PME的lib目录
            self.common.copy_file_or_dir(f"{self.common.manifest_output_path}/lib/libmuparser.so*", pme_lib_dest_path)

            # 用iBMC V3的头文件来替换PME的头文件
            pme_include_dest_path = self.common.get_shell_command_result(f"find {self.common.pme_dir}/sdk -type d -name include")
            self.common.remove_path(pme_include_dest_path)
            self.common.check_path(pme_include_dest_path)
            command_list = [f"find {self.common.conan_v3_header_path} -type d -name include",
                            "| xargs -I", "{}", "cp -rf {}", f"{pme_include_dest_path}/.."]
            subprocess.run(" ".join(command_list), shell=True)

            # iBMC V2的APP源代码中包含的驱动头文件路径为/pme/drivers路径
            self.common.move_all(f"{pme_include_dest_path}/libsoc-adapter", f"{pme_include_dest_path}/pme/drivers")

            # iBMC V2的APP源代码中包含的file_manage头文件路径为pme/file_manage路径
            self.common.move_all(f"{pme_include_dest_path}/file_manage", f"{pme_include_dest_path}/pme/file_manage")
        return

    def build_storagecore(self):
        self.config.set_env_global()

        os.chdir(f"{self.common.code_root}/vendor/StorageCore")

        if self.config.board_version == "V6":
            shutil.copy(f"{self.common.code_root}/open_source/global64.sh", "./")
            subprocess.run("chmod +x build64.sh", shell=True)
            subprocess.run(f"sh build64.sh", shell=True)
            self.common.copy_file_or_dir(f"output/*", self.common.lib_open_source)
        return

    def move_vpp(self):
        # vpp组件
        os.chdir(f"{self.common.code_root}/application/src_bin")
        if os.path.isdir("./platforms/vpp"):
            shutil.rmtree("./platforms/vpp")
        shutil.copytree(f"{self.common.code_root}/temp/vpp", "./platforms/vpp")

        # libatomic 1710编译vpp组件时, openssl3.0依赖libatomic.so, 1711无此依赖
        if self.config.board_version == "V5":
            shutil.copy("/opt/hcc_arm32le/arm-linux-gnueabi/lib/libatomic.so.1.2.0", "./platforms/vpp/usr/lib/")
            shutil.copy("/opt/hcc_arm32le/arm-linux-gnueabi/lib/libatomic.so.1", "./platforms/vpp/usr/lib/")
            shutil.copy("/opt/hcc_arm32le/arm-linux-gnueabi/lib/libatomic.so", "./platforms/vpp/usr/lib/")
        return

    def run(self):
        self.set_global()
        # 自动更新 event_lang_template_v2.xml 版本
        self.update_event_lang_template_version()

        subprocess.run(f"sh common.sh {self.common.code_path} {self.config.board_name}", shell=True)

        # 这是主要操作
        if not self.only_app:
            self.xml_formula_check()
            self.generate_conf()

        # 为平台资源打补丁
        os.chdir(self.common.code_path)
        subprocess.run("sh patch_for_platforms.sh", shell=True)

        self.common.remove_path(self.config.work_out)
        os.makedirs(self.config.work_out)

        self.copy_rootfs()
        if self.config.board_version == "V6":
            self.move_rtl8382()
            self.move_rtl8367()
            self.move_sf2507()

            os.chdir(f"{self.common.code_path}/../../vendor/MSWITCH-8380M-SOFTWARE")
            subprocess.run(f"chmod 777 -R {self.common.code_path}/../../vendor/MSWITCH-8380M-SOFTWARE/*", shell=True)
            subprocess.run(f"ls -l .", shell=True)
            subprocess.run(f"{self.common.code_path}/../../vendor/MSWITCH-8380M-SOFTWARE/build64.sh", shell=True)

            os.chdir(f"{self.common.code_path}/../../vendor/Realtek_RTL8367RB_Firmware")
            subprocess.run(f"chmod 777 -R {self.common.code_path}/../../vendor/Realtek_RTL8367RB_Firmware/*", shell=True)
            subprocess.run(f"ls -l .", shell=True)
            subprocess.run(f"{self.common.code_path}/../../vendor/Realtek_RTL8367RB_Firmware/build64.sh", shell=True)

            os.chdir(f"{self.common.code_path}/../../vendor/HUYANG010_HY010-4304-0001")
            subprocess.run(f"chmod 777 -R {self.common.code_path}/../../vendor/HUYANG010_HY010-4304-0001/*", shell=True)
            subprocess.run(f"ls -l .", shell=True)
            subprocess.run(f"{self.common.code_path}/../../vendor/HUYANG010_HY010-4304-0001/build64.sh", shell=True)

            os.chdir(f"{self.common.code_path}/../../vendor/REALTEK-DRV-LSW-8363")
            subprocess.run(f"chmod 777 -R {self.common.code_path}/../../vendor/REALTEK-DRV-LSW-8363/*", shell=True)
            subprocess.run(f"ls -l .", shell=True)
            subprocess.run(f"{self.common.code_path}/../../vendor/REALTEK-DRV-LSW-8363/build64.sh", shell=True)

        if self.common.security_enhanced_compatible_board_v2 == 0:
            self.add_weakdictionary()

        self.get_sdk()

        self.build_storagecore()

        self.move_vpp()


# 提供脚本单独调试功能，在application/build目录下运行python3 works/xxxx.py
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="build target")
    parser.add_argument("-b", "--board_name", help="Board list must in '' and separated by space, default value is 2488hv6", default="2488hv6")
    args = parser.parse_args()
    cfg = Config(args.board_name)
    wk = WorkBuildAppPrepare(cfg)
    wk.run()
