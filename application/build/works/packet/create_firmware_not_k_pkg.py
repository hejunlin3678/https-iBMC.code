#!/usr/bin/env python
# coding: utf-8
"""
功 create_firmware_not_k_pkg.py脚本 -- 用于生成鲲鹏K与非K需求互转，K转非K
版权信息：华为技术有限公司，版本所有(C) 2023
"""

import os
import shutil
import subprocess
import sys

cwd=os.path.dirname(__file__)
sys.path.append(f"{cwd}/../")
sys.path.append(f"{cwd}/../../")
sys.path.append(f"{cwd}/../../../")
from works.work import Work
from utils.config import Config

class FirmwareNotK(Work):
    def __init__(self, config, work_name=""):
        super(FirmwareNotK, self).__init__(config)
        self.config = config
        self.archive_conf_dic = self.common.get_archive_config(self.config.board_name)
        self.release_dir = f"{self.config.board_name}_hqh_release"
        self.resource_dir = f"{self.common.code_root}/application/src/resource"
        self.archive_emmc_nand_dir = f"{self.resource_dir}/board/{self.config.board_name}/archive_emmc_nand"
        self.customize_dir = f"{self.release_dir}/dft_tools/config/Common/customize"
        self.sw_code = self.archive_conf_dic['togdp_dir'].split('/')[-1]
        self.product_arch = self.common.get_product_arch(self.config.board_name)
        self.hpmfwupg_tool_dir = f"{self.release_dir}/dft_tools/hpmfwupg/{self.product_arch}"

    def run(self):
        # 没有配置 togdp_dir_hqh ，不需要打包
        if self.archive_conf_dic.get("togdp_dir_hqh", "") == "":
            return
        self.logger.info(f"create compressed packet for FirmwareNotK of hqh PKG ------------> [begin]")
        # 创建临时目录
        os.chdir(self.config.build_path)
        self.common.remove_path(self.release_dir)
        self.common.check_path(self.release_dir)
        self.common.get_dft_tools(self.release_dir, self.config.board_name)
        # copy archive 下的文件
        copy_file_list = self.common.py_find_dir(".*version\.ini", self.archive_emmc_nand_dir)
        for copy_file in copy_file_list:
            shutil.copy(copy_file, self.release_dir)

        # 更新权限；
        os.chdir(self.release_dir)
        # 更新version.ini文件；
        ar_ver_file = f"{self.sw_code}_version.ini"
        if not os.path.exists(ar_ver_file):
            self.logger.error("can not find the version profile")
            self.common.remove_path(f"{self.config.build_path}/{self.release_dir}")
            return
        # 提取xml中的版本号
        ver_num = self.common.extract_ver_num(ar_ver_file, self.config.board_name)
        # 生成ToGDP目录下的zip包
        os.chdir(self.config.build_path)
        target_pkg = f"{self.archive_conf_dic['togdp_name_hqh']}{ver_num}"
        self.common.check_path(target_pkg)

        # 拷贝脚本及镜像文件到目标目录
        config_file_tuple = (
            f"{self.archive_emmc_nand_dir}/{self.archive_conf_dic['togdp_hqh_0502']}_load.sh",
            f"{self.archive_emmc_nand_dir}/add_temp_user.sh",
            f"{self.archive_emmc_nand_dir}/upgrade_hpm.sh",
            f"{self.customize_dir}/{self.product_arch}/conver",
            f"{self.customize_dir}/dftimage_v5.hpm",
            f"{self.hpmfwupg_tool_dir}/hpmfwupg_v5",
            f"{self.customize_dir}/driver.tar.gz"
        )
        for config_file in config_file_tuple:
            self.common.copy(config_file, f"{target_pkg}/{config_file.rsplit('/')[-1]}")
        os.rename(f"{target_pkg}/dftimage_v5.hpm", f"{target_pkg}/dftimage.hpm")
        os.rename(f"{target_pkg}/hpmfwupg_v5", f"{target_pkg}/hpmfwupg")

        with open(f"{target_pkg}/hqe.sh", 'w') as f:
            f.write('#!/bin/sh\n')
            f.write('file_name=`echo $0 | awk -F \"/\" \'{printf $2}\'`\n')
            f.write('echo \"execute $file_name success\"\n')
            f.write('exit 0\n')
        self.common.copy(f"{target_pkg}/hqe.sh", 
            f"{target_pkg}/{self.archive_conf_dic['togdp_hqh_0502']}_config.sh")
        self.common.copy(f"{target_pkg}/hqe.sh", 
            f"{target_pkg}/{self.archive_conf_dic['togdp_hqh_0502']}_check.sh")
        self.common.copy(f"{target_pkg}/hqe.sh", 
            f"{target_pkg}/{self.archive_conf_dic['togdp_hqh_0502']}_verify.sh")
        self.common.remove_file(f"{target_pkg}/hqe.sh")
        
        xml_file = f"{self.common.code_root}/application/src/resource/pme_profile/platform_v6.xml"
        sw_code = self.archive_conf_dic['togdp_dir'].split("/")[-1]
        ar_ver_file = f"{self.common.code_root}/application/src/resource/board/{self.config.board_name}/archive_emmc_nand/{sw_code}_version.ini"

        major_ver = self.common.get_xml_value(xml_file, 'PROPERTY', 'PMEMajorVer', 'VALUE')
        minor_ver = self.common.get_xml_value(xml_file, 'PROPERTY', 'PMEMinorVer', 'VALUE').zfill(2)
        release_major_ver = self.common.get_xml_value(xml_file, 'PROPERTY', 'ReleaseMajorVer', 'VALUE').zfill(2)
        release_minor_ver = self.common.get_xml_value(xml_file, 'PROPERTY', 'ReleaseMinorVer', 'VALUE').zfill(2)

        old_archieve_ver = self.common.get_file_info(ar_ver_file)['archive_version']
        active_uboot_ver = self.common.get_file_info(ar_ver_file)['ActiveBMC']
        backup_uboot_ver = self.common.get_file_info(ar_ver_file)['BackupBMC']
        shutil.copy(ar_ver_file, f"{target_pkg}/{self.archive_conf_dic['togdp_hqh_0502']}_version.ini")
        subprocess.run(f"sed -i '/^archive_version/s/{old_archieve_ver}/{major_ver}.{minor_ver}.{release_major_ver}.{release_minor_ver}/g' {target_pkg}/{self.archive_conf_dic['togdp_hqh_0502']}_version.ini", shell=True)
        subprocess.run(f"sed -i '/^ActiveBMC/s/{old_archieve_ver}/{major_ver}.{minor_ver}.{release_major_ver}.{release_minor_ver}/g' {target_pkg}/{self.archive_conf_dic['togdp_hqh_0502']}_version.ini", shell=True)
        subprocess.run(f"sed -i '/^BackupBMC/s/{old_archieve_ver}/{major_ver}.{minor_ver}.{release_major_ver}.{release_minor_ver}/g' {target_pkg}/{self.archive_conf_dic['togdp_hqh_0502']}_version.ini", shell=True)


        os.chdir(self.config.build_path)
        self.run_command(f'zip -rq "{target_pkg}".zip {target_pkg}')
        self.common.remove_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_hqh']}")
        self.common.check_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_hqh']}")
        self.logger.info(f"move file to  '{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_hqh']}'")
        shutil.move(f"{self.config.build_path}/{target_pkg}.zip", f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_hqh']}")
        self.common.remove_path(f"{self.config.build_path}/{target_pkg}")
        self.logger.info(f"create compressed packet for FirmwareNotK of hqh PKG ------------> [end]")


if __name__ == "__main__":
    cfg = Config("1288hv6_2288hv6_5288v6", "target_personal")
    wk = FirmwareNotK(cfg)
    wk.run()