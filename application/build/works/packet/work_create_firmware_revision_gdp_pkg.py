#!/usr/bin/env python
# coding: utf-8
"""
功 能：work_create_leakdetect_abs_alarm_gdp.py脚本
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

class Firmware_Revision(Work):
    def __init__(self, config, work_name=""):
        super(Firmware_Revision, self).__init__(config)
        self.config = config
        self.archive_conf_dic = self.common.get_archive_config(self.config.board_name)
        self.release_dir = f"{self.config.board_name}_revision_release"
        self.resource_dir = f"{self.common.code_root}/application/src/resource"
        self.archive_emmc_nand_dir = f"{self.resource_dir}/board/{self.config.board_name}/archive_emmc_nand"
        self.customize_dir = f"{self.release_dir}/dft_tools/config/Common/customize"
        self.sw_code = self.archive_conf_dic['togdp_dir'].split('/')[-1]
        self.version = self.common.get_board_version(self.config.board_name) 
        self.product_arch = self.common.get_product_arch(self.config.board_name) 
        self.lcu_flag = False
        self.fw_revs_flag = False
        self.hmf_flag = False
        self.tbj_flag = False
        self.ht_flag = False

    def run(self):

        if self.config.board_name not in ("TaiShan2280v2", "S920X05", "TaiShan2280Mv2", "Atlas880", "BM320", "TaiShan2280v2_1711"):
            return 
        if self.config.board_name == "TaiShan2280v2":
            if self.archive_conf_dic.get("togdp_dir_lcu", "") == "":
                return
            target_pkg = f"{self.archive_conf_dic['togdp_name_lcu']}"
            self.lcu_flag = True
            copy_file_list = [f"{self.archive_emmc_nand_dir}/revision.ini", 
                            f"{self.archive_emmc_nand_dir}/{self.archive_conf_dic['togdp_lcu_0502']}_load.sh"]
        elif self.config.board_name in ("S920X05", "TaiShan2280Mv2", "Atlas880", "BM320"):
            if self.archive_conf_dic.get("togdp_dir_fw_revs", "") == "":
                return
            target_pkg = f"{self.archive_conf_dic['togdp_name_fw_revs']}"
            self.fw_revs_flag = True
            copy_file_list = [f"{self.archive_emmc_nand_dir}/revision.ini"]
        if self.config.board_name in ("TaiShan2280Mv2", "BM320"):
            if self.archive_conf_dic.get("togdp_dir_hmf", "") == "":
                return
            target_pkg_trustzone = f"{self.archive_conf_dic['togdp_name_hmf']}"
            self.hmf_flag = True
            copy_trustzone_file_list = [f"{self.archive_emmc_nand_dir}/trustzone_revision.ini"]
        if self.config.board_name == "TaiShan2280Mv2":
            if self.archive_conf_dic.get("togdp_dir_tbj", "") == "":
                return
            target_pkg_tbj = f"{self.archive_conf_dic['togdp_name_tbj']}"
            self.tbj_flag = True
        if self.config.board_name == "TaiShan2280Mv2":
            if self.archive_conf_dic.get("togdp_ht_dir_fw_revs", "") == "":
                return
            target_pkg_ht = f"{self.archive_conf_dic['togdp_ht_name_fw_revs']}"
            self.ht_flag = True
        if self.config.board_name == "TaiShan2280v2_1711":
            if self.archive_conf_dic.get("togdp_ht_dir_fw_revs", "") == "":
                return
            target_pkg_ht = f"{self.archive_conf_dic['togdp_ht_name_fw_revs']}"
            copy_file_list = [f"{self.archive_emmc_nand_dir}/revision.ini"]
            target_pkg = f"{self.archive_conf_dic['togdp_ht_name_fw_revs']}"
            self.ht_flag = True

        self.logger.info(f"create compressed packet for {self.config.board_name} of  Firmware_Revision GDP PKG ------------> [begin]")
        # 创建临时目录 
        os.chdir(self.config.build_path)
        self.common.get_dft_tools(self.release_dir, self.config.board_name)

        # copy archive 下的文件
        file_list = self.common.py_find_dir(".*version\.ini", self.archive_emmc_nand_dir)
        for copy_file in file_list:
            shutil.copy(copy_file, self.release_dir)

        os.chdir(self.release_dir)

        # 更新version.ini文件；
        ar_ver_file = f"{self.sw_code}_version.ini"
        if not os.path.exists(ar_ver_file):
            self.logger.error("can not find version profile")
            self.common.remove_path(f"{self.config.build_path}/{self.release_dir}")
            return
        # 提取xml中的版本号
        ver_num = self.common.extract_ver_num(ar_ver_file, self.config.board_name, readonly=True)
        target_pkg = target_pkg + ver_num
        self.common.check_path(target_pkg)
        if self.hmf_flag:
            target_pkg_trustzone = target_pkg_trustzone + ver_num
            self.common.check_path(target_pkg_trustzone)
        if self.tbj_flag:
            target_pkg_tbj = target_pkg_tbj + ver_num
            self.common.check_path(target_pkg_tbj)
        if self.ht_flag:
            target_pkg_ht = target_pkg_ht + ver_num
            self.common.check_path(target_pkg_ht)
        os.chdir(self.config.build_path)
         # 拷贝文件
        common_list = [ f"{self.archive_emmc_nand_dir}/add_temp_user.sh",
                        f"{self.archive_emmc_nand_dir}/upgrade_hpm.sh",
                        f"{self.customize_dir}/{self.product_arch}/conver",
                        f"{self.customize_dir}/driver.tar.gz" ]
        
        copy_file_list = copy_file_list + common_list
        for file in copy_file_list:
            self.common.copy(file, 
            f"{self.release_dir}/{target_pkg}/{file.rsplit('/')[-1]}")
        self.common.copy(f"{self.customize_dir}/dftimage_v5.hpm", f"{self.release_dir}/{target_pkg}/dftimage.hpm")
        self.common.copy(f"{self.release_dir}/dft_tools/hpmfwupg/{self.product_arch}/hpmfwupg_v5",
                    f"{self.release_dir}/{target_pkg}/hpmfwupg")	
        if self.fw_revs_flag :
            self.common.copy(f"{self.customize_dir}/revision_config.sh", 
            f"{self.release_dir}/{target_pkg}/{self.archive_conf_dic['togdp_fw_revs_0502']}_load.sh")
            with open(f"{self.release_dir}/{target_pkg}/revision_config.sh", 'w') as f:
                f.write('#!/bin/sh\n')
                f.write('file_name=`echo $0 | awk -F \"/\" \'{printf $2}\'`\n')
                f.write('echo \"execute $file_name success\"\n')
                f.write('exit 0\n')
            self.common.copy(f"{self.release_dir}/{target_pkg}/revision_config.sh", 
                f"{self.release_dir}/{target_pkg}/{self.archive_conf_dic['togdp_fw_revs_0502']}_config.sh")
            self.common.copy(f"{self.release_dir}/{target_pkg}/revision_config.sh", 
                f"{self.release_dir}/{target_pkg}/{self.archive_conf_dic['togdp_fw_revs_0502']}_check.sh")
            self.common.copy(f"{self.release_dir}/{target_pkg}/revision_config.sh", 
                f"{self.release_dir}/{target_pkg}/{self.archive_conf_dic['togdp_fw_revs_0502']}_verify.sh")
            self.common.remove_file(f"{self.release_dir}/{target_pkg}/revision_config.sh")
            
            xml_file = f"{self.common.code_root}/application/src/resource/pme_profile/platform_v6.xml"
            sw_code = self.archive_conf_dic['togdp_dir'].split("/")[-1]
            ar_ver_file = f"{self.common.code_root}/application/src/resource/board/{self.config.board_name}/archive_emmc_nand/{sw_code}_version.ini"

            major_ver = self.common.get_xml_value(xml_file, 'PROPERTY', 'PMEMajorVer', 'VALUE')
            minor_ver = self.common.get_xml_value(xml_file, 'PROPERTY', 'PMEMinorVer', 'VALUE').zfill(2)
            release_major_ver = self.common.get_xml_value(xml_file, 'PROPERTY', 'ReleaseMajorVer', 'VALUE').zfill(2)
            release_minor_ver = self.common.get_xml_value(xml_file, 'PROPERTY', 'ReleaseMinorVer', 'VALUE').zfill(2)

            old_archieve_ver = self.common.get_file_info(ar_ver_file)['archive_version']
            active_uboot_ver = self.common.get_file_info(ar_ver_file)['ActiveBMC']
            backup_uboot_ver = self.common.get_file_info(ar_ver_file)['BackupBMC']
            shutil.copy(ar_ver_file, f"{self.release_dir}/{target_pkg}/{self.archive_conf_dic['togdp_fw_revs_0502']}_version.ini")
            subprocess.run(f"sed -i '/^archive_version/s/{old_archieve_ver}/{major_ver}.{minor_ver}.{release_major_ver}.{release_minor_ver}/g' {self.release_dir}/{target_pkg}/{self.archive_conf_dic['togdp_fw_revs_0502']}_version.ini", shell=True)
            subprocess.run(f"sed -i '/^ActiveBMC/s/{old_archieve_ver}/{major_ver}.{minor_ver}.{release_major_ver}.{release_minor_ver}/g' {self.release_dir}/{target_pkg}/{self.archive_conf_dic['togdp_fw_revs_0502']}_version.ini", shell=True)
            subprocess.run(f"sed -i '/^BackupBMC/s/{old_archieve_ver}/{major_ver}.{minor_ver}.{release_major_ver}.{release_minor_ver}/g' {self.release_dir}/{target_pkg}/{self.archive_conf_dic['togdp_fw_revs_0502']}_version.ini", shell=True)
        if self.hmf_flag :
            copy_trustzone_file_list = copy_trustzone_file_list + common_list
            for file in copy_trustzone_file_list:
                self.common.copy(file, 
                f"{self.release_dir}/{target_pkg_trustzone}/{file.rsplit('/')[-1]}")
            self.common.copy(f"{self.customize_dir}/dftimage_v5.hpm", f"{self.release_dir}/{target_pkg_trustzone}/dftimage.hpm")
            self.common.copy(f"{self.release_dir}/dft_tools/hpmfwupg/{self.product_arch}/hpmfwupg_v5",
                        f"{self.release_dir}/{target_pkg_trustzone}/hpmfwupg")	
            self.common.copy(f"{self.archive_emmc_nand_dir}/05023HMF_load.sh", 
                f"{self.release_dir}/{target_pkg_trustzone}/{self.archive_conf_dic['togdp_hmf_0502']}_load.sh")
            with open(f"{self.release_dir}/{target_pkg_trustzone}/hmf.sh", 'w') as f:
                f.write('#!/bin/sh\n')
                f.write('file_name=`echo $0 | awk -F \"/\" \'{printf $2}\'`\n')
                f.write('echo \"execute $file_name success\"\n')
                f.write('exit 0\n')
            self.common.copy(f"{self.release_dir}/{target_pkg_trustzone}/hmf.sh", 
                f"{self.release_dir}/{target_pkg_trustzone}/{self.archive_conf_dic['togdp_hmf_0502']}_config.sh")
            self.common.copy(f"{self.release_dir}/{target_pkg_trustzone}/hmf.sh", 
                f"{self.release_dir}/{target_pkg_trustzone}/{self.archive_conf_dic['togdp_hmf_0502']}_check.sh")
            self.common.copy(f"{self.release_dir}/{target_pkg_trustzone}/hmf.sh", 
                f"{self.release_dir}/{target_pkg_trustzone}/{self.archive_conf_dic['togdp_hmf_0502']}_verify.sh")
            self.common.remove_file(f"{self.release_dir}/{target_pkg_trustzone}/hmf.sh")
            
            xml_file = f"{self.common.code_root}/application/src/resource/pme_profile/platform_v6.xml"
            sw_code = self.archive_conf_dic['togdp_dir'].split("/")[-1]
            ar_ver_file = f"{self.common.code_root}/application/src/resource/board/{self.config.board_name}/archive_emmc_nand/{sw_code}_version.ini"
 
            major_ver = self.common.get_xml_value(xml_file, 'PROPERTY', 'PMEMajorVer', 'VALUE')
            minor_ver = self.common.get_xml_value(xml_file, 'PROPERTY', 'PMEMinorVer', 'VALUE').zfill(2)
            release_major_ver = self.common.get_xml_value(xml_file, 'PROPERTY', 'ReleaseMajorVer', 'VALUE').zfill(2)
            release_minor_ver = self.common.get_xml_value(xml_file, 'PROPERTY', 'ReleaseMinorVer', 'VALUE').zfill(2)
 
            old_archieve_ver = self.common.get_file_info(ar_ver_file)['archive_version']
            active_uboot_ver = self.common.get_file_info(ar_ver_file)['ActiveBMC']
            backup_uboot_ver = self.common.get_file_info(ar_ver_file)['BackupBMC']
            shutil.copy(ar_ver_file, f"{self.release_dir}/{target_pkg_trustzone}/{self.archive_conf_dic['togdp_hmf_0502']}_version.ini")
            subprocess.run(f"sed -i '/^archive_version/s/{old_archieve_ver}/{major_ver}.{minor_ver}.{release_major_ver}.{release_minor_ver}/g' {self.release_dir}/{target_pkg_trustzone}/{self.archive_conf_dic['togdp_hmf_0502']}_version.ini", shell=True)
            subprocess.run(f"sed -i '/^ActiveBMC/s/{old_archieve_ver}/{major_ver}.{minor_ver}.{release_major_ver}.{release_minor_ver}/g' {self.release_dir}/{target_pkg_trustzone}/{self.archive_conf_dic['togdp_hmf_0502']}_version.ini", shell=True)
            subprocess.run(f"sed -i '/^BackupBMC/s/{old_archieve_ver}/{major_ver}.{minor_ver}.{release_major_ver}.{release_minor_ver}/g' {self.release_dir}/{target_pkg_trustzone}/{self.archive_conf_dic['togdp_hmf_0502']}_version.ini", shell=True)
        if self.tbj_flag :
            copy_tbj_file_list = common_list
            for file in copy_tbj_file_list:
                self.common.copy(file, 
                f"{self.release_dir}/{target_pkg_tbj}/{file.rsplit('/')[-1]}")
            self.common.copy(f"{self.archive_emmc_nand_dir}/revision_tbj.ini", 
                        f"{self.release_dir}/{target_pkg_tbj}/revision.ini")
            self.common.copy(f"{self.customize_dir}/dftimage_v5.hpm", f"{self.release_dir}/{target_pkg_tbj}/dftimage.hpm")
            self.common.copy(f"{self.release_dir}/dft_tools/hpmfwupg/{self.product_arch}/hpmfwupg_v5",
                        f"{self.release_dir}/{target_pkg_tbj}/hpmfwupg")
            self.common.copy(f"{self.archive_emmc_nand_dir}/05023TBJ_load.sh", 
                        f"{self.release_dir}/{target_pkg_tbj}/{self.archive_conf_dic['togdp_tbj_0502']}_load.sh")
            with open(f"{self.release_dir}/{target_pkg_tbj}/tbj.sh", 'w') as f:
                f.write('#!/bin/sh\n')
                f.write('file_name=`echo $0 | awk -F \"/\" \'{printf $2}\'`\n')
                f.write('echo \"execute $file_name success\"\n')
                f.write('exit 0\n')
            self.common.copy(f"{self.release_dir}/{target_pkg_tbj}/tbj.sh", 
                f"{self.release_dir}/{target_pkg_tbj}/{self.archive_conf_dic['togdp_tbj_0502']}_config.sh")
            self.common.copy(f"{self.release_dir}/{target_pkg_tbj}/tbj.sh", 
                f"{self.release_dir}/{target_pkg_tbj}/{self.archive_conf_dic['togdp_tbj_0502']}_check.sh")
            self.common.copy(f"{self.release_dir}/{target_pkg_tbj}/tbj.sh", 
                f"{self.release_dir}/{target_pkg_tbj}/{self.archive_conf_dic['togdp_tbj_0502']}_verify.sh")
            self.common.remove_file(f"{self.release_dir}/{target_pkg_tbj}/tbj.sh")
            
            xml_file = f"{self.common.code_root}/application/src/resource/pme_profile/platform_v6.xml"
            sw_code = self.archive_conf_dic['togdp_dir'].split("/")[-1]
            ar_ver_file = f"{self.common.code_root}/application/src/resource/board/{self.config.board_name}/archive_emmc_nand/{sw_code}_version.ini"

            major_ver = self.common.get_xml_value(xml_file, 'PROPERTY', 'PMEMajorVer', 'VALUE')
            minor_ver = self.common.get_xml_value(xml_file, 'PROPERTY', 'PMEMinorVer', 'VALUE').zfill(2)
            release_major_ver = self.common.get_xml_value(xml_file, 'PROPERTY', 'ReleaseMajorVer', 'VALUE').zfill(2)
            release_minor_ver = self.common.get_xml_value(xml_file, 'PROPERTY', 'ReleaseMinorVer', 'VALUE').zfill(2)

            old_archieve_ver = self.common.get_file_info(ar_ver_file)['archive_version']
            active_uboot_ver = self.common.get_file_info(ar_ver_file)['ActiveBMC']
            backup_uboot_ver = self.common.get_file_info(ar_ver_file)['BackupBMC']
            shutil.copy(ar_ver_file, f"{self.release_dir}/{target_pkg_tbj}/{self.archive_conf_dic['togdp_tbj_0502']}_version.ini")
            subprocess.run(f"sed -i '/^archive_version/s/{old_archieve_ver}/{major_ver}.{minor_ver}.{release_major_ver}.{release_minor_ver}/g' {self.release_dir}/{target_pkg_tbj}/{self.archive_conf_dic['togdp_tbj_0502']}_version.ini", shell=True)
            subprocess.run(f"sed -i '/^ActiveBMC/s/{old_archieve_ver}/{major_ver}.{minor_ver}.{release_major_ver}.{release_minor_ver}/g' {self.release_dir}/{target_pkg_tbj}/{self.archive_conf_dic['togdp_tbj_0502']}_version.ini", shell=True)
            subprocess.run(f"sed -i '/^BackupBMC/s/{old_archieve_ver}/{major_ver}.{minor_ver}.{release_major_ver}.{release_minor_ver}/g' {self.release_dir}/{target_pkg_tbj}/{self.archive_conf_dic['togdp_tbj_0502']}_version.ini", shell=True)
        if self.ht_flag :
            copy_ht_file_list = common_list
            for file in copy_ht_file_list:
                self.common.copy(file, 
                f"{self.release_dir}/{target_pkg_ht}/{file.rsplit('/')[-1]}")
            self.common.copy(f"{self.archive_emmc_nand_dir}/revision_ht.ini", 
                        f"{self.release_dir}/{target_pkg_ht}/revision.ini")
            self.common.copy(f"{self.customize_dir}/dftimage_v5.hpm", f"{self.release_dir}/{target_pkg_ht}/dftimage.hpm")
            self.common.copy(f"{self.release_dir}/dft_tools/hpmfwupg/{self.product_arch}/hpmfwupg_v5",
                        f"{self.release_dir}/{target_pkg_ht}/hpmfwupg")
            if self.config.board_name == "TaiShan2280v2_1711":
                self.common.copy(f"{self.archive_emmc_nand_dir}/05024HTL_load.sh", 
                        f"{self.release_dir}/{target_pkg_ht}/{self.archive_conf_dic['togdp_ht_fw_revs_0502']}_load.sh")
            if self.config.board_name == "TaiShan2280Mv2":
                self.common.copy(f"{self.archive_emmc_nand_dir}/05024HTJ_load.sh", 
                        f"{self.release_dir}/{target_pkg_ht}/{self.archive_conf_dic['togdp_ht_fw_revs_0502']}_load.sh")

            with open(f"{self.release_dir}/{target_pkg_ht}/hmf.sh", 'w') as f:
                f.write('#!/bin/sh\n')
                f.write('file_name=`echo $0 | awk -F \"/\" \'{printf $2}\'`\n')
                f.write('echo \"execute $file_name success\"\n')
                f.write('exit 0\n')
            self.common.copy(f"{self.release_dir}/{target_pkg_ht}/hmf.sh", 
                f"{self.release_dir}/{target_pkg_ht}/{self.archive_conf_dic['togdp_ht_fw_revs_0502']}_config.sh")
            self.common.copy(f"{self.release_dir}/{target_pkg_ht}/hmf.sh", 
                f"{self.release_dir}/{target_pkg_ht}/{self.archive_conf_dic['togdp_ht_fw_revs_0502']}_check.sh")
            self.common.copy(f"{self.release_dir}/{target_pkg_ht}/hmf.sh", 
                f"{self.release_dir}/{target_pkg_ht}/{self.archive_conf_dic['togdp_ht_fw_revs_0502']}_verify.sh")
            self.common.remove_file(f"{self.release_dir}/{target_pkg_ht}/hmf.sh")
            
            xml_file = f"{self.common.code_root}/application/src/resource/pme_profile/platform_v6.xml"
            sw_code = self.archive_conf_dic['togdp_dir'].split("/")[-1]
            ar_ver_file = f"{self.common.code_root}/application/src/resource/board/{self.config.board_name}/archive_emmc_nand/{sw_code}_version.ini"
 
            major_ver = self.common.get_xml_value(xml_file, 'PROPERTY', 'PMEMajorVer', 'VALUE')
            minor_ver = self.common.get_xml_value(xml_file, 'PROPERTY', 'PMEMinorVer', 'VALUE').zfill(2)
            release_major_ver = self.common.get_xml_value(xml_file, 'PROPERTY', 'ReleaseMajorVer', 'VALUE').zfill(2)
            release_minor_ver = self.common.get_xml_value(xml_file, 'PROPERTY', 'ReleaseMinorVer', 'VALUE').zfill(2)
 
            old_archieve_ver = self.common.get_file_info(ar_ver_file)['archive_version']
            active_uboot_ver = self.common.get_file_info(ar_ver_file)['ActiveBMC']
            backup_uboot_ver = self.common.get_file_info(ar_ver_file)['BackupBMC']
            shutil.copy(ar_ver_file, f"{self.release_dir}/{target_pkg_ht}/{self.archive_conf_dic['togdp_ht_fw_revs_0502']}_version.ini")
            subprocess.run(f"sed -i '/^archive_version/s/{old_archieve_ver}/{major_ver}.{minor_ver}.{release_major_ver}.{release_minor_ver}/g' {self.release_dir}/{target_pkg_ht}/{self.archive_conf_dic['togdp_ht_fw_revs_0502']}_version.ini", shell=True)
            subprocess.run(f"sed -i '/^ActiveBMC/s/{old_archieve_ver}/{major_ver}.{minor_ver}.{release_major_ver}.{release_minor_ver}/g' {self.release_dir}/{target_pkg_ht}/{self.archive_conf_dic['togdp_ht_fw_revs_0502']}_version.ini", shell=True)
            subprocess.run(f"sed -i '/^BackupBMC/s/{old_archieve_ver}/{major_ver}.{minor_ver}.{release_major_ver}.{release_minor_ver}/g' {self.release_dir}/{target_pkg_ht}/{self.archive_conf_dic['togdp_ht_fw_revs_0502']}_version.ini", shell=True)
            self.logger.info(f"1111111111111111111111111111111111111111111111111111 ------------> [end]")
        os.chdir(self.release_dir)
        
        if self.lcu_flag:
            self.run_command(f'zip -rq "{target_pkg}".zip {target_pkg}')
            self.common.remove_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_lcu']}")
            self.common.check_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_lcu']}")
            self.logger.info(f"move togdp_dir_lcu file to {self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_lcu']}")
            shutil.move(f"{self.config.build_path}/{self.release_dir}/{target_pkg}.zip",
                    f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_lcu']}")
        if self.fw_revs_flag:
            self.run_command(f'zip -rq "{target_pkg}".zip {target_pkg}')
            self.common.remove_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_fw_revs']}")
            self.common.check_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_fw_revs']}")
            self.logger.info(f"move togdp_dir_fw_revs file to {self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_fw_revs']}")
            shutil.move(f"{self.config.build_path}/{self.release_dir}/{target_pkg}.zip",
                    f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_fw_revs']}")
        if self.tbj_flag:
            self.run_command(f'zip -rq "{target_pkg_tbj}".zip {target_pkg_tbj}')
            self.common.remove_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_tbj']}")
            self.common.check_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_tbj']}")
            self.logger.info(f"move togdp_dir_tbj file to {self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_tbj']}")
            shutil.move(f"{self.config.build_path}/{self.release_dir}/{target_pkg_tbj}.zip",
                    f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_tbj']}")

        if self.hmf_flag :
            self.run_command(f'zip -rq "{target_pkg_trustzone}".zip {target_pkg_trustzone}')
            self.common.remove_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_hmf']}")
            self.common.check_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_hmf']}")
            self.logger.info(f"move togdp_dir_hmf file to {self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_hmf']}")
            shutil.move(f"{self.config.build_path}/{self.release_dir}/{target_pkg_trustzone}.zip",
                    f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_hmf']}")

        if self.ht_flag:
            self.run_command(f'zip -rq "{target_pkg_ht}".zip {target_pkg_ht}')
            self.logger.info(f"1111111111111111111111111111111111111111111111111111 ------------> [end]")
            self.common.remove_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_ht_dir_fw_revs']}")
            self.common.check_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_ht_dir_fw_revs']}")
            self.logger.info(f"move togdp_ht_dir_fw_revs file to {self.common.work_out}/packet/{self.archive_conf_dic['togdp_ht_dir_fw_revs']}")
            shutil.move(f"{self.config.build_path}/{self.release_dir}/{target_pkg_ht}.zip",
                    f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_ht_dir_fw_revs']}")

        self.common.remove_path(f"{self.config.build_path}/{self.release_dir}")

        self.logger.info(f"create compressed packet for {self.config.board_name} of Firmware_Revision GDP PKG ------------> [end]")


if __name__ == "__main__":
    cfg = Config("TaiShan2280v2", "target_personal")
    wk = Firmware_Revision(cfg)
    wk.run()