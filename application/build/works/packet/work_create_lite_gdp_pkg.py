#!/usr/bin/env python
# coding: utf-8
"""
功 work_create_lite_gdp_pkg.py脚本
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

class Lite(Work):
    def __init__(self, config, work_name=""):
        super(Lite, self).__init__(config)
        self.config = config
        self.archive_conf_dic = self.common.get_archive_config(self.config.board_name)
        self.release_dir = f"{self.config.board_name}_lite_release"
        self.resource_dir = f"{self.common.code_root}/application/src/resource"
        self.archive_emmc_nand_dir = f"{self.resource_dir}/board/{self.config.board_name}/archive_emmc_nand"
        self.sw_code = self.archive_conf_dic['togdp_dir'].split('/')[-1]

    def run(self):

        # 没有配置 TOGDP_PATH_LITE ，不需要做精简打包
        if self.archive_conf_dic.get("togdp_dir_lite", "") == "":
            return

        self.logger.info(f"create compressed packet for {self.config.board_name} of  LITE GDP PKG ------------> [begin]")
        # 创建临时目录
        os.chdir(self.config.build_path)
        self.common.remove_path(self.release_dir)
        self.common.check_path(self.release_dir)

        # copy archive 下的文件
        copy_file_list = self.common.py_find_dir(".*load\.sh", self.archive_emmc_nand_dir)
        copy_file_list += self.common.py_find_dir(".*check\.sh", self.archive_emmc_nand_dir)
        copy_file_list += self.common.py_find_dir(".*version\.ini", self.archive_emmc_nand_dir)
        copy_file_list += self.common.py_find_dir(".*product_strategy_config\.ini", self.archive_emmc_nand_dir)
        copy_file_list += self.common.py_find_dir(".*restore\.sh", self.archive_emmc_nand_dir)
        copy_file_list += self.common.py_find_dir(".*check\.sh", self.archive_emmc_nand_dir)
        copy_file_list += [f"{self.archive_emmc_nand_dir}/map_config.ini", f"{self.archive_emmc_nand_dir}/920_load_bmc.sh"]
        for copy_file in copy_file_list:
            self.common.copy(copy_file, f"{self.release_dir}/{copy_file.split('/')[-1]}")
        self.common.copy(f"{self.config.work_out}/rootfs_{self.config.board_name}.hpm", f"{self.release_dir}/image.hpm")
        self.common.get_dft_tools(self.release_dir, self.config.board_name)
        os.chdir(self.release_dir)
        # 检查单板是否有专有的工具，如有，将其目录下的文件拷贝过来，覆盖同名文件和目录
        if os.path.isdir(f"{self.resource_dir}/board/{self.config.board_name}/archive/proprietary"):
            self.common.copy_all(f"{self.release_dir}/dft_tools/config/{self.config.board_name}", self.release_dir)

        os.chdir(self.config.build_path)
        if self.config.board_name == "SMM":
            config_list = self.common.py_find_dir(".*config\.sh", self.archive_emmc_nand_dir)
            for f in config_list:
                shutil.copy(f, self.release_dir)
            if os.path.isdir(f"{self.release_dir}/dft_tools/config/{self.config.board_name}/ft_st_tool"):
                self.common.copy_all(f"{self.release_dir}/dft_tools/config/{self.config.board_name}/ft_st_tool", 
                self.release_dir)

        tce_tuple = ("CM221", "CM221S", "CM221")
        if self.config.board_name in tce_tuple:
            if os.path.isdir(f"{self.release_dir}/dft_tools/config/{self.config.board_name}"):
                self.common.copy_all(f"{self.release_dir}/dft_tools/config/{self.config.board_name}", self.release_dir)

        self.common.remove_path(f"{self.release_dir}/dft_tools")
        os.chdir(self.release_dir)
        # 更新version.ini文件；
        ar_ver_file = f"{self.sw_code}_version.ini"
        if not os.path.exists(ar_ver_file):
            self.logger.error(f"can not find version profile")
            self.common.remove_path(f"{self.config.build_path}/{self.release_dir}")
            return
        # 提取xml中的版本号
        ver_num = self.common.extract_ver_num(ar_ver_file, self.config.board_name)
        # 添加执行权限
        self.run_command("chmod +x *.sh", ignore_error=True)

        # 更新修改脚本名称
        files = os.listdir(f"{self.config.build_path}/{self.release_dir}")
        for f in files:
            if self.archive_conf_dic['togdp_0502'] in f:
                new_file = f.replace(self.archive_conf_dic['togdp_0502'], 
                self.archive_conf_dic['togdp_lite_0502'])
                os.rename(f, new_file)

        os.chdir(self.config.build_path)
        target_pkg = f"{self.archive_conf_dic['togdp_name_lite']}{ver_num}"
        os.rename(self.release_dir, target_pkg)
        self.run_command(f'zip -rq "{target_pkg}".zip {target_pkg}')

        self.common.remove_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_lite']}")
        self.common.check_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_lite']}")

        self.logger.info(f"move file to  '{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_lite']}'")
        shutil.move(f"{self.config.build_path}/{target_pkg}.zip",
                    f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_lite']}")

        self.common.remove_path(f"{self.config.build_path}/{target_pkg}")

        self.logger.info(f"create compressed packet for {self.config.board_name} of LITE GDP PKG ------------> [end]")


if __name__ == "__main__":
    cfg = Config("RM110", "target_personal")
    wk = Lite(cfg)
    wk.run()