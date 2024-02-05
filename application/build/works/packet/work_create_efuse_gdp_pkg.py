#!/usr/bin/env python
# coding: utf-8
"""
功 能：work_create_efuse_gdp_pkg.py 脚本
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

class Efuse(Work):
    def __init__(self, config, work_name=""):
        super(Efuse, self).__init__(config)
        self.config = config
        self.archive_conf_dic = self.common.get_archive_config(self.config.board_name)
        self.release_dir = f"{self.config.board_name}_efuse_release"
        self.resource_dir = f"{self.common.code_root}/application/src/resource"
        self.archive_emmc_nand_dir = f"{self.resource_dir}/board/{self.config.board_name}/archive_emmc_nand"
        self.sw_code = self.archive_conf_dic['togdp_dir'].split('/')[-1]
        self.version = self.common.get_board_version(self.config.board_name) 

    def run(self):

        if self.version == "V5":
            return
        # 没有配置 togdp_dir_code ，不需要打包
        if self.archive_conf_dic.get("togdp_dir_code", "") == "":
            return

        self.logger.info(f"create compressed packet for {self.config.board_name} of  EFUSE GDP PKG ------------> [begin]")
        # 创建临时目录
        os.chdir(self.config.build_path)
        self.common.remove_path(self.release_dir)
        self.common.check_path(self.release_dir)
        self.common.get_dft_tools(self.release_dir, self.config.board_name)
        # copy archive 下的文件
        copy_file_list = self.common.py_find_dir(".*version\.ini", self.archive_emmc_nand_dir)
        copy_file_list += self.common.py_find_dir(f"{self.archive_conf_dic['togdp_code_efuse']}.*", self.archive_emmc_nand_dir)
        for copy_file in copy_file_list:
            shutil.copy(copy_file, self.release_dir)
        # 拷贝文件
        if self.config.board_name == "SMM":
            self.common.copy(f"{self.release_dir}/dft_tools/config/CN90SMMA/efuse-crypt-image.hpm",
                      f"{self.release_dir}/image.hpm")
        os.chdir(self.release_dir)
        # 添加执行权限：
        self.run_command("chmod +x *.sh", ignore_error=True)
        # 更新version.ini文件；
        ar_ver_file = f"{self.sw_code}_version.ini"
        if not os.path.exists(ar_ver_file):
            self.logger.error("can not find version profile")
            self.common.remove_path(f"{self.config.build_path}/{self.release_dir}")
            return
        # 提取xml中的版本号
        ver_num = self.common.extract_ver_num(ar_ver_file, self.config.board_name, readonly=True)
        os.chdir(self.config.build_path)
        target_pkg = f"{self.archive_conf_dic['togdp_name_code']}{ver_num}"
        self.common.delete_for_matched(self.release_dir, "version.ini")
        self.common.remove_path(f"{self.release_dir}/dft_tools")
        os.rename(self.release_dir, target_pkg)
        self.run_command(f'zip -rq "{target_pkg}".zip {target_pkg}')

        self.common.remove_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_code']}")
        self.common.check_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_code']}")

        self.logger.info(f"move file to  '{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_code']}'")
        shutil.move(f"{self.config.build_path}/{target_pkg}.zip",
                    f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_code']}")

        self.common.remove_path(f"{self.config.build_path}/{target_pkg}")

        self.logger.info(f"create compressed packet for {self.config.board_name} of EFUSE GDP PKG ------------> [end]")


if __name__ == "__main__":
    cfg = Config("SMM", "target_personal")
    wk = Efuse(cfg)
    wk.run()