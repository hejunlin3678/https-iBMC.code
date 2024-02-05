#!/usr/bin/env python
# coding:utf-8
# Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
"""
Copyright (c) Huawei Technologies Co., Ltd. 2023. All rights reserved.
Description: 打包目录形成
"""

import os
import sys
import argparse
import subprocess
import shutil

cwd = os.path.dirname(__file__)
sys.path.append(f"{cwd}/../")
sys.path.append(f"{cwd}/../../")
sys.path.append(f"{cwd}/../../../")
from works.packet.work_envir_prepare import WorkEnvirPrepare
from utils.config import Config


class WorkEnvirPrepare(WorkEnvirPrepare):

    def preapre_gpp_rootfs_bin(self):
        gpp_build_dir = f"{self.config.build_path}/gpp_build_dir"
        self.common.remove_path(gpp_build_dir)
        self.common.check_path(gpp_build_dir)

        hpm_gpp_bin_build_dir_src = f"{self.common.tools_path}/toolchain/gppbin"
        huaweirootca_bin_dir = f"{self.common.src_bin}/defaultCA"
        self.common.copy_all(hpm_gpp_bin_build_dir_src, gpp_build_dir)
        if self.sign_type == "PSS":
            self.common.copy(f"{huaweirootca_bin_dir}/defaultCAG2.der",
                        f"{gpp_build_dir}/defaultCA.der")
        else:
            self.common.copy(f"{huaweirootca_bin_dir}/defaultCA.der",
                        f"{gpp_build_dir}/defaultCA.der")
        self.common.run_command(f"sha256sum {gpp_build_dir}/defaultCA.der")
        self.common.copy(f"{self.config.work_out}/rootfs_{self.config.board_name}.img",
                    f"{gpp_build_dir}/rootfs_{self.config.board_name}.img")
        self.common.copy(f"{self.config.work_out}/rootfs_{self.config.board_name}.img.cms",
                    f"{gpp_build_dir}/rootfs_{self.config.board_name}.img.cms")
        # 合作伙伴模式使用CMS版本Uboot
        if os.path.exists(self.g_partner_config_file):
            self.common.copy(f"{self.config.work_out}/Hi1711_boot_4096_pmode.bin",
                        f"{gpp_build_dir}/Hi1711_boot_4096.bin")
            self.common.copy(f"{self.config.work_out}/Hi1711_boot_4096_pmode_debug.bin",
                        f"{gpp_build_dir}/Hi1711_boot_4096_debug.bin")
        else:
            self.common.copy(f"{self.config.work_out}/Hi1711_boot_4096.bin",
                        f"{gpp_build_dir}/Hi1711_boot_4096.bin")
            self.common.copy(f"{self.config.work_out}/Hi1711_boot_4096_debug.bin",
                        f"{gpp_build_dir}/Hi1711_boot_4096_debug.bin")
        self.common.copy(f"{self.config.work_out}/cms.crl",
                    f"{gpp_build_dir}/cms.crl")

        os.chdir(gpp_build_dir)
        self.common.run_command(f'sed -i "s/BOARNAME/{self.config.board_name}/g" emmc_header.config')

    def copy_gpp_bin(self, gpp_bin_build_dir):
        self.common.copy(f"{self.config.work_out}/rootfs_{self.config.board_name}.img",
                    f"{gpp_bin_build_dir}/rootfs_{self.config.board_name}.img")
        self.common.copy(f"{self.config.work_out}/rootfs_{self.config.board_name}.img.cms",
                    f"{gpp_bin_build_dir}/rootfs_{self.config.board_name}.img.cms")
        if self.config.cb_flag:
            self.common.copy(f"{self.config.work_out}/rootfs_CB_FLAG_{self.config.board_name}.img",
                    f"{gpp_bin_build_dir}/rootfs_CB_FLAG_{self.config.board_name}.img")
            self.common.copy(f"{self.config.work_out}/rootfs_CB_FLAG_{self.config.board_name}.img.cms",
                    f"{gpp_bin_build_dir}/rootfs_CB_FLAG_{self.config.board_name}.img.cms")
        if self.config.build_type == "dft_separation":
            self.common.copy(f"{self.config.work_out}/rootfs_ro_{self.config.board_name}.img",
                    f"{gpp_bin_build_dir}/rootfs_ro_{self.config.board_name}.img")
            self.common.copy(f"{self.config.work_out}/rootfs_ro_{self.config.board_name}.img.cms",
                    f"{gpp_bin_build_dir}/rootfs_ro_{self.config.board_name}.img.cms")
            if self.config.cb_flag:
                self.common.copy(f"{self.config.work_out}/rootfs_CB_FLAG_{self.config.board_name}_release.img",
                        f"{gpp_bin_build_dir}/rootfs_CB_FLAG_{self.config.board_name}_release.img")
                self.common.copy(f"{self.config.work_out}/rootfs_CB_FLAG_{self.config.board_name}_release.img.cms",
                        f"{gpp_bin_build_dir}/rootfs_CB_FLAG_{self.config.board_name}_release.img.cms")
                self.common.copy(f"{self.config.work_out}/rootfs_rw_CB_FLAG_{self.config.board_name}_release.img",
                        f"{gpp_bin_build_dir}/rootfs_rw_CB_FLAG_{self.config.board_name}_release.img")
                self.common.copy(f"{self.config.work_out}/rootfs_rw_CB_FLAG_{self.config.board_name}_release.img.cms",
                        f"{gpp_bin_build_dir}/rootfs_rw_CB_FLAG_{self.config.board_name}_release.img.cms")
        self.common.copy(f"{self.config.work_out}/Hi1711_boot_4096.bin",
                    f"{gpp_bin_build_dir}/Hi1711_boot_4096.bin")
        self.common.copy(f"{self.config.work_out}/Hi1711_boot_4096_debug.bin",
                    f"{gpp_bin_build_dir}/Hi1711_boot_4096_debug.bin")
        self.common.copy(f"{self.config.work_out}/Hi1711_boot_4096_factory.bin",
                    f"{gpp_bin_build_dir}/Hi1711_boot_4096_factory.bin")  

    def prepare_gpp_bin(self):
        gpp_bin_build_dir = f"{self.config.build_path}/gpp_bin_build"
        self.common.remove_path(gpp_bin_build_dir)
        self.common.check_path(gpp_bin_build_dir)

        hpm_gpp_build_dir_src = f"{self.common.tools_path}/toolchain/emmc_hpm_header"
        huaweirootca_bin_dir = f"{self.common.src_bin}/defaultCA"

        self.common.copy_all(hpm_gpp_build_dir_src, gpp_bin_build_dir)
        if self.sign_type == "PSS":
            self.logger.info("copy %s/defaultCAG2.der %s/rootca.der", huaweirootca_bin_dir, gpp_bin_build_dir)
            self.common.copy(f"{huaweirootca_bin_dir}/defaultCAG2.der",
                        f"{gpp_bin_build_dir}/rootca.der")
        else:
            self.logger.info("cp %s/defaultCA.der %s/rootca.der", huaweirootca_bin_dir, gpp_bin_build_dir)
            self.common.copy(f"{huaweirootca_bin_dir}/defaultCA.der",
                        f"{gpp_bin_build_dir}/rootca.der")
        self.common.run_command(f"sha256sum {gpp_bin_build_dir}/rootca.der")
        self.copy_gpp_bin(gpp_bin_build_dir)
        self.common.run_command(f"dd if={self.config.work_out}/Hi1711_boot_4096_pmode.bin \
            of={gpp_bin_build_dir}/Hi1711_boot_pmode.bin bs=1k count=1024 skip=768")
        self.common.run_command(f"dd if={self.config.work_out}/Hi1711_boot_4096_pmode_debug.bin \
            of={gpp_bin_build_dir}/Hi1711_boot_pmode_debug.bin bs=1k count=1024 skip=768")
        self.common.run_command(f"dd if={self.config.work_out}/Hi1711_boot_4096_pmode_factory.bin \
            of={gpp_bin_build_dir}/Hi1711_boot_pmode_factory.bin bs=1k count=1024 skip=768")
        self.common.copy(f"{self.config.work_out}/cms.crl",
                    f"{gpp_bin_build_dir}/cms.crl")

        os.chdir(gpp_bin_build_dir)
        self.common.run_command(f'sed -i "s/BOARNAME/{self.config.board_name}/g" hpm_header.config')
        self.common.run_command("chmod u+x gpp_header")

        if os.path.getsize("Hi1711_boot_4096.bin") != 2097152 or \
            os.path.getsize("Hi1711_boot_4096_debug.bin") != 2097152:
            err_msg = f"{EXCEPTION_CODE[603]}{EXCEPTION_STAGE['P']}{EXCEPTION_RESPONSE[2]}"
            self.logger.error(f"{err_msg}The size of Hi1711_boot_4096.bin or Hi1711_boot_4096_debug.bin is \
                not equal 2M!")
            raise Exception

    def sign_cms_crl(self):
        sing_img_xml_filename = "sign_img_pss.xml" if self.sign_type == 'PSS' else "sign_img.xml"
        self.logger.info("sign type:%s, sign file:%s/%s", self.sign_type, self.common.code_root, sing_img_xml_filename)
        if os.path.exists(f"{self.common.code_root}/{sing_img_xml_filename}"):
            self.logger.info("Use CI signature file!!!")
            self.common.copy(f"{self.common.code_root}/{sing_img_xml_filename}", 
                f"{self.config.build_path}/{sing_img_xml_filename}")
        else:
            self.logger.info("Use empty signature file!!!")
            
        virtual_cms_file = f"{self.common.code_root}/application/sign/image.img.cms"
        virtual_crl_file = f"{self.common.code_root}/application/sign/cms.crl"
        # 复制下载的crldata.crl文件。
        self.common.copy_pss_crl(virtual_crl_file)
        self.logger.info("Use virtual signature file!")
        if self.config.support_dft_separation:
            sign_file_tuple = (
                f"{self.config.work_out}/rootfs_{self.config.board_name}.img.cms", 
                f"{self.config.work_out}/rootfs_rw_{self.config.board_name}.img.cms", 
                f"{self.config.work_out}/rootfs_{self.config.board_name}_release.img.cms", 
                f"{self.config.work_out}/rootfs_rw_{self.config.board_name}_release.img.cms", 
                f"{self.config.work_out}/cms.crl"
            )
        else:
            sign_file_tuple = (
                f"{self.config.work_out}/rootfs_{self.config.board_name}.img.cms", 
                f"{self.config.work_out}/rootfs_rw_{self.config.board_name}.img.cms", 
                f"{self.config.work_out}/cms.crl"
            )
        for f in sign_file_tuple:
            if f != sign_file_tuple[-1]:
                shutil.copy(virtual_cms_file, f)
        shutil.copy(virtual_crl_file, sign_file_tuple[-1])

    def run(self):
        self.set_global()
        if not self.config.support_dft_separation and self.config.build_type == "dft_separation":
            return
        if self.config.board_version == 'V6':
            self.sign_cms_crl()
            flag_rw = self.common.is_rw_emmc(self.config.board_name)
            is_debug_prepare = self.config.build_type == "debug" and flag_rw is True
            is_release_prepare = self.config.build_type == "release" and flag_rw is False
            if is_release_prepare or is_debug_prepare:
                self.preapre_gpp_rootfs_bin()
            self.prepare_gpp_bin()
        self.prepare_hpm()

# 提供脚本单独调试功能，在application/build目录下运行python3 works/xxxx.py
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="build target")
    parser.add_argument("-b", "--board_name", 
        help="Board list must in '' and separated by space, default value is 2488hv6", default="2488hv6")
    args = parser.parse_args()
    cfg = Config(args.board_name)
    wk = WorkEnvirPrepare(cfg)
    wk.run()
