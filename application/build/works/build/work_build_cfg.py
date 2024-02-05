#!/usr/bin/env python
# coding: utf-8

"""
功 能：build_cfg脚本，该脚本处理 opt/pme/extern下配置文件
版权信息：华为技术有限公司，版本所有(C) 2019-2020
"""

import shutil
import subprocess
import sys
import os
import re
import json

cwd=os.path.dirname(__file__)
sys.path.append(f"{cwd}/../")
sys.path.append(f"{cwd}/../../")
from works.work import Work
from utils.common import Common
from utils.config import Config
from works.build.redfish_event_registry_generator import Redfish_Event_Generator
from works.build.differentiated_alarm import Differentiated_Alarm

from utils.error_info import EXCEPTION_STAGE
from utils.error_info import EXCEPTION_RESPONSE
from utils.error_info import EXCEPTION_CODE

class WorkBuildCfg(Work):

    def __init__(self, config, work_name=""):
        super(WorkBuildCfg, self).__init__(config, work_name)
        self.pdt_dir = os.path.join(self.common.code_root, f"application/src/resource/board/{self.config.board_name}")
        self.version = self.common.get_board_version(self.config.board_name)
        self.temp_data_dir = os.path.join(self.common.temp_path, f"cfgfs_{self.config.board_name}")
        self.mnt_datafs = f"{self.config.build_path}/mnt_datafs"
        self.common.check_path(self.mnt_datafs)
        self.cml_flag = False
        if self.config.board_name == "2488hv6" or self.config.board_name == 'TaiShan2480v2' or self.config.board_name == 'TaiShan2280v2_1711' or self.config.board_name == 'BM320' or self.config.board_name == 'TaiShan2280Mv2':
            self.cml_flag = True
        self.temp_cml_dir = os.path.join(self.common.temp_path, f"cml_{self.config.board_name}")
        self.extern_name_list = [".json"]
    
    def remove_file_blank_char(self, filepath):
        with open(filepath, mode='rb') as fp:
            data = json.load(fp)

        content = json.dumps(data, separators=(',', ':'))
        with open(filepath, mode='wb') as fd:
            fd.write(content.encode('utf-8'))
 
    def remove_dir_blank_char(self, dirpath):
        filename_list = os.listdir(dirpath)
        for filename in filename_list:
            filepath = dirpath + "/" + filename
            if os.path.isdir(filepath):
                self.remove_dir_blank_char(filepath)
            else:
                for extern_name in self.extern_name_list:
                    if filepath[-len(extern_name):] == extern_name:
                        self.remove_file_blank_char(filepath)
                        break

    def get_redfish_conf(self):
        redfish_generator = Redfish_Event_Generator(self.config)
        self.common.remove_path(f"{self.common.temp_path}/redfish")
        self.common.copy_all(f"{self.common.code_root}/application/src/resource/config/redfish",
                        f"{self.common.temp_path}/redfish")
        if self.version == "V5":
            self.common.remove_path(f"{self.common.temp_path}/redfish/v1/managers/1/securityservice/dicecert")

        self.common.delete_for_matched(f"{self.common.temp_path}/redfish/v1/registrystore/messages/en/", "ibmcevents")
        arch_tuple = ("xa320", "TaiShan2280v2", "TaiShan2280_5280", "xr320", "TaiShan1280v2", "TaiShan5290v2", "TaiShan2480v2", "TaiShan2480v2Pro", "DA121Cv2", "DA122Cv2", "TaiShan2280Ev2", "BM320", "BM320_V3", "S920XA0", "TaiShan2280v2_1711", "S920X05", "S920S03", "S920", "TaiShan2280Mv2", "Atlas200I_SoC_A1", "CST1220V6", "S902X20", "Atlas800D_G1", "Atlas800D_RM")
        if self.config.board_name in arch_tuple:
            df = Differentiated_Alarm(self.config)
            df.generate_event_registry_record(f"{self.common.code_path}/../rootfs/opt/pme/conf/language/event_lang_arm_template_v2.xml")
        try:
            redfish_generator.generate_event_registry_record()
        except Exception as e:
            raise Exception(f"generate_event_registry_record faile {e}")

        if self.common.need_update_pangea_alarm_suggestions(self.config.board_name):
            df = Differentiated_Alarm(self.config)
            df.generate_event_registry_record(f"{self.common.code_path}/../src/resource/profile/pangea_01/event_lang_pangea_template_v2.xml")
        try:
            redfish_generator.generate_event_registry_record()
        except Exception as e:
            raise Exception(f"generate_event_registry_record faile {e}")

         # 天池架构告警处理建议替换
        if self.config.board_name == "BM320":
            df = Differentiated_Alarm(self.config)
            df.generate_event_registry_record(f"{self.common.code_path}/../rootfs/opt/pme/conf/language/event_lang_tianchi_template_v2.xml")
        try:
            redfish_generator.generate_event_registry_record()
        except Exception as e:
            raise Exception(f"generate_event_registry_record faile {e}")

        self.common.run_command(f"find {self.common.temp_path}/redfish -name '.svn' -type d | xargs rm -rf >/dev/null 2>&1")
        self.common.run_command(f"find {self.common.temp_path}/redfish -name '.gitkeep' | xargs rm -rf >/dev/null 2>&1")

        self.remove_dir_blank_char(f"{self.common.temp_path}/redfish")
        self.customization.redfish_conf(self.common.temp_path, self.temp_data_dir)

    def pre_cfgs(self,temp_data_dir, pdt_dir):
        # 针对bmc_card类型产品，先切换到任意目录，防止打包完之后停留在当前目录后又执行删除操作，导致后续扣卡联机帮助打包失败
        os.chdir(self.common.code_path)
        web_online_ant_file = os.path.join(pdt_dir, "web_online_help.xml")
        self.common.remove_path(temp_data_dir)
        self.common.check_path(temp_data_dir)

        if os.path.exists(web_online_ant_file):
            self.common.run_command(f"ant -f {web_online_ant_file} -Ddest_dir={temp_data_dir}/cfgfs/web/online_help")

        self.common.copy_all(f"{pdt_dir}/cfgfs", f"{temp_data_dir}/cfgfs")
        profile = f"{pdt_dir}/profile.txt"
        if os.path.exists(profile):
            with open(rf'{profile}') as somefile:
                lines = somefile.readlines()
                for line in lines:
                    result = line.strip('\n')
                    if result != "":
                        real_file = os.path.split(result)[-1]
                        self.common.copy(f"{self.common.code_root}/application/src/resource/profile/{result}",
                                    f"{temp_data_dir}/cfgfs/profile/{real_file}")
        else:
            err_msg = f"{EXCEPTION_CODE[600]}{EXCEPTION_STAGE['P']}{EXCEPTION_RESPONSE[2]}"
            self.logger.error(f"{err_msg}copy profile({profile}) fail")
            os._exit(1)

        # xml格式化
        self.common.run_py_command(f'{self.common.code_path}/xml_format.py',
                                   "xml_format.py covert_keywords ERR!",
                                   f'{temp_data_dir}/cfgfs/profile covert_keywords')

        os.symlink("/opt/pme/conf/profile/base.xml", f"{temp_data_dir}/cfgfs/profile/base.xml")
        os.symlink("/opt/pme/conf/profile/class.xml", f"{temp_data_dir}/cfgfs/profile/class.xml")
        os.symlink("/opt/pme/conf/profile/platform.xml", f"{temp_data_dir}/cfgfs/profile/platform.xml")
        os.symlink("/data/opt/pme/conf/oem/profile/CustomizeSensor.xml",
                f"{temp_data_dir}/cfgfs/profile/CustomizeSensor_ff.xml")
        try:
            os.symlink("/data/opt/pme/conf/oem/profile/PSUCustomizeSensor.xml",
                f"{temp_data_dir}/cfgfs/profile/PSUCustomizeSensor_ff.xml")
        except:
            self.logger.info(f"This board dose not support this soft link \"/data/opt/pme/conf/oem/profile/PSUCustomizeSensor.xml\"")

        if self.version == "V5" and self.config.cb_flag:
            cb_build_tmp = f"{self.common.temp_path}/cb_cfgs_img/profile"
            self.common.check_path(cb_build_tmp)
            self.common.copy_all(f"{temp_data_dir}/cfgfs/profile", cb_build_tmp)
            self.create_cb_xml(cb_build_tmp)

        self.common.run_command(f"find {temp_data_dir} -name '.svn' -type d | xargs rm -rf >/dev/null 2>&1")
        self.common.run_command(f"find {temp_data_dir} -name '.gitkeep' | xargs rm -rf >/dev/null 2>&1")


    def build_bmc_card_cfgfs(self, temp_data_dir, board):
        build_tmp = f"{self.temp_data_dir}/cfgs_img/{board}"
        self.common.copy_all(os.path.join(temp_data_dir, "cfgfs"), f"{build_tmp}/cfgfs")

        # 将profile文件制作成img文件；profile文件的大小加5M之后，作为img文件的大小。
        profile_capacity = subprocess.run(f"du -sh {build_tmp}/cfgfs/profile/ -k ",
                                        shell=True,capture_output=True).stdout.decode().strip('\n').split('\t')[0]

        profile_capacity = int((int(profile_capacity) + 11000) * 1000 / 4096)

        profile_img_path = f"{build_tmp}/profile.img"
        self.common.create_image_and_mount_datafs(profile_capacity, profile_img_path, self.mnt_datafs)
        self.customization.profile_img_conf(build_tmp)
        self.run_command(f"cp -dfr {build_tmp}/cfgfs/profile/* {self.mnt_datafs}/", super_user=True)
        self.run_command(f"chown 0:0 {self.mnt_datafs}", super_user=True)
        self.common.umount_datafs(self.mnt_datafs)

        if self.config.cb_flag:
            cb_build_tmp = f"{self.common.temp_path}/cb_cfgs_img/{board}"
            self.common.check_path(cb_build_tmp)
            self.create_cb_xml(f"{build_tmp}/cfgfs/profile")
            profile_capacity = subprocess.run(f"du -sh {build_tmp}/cfgfs/profile/ -k ",
                                        shell=True,capture_output=True).stdout.decode().strip('\n').split('\t')[0]

            profile_capacity = int((int(profile_capacity) + 8000) * 1000 / 4096)

            cb_profile_img_path = f"{cb_build_tmp}/profile.img"
            self.common.create_image_and_mount_datafs(profile_capacity, cb_profile_img_path, self.mnt_datafs)
            self.customization.profile_img_conf(build_tmp)
            self.run_command(f"cp -dfr {build_tmp}/cfgfs/profile/* {self.mnt_datafs}/", super_user=True)
            self.run_command(f"chown 0:0 {self.mnt_datafs}", super_user=True)
            self.common.umount_datafs(self.mnt_datafs)

        # 将web目录制作成img文件；文件的大小加3M之后，作为img文件的大小
        web_capacity = subprocess.run(f"du -sh {build_tmp}/cfgfs/web/ -k ",
                                        shell=True, capture_output=True).stdout.decode().strip('\n').split('\t')[0]

        web_capacity = int((int(web_capacity) + 3000) * 1000 / 4096)
        web_img_path = f"{build_tmp}/web.img"
        self.common.create_image_and_mount_datafs(web_capacity, web_img_path, self.mnt_datafs)
        self.customization.web_img_conf(build_tmp)
        # 超级用户复制，属主全部变更为root
        self.run_command(f"cp -dfr {build_tmp}/cfgfs/web/* {self.mnt_datafs}/", super_user=True)
        self.common.umount_datafs(self.mnt_datafs)

        board_id = ""
        os.chdir(f"{build_tmp}/cfgfs/profile")
        profiles = os.listdir(f"{build_tmp}/cfgfs/profile")
        profiles.sort()
        for f in profiles:
            if f[0] == "_":
                board_id = f"{board_id}{f.split('.')[0]}"

        self.common.remove_path(f"{build_tmp}/cfgfs/profile")
        self.common.remove_path(f"{build_tmp}/cfgfs/web")

        try:
            shutil.move(f"{build_tmp}/cfgfs/app.list", build_tmp)
            shutil.move(f"{build_tmp}/cfgfs/insmod_driver.sh", build_tmp)
        except:
            self.logger.info("file is exist")
        os.chdir(build_tmp)
        profiles = os.listdir(build_tmp)
        for f in profiles:
            if f == "cfgfs":
                continue
            if os.path.isfile(f"{board}_tmp.tar.gz.sha"):
                subprocess.run(f"chmod a+rw {board}_tmp.tar.gz.sha", shell=True)
            subprocess.run(f"sha256sum {f} >> {board}_tmp.tar.gz.sha", shell=True)
            subprocess.run(f"chmod o-rw {board}_tmp.tar.gz.sha", shell=True)
        subprocess.run("chmod o+rwx .", shell=True)
        cmd = "awk -F '/| ' '{print $NF \":\" $1}'  %s_tmp.tar.gz.sha > %s.tar.gz.sha256"%(board, board_id)
        subprocess.run(cmd, shell=True)
        subprocess.run(f"chmod o-rwx {board_id}.tar.gz.sha256", shell=True)
        subprocess.run(f"chmod o-rwx .", shell=True)
        os.remove(f"{board}_tmp.tar.gz.sha")

        # 白牌包配置赋权
        if self.config.cb_flag:
            shutil.copy(web_img_path, cb_build_tmp)
            shutil.copy(f"{board_id}.tar.gz.sha256", cb_build_tmp)

            os.chdir(cb_build_tmp)
            self.customization.profile_web_img_packet(cb_build_tmp, board_id)
            shutil.move(f"{board_id}.tar.gz", f"{self.common.temp_path}/cb_cfgs_img")
            shutil.move(f"{board_id}.tar.gz.sha256", f"{self.common.temp_path}/cb_cfgs_img")
            subprocess.run(f"chmod 640 {self.common.temp_path}/cb_cfgs_img/{board_id}.tar.gz", shell=True)
            self.common.remove_path(cb_build_tmp)

        # 正常包配置赋权
        os.chdir(build_tmp)
        self.customization.profile_web_img_packet(build_tmp, board_id)
        shutil.move(f"{board_id}.tar.gz", self.temp_data_dir)
        self.common.copy(f"{build_tmp}/app.list", f"{self.temp_data_dir}/app.list")
        self.common.copy(f"{build_tmp}/insmod_driver.sh", f"{self.temp_data_dir}/insmod_driver.sh")
        shutil.move(f"{build_tmp}/{board_id}.tar.gz.sha256", self.temp_data_dir)
        subprocess.run(f"chmod 640 {self.temp_data_dir}/{board_id}.tar.gz", shell=True)
        self.logger.info("build_bmc_card_cfgfs end")


    def build_cfg_fs(self):
        self.logger.info(f"Make {self.config.board_name} cfgfs ...")
        if self.cml_flag:
            self.build_cml()
        if self.version == "V6":
            if self.config.board_name == "bmc_card" or self.config.board_name == "1288hv6_2288hv6_5288v6" or self.config.board_name == "DP1210_DP2210_DP4210":
                files = os.listdir(f"{self.pdt_dir}")
                for f in files:
                    if os.path.isdir(f"{self.pdt_dir}/{f}") and f != "archive_emmc_nand":
                        self.logger.info(f"{f}")
                        temp_data_dir = os.path.join(self.common.temp_path, f"{self.config.board_name}/cfgfs_{f}")
                        pdt_dir = os.path.join(self.common.code_root, f"application/src/resource/board/{self.config.board_name}/{f}")
                        self.pre_cfgs(temp_data_dir, pdt_dir)
                        self.build_bmc_card_cfgfs(temp_data_dir, f)
            else:
                self.pre_cfgs(self.temp_data_dir, self.pdt_dir)
                self.build_bmc_card_cfgfs(self.temp_data_dir, self.config.board_name)

        if self.version == "V5":
            self.pre_cfgs(self.temp_data_dir, self.pdt_dir)
            self.run_command(f"chmod 440 -R {self.temp_data_dir}/cfgfs/web", super_user=True)
            self.run_command(f"find {self.temp_data_dir}/cfgfs/web -type d -exec chmod 550 {{}} \;", super_user=True)
            self.run_command(f"chmod 755 {self.temp_data_dir}/cfgfs", super_user=True)

        self.get_redfish_conf()

    def cml_converter(self, fbs_file_name, model_type, json_file_name):
        shutil.copytree(f"{self.common.code_root}/application/src/apps/ai_mgmt/src/model/{model_type}", f"{self.temp_cml_dir}/models/{model_type}")
        json_file_path = os.path.join(f"{self.temp_cml_dir}/models", model_type, json_file_name)
        converter_file_name = os.path.splitext(json_file_name)[0]
        converter_file_path = os.path.join(f"{self.temp_cml_dir}/models", model_type)
        subprocess.run(f"flatc -o {converter_file_path} -b {fbs_file_name} {json_file_path}", shell=True)

        # bin文件转换为cml
        shutil.move(os.path.join(converter_file_path, converter_file_name + ".bin"), os.path.join(converter_file_path, converter_file_name + ".cml"))
        # copy model
        dest_model = model_type + "_1610679782"
        os.rename(f"{self.temp_cml_dir}/models/{model_type}", f"{self.temp_cml_dir}/models/{dest_model}")

        self.common.remove_file(f"{self.temp_cml_dir}/models/{dest_model}/{json_file_name}")
        return

    def build_cml(self):
        fbs_file_path = f"{self.common.bin_cmc_path}/application/src_bin/platforms/site_ai/CML.fbs"
        self.common.remove_path(f"{self.temp_cml_dir}/models")
        self.common.check_path(f"{self.temp_cml_dir}/models")
        self.cml_converter(fbs_file_path, "sas", "model_sas.json")
        self.cml_converter(fbs_file_path, "sas_hgst", "model_sas_hgst.json")
        self.cml_converter(fbs_file_path, "sas_hitachi", "model_sas_hitachi.json")
        self.cml_converter(fbs_file_path, "sas_hitachi_v20211215", "model_sas_hitachi_v20211215.json")
        self.cml_converter(fbs_file_path, "sas_seagate", "model_sas_seagate.json")
        self.cml_converter(fbs_file_path, "sas_seagate_v20220326", "model_sas_seagate_v20220326.json")
        self.cml_converter(fbs_file_path, "hitachi", "model_hitachi.json")
        self.cml_converter(fbs_file_path, "seagate", "model_seagate.json")
        self.cml_converter(fbs_file_path, "ssd", "model_ssd.json")
        self.cml_converter(fbs_file_path, "pcie", "model_pcie.json")
        self.cml_converter(fbs_file_path, "uce", "model_dimm_uce.json")
    
    def replace_xml_via_CBList(self, xml_file, id_cfg):
        id_cfg_lines = self.common.get_shell_command_result(f"cat '{id_cfg}' | grep -v '^#'").strip('\n').split('\n')
        for id_cfg_line in id_cfg_lines:
            cb_flag = self.common.get_shell_command_result(f"echo {id_cfg_line} | cut -d ':' -f 1")
            old_cb_val = self.common.get_shell_command_result(f"echo {id_cfg_line} | cut -d ':' -f 2")
            new_cb_val = self.common.get_shell_command_result(f"echo {id_cfg_line} | cut -d ':' -f 3")
            if cb_flag == "" or old_cb_val == "" or new_cb_val == "":
                self.logger.info(f"echo 'invalid {id_cfg} for line {id_cfg_line}'")
                continue
            target_line = ""
            if cb_flag == "CBProductUniqueID_Exp":
                target_line = self.common.get_shell_command_result(f"grep -wn -A 6 CBProductUniqueID_Exp.*Expression {xml_file} | grep -w Formula | cut -d '-' -f 1")
            if cb_flag == "CBProductID":
                target_line = self.common.get_shell_command_result(f"grep -wn CBProductID {xml_file} | grep -vw rp | cut -d ':' -f 1")
            if cb_flag == "CBProductUniqueID":
                target_line = self.common.get_shell_command_result(f"grep -wn CBProductUniqueID {xml_file} | grep -vw rp | cut -d ':' -f 1")
            if cb_flag == "IsCustomBrand":
                target_line = self.common.get_shell_command_result(f"grep -wn IsCustomBrand {xml_file} | grep -vw rp | cut -d ':' -f 1")
            self.logger.info(f"target_line:{target_line}")
            if target_line != "":
                self.run_command(f"sed -i \"{target_line}s/{old_cb_val}/{new_cb_val}/\" \"{xml_file}\"")

    def create_cb_xml(self, profile):
        cb_productid_list = self.common.py_find_dir(".*_cb_flag_productid.list", self.pdt_dir)
        for cb_file in cb_productid_list:
            self.run_command(f"dos2unix '{cb_file}'")
            prod_xml_list = ""
            prod_xml_list = self.common.get_shell_command_result(f"grep -srw IsCustomBrand {profile}|grep -v class.xml|cut -d ':' -f 1|sort|uniq").split('\n')
            for prod_xml in prod_xml_list:
                if prod_xml != "":
                    self.replace_xml_via_CBList(prod_xml, f"{cb_file}")

    def run(self):
        self.build_cfg_fs()
        return


    def install(self):
        if not self.config.support_dft_separation and self.config.build_type == "dft_separation":
            return
        dst_path = f"{self.config.rootfs_path}/opt/pme/extern"
        src_path = self.temp_data_dir
        os.chdir(src_path)
        if os.path.isdir(dst_path):
            shutil.rmtree(dst_path, ignore_errors=True)
        os.makedirs(dst_path, 0o755, exist_ok=True)
        if self.version == 'V6':
            self.common.remove_path(f"{src_path}/cfgfs")
            self.common.remove_path(f"{src_path}/cfgs_img")
            self.common.check_path(f"{dst_path}/web")
            self.common.check_path(f"{dst_path}/profile")
            self.run_command(f"cp -af {src_path}/* {dst_path}", super_user=True)
        else:
            self.run_command(f"cp -af {src_path}/cfgfs/* {dst_path}", super_user=True)
        if self.cml_flag:
            self.common.check_path(os.path.join(self.config.rootfs_path, "opt/pme/conf/aimgmt/"))
            shutil.copy(os.path.join(f"{self.common.code_root}/application/src/apps/ai_mgmt/src/model/pcie_dictionary"),
                    os.path.join(self.config.rootfs_path, "opt/pme/conf/aimgmt/"))
             # 压缩
            subprocess.run(f"tar --format=gnu -czf {self.config.rootfs_path}/opt/pme/conf/aimgmt/models.tar.gz -C {self.temp_cml_dir}/ models", shell=True)
            if not os.path.exists(f"{self.config.rootfs_path}/opt/pme/lib"):
                os.makedirs(f"{self.config.rootfs_path}/opt/pme/lib")
            site_ai_path = "site_ai_v6" if self.version == "V6" else "site_ai"
            site_data_path = "site_data_v6" if self.version == "V6" else "site_data"
            self.run_command(f"cp -af {self.common.bin_cmc_path}/application/src_bin/platforms/{site_ai_path}/* {self.config.rootfs_path}/opt/pme/lib", super_user=True)
            self.run_command(f"cp -af {self.common.bin_cmc_path}/application/src_bin/platforms/{site_data_path}/* {self.config.rootfs_path}/opt/pme/lib", super_user=True)
            self.common.remove_file(os.path.join(self.config.rootfs_path, "opt/pme/lib/CML.fbs"))



# 提供脚本单独调试功能，在application/build目录下运行python3 works/xxxx.py
if __name__ == "__main__":
    # pack_java.py未移植完成，以下代码是为保持shell调用方式功能正确
    board = "2488hv6"
    if len(sys.argv) >= 2:
        board = sys.argv[1]
    cfg = Config(board, "target_personal")
    wk = WorkBuildCfg(cfg)
    wk.run()
    wk.install()
