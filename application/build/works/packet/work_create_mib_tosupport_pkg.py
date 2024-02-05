#!/usr/bin/env python
# coding: utf-8
"""
功 work_create_mib_tosupport_pkg.py脚本
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

class Mib(Work):
    def __init__(self, config, work_name=""):
        super(Mib, self).__init__(config)
        self.config = config
        self.archive_conf_dic = self.common.get_archive_config(self.config.board_name)
        self.release_dir = f"{self.config.board_name}_mib_release"
        self.resource_dir = f"{self.common.code_root}/application/src/resource"
        self.archive_emmc_nand_dir = f"{self.resource_dir}/board/{self.config.board_name}/archive_emmc_nand"
        self.sw_code = self.archive_conf_dic['togdp_dir'].split('/')[-1]
        self.version = self.common.get_board_version(self.config.board_name) 

    def create_cb_mib_pkg(self):
        cb_archives = self.common.py_find_dir(f".*_cb_flag_archive.ini", f"{self.resource_dir}/board/{self.config.board_name}")
        for cb_archive in cb_archives:
            cb_archive_dic = self.common.get_cb_archive_config(cb_archive)
            # 生成ToSupport目录下MIB文件的zip包
            if cb_archive_dic['cb_flag_tosupport_mib_dir'] != "" and cb_archive_dic['cb_flag_tosupport_mib_name'] != "":
                release_dir = f"{self.config.board_name}_mib_release_{cb_archive_dic['cb_flag_sw_code']}"
                os.chdir(self.config.build_path)
                self.common.remove_path(release_dir)
                self.common.check_path(release_dir)
                ver_num = self.common.extract_ver_num("", self.config.board_name, True)
                if self.version == "V5":
                    self.common.copy(f"{self.resource_dir}/config/Common/HUAWEI-SERVER-iBMC-MIB.mib", 
                    f"{release_dir}/HUAWEI-SERVER-iBMC-MIB.mib")
                if self.version == "V6":
                    self.common.copy(f"{self.resource_dir}/config/Common/HUAWEI-SERVER-iBMC-MIB-V6.mib", 
                    f"{release_dir}/HUAWEI-SERVER-iBMC-MIB.mib")
                
                target_pkg = f"{cb_archive_dic['cb_flag_tosupport_mib_name']}{ver_num}"
                self.common.remove_path(target_pkg)
                shutil.move(release_dir, target_pkg)
                os.chdir(target_pkg)
                self.common.run_command("sed -i \"/Copyright/d\" ./HUAWEI-SERVER-iBMC-MIB.mib")
                self.common.run_command("sed -i \"s/HUAWEI-SERVER-IBMC-MIB/OEM-SERVER-BMC-MIB/g\" ./HUAWEI-SERVER-iBMC-MIB.mib")
                self.common.run_command("sed -i \"s/Default value is huawei/Default value is BMC/g\" ./HUAWEI-SERVER-iBMC-MIB.mib")
                self.common.run_command("sed -i \"s/Huawei Technologies Co.,Ltd.//g\" ./HUAWEI-SERVER-iBMC-MIB.mib")
                self.common.run_command("sed -i \"/http:\/\/www.huawei.com/d\" ./HUAWEI-SERVER-iBMC-MIB.mib")
                self.common.run_command("sed -i \"/support@huawei.com/d\" ./HUAWEI-SERVER-iBMC-MIB.mib")
                self.common.run_command("sed -i \"s/CONTACT-INFO/CONTACT-INFO      \"\"/g\" ./HUAWEI-SERVER-iBMC-MIB.mib")
                self.common.run_command("sed -i \"s/huawei/oem/g\" ./HUAWEI-SERVER-iBMC-MIB.mib")
                # iBMC全局直接替换为BMC导致“hwBMC OBJECT IDENTIFIER”和“hwiBMC MODULE-IDENTITY”对象名重复，导致MIB使用有问题，需要先将hwiBMC重命名oemBMCObject
                self.common.run_command("sed -i \"s/hwiBMC/oemBMCObject/g\" ./HUAWEI-SERVER-iBMC-MIB.mib")
                # firmwareType中的iBMC作为对象名必须小写开头，否则编译会有warning
                self.common.run_command("sed -i \"s/iBMC(1)/bmc(1)/g\" ./HUAWEI-SERVER-iBMC-MIB.mib")
                self.common.run_command("sed -i \"s/iBMC/BMC/g\" ./HUAWEI-SERVER-iBMC-MIB.mib")
                self.common.run_command("sed -i \"s/ibmc/bmc/g\" ./HUAWEI-SERVER-iBMC-MIB.mib")
                # 需要对hwOem做特殊处理
                self.common.run_command("sed -i \"s/hwOEM/hwCustomized/g\" ./HUAWEI-SERVER-iBMC-MIB.mib")
                self.common.run_command("sed -i \"s/Huawei OEM event/Oem Customized event/g\" ./HUAWEI-SERVER-iBMC-MIB.mib")
                self.common.run_command("sed -i \"s/hw/oem/g\" ./HUAWEI-SERVER-iBMC-MIB.mib")
                shutil.move("HUAWEI-SERVER-iBMC-MIB.mib", "OEM-SERVER-BMC-MIB.mib")
                self.common.run_command(f"zip -r \"{target_pkg}.zip\" OEM-SERVER-BMC-MIB.mib")
                self.common.check_path(f"{self.common.work_out}/packet/{cb_archive_dic['cb_flag_tosupport_mib_dir']}")
                if os.path.exists(f"{self.common.work_out}/packet/{cb_archive_dic['cb_flag_tosupport_mib_dir']}/{target_pkg}.zip"):
                    os.remove(f"{self.common.work_out}/packet/{cb_archive_dic['cb_flag_tosupport_mib_dir']}/{target_pkg}.zip")
                shutil.move(f"{target_pkg}.zip", f"{self.common.work_out}/packet/{cb_archive_dic['cb_flag_tosupport_mib_dir']}")
                self.common.remove_path(f"{self.config.build_path}/{target_pkg}")

    def run(self):

        # 没有配置 TOGDP_PATH_LIQUID_COOLING ，不需要做打包
        if self.archive_conf_dic.get("tosupport_mib_path", "") == "":
            return

        self.logger.info(f"create compressed packet for {self.config.board_name} of  MIB TOSUPPORT PKG ------------> [begin]")
        # 创建临时目录
        os.chdir(self.config.build_path)
        self.common.remove_path(self.release_dir)
        self.common.check_path(self.release_dir)
        
        os.chdir(self.release_dir)
        # 更新version.ini文件；
        ar_ver_file = f"{self.archive_emmc_nand_dir}/{self.sw_code}_version.ini"
        if not os.path.exists(ar_ver_file):
            self.logger.error("can not find version profile")
            self.common.remove_path(f"{self.config.build_path}/{self.release_dir}")
            return
        # 提取xml中的版本号
        ver_num = self.common.extract_ver_num(ar_ver_file, self.config.board_name, True)
        os.chdir(self.config.build_path)
        # 拷贝文件
        if self.version == "V5":
            self.common.copy(f"{self.resource_dir}/config/Common/HUAWEI-SERVER-iBMC-MIB.mib",
                          f"{self.release_dir}/HUAWEI-SERVER-iBMC-MIB.mib")
            if self.config.board_name == "dh120v5":
                target_pkg = f"{self.archive_conf_dic['tosupport_mib_name']}_{ver_num}_MIB"
            else:
                target_pkg = f"{self.archive_conf_dic['tosupport_mib_name']}{ver_num}"
        if self.version == "V6":
            self.common.copy(f"{self.resource_dir}/config/Common/HUAWEI-SERVER-iBMC-MIB-V6.mib",
                          f"{self.release_dir}/HUAWEI-SERVER-iBMC-MIB.mib")
            if self.config.board_name == "TaiShan2480v2Pro" or self.config.board_name == "TaiShan2280v2Pro":
                target_pkg = f"{self.archive_conf_dic['tosupport_mib_name']}_{ver_num}_MIB"
            else:
                target_pkg = f"{self.archive_conf_dic['tosupport_mib_name']}_{ver_num}_MIB{self.archive_conf_dic['tosupport_supply_name']}"
        os.chdir(f"{self.release_dir}")
        self.run_command(f'zip -rq "{target_pkg}".zip HUAWEI-SERVER-iBMC-MIB.mib')
        special_board_tuple = ("TaiShan2280Mv2", "TaiShan2480v2", "TaiShan2480v2Pro", "TaiShan2280v2Pro", "TaiShan2280Ev2", "TaiShan2280v2", "TaiShan5290v2","TaiShan2280v2_1711", "BM320", "BM320_V3")
        if self.config.board_name in special_board_tuple:
            if self.version == "V5":
                s920_target_pkg = f"{self.archive_conf_dic['tosupport_mib_name_s920']}{ver_num}"
            if self.version  == "V6":
                if self.archive_conf_dic.get("tosupport_supply_S920_name", "")!= "":
                    s920_target_pkg = f"{self.archive_conf_dic['tosupport_mib_name_s920']}_{ver_num}_MIB{self.archive_conf_dic['tosupport_supply_S920_name']}"
                elif self.config.board_name == "BM320":
                    s920_target_pkg = f"{self.archive_conf_dic['tosupport_mib_name_s920']}_{ver_num}_MIB"
                else:
                    s920_target_pkg = f"{self.archive_conf_dic['tosupport_mib_name_s920']}_{ver_num}_MIB{self.archive_conf_dic['tosupport_supply_name']}"
            self.run_command(f'zip -rq "{s920_target_pkg}".zip HUAWEI-SERVER-iBMC-MIB.mib')
            self.common.check_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_mib_path']}")
            if os.path.exists(f"{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_mib_path']}/{s920_target_pkg}.zip"):
                os.remove(f"{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_mib_path']}/{s920_target_pkg}.zip")
            shutil.move(f"{self.config.build_path}/{self.release_dir}/{s920_target_pkg}.zip",
                    f"{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_mib_path']}")

        if self.config.board_name == "TaiShan2280Mv2":
            os.chdir(f"{self.config.build_path}/{self.release_dir}")
            target_pkg_pro = f"{self.archive_conf_dic['tosupport_mib_name_pro']}_{ver_num}_MIB"
            self.run_command(f'zip -rq "{target_pkg_pro}".zip HUAWEI-SERVER-iBMC-MIB.mib')
            if self.archive_conf_dic.get("tosupport_pro_name_s920", "") != "":
                target_pkg_S920 = f"{self.archive_conf_dic['tosupport_pro_mib_name_s920']}_{ver_num}_MIB_Pro"
                self.run_command(f'zip -rq "{target_pkg_S920}".zip HUAWEI-SERVER-iBMC-MIB.mib')
                if os.path.exists(f"{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_mib_path']}/{target_pkg_S920}.zip"):
                    os.remove(f"{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_mib_path']}/{target_pkg_S920}.zip")
                shutil.move(f"{self.config.build_path}/{self.release_dir}/{target_pkg_S920}.zip",
                    f"{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_mib_path']}")
            self.common.check_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_mib_path']}")
            if os.path.exists(f"{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_mib_path']}/{target_pkg_pro}.zip"):
                os.remove(f"{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_mib_path']}/{target_pkg_pro}.zip")
            shutil.move(f"{self.config.build_path}/{self.release_dir}/{target_pkg_pro}.zip",
                f"{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_mib_path']}")

        if self.config.board_name == "Atlas500_3000":
            os.chdir(f"{self.config.build_path}/{self.release_dir}")
            target_pkg_pro = f"{self.archive_conf_dic['tosupport_mib_name_k']}_{ver_num}_MIB"
            self.run_command(f'zip -rq "{target_pkg_pro}".zip HUAWEI-SERVER-iBMC-MIB.mib')
            self.common.check_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_mib_path']}")
            if os.path.exists(f"{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_mib_path']}/{target_pkg_pro}.zip"):
                os.remove(f"{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_mib_path']}/{target_pkg_pro}.zip")
            shutil.move(f"{self.config.build_path}/{self.release_dir}/{target_pkg_pro}.zip",
                f"{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_mib_path']}")

        self.common.check_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_mib_path']}")
        if os.path.exists(f"{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_mib_path']}/{target_pkg}.zip"):
                os.remove(f"{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_mib_path']}/{target_pkg}.zip")
        self.logger.info(f"move file to  '{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_mib_path']}'")
        shutil.move(f"{self.config.build_path}/{self.release_dir}/{target_pkg}.zip",
                    f"{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_mib_path']}")

        self.common.remove_path(f"{self.config.build_path}/{self.release_dir}")
        if self.config.cb_flag:
            self.create_cb_mib_pkg()

        self.logger.info(f"create compressed packet for {self.config.board_name} of MIB TOSUPPORT PKG ------------> [end]")


if __name__ == "__main__":
    cfg = Config("1288hv6_2288hv6_5288v6", "target_personal")
    wk = Mib(cfg)
    wk.run()