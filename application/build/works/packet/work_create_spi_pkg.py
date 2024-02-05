#!/usr/bin/env python
# coding: utf-8
"""
功 能：work_create_spi_pkg.py脚本
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

class Spi(Work):
    def __init__(self, config, work_name=""):
        super(Spi, self).__init__(config)
        self.config = config
        self.archive_conf_dic = self.common.get_archive_config(self.config.board_name)
        self.release_dir = f"{self.config.board_name}_separate_spi_release"
        self.resource_dir = f"{self.common.code_root}/application/src/resource"
        self.archive_emmc_nand_dir = f"{self.resource_dir}/board/{self.config.board_name}/archive_emmc_nand"
        self.archive_uboot = f"{self.resource_dir}/board/{self.config.board_name}/archive_uboot"

    def run(self):
        if os.path.exists(self.archive_uboot):
            self.sw_code = self.archive_conf_dic['togdp_uboot_dir'].split('/')[-1]
            self.logger.info(f"create compressed packet for {self.config.board_name} of SPI GDP PKG ------------> [begin]")
            # 创建临时目录
            os.chdir(self.config.build_path)
            self.common.remove_path(self.release_dir)
            self.common.check_path(self.release_dir)
            self.common.copy_all(self.archive_uboot, self.release_dir)
            self.common.copy(f"{self.config.work_out}/{self.config.board_name}_uboot.bin", f"{self.release_dir}/u-boot.bin")

            os.chdir(self.release_dir)
            # 更新version.ini文件；
            ar_ver_file = f"{self.sw_code}_version.ini"
            if not os.path.exists(ar_ver_file):
                self.logger.error("can not find version profile")
                self.common.remove_path(f"{self.config.build_path}/{self.release_dir}")
                return
            self.common.copy(ar_ver_file, "tmp_version.ini")
            os.rename(ar_ver_file, "tmp_version.ini")
            old_archieve_ver = self.common.get_file_info("tmp_version.ini")['archive_version']
            active_uboot_ver = self.common.get_file_info("tmp_version.ini")['ActiveUboot']
            backup_uboot_ver = self.common.get_file_info("tmp_version.ini")['BackupUboot']
            # 临时处理，后续由工具从uboot.bin文件获取版本后填充
            self.common.run_command('strings u-boot.bin | grep "^U-Boot [0-9]" |head -1 > uboot_tmp.txt')
            uboot_ver = self.common.run_command("cat uboot_tmp.txt | awk -F ' ' '{print $2}'").stdout.decode().strip('\n')
            cmd = "echo %s |awk -F '.' '{print $1$2$3}'"%(uboot_ver)
            archieve_uboot_ver = self.common.run_command(cmd).stdout.decode().strip('\n')
            os.remove("uboot_tmp.txt")
            self.common.run_command(
                f'sed -i "/^archive_version/s/{old_archieve_ver}$/{archieve_uboot_ver}/g" tmp_version.ini')
            self.common.run_command(
                f'sed -i "/^ActiveUboot/s/{active_uboot_ver}$/{uboot_ver}/g" tmp_version.ini')
            self.common.run_command(
                f'sed -i "/^BackupUboot/s/{backup_uboot_ver}$/{uboot_ver}/g" tmp_version.ini')

            os.remove("archive.ini")
            os.rename("tmp_version.ini", ar_ver_file)
            # 更新权限；
            os.chdir(self.config.build_path)
            target_pkg = f"{self.archive_conf_dic['togdp_uboot_name']}{archieve_uboot_ver}"

            os.rename(self.release_dir, target_pkg)
            self.run_command(f'zip -rq "{target_pkg}".zip {target_pkg}')

            os.chdir(self.config.work_out)
            self.common.remove_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_uboot_dir']}")
            self.common.check_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_uboot_dir']}")

            self.logger.info(f"move file to  '{self.common.work_out}/packet/{self.archive_conf_dic['togdp_uboot_dir']}'")
            shutil.move(f"{self.config.build_path}/{target_pkg}.zip",
                        f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_uboot_dir']}")

            self.common.remove_path(f"{self.config.build_path}/{target_pkg}")

            self.logger.info(f"create compressed packet for {self.config.board_name} of SPI GDP PKG ------------> [end]")


if __name__ == "__main__":
    cfg = Config("2288hv5", "target_personal")
    wk = Spi(cfg)
    wk.run()