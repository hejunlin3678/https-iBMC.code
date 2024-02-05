#!/usr/bin/python
# -*- coding: UTF-8 -*-

'''
功    能：
版权信息：华为技术有限公司，版本所有(C) 2021
修改记录：2021-10-11 创建
'''
import time
import subprocess
import sys
import os
import shutil
import argparse
import json

cwd=os.path.dirname(__file__)
sys.path.append(f"{cwd}/../")
sys.path.append(f"{cwd}/../../")
from works.work import Work
from works.dependency.parse_to_xml import DependencyXmlParse
from utils.config import Config
from utils.error_info import EXCEPTION_STAGE
from utils.error_info import EXCEPTION_RESPONSE
from utils.error_info import EXCEPTION_CODE

class WorkDownloadDependency(Work):
    def move_kmc_cbb(self):
        self.logger.info("move kmc cbb start.")
        kmc_path = f"{self.common.code_root}/platforms/kmc"
        cbb_path = f"{self.common.code_root}/platforms/cms_vrf_cbb"
        self.common.remove_path(kmc_path)
        self.common.remove_path(cbb_path)
        self.common.copy_all(f"{self.common.temp_path}/kmc", kmc_path)
        self.common.copy_all(f"{self.common.temp_path}/cms_vrf_cbb", cbb_path)
        self.logger.info("move kmc cbb done.")

    def move_middle_binary(self):
        """
        拷贝二进制文件
        """
        internal_release_dir = f"{self.common.code_root}/internal_release"
        if os.path.isdir(self.common.temp_path):
            self.logger.info(f"move binary files from {self.common.temp_path}")
            self.common.move_all(f"{self.common.temp_path}/bin_cmc/internal_release", internal_release_dir)
            self.common.move_all(f"{self.common.temp_path}/bin_cmc", self.common.bin_cmc_path)
        else:
            self.logger.info("no binary file folders")

        self.logger.info("##############move tools from biniBMC begin##################")
        self.common.copy_all(f"{internal_release_dir}/release", f"{self.common.code_root}/tools/release")
        self.logger.info("move internal_release/release from binIBMC done")
        self.common.copy_all(f"{self.common.bin_cmc_path}/tools/release", f"{self.common.code_root}/tools/release")
        self.logger.info("move tools/release from cmc done")
        self.common.copy_all(f"{internal_release_dir}/dft_tools", f"{self.common.code_root}/tools/dft_tools")
        self.logger.info("move internal_release/dft_tools from binIBMC done")
        self.logger.info("##############move tools from biniBMC done##################")
        return

    def make_efuse_hpm(self, txt_type):
        efuse_path = f"{self.common.code_root}/tools/dev_tools/hpmpack_release/efuse-packet"
        # 生成文件
        self.run_command(f"sh {efuse_path}/make_efuse_hpm.sh {txt_type}")

        # 比较记录的efuse.bin的hash值，不匹配就终止构建
        sha256_arm = f"642024f3b97cfc8e2ffbca6e9cf36c2529e52d0eb7e30cc3943e82488a229239"
        sha256_x86 = f"680455aebac03c6020071a9cd2ef9661ec5d5847b5168829aa5b935113ed79a1"
        sha256_partner = f"5b9997e1c8cb9ab655bd7dddc67a047f7748bf058511a0b33ea1dfc13cec1db6"

        efuse_bin_hash = subprocess.run(f"sha256sum {efuse_path}/efuse.bin",
            shell=True, capture_output=True).stdout.decode().strip('\n').split(" ")[0]
        if not ((txt_type == "arm" and efuse_bin_hash == sha256_arm) or
               (txt_type == "x86" and efuse_bin_hash == sha256_x86) or
               (txt_type == "partner" and efuse_bin_hash == sha256_partner)):
            self.logger.error(f"Parameter mismatch, txt_type={txt_type}, efuse_bin_hash={efuse_bin_hash}")
            os._exit(1)

        # 结果处理，移动hpm到目标，删除关键文件
        dst_path = f"{self.common.code_root}/application/src/resource/board/{self.config.board_name}/archive_emmc_nand"
        if txt_type == "partner":
            shutil.copyfile(f"{efuse_path}/efuse-crypt-image.hpm", f"{dst_path}/efuse-crypt-image-partner.hpm")
        else:
            shutil.copyfile(f"{efuse_path}/efuse-crypt-image.hpm", f"{dst_path}/efuse-crypt-image.hpm")

        self.common.remove_file(f"{efuse_path}/efuse-crypt-image.hpm")
        self.common.remove_file(f"{efuse_path}/efuse.bin")

    def move_rootfs(self):
        os.chdir(f"{self.common.code_root}/application/rootfs/opt/pme")
        self.common.copy_all(f"{self.common.bin_cmc_path}/application/rootfs/opt/pme/upgrade", "./upgrade")
        self.common.copy_all(f"{self.common.bin_cmc_path}/application/rootfs/data/opt/pme", f"{self.common.code_root}/application/rootfs/opt/pme/conf/ssl/")
        self.logger.info("move rootfs from cmc done")
        return

    def remove_and_copy_dir(self,src_dir, dst_dir):
        if os.path.isdir(dst_dir):
            shutil.rmtree(dst_dir)
        shutil.copytree(src_dir, dst_dir)
        return

    def move_src_bin(self):

        # lsw
        os.chdir(f"{self.common.code_root}/application/src_bin")

        # open_source
        self.remove_and_copy_dir(f"{self.common.bin_cmc_path}/application/src_bin/open_source", "./open_source")

        # platforms
        self.remove_and_copy_dir(f"{self.common.bin_cmc_path}/application/src_bin/platforms", "./platforms")

        # third_party_groupware
        self.remove_and_copy_dir(f"{self.common.bin_cmc_path}/application/src_bin/third_party_groupware", "./third_party_groupware")

        # user_interface
        self.remove_and_copy_dir(f"{self.common.bin_cmc_path}/application/src_bin/user_interface", "./user_interface")

        # tsb二进制
        self.common.copy_all(f"{self.common.bin_cmc_path}/tsb", "../../internal_release")

        # src_bin/rootfs
        shutil.copyfile(f"{self.common.internal_release_path}/ftools/ftools",
                        f"{self.common.code_root}/application/src_bin/rootfs/opt/pme/bin/ftools")

        self.common.copy_all(f"{self.common.bin_cmc_path}/application/src_bin/rootfs/opt/pme/conf/lan_switch",
                        f"{self.common.code_root}/application/src_bin/rootfs/opt/pme/conf/lan_switch")
        self.common.copy_all(f"{self.common.bin_cmc_path}/application/src_bin/rootfs/opt/pme/conf/pcie_retimer",
                        f"{self.common.code_root}/application/src_bin/rootfs/opt/pme/conf/pcie_retimer")
        self.common.copy_all(f"{self.common.bin_cmc_path}/application/src_bin/rootfs/opt/pme/conf/pcie_switch",
                        f"{self.common.code_root}/application/src_bin/rootfs/opt/pme/conf/pcie_switch")
        self.common.copy_all(f"{self.common.bin_cmc_path}/application/src_bin/rootfs/opt/pme/conf/pcie_switch_9008advance",
                        f"{self.common.code_root}/application/src_bin/rootfs/opt/pme/conf/pcie_switch_9008advance")
        self.common.copy_all(f"{self.common.bin_cmc_path}/application/src_bin/rootfs/opt/pme/conf/pcie_switch_9032",
                        f"{self.common.code_root}/application/src_bin/rootfs/opt/pme/conf/pcie_switch_9032")
        self.common.copy_all(f"{self.common.bin_cmc_path}/application/src_bin/rootfs/opt/pme/conf/switchcard",
                        f"{self.common.code_root}/application/src_bin/rootfs/opt/pme/conf/switchcard")
        self.common.copy_all(f"{self.common.bin_cmc_path}/application/src_bin/rootfs/opt/pme/conf/cpld",
                        f"{self.common.code_root}/application/src_bin/rootfs/opt/pme/conf/cpld")
        self.common.copy_all(f"{self.common.bin_cmc_path}/application/src_bin/rootfs/opt/pme/conf/bios",
                        f"{self.common.code_root}/application/src_bin/rootfs/opt/pme/conf/bios")
        self.common.copy_all(f"{self.common.bin_cmc_path}/application/src_bin/rootfs/opt/pme/conf/cdr",
                        f"{self.common.code_root}/application/src_bin/rootfs/opt/pme/conf/cdr")
        self.common.copy_all(f"{self.common.bin_cmc_path}/application/src_bin/rootfs/opt/pme/conf/vrd_mgnt",
                        f"{self.common.code_root}/application/src_bin/rootfs/opt/pme/conf/vrd_mgnt")

        self.logger.info("move src_bin from cmc done")
        return

    def move_efuse(self):
        board_list = ["Atlas880", "TaiShan2480v2", "TaiShan2480v2Pro", "DA121Cv2", "DA122Cv2", "TaiShan2280Ev2", "Atlas500_3000", "Atlas800_9010", "TaiShan2280v2_1711", "BM320", "BM320_V3",
                    "S920X05", "S920S03", "RM210", "RM211", "RM210_SECURITY_3V0", "TaiShan2280Mv2", "dh120v6", "TaiShan5290v2","CM221","CM221S","CN221","CN221V2","CN221SV2","Atlas200I_SoC_A1", "CST1220V6", "TaiShan2280v2Pro", "S902X20", "Atlas800D_G1", "Atlas800D_RM"]
        x86_board_list = ["1288hv6_2288hv6_5288v6", "2488hv6", "2288xv5_1711"]
        open_sign_board_list = ["TaiShan2480v2", "TaiShan2480v2Pro", "S920S03", "TaiShan2280Ev2", "TaiShan2280Mv2", "TaiShan2280v2_1711", "TaiShan5290v2", "S920X05", "BM320", "BM320_V3", "CST1220V6", "S902X20", "Atlas900_PoD_A2", "TaiShan2280v2Pro"]

        if self.config.board_name in board_list:
            self.make_efuse_hpm("arm")
        elif self.config.board_name in x86_board_list:
            self.make_efuse_hpm("x86")

        if self.config.board_name in open_sign_board_list:
            self.make_efuse_hpm("partner")

        self.logger.info("move efuse from cmc done")
        return

    def copy_platform_files(self, platform_name, platform_dir):
        # platform文件验证存在则删除
        if os.path.isdir(platform_dir):
            shutil.rmtree(platform_dir)

        # 组装平台文件路径并执行拷贝
        shutil.copytree(f"{self.common.bin_cmc_path}/application/platforms/{platform_name}", platform_dir)
        return

    def move_platforms(self):
        """
        platform文件验证并拷贝
        """
        # 改变工作目录到应用文件夹
        os.chdir(f"{self.common.code_root}/application")
        self.copy_platform_files("platform_v5", self.common.platform_v5_path)
        self.copy_platform_files("platform_v6", self.common.platform_v6_path)
        self.logger.info("move platforms from cmc done")
        return

    def download_dependency(self):
        self.logger.info('download dependency tree begin...')
        agentpath = os.path.join(self.common.temp_path, "bin_cmc")
        dependency_file = self.get_path("dependency_for_hpm/downxml/board/" + self.config.board_name + "_dependency.xml")
        subprocess.run(f"cat {dependency_file}", shell=True)
        cmd = f"artget pull -d {dependency_file} -ap {agentpath}"
        self.run_command(cmd)
        self.logger.info('download dependency tree end!')
    def cpld_change_name(self):
        cpld_source = f"{self.common.temp_path}/bin_cmc/application/src_bin/rootfs/opt/pme/conf/cpld"
        board_name_lists = ["PangeaV6_Pacific", "PangeaV6_Atlantic_Smm", "PangeaV6_Atlantic", "PangeaV6_Arctic", "PAC1220V6"]
        for board_name in board_name_lists:
            if os.path.isdir(f"{cpld_source}/{board_name}"):
                os.rename(f"{cpld_source}/{board_name}/cpld.hpm", f"{cpld_source}/{board_name}/cpld_resume.hpm")
                # 多板子是分配到不同机器分别执行，这里可以及时返回
                return


    def move_hinic(self):

        self.common.remove_path(f"{self.common.bin_cmc_path}/application/src_bin/hinic")
        self.common.unzip_to_dir(f"{self.common.bin_cmc_path}/application/src/drivers/1822/firmware.zip",
                            f"{self.common.bin_cmc_path}/application/src_bin/hinic")
        files = os.listdir(f"{self.common.bin_cmc_path}/application/src_bin/hinic/firmware/update_bin/cfg_data_nic_prd_1h_board_2x25G")
        for f in files:
            if ".bin" in f:
                shutil.copyfile(f"{self.common.bin_cmc_path}/application/src_bin/hinic/firmware/update_bin/cfg_data_nic_prd_1h_board_2x25G/{f}",
                                f"{self.common.code_root}/application/src_bin/rootfs/opt/pme/conf/hinic/{f}")
        self.logger.info("move self.move_hinic from cmc done")


    def get_open_source_lib(self):
        """
        获取开源库
        """
        os.chdir(self.common.code_path)
        # 公共路径重命名，简化代码
        if self.config.board_version == "V6":
            pme_open_source = f"{self.common.pme_dir}/src_bin/open_source/arm64"
            pme_third_part = f"{self.common.pme_dir}/src_bin/third_party_groupware/arm64"
            pme_third_switch = f"{self.common.pme_dir}/src_bin/third_party_groupware"
            # tcl
            self.common.copy_file_or_dir(f"{pme_open_source}/tcl/libtcl8.6.so", self.common.lib_open_source)
            # rtl8382
            self.common.copy_file_or_dir(f"{pme_third_switch}/rtl8382/librtl8380_sdk_image64.so", self.common.lib_open_source)
            # sf2507e
            self.common.copy_file_or_dir(f"{pme_third_switch}/SF2507E/libsf2507e.so", self.common.lib_open_source)
            # AdaptiveLM
            self.common.copy_file_or_dir(f"{self.common.pme_dir}/src_bin/platforms/arm64/AdaptiveLM/liblic.so", self.common.lib_open_source)
            # lldpd
            self.common.copy_file_or_dir(f"{pme_open_source}/lldpd/liblldpctl.so.4.9.1", self.common.lib_open_source)

        elif self.config.board_version == "V5":
            pme_open_source = f"{self.common.pme_dir}/src_bin/open_source"
            pme_third_part = f"{self.common.pme_dir}/src_bin/third_party_groupware"
            # lldpd
            self.common.copy_file_or_dir(f"{pme_open_source}/lldpd/liblldpctl.so.4.9.1", self.common.lib_open_source)
            # AdaptiveLM
            self.common.copy_file_or_dir(f"{self.common.pme_dir}/src_bin/platforms/AdaptiveLM/liblic.so", self.common.lib_open_source)

        # curl
        self.common.copy_file_or_dir(f"{pme_open_source}/curl/libcurl.so.4.7.0", self.common.lib_open_source)

        # jpeg
        self.common.copy_file_or_dir(f"{pme_open_source}/jpeg/libjpeg.so.9.5.0", self.common.lib_open_source)

        # libedit
        self.common.copy_file_or_dir(f"{pme_open_source}/Editline_library-libedit/libedit.so.0.0.70", self.common.lib_open_source)

        # JSON-C
        self.common.copy_file_or_dir(f"{pme_open_source}/JSON-C/libjson-c.so.5.2.0", self.common.lib_open_source)

        # OpenLDAP
        self.common.copy_file_or_dir(f"{pme_open_source}/OpenLDAP/liblber.so.2.0.200", self.common.lib_open_source)
        self.common.copy_file_or_dir(f"{pme_open_source}/OpenLDAP/libldap.so.2.0.200", self.common.lib_open_source)

        # SASL
        self.common.copy_file_or_dir(f"{pme_open_source}/SASL/libntlm.so.3.0.0", self.common.lib_open_source)
        self.common.copy_file_or_dir(f"{pme_open_source}/SASL/libsasl2.so.3.0.0", self.common.lib_open_source)

        #pcre
        self.common.copy_file_or_dir(f"{pme_open_source}/pcre/lib*", self.common.lib_open_source)

        # snmp
        self.common.copy_file_or_dir(f"{pme_open_source}/snmp/libnetsnmp.a", self.common.lib_open_source)
        self.common.copy_file_or_dir(f"{pme_open_source}/snmp/libnetsnmp.so.40.1.0", self.common.lib_open_source)
        self.common.copy_file_or_dir(f"{pme_open_source}/snmp/libnetsnmpagent.so.40.1.0", self.common.lib_open_source)
        self.common.copy_file_or_dir(f"{pme_open_source}/snmp/libnetsnmphelpers.so.40.1.0", self.common.lib_open_source)
        self.common.copy_file_or_dir(f"{pme_open_source}/snmp/libnetsnmpmibs.so.40.1.0", self.common.lib_open_source)
        self.common.copy_file_or_dir(f"{pme_open_source}/snmp/libnetsnmptrapd.a", self.common.lib_open_source)
        self.common.copy_file_or_dir(f"{pme_open_source}/snmp/libnetsnmptrapd.so.40.1.0", self.common.lib_open_source)

        # SQLite
        self.common.copy_file_or_dir(f"{pme_open_source}/SQLite/libsqlite3.so.0.8.6", self.common.lib_open_source)

        # ssh2
        self.common.copy_file_or_dir(f"{pme_open_source}/ssh2/libssh2.so.1.0.1", self.common.lib_open_source)

        # StoreLib
        self.common.copy_file_or_dir(f"{pme_third_part}/StoreLib/libstorelib.so.07.1602.0100.0000", self.common.lib_open_source)

        # StoreLib3108
        self.common.copy_file_or_dir(f"{pme_third_part}/StoreLib3108/libstorelib.so.07.0604.0100.1000", self.common.lib_open_source)

        # StoreLibIR3
        self.common.copy_file_or_dir(f"{pme_third_part}/StoreLibIR3/libstorelibir3.so.16.02-0", self.common.lib_open_source)

        # StoreLibIT
        self.common.copy_file_or_dir(f"{pme_third_part}/StoreLibIT/libstorelibit.so.07.0700.0200.0600", self.common.lib_open_source)

        # krb5
        self.common.copy_file_or_dir(f"{pme_open_source}/krb5/libkrb5.so.3.3", self.common.lib_open_source)
        self.common.copy_file_or_dir(f"{pme_open_source}/krb5/libk5crypto.so.3.1", self.common.lib_open_source)
        self.common.copy_file_or_dir(f"{pme_open_source}/krb5/libkrb5support.so.0.1", self.common.lib_open_source)
        self.common.copy_file_or_dir(f"{pme_open_source}/krb5/libcom_err.so.3.0", self.common.lib_open_source)
        self.common.copy_file_or_dir(f"{pme_open_source}/krb5/libgssapi_krb5.so.2.2", self.common.lib_open_source)

        # luajit2
        self.common.copy_file_or_dir(f"{pme_open_source}/luajit2/libluajit-5.1.so.2.1.0", self.common.lib_open_source)

        # 为 java 编译拷贝
        self.common.copy_file_or_dir(f"{self.common.pme_dir}/src_bin/user_interface/*", f"{self.common.pme_dir}/src/user_interface/")

        os.chdir(self.common.code_path)
        return

    def read_json(self, json_path):
        if not os.path.exists(json_path):
            raise Exception
        with open(json_path, "r") as json_file:
            return json.load(json_file)
        raise Exception

    def check_need_pack(self, artfactory):
        dependency_file = f"./works/platforms/dependency_extra.json"
        dependency_list = self.read_json(dependency_file)

        if artfactory not in dependency_list.keys():
            return True
        if self.config.board_name in dependency_list[artfactory]:
            return True
        else:
            return False

    def get_platform_lib(self):
        """
        Hi1880 histore lib库
        """
        if self.check_need_pack('hi1880') == False:
            return

        hs_lib_dir = f"{self.common.temp_path}/platforms/hs/host/arm"
        if not os.path.exists(hs_lib_dir):
            return

        hs_tar_file = f"{self.common.temp_path}/platforms/hs/host/arm/lib.tar.gz"
        if os.path.exists(hs_tar_file):
            self.common.untar_to_dir(f"{hs_lib_dir}/lib.tar.gz", hs_lib_dir)

        self.common.copy_if_exist(f"{hs_lib_dir}/lib_api.h", f"{self.common.code_root}/tools/others/other_tools/open_source/raid/histore")
        self.common.copy_if_exist(f"{hs_lib_dir}/tool_lib.h", f"{self.common.code_root}/tools/others/other_tools/open_source/raid/histore")
        self.common.copy_if_exist(f"{hs_lib_dir}/error_code.h", f"{self.common.code_root}/tools/others/other_tools/open_source/raid/histore")

        if self.config.board_version == "V6":
            self.common.copy_if_exist(f"{hs_lib_dir}/64le/raidlib.so", self.common.lib_open_source)
        return

    def set_open_source_lib_env(self):
        # 编译所有开源库符号链接
        os.chdir(self.common.code_path)
        ret = subprocess.run(f"python3 create_lib_symlinks.py -d {self.common.lib_open_source}", shell=True)
        if ret.returncode:
            err_msg = f"{EXCEPTION_CODE[600]}{EXCEPTION_STAGE['D']}{EXCEPTION_RESPONSE[1]}"
            self.logger.error(f"{err_msg}make open source lib symbol link failed")
            # os._exit(1)
        else:
            self.logger.info("make open source lib symbol link success")
        self.common.remove_file(f"{self.common.lib_open_source}/.gitkeep")


    def build_huawei_developed_tool(self):
        """
        运行tools/toolchain/build.py脚本构建自研工具
        复制binIBMC的tools/toolchain到tools/toolchain/目录
        """
        self.logger.info("build tools.toolchain.build ...!")
        os.chdir(self.common.code_root)
        subprocess.run(f"python3 -B -m tools.toolchain.build", shell=True)
        self.logger.info("build tools.toolchain.build successfully ...!")
        os.chdir(self.common.code_path)
        self.common.copy_all(f"{self.common.bin_cmc_path}/tools/toolchain", f"{self.common.code_root}/tools/toolchain")
        self.logger.info("move toolchain done")
        return

    def check_file_exist(self):
        # 全量源码时，platforms路径下的组件由其他容器编译，CI拷贝过来
        path = f"{self.common.temp_path}/platforms/"
        if not os.path.exists(path):
            self.logger.info(f"{path} is not exist")
        else:
            self.logger.info(os.listdir(path))

    def get_open_source_include(self): 
        # 复制头文件到构建目录
        subprocess.run(f"cp -af {self.common.code_root}/tools/others/other_tools/open_source \
            {self.common.code_root}/application/src/include/", shell=True)
    
    def get_pss_uboot(self):
        if self.config.from_source:
            platform_v6_dir = f"{self.common.bin_cmc_path}/application/platforms/platform_v6"
            platform_v6_source = f"{self.common.temp_path}/platforms/platform_v6"
            self.common.copy(f"{platform_v6_source}/pss_Hi1711_boot_4096.bin", f"{platform_v6_dir}/Hi1711_boot_4096.bin")
            self.common.copy(f"{platform_v6_source}/pss_Hi1711_boot_4096_debug.bin", f"{platform_v6_dir}/Hi1711_boot_4096_debug.bin")
            self.common.copy(f"{platform_v6_source}/pss_Hi1711_boot_4096_pmode.bin", f"{platform_v6_dir}/Hi1711_boot_4096_pmode.bin")
            self.common.copy(f"{platform_v6_source}/pss_Hi1711_boot_4096_pmode_debug.bin", f"{platform_v6_dir}/Hi1711_boot_4096_pmode_debug.bin")
            subprocess.run(f"dd if={platform_v6_source}/pss_Hi1711_boot_4096_pmode.bin of={platform_v6_dir}/Hi1711_boot_pmode.bin bs=1k count=1024 skip=768", shell=True, check=True)
            subprocess.run(f"dd if={platform_v6_source}/pss_Hi1711_boot_4096_pmode_debug.bin of={platform_v6_dir}/Hi1711_boot_pmode_debug.bin bs=1k count=1024 skip=768", shell=True, check=True)
            self.common.copy(f"{platform_v6_source}/pss_Hi1711_boot_4096_factory.bin", f"{platform_v6_dir}/Hi1711_boot_4096_factory.bin")
            self.common.copy(f"{platform_v6_source}/pss_Hi1711_boot_4096_pmode_factory.bin", f"{platform_v6_dir}/Hi1711_boot_4096_pmode_factory.bin")
            subprocess.run(f"dd if={platform_v6_source}/pss_Hi1711_boot_4096_pmode_factory.bin of={platform_v6_dir}/Hi1711_boot_pmode_factory.bin bs=1k count=1024 skip=768", shell=True, check=True)
        else:
            # 保留非全量场景自动刷新pss 证书 签名
            pass

    def run(self):
        self.check_file_exist()
        # 生成{board_name}_dependency.xml依赖配置文件
        xml = DependencyXmlParse(self.config.board_name, False, self.common)
        xml.parse()
        if self.config.info_tree:
            self.logger.info(f"Make info tree xml, no need download!")
            return
        if self.config.board_version == "V5" or self.config.from_source is False:
            self.logger.info("verify board version is V5 or not full source ==============")
            self.download_dependency()  
        # 三大洋和东海单板cpld需要改名为cpld_resume.hpm
        self.cpld_change_name()
        os.chdir(self.common.code_path)
        subprocess.run(f"python3 infotree_post_process.py {self.config.local}", shell=True, check=True)
        if self.common.is_pss_uboot(self.config.board_name):
            self.get_pss_uboot()
        self.move_kmc_cbb()
        self.move_middle_binary()
        self.move_platforms()
        self.move_rootfs()
        self.move_src_bin()
        self.move_efuse()
        if self.config.board_name == "RM210" or self.config.board_name == "RM210_SECURITY_3V0":
            self.move_hinic()
        self.get_open_source_lib()
        self.get_platform_lib()
        self.set_open_source_lib_env()
        self.build_huawei_developed_tool()
        self.get_open_source_include()

# 提供脚本单独调试功能，在application/build目录下运行python3 works/xxxx.py
if __name__ == "__main__":
    cfg = Config("2488hv6", "target_personal")
    wk = WorkDownloadDependency(cfg)
    wk.run()
