#!/usr/bin/env python
# coding: utf-8
"""
功 work_create_tcmib_tosupport_pkg.py脚本 -- 用于生成天池产品tosupport mib包
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

class TC(Work):
    def __init__(self, config, work_name=""):
        super(TC, self).__init__(config)
        self.config = config
        self.archive_conf_dic = self.common.get_archive_config(self.config.board_name)
        self.release_dir = f"{self.config.board_name}_tc_release"
        self.resource_dir = f"{self.common.code_root}/application/src/resource"
        self.archive_emmc_nand_dir = f"{self.resource_dir}/board/{self.config.board_name}/archive_emmc_nand"
        self.sw_code = self.archive_conf_dic['togdp_dir'].split('/')[-1]
        self.version = self.common.get_board_version(self.config.board_name) 

    def run(self):
        # 没有配置 ，不需要做打包
        if self.archive_conf_dic.get("tosupport_qz_name_s920", "") == "":
            return

        self.logger.info(f"create compressed packet for {self.config.board_name} of  QZ MIB TOSUPPORT PKG ------------> [begin]")
        # 创建临时目录
        os.chdir(self.config.build_path)
        self.common.remove_path(self.release_dir)
        self.common.check_path(self.release_dir)
        
        os.chdir(self.release_dir)
        # 更新version.ini文件；
        ar_ver_file = f"{self.archive_emmc_nand_dir}/{self.sw_code}_version.ini"
        if not os.path.exists(ar_ver_file):
            self.logger.error("can not find version profile")
            self.common.remove_path(f"{self.config.build_path}/{self.release_dir}")
            return
        # 提取xml中的版本号
        ver_num = self.common.extract_ver_num(ar_ver_file, self.config.board_name, True)
        os.chdir(self.config.build_path)
        # 拷贝文件
        self.common.copy(f"{self.resource_dir}/config/Common/HUAWEI-SERVER-iBMC-MIB-V6.mib",
                        f"{self.release_dir}/HUAWEI-SERVER-iBMC-MIB.mib")
        s920_target_pkg = f"{self.archive_conf_dic['tosupport_qz_mib_name_s920']}_{ver_num}_MIB{self.archive_conf_dic['tosupport_supply_name']}"
        os.chdir(f"{self.release_dir}")
        self.run_command(f'zip -rq "{s920_target_pkg}".zip HUAWEI-SERVER-iBMC-MIB.mib')
        self.common.check_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_mib_path']}")
        if os.path.exists(f"{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_mib_path']}/{s920_target_pkg}.zip"):
            os.remove(f"{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_mib_path']}/{s920_target_pkg}.zip")
        shutil.move(f"{self.config.build_path}/{self.release_dir}/{s920_target_pkg}.zip",
                f"{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_mib_path']}")

        self.common.remove_path(f"{self.config.build_path}/{self.release_dir}")

        self.logger.info(f"create compressed packet for {self.config.board_name} of QZ MIB TOSUPPORT PKG ------------> [end]")


if __name__ == "__main__":
    cfg = Config("1288hv6_2288hv6_5288v6", "target_personal")
    wk = TC(cfg)
    wk.run()