#!/usr/bin/env python
# coding:utf-8

"""
文件名：envir_prepare.py
功能：打包目录形成
版权信息：华为技术有限公司，版本所有(C) 2020-2021
"""
import os
import sys
import argparse
import subprocess

cwd=os.path.dirname(__file__)
sys.path.append(f"{cwd}/../")
sys.path.append(f"{cwd}/../../")
sys.path.append(f"{cwd}/../../../")
from utils.common import Common
from works.work import Work
from utils.config import Config


class WorkEnvirPrepare(Work):
    # # 参数配置
    # parser = argparse.ArgumentParser(description="build_one_click.py")
    # parser.add_argument("-f", "--func_type", default="hpm")
    # # parser.add_argument("-b", "--board_name", default="")
    # args = parser.parse_args()

    def set_global(self):
        self.g_src_resource = f"{self.common.code_root}/application/src/resource"
        self.g_hpm_build_dir_src = f"{self.common.code_root}/tools/toolchain/hpmpack/ipmcimage"
        self.g_partner_config_file = f"{self.common.code_root}/application/src/resource/board/{self.config.board_name}/archive_emmc_nand/partner_signature.ini"
        if self.config.from_source:
            if self.common.is_pss_uboot(self.config.board_name):
                self.sign_type = "PSS"
            else:
                self.sign_type = "PKCS"
        else:
            self.sign_type = self.common.get_sign_algorithm_type(self.config.board_name)

    def copy_file_or_dir(self, src_dir, dst_dir):
        subprocess.run(f"yes y | cp -ai {src_dir} {dst_dir}", shell=True)
        return

    def exit_with_create_flag(self, board_name, exit_code=0):
        flag_file = f"{self.common.temp_path}/buildhpm_flag_{board_name}"
        if not os.path.isfile(flag_file):
            create_empty_text_file(flag_file)
        if exit_code:
            err_msg = f"{EXCEPTION_CODE[600]}{EXCEPTION_STAGE['P']}{EXCEPTION_RESPONSE[1]}"
            self.logger.error(f"{err_msg}create empty text file fail!")
            os._exit(exit_code)
        return

    def preapre_gpp_rootfs_bin(self):
        gpp_build_dir = f"{self.config.build_path}/gpp_build_dir"
        self.common.remove_path(gpp_build_dir)
        self.common.check_path(gpp_build_dir)

        hpm_gpp_bin_build_dir_src = f"{self.common.tools_path}/toolchain/gppbin"
        huaweirootca_bin_dir = f"{self.common.src_bin}/HuaweiRootCA"
        self.common.copy_all(hpm_gpp_bin_build_dir_src, gpp_build_dir)
        if self.sign_type == "PSS":
            self.common.copy(f"{huaweirootca_bin_dir}/HuaweiRootCAG2.der",
                        f"{gpp_build_dir}/HuaweiRootCA.der")
        else:
            self.common.copy(f"{huaweirootca_bin_dir}/HuaweiRootCA.der",
                        f"{gpp_build_dir}/HuaweiRootCA.der")
        subprocess.run(f"sha256sum {gpp_build_dir}/HuaweiRootCA.der", shell=True, check=True)
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

    def prepare_gpp_bin(self):
        gpp_bin_build_dir = f"{self.config.build_path}/gpp_bin_build"
        self.common.remove_path(gpp_bin_build_dir)
        self.common.check_path(gpp_bin_build_dir)

        hpm_gpp_build_dir_src = f"{self.common.tools_path}/toolchain/emmc_hpm_header"
        huaweirootca_bin_dir = f"{self.common.src_bin}/HuaweiRootCA"

        self.common.copy_all(hpm_gpp_build_dir_src, gpp_bin_build_dir)
        if self.sign_type == "PSS":
            self.logger.info("copy {}/HuaweiRootCAG2.der {}/rootca.der".format(huaweirootca_bin_dir, gpp_bin_build_dir))
            self.common.copy(f"{huaweirootca_bin_dir}/HuaweiRootCAG2.der",
                        f"{gpp_bin_build_dir}/rootca.der")
        else:
            self.logger.info("cp {}/HuaweiRootCA.der {}/rootca.der".format(huaweirootca_bin_dir, gpp_bin_build_dir))
            self.common.copy(f"{huaweirootca_bin_dir}/HuaweiRootCA.der",
                        f"{gpp_bin_build_dir}/rootca.der")
        subprocess.run(f"sha256sum {gpp_bin_build_dir}/rootca.der", shell=True, check=True)
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
        subprocess.run(f"dd if={self.config.work_out}/Hi1711_boot_4096_pmode.bin " \
            f"of={gpp_bin_build_dir}/Hi1711_boot_pmode.bin bs=1k count=1024 skip=768", shell=True)
        subprocess.run(f"dd if={self.config.work_out}/Hi1711_boot_4096_pmode_debug.bin " \
            f"of={gpp_bin_build_dir}/Hi1711_boot_pmode_debug.bin bs=1k count=1024 skip=768", shell=True)
        subprocess.run(f"dd if={self.config.work_out}/Hi1711_boot_4096_pmode_factory.bin "\
            f"of={gpp_bin_build_dir}/Hi1711_boot_pmode_factory.bin bs=1k count=1024 skip=768", shell=True)
        self.common.copy(f"{self.config.work_out}/cms.crl",
                    f"{gpp_bin_build_dir}/cms.crl")

        os.chdir(gpp_bin_build_dir)
        self.common.run_command(f'sed -i "s/BOARNAME/{self.config.board_name}/g" hpm_header.config')
        self.common.run_command("chmod u+x gpp_header")

        if os.path.getsize("Hi1711_boot_4096.bin") != 2097152 or os.path.getsize("Hi1711_boot_4096_debug.bin") != 2097152:
            err_msg = f"{EXCEPTION_CODE[603]}{EXCEPTION_STAGE['P']}{EXCEPTION_RESPONSE[2]}"
            self.logger.error(f"{err_msg}The size of Hi1711_boot_4096.bin or Hi1711_boot_4096_debug.bin is not equal 2M!")
            os._exit(1)

    def prepare_hpm(self):
        pdt_dir = f"{self.common.code_path}/../src/resource/board/{self.config.board_name}"
        self.g_hpm_build_dir_src = f"{self.common.code_root}/tools/toolchain/hpmpack/ipmcimage"
        hpm_build_dir = f"{self.config.build_path}/hpm_build"
        if not os.path.isdir(pdt_dir):
            err_msg = f"{EXCEPTION_CODE[600]}{EXCEPTION_STAGE['P']}{EXCEPTION_RESPONSE[1]}"
            self.logger.error(f"{err_msg}{pdt_dir} is not exit!")
            os._exit(1)

        if not os.path.isfile(f"{pdt_dir}/update_ext4.cfg"):
            self.logger.info(f"{pdt_dir}/update_ext4 is not exist!")
            self.exit_with_create_flag(self.config.board_name, 1)

        self.common.remove_path(hpm_build_dir)
        self.common.check_path(hpm_build_dir)

        self.common.copy_all(self.g_hpm_build_dir_src, hpm_build_dir)
        self.common.copy(f"{pdt_dir}/update_ext4.cfg", f"{hpm_build_dir}/update.cfg")

        os.chdir(hpm_build_dir)
        curr_ver = self.common.get_shell_command_result("cat update.cfg | grep '^Version' | awk -F '=' '{print $2}'")

        # 读取发布时用的包名， 用以定位是否有预置的密钥， 签名文件
        if self.config.board_version == "V5":
            dst_xml_file = f"{self.g_src_resource}/pme_profile/platform.xml"
            major_ver = self.common.get_xml_value(dst_xml_file, 'PROPERTY', 'PMEMajorVer', 'VALUE')
            minor_ver = self.common.get_xml_value(dst_xml_file, 'PROPERTY', 'PMEMinorVer', 'VALUE')
        elif self.config.board_version == "V6":
            dst_xml_file = f"{self.g_src_resource}/pme_profile/platform_v6.xml"
            major_ver = self.common.get_xml_value(dst_xml_file, 'PROPERTY', 'PMEMajorVer', 'VALUE')
            minor_ver = self.common.get_xml_value(dst_xml_file, 'PROPERTY', 'PMEMinorVer', 'VALUE').zfill(2)
            release_major_ver = self.common.get_xml_value(dst_xml_file, 'PROPERTY', 'ReleaseMajorVer', 'VALUE').zfill(2)
            release_minor_ver = self.common.get_xml_value(dst_xml_file, 'PROPERTY', 'ReleaseMinorVer', 'VALUE').zfill(2)

        tosupport_name_hpm = self.common.get_shell_command_result(
            f"cat {self.g_src_resource}/board/{self.config.board_name}/archive_emmc_nand/archive.ini | grep -n TOSUPPORT_HPM_NAME | awk -F '=' '{{print $2}}'")
        # prefix_name = self.common.run_command(
        #     f"ls {self.common.code_root} | grep prefix_{tosupport_name_hpm}").stdout.decode().strip('\n')
        prefix_name = subprocess.run(
            f"ls {self.common.code_root} | grep prefix_{tosupport_name_hpm}", shell=True, capture_output=True).stdout.decode().strip('\n')
        self.logger.info(f"build hpm prefix_path = {self.common.code_root}/{prefix_name}/key")

        if os.path.isdir(f"{self.common.code_root}/{prefix_name}/key"):
            self.logger.info("use prefix key for upgrade package")
            self.copy_file_or_dir(f"{self.common.code_root}/{prefix_name}/key/aes_key*", ".")

        if minor_ver[0:2] == "0x":
            minor_ver = minor_ver[2:]

        # 正常包
        if self.config.board_version == "V5":
            self.logger.info(f"bmc version : {major_ver}.{minor_ver}")
            subprocess.run(f"sed -i \"/^Version/s/{curr_ver}/{major_ver}.{minor_ver}/g\" update.cfg", shell=True)
        elif self.config.board_version == "V6":
            self.logger.info( f"bmc version : {major_ver}.{minor_ver}.{release_major_ver}.{release_minor_ver}")
            subprocess.run(
                f"sed -i \"/^Version/s/{curr_ver}/{major_ver}.{minor_ver}.{release_major_ver}.{release_minor_ver}/g\" update.cfg",
                shell=True)
        subprocess.run("chmod +x *", shell=True)


    def sign_cms_crl(self):
        sing_img_xml_filename = "sign_img_pss.xml" if self.sign_type == 'PSS' else "sign_img.xml"
        self.logger.info(f"sign type:{self.sign_type}, sign file:{self.common.code_root}/{sing_img_xml_filename}")
        if os.path.exists(f"{self.common.code_root}/{sing_img_xml_filename}"):
            self.logger.info("Use CI signature file!!!")
            self.common.copy(f"{self.common.code_root}/{sing_img_xml_filename}", f"{self.config.build_path}/{sing_img_xml_filename}")
        else:
            self.logger.info("Use empty signature file!!!")
        try:
            self.common.py_sed(f"{self.config.build_path}/{sing_img_xml_filename}", "(?<=path=\").*(?=\")", self.config.work_out)
            os.chdir(self.config.build_path)
            subprocess.run(f"java -jar /usr/local/signature-jenkins-slave/signature.jar {self.config.build_path}/{sing_img_xml_filename}", shell=True)
            # PSS需要手动更新crl文件
            if self.sign_type == 'PSS':
                subprocess.run(f"mkdir -p {self.config.build_path}/signature/crl/", shell=True)
                self.common.copy_pss_crl(f"{self.config.build_path}/signature/crl/crldata.crl")
        except:
            if self.config.sign == "":
                sign_file_tuple = (f"{self.config.work_out}/rootfs_{self.config.board_name}.img.cms",
                                    f"{self.config.work_out}/cms.crl")
                for f in sign_file_tuple:
                    with open(f, mode="w") as file_sign:
                        file_sign.close()
            else:
                err_msg = f"{EXCEPTION_CODE[600]}{EXCEPTION_STAGE['C']}{EXCEPTION_RESPONSE[2]}"
                self.logger.error(f"{err_msg}Sign file '{sing_img_xml_filename}' doesn't exist!!!")
        self.common.copy(f"{self.config.build_path}/signature/crl/crldata.crl", f"{self.config.work_out}/cms.crl")

    def run(self):
        self.set_global()
        if not self.config.support_dft_separation and self.config.build_type == "dft_separation":
            return
        if self.config.board_version == 'V6':
            self.sign_cms_crl()
            flag_rw = self.common.is_rw_emmc(self.config.board_name)
            if (self.config.build_type == "release" and flag_rw == False) or \
                (self.config.build_type == "debug" and flag_rw == True):
                self.preapre_gpp_rootfs_bin()
            self.prepare_gpp_bin()
        self.prepare_hpm()


# 提供脚本单独调试功能，在application/build目录下运行python3 works/xxxx.py
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="build target")
    parser.add_argument("-b", "--board_name", help="Board list must in '' and separated by space, default value is 2488hv6", default="2488hv6")
    args = parser.parse_args()
    cfg = Config(args.board_name)
    wk = WorkEnvirPrepare(cfg)
    wk.run()
    print("envir prepare end here")
