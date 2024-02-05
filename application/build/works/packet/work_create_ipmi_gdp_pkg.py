#!/usr/bin/env python
# coding: utf-8
"""
功 能：work_create_ipmi_gdp_pkg.py脚本 
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

class Ipmi(Work):
    def __init__(self, config, work_name=""):
        super(Ipmi, self).__init__(config)
        self.config = config
        self.archive_conf_dic = self.common.get_archive_config(self.config.board_name)
        self.release_dir = f"{self.config.board_name}_ipmi_release"
        self.resource_dir = f"{self.common.code_root}/application/src/resource"
        self.archive_emmc_nand_dir = f"{self.resource_dir}/board/{self.config.board_name}/archive_emmc_nand"
        self.sw_code = self.archive_conf_dic['togdp_dir'].split('/')[-1]
    
    def rename(self, dir, name):
        # 更新修改脚本名称
        os.chdir(dir)
        files = os.listdir(dir)
        for f in files:
            if self.archive_conf_dic['togdp_0502'] in f:
                new_file = f.replace(self.archive_conf_dic['togdp_0502'], name)
                os.rename(f, new_file)
        self.run_command("chmod +x *.sh", ignore_error=True)

    def run(self):
        irm_config_board_name = ("RM210", "RM210_SECURITY_3V0", "RM211")
        # 不是RM210/RM211 ，不需要做打包
        if self.config.board_name not in irm_config_board_name:
            return
        self.logger.info(f"create compressed packet for {self.config.board_name} of  IPMI GDP PKG ------------> [begin]")
        # 创建临时目录
        os.chdir(self.config.build_path)
        self.common.remove_path(self.release_dir)
        self.common.check_path(self.release_dir)

        # copy archive 下的文件
        copy_file_list = self.common.py_find_dir(".*version\.ini", self.archive_emmc_nand_dir)
        for copy_file in copy_file_list:
            shutil.copy(copy_file, self.release_dir)
        
        if os.path.exists(f"{self.release_dir}/{self.archive_conf_dic['delivery_0502_name']}_version.ini"):
            os.remove(f"{self.release_dir}/{self.archive_conf_dic['delivery_0502_name']}_version.ini")
        
        os.chdir(self.release_dir)
        # 更新version.ini文件；
        ar_ver_file = f"{self.sw_code}_version.ini"
        if not os.path.exists(ar_ver_file):
            self.logger.error("can not find version profile")
            self.common.remove_path(f"{self.config.build_path}/{self.release_dir}")
            return
        # 提取xml中的版本号
        ver_num = self.common.extract_ver_num(ar_ver_file, self.config.board_name)
        os.chdir(self.config.build_path)
        # 拷贝文件
        target_enable_pkg = f"{self.archive_conf_dic['togdp_name_ipmi_enable']}{ver_num}"
        target_disable_pkg = f"{self.archive_conf_dic['togdp_name_ipmi_disable']}{ver_num}"
        self.common.check_path(target_enable_pkg)
        self.common.check_path(target_disable_pkg)
        self.common.copy_all(self.release_dir, target_enable_pkg)
        self.common.copy_all(self.release_dir, target_disable_pkg)
        shutil.copy(f"{self.archive_emmc_nand_dir}/ipmi_enable.sh",
                f"{target_enable_pkg}/{self.archive_conf_dic['togdp_ipmi_enable_0502']}_load.sh")
        shutil.copy(f"{self.archive_emmc_nand_dir}/ipmi_disable.sh",
                f"{target_disable_pkg}/{self.archive_conf_dic['togdp_ipmi_disable_0502']}_load.sh")

        self.rename(f"{self.config.build_path}/{target_enable_pkg}", self.archive_conf_dic['togdp_ipmi_enable_0502'])
        self.rename(f"{self.config.build_path}/{target_disable_pkg}", self.archive_conf_dic['togdp_ipmi_disable_0502'])

        os.chdir(self.config.build_path)
        os.chdir(target_enable_pkg)
        self.common.create_sign_filelist(self.config.board_name, "IPMI_ENABLE")

        os.chdir(self.config.build_path)
        os.chdir(target_disable_pkg)
        self.common.create_sign_filelist(self.config.board_name, "IPMI_DISABLE")


        os.chdir(self.config.build_path)
        self.run_command(f'zip -rq "{target_enable_pkg}".zip {target_enable_pkg}')
        self.run_command(f'zip -rq "{target_disable_pkg}".zip {target_disable_pkg}')

        self.common.remove_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_ipmi_enable']}")
        self.common.check_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_ipmi_enable']}")
        self.common.remove_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_ipmi_disable']}")
        self.common.check_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_ipmi_disable']}")

        self.logger.info(f"move file to  '{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_ipmi_enable']}'")
        self.logger.info(f"move file to  '{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_ipmi_disable']}'")
        shutil.move(f"{self.config.build_path}/{target_enable_pkg}.zip",
                    f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_ipmi_enable']}")
        shutil.move(f"{self.config.build_path}/{target_disable_pkg}.zip",
                    f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_ipmi_disable']}")
        self.common.remove_path(f"{self.config.build_path}/{target_enable_pkg}")
        self.common.remove_path(f"{self.config.build_path}/{target_disable_pkg}")
        self.common.remove_path(f"{self.config.build_path}/{self.release_dir}")

        self.logger.info(f"create compressed packet for {self.config.board_name} of IPMI GDP PKG ------------> [end]")


if __name__ == "__main__":
    cfg = Config("RM210", "target_personal")
    wk = Ipmi(cfg)
    wk.run()