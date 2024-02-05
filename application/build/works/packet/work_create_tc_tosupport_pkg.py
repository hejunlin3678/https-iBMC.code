#!/usr/bin/env python
# coding: utf-8
"""
功 能：work_create_tc_tosupport_pkg.py脚本 -- 用于生成天池版本青竹tosupport包
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

class TosupportTC(Work):
    def __init__(self, config, work_name=""):
        super(TosupportTC, self).__init__(config)
        self.config = config
        self.archive_conf_dic = self.common.get_archive_config(self.config.board_name)
        self.release_dir = f"{self.config.board_name}_tosupport_tc_release"
        self.resource_dir = f"{self.common.code_root}/application/src/resource"
        self.archive_emmc_nand_dir = f"{self.resource_dir}/board/{self.config.board_name}/archive_emmc_nand"
        self.sw_code = self.archive_conf_dic['togdp_dir'].split('/')[-1]
        self.version = self.common.get_board_version(self.config.board_name)
        self.platform_file_dic = {
            "V5": f"{self.resource_dir}/pme_profile/platform.xml",
            "V6": f"{self.resource_dir}/pme_profile/platform_v6.xml"
        }

    def s920_function(self, ver_num):
        # 拷贝S920XXX
        os.chdir(f"{self.config.build_path}/{self.release_dir}")
        # 制作ToSupport目录下的文件
        self.logger.info(f"create compressed packet for {self.config.board_name} of  s920 QZ ToSupport ------------> [begin]")
        self.common.remove_file("version_s903.xml")
        self.common.copy(f"{self.resource_dir}/board/{self.config.board_name}/version_s903.xml", "version_s903.xml")
        # 更新version.xml文件中的版本信息和产品信息，升级包名称
        self.common.copy("version_s903.xml", "tmp_version.xml")
        # xml 中 Version字段需要带点号替换
        xml_ver_num = self.common.ver_num_assemb(self.platform_file_dic.get(self.version))
        self.common.py_sed("tmp_version.xml", "TMP_VERSION", str(xml_ver_num))
        self.common.py_sed("tmp_version.xml", "TMP_SUPPORTMODEL", self.archive_conf_dic['tosupport_qz_model_name_s920'])

        hpm_file_name = f"{self.archive_conf_dic['tosupport_qz_hpm_name_s920']}_{ver_num}{self.archive_conf_dic['tosupport_supply_name']}.hpm"
        shutil.move("image.hpm", hpm_file_name)
        self.run_command(f'sed -i "s/image.hpm/{hpm_file_name}/g" tmp_version.xml')
        self.run_command(f'sed -i "s/iBMC_Firmware/{self.archive_conf_dic["tosupport_qz_hpm_name_s920"]}_{ver_num}{self.archive_conf_dic["tosupport_supply_name"]}/g" tmp_version.xml')

        hpm_size = os.path.getsize(hpm_file_name)
        self.common.run_command(f'sed -i "s/TMP_SIZE/{hpm_size}/g" tmp_version.xml')
        shutil.move("tmp_version.xml", "version.xml")
        # 生成ToSupport目录下的zip包
        target_pkg = f"{self.archive_conf_dic['tosupport_qz_name_s920']}_{ver_num}{self.archive_conf_dic['tosupport_supply_name']}"

        self.run_command(f'zip -rq "{target_pkg}".zip {hpm_file_name} version.xml "Open Source Software Notice.doc"')
        self.common.check_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_dir']}")
        if os.path.exists(f"{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_dir']}/{target_pkg}.zip"):
            os.remove(f"{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_dir']}/{target_pkg}.zip")
        self.logger.info(f"move file to {target_pkg}.zip '{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_dir']}'")
        shutil.move(f"{self.config.build_path}/{self.release_dir}/{target_pkg}.zip",
            f"{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_dir']}")
        self.logger.info(f"create compressed packet for {self.config.board_name} of  s920 QZ ToSupport ------------> [end]")
        return

    def run(self):
        if self.archive_conf_dic.get("tosupport_qz_name_s920", "") == "":
            return 

        self.logger.info(f"create compressed packet for {self.config.board_name} of QZ Tosupport PKG ------------> [begin]")
        # 创建临时目录
        os.chdir(self.config.build_path)
        self.common.remove_path(self.release_dir)
        self.common.check_path(self.release_dir)

        if self.config.support_dft_separation:
            self.common.copy(f"{self.common.work_out}/rootfs_{self.config.board_name}_release.hpm", 
            f"{self.release_dir}/image.hpm")
        else :
            self.common.copy(f"{self.common.work_out}/rootfs_{self.config.board_name}.hpm", f"{self.release_dir}/image.hpm")

        os.chdir(self.release_dir)
        # 更新version.ini文件；
        ar_ver_file = f"{self.archive_emmc_nand_dir}/{self.sw_code}_version.ini"
        if not os.path.exists(ar_ver_file):
            self.logger.error("can not find version profile")
            self.common.remove_path(f"{self.config.build_path}/{self.release_dir}")
            return
        # 提取xml中的版本号
        ver_num = self.common.extract_ver_num(ar_ver_file, self.config.board_name, readonly=True)
     
        self.s920_function(ver_num)
        self.logger.info(f"create compressed packet for {self.config.board_name} of QZ ToSupport ------------> [end]")

if __name__ == "__main__":
    cfg = Config("1288hv6_2288hv6_5288v6", "target_personal")
    cfg.set_build_type("release")
    wk = TosupportTC(cfg)
    wk.run()