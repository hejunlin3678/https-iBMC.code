#!/usr/bin/env python
# coding: utf-8
"""
功 能：work_create_liquid_cooling_gdp_pkg.py脚本，该脚本 液冷打包
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

class Liquid_Cooling(Work):
    def __init__(self, config, work_name=""):
        super(Liquid_Cooling, self).__init__(config)
        self.config = config
        self.archive_conf_dic = self.common.get_archive_config(self.config.board_name)
        self.release_dir = f"{self.config.board_name}_liquid_release"
        self.resource_dir = f"{self.common.code_root}/application/src/resource"
        self.archive_emmc_nand_dir = f"{self.resource_dir}/board/{self.config.board_name}/archive_emmc_nand"
        self.customize_dir = f"{self.resource_dir}/config/Common/customize"
        self.sw_code = self.archive_conf_dic['togdp_dir'].split('/')[-1]

    def run(self):

        # 没有配置 TOGDP_PATH_LIQUID_COOLING ，不需要做液冷打包
        if self.archive_conf_dic.get("togdp_dir_liquid_cooling", "") == "":
            return

        self.logger.info(f"create compressed packet for {self.config.board_name} of  LIQUID_COOLING GDP PKG ------------> [begin]")
        # 创建临时目录
        os.chdir(self.config.build_path)
        self.common.remove_path(self.release_dir)
        self.common.check_path(self.release_dir)

        # copy archive 下的文件
        copy_file_list = self.common.py_find_dir(".*version\.ini", self.archive_emmc_nand_dir)
        for copy_file in copy_file_list:
            shutil.copy(copy_file, self.release_dir)
        
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
        shutil.copy(f"{self.customize_dir}/liquid_cooling.sh",
                    f"{self.release_dir}/{self.archive_conf_dic['togdp_liquid_cooling_0502']}_config.sh")
        shutil.copy(f"{self.customize_dir}/liquid_check.sh",
                    f"{self.release_dir}/{self.archive_conf_dic['togdp_liquid_cooling_0502']}_check.sh")

        # 添加执行权限
        os.chdir(self.release_dir)
        self.run_command("chmod +x *.sh", ignore_error=True)

        # 更新修改脚本名称
        files = os.listdir(f"{self.config.build_path}/{self.release_dir}")
        for f in files:
            if self.archive_conf_dic['togdp_0502'] in f:
                new_file = f.replace(self.archive_conf_dic['togdp_0502'], 
                self.archive_conf_dic['togdp_liquid_cooling_0502'])
                os.rename(f, new_file)

        if self.config.support_dft_separation:
            os.remove(f"{self.config.build_path}/{self.release_dir}/{self.archive_conf_dic['delivery_0502_name']}_version.ini")

        os.chdir(self.config.build_path)
        target_pkg = f"{self.archive_conf_dic['togdp_name_liquid_cooling']}{ver_num}"
        os.rename(self.release_dir, target_pkg)
        self.run_command(f'zip -rq "{target_pkg}".zip {target_pkg}')

        self.common.remove_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_liquid_cooling']}")
        self.common.check_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_liquid_cooling']}")

        self.logger.info(f"move file to  '{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_liquid_cooling']}'")
        shutil.move(f"{self.config.build_path}/{target_pkg}.zip",
                    f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_liquid_cooling']}")

        self.common.remove_path(f"{self.config.build_path}/{target_pkg}")

        self.logger.info(f"create compressed packet for {self.config.board_name} of LIQUID_COOLING GDP PKG ------------> [end]")


if __name__ == "__main__":
    cfg = Config("2288hv5", "target_personal")
    wk = Liquid_Cooling(cfg)
    wk.run()