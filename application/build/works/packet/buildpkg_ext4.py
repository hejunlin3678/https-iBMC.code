#!/usr/bin/env python
# coding: utf-8
"""
功 能：buildpkg_ext4脚本，该脚本 make ext4 image
版权信息：华为技术有限公司，版本所有(C) 2019-2020
构建命令：python3 buildpkg_ext4.py "board_name"
"""

import os
import shutil
import subprocess
import sys

cwd=os.path.dirname(__file__)
sys.path.append(f"{cwd}/../")
sys.path.append(f"{cwd}/../../")
sys.path.append(f"{cwd}/../../../")
from utils.common import Common
from utils.log import Logger
from utils.error_info import EXCEPTION_STAGE
from utils.error_info import EXCEPTION_RESPONSE
from utils.error_info import EXCEPTION_CODE

class PacketPkgExt4():
    def __init__(self, config, common, logger):
        self.config = config
        self.common = common
        self.logger = logger
        self.product_is_x86 = False

    def set_evn(self):
        self.agent_path = f"{self.common.temp_path}/spftp"
        # 未配置则默认为 x86
        product_arch = self.common.get_product_arch(self.config.board_name)
        if product_arch == "x86":
            self.product_is_x86 = True

    def download_sp_ibma(self):
        dependency_file = "dependency.xml"
        if self.product_is_x86:
            dependency_file = "dependency_X86.xml"
        else:
            dependency_file = "dependency_AARC64.xml"

        dependency_path = f"{self.common.code_path}/dependency_for_bin/{dependency_file}"
        logname = f"{self.common.log_manage_dic.get('download_log')}/download_sp.log"
        shcmd = f"artget pull -d {dependency_path} -ap {self.agent_path} >{logname} 2>&1 "
        self.logger.info(f"artget pull -d  {dependency_path} -ap {self.agent_path} >{logname} 2>&1 ")
        subprocess.run(shcmd, shell=True)

    def add_eco_ft_files_to_rootfs(self, file):
        eco_ft_dir = f"{self.config.build_path}/eco_ft"
        self.common.check_path(eco_ft_dir)
        subprocess.run(f"mount -t ext4 {file} {eco_ft_dir}", shell=True)
        version_file_path = f"{eco_ft_dir}/opt/pme/conf/profile/platform.xml"
        ftools_path = f"{self.common.src_bin_rootfs}/bin/ftools"
        ftools_conf_path = f"{self.common.src_bin_rootfs}/conf/ftools/{self.config.board_name}_config.xml"
        ftools_start_sh_path = f"{self.common.src_bin_rootfs}/conf/ftools/start_ft.sh"
        subprocess.run(f'sed -i "/AuxVersion/s/V>.*<\/V/V>1<\/V/" {version_file_path}', shell=True)
        aux_ver = self.common.get_xml_value(version_file_path, 'P', 'AuxVersion', 'V')
        self.logger.info(f"eco_ft aux_ver = {aux_ver} ")
        if str(f"-{aux_ver}") != "-1":
            err_msg = f"{EXCEPTION_CODE[601]}{EXCEPTION_STAGE['P']}{EXCEPTION_RESPONSE[2]}"
            self.logger.error(f"{err_msg}modify eco_ft AuxVersion failed, return error")
            os._exit(1)

        self.common.copy(ftools_path, f"{eco_ft_dir}/opt/pme/bin/ftools")
        self.common.py_chmod(f"{eco_ft_dir}/opt/pme/bin/ftools", "755")

        self.common.check_path(f"{eco_ft_dir}/opt/pme/conf/ftools")
        self.common.copy(ftools_conf_path, f"{eco_ft_dir}/opt/pme/conf/ftools/ft_config.xml")
        self.common.py_chmod(f"{eco_ft_dir}/opt/pme/conf/ftools/ft_config.xml", "755")

        self.common.copy(ftools_start_sh_path, f"{eco_ft_dir}/opt/pme/init/start_ft.sh")
        self.common.py_chmod(f"{eco_ft_dir}/opt/pme/init/start_ft.sh", "755")
        subprocess.run(f"umount {eco_ft_dir}", shell=True)
        self.common.remove_path(eco_ft_dir)

    def build_pkg(self):
        board_pkg = f"{self.config.build_path}/{self.config.board_name}_pkg"
        self.common.remove_path(board_pkg)
        self.common.check_path(board_pkg)
        if self.config.board_version == "V5":
            self.common.copy_all(f"{self.common.tools_path}/toolchain/package_emmc_nand_img", board_pkg)
            self.common.copy_all(f"{self.common.src_bin}/emmc_nand_partition", board_pkg)
            self.common.copy(f"{self.config.work_out}/u-boot.bin", f"{board_pkg}/u-boot.bin")
            self.common.run_command(f"sed -i 's/rh2288v3/'{self.config.board_name}'/g' {board_pkg}/config.cfg")

            self.common.copy(f"{self.common.work_out}/rootfs_{self.config.board_name}.img",
            f"{board_pkg}/rootfs_{self.config.board_name}.img")
            if self.config.board_name == "2288hv5":
                self.add_eco_ft_files_to_rootfs(f"{self.common.work_out}/rootfs_{self.config.board_name}.img")
            self.common.copy(f"{self.common.work_out}/datafs_{self.config.board_name}.img",
                f"{board_pkg}/datafs_{self.config.board_name}.img")
            self.common.copy(f"{self.common.work_out}/datafs_debug_{self.config.board_name}.img",
                        f"{self.common.work_out}/CI_Package/datafs_debug_{self.config.board_name}.img")
            os.chdir(board_pkg)
            ret=self.common.run_command("make")
            if ret.returncode:
                err_msg = f"{EXCEPTION_CODE[610]}{EXCEPTION_STAGE['C']}{EXCEPTION_RESPONSE[1]}"
                self.logger.error(f"{err_msg} make converfile failed")
                os._exit(1)
            subprocess.run("./converfile", shell=True)
            shutil.copy("ipmc.bin", f"{self.common.work_out}/{self.config.board_name}.bin")

            # 增加 SPI 4M flash 的烧片包制作脚本
            os.chdir(self.config.build_path)
            self.common.remove_path(board_pkg)
            self.common.check_path(board_pkg)
            self.common.copy_all(f"{self.common.tools_path}/toolchain/package", board_pkg)
            self.common.copy(f"{board_pkg}/config_uboot.cfg", f"{board_pkg}/config.cfg")
            self.common.copy(f"{board_pkg}/converFile_uboot.h", f"{board_pkg}/converFile.h")
            self.common.copy(f"{self.config.work_out}/u-boot.bin", f"{board_pkg}/u-boot.bin")
            self.common.copy(f"{self.config.work_out}/u-boot.bin", f"{self.common.work_out}/u-boot.bin")
            self.common.copy(f"{self.config.work_out}/u-boot_debug.bin", f"{self.common.work_out}/u-boot_debug.bin")
            os.chdir(board_pkg)
            ret=self.common.run_command("make")
            if ret.returncode:
                err_msg = f"{EXCEPTION_CODE[610]}{EXCEPTION_STAGE['C']}{EXCEPTION_RESPONSE[1]}"
                self.logger.error(f"{err_msg} make converfile failed")
                os._exit(1)
            subprocess.run("./converfile", shell=True)
            shutil.copy("ipmc.bin", f"{self.common.work_out}/{self.config.board_name}_uboot.bin")

        elif self.config.board_version == "V6":
            self.common.copy_all(f"{self.common.tools_path}/toolchain/package_emmc_nand_img_v6", board_pkg)
            self.common.copy_all(f"{self.common.src_bin}/emmc_partition_v6", board_pkg)
            self.common.run_command(f"sed -i 's/BOARNAME/'{self.config.board_name}'/g' {board_pkg}/config.cfg")
            self.common.copy(f"{self.common.work_out}/rootfs_{self.config.board_name}.img",
                f"{board_pkg}/rootfs_{self.config.board_name}.img")
            self.common.copy(f"{self.common.work_out}/datafs_{self.config.board_name}.img",
                f"{board_pkg}/datafs_{self.config.board_name}.img")

            self.common.copy(f"{self.config.work_out}/Hi1711_boot_4096.bin", f"{self.common.work_out}/Hi1711_boot_4096.bin")

            os.chdir(board_pkg)

            self.download_sp_ibma()

            os.chdir(board_pkg)
            if self.product_is_x86:
                # x86 执行
                if os.path.isfile(f"{self.agent_path}/X86/sp.bin"):
                    self.logger.info("get X86/sp.bin frome local ok ------------>")
                    shutil.copy(f"{self.agent_path}/X86/sp.bin", board_pkg)
                else:
                    err_msg = f"{EXCEPTION_CODE[410]}{EXCEPTION_STAGE['D']}{EXCEPTION_RESPONSE[2]}"
                    self.logger.error(f"{err_msg}sp.bin file is not exist")
                    os._exit(1)
            else:
                # arm 执行
                if os.path.isfile(f"{self.agent_path}/aarch64/sp_aarch64.bin"):
                    self.logger.info("get aarch64/sp_aarch64.bin frome local ok ------------>")
                    shutil.copy(f"{self.agent_path}/aarch64/sp_aarch64.bin", f"{board_pkg}/sp.bin")
                else:
                    err_msg = f"{EXCEPTION_CODE[410]}{EXCEPTION_STAGE['D']}{EXCEPTION_RESPONSE[2]}"
                    self.logger.error(f"{err_msg}sp_aarch64.bin file is not exist")
                    os._exit(1)

            if not os.path.isdir(f"{board_pkg}/ibmapkg"):
                os.makedirs(f"{board_pkg}/ibmapkg")

            ibma_bin_file = ""
            os.chdir(board_pkg)
            if self.product_is_x86:
                # x86 执行
                os.chdir(f"{self.agent_path}/iBMA/X86")
                self.common.untar_to_dir("platform.tar.gz")
                self.common.copy_all("platform", ".")
                os.chdir(board_pkg)
                ibma_bin_file = f"{self.agent_path}/iBMA/X86/iBMA.bin"
            else:
                ibma_bin_file = f"{self.agent_path}/iBMA/aarch64/iBMA_aarch64.bin"
            if os.path.isfile(ibma_bin_file):
                self.logger.info("get iBMA.bin frome local ok ------------>")
                shutil.copy(ibma_bin_file, f"{board_pkg}/iBMA.bin")
            else:
                err_msg = f"{EXCEPTION_CODE[410]}{EXCEPTION_STAGE['D']}{EXCEPTION_RESPONSE[2]}"
                self.logger.error(f"{err_msg}iBMA.bin file is not exist")
                os._exit(1)

            ret=self.common.run_command("make")
            if ret.returncode:
                err_msg = f"{EXCEPTION_CODE[610]}{EXCEPTION_STAGE['C']}{EXCEPTION_RESPONSE[1]}"
                self.logger.error(f"{err_msg} make converfile failed")
                os._exit(1)
            subprocess.run("./converfile", shell=True)
            subprocess.run("dd if=ipmc.bin of=user_data_256.bin bs=1M count=256", shell=True)
            shutil.copy("user_data_256.bin", f"{self.common.output_path}/user_data_256.bin")
            shutil.copy("ipmc.bin", f"{self.common.work_out}/{self.config.board_name}_User.bin")


    def run(self):
        self.set_evn()
        self.build_pkg()
