#!/usr/bin/env python
# coding: utf-8

"""
功 能：build_java脚本，该脚本构建kvm_client
版权信息：华为技术有限公司，版本所有(C) 2019-2020
"""

import shutil
import subprocess
import sys
import os
import re
import random
import time

cwd=os.path.dirname(__file__)
sys.path.append(f"{cwd}/../")
sys.path.append(f"{cwd}/../../")
from works.work import Work
from utils.common import Common
from utils.config import Config

from utils.error_info import EXCEPTION_STAGE
from utils.error_info import EXCEPTION_RESPONSE
from utils.error_info import EXCEPTION_CODE

class WorkBuildJava(Work):
    timeout = 600
    def get_compatible_board_flag(self):
        flag = False
        file = os.path.join(self.common.code_path, "compatible_board.list")
        if os.path.exists(file):
            with open(rf'{file}') as somefile:
                if len(re.findall(self.config.board_name, somefile.read())) > 0:
                    flag = True
        return flag


    def get_security_board_flag(self):
        flag = False
        file = os.path.join(self.common.code_path, "security_enhanced_compatible_board_list")
        if os.path.exists(file):
            with open(rf'{file}') as somefile:
                if len(re.findall(self.config.board_name, somefile.read())) > 0:
                    flag = True
        return flag


    def check_maven(self):
        if os.environ.get("MAVEN_HOME") not in os.environ.get("PATH"):
            err_msg = f"{EXCEPTION_CODE[403]}{EXCEPTION_STAGE['D']}{EXCEPTION_RESPONSE[2]}"
            self.logger.error(f"{err_msg}there is no MAVEN_HOME in PATH, please install it!")
            os._exit(1)

    def set_evn(self):
        self.check_maven()
        self.kvm_src_dest_dir = os.path.join(self.common.temp_path, 'kvm_client')
        self.resource_dir = os.path.join(self.common.code_root, 'application/src/resource')
        self.jar_dir = self.common.kvm_jar_path
        self.common.remove_path(self.kvm_src_dest_dir)
        kvm_src_source_dir = os.path.join(self.common.code_root, 'application/src/user_interface/kvm_client')
        self.common.copy_all(kvm_src_source_dir, self.kvm_src_dest_dir)

        os.chdir(self.kvm_src_dest_dir)
        if self.config.board_name == '':
            self.common.copy("BuildMaterials/FuncSwitch_on.java", "KVM/src/com/kvm/FuncSwitch.java")
            self.common.copy("BuildMaterials/VmmFuncSwitchOn.java", "KVM/src/com/kvm/VmmFuncSwitch.java")
        else:
            if self.get_compatible_board_flag():
                self.common.copy("BuildMaterials/FuncSwitch_on.java", "KVM/src/com/kvm/FuncSwitch.java")
            else:
                self.common.copy("BuildMaterials/FuncSwitch_off.java", "KVM/src/com/kvm/FuncSwitch.java")
            if self.get_security_board_flag():
                self.common.copy("BuildMaterials/VmmFuncSwitchOn.java", "KVM/src/com/kvm/VmmFuncSwitch.java")
            else:
                self.common.copy("BuildMaterials/VmmFuncSwitchOff.java", "KVM/src/com/kvm/VmmFuncSwitch.java")

    def build_web(self):
        self.logger.info("build vconsole_web.jar begin...")
        os.chdir(self.kvm_src_dest_dir)
        log_name = f"{self.common.temp_path}/work_build_java.log"

        self.logger.info(f"build java start")
        self.logger.info("mvn clean----------------")
        self.run_command("mvn clean")
        self.logger.info("mvn install----------------")
        self.run_command("mvn install")
        self.logger.info("mvn package----------------")
        self.run_command("mvn clean package")
        self.logger.info(f"build java successfully")

    def release(self):
        self.logger.info("release begin...")
        self.common.remove_path(self.jar_dir)
        os.mkdir(self.jar_dir)
        self.common.copy(os.path.join(self.kvm_src_dest_dir, 'KVM/target/vconsole_web.jar'),
                    os.path.join(self.jar_dir, 'vconsole.jar'))
        self.common.copy(os.path.join(self.kvm_src_dest_dir, 'ScreenKinescopePlayer/target/videoplayer.jar'),
                    os.path.join(self.jar_dir, 'videoplayer.jar'))
        self.logger.info("release end!")

    def repack_jar(self, jar_file, sign_dir=""):
        self.logger.info("repack jar begin...")
        os.chdir(self.jar_dir)
        os.mkdir("pack")
        shutil.move(jar_file, f"pack/{jar_file}")
        os.chdir("pack")
        self.run_command(f"jar -xvf {jar_file}")
        os.remove(jar_file)

        if os.path.isdir(sign_dir):
            shutil.rmtree("META-INF")
            self.common.copy_all(os.path.join(sign_dir, "META-INF"), f"{self.jar_dir}/pack/META-INF")

        self.run_command(f"jar -cvfm {jar_file} META-INF/MANIFEST.MF -C ./ .")
        shutil.move(jar_file, self.jar_dir)
        self.common.remove_path(f"{self.jar_dir}/pack")
        self.logger.info("repack jar end!")

    def online_sign(self, jar_file):
        signed_string = "verified"
        proxies = ["proxy.huawei.com", "proxycn2.huawei.com",
                "proxyhk.huawei.com", "proxyuk.huawei.com",
                "proxysg.huawei.com", "proxyjp.huawei.com"]
        timestamp_servers = ["http://sha256timestamp.ws.symantec.com/sha256/timestamp",
                            "http://timestamp.globalsign.com/?signature=sha2"]
        signed_flag = False
        i = 1
        for proxie in proxies:
            self.logger.info(f"Jar Sign Attempt No.{i}")

            keystore_dir = os.path.join(self.common.tools_path, "release/kvm_client/vconsole_keystore/vconsole_keystore")
            for timestamp in timestamp_servers:
                subprocess.run(f"jarsigner -tsa {timestamp} -keystore {keystore_dir} {self.jar_dir}/{jar_file} \
                vconsole_alias -J-Dhttp.proxyHost={proxie} -J-Dhttp.proxyPort=8080 \
                -storepass Huawei12#$ > /dev/null 2>&1", shell=True)

                self.repack_jar(jar_file)
                os.chdir(self.jar_dir)
                # 验证签名是否成功
                sign_result = subprocess.run(f"jarsigner -verify {jar_file}", shell=True, capture_output=True).stdout.decode().strip('\n')
                if signed_string in sign_result:
                    self.logger.info("Sign successful")
                    signed_flag = True
                    break

            if signed_flag:
                break
            i = i + 1

        if i == 7:
            err_msg = f"{EXCEPTION_CODE[602]}{EXCEPTION_STAGE['D']}{EXCEPTION_RESPONSE[2]}"
            self.logger.error(f"{err_msg}Jar sign failed!")
            os._exit(1)

    def sign(self):
        self.logger.info("sign jar begin...")
        plat_file = f"{self.resource_dir}/pme_profile/platform.xml"
        major_ver = self.common.get_xml_value(plat_file, 'PROPERTY', 'PMEMajorVer', 'VALUE')
        minor_ver = self.common.get_xml_value(plat_file, 'PROPERTY', 'PMEMinorVer', 'VALUE')
        if self.config.board_name != "":
            hpm_file = os.path.join(self.common.code_path, f'../src/resource/board/{self.config.board_name}/archive_emmc_nand/archive.ini')
            tosupport_name_hpm = ""
            if "TOSUPPORT_HPM_NAME" in self.common.get_file_info(hpm_file):
                tosupport_name_hpm = self.common.get_file_info(hpm_file)['TOSUPPORT_HPM_NAME']
            prefix_name = subprocess.run(
                f"ls {self.common.code_root} | grep prefix_{tosupport_name_hpm}", shell=True, capture_output=True).stdout.decode().strip('\n')
            prefix_path = os.path.join(os.path.join(self.common.code_root, prefix_name), "jar_signature")
        else:
            prefix_path = os.path.join(self.common.code_root,
                                        f"prefix_{major_ver}{minor_ver}/jar_signature")

        if os.path.isdir(prefix_path):
            self.logger.info(f"use prefix {prefix_path} to build java")
            os.chdir(prefix_path)
            console_sign_info_dir = os.path.join(prefix_path,
                                                subprocess.run("ls | grep vconsole*", shell=True, capture_output=True).stdout.decode().strip('\n'))
            video_sign_info_dir = os.path.join(prefix_path,
                                            subprocess.run("ls | grep videoplayer*", shell=True, capture_output=True).stdout.decode().strip('\n'))

            self.repack_jar("vconsole.jar", console_sign_info_dir)
            self.repack_jar("videoplayer.jar", video_sign_info_dir)
        else:
            self.online_sign("vconsole.jar")
            self.online_sign("videoplayer.jar")

        self.logger.info("sign jar end!")

    def run(self):
        if not self.config.support_dft_separation and self.config.build_type == "dft_separation":
            return
        self.set_evn()
        self.build_web()
        self.release()
        self.sign()
        if self.config.board_version == "V5":
            randoms = f"{time.time_ns()}"
            vconsole_name = f"vconsole{randoms}.jar"
            videoplayer_name = f"videoplayer{randoms}.jar"
            os.chdir(self.common.kvm_jar_path)
            if not os.path.exists(vconsole_name):
                os.rename("vconsole.jar",vconsole_name)
            if not os.path.exists(videoplayer_name):
                os.rename("videoplayer.jar", videoplayer_name)
        return

# 提供脚本单独调试功能，在application/build目录下运行python3 works/xxxx.py
if __name__ == "__main__":
    # pack_java.py未移植完成，以下代码是为保持shell调用方式功能正确
    board = "2488hv6"
    if len(sys.argv) >= 2:
        board = sys.argv[1]
    cfg = Config(board, "target_personal")
    wk = WorkBuildJava(cfg)
    wk.run()
