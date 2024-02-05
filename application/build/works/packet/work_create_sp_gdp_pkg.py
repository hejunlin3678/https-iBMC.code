#!/usr/bin/env python
# coding: utf-8
"""
功 能：work_create_sp_gdp_pkg.py脚本
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

class Sp(Work):
    def __init__(self, config, work_name=""):
        super(Sp, self).__init__(config)
        self.config = config
        self.archive_conf_dic = self.common.get_archive_config(self.config.board_name)
        self.release_dir = f"{self.config.board_name}_sp_release"
        self.resource_dir = f"{self.common.code_root}/application/src/resource"
        self.archive_emmc_nand_dir = f"{self.resource_dir}/board/{self.config.board_name}/archive_emmc_nand"
        self.customize_dir = f"{self.release_dir}/dft_tools/config/Common/customize"
        self.sw_code = self.archive_conf_dic['togdp_dir'].split('/')[-1]
        self.version = self.common.get_board_version(self.config.board_name)
        self.product_arch = self.common.get_product_arch(self.config.board_name)
        self.function_dir = f"{self.customize_dir}/function"
        self.hpmfwupg_tool_dir = f"{self.release_dir}/dft_tools/hpmfwupg/{self.product_arch}"
        self.partner_config_file = f"{self.common.code_root}/application/src/resource/board/{self.config.board_name}/archive_emmc_nand/partner_signature.ini"
    
    def add_ibma(self):
        if self.archive_conf_dic.get("togdp_path_ibma", "") != "":
            # 合并 ipmc.bin + sp.bin + ibma.bin
            os.chdir(self.common.temp_path)
            if self.product_arch == "x86":
                os.chdir("spftp/iBMA/X86")
                self.common.untar_to_dir("platform.tar.gz", ".")
                self.run_command("cp platform/* .")
                os.chdir(self.common.temp_path)
                ibma_bin_dir = "spftp/iBMA/X86/iBMA.bin"
                ibma_ver_dir = "spftp/iBMA/X86/iBMA_version.ini"
            else:
                ibma_bin_dir = "spftp/iBMA/aarch64/iBMA_aarch64.bin"
                ibma_ver_dir = "spftp/iBMA/aarch64/iBMA_aarch64_version.ini"

            ibma_bin_name_out = "iBMA.bin"
            ibma_ver_name_out = "iBMA_version.ini"
            # 这里的文件名和文件夹名未搞清， 原文件中也是判断是否是文件

            if os.path.exists(f"{self.common.temp_path}/{ibma_bin_dir}"):
                self.logger.info(f"get iBMA.bin frome local ok ------------>")
                if self.version == "V5":
                    self.common.run_command(f"cat {ibma_bin_dir} >> {self.config.build_path}/{self.release_dir}/ipmc.bin")
                self.common.copy(ibma_ver_dir, f"{self.config.build_path}/{self.release_dir}/iBMA_version.ini")
            else:
                self.logger.error(f"{self.common.temp_path}/{ibma_bin_dir} file is not exist")
                os._exit(1)

            if self.version == "V5":
                total_size = os.stat(f"{self.config.build_path}/{self.release_dir}/ipmc.bin").st_size
                if total_size != 7066563584 :
                    self.logger.error(f"the size of file (ipmc_ibma.bin) is uncorrect, expected value is 7066563584,acture size is {total_size}")
                    os._exit(1)

            os.chdir(self.config.build_path)

    def packet_eco_ft(self):
        bin_dir = f"{self.release_dir}/烧片文件"
        other_dir = f"{self.release_dir}/其它文档"

        os.chdir(self.config.build_path)
        self.common.get_dft_tools(self.release_dir, self.config.board_name)
        self.common.remove_path(bin_dir)
        self.common.remove_path(other_dir)
        self.common.check_path(bin_dir)
        self.common.check_path(other_dir)
        self.logger.info(f"packet ECO_FT file  ----------> [begin]")

        # 拷贝文件
        self.run_command(f"mv {self.release_dir}/*.ini {bin_dir}")
        self.run_command(f"mv {self.release_dir}/*.bin {bin_dir}")

        self.common.copy(f"{self.release_dir}/dft_tools/config/Common/customize/config.sh",
                    f"{self.release_dir}/config.sh")
        self.common.copy(f"{self.customize_dir}/white_branding.sh",
                    f"{self.release_dir}/white_branding.sh")
        self.common.copy(f"{self.customize_dir}/bmc_upload_cmes.sh",
                    f"{self.release_dir}/bmc_upload_cmes.sh")
        self.common.copy(f"{self.customize_dir}/{self.product_arch}/conver",
                    f"{self.release_dir}/conver")
        self.common.copy(f"{self.customize_dir}/verify.sh",
                    f"{self.release_dir}/verify.sh")
        self.common.copy(f"{self.customize_dir}/defaultserverport.ini",
                    f"{self.release_dir}/defaultserverport.ini")
        self.common.copy(f"{self.customize_dir}/common_factory_restore.sh",
                        f"{self.release_dir}/common_factory_restore.sh")
        self.common.copy_all(f"{self.customize_dir}/function",
                        f"{self.release_dir}/function")
        self.common.copy(f"{self.customize_dir}/driver.tar.gz",
                    f"{self.release_dir}/driver.tar.gz")
        # 拷贝健康状态解析工具
        self.common.copy(f"{self.release_dir}/dft_tools/GetBmcHealthevents/{self.product_arch}/GetBmcHealthevents",
                    f"{self.release_dir}/GetBmcHealthevents")

        # 检查单板是否有专有的定制化脚本及配置，如有，将其目录下的文件拷贝过来，覆盖同名文件和目录
        if os.path.isdir(f"{self.resource_dir}/board/{self.config.board_name}/archive/proprietary"):
            self.common.copy_all(f"{self.resource_dir}/board/{self.config.board_name}/archive/proprietary", self.release_dir)
            self.common.copy_all(f"{self.resource_dir}/config/{self.config.board_name}", self.release_dir)

        self.common.remove_path(f"{self.release_dir}/function/.svn")
        self.common.run_command(f"chmod +x {self.release_dir}/function/*.sh")
        os.chdir(self.release_dir)
        # 解决Bep 比对差异 加入--format=gnu 参数
        self.common.run_command("tar --format=gnu -czf function.tar.gz function")
        os.chdir(self.config.build_path)
        self.common.remove_path(f"{self.release_dir}/function")
        self.common.copy(
            f"{self.release_dir}/dft_tools/hpmfwupg/{self.product_arch}/hpmfwupg_v5",
            f"{self.release_dir}/hpmfwupg")
        # 备注要整改
        ipmcimage_dir = f"{self.common.code_root}/tools/toolchain/hpmpack/ipmcimage"
        if os.path.exists(f"{ipmcimage_dir}/{self.product_arch}/crypto_tool_ext4"):
            self.common.copy(f"{ipmcimage_dir}/{self.product_arch}/crypto_tool_ext4",
                        f"{self.release_dir}/crypto_tool")
        else:
            self.common.copy(f"{ipmcimage_dir}/{self.product_arch}/crypto_tool",
                        f"{self.release_dir}/crypto_tool")

        self.common.copy_all(self.archive_emmc_nand_dir, self.release_dir)
        self.common.copy(f"{self.common.work_out}/rootfs_{self.config.board_name}.hpm", f"{self.release_dir}/image.hpm")

        found_list = self.common.py_find_dir("\.svn", self.release_dir, True)
        found_list += self.common.py_find_dir("\.gitkeep", self.release_dir, True)
        if len(found_list):
            for found_file in found_list:
                if os.path.isdir(found_file):
                    shutil.rmtree(found_file)
                else:
                    os.remove(found_file)
        self.common.remove_path(f"{self.release_dir}/dft_tools")

        os.chdir(self.release_dir)
        self.run_command("chmod +x *.sh")
        self.run_command("chmod +x conver hpmfwupg")
        self.run_command("chmod -x *.ini *.cfg")

        # 复制ini文件到上层目录，当前层更新名字
        os.chdir(f"{self.config.build_path}/{bin_dir}")
        files = os.listdir(".")
        for f in files:
            if self.archive_conf_dic['togdp_0502'] in f:
                shutil.copy(f, f"{self.config.build_path}/{self.release_dir}")
                new_file = f.replace(self.archive_conf_dic['togdp_0502'], 
                self.archive_conf_dic['togdp_sp_0502'])
                os.rename(f, new_file)

        self.logger.info(f"packet ECO_FT file  ----------> [End]")

    def run(self):
        # 没有配置 togdp_dir_sp ，不需要打包
        if self.archive_conf_dic.get("togdp_dir_sp", "") == "":
            return
        self.logger.info(f"create compressed packet for {self.config.board_name} of Sp GDP PKG ------------> [begin]")
        # 创建临时目录
        os.chdir(self.config.build_path)
        self.run_command(f"umount {self.release_dir}", super_user=True, ignore_error=True)
        self.common.remove_path(self.release_dir)
        self.common.check_path(self.release_dir)
        self.run_command(f"mount -t tmpfs -o size=16384m tmpfs {self.release_dir}", super_user=True, ignore_error=True)
        self.run_command(f"chown -R {self.config.common.non_root_ug} {self.release_dir}", super_user=True, ignore_error=True)
        # copy archive 下的文件
        copy_file_list = self.common.py_find_dir(".*version\.ini", self.archive_emmc_nand_dir)
        for copy_file in copy_file_list:
            shutil.copy(copy_file, self.release_dir)
        if self.version == "V5":
            if self.config.board_name == "xa320" or self.config.board_name == "TaiShan2280v2":
                tmp_sp_dir = "aarch64"
                sp_bin_name = "sp_aarch64.bin"
                sp_bin_sha_name = "sp_aarch64.bin.sha"
            elif self.config.board_name == "S920" or self.config.board_name == "BM320":
                tmp_sp_dir = "Kunpeng"
                sp_bin_name = "sp_aarch64-Kunpeng.bin"
                sp_bin_sha_name = "sp_aarch64-Kunpeng.bin.sha"
            else:
                tmp_sp_dir = "X86"
                sp_bin_name = "sp.bin"
                sp_bin_sha_name = "sp.bin.sha"

            if os.path.exists(f"{self.common.temp_path}/spftp/{tmp_sp_dir}/{sp_bin_name}"):
                self.logger.info(f"get {sp_bin_name} frome local ok ------------>")
                shutil.copy(f"{self.common.work_out}/{self.config.board_name}.bin", f"{self.release_dir}/ipmc.bin")
                self.common.run_command(f"cat {self.common.temp_path}/spftp/{tmp_sp_dir}/{sp_bin_name} >> {self.release_dir}/ipmc.bin")
                self.common.copy(f"{self.common.temp_path}/spftp/{tmp_sp_dir}/sp_version.ini",
                f"{self.release_dir}/sp_version.ini")
            else:
                self.logger.error(f"{err_msg}{self.config.build_path}/sppkg/sp.bin file is not exist")
                os._exit(1)
            total_size = os.stat(f"{self.release_dir}/ipmc.bin").st_size
            if total_size != 6744440832:
                self.logger.error(f"the size of file (ipmc.bin) is uncorrect, expected value is 6744440832,acture size is {total_size}")
        elif self.version == "V6":
            self.common.copy(f"{self.common.temp_path}/spftp/sp_version.ini", f"{self.release_dir}/sp_version.ini")
            if os.path.exists(f"{self.common.work_out}/{self.config.board_name}_User.bin"):
                self.logger.info("get ipmc.bin frome buildpkg ok ------------>")
                # 编译 ipmc 二进制文件
                self.run_command(f"umount ipmc_bin_{self.config.board_name}", ignore_error=True, super_user=True)
                self.common.remove_path(f"ipmc_bin_{self.config.board_name}")
                self.common.check_path(f"ipmc_bin_{self.config.board_name}")
                self.run_command(f"mount -t tmpfs -o size=16384m tmpfs ipmc_bin_{self.config.board_name}", ignore_error=True, super_user=True)
                self.run_command(f"chown -R {self.config.common.non_root_ug} ipmc_bin_{self.config.board_name}", super_user=True, ignore_error=True)
                # 给 GPP4 烧写空文件，使 GPP4 分区能够 mount 上 (128M)
                self.run_command(f"dd if=/dev/zero of=ipmc_bin_{self.config.board_name}/GPP_4.img bs=128M count=1")
                self.run_command(f"mkfs.ext4 ipmc_bin_{self.config.board_name}/GPP_4.img", super_user=True)
                self.run_command(f"chown {self.config.common.non_root_ug} ipmc_bin_{self.config.board_name}/GPP_4.img", super_user=True, ignore_error=True)

                shutil.copy(f"{self.common.work_out}/GPP_rootfs_{self.config.board_name}.bin",
                            f"ipmc_bin_{self.config.board_name}/GPP_rootfs_{self.config.board_name}.bin")
                shutil.copy(f"{self.common.work_out}/{self.config.board_name}_User.bin",
                            f"ipmc_bin_{self.config.board_name}/ipmc.bin")
                if os.path.exists(self.partner_config_file):
                    shutil.copy(f"{self.config.work_out}/Hi1711_boot_4096_pmode.bin",
                                f"ipmc_bin_{self.config.board_name}/Hi1711_boot_4096.bin")
                else:
                    #将RM210机型烧片中的uboot改为debug模式
                     if self.config.board_name == "RM210" or self.config.board_name == "RM211" or self.config.board_name == "Atlas800D_RM":
                        self.logger.info("increase RM210/RM211 debug.bin ----------->")
                        shutil.copy(f"{self.config.work_out}/Hi1711_boot_4096_debug.bin",
                                    f"ipmc_bin_{self.config.board_name}/Hi1711_boot_4096.bin")
                     else:
                        shutil.copy(f"{self.config.work_out}/Hi1711_boot_4096.bin",
                                    f"ipmc_bin_{self.config.board_name}/Hi1711_boot_4096.bin")
                shutil.move(f"{self.common.code_root}/tools/toolchain/make_emmc_flash_bin/lib",
                            f"ipmc_bin_{self.config.board_name}")
                shutil.copytree(f"{self.common.code_root}/tools/toolchain/include",
                                f"ipmc_bin_{self.config.board_name}/include", dirs_exist_ok=True)
                self.common.copy_all(f"{self.common.code_root}/tools/toolchain/make_emmc_flash_bin",
                                f"ipmc_bin_{self.config.board_name}")

                os.chdir(f"ipmc_bin_{self.config.board_name}")

                self.common.py_sed("make_emmc_flash_bin.h", "BOARDNAME", self.config.board_name)
                self.run_command("make")
                self.run_command("chmod +x make_emmc_flash_bin")
                os.environ['LD_LIBRARY_PATH'] = f"./lib"
                self.run_command("./make_emmc_flash_bin")

                shutil.copy("emmc_output_ipmc.bin", f"{self.config.build_path}/{self.release_dir}/ipmc.bin")
                shutil.copy("Hi1711_THGBMHG6C1LBAIL_output.m4i", f"{self.config.build_path}/{self.release_dir}/THGBMHG6C1LBAIL.m4i")
                shutil.copy("Hi1711_THGBMHG6C1LBAIL_output.m4i", f"{self.config.build_path}/{self.release_dir}/H26M41204HPR.m4i")
                self.common.py_sed(f"{self.config.build_path}/{self.release_dir}/H26M41204HPR.m4i", "THGBMHG6C1LBAIL", "H26M41204HPR")

                shutil.copy("Hi1711_THGBMHG6C1LBAIL_output.m4i", f"{self.config.build_path}/{self.release_dir}/H26M41204HPRR.m4i")
                self.common.py_sed(f"{self.config.build_path}/{self.release_dir}/H26M41204HPRR.m4i", "THGBMHG6C1LBAIL", "H26M41204HPRR")

                shutil.copy("Hi1711_THGBMHG6C1LBAIL_output.m4i", f"{self.config.build_path}/{self.release_dir}/THGBMJG6C1LBAIL.m4i")
                self.common.py_sed(f"{self.config.build_path}/{self.release_dir}/THGBMJG6C1LBAIL.m4i", "THGBMHG6C1LBAIL", "THGBMJG6C1LBAIL")

                # 支持闪迪烧片
                shutil.copy("Hi1711_THGBMHG6C1LBAIL_output.m4i", f"{self.config.build_path}/{self.release_dir}/SDINBDG4-8G.m4i")

                sed_dic = {
                    "THGBMHG6C1LBAIL": "SDINBDG4-8G-Q",
                    "512x\$E90000": "512x$E90E80",
                    "4Mx\$3A4": "8Mx$1B5",
                    "\! 000060": "! 000030",
                    "\! 000020": "! 000010"
                }
                for key in sed_dic.keys():
                    self.common.py_sed(f"{self.config.build_path}/{self.release_dir}/SDINBDG4-8G.m4i", key, sed_dic[key], def_line="g")

                # 支持长江存储32GEMMC YMEC6A2TB1A2C3型号烧片
                shutil.copy("Hi1711_THGBMHG6C1LBAIL_output.m4i", f"{self.config.build_path}/{self.release_dir}/YMEC6A2TB1A2C3.m4i")

                sed_dic = {
                    "THGBMHG6C1LBAIL": "YMEC6A2TB1A2C3",
                    "M1PLS1": "M2PLS1",
                    "512x\$E90000\+4Mx2\+4M": "512x$3A3E000+4Mx2+16M",
                    "4Mx\$3A4": "8Mx$4BD",
                    "\! 000060": "! 000030",
                    "\! 000020": "! 000010",
                    ".*EOF": "    ! \"AUTO\"	* Set PARTITION_SETTING_COMPLETED[155]\n*EOF"
                }
                for key in sed_dic.keys():
                    self.common.py_sed(f"{self.config.build_path}/{self.release_dir}/YMEC6A2TB1A2C3.m4i", key, sed_dic[key], def_line="g")

                # 支持三星32EMMC KLMBG2JETD-B0410009型号烧片
                shutil.copy("Hi1711_THGBMHG6C1LBAIL_output.m4i", f"{self.config.build_path}/{self.release_dir}/KLMBG2JETD-B041.m4i")

                sed_dic = {
                    "THGBMHG6C1LBAIL": "KLMBG2JETD-B041",
                    "M1PLS1": "M2PLS1",
                    "512x\$E90000\+4Mx2\+4M": "512x$3A3E000+4Mx2+4M",
                    "4Mx\$3A4": "8Mx$747",
                    "\! 000060": "! 000030",
                    "\! 000020": "! 000010",
                    ".*EOF": "    ! \"AUTO\"	* Set PARTITION_SETTING_COMPLETED[155]\n*EOF"
                }
                for key in sed_dic.keys():
                    self.common.py_sed(f"{self.config.build_path}/{self.release_dir}/KLMBG2JETD-B041.m4i", key, sed_dic[key], def_line="g")

                # 支持海力士64G H26M74002HMR 型号烧片
                shutil.copy("Hi1711_THGBMHG6C1LBAIL_output.m4i", f"{self.config.build_path}/{self.release_dir}/H26M74002HMR.m4i")
                sed_dic = {
                    "THGBMHG6C1LBAIL": "H26M74002HMR",
                    "M1PLS1": "M2PLS1",
                    "512x\$E90000\+4Mx2\+4M": "512x$747C000+4Mx2+16M",
                    "4Mx\$3A4": "8Mx$E8F",
                    "\! 000060": "! 000030", "\! 000020": "! 000010",
                    ".*EOF": "    ! \"AUTO\"	* Set PARTITION_SETTING_COMPLETED[155]\n*EOF",
                }
                for key in sed_dic.keys():
                    self.common.py_sed(f"{self.config.build_path}/{self.release_dir}/H26M74002HMR.m4i", key, sed_dic[key], def_line="g")

                # 支持长江存储64G YMEC7A2TB2A2C3 型号烧片
                shutil.copy("Hi1711_THGBMHG6C1LBAIL_output.m4i", f"{self.config.build_path}/{self.release_dir}/YMEC7A2TB2A2C3.m4i")
                sed_dic = {
                    "THGBMHG6C1LBAIL": "YMEC7A2TB2A2C3",
                    "M1PLS1": "M2PLS1",
                    "512x\$E90000\+4Mx2\+4M": "512x$747C000+4Mx2+16M",
                    "4Mx\$3A4": "8Mx$9B7",
                    "\! 000060": "! 000030", "\! 000020": "! 000010",
                    ".*EOF": "    ! \"AUTO\"	* Set PARTITION_SETTING_COMPLETED[155]\n*EOF",
                }
                for key in sed_dic.keys():
                    self.common.py_sed(f"{self.config.build_path}/{self.release_dir}/YMEC7A2TB2A2C3.m4i", key, sed_dic[key], def_line="g")

                # 支持东芝/铠侠64G THGBMHG9C4LBAIR 型号烧片
                shutil.copy("Hi1711_THGBMHG6C1LBAIL_output.m4i", f"{self.config.build_path}/{self.release_dir}/THGBMHG9C4LBAIR.m4i")
                sed_dic = {
                    "THGBMHG6C1LBAIL": "THGBMHG9C4LBAIR",
                    "M1PLS1": "M2PLS1",
                    "512x\$E90000\+4Mx2\+4M": "512x$747C000+4Mx2+4M",
                    "4Mx\$3A4": "8Mx$E90",
                    "\! 000060": "! 000030", "\! 000020": "! 000010",
                    ".*EOF": "    ! \"AUTO\"	* Set PARTITION_SETTING_COMPLETED[155]\n*EOF",
                }
                for key in sed_dic.keys():
                    self.common.py_sed(f"{self.config.build_path}/{self.release_dir}/THGBMHG9C4LBAIR.m4i", key, sed_dic[key], def_line="g")
        
                # 支持海力士32GEMMCJPR型号烧片
                shutil.copy("Hi1711_THGBMHG6C1LBAIL_output.m4i", f"{self.config.build_path}/{self.release_dir}/H26M62002JPR.m4i")
                sed_dic = {
                    "THGBMHG6C1LBAIL": "H26M62002JPR",
                    "M1PLS1": "M2PLS1",
                    "512x\$E90000\+4Mx2\+4M": "512x$3A3E000+4Mx2+16M",
                    "4Mx\$3A4": "8Mx$747",
                    "\! 000060": "! 000030",
					"\! 000020": "! 000010",
                    ".*EOF": "    ! \"AUTO\"	* Set PARTITION_SETTING_COMPLETED[155]\n*EOF",
                }
                for key in sed_dic.keys():
                    self.common.py_sed(f"{self.config.build_path}/{self.release_dir}/H26M62002JPR.m4i", key, sed_dic[key], def_line="g")
                # 国产JBL EMMc
                shutil.copy("Hi1711_THGBMHG6C1LBAIL_output.m4i", f"{self.config.build_path}/{self.release_dir}/FEMDRM032G-A3A55.m4i")
                sed_dic = {
                    "THGBMHG6C1LBAIL": "FEMDRM032G-A3A55",
                    "M1PLS1": "M2PLS1",
                    "512x\$E90000\+4Mx2\+4M": "512x$39d0000+4Mx2+16M",
                    "4Mx\$3A4": "8Mx$4D1",
                    "\! 000060": "! 000030",
                    "\! 000020": "! 000010",
                    ".*EOF": "    ! \"AUTO\"	* Set PARTITION_SETTING_COMPLETED[155]\n*EOF",
                }
                for key in sed_dic.keys():
                    self.common.py_sed(f"{self.config.build_path}/{self.release_dir}/FEMDRM032G-A3A55.m4i", key, sed_dic[key], def_line="g")

                # 支持昂科烧片
                shutil.copytree(f"{self.common.code_root}/internal_release/acxml_generator", "acxml_generator", dirs_exist_ok=True)
                os.chdir("acxml_generator")
                self.run_command("make")
                shutil.copy("release/acxmlgenerator", f"{self.config.build_path}/ipmc_bin_{self.config.board_name}/acxmlgenerator")
                os.chdir(f"{self.config.build_path}/ipmc_bin_{self.config.board_name}")
                self.run_command("chmod +x acxml_generator")
                self.run_command(f"./acxmlgenerator -b emmc_output_ipmc.bin -x AUTO_BURNING_CONFIG.hwxml -o {self.config.build_path}/{self.release_dir}/AUTO_BURNING_CONFIG.acxml")

                os.chdir(self.config.build_path)
                self.run_command(f"umount ipmc_bin_{self.config.board_name}", super_user=True)
            else:
                self.logger.error(f"sp.bin file is not exist")
                os._exit(1)
            total_size = os.stat(f"{self.release_dir}/ipmc.bin").st_size
            self.logger.info(f"The size of file (ipmc.bin) is {total_size}")

        self.add_ibma()

        # 添加执行权限
        os.chdir(self.release_dir)
        self.run_command("chmod +x *.sh", ignore_error=True)
        # 更新version.ini文件；
        ar_ver_file = f"{self.sw_code}_version.ini"
        if not os.path.exists(ar_ver_file):
            self.logger.error("can not find version profile")
            self.common.remove_path(f"{self.config.build_path}/{self.release_dir}")
            return
        # 提取xml中的版本号
        if self.config.support_dft_separation:
            ver_num = self.common.extract_ver_num(ar_ver_file, self.config.board_name, dft_flag=True)
        else:
            ver_num = self.common.extract_ver_num(ar_ver_file, self.config.board_name)

        if self.config.board_name == "2288hv5":
            os.chdir(self.config.build_path)
            eco_pkg = f"{self.archive_conf_dic['togdp_name_eco_ft']}{ver_num}"
            self.common.check_path(eco_pkg)
            shutil.copy(f"{self.archive_emmc_nand_dir}/{self.archive_conf_dic['togdp_eco_ft_0502']}_check.sh", eco_pkg)
            shutil.copy(f"{self.release_dir}/{ar_ver_file}", 
            f"{eco_pkg}/{self.archive_conf_dic['togdp_eco_ft_0502']}_version.ini")
            self.run_command(f"zip -1 -rq {eco_pkg}.zip {eco_pkg}")
            self.logger.info(f"move eco_ft file to  '{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_eco_ft']}'")
            self.common.remove_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_eco_ft']}")
            self.common.check_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_eco_ft']}")
            shutil.move(f"{eco_pkg}.zip", f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_eco_ft']}")
            # 清理前面的文件重新打包一份
            self.packet_eco_ft()

        os.chdir(f"{self.config.build_path}/{self.release_dir}")
        files = os.listdir(f"{self.config.build_path}/{self.release_dir}")
        for f in files:
            if self.archive_conf_dic['togdp_0502'] in f:
                new_file = f.replace(self.archive_conf_dic['togdp_0502'], 
                self.archive_conf_dic['togdp_sp_0502'])
                os.rename(f, new_file)
        self.common.remove_file(f"{self.archive_conf_dic['togdp_eco_ft_0502']}_check.sh")
        # 生成ToGDP目录下的zip包
        target_pkg = f"{self.archive_conf_dic['togdp_name_sp']}{ver_num}"
        os.chdir(self.config.build_path)
        self.run_command(f"cp -a {self.release_dir} {target_pkg}")
        self.run_command(f"umount {self.release_dir}", super_user=True)
        self.run_command(f'zip -1 -rq "{target_pkg}".zip {target_pkg}')
        self.common.remove_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_sp']}")
        self.common.check_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_sp']}")
        self.logger.info(f"move file to  '{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_sp']}'")
        shutil.move(f"{self.config.build_path}/{target_pkg}.zip", f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_sp']}")
        self.common.remove_path(f"{self.config.build_path}/{target_pkg}")
        self.logger.info(f"create compressed packet for {self.config.board_name} of Sp GDP PKG ------------> [end]")


if __name__ == "__main__":
    cfg = Config("2288hv5", "target_personal")
    cfg.set_build_type("release")
    wk = Sp(cfg)
    wk.run()