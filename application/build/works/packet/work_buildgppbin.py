#!/usr/bin/env python
# coding: utf-8
"""
功 能：buildgppbin脚本，该脚本 make pme jffs2 cramfs gpp bin image
版权信息：华为技术有限公司，版本所有(C) 2019-2020
"""

import os
import sys
import subprocess
import shutil
cwd=os.path.dirname(__file__)
sys.path.append(f"{cwd}/../")
sys.path.append(f"{cwd}/../../")
sys.path.append(f"{cwd}/../../../")
from works.work import Work
from utils.config import Config
from utils.common import Common

from utils.error_info import EXCEPTION_STAGE
from utils.error_info import EXCEPTION_RESPONSE
from utils.error_info import EXCEPTION_CODE


class WorkBuildGppBin(Work):
    def set_env(self):
        self.gpp_build_dir = f"{self.config.build_path}/gpp_build_dir"

    def build_gpp_rootfs_bin(self):
        os.chdir(self.gpp_build_dir)
        subprocess.run("chmod u+x gpp_header", shell=True)
        subprocess.run("./gpp_header emmc", shell=True)
        if self.common.is_rw_emmc(self.config.board_name):
            shutil.move(f"rootfs_{self.config.board_name}.img", f"rootfs_rw_{self.config.board_name}.img")
            shutil.move(f"rootfs_{self.config.board_name}.img.cms", f"rootfs_rw_{self.config.board_name}.img.cms")
            # 制作rw版本的烧片包
            self.common.run_command(f'./make_emmc_gpp.sh {self.config.board_name} 1')
        else:
            self.common.run_command(f'./make_emmc_gpp.sh {self.config.board_name} 0')
        self.common.copy(f"GPP_rootfs_{self.config.board_name}.bin", f"{self.common.work_out}/GPP_rootfs_{self.config.board_name}.bin")

    def build_gpp_bin(self):
        gpp_bin_build_dir = f"{self.config.build_path}/gpp_bin_build"
        os.chdir(gpp_bin_build_dir)
        self.common.run_command("chmod u+x gpp_header")
        self.common.run_command(f'sed -i "s/BOARNAME/{self.config.board_name}/g" hpm_header.config')
        if self.config.build_type != "release" and self.config.partner_compile_mode == "":
            self.common.run_command(f'sed -i "s/Hi1711_boot_4096/Hi1711_boot_4096_debug/g" hpm_header.config')
            self.common.run_command(f'sed -i "s/Hi1711_boot_pmode/Hi1711_boot_pmode_debug/g" hpm_header.config')
        elif self.config.partner_compile_mode == "repair":
            self.common.run_command(f'sed -i "s/Hi1711_boot_4096/Hi1711_boot_4096_factory/g" hpm_header.config')
            self.common.run_command(f'sed -i "s/Hi1711_boot_pmode/Hi1711_boot_pmode_factory/g" hpm_header.config')
        self.common.copy(f"rootfs_{self.config.board_name}.img", "rootfs_iBMC.img")
        self.common.copy(f"rootfs_{self.config.board_name}.img.cms", "rootfs_iBMC.img.cms")
        if os.path.exists("rootfs_iBMC.tar.gz"):
            os.remove("rootfs_iBMC.tar.gz")
        self.run_command("chown 0:0 rootfs_iBMC.img", super_user=True, ignore_error=True)
        self.run_command("tar --format=gnu -czf rootfs_iBMC.tar.gz rootfs_iBMC.img --exclude rootfs_iBMC.tar.gz", super_user=True)
        self.run_command(f"chown {self.common.non_root_ug} rootfs_iBMC.tar.gz", super_user=True, ignore_error=True)
        self.common.run_command("./gpp_header hpm")

        if not os.path.exists("hpm_top_header"):
            err_msg = f"{EXCEPTION_CODE[601]}{EXCEPTION_STAGE['P']}{EXCEPTION_RESPONSE[2]}"
            self.logger.error(f"{err_msg}{gpp_bin_build_dir}/hpm_top_header is not exist!create hpm_sub_header fail!")
            os._exit(1)

        if not os.path.exists("hpm_sub_header"):
            err_msg = f"{EXCEPTION_CODE[601]}{EXCEPTION_STAGE['P']}{EXCEPTION_RESPONSE[2]}"
            self.logger.error(f"{err_msg}{gpp_bin_build_dir}/hpm_sub_header is not exist!create hpm_sub_header fail!")
            os._exit(1)

        self.run_command("chown 0:0 rootfs_iBMC.img", super_user=True, ignore_error=True)
        self.run_command("tar --format=gnu -czf rootfs_iBMC.tar.gz rootfs_iBMC.img --exclude rootfs_iBMC.tar.gz", super_user=True)
        self.run_command(f"chown {self.common.non_root_ug} rootfs_iBMC.tar.gz", super_user=True, ignore_error=True)
        self.logger.info("rootca sha256sum:")
        subprocess.run("sha256sum rootca.der", shell=True)
        if self.config.build_type != "release" and self.config.partner_compile_mode == "":
            self.common.run_command(f"cat hpm_top_header Hi1711_boot_4096_debug.bin Hi1711_boot_pmode_debug.bin hpm_sub_header rootca.der rootfs_iBMC.img.cms cms.crl rootfs_iBMC.tar.gz > {self.config.board_name}_gpp.bin")
        elif self.config.partner_compile_mode == "repair":
            self.common.run_command(f"cat hpm_top_header Hi1711_boot_4096_factory.bin Hi1711_boot_pmode_factory.bin hpm_sub_header rootca.der rootfs_iBMC.img.cms cms.crl rootfs_iBMC.tar.gz > {self.config.board_name}_gpp.bin")
            self.common.copy(f"{self.common.temp_path}/debug/output/rootfs_{self.config.board_name}.img.bak", 
                f"{self.common.temp_path}/debug/output/rootfs_{self.config.board_name}.img")
        else:
            self.common.run_command(f"cat hpm_top_header Hi1711_boot_4096.bin Hi1711_boot_pmode.bin hpm_sub_header rootca.der rootfs_iBMC.img.cms cms.crl rootfs_iBMC.tar.gz > {self.config.board_name}_gpp.bin")
        self.common.copy(f"{self.config.board_name}_gpp.bin", f"{self.config.work_out}/{self.config.board_name}_gpp.bin")
        self.common.run_command(f'sed -i "s/{self.config.board_name}/BOARNAME/g" hpm_header.config')

        if os.path.exists(f"rootfs_ro_{self.config.board_name}.img") and self.config.build_type == "dft_separation" :
            self.common.run_command(f'sed -i "s/BOARNAME/ro_{self.config.board_name}/g" hpm_header.config')
            self.common.run_command(f'sed -i "s/Hi1711_boot_4096_debug/Hi1711_boot_4096/g" hpm_header.config')
            self.common.run_command(f'sed -i "s/Hi1711_boot_pmode_debug/Hi1711_boot_pmode/g" hpm_header.config')
            self.common.copy(f"rootfs_ro_{self.config.board_name}.img", "rootfs_iBMC.img")
            self.common.copy(f"rootfs_ro_{self.config.board_name}.img.cms", "rootfs_iBMC.img.cms")
            if os.path.exists("rootfs_iBMC.tar.gz"):
                os.remove("rootfs_iBMC.tar.gz")
            self.run_command("chown 0:0 rootfs_iBMC.img", super_user=True, ignore_error=True)
            self.run_command("tar --format=gnu -czf rootfs_iBMC.tar.gz rootfs_iBMC.img --exclude rootfs_iBMC.tar.gz", super_user=True)
            self.run_command(f"chown {self.common.non_root_ug} rootfs_iBMC.tar.gz", super_user=True, ignore_error=True)
            self.common.run_command("./gpp_header hpm")
            self.common.run_command(f"cat hpm_top_header Hi1711_boot_4096.bin Hi1711_boot_pmode.bin hpm_sub_header rootca.der rootfs_iBMC.img.cms cms.crl rootfs_iBMC.tar.gz > {self.config.board_name}_ro_gpp.bin")
            self.common.copy(f"{self.config.board_name}_ro_gpp.bin", f"{self.config.work_out}/{self.config.board_name}_ro_gpp.bin")
            self.common.run_command(f'sed -i "s/ro_{self.config.board_name}/BOARNAME/g" hpm_header.config')

        if self.config.cb_flag and self.config.build_type == "dft_separation":
            self.common.run_command(f'sed -i "s/BOARNAME/CB_FLAG_{self.config.board_name}_release/g" hpm_header.config')
            self.common.copy(f"rootfs_CB_FLAG_{self.config.board_name}_release.img", "rootfs_iBMC.img")
            self.common.copy(f"rootfs_CB_FLAG_{self.config.board_name}_release.img.cms", "rootfs_iBMC.img.cms")
            if os.path.exists("rootfs_iBMC.tar.gz"):
                os.remove("rootfs_iBMC.tar.gz")
            self.run_command("chown 0:0 rootfs_iBMC.img", super_user=True, ignore_error=True)
            self.run_command("tar --format=gnu -czf rootfs_iBMC.tar.gz rootfs_iBMC.img --exclude rootfs_iBMC.tar.gz", super_user=True)
            self.run_command(f"chown {self.common.non_root_ug} rootfs_iBMC.tar.gz", super_user=True, ignore_error=True)
            self.common.run_command("./gpp_header hpm")
            self.common.run_command(f"cat hpm_top_header Hi1711_boot_4096.bin Hi1711_boot_pmode.bin hpm_sub_header rootca.der rootfs_iBMC.img.cms cms.crl rootfs_iBMC.tar.gz > CB_FLAG_{self.config.board_name}_gpp_release.bin")
            self.common.copy(f"CB_FLAG_{self.config.board_name}_gpp_release.bin", f"{self.config.work_out}/CB_FLAG_{self.config.board_name}_gpp_release.bin")
            self.common.run_command(f'sed -i "s/CB_FLAG_{self.config.board_name}_release/BOARNAME/g" hpm_header.config')

            self.common.run_command(f'sed -i "s/rootfs_BOARNAME/rootfs_rw_CB_FLAG_{self.config.board_name}_release/g" hpm_header.config')
            self.common.run_command(f'sed -i "s/Hi1711_boot_4096/Hi1711_boot_4096_debug/g" hpm_header.config')
            self.common.run_command(f'sed -i "s/Hi1711_boot_pmode/Hi1711_boot_pmode_debug/g" hpm_header.config')
            self.common.copy(f"rootfs_rw_CB_FLAG_{self.config.board_name}_release.img", "rootfs_iBMC.img")
            self.common.copy(f"rootfs_rw_CB_FLAG_{self.config.board_name}_release.img.cms", "rootfs_iBMC.img.cms")
            if os.path.exists("rootfs_iBMC.tar.gz"):
                os.remove("rootfs_iBMC.tar.gz")
            self.run_command("chown 0:0 rootfs_iBMC.img", super_user=True, ignore_error=True)
            self.run_command("tar --format=gnu -czf rootfs_iBMC.tar.gz rootfs_iBMC.img --exclude rootfs_iBMC.tar.gz", super_user=True)
            self.run_command(f"chown {self.common.non_root_ug} rootfs_iBMC.tar.gz", super_user=True, ignore_error=True)
            self.common.run_command("./gpp_header hpm")
            self.common.run_command(f"cat hpm_top_header Hi1711_boot_4096.bin Hi1711_boot_pmode.bin hpm_sub_header rootca.der rootfs_iBMC.img.cms cms.crl rootfs_iBMC.tar.gz > CB_FLAG_{self.config.board_name}_gpp_release_rw.bin")
            self.common.copy(f"CB_FLAG_{self.config.board_name}_gpp_release_rw.bin", f"{self.config.work_out}/CB_FLAG_{self.config.board_name}_gpp_release_rw.bin")
            self.common.run_command(f'sed -i "s/rootfs_rw_CB_FLAG_{self.config.board_name}_release/rootfs_BOARNAME/g" hpm_header.config')

        if self.config.cb_flag and self.config.build_type != "dft_separation":
            self.common.run_command(f'sed -i "s/BOARNAME/CB_FLAG_{self.config.board_name}/g" hpm_header.config')
            self.common.copy(f"rootfs_CB_FLAG_{self.config.board_name}.img", "rootfs_iBMC.img")
            self.common.copy(f"rootfs_CB_FLAG_{self.config.board_name}.img.cms", "rootfs_iBMC.img.cms")
            if os.path.exists("rootfs_iBMC.tar.gz"):
                os.remove("rootfs_iBMC.tar.gz")
            self.run_command("chown 0:0 rootfs_iBMC.img", super_user=True, ignore_error=True)
            self.run_command("tar --format=gnu -czf rootfs_iBMC.tar.gz rootfs_iBMC.img --exclude rootfs_iBMC.tar.gz", super_user=True)
            self.run_command(f"chown {self.common.non_root_ug} rootfs_iBMC.tar.gz", super_user=True, ignore_error=True)
            self.common.run_command("./gpp_header hpm")
            
            if self.config.build_type != "release":
                self.common.run_command(f"cat hpm_top_header Hi1711_boot_4096_debug.bin Hi1711_boot_pmode_debug.bin hpm_sub_header rootca.der rootfs_iBMC.img.cms cms.crl rootfs_iBMC.tar.gz >  CB_FLAG_{self.config.board_name}_gpp.bin")
            else:
                self.common.run_command(f"cat hpm_top_header Hi1711_boot_4096.bin Hi1711_boot_pmode.bin hpm_sub_header rootca.der rootfs_iBMC.img.cms cms.crl rootfs_iBMC.tar.gz > CB_FLAG_{self.config.board_name}_gpp.bin")
            self.common.copy(f"CB_FLAG_{self.config.board_name}_gpp.bin", f"{self.config.work_out}/CB_FLAG_{self.config.board_name}_gpp.bin")
            self.common.run_command(f'sed -i "s/CB_FLAG_{self.config.board_name}/BOARNAME/g" hpm_header.config')

    def run(self):
        if self.config.board_version != "V6":
            return
        self.set_env()
        if not self.config.support_dft_separation and self.config.build_type == "dft_separation":
            return
        if self.config.board_version == 'V6':
            flag_rw = self.common.is_rw_emmc(self.config.board_name)
            if (self.config.build_type == "release" and flag_rw == False) or \
                    (self.config.build_type == "debug" and flag_rw == True):
                self.build_gpp_rootfs_bin()
            if os.path.exists(f"{self.config.build_path}/gpp_bin_build"):
                self.build_gpp_bin()


# 提供脚本单独调试功能，在application/build目录下运行python3 works/xxxx.py
if __name__ == "__main__":
    cfg = Config()
    wk = WorkBuildGppBin(cfg)
    wk.run()
    print("build gpp bin end here")
