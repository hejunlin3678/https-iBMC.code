#!/usr/bin/env python
# coding: utf-8
"""
功 能： 用于生成天池产品efuse包，包含伙伴模式和TCM使能
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

class EfusePartnerTcmSupport(Work):
    def __init__(self, config, work_name=""):
        super(EfusePartnerTcmSupport, self).__init__(config)
        self.config = config
        self.archive_conf_dic = self.common.get_archive_config(self.config.board_name)
        self.release_dir = f"{self.config.board_name}_efuse_partner_tcm_release"
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

    def generate_efuse_partner_tcm_hpm(self, pkg_path):
        efuse_path = f"{self.common.code_root}/tools/dev_tools/hpmpack_release/efuse-packet"
        self.run_command(f"sh {efuse_path}/make_efuse_hpm.sh partner_tcm")

        # 比较记录的efuse.bin的hash值，不匹配就终止构建。
        sha256_partner_tcm = f"f4e9e92a408d05985f8a96f3ffd81abab2e5c302d4810874798d51091927980f"
        efuse_bin_hash = subprocess.run(f"sha256sum {efuse_path}/efuse.bin",
            shell=True, capture_output=True).stdout.decode().strip('\n').split(" ")[0]
        if sha256_partner_tcm != efuse_bin_hash:
            self.logger.error(f"Parameter mismatch, efuse_bin_hash={efuse_bin_hash}")
            self.common.remove_file(f"{efuse_path}/efuse-crypt-image.hpm")
            self.common.remove_file(f"{efuse_path}/efuse.bin")
            os._exit(1)

        # 结果处理，移动hpm到目标，删除关键文件
        shutil.copyfile(f"{efuse_path}/efuse-crypt-image.hpm", f"{pkg_path}/efuse-crypt-image-partner-tcm.hpm")
        self.common.remove_file(f"{efuse_path}/efuse-crypt-image.hpm")
        self.common.remove_file(f"{efuse_path}/efuse.bin")

    def modify_copyright_year(self, file):
        new_year = datetime.now().year
        self.common.py_sed(file, "\d{4}(?=\. All rights reserved)", str(new_year))
        return

    def run(self):
        self.logger.info(f"create customize efuse (partner&tcm enable) packet for {self.config.board_name} PKG ------------> [begin]")

        if self.archive_conf_dic.get("togdp_dir_efuse_partner_tcm", "") == "":
            self.logger.info("no TOGDP_PATH_EFUSE_PARTNER_TCM config, so needn't pack efuse pkg")
            return

        os.chdir(self.config.build_path)
        self.common.remove_path(self.release_dir)
        self.common.check_path(self.release_dir)

        self.common.get_dft_tools(self.release_dir, self.config.board_name)
        copy_file_list = self.common.py_find_dir(".*version\.ini", self.archive_emmc_nand_dir)
        for copy_file in copy_file_list:
            shutil.copy(copy_file, self.release_dir)

        os.chdir(self.release_dir)
        subprocess.run("LD_PRELOAD= sudo LD_PRELOAD=${LD_PRELOAD} -E chown -R huawei:users *", shell=True)

        # 提取xml文件中的版本号
        ver_num = self.common.extract_ver_num(None, self.config.board_name, readonly=True)
        target_pkg = f"{self.archive_conf_dic['togdp_name_efuse_partner_tcm']}{ver_num}"

        os.chdir(self.config.build_path)
        pkg_path = f"{self.config.board_name}_efuse_partner_tcm_temp/{target_pkg}"
        self.common.check_path(f"{pkg_path}")

        # 拷贝脚本及镜像文件到目标目录
        config_file_tuple = (
            f"{self.archive_emmc_nand_dir}/{self.archive_conf_dic['togdp_efuse_partner_tcm_0502']}_load.sh",
            f"{self.archive_emmc_nand_dir}/add_temp_user.sh",
            f"{self.customize_dir}/{self.product_arch}/conver",
            f"{self.customize_dir}/driver.tar.gz",
            f"{self.hpmfwupg_tool_dir}/hpmfwupg_v5",
            f"{self.customize_dir}/dftimage_v5.hpm",
            f"{self.function_dir}/file_restore.sh",
            f"{self.customize_dir}/common_factory_restore.sh",
            f"{self.archive_emmc_nand_dir}/upgrade_hpm.sh",
            f"{self.resource_dir}/config/Common/customize/signature_file/common_transfer_flie.sh"
        )
        for config_file in config_file_tuple:
            self.common.copy(config_file, f"{pkg_path}/{config_file.rsplit('/')[-1]}")
            if not os.path.exists(f"{pkg_path}/{config_file.rsplit('/')[-1]}"):
                self.logger.info(f"move file failed {config_file}")

        os.rename(f"{pkg_path}/dftimage_v5.hpm", f"{pkg_path}/dftimage.hpm")
        os.rename(f"{pkg_path}/hpmfwupg_v5", f"{pkg_path}/hpmfwupg")
        self.common.run_command(f"cp {self.equipment_test}/dft_enable.sh {pkg_path}/")

        self.generate_efuse_partner_tcm_hpm(pkg_path)

        self.common.copy_all(f"{self.customize_dir}/function", f"{pkg_path}/function")
        self.modify_copyright_year(f"{pkg_path}/function/sys_check.sh")
        self.modify_copyright_year(f"{pkg_path}/function/sys_config.sh")
        if self.config.support_dft_separation:
            self.common.run_command(f"sed -i 's/EQUIPMENT_SEPARATION_SUPPORT_STATE=.*/EQUIPMENT_SEPARATION_SUPPORT_STATE=1/' {pkg_path}/function/common.sh")
        self.common.remove_path(f"{pkg_path}/function/.svn")
        self.run_command(f"chmod +x {pkg_path}/function/*.sh")
        os.chdir(f"{pkg_path}")
        # 解决Bep 比对差异 加入--format=gnu 参数
        self.run_command("tar --format=gnu -czf function.tar.gz function")
        os.chdir(self.config.build_path)
        self.common.remove_path(f"{pkg_path}/function")

        os.chdir(f"{self.config.board_name}_efuse_partner_tcm_temp")
        subprocess.run(f"zip -rq {target_pkg}.zip {target_pkg}", shell=True)

        self.logger.info(f"move efuse file to {self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_efuse_partner_tcm']}")
        self.common.remove_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_efuse_partner_tcm']}")
        self.common.check_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_efuse_partner_tcm']}")
        shutil.move(f"{self.config.build_path}/{pkg_path}.zip",
                    f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_efuse_partner_tcm']}")

        os.chdir(self.config.build_path)
        self.common.remove_path(f"{self.config.build_path}/{self.release_dir}")
        self.common.remove_path(f"{self.config.build_path}/{self.config.board_name}_efuse_partner_tcm_temp")
        self.logger.info(f"create compressed packet for {self.config.board_name} of efuse PKG ------------> [end]")

if __name__ == "__main__":
    cfg = Config("BM320", "target_personal")
    wk = EfusePartnerTcmSupport(cfg)
    wk.run()