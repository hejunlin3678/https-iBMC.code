#!/usr/bin/env python
# coding=utf-8

"""
文件名：work_build_partner.py
功能：合作伙伴相关包（现网包、不校验包、维修包）打包，分为只读和可读写版本
版权信息：华为技术有限公司，版本所有(C) 2022-2023
"""

import os
import shutil
import argparse
import subprocess
import sys
cwd=os.path.dirname(__file__)
sys.path.append(f"{cwd}/../")
sys.path.append(f"{cwd}/../../")
from works.work import Work
from utils.config import Config
from utils.common import Common

class WorkBuildPartner(Work):

    def __init__(self, config, work_name=""):
        super(WorkBuildPartner, self).__init__(config, work_name)
        self.major_ver_bak = ""
        self.minor_ver_bak = ""
        self.pme_ver_bak = ""
        self.pmebackup_ver_bak = ""
        self.bmcaval_ver_bak = ""
        self.partner_packet_dir = os.path.join(self.common.temp_path, 'partnerpkt')
        self.packet_tmp_dir = os.path.join(self.partner_packet_dir, 'temppacket')
        self.partner_src_dir = os.path.join(self.common.internal_release_path, 'partner')
        self.partner_release = os.path.join(self.common.work_out, 'packet')
        self.common.check_path(self.partner_release)
        self.hpm_gpp_build_dir_src = f"{self.common.tools_path}/toolchain/emmc_hpm_header"
        self.hpm_build_dir_src = f"{self.common.tools_path}/toolchain/hpmpack/ipmcimage"
        self.dst_xml_file = f"{self.common.code_root}/application/src/resource/pme_profile/platform_v6.xml"
        self.partner_config_file = f"{self.common.code_root}/application/src/resource/board/{self.config.board_name}/archive_emmc_nand/partner_signature.ini"
        self.archive_conf_dic = self.common.get_archive_config(self.config.board_name)
        self.product_arch = self.common.get_product_arch(self.config.board_name)
        if self.config.board_name == "S920":
            self.product_arch = "Kunpeng"
        self.common.check_path(self.partner_packet_dir)

    def del_4k_file(self, img_path):
        size = os.path.getsize(img_path)
        with open(img_path, "r+") as img:
            img.truncate(size - 4 * 1024)

    def replace_files(self, type, p_type, img, file_list):
        tmp_rootfs = f"{self.partner_packet_dir}/rootfs"
        self.common.remove_path(tmp_rootfs)
        self.common.check_path(tmp_rootfs)
        self.common.get_img_flag()
        # 指定loop，解决偶现mount失败问题
        self.common.get_loop()
        self.run_command(f"LD_PRELOAD= sudo LD_PRELOAD=${{LD_PRELOAD}} -E mount -o loop={self.common.loop_dev} -t ext4 {img} {tmp_rootfs}")
        for file in file_list:
            if type == "ro":
                self.run_command(f"cp -dfr {self.common.temp_path}/{p_type}/release/tmp_root/{file} {tmp_rootfs}/{file}", super_user=True)
            else:
                self.run_command(f"cp -dfr {self.common.temp_path}/{p_type}/debug/tmp_root/{file} {tmp_rootfs}/{file}", super_user=True)
        xml_format_script = f"{self.config.common.code_path}/xml_format.py"
        profile_path = f"{tmp_rootfs}/opt/pme/conf/profile"
        self.run_command(f"python3 {xml_format_script} {profile_path} covert_keywords", super_user=True)
        self.common.umount_datafs(tmp_rootfs)

    def bak_ver(self):
        self.major_ver_bak = self.common.get_xml_value(self.dst_xml_file, 'PROPERTY', 'PMEMajorVer',     'VALUE')
        self.minor_ver_bak = self.common.get_xml_value(self.dst_xml_file, 'PROPERTY', 'PMEMinorVer',     'VALUE')
        self.pme_ver_bak = self.common.get_xml_value(self.dst_xml_file, 'PROPERTY', 'PMEVer',          'VALUE')
        self.pmebackup_ver_bak = self.common.get_xml_value(self.dst_xml_file, 'PROPERTY', 'PMEBackupVer',    'VALUE')
        self.bmcaval_ver_bak = self.common.get_xml_value(self.dst_xml_file, 'PROPERTY', 'BMCAvailableVer', 'VALUE')

    def update_ver(self, packet_mode):
        if packet_mode == "noverify":
            minor_ver = '01'

        if packet_mode == "repair":
            minor_ver = '02'

        major_ver = '99'
        pme_ver = major_ver + '.' + minor_ver + '.' + self.pme_ver_bak.split('.', 2)[2]
        pmebackup_ver = major_ver + '.' + minor_ver + '.' + self.pmebackup_ver_bak.split('.', 2)[2]
        bmcaval_ver = major_ver + '.' + minor_ver + '.' + self.bmcaval_ver_bak.split('.', 2)[2]
        self.common.set_xml_subelement_value(self.dst_xml_file, 'PROPERTY', 'PMEMajorVer',     'VALUE', major_ver)
        self.common.set_xml_subelement_value(self.dst_xml_file, 'PROPERTY', 'PMEMinorVer',     'VALUE', minor_ver)
        self.common.set_xml_subelement_value(self.dst_xml_file, 'PROPERTY', 'PMEVer',          'VALUE', pme_ver)
        self.common.set_xml_subelement_value(self.dst_xml_file, 'PROPERTY', 'PMEBackupVer',    'VALUE', pmebackup_ver)
        self.common.set_xml_subelement_value(self.dst_xml_file, 'PROPERTY', 'BMCAvailableVer', 'VALUE', bmcaval_ver)

    def restore_ver(self):
        self.common.set_xml_subelement_value(self.dst_xml_file, 'PROPERTY', 'PMEMajorVer',     'VALUE', self.major_ver_bak)
        self.common.set_xml_subelement_value(self.dst_xml_file, 'PROPERTY', 'PMEMinorVer',     'VALUE', self.minor_ver_bak)
        self.common.set_xml_subelement_value(self.dst_xml_file, 'PROPERTY', 'PMEVer',          'VALUE', self.pme_ver_bak)
        self.common.set_xml_subelement_value(self.dst_xml_file, 'PROPERTY', 'PMEBackupVer',    'VALUE', self.pmebackup_ver_bak)
        self.common.set_xml_subelement_value(self.dst_xml_file, 'PROPERTY', 'BMCAvailableVer', 'VALUE', self.bmcaval_ver_bak)

    def packet_public(self):
        self.common.remove_path(self.packet_tmp_dir)
        self.common.check_path(self.packet_tmp_dir)

        self.common.check_path(f"{self.packet_tmp_dir}/packet")
        self.common.check_path(f"{self.packet_tmp_dir}/output")
        self.common.check_path(f"{self.packet_tmp_dir}/partner_cert")
        self.common.check_path(f"{self.packet_tmp_dir}/packet/rootfs")
        self.common.check_path(f"{self.packet_tmp_dir}/packet/uboot")
        self.common.check_path(f"{self.packet_tmp_dir}/packet/others")

        shutil.copy(f"{self.partner_src_dir}/Signature_Guide.docx", self.packet_tmp_dir)
        shutil.copy(f"{self.partner_src_dir}/signedhpm_packet.py", self.packet_tmp_dir)
        shutil.copy(f"{self.partner_src_dir}/unsignedhpm_packet.py", self.packet_tmp_dir)
        shutil.copy(f"{self.partner_src_dir}/add_partner_cert_to_img.py", self.packet_tmp_dir)
        subprocess.run(f"sed -i s/\"G_BOARD_NAME\"/\"{self.config.board_name}\"/g {self.packet_tmp_dir}/unsignedhpm_packet.py", shell=True)
        self.common.copy_all(f"{self.common.code_path}/utils", f"{self.packet_tmp_dir}/packet/utils")
        shutil.copy(f"{self.partner_src_dir}/partner_common.py", f"{self.packet_tmp_dir}/packet/utils/common.py")

        pdt_dir = os.path.join(self.common.code_root, f"application/src/resource/board/{self.config.board_name}")
        uboot_dir = os.path.join(self.packet_tmp_dir, "packet/uboot")
        others_dir = os.path.join(self.packet_tmp_dir, "packet/others")

        shutil.copy(f"{self.partner_src_dir}/buildgppbin.py", f"{self.packet_tmp_dir}/packet")
        shutil.copy(f"{self.partner_src_dir}/buildhpm_ext4.py", f"{self.packet_tmp_dir}/packet")
        shutil.copy(f"{self.hpm_gpp_build_dir_src}/hpm_header.config", others_dir)
        shutil.copy(f"{self.hpm_gpp_build_dir_src}/gpp_header", others_dir)

        shutil.copy(f"{self.hpm_build_dir_src}/afteraction.sh", others_dir)
        shutil.copy(f"{self.hpm_build_dir_src}/beforeaction.sh", others_dir)
        shutil.copy(f"{self.hpm_build_dir_src}/CfgFileList.conf", others_dir)
        shutil.copy(f"{self.hpm_build_dir_src}/hpm_ipmc_ext4.config", others_dir)
        shutil.copy(f"{self.hpm_build_dir_src}/packethpm_ext4.sh", others_dir)
        shutil.copy(f"{self.hpm_build_dir_src}/hpmimage", others_dir)
        shutil.copy(f"{self.hpm_build_dir_src}/hpmimage_ext4", others_dir)
        shutil.copy(f"{self.hpm_build_dir_src}/crypto_tool_ext4", others_dir)
        shutil.copy(f"{self.hpm_build_dir_src}/filesizecheck", others_dir)
        shutil.copy(f"{self.partner_src_dir}/uboot/common", uboot_dir)
        shutil.copy(f"{self.partner_src_dir}/uboot/generate_sign_image.sh", uboot_dir)
        shutil.copy(f"{self.partner_src_dir}/uboot/version", uboot_dir)
        shutil.copy(f"{pdt_dir}/update_ext4.cfg", f"{others_dir}/update.cfg")

        os.chdir(others_dir)
        curr_ver = self.common.get_shell_command_result("cat update.cfg | grep '^Version' | awk -F '=' '{print $2}'")
        major_ver = self.common.get_xml_value(self.dst_xml_file, 'PROPERTY', 'PMEMajorVer', 'VALUE')
        minor_ver = self.common.get_xml_value(self.dst_xml_file, 'PROPERTY', 'PMEMinorVer', 'VALUE').zfill(2)
        release_major_ver = self.common.get_xml_value(self.dst_xml_file, 'PROPERTY', 'ReleaseMajorVer', 'VALUE').zfill(2)
        release_minor_ver = self.common.get_xml_value(self.dst_xml_file, 'PROPERTY', 'ReleaseMinorVer', 'VALUE').zfill(2)
        if minor_ver[0:2] == "0x":
            minor_ver = minor_ver[2:]
        subprocess.run(
            f"sed -i \"/^Version/s/{curr_ver}/{major_ver}.{minor_ver}.{release_major_ver}.{release_minor_ver}/g\" {others_dir}/update.cfg",
            shell=True)

    def uboot_parse(self, mode):
        if mode == "ro":
            uboot_name = "Hi1711_boot_4096.bin"
        elif mode == "rw":
            uboot_name = "Hi1711_boot_4096_debug.bin"

        ubootoffset = 768*1024
        oriubootoffset = ubootoffset + 12*6 + 76 + 4
        oriubootlenoffset = oriubootoffset + 4
        offset = int(subprocess.getstatusoutput(f"od -t x4 --skip-bytes={oriubootoffset} --read-bytes=4 {uboot_name} | head -1 | awk '{{print $2}}'")[1], 16)
        ubootlen = int(subprocess.getstatusoutput(f"od -t x4 --skip-bytes={oriubootlenoffset} --read-bytes=4 {uboot_name} | head -1 | awk '{{print $2}}'")[1], 16)
        with open(uboot_name, 'rb') as f1:
            f1.seek(ubootoffset+offset)
            with open("u-boot.bin", 'wb') as f2:
                data = f1.read(ubootlen)
                f2.write(data)

    def packet_special(self, mode, type):
        if mode == "ro":
            shutil.copy(f"{self.partner_src_dir}/uboot/uboot_merge.sh", f"{self.packet_tmp_dir}/packet")
            shutil.copy(f"{self.partner_src_dir}/uboot/u-boot_cms.cfg", f"{self.packet_tmp_dir}/packet/uboot")
            if type == "":
                shutil.copy(f"{self.common.temp_path}/release/output/rootfs_{self.config.board_name}.img", f"{self.packet_tmp_dir}/packet/rootfs")
            else:
                shutil.copy(f"{self.partner_packet_dir}/{type}/release/rootfs_{self.config.board_name}.img", f"{self.packet_tmp_dir}/packet/rootfs")
            shutil.copy(f"{self.common.bin_cmc_path}/application/platforms/platform_v6/Hi1711_boot_4096.bin", f"{self.packet_tmp_dir}/packet/uboot")
            img = f"rootfs_{self.config.board_name}.img"
            
        if mode == "rw":
            shutil.copy(f"{self.partner_src_dir}/uboot/uboot_debug_merge.sh", f"{self.packet_tmp_dir}/packet")
            shutil.copy(f"{self.partner_src_dir}/uboot/u-boot_cms_debug.cfg", f"{self.packet_tmp_dir}/packet/uboot")
            if type == "":
                shutil.copy(f"{self.common.temp_path}/debug/output/rootfs_{self.config.board_name}.img", f"{self.packet_tmp_dir}/packet/rootfs/rootfs_rw_{self.config.board_name}.img")
            else:
                shutil.copy(f"{self.partner_packet_dir}/{type}/debug/rootfs_{self.config.board_name}.img", f"{self.packet_tmp_dir}/packet/rootfs/rootfs_rw_{self.config.board_name}.img")
            shutil.copy(f"{self.common.bin_cmc_path}/application/platforms/platform_v6/Hi1711_boot_4096_debug.bin", f"{self.packet_tmp_dir}/packet/uboot")
            img = f"rootfs_rw_{self.config.board_name}.img"

        tmp_rootfs = f"{self.partner_packet_dir}/rootfs"
        self.common.remove_path(tmp_rootfs)
        self.common.check_path(tmp_rootfs)
        self.common.get_img_flag()
        os.chdir(f"{self.packet_tmp_dir}/packet/rootfs")
        self.common.get_loop()
        self.run_command(f"LD_PRELOAD= sudo LD_PRELOAD=${{LD_PRELOAD}} -E mount -o loop={self.common.loop_dev} -t ext4 {img} {tmp_rootfs}")
        # 删除online包和noverify包中rootfs分区中的华为根证书
        self.run_command(f"rm -rf {tmp_rootfs}/opt/pme/upgrade/partner_pss_root_cert", super_user=True)
        self.run_command(f"rm -rf {tmp_rootfs}/opt/pme/upgrade/partner_pkcs_root_cert", super_user=True)
        self.common.umount_datafs(tmp_rootfs)
    
        os.chdir(f"{self.packet_tmp_dir}/packet/uboot")
        self.uboot_parse(mode)

    def partner_packet(self, type=""):
        self.packet_public()
        self.packet_special("ro", type)
        os.chdir(self.packet_tmp_dir)
        subprocess.run("zip -r partner_packet.zip *", shell=True)
        subprocess.run(f"cp -f {self.packet_tmp_dir}/partner_packet.zip {self.partner_packet_dir}/{self.config.board_name}_ro_partner_packet.zip", shell=True)

        self.packet_public()
        self.packet_special("rw", type)
        os.chdir(self.packet_tmp_dir)
        subprocess.run("zip -r partner_packet.zip *", shell=True)
        subprocess.run(f"cp -f {self.packet_tmp_dir}/partner_packet.zip {self.partner_packet_dir}/{self.config.board_name}_rw_partner_packet.zip", shell=True)
        self.common.remove_path(self.packet_tmp_dir)
    
    def set_img_parma_area(self, image_path, platfile):
        subprocess.run(f"{self.common.code_path}/get_img_parma_area.sh {platfile} {image_path}", shell=True)

    def build_online(self):
        self.logger.info("build online mode start --->>>")
        self.partner_packet()
        subprocess.run(f"mv {self.partner_packet_dir}/{self.config.board_name}_ro_partner_packet.zip {self.common.work_out}/{self.config.board_name}_online.zip", shell=True)
        subprocess.run(f"mv {self.partner_packet_dir}/{self.config.board_name}_rw_partner_packet.zip {self.common.work_out}/{self.config.board_name}_rw_online.zip", shell=True)
        self.logger.info("build online mode end --->>>")

    def build_noverify(self):
        self.logger.info("build noverify mode start --->>>")
        self.common.check_path(f"{self.partner_packet_dir}/noverify/debug")
        self.common.check_path(f"{self.partner_packet_dir}/noverify/release")
        os.chdir(self.common.code_path)
        # 更改xml
        self.bak_ver()
        self.update_ver("noverify")
        self.common.copy(self.dst_xml_file, f"{self.common.temp_path}/noverify/release/tmp_root/opt/pme/conf/profile/platform.xml")
        self.common.copy(self.dst_xml_file, f"{self.common.temp_path}/noverify/debug/tmp_root/opt/pme/conf/profile/platform.xml")
        shutil.copy(f"{self.common.temp_path}/release/output/rootfs_{self.config.board_name}.img", f"{self.partner_packet_dir}/noverify/release")
        shutil.copy(f"{self.common.temp_path}/debug/output/rootfs_{self.config.board_name}.img", f"{self.partner_packet_dir}/noverify/debug")
        self.del_4k_file(f"{self.partner_packet_dir}/noverify/release/rootfs_{self.config.board_name}.img")
        self.del_4k_file( f"{self.partner_packet_dir}/noverify/debug/rootfs_{self.config.board_name}.img")
        files = ["opt/bmc/apps/legacy/upgrade/upgrade", "opt/pme/conf/profile/platform.xml"]
        self.replace_files("ro", "noverify", f"{self.partner_packet_dir}/noverify/release/rootfs_{self.config.board_name}.img" , files)
        self.set_img_parma_area(f"{self.partner_packet_dir}/noverify/release/rootfs_{self.config.board_name}.img", self.dst_xml_file)
        self.replace_files("rw", "noverify", f"{self.partner_packet_dir}/noverify/debug/rootfs_{self.config.board_name}.img" , files)
        self.set_img_parma_area(f"{self.partner_packet_dir}/noverify/debug/rootfs_{self.config.board_name}.img", self.dst_xml_file)
        self.partner_packet("noverify")
        subprocess.run(f"mv {self.partner_packet_dir}/{self.config.board_name}_ro_partner_packet.zip {self.common.work_out}/{self.config.board_name}_noverify.zip", shell=True)
        subprocess.run(f"mv {self.partner_packet_dir}/{self.config.board_name}_rw_partner_packet.zip {self.common.work_out}/{self.config.board_name}_rw_noverify.zip", shell=True)
        self.restore_ver()
        self.logger.info("build noverify mode end --->>>")

    def build_repair(self):
        self.logger.info("build repair mode start --->>>")
        os.chdir(self.common.code_path)
        self.bak_ver()
        self.update_ver("repair")
        self.common.copy(self.dst_xml_file, f"{self.common.temp_path}/repair/debug/tmp_root/opt/pme/conf/profile/platform.xml")
        self.common.check_path(f"{self.partner_packet_dir}/repair/debug")
        shutil.copy(f"{self.common.temp_path}/debug/output/rootfs_{self.config.board_name}.img", f"{self.partner_packet_dir}/repair/debug")
        shutil.copy(f"{self.common.temp_path}/debug/output/rootfs_{self.config.board_name}.img", 
            f"{self.common.temp_path}/debug/output/rootfs_{self.config.board_name}.img.bak")
        self.del_4k_file( f"{self.partner_packet_dir}/repair/debug/rootfs_{self.config.board_name}.img")
        files = ["opt/bmc/apps/legacy/bmc_global/bmc_global", "opt/pme/conf/profile/platform.xml"]
        self.replace_files("rw", "repair", f"{self.partner_packet_dir}/repair/debug/rootfs_{self.config.board_name}.img" , files)
        self.set_img_parma_area(f"{self.partner_packet_dir}/repair/debug/rootfs_{self.config.board_name}.img", self.dst_xml_file)
        self.common.copy(f"{self.partner_packet_dir}/repair/debug/rootfs_{self.config.board_name}.img", 
            f"{self.common.temp_path}/debug/output/rootfs_{self.config.board_name}.img")
        self.restore_ver()
        self.logger.info("build repair mode end --->>>")

    def partner_packet_zip(self):
        board_config_dir = f"{self.common.code_root}/application/src/resource/board/{self.config.board_name}/archive_emmc_nand"
        xml_file = f"{self.common.code_root}/application/src/resource/pme_profile/platform_v6.xml"
        sw_code = self.archive_conf_dic['togdp_dir'].split("/")[-1]
        ar_ver_file = f"{self.common.code_root}/application/src/resource/board/{self.config.board_name}/archive_emmc_nand/{sw_code}_version.ini"
        partner_packet_dir = f"{self.common.temp_path}/partner_packet_dir"
        signature_dir = f"{self.common.code_root}/application/src/resource/config/Common/customize/signature_file"
        
        if not os.path.exists(signature_dir):
            return
        self.common.copy_all(signature_dir, board_config_dir)
        
        partner_online_dir = self.common.get_file_info(self.partner_config_file).get("TOGDP_PATH_PARTNER_ONLINE")
        partner_online_name = self.common.get_file_info(self.partner_config_file).get("TOGDP_NAME_PARTNER_ONLINE")
        partner_online_0502name = self.common.get_file_info(self.partner_config_file).get("PARTNER_ONLINE_0502_NAME")    
        verify_pcert_dir = self.common.get_file_info(self.partner_config_file).get("TOGDP_PATH_VERIFY_PCERT")
        verify_pcert_name = self.common.get_file_info(self.partner_config_file).get("TOGDP_NAME_VERIFY_PCERT")
        verify_pcert_0502name = self.common.get_file_info(self.partner_config_file).get("VERIFY_PCERT_0502_NAME")
        partner_noverify_dir = self.common.get_file_info(self.partner_config_file).get("TOGDP_PATH_PARTNER_NOVERIFY")
        partner_noverify_name = self.common.get_file_info(self.partner_config_file).get("TOGDP_NAME_PARTNER_NOVERIFY")
        partner_noverify_0502name = self.common.get_file_info(self.partner_config_file).get("PARTNER_NOVERIFY_0502_NAME")
        partner_repair_dir = self.common.get_file_info(self.partner_config_file).get("TOGDP_PATH_PARTNER_REPAIR")
        partner_repair_name = self.common.get_file_info(self.partner_config_file).get("TOGDP_NAME_PARTNER_REPAIR")
        partner_repair_0502name = self.common.get_file_info(self.partner_config_file).get("PARTNER_REPAIR_0502_NAME")

        major_ver = self.common.get_xml_value(xml_file, 'PROPERTY', 'PMEMajorVer', 'VALUE')
        minor_ver = self.common.get_xml_value(xml_file, 'PROPERTY', 'PMEMinorVer', 'VALUE').zfill(2)
        release_major_ver = self.common.get_xml_value(xml_file, 'PROPERTY', 'ReleaseMajorVer', 'VALUE').zfill(2)
        release_minor_ver = self.common.get_xml_value(xml_file, 'PROPERTY', 'ReleaseMinorVer', 'VALUE').zfill(2)

        old_archieve_ver = self.common.get_file_info(ar_ver_file)['archive_version']
        active_uboot_ver = self.common.get_file_info(ar_ver_file)['ActiveBMC']
        backup_uboot_ver = self.common.get_file_info(ar_ver_file)['BackupBMC']

        # 现网版本
        self.common.remove_path(partner_packet_dir)
        self.common.check_path(partner_packet_dir)
        os.chdir(partner_packet_dir)
        target_pkg = f"{partner_online_name}{major_ver}.{minor_ver}.{release_major_ver}.{release_minor_ver}"
        self.common.check_path(target_pkg)

        shutil.copy(f"{self.common.work_out}/{self.config.board_name}_online.zip", target_pkg)
        shutil.copy(f"{board_config_dir}/add_temp_user.sh", target_pkg)
        shutil.copy(f"{board_config_dir}/common_transfer_flie.sh", target_pkg)
        shutil.copy(f"{board_config_dir}/pcert_load.sh", f"{target_pkg}/{partner_online_0502name}_load.sh")
        shutil.copy(ar_ver_file, f"{target_pkg}/{partner_online_0502name}_version.ini")
        shutil.copy(f"{board_config_dir}/modify_hpm_for_bt_upgrade.sh", target_pkg)
        shutil.copy(f"{board_config_dir}/efuse-crypt-image-partner.hpm", target_pkg)
        shutil.copy(f"{self.common.code_root}/internal_release/dft_tools/config/Common/customize/partner_pcert_fvbv_s1.bin", target_pkg)
        shutil.copy(f"{self.common.code_root}/internal_release/dft_tools/config/Common/customize/partner_pcert_fvbv_s2.bin", target_pkg)
        shutil.copy(f"{self.common.tools_path}/dft_tools/config/Common/customize/driver.tar.gz", target_pkg)
        shutil.copy(f"{self.common.tools_path}/dft_tools/hpmfwupg/{self.product_arch}/hpmfwupg_v5", f"{target_pkg}/hpmfwupg")
        shutil.copy(f"{self.common.tools_path}/dft_tools/config/Common/customize/{self.product_arch}/conver", target_pkg)
        shutil.copy(f"{board_config_dir}/upgrade_hpm.sh", target_pkg)
        shutil.copy(f"{self.common.tools_path}/dft_tools/config/Common/customize/wbd_verify.hpm", target_pkg)

        subprocess.run(f"sed -i '/^archive_version/s/{old_archieve_ver}/{major_ver}.{minor_ver}.{release_major_ver}.{release_minor_ver}/g' {target_pkg}/{partner_online_0502name}_version.ini", shell=True)
        subprocess.run(f"sed -i '/^ActiveBMC/s/{old_archieve_ver}/{major_ver}.{minor_ver}.{release_major_ver}.{release_minor_ver}/g' {target_pkg}/{partner_online_0502name}_version.ini", shell=True)
        subprocess.run(f"sed -i '/^BackupBMC/s/{old_archieve_ver}/{major_ver}.{minor_ver}.{release_major_ver}.{release_minor_ver}/g' {target_pkg}/{partner_online_0502name}_version.ini", shell=True)
        subprocess.run(f"zip -r {target_pkg}.zip {target_pkg}", shell=True)

        self.common.check_path(f"{self.partner_release}/{partner_online_dir}")
        shutil.move(f"{target_pkg}.zip", f"{self.partner_release}/{partner_online_dir}")

        # 不导根验证包
        self.common.remove_path(partner_packet_dir)
        self.common.check_path(partner_packet_dir)
        os.chdir(partner_packet_dir)
        target_pkg = f"{verify_pcert_name}{major_ver}.{minor_ver}.{release_major_ver}.{release_minor_ver}"
        self.common.check_path(target_pkg)
    
        shutil.copy(f"{board_config_dir}/add_temp_user.sh", target_pkg)
        shutil.copy(f"{board_config_dir}/common_transfer_flie.sh", target_pkg)
        shutil.copy(f"{board_config_dir}/verify_no_pcert_load.sh", f"{target_pkg}/{verify_pcert_0502name}_load.sh")
        shutil.copy(ar_ver_file, f"{target_pkg}/{verify_pcert_0502name}_version.ini")
        shutil.copy(f"{board_config_dir}/modify_hpm_for_bt_upgrade.sh", target_pkg)
        shutil.copy(f"{self.common.tools_path}/dft_tools/config/Common/customize/driver.tar.gz", target_pkg)
        shutil.copy(f"{self.common.tools_path}/dft_tools/hpmfwupg/{self.product_arch}/hpmfwupg_v5", f"{target_pkg}/hpmfwupg")
        shutil.copy(f"{self.common.tools_path}/dft_tools/config/Common/customize/{self.product_arch}/conver", target_pkg)
        shutil.copy(f"{board_config_dir}/upgrade_hpm.sh", target_pkg)
        shutil.copy(f"{self.common.tools_path}/dft_tools/config/Common/customize/wbd_verify.hpm", target_pkg)
    
        subprocess.run(f"sed -i '/^archive_version/s/{old_archieve_ver}/{major_ver}.{minor_ver}.{release_major_ver}.{release_minor_ver}/g' {target_pkg}/{verify_pcert_0502name}_version.ini", shell=True)
        subprocess.run(f"sed -i '/^ActiveBMC/s/{old_archieve_ver}/{major_ver}.{minor_ver}.{release_major_ver}.{release_minor_ver}/g' {target_pkg}/{verify_pcert_0502name}_version.ini", shell=True)
        subprocess.run(f"sed -i '/^BackupBMC/s/{old_archieve_ver}/{major_ver}.{minor_ver}.{release_major_ver}.{release_minor_ver}/g' {target_pkg}/{verify_pcert_0502name}_version.ini", shell=True)
        subprocess.run(f"zip -r {target_pkg}.zip {target_pkg}", shell=True)
    
        self.common.check_path(f"{self.partner_release}/{verify_pcert_dir}")
        shutil.move(f"{target_pkg}.zip", f"{self.partner_release}/{verify_pcert_dir}")
    
        # 不校验版本
        major_ver = 99
        minor_ver = "1".zfill(2)
        self.common.remove_path(partner_packet_dir)
        self.common.check_path(partner_packet_dir)
        os.chdir(partner_packet_dir)
        target_pkg = f"{partner_noverify_name}{major_ver}.{minor_ver}.{release_major_ver}.{release_minor_ver}"
        self.common.check_path(target_pkg)

        shutil.copy(f"{self.common.work_out}/{self.config.board_name}_noverify.zip", target_pkg)
        shutil.copy(f"{board_config_dir}/add_temp_user.sh", target_pkg)
        shutil.copy(f"{board_config_dir}/common_transfer_flie.sh", target_pkg)
        shutil.copy(f"{board_config_dir}/common_load.sh", target_pkg)
        shutil.copy(f"{board_config_dir}/{partner_noverify_0502name}_import_partner_root.sh", target_pkg)
        shutil.copy(f"{board_config_dir}/{partner_noverify_0502name}_load_no_verify.sh", target_pkg)
        shutil.copy(f"{board_config_dir}/pme_dft_video_test.bmp", target_pkg)
        shutil.copy(f"{board_config_dir}/modify_hpm_for_bt_upgrade.sh", target_pkg)
        shutil.copy(f"{self.common.tools_path}/dft_tools/hpmfwupg/{self.product_arch}/hpmfwupg_v5", f"{target_pkg}/hpmfwupg")
        shutil.copy(f"{self.common.tools_path}/dft_tools/config/Common/customize/driver.tar.gz", target_pkg)
        shutil.copy(ar_ver_file, f"{target_pkg}/{partner_noverify_0502name}_version.ini")

        subprocess.run(f"sed -i '/^archive_version/s/{old_archieve_ver}/{major_ver}.{minor_ver}.{release_major_ver}.{release_minor_ver}/g' {target_pkg}/{partner_noverify_0502name}_version.ini", shell=True)
        subprocess.run(f"sed -i '/^ActiveBMC/s/{old_archieve_ver}/{major_ver}.{minor_ver}.{release_major_ver}.{release_minor_ver}/g' {target_pkg}/{partner_noverify_0502name}_version.ini", shell=True)
        subprocess.run(f"sed -i '/^BackupBMC/s/{old_archieve_ver}/{major_ver}.{minor_ver}.{release_major_ver}.{release_minor_ver}/g' {target_pkg}/{partner_noverify_0502name}_version.ini", shell=True)
        subprocess.run(f"zip -r {target_pkg}.zip {target_pkg}", shell=True)

        self.common.check_path(f"{self.partner_release}/{partner_noverify_dir}")
        shutil.move(f"{target_pkg}.zip", f"{self.partner_release}/{partner_noverify_dir}")

        # 维修版本
        major_ver = 99
        minor_ver = "2".zfill(2)
        self.common.remove_path(partner_packet_dir)
        self.common.check_path(partner_packet_dir)
        os.chdir(partner_packet_dir)
        target_pkg = f"{partner_repair_name}{major_ver}.{minor_ver}.{release_major_ver}.{release_minor_ver}"
        self.common.check_path(target_pkg)

        shutil.copy(f"{self.common.work_out}/rootfs_rw_{self.config.board_name}_repair.hpm", f"{target_pkg}/image.hpm")
        shutil.copy(f"{board_config_dir}/add_temp_user.sh", target_pkg)
        shutil.copy(f"{board_config_dir}/common_transfer_flie.sh", target_pkg)
        shutil.copy(f"{board_config_dir}/common_load.sh", target_pkg)
        self.common.copy(f"{board_config_dir}/{partner_repair_0502name}_load_repair.sh", f"{target_pkg}/{partner_repair_0502name}_load.sh")
        shutil.copy(f"{board_config_dir}/{partner_repair_0502name}_import_repair_cert.sh", target_pkg)
        shutil.copy(f"{board_config_dir}/{partner_repair_0502name}_export_repair_cert.sh", target_pkg)
        shutil.copy(f"{board_config_dir}/pme_dft_video_test.bmp", target_pkg)
        shutil.copy(f"{board_config_dir}/modify_hpm_for_bt_upgrade.sh", target_pkg)
        shutil.copy(f"{self.common.tools_path}/dft_tools/hpmfwupg/{self.product_arch}/hpmfwupg_v5", f"{target_pkg}/hpmfwupg")
        shutil.copy(f"{self.common.tools_path}/dft_tools/config/Common/customize/driver.tar.gz", target_pkg)
        shutil.copy(ar_ver_file, f"{target_pkg}/{partner_repair_0502name}_version.ini")

        subprocess.run(f"sed -i '/^archive_version/s/{old_archieve_ver}/{major_ver}.{minor_ver}.{release_major_ver}.{release_minor_ver}/g' {target_pkg}/{partner_repair_0502name}_version.ini", shell=True)
        subprocess.run(f"sed -i '/^ActiveBMC/s/{old_archieve_ver}/{major_ver}.{minor_ver}.{release_major_ver}.{release_minor_ver}/g' {target_pkg}/{partner_repair_0502name}_version.ini", shell=True)
        subprocess.run(f"sed -i '/^BackupBMC/s/{old_archieve_ver}/{major_ver}.{minor_ver}.{release_major_ver}.{release_minor_ver}/g' {target_pkg}/{partner_repair_0502name}_version.ini", shell=True)
        subprocess.run(f"zip -r {target_pkg}.zip {target_pkg}", shell=True)

        self.common.check_path(f"{self.partner_release}/{partner_repair_dir}")
        shutil.move(f"{target_pkg}.zip", f"{self.partner_release}/{partner_repair_dir}")

    def run(self):
        self.logger.info("build partner start --->>>")
        if self.config.partner_compile_mode == "repair":
            # 维修包
            self.build_repair()
            return
        # 现网包
        self.build_online()
        # 不校验包
        self.build_noverify()
        # 组装zip包
        self.partner_packet_zip()
        self.logger.info("build partner end --->>>")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="build target")
    parser.add_argument("-b", "--board_name", help="Board list must in '' and separated by space, default value is 2488hv6", default="2488hv6")
    parser.add_argument("-t", "--build_type", help="build type, default value is debug, can be: debug, release, dft_separation", default="debug")
    args = parser.parse_args()
    cfg = Config(args.board_name)
    cfg.set_build_type(args.build_type)
    wk = WorkBuildPartner(cfg)
    wk.run()