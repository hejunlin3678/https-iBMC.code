#!/usr/bin/env python
# coding: utf-8
"""
功 能：work_create_customize_efuse_pkg.py脚本 -- 用于生成天池产品efuse包
版权信息：华为技术有限公司，版本所有(C) 2022
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
from datetime import datetime

class Efusesupport(Work):
    def __init__(self, config, work_name=""):
        super(Efusesupport, self).__init__(config)
        self.config = config
        self.archive_conf_dic = self.common.get_archive_config(self.config.board_name)
        self.release_dir = f"{self.config.board_name}_efuse_release"
        self.resource_dir = f"{self.common.code_root}/application/src/resource"
        self.archive_emmc_nand_dir = f"{self.resource_dir}/board/{self.config.board_name}/archive_emmc_nand"
        self.customize_dir = f"{self.release_dir}/dft_tools/config/Common/customize"
        self.product_arch = self.common.get_product_arch(self.config.board_name)
        self.hpmfwupg_tool_dir = f"{self.release_dir}/dft_tools/hpmfwupg/{self.product_arch}"
        self.function_dir = f"{self.resource_dir}/config/Common/customize/function"
        self.equipment_test = f"{self.resource_dir}/config/Common/equipment_test"

        self.sw_code = self.archive_conf_dic['togdp_dir'].split('/')[-1]
        self.version = self.common.get_board_version(self.config.board_name)
        self.platform_file_dic = {
            "V5": f"{self.resource_dir}/pme_profile/platform.xml",
            "V6": f"{self.resource_dir}/pme_profile/platform_v6.xml"
        }

    def modify_copyright_year(self, file):
        new_year = datetime.now().year
        self.common.py_sed(file, "\d{4}(?=\. All rights reserved)", str(new_year))
        return

    def run(self):     
        self.logger.info(f"create customize efuse packet for {self.config.board_name} PKG ------------> [begin]")
        # 创建临时目录
        os.chdir(self.archive_emmc_nand_dir)

        if self.archive_conf_dic.get("togdp_dir_efuse", "") == "":
            self.logger.info("no TOGDP_DIR_EFUSE config,so needn't pack efuse pkg")
            return

        os.chdir(self.config.build_path)
        self.common.remove_path(self.release_dir)
        self.common.check_path(self.release_dir)
        self.common.get_dft_tools(self.release_dir, self.config.board_name)
        copy_file_list = self.common.py_find_dir(".*version\.ini", self.archive_emmc_nand_dir)
        for copy_file in copy_file_list:
            shutil.copy(copy_file, self.release_dir)
        # 更新权限；
        os.chdir(self.release_dir)
        subprocess.run("LD_PRELOAD= sudo LD_PRELOAD=${LD_PRELOAD} -E chown -R huawei:users *", shell=True)
        # 更新version.ini文件；
        ar_ver_file = f"{self.sw_code}_version.ini"
        if not os.path.exists(ar_ver_file):
            self.logger.error(f"can not find version profile")
            self.common.remove_path(f"{self.config.build_path}/{self.release_dir}")
            return

        self.common.copy(ar_ver_file, "tmp_version.ini")
        # 提取xml文件中的版本号
        ver_num = self.common.extract_ver_num(ar_ver_file, self.config.board_name, readonly=True)
        target_pkg = f"{self.archive_conf_dic['togdp_name_efuse']}{ver_num}"

        os.chdir(self.config.build_path)
        self.common.check_path(f"{self.config.board_name}_efuse_tmp/{target_pkg}")

        # 拷贝脚本及镜像文件到目标目录
        config_file_tuple = (
            f"{self.archive_emmc_nand_dir}/{self.archive_conf_dic['togdp_efuse_0502']}_load.sh",
            f"{self.archive_emmc_nand_dir}/add_temp_user.sh",
            f"{self.customize_dir}/{self.product_arch}/conver",
            f"{self.customize_dir}/driver.tar.gz",
            f"{self.hpmfwupg_tool_dir}/hpmfwupg_v5",
            f"{self.customize_dir}/dftimage_v5.hpm",
            f"{self.function_dir}/file_restore.sh",
            f"{self.archive_emmc_nand_dir}/upgrade_hpm.sh"
        )
        for config_file in config_file_tuple:
            self.common.copy(config_file, f"{self.config.board_name}_efuse_tmp/{target_pkg}/{config_file.rsplit('/')[-1]}")
            if not os.path.exists(f"{self.config.board_name}_efuse_tmp/{target_pkg}/{config_file.rsplit('/')[-1]}"):
                self.logger.info(f"move file failed  {config_file}")

        os.rename(f"{self.config.board_name}_efuse_tmp/{target_pkg}/dftimage_v5.hpm",
                f"{self.config.board_name}_efuse_tmp/{target_pkg}/dftimage.hpm")
        os.rename(f"{self.config.board_name}_efuse_tmp/{target_pkg}/hpmfwupg_v5",
                f"{self.config.board_name}_efuse_tmp/{target_pkg}/hpmfwupg")

        special_board_tuple = ("BM320", "TaiShan2280Mv2", "TaiShan2280v2_1711")
        if self.config.board_name in special_board_tuple:
            self.common.copy(f"{self.archive_emmc_nand_dir}/efuse-crypt-image-partner.hpm",
                        f"{self.config.board_name}_efuse_tmp/{target_pkg}/efuse-crypt-image-partner.hpm")
            self.common.copy(f"{self.resource_dir}/config/Common/customize/signature_file/common_transfer_flie.sh", 
                        f"{self.config.board_name}_efuse_tmp/{target_pkg}/common_transfer_flie.sh")
            self.common.run_command(f"cp {self.equipment_test}/dft_enable.sh {self.config.board_name}_efuse_tmp/{target_pkg}/") 

        special_board_load_BMC = ("TaiShan2280Mv2", "TaiShan2280v2_1711")
        if self.config.board_name in special_board_load_BMC:
            self.common.copy(f"{self.common.temp_path}/release/output/rootfs_{self.config.board_name}.hpm",
                        f"{self.config.board_name}_efuse_tmp/{target_pkg}/image.hpm")
            with open(f"{self.config.board_name}_efuse_tmp/{target_pkg}/temp.sh", 'w') as f:
                f.write('#!/bin/sh\n')
                f.write('file_name=`echo $0 | awk -F \"/\" \'{printf $2}\'`\n')
                f.write('echo \"execute $file_name success\"\n')
                f.write('exit 0\n')
            self.common.copy(f"{self.config.board_name}_efuse_tmp/{target_pkg}/temp.sh", 
                f"{self.config.board_name}_efuse_tmp/{target_pkg}/{self.archive_conf_dic['togdp_efuse_0502']}_config.sh")
            self.common.copy(f"{self.config.board_name}_efuse_tmp/{target_pkg}/temp.sh", 
                f"{self.config.board_name}_efuse_tmp/{target_pkg}/{self.archive_conf_dic['togdp_efuse_0502']}_check.sh")
            self.common.copy(f"{self.config.board_name}_efuse_tmp/{target_pkg}/temp.sh", 
                f"{self.config.board_name}_efuse_tmp/{target_pkg}/{self.archive_conf_dic['togdp_efuse_0502']}_verify.sh")
            self.common.remove_file(f"{self.config.board_name}_efuse_tmp/{target_pkg}/temp.sh")
            
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
            shutil.copy(ar_ver_file, f"{self.config.board_name}_efuse_tmp/{target_pkg}/{self.archive_conf_dic['togdp_efuse_0502']}_version.ini")
            subprocess.run(f"sed -i '/^archive_version/s/{old_archieve_ver}/{major_ver}.{minor_ver}.{release_major_ver}.{release_minor_ver}/g' {self.config.board_name}_efuse_tmp/{target_pkg}/{self.archive_conf_dic['togdp_efuse_0502']}_version.ini", shell=True)
            subprocess.run(f"sed -i '/^ActiveBMC/s/{old_archieve_ver}/{major_ver}.{minor_ver}.{release_major_ver}.{release_minor_ver}/g' {self.config.board_name}_efuse_tmp/{target_pkg}/{self.archive_conf_dic['togdp_efuse_0502']}_version.ini", shell=True)
            subprocess.run(f"sed -i '/^BackupBMC/s/{old_archieve_ver}/{major_ver}.{minor_ver}.{release_major_ver}.{release_minor_ver}/g' {self.config.board_name}_efuse_tmp/{target_pkg}/{self.archive_conf_dic['togdp_efuse_0502']}_version.ini", shell=True)
        
        special_board_need_function = ("BM320")
        if self.config.board_name in special_board_need_function:
            self.modify_copyright_year(f"{self.customize_dir}/function/sys_check.sh")
            self.modify_copyright_year(f"{self.customize_dir}/function/sys_config.sh")
            self.common.copy_all(f"{self.customize_dir}/function", f"{self.config.board_name}_efuse_tmp/{target_pkg}/function")
            if self.config.support_dft_separation :
                self.common.run_command(f"sed -i 's/EQUIPMENT_SEPARATION_SUPPORT_STATE=.*/EQUIPMENT_SEPARATION_SUPPORT_STATE=1/' {self.config.board_name}_efuse_tmp/{target_pkg}/function/common.sh")

            self.common.remove_path(f"{self.config.board_name}_efuse_tmp/{target_pkg}/function/.svn")
            self.run_command(f"chmod +x {self.config.board_name}_efuse_tmp/{target_pkg}/function/*.sh")
            os.chdir(f"{self.config.board_name}_efuse_tmp/{target_pkg}")
            # 解决Bep 比对差异 加入--format=gnu 参数
            self.run_command("tar --format=gnu -czf function.tar.gz function")
            os.chdir(self.config.build_path)
            self.common.remove_path(f"{self.config.board_name}_efuse_tmp/{target_pkg}/function")

            self.common.copy( f"{self.customize_dir}/common_factory_restore.sh",
                        f"{self.config.board_name}_efuse_tmp/{target_pkg}/common_factory_restore.sh")

        os.chdir(f"{self.config.board_name}_efuse_tmp")
        subprocess.run(f"zip -rq {target_pkg}.zip {target_pkg}", shell=True)
        self.logger.info(f"move eco_ft file to  'output/{self.archive_conf_dic['togdp_dir_efuse']}")
        self.common.remove_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_efuse']}")
        self.common.check_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_efuse']}")
        shutil.move(f"{self.config.build_path}/{self.config.board_name}_efuse_tmp/{target_pkg}.zip",
                    f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_efuse']}")
        self.common.remove_path(f"{self.config.build_path}/{target_pkg}")
        self.common.remove_path(f"{self.config.build_path}/{self.release_dir}")
        self.common.remove_path(f"{self.config.build_path}/{self.config.board_name}_efuse_tmp")
        self.common.remove_path(f"{self.config.build_path}/{self.release_dir}")
        self.logger.info(f"create compressed packet for {self.config.board_name} of efuse PKG ------------> [end]")

if __name__ == "__main__":
    cfg = Config("BM320", "target_personal")
    wk = Efusesupport(cfg)
    wk.run()