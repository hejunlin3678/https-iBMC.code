#!/usr/bin/env python
# coding: utf-8
"""
功 能：work_create_customize_forward_gdp_pkg.py脚本
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

class Customize_Forward(Work):
    def __init__(self, config, work_name=""):
        super(Customize_Forward, self).__init__(config)
        self.config = config
        self.archive_conf_dic = self.common.get_archive_config(self.config.board_name)
        self.release_dir = f"{self.config.board_name}_customize_forward_release"
        self.resource_dir = f"{self.common.code_root}/application/src/resource"
        self.archive_emmc_nand_dir = f"{self.resource_dir}/board/{self.config.board_name}/archive_emmc_nand"
        self.customize_dir = f"{self.release_dir}/dft_tools/config/Common/customize"
        self.sw_code = self.archive_conf_dic['togdp_dir'].split('/')[-1]
        self.version = self.common.get_board_version(self.config.board_name)
        self.product_arch = self.common.get_product_arch(self.config.board_name)
        self.function_dir = f"{self.customize_dir}/function"
        self.hpmfwupg_tool_dir = f"{self.release_dir}/dft_tools/hpmfwupg/{self.product_arch}"

    def run(self):
        if self.version == "V5":
            return
        # 没有配置 togdp_dir_fcus ，不需要打包
        if self.archive_conf_dic.get("togdp_dir_fcus", "") == "":
            return
        self.logger.info(f"create compressed packet for {self.config.board_name} of Customize_Forward GDP PKG ------------> [begin]")
        # 创建临时目录
        os.chdir(self.config.build_path)
        self.common.remove_path(self.release_dir)
        self.common.check_path(self.release_dir)
        self.common.get_dft_tools(self.release_dir, self.config.board_name)
        # copy archive 下的文件
        copy_file_list = self.common.py_find_dir(".*version\.ini", self.archive_emmc_nand_dir)
        for copy_file in copy_file_list:
            shutil.copy(copy_file, self.release_dir)
        sp_upgrade_name_zip = ""
        files = os.listdir(f"{self.common.temp_path}/spftp/Kunpeng")
        for f in files:
            if "Kunpeng.zip" in f:
                self.logger.info(f"find the forward sp upgrade zip file,the file name is {f}")
                sp_upgrade_name_zip = f
        if sp_upgrade_name_zip == "":
            self.logger.info("there is no sp upgrade zip file in cmc path, please make sure")
            self.common.remove_path(self.release_dir)
            return
        # 更新权限；
        os.chdir(self.release_dir)
        # 更新version.ini文件；
        ar_ver_file = f"{self.sw_code}_version.ini"
        if not os.path.exists(ar_ver_file):
            self.logger.error("can not find version profile")
            self.common.remove_path(f"{self.config.build_path}/{self.release_dir}")
            return
        # 提取xml中的版本号
        ver_num = self.common.extract_ver_num(ar_ver_file, self.config.board_name)
        # 生成ToGDP目录下的zip包
        os.chdir(self.config.build_path)
        target_pkg = f"{self.archive_conf_dic['togdp_name_fcus']}{ver_num}"
        self.common.check_path(target_pkg)

        # 拷贝脚本及镜像文件到目标目录
        config_file_tuple = (
            f"{self.archive_emmc_nand_dir}/{self.archive_conf_dic['togdp_fcus_0502']}_load.sh",
            f"{self.archive_emmc_nand_dir}/add_temp_user.sh",
            f"{self.archive_emmc_nand_dir}/wbd_load.sh",
            f"{self.archive_emmc_nand_dir}/upgrade_hpm.sh",
            f"{self.customize_dir}/{self.product_arch}/conver",
            f"{self.customize_dir}/wbd_image.hpm",
            f"{self.customize_dir}/dftimage_v5.hpm",
            f"{self.hpmfwupg_tool_dir}/hpmfwupg_v5",
            f"{self.function_dir}/file_restore.sh",
            f"{self.customize_dir}/driver.tar.gz",
            f"{self.common.temp_path}/spftp/Kunpeng/{sp_upgrade_name_zip}"
        )
        for config_file in config_file_tuple:
            self.common.copy(config_file, f"{target_pkg}/{config_file.rsplit('/')[-1]}")
        os.rename(f"{target_pkg}/wbd_image.hpm", f"{target_pkg}/image.hpm")
        os.rename(f"{target_pkg}/dftimage_v5.hpm", f"{target_pkg}/dftimage.hpm")
        os.rename(f"{target_pkg}/hpmfwupg_v5", f"{target_pkg}/hpmfwupg")

        special_board_tuple = ("TaiShan2280Mv2", "TaiShan2480v2", "TaiShan2280Ev2", "TaiShan5290v2")
        if self.config.board_name in special_board_tuple:
            self.common.copy(f"{self.customize_dir}/wbd_verify.hpm", f"{target_pkg}/wbd_verify.hpm")
        # ccus包
        if self.archive_conf_dic.get("togdp_dir_ccus", "") == "":
            self.logger.info("no TOGDP_DIR_CCUS config,so needn't pack common custom forward pkg")
        else:
            ccus_target_pkg = f"{self.archive_conf_dic['togdp_name_ccus']}{ver_num}"
            self.common.check_path(ccus_target_pkg)
            self.common.copy_all(target_pkg, ccus_target_pkg)
            self.common.remove_file(f"{ccus_target_pkg}/{self.archive_conf_dic['togdp_fcus_0502']}_load.sh")
            self.common.copy(f"{self.customize_dir}/unify_forward_config.sh",
                f"{ccus_target_pkg}/{self.archive_conf_dic['togdp_ccus_0502']}_load.sh")
            self.common.copy(f"{self.archive_emmc_nand_dir}/unify_custom_config.sh",
                f"{ccus_target_pkg}/unify_custom_config.sh")
            self.common.copy(f"{self.archive_emmc_nand_dir}/efuse-crypt-image-partner.hpm", 
                f"{ccus_target_pkg}/efuse-crypt-image-partner.hpm")

        if self.config.support_dft_separation:
            self.run_command(f"sed -i 's/EQUIPMENT_SEPARATION_SUPPORT_STATE=.*/EQUIPMENT_SEPARATION_SUPPORT_STATE=1/' {target_pkg}/{self.archive_conf_dic['togdp_fcus_0502']}_load.sh")
            self.common.copy(f"{self.customize_dir}/wbd-clearlog-image.hpm",
                        f"{target_pkg}/wbd-clearlog-image.hpm")
            if self.archive_conf_dic.get("togdp_dir_ccus", "") != "":
                self.run_command(f"sed -i 's/EQUIPMENT_SEPARATION_SUPPORT_STATE=.*/EQUIPMENT_SEPARATION_SUPPORT_STATE=1/' {ccus_target_pkg}/{self.archive_conf_dic['togdp_ccus_0502']}_load.sh")
                self.common.copy(f"{self.customize_dir}/wbd-clearlog-image.hpm", f"{ccus_target_pkg}/wbd-clearlog-image.hpm")

        special_board_tuple = ("TaiShan2280Mv2", "TaiShan2480v2", "TaiShan2280Ev2", "TaiShan5290v2", "BM320")
        if self.config.board_name in special_board_tuple:
            self.common.copy(f"{self.archive_emmc_nand_dir}/efuse-crypt-image-partner.hpm", 
                        f"{target_pkg}/efuse-crypt-image-partner.hpm")
            self.common.copy(f"{self.resource_dir}/config/Common/customize/signature_file/common_transfer_flie.sh", 
                        f"{target_pkg}/common_transfer_flie.sh")
            self.common.copy(f"{self.resource_dir}/config/Common/customize/signature_file/unify_pcert_load.sh", 
                        f"{target_pkg}/pcert_load.sh")
            self.common.copy(f"{self.customize_dir}/partner_pcert_fvbv_s1.bin", 
                        f"{target_pkg}/partner_pcert_fvbv_s1.bin")
            self.common.copy(f"{self.customize_dir}/partner_pcert_fvbv_s2.bin", 
                        f"{target_pkg}/partner_pcert_fvbv_s2.bin")

        os.chdir(self.config.build_path)
        self.run_command(f'zip -rq "{target_pkg}".zip {target_pkg}')
        self.common.remove_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_fcus']}")
        self.common.check_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_fcus']}")
        self.logger.info(f"move file to  '{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_fcus']}'")
        shutil.move(f"{self.config.build_path}/{target_pkg}.zip", f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_fcus']}")
        self.common.remove_path(f"{self.config.build_path}/{target_pkg}")

        if self.archive_conf_dic.get("togdp_dir_ccus", "") != "":
            self.common.remove_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_ccus']}")
            self.common.check_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_ccus']}")
            self.run_command(f'zip -rq "{ccus_target_pkg}".zip {ccus_target_pkg}')
            self.logger.info(f"move file to  '{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_ccus']}'")
            shutil.move(f"{self.config.build_path}/{ccus_target_pkg}.zip",
                    f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_ccus']}")
            self.common.remove_path(f"{self.config.build_path}/{ccus_target_pkg}")
        self.common.remove_path(f"{self.config.build_path}/{self.release_dir}")
        self.logger.info(f"create compressed packet for {self.config.board_name} of Customize_Forward GDP PKG ------------> [end]")


if __name__ == "__main__":
    cfg = Config("TaiShan2480v2", "target_personal")
    wk = Customize_Forward(cfg)
    wk.run()