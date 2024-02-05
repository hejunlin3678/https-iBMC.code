#!/usr/bin/env python
# coding=utf-8

"""
文件名：buildhpm_ext4.py
功能：编译生成 hpm 包
输入参数：板名 board_name
调用方法：python3 -B buildhpm_ext4.py -b board_name -l support_list(-l 可选)
版权信息：华为技术有限公司，版本所有(C) 2019-2020
"""

import os
import sys
import time
import argparse
import subprocess
import shutil

cwd=os.path.dirname(__file__)
sys.path.append(f"{cwd}/../")
sys.path.append(f"{cwd}/../../")
sys.path.append(f"{cwd}/../../../")
from works.work import Work
from utils.config import Config
from utils.common import Common

from utils.error_info import EXCEPTION_STAGE
from utils.error_info import EXCEPTION_RESPONSE
from utils.error_info import EXCEPTION_CODE

class WorkBuildhpmExt4(Work):
    def create_empty_text_file(self, file_name):
        with open(file_name, 'w+') as file_fp:
            file_fp.close

    def exit_with_create_flag(self, board_name, exit_code=0):
        flag_file = f"{self.common.temp_path}/buildhpm_flag_{board_name}"
        if not os.path.isfile(flag_file):
            self.create_empty_text_file(flag_file)
        if exit_code:
            err_msg = f"{EXCEPTION_CODE[600]}{EXCEPTION_STAGE['P']}{EXCEPTION_RESPONSE[1]}"
            self.logger.error(f"{err_msg}create empty text file fail!")
            os._exit(exit_code)

    def check_hpm_size(self, board_name, hpm_name):
        hpm_size = os.stat(hpm_name).st_size
        v5_max_size = 52428800
        board_name_tuple_max = ("S920", "TaiShan2280v2", "S920X05", "TaiShan2280v2_1711", "TaiShan2280Mv2", "S902X20")
        if board_name in board_name_tuple_max:
            v5_max_size = 68157440
        else:
            v5_max_size = 52428800
        if hpm_size >= v5_max_size:
            self.logger.info(f"ERROR: {hpm_name} is too big, size={hpm_size}, bigger than {v5_max_size}, delete it")
            return
            os.remove(hpm_name)
            self.exit_with_create_flag(board_name, 1)
        else:
            self.logger.info(f"now {hpm_name} size={hpm_size}")
        return

    def build_hpm(self, board_name):
        hpm_build_dir = f"{self.config.build_path}/hpm_build"

        platform_file_dic = {
            "V5": f"{self.common.code_root}/application/src/resource/pme_profile/platform_v5.xml",
            "V6": f"{self.common.code_root}/application/src/resource/pme_profile/platform_v6.xml"
        }
        pdt_dir = f"{self.common.code_root}/application/src/resource/board/{board_name}"
        if self.config.build_type == "release":
            hpm_name = f"rootfs_{board_name}.hpm"
        elif self.config.build_type == "debug":
            hpm_name = f"rootfs_rw_{board_name}.hpm"
        elif self.config.build_type == "dft_separation":
            hpm_name = f"rootfs_rw_{board_name}_release.hpm"

        os.chdir(hpm_build_dir)
        self.logger.info(f"Build {board_name} hpm ...")
        if self.common.need_update_version_xml(board_name):
            major_ver = self.common.get_xml_value(platform_file_dic.get(self.config.board_version), 'PROPERTY', 'PMEMajorVer', 'VALUE')
            minor_ver = self.common.get_xml_value(platform_file_dic.get(self.config.board_version), 'PROPERTY', 'PMEMinorVer', 'VALUE').zfill(2)
            release_major_ver = self.common.get_xml_value(platform_file_dic.get(self.config.board_version), 'PROPERTY', 'ReleaseMajorVer', 'VALUE').zfill(2)
            release_minor_ver = self.common.get_xml_value(platform_file_dic.get(self.config.board_version), 'PROPERTY', 'ReleaseMinorVer', 'VALUE').zfill(2)
            dft_minor_ver = str(int(release_minor_ver) + 1).zfill(2)
            release_xml_ver = f"{major_ver}.{minor_ver}.{release_major_ver}.{release_minor_ver}"
            debug_xml_ver = f"{major_ver}.{minor_ver}.{release_major_ver}.{dft_minor_ver}"
            # 生成version_release.xml
            self.logger.info(f"create {board_name} version_release.xml ...")
            self.common.copy(f"{pdt_dir}/version.xml", "tmp_version.xml")
            self.common.py_sed("tmp_version.xml", "TMP_VERSION", str(release_xml_ver))
            shutil.move("tmp_version.xml", f"{self.common.work_out}/version_release.xml")
            # 生成version_debug.xml
            self.logger.info(f"create {board_name} version_debug.xml ...")
            self.common.copy(f"{pdt_dir}/version.xml", "tmp_version.xml")
            self.common.py_sed("tmp_version.xml", "TMP_VERSION", str(debug_xml_ver))
            shutil.move("tmp_version.xml", f"{self.common.work_out}/version_debug.xml")

        self.g_src_resource = f"{self.common.code_root}/application/src/resource"
        dst_xml_file = f"{self.g_src_resource}/pme_profile/platform_v6.xml"
        major_ver = self.common.get_xml_value(dst_xml_file, 'PROPERTY', 'PMEMajorVer', 'VALUE')
        minor_ver = self.common.get_xml_value(dst_xml_file, 'PROPERTY', 'PMEMinorVer', 'VALUE').zfill(2)
        release_major_ver = self.common.get_xml_value(dst_xml_file, 'PROPERTY', 'ReleaseMajorVer', 'VALUE').zfill(2)
        release_minor_ver = self.common.get_xml_value(dst_xml_file, 'PROPERTY', 'ReleaseMinorVer', 'VALUE').zfill(2)
        curr_ver = self.common.get_shell_command_result("cat update.cfg | grep '^Version' | awk -F '=' '{print $2}'")
        dft_minor_ver = str(int(release_minor_ver) + 1).zfill(2)

        if os.path.exists(f"{self.config.work_out}/{board_name}_rootfs.tar.gz") or os.path.exists(f"{self.config.work_out}/{board_name}_gpp.bin"):
            # 可读写包
            if self.config.board_version == "V5":
                subprocess.run(
                    f"sh packethpm_ext4.sh {self.config.work_out}/{board_name}_rootfs.tar.gz hpm_ipmc_ext4.config", shell=True)
            elif self.config.board_version == "V6":
                if self.config.support_dft_separation and self.config.build_type != "dft_separation": 
                    subprocess.run(
                        f"sed -i \"/^Version/s/{curr_ver}/{major_ver}.{minor_ver}.{release_major_ver}.{dft_minor_ver}/g\" update.cfg",
                        shell=True)
                subprocess.run(
                    f"sh packethpm_ext4.sh {self.config.work_out}/{board_name}_gpp.bin hpm_ipmc_ext4.config", shell=True)
            ret = subprocess.run(f"cp ipmc-crypt-image.hpm {self.config.work_out}/{hpm_name}", shell=True)
            if ret.returncode:
                self.logger.info("make ipmcimage.hpm failed")
                self.exit_with_create_flag(board_name, 1)
            if os.path.exists("ipmc-crypt-image.hpm"):
                os.remove("ipmc-crypt-image.hpm")

            if self.config.board_version == "V5":
                self.check_hpm_size(board_name, f"{self.config.work_out}/{hpm_name}")
        
        if self.config.build_type == "dft_separation" and os.path.exists(f"{self.config.work_out}/{board_name}_ro_gpp.bin"):
            subprocess.run(
                f"sh packethpm_ext4.sh {self.config.work_out}/{board_name}_ro_gpp.bin hpm_ipmc_ext4.config", shell=True)
            ret = subprocess.run(f"cp ipmc-crypt-image.hpm {self.config.work_out}/rootfs_{board_name}_release.hpm", shell=True)
            if ret.returncode:
                self.logger.info("make ipmcimage.hpm failed")
                self.exit_with_create_flag(board_name, 1)
            if os.path.exists("ipmc-crypt-image.hpm"):
                os.remove("ipmc-crypt-image.hpm")
            self.sign_filelist(f"rootfs_{board_name}_release.hpm")

        if self.config.cb_flag:
            self.create_cb_hpm(hpm_build_dir)
        if self.config.partner_compile_mode == "repair":
            os.rename(f"{self.config.work_out}/{hpm_name}", f"{self.config.work_out}/rootfs_rw_{board_name}_repair.hpm")
            self.sign_filelist(f"rootfs_rw_{board_name}_repair.hpm")
            self.logger.info("repair hpm build success!!")
            return
        self.logger.info("hpm build success!!")
        self.exit_with_create_flag(board_name)
        self.sign_filelist(hpm_name)
        self.logger.info(f"buildhmp for {self.config.board_name} end !")
        return

    def create_cb_hpm(self, hpm_build_dir):
        os.chdir(hpm_build_dir)
        platform_file_dic = {
            "V5": f"{self.common.code_root}/application/src/resource/pme_profile/platform_v5.xml",
            "V6": f"{self.common.code_root}/application/src/resource/pme_profile/platform_v6.xml"
        }
        pdt_dir = os.path.join(self.common.code_root, f"application/src/resource/board/{self.config.board_name}")
        cb_update_cfgs = self.common.py_find_dir(".*_cb_productid_update_ext4.cfg", pdt_dir)
        for cb_update_cfg in cb_update_cfgs:
            cb_hpm_name = self.common.get_shell_command_result(f"echo '{cb_update_cfg}' | awk -F '/' '{{print $NF}}' | cut -d '_' -f 1")
            self.logger.info(f"{cb_update_cfg}")
            shutil.copy(f"{cb_update_cfg}", f"{hpm_build_dir}/update.cfg")
            curr_ver = self.common.get_shell_command_result(f"cat '{hpm_build_dir}/update.cfg' | grep '^Version' | awk -F '=' '{{print $2}}'").strip('\n')
            ver_num = self.common.ver_num_assemb(platform_file_dic.get(self.config.board_version))
            subprocess.run(f"sed -i \"/^Version/s/{curr_ver}/{ver_num}/g\" update.cfg", shell=True)
        if self.config.build_type == "dft_separation":
            subprocess.run(
                    f"sh packethpm_ext4.sh {self.config.work_out}/CB_FLAG_{self.config.board_name}_gpp_release.bin hpm_ipmc_ext4.config", shell=True)
            shutil.copy("ipmc-crypt-image.hpm", f"{self.config.work_out}/rootfs_cb_flag_{self.config.board_name}_{cb_hpm_name}_release.hpm")
            if os.path.exists("ipmc-crypt-image.hpm"):
                os.remove("ipmc-crypt-image.hpm")

            subprocess.run(
                    f"sh packethpm_ext4.sh {self.config.work_out}/CB_FLAG_{self.config.board_name}_gpp_release_rw.bin hpm_ipmc_ext4.config", shell=True)
            shutil.copy("ipmc-crypt-image.hpm", f"{self.config.work_out}/rootfs_rw_cb_flag_{self.config.board_name}_{cb_hpm_name}_release.hpm")
            if os.path.exists("ipmc-crypt-image.hpm"):
                os.remove("ipmc-crypt-image.hpm")
            self.sign_filelist(f"rootfs_cb_flag_{self.config.board_name}_{cb_hpm_name}_release.hpm")
            self.sign_filelist(f"rootfs_rw_cb_flag_{self.config.board_name}_{cb_hpm_name}_release.hpm")
        else:
            if self.config.board_version == "V5":
                subprocess.run(
                    f"sh packethpm_ext4.sh {self.config.work_out}/CB_FLAG_{self.config.board_name}_rootfs.tar.gz hpm_ipmc_ext4.config", shell=True)
            elif self.config.board_version == "V6":
                subprocess.run(
                    f"sh packethpm_ext4.sh {self.config.work_out}/CB_FLAG_{self.config.board_name}_gpp.bin hpm_ipmc_ext4.config", shell=True)
            if self.config.build_type == "release":
                cb_hpm_name = f"rootfs_cb_flag_{self.config.board_name}_{cb_hpm_name}.hpm"
            elif self.config.build_type == "debug":
                cb_hpm_name = f"rootfs_rw_cb_flag_{self.config.board_name}_{cb_hpm_name}.hpm"
            shutil.copy("ipmc-crypt-image.hpm", f"{self.config.work_out}/{cb_hpm_name}")
            if self.config.board_version == "V5":
                self.check_hpm_size(self.config.board_name, f"{self.config.work_out}/{cb_hpm_name}")
            if os.path.exists("ipmc-crypt-image.hpm"):
                os.remove("ipmc-crypt-image.hpm")
            self.sign_filelist(cb_hpm_name)

    def sign(self, filelist, hpm_file):
        os.chdir(self.config.work_out)
        sha = subprocess.run(f"sha256sum {hpm_file}", shell=True, capture_output=True).stdout.decode().strip('\n').split("  ")[0]
        with open(filelist, 'w') as f:
            f.write('Manifest Version: 1.0\n')
            f.write('Create By: Huawei Technology Inc.\n')
            f.write(f'Name: {hpm_file}\n')
            f.write(f"SHA256-Digest: {sha}\n")


    def sign_filelist(self, hpm_name):
        hpm_file_list = os.path.join(self.config.work_out, f'{hpm_name.split(".")[0]}.filelist')
        self.sign(hpm_file_list, hpm_name)


    def run(self):
        if not self.config.support_dft_separation and self.config.build_type == "dft_separation":
            return
        if os.path.exists(f"{self.config.build_path}/hpm_build"):
            self.build_hpm(self.config.board_name)


if __name__ == "__main__":
    cfg = Config()
    wk = WorkBuildhpmExt4(cfg)
    wk.run()
    print("build hpm end here")
