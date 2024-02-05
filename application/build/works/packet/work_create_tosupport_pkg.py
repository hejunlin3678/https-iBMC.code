#!/usr/bin/env python
# coding: utf-8
"""
功 能：work_create_tosupport_pkg.py脚本
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

class Tosupport(Work):
    def __init__(self, config, work_name=""):
        super(Tosupport, self).__init__(config)
        self.config = config
        self.archive_conf_dic = self.common.get_archive_config(self.config.board_name)
        self.release_dir = f"{self.config.board_name}_tosupport_release"
        self.resource_dir = f"{self.common.code_root}/application/src/resource"
        self.archive_emmc_nand_dir = f"{self.resource_dir}/board/{self.config.board_name}/archive_emmc_nand"
        self.sw_code = self.archive_conf_dic['togdp_dir'].split('/')[-1]
        self.version = self.common.get_board_version(self.config.board_name)
        self.platform_file_dic = {
            "V5": f"{self.resource_dir}/pme_profile/platform.xml",
            "V6": f"{self.resource_dir}/pme_profile/platform_v6.xml"
        }

    def s920_function(self, ver_num):
        # 拷贝S920XXX
        os.chdir(f"{self.config.build_path}/{self.release_dir}")
        special_board_tuple = ("TaiShan2280Mv2", "TaiShan2480v2", "TaiShan2480v2Pro", "TaiShan2280v2Pro", "TaiShan2280Ev2", "TaiShan2280v2", "TaiShan5290v2", "TaiShan2280v2_1711", "BM320", "BM320_V3")
        if self.config.board_name in special_board_tuple:
        # 制作ToSupport目录下的文件
            self.logger.info(f"create compressed packet for {self.config.board_name} of  s920 ToSupport ------------> [begin]")
            self.common.remove_file("version.xml")
            self.common.copy(f"{self.resource_dir}/board/{self.config.board_name}/version_s920.xml", "version_s920.xml")
            # 更新version.xml文件中的版本信息和产品信息，升级包名称
            self.common.copy("version_s920.xml", "tmp_version.xml")
            # xml 中 Version字段需要带点号替换
            xml_ver_num = self.common.ver_num_assemb(self.platform_file_dic.get(self.version))
            self.common.py_sed("tmp_version.xml", "TMP_VERSION", str(xml_ver_num))
            self.common.py_sed("tmp_version.xml", "TMP_SUPPORTMODEL", self.archive_conf_dic['tosupport_model_name_s920'])

            # tosupport_hpm_name_s920不为空，则用TOSUPPORT_NAME_HPM+版本替换image.hpm
            if self.archive_conf_dic.get("tosupport_hpm_name_s920", "")!= "":
                if self.version == "V5":
                    hpm_file_name = f"{self.archive_conf_dic['tosupport_hpm_name_s920']}{ver_num}.hpm"
                if self.version == "V6":
                    if self.archive_conf_dic.get("tosupport_supply_S920_name", "")!= "":
                        hpm_file_name = f"{self.archive_conf_dic['tosupport_hpm_name_s920']}_{ver_num}{self.archive_conf_dic['tosupport_supply_S920_name']}.hpm"
                    elif self.config.board_name == "BM320":
                        hpm_file_name = f"{self.archive_conf_dic['tosupport_hpm_name_s920']}_{ver_num}.hpm"
                    else:
                        hpm_file_name = f"{self.archive_conf_dic['tosupport_hpm_name_s920']}_{ver_num}{self.archive_conf_dic['tosupport_supply_name']}.hpm"
                shutil.move("image.hpm", hpm_file_name)
                self.run_command(f'sed -i "s/image.hpm/{hpm_file_name}/g" tmp_version.xml')
            else:
                hpm_file_name = "image.hpm"

            special_board_tuple = ("TaiShan2480v2", "TaiShan2480v2Pro", "TaiShan2280v2Pro", "TaiShan2280v2_1711", "BM320_V3")
            if self.config.board_name in special_board_tuple:
                self.run_command(f'sed -i "s/iBMC_Firmware/{self.archive_conf_dic["tosupport_hpm_name_s920"]}_{ver_num}/g" tmp_version.xml')
            elif self.config.board_name == "BM320":
                self.run_command(f'sed -i "s/iBMC_Firmware/{self.archive_conf_dic["tosupport_hpm_name_s920"]}_{ver_num}/g" tmp_version.xml')
            else:
                self.run_command(f'sed -i "s/iBMC_Firmware/iBMC_Firmware_{ver_num}/g" tmp_version.xml')
            hpm_size = os.path.getsize(hpm_file_name)
            self.common.run_command(f'sed -i "s/TMP_SIZE/{hpm_size}/g" tmp_version.xml')
            shutil.move("tmp_version.xml", "version.xml")
            # 生成ToSupport目录下的zip包
            if self.version == "V5":
                target_pkg = f"{self.archive_conf_dic['tosupport_name_s920']}{ver_num}"
            if self.version == "V6":
                if self.archive_conf_dic.get("tosupport_supply_S920_name", "")!= "":
                    target_pkg = f"{self.archive_conf_dic['tosupport_name_s920']}_{ver_num}{self.archive_conf_dic['tosupport_supply_S920_name']}"
                elif self.config.board_name == "BM320":
                    target_pkg = f"{self.archive_conf_dic['tosupport_name_s920']}_{ver_num}"
                else:
                    target_pkg = f"{self.archive_conf_dic['tosupport_name_s920']}_{ver_num}{self.archive_conf_dic['tosupport_supply_name']}"
            self.run_command(f'zip -rq "{target_pkg}".zip {hpm_file_name} version.xml "Open Source Software Notice.doc"')
            self.common.check_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_dir']}")
            if os.path.exists(f"{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_dir']}/{target_pkg}.zip"):
                os.remove(f"{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_dir']}/{target_pkg}.zip")
            self.logger.info(f"move file to {target_pkg}.zip '{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_dir']}'")
            shutil.move(f"{self.config.build_path}/{self.release_dir}/{target_pkg}.zip",
                f"{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_dir']}")
            self.logger.info(f"create compressed packet for {self.config.board_name} of  s920 ToSupport ------------> [end]")
        return

    def create_cb_support_pkg(self, tosupport_dir):
        cb_archives = self.common.py_find_dir(f".*_cb_flag_archive.ini", f"{self.resource_dir}/board/{self.config.board_name}")
        for cb_archive in cb_archives:
            cb_archive_dic = self.common.get_cb_archive_config(cb_archive)
            release_dir = f"{self.config.board_name}_tosupport_release_{cb_archive_dic['cb_flag_sw_code']}"
            os.chdir(self.config.build_path)
            self.common.remove_path(release_dir)
            self.common.check_path(release_dir)
            self.common.copy(f"{self.common.work_out}/rootfs_cb_flag_{self.config.board_name}_{cb_archive_dic['cb_flag_sw_code']}.hpm", f"{release_dir}/image.hpm")
            self.common.copy(f"{self.resource_dir}/board/{self.config.board_name}/{cb_archive_dic['cb_flag_sw_code']}_cb_flag_version.xml", f"{release_dir}/version.xml")
            self.common.copy(f"{self.resource_dir}/config/Common/custombrand/Open Source Software Notice.doc", f"{release_dir}/Open Source Software Notice.doc")
            # 更新version.xml文件中的版本信息和产品信息，升级包名称
            os.chdir(release_dir)
            self.common.copy("version.xml", "tmp_version.xml")
            self.common.run_command(f"sed -i \"s/TMP_SUPPORTMODEL/{cb_archive_dic['cb_flag_support_model_name']}/g\" tmp_version.xml")
            # TOSUPPORT_NAME_HPM不为空，则用TOSUPPORT_NAME_HPM+版本替换image.hpm
            hpm_file_name = ""
            ver_num = self.common.extract_ver_num("", self.config.board_name, True)
            if cb_archive_dic['cb_flag_tosupport_name_hpm'] != "":
                hpm_file_name=f"{cb_archive_dic['cb_flag_tosupport_name_hpm']}{ver_num}.hpm"
                self.common.run_command(f"mv image.hpm \"{hpm_file_name}\"")
                self.common.run_command(f"sed -i \"s/image.hpm/{hpm_file_name}/g\" tmp_version.xml")
            else:
                hpm_file_name="image.hpm"       
            self.common.run_command(f"sed -i \"s/BMC_Firmware/{cb_archive_dic['cb_flag_tosupport_name']}_{ver_num}/g\" tmp_version.xml")     
            # 获取HPM包的大小
            if os.path.exists(hpm_file_name):
                hpm_size = os.path.getsize(hpm_file_name)
                self.common.run_command(f"sed -i \"s/TMP_SIZE/{hpm_size}/g\" tmp_version.xml")
            self.common.copy("tmp_version.xml", "version.xml")
            self.common.remove_file("tmp_version.xml")     
            # 生成ToSupport目录下的zip包
            target_pkg = f"{cb_archive_dic['cb_flag_tosupport_name']}_{ver_num}"
            os.chdir(self.config.build_path)
            self.common.remove_path(target_pkg)
            shutil.move(release_dir, target_pkg)
            os.chdir(f"{target_pkg}")
            self.common.run_command(f"zip -rq \"{target_pkg}.zip\" \"{hpm_file_name}\" version.xml 'Open Source Software Notice.doc'")
            self.common.check_path(f"{self.common.work_out}/packet/{tosupport_dir}")
            if os.path.exists(f"{self.common.work_out}/packet/{tosupport_dir}/{target_pkg}.zip"):
                os.remove(f"{self.common.work_out}/packet/{tosupport_dir}/{target_pkg}.zip")
            shutil.move(f"{self.config.build_path}/{target_pkg}/{target_pkg}.zip", f"{self.common.work_out}/packet/{tosupport_dir}")
            self.common.remove_path(f"{self.config.build_path}/{target_pkg}")
        os.chdir(self.config.build_path)
        return

    def run(self):
        if self.config.support_dft_separation and self.config.build_type != "dft_separation":
            return
        if not self.config.support_dft_separation and self.config.build_type != "release":
            return
        
        self.logger.info(f"create compressed packet for {self.config.board_name} of Tosupport PKG ------------> [begin]")
        # 创建临时目录
        os.chdir(self.config.build_path)
        self.common.remove_path(self.release_dir)
        self.common.check_path(self.release_dir)

        if self.config.support_dft_separation:
            self.common.copy(f"{self.common.work_out}/rootfs_{self.config.board_name}_release.hpm", 
            f"{self.release_dir}/image.hpm")
        else :
            self.common.copy(f"{self.common.work_out}/rootfs_{self.config.board_name}.hpm", f"{self.release_dir}/image.hpm")
        self.common.copy(f"{self.resource_dir}/board/{self.config.board_name}/version.xml", f"{self.release_dir}/version.xml")
        self.common.copy(f'{self.resource_dir}/config/Common/Open Source Software Notice.doc',
                  f"{self.release_dir}/Open Source Software Notice.doc")

        os.chdir(self.release_dir)
        # 更新version.ini文件；
        ar_ver_file = f"{self.archive_emmc_nand_dir}/{self.sw_code}_version.ini"
        if not os.path.exists(ar_ver_file):
            self.logger.error("can not find version profile")
            self.common.remove_path(f"{self.config.build_path}/{self.release_dir}")
            return
        # 提取xml中的版本号
        ver_num = self.common.extract_ver_num(ar_ver_file, self.config.board_name, readonly=True)
        self.common.copy("version.xml", "tmp_version.xml")
        # xml 中 Version字段需要带点号替换
        xml_ver_num = self.common.ver_num_assemb(self.platform_file_dic.get(self.version))
        self.common.py_sed("tmp_version.xml", "TMP_VERSION", str(xml_ver_num))
        self.common.py_sed("tmp_version.xml", "TMP_SUPPORTMODEL", self.archive_conf_dic['support_model_name'])
        # tosupport_hpm_name不为空，则用TOSUPPORT_NAME_HPM+版本替换image.hpm
        if self.archive_conf_dic.get("tosupport_hpm_name", "") != "":
            if self.version == "V5":
                hpm_file_name = f"{self.archive_conf_dic['tosupport_hpm_name']}{ver_num}.hpm"
            if self.version == "V6":
                if self.config.board_name == "TaiShan2480v2Pro" or self.config.board_name == "TaiShan2280v2Pro":
                    hpm_file_name = f"{self.archive_conf_dic['tosupport_hpm_name']}_{ver_num}.hpm"
                else:
                    hpm_file_name = f"{self.archive_conf_dic['tosupport_hpm_name']}_{ver_num}{self.archive_conf_dic['tosupport_supply_name']}.hpm"
            self.common.copy("image.hpm", hpm_file_name)
            self.common.py_sed("tmp_version.xml", "image\.hpm", hpm_file_name)
        else:
            hpm_file_name = "image.hpm"

        Fusion_pod_v5_board_tuple = ("dh120v5",)
        under_develop_board_tuple_v5 = ("xa320", "TaiShan2280v2")
        if self.version == "V5":
            if self.config.board_name in Fusion_pod_v5_board_tuple or self.config.board_name in under_develop_board_tuple_v5:
                self.common.py_sed("tmp_version.xml", "iBMC_Firmware", f"{self.archive_conf_dic['tosupport_name']}_{ver_num}")
            else:
                self.common.py_sed("tmp_version.xml", "iBMC_Firmware", f"iBMC_Firmware_{ver_num}")
        if self.version == "V6":
            if self.config.board_name == "TaiShan2480v2Pro" or self.config.board_name == "TaiShan2280v2Pro":
                self.common.py_sed("tmp_version.xml", "iBMC_Firmware", f"{self.archive_conf_dic['tosupport_hpm_name']}_{ver_num}")
            else:
                self.common.py_sed("tmp_version.xml", "iBMC_Firmware", f"{self.archive_conf_dic['tosupport_name']}_{ver_num}{self.archive_conf_dic['tosupport_supply_name']}")

        hpm_size = os.path.getsize(hpm_file_name)
        self.common.py_sed("tmp_version.xml", "TMP_SIZE", str(hpm_size))
        shutil.move("tmp_version.xml", "version.xml")

        # 更新1280Pro version.xml
        if self.config.board_name == "TaiShan2280Mv2":
            self.common.copy(f"{self.resource_dir}/board/{self.config.board_name}/version_pro.xml", "tmp_version.xml")
            hpm_file_name_pro = f"{self.archive_conf_dic['tosupport_hpm_name_pro']}_{ver_num}.hpm"
            self.run_command(f'sed -i "s/image.hpm/{hpm_file_name_pro}/g" tmp_version.xml')
            self.run_command(f'sed -i "s/TMP_VERSION/{ver_num}/g" tmp_version.xml')
            self.run_command(f'sed -i "s/TMP_SUPPORTMODEL/{self.archive_conf_dic["tosupport_model_name_pro"]}/g" tmp_version.xml')
            self.run_command(f'sed -i "s/iBMC_Firmware/{self.archive_conf_dic["tosupport_hpm_name_pro"]}_{ver_num}/g" tmp_version.xml')
            self.run_command(f'sed -i "s/TMP_SIZE/{hpm_size}/g" tmp_version.xml')
            # 1280 pro copy hpm包
            self.common.copy(hpm_file_name, hpm_file_name_pro)
            target_pkg_pro = f"{self.archive_conf_dic['tosupport_name_pro']}_{ver_num}"

            self.common.copy(f"{self.resource_dir}/board/{self.config.board_name}/version_s920X00_1U.xml", "tmp_version_s920.xml")
            hpm_file_name_pro_s920 = f"{self.archive_conf_dic['tosupport_pro_hpm_name_s920']}_{ver_num}_Pro.hpm"
            self.run_command(f'sed -i "s/image.hpm/{hpm_file_name_pro_s920}/g" tmp_version_s920.xml')
            self.run_command(f'sed -i "s/TMP_VERSION/{ver_num}/g" tmp_version_s920.xml')
            self.run_command(f'sed -i "s/TMP_SUPPORTMODEL/{self.archive_conf_dic["tosupport_pro_model_name_s920"]}/g" tmp_version_s920.xml')
            self.run_command(f'sed -i "s/iBMC_Firmware/{self.archive_conf_dic["tosupport_pro_hpm_name_s920"]}_{ver_num}_Pro/g" tmp_version_s920.xml')
            self.run_command(f'sed -i "s/TMP_SIZE/{hpm_size}/g" tmp_version_s920.xml')
            # S920X00-1U(Pro) copy hpm包
            self.common.copy(hpm_file_name, hpm_file_name_pro_s920)
            target_pkg_pro_s920 = f"{self.archive_conf_dic['tosupport_pro_name_s920']}_{ver_num}_Pro"

        # 更新A500K Pro-3000K version.xml
        if self.config.board_name == "Atlas500_3000":
            self.common.copy(f"{self.resource_dir}/board/{self.config.board_name}/version_0.xml", "tmp_version.xml")
            hpm_file_name_k = f"{self.archive_conf_dic['tosupport_hpm_name_k']}_{ver_num}.hpm"
            self.run_command(f'sed -i "s/image.hpm/{hpm_file_name_k}/g" tmp_version.xml')
            self.run_command(f'sed -i "s/TMP_VERSION/{ver_num}/g" tmp_version.xml')
            self.run_command(f'sed -i "s/TMP_SUPPORTMODEL/{self.archive_conf_dic["tosupport_model_name_k"]}/g" tmp_version.xml')
            self.run_command(f'sed -i "s/iBMC_Firmware/{self.archive_conf_dic["tosupport_hpm_name_k"]}_{ver_num}/g" tmp_version.xml')
            self.run_command(f'sed -i "s/TMP_SIZE/{hpm_size}/g" tmp_version.xml')
            # A500K Pro-3000K copy hpm包
            self.common.copy(hpm_file_name, hpm_file_name_k)
            target_pkg_pro = f"{self.archive_conf_dic['tosupport_name_k']}_{ver_num}"
        # 检查大小, V5最大为 52428800 50.0MB, V6 73400320 70MB(TaiShan2280v2、S920除外), 如果超出，删除文件，打印log
        if self.version == "V5":
            max_size = 52428800
        if self.version == "V6":
            max_size = 104857600

        special_board_tuple = ("S920", "TaiShan2280v2")
        if hpm_size > max_size and self.config.board_name not in special_board_tuple:
            self.logger.error(f"{hpm_file_name} is too big, size={hpm_size}, bigger than {max_size}, delete it")
            os.remove(hpm_file_name)
            os._exit(1)
        else:
            self.logger.info(f"now {hpm_file_name} size={hpm_size}") 


        # 生成ToSupport目录下的zip包
        if self.version == "V5":
            if self.config.board_name == "dh120v5" or self.config.board_name in under_develop_board_tuple_v5:
                target_pkg = f"{self.archive_conf_dic['tosupport_name']}_{ver_num}"
            else:
                target_pkg = f"{self.archive_conf_dic['tosupport_name']}{ver_num}"
        if self.version == "V6":
            if self.config.board_name == "TaiShan2480v2Pro" or self.config.board_name == "TaiShan2280v2Pro":
                target_pkg = f"{self.archive_conf_dic['tosupport_name']}_{ver_num}"
            else:
                target_pkg = f"{self.archive_conf_dic['tosupport_name']}_{ver_num}{self.archive_conf_dic['tosupport_supply_name']}"
            self.logger.info(f"create compressed packet for {self.config.board_name} of Tosupport PKG ------------> [end]")
        self.run_command(f'zip -rq "{target_pkg}".zip {hpm_file_name} version.xml "Open Source Software Notice.doc"')
         # 生成1280 pro zip压缩包
        if self.config.board_name == "TaiShan2280Mv2":
            shutil.move("tmp_version.xml", "version.xml")
            self.run_command(f'zip -rq "{target_pkg_pro}".zip {hpm_file_name_pro} version.xml "Open Source Software Notice.doc"')
            self.common.check_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_dir']}")
            if os.path.exists(f"{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_dir']}/{target_pkg_pro}.zip"):
                os.remove(f"{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_dir']}/{target_pkg_pro}.zip")
            self.logger.info(f"move file to {target_pkg_pro}.zip '{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_dir']}'")
            shutil.move(f"{self.config.build_path}/{self.release_dir}/{target_pkg_pro}.zip",
                f"{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_dir']}")
            # 生成S920X00-1U(Pro) zip压缩包
            shutil.move("tmp_version_s920.xml", "version.xml")
            self.run_command(f'zip -rq "{target_pkg_pro_s920}".zip "{hpm_file_name_pro_s920}" version.xml "Open Source Software Notice.doc"')
            self.common.check_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_dir']}")
            if os.path.exists(f"{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_dir']}/{target_pkg_pro_s920}.zip"):
                os.remove(f"{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_dir']}/{target_pkg_pro_s920}.zip")
            self.logger.info(f"move file to {target_pkg_pro_s920}.zip '{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_dir']}'")
            shutil.move(f"{self.config.build_path}/{self.release_dir}/{target_pkg_pro_s920}.zip",
                f"{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_dir']}")

        # 生成A500K Pro-3000K zip压缩包
        if self.config.board_name == "Atlas500_3000":
            shutil.move("tmp_version.xml", "version.xml")
            self.run_command(f'zip -rq "{target_pkg_pro}".zip {hpm_file_name_k} version.xml "Open Source Software Notice.doc"')
            self.common.check_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_dir']}")
            if os.path.exists(f"{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_dir']}/{target_pkg_pro}.zip"):
                os.remove(f"{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_dir']}/{target_pkg_pro}.zip")
            self.logger.info(f"move file to {target_pkg_pro}.zip '{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_dir']}'")
            shutil.move(f"{self.config.build_path}/{self.release_dir}/{target_pkg_pro}.zip",
                f"{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_dir']}")

        self.s920_function(ver_num)
        self.common.check_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_dir']}")
        if os.path.exists(f"{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_dir']}/{target_pkg}.zip"):
                os.remove(f"{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_dir']}/{target_pkg}.zip")
        self.logger.info(f"move file to {target_pkg}.zip '{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_dir']}'")
        shutil.move(f"{self.config.build_path}/{self.release_dir}/{target_pkg}.zip",
                f"{self.common.work_out}/packet/{self.archive_conf_dic['tosupport_dir']}")
        self.common.remove_path(f"{self.config.build_path}/{self.release_dir}")
        if self.config.cb_flag:
            self.create_cb_support_pkg(self.archive_conf_dic['tosupport_dir'])
        self.logger.info(f"create compressed packet for {self.config.board_name} of ToSupport ------------> [end]")

if __name__ == "__main__":
    cfg = Config("1288hv6_2288hv6_5288v6", "target_personal")
    cfg.set_build_type("release")
    wk = Tosupport(cfg)
    wk.run()