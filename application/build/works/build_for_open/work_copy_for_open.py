#!/usr/bin/env python
# coding=utf-8

"""
文件名：work_copy_for_open.py
功能：从copy_for_open.py中拆出，用于执行代码裁剪
版权信息：华为技术有限公司，版本所有(C) 2019-2020
"""

import os
import time
import shutil
import sys
import re
import argparse
import subprocess
import json

cwd=os.path.dirname(__file__)
sys.path.append(f"{cwd}/../")
sys.path.append(f"{cwd}/../../")
from works.work import Work
from utils.config import Config
from utils.common import Common
from utils.error_info import EXCEPTION_STAGE
from utils.error_info import EXCEPTION_RESPONSE
from utils.error_info import EXCEPTION_CODE

class WorkCopyForOpen(Work):
    def __init__(self, config, work_name=""):
        super(WorkCopyForOpen, self).__init__(config, work_name)
        self.manifest_file_path = f"{self.common.code_root}/prepare_for_open/manifest.yml"
    
    # 根据底板profile.txt文件拷贝板卡xml文件。
    def copy_application_src_resource_profile_by_board(self):
        resource_board_profile_files = self.get_manufacture_config(self.manifest_file_path, f"resource_board_profile/files")
        for resource_board_profile in resource_board_profile_files:
            cur_profile_txt = resource_board_profile.get("file")
            if not os.path.exists(cur_profile_txt):
                err_msg = f"{EXCEPTION_CODE[600]}{EXCEPTION_STAGE['P']}{EXCEPTION_RESPONSE[2]}"
                self.logger.error(f"{err_msg}copy {resource_board} profile.txt fail")
                os._exit(0)
            with open(rf'{cur_profile_txt}') as somefile:
                lines = somefile.readlines()
                for line in lines:
                    result = line.strip('\n')
                    if result != "":
                        # 创建对应的目录
                        dir_list = result.split("/")
                        dir_list.pop(len(dir_list) - 1)
                        relative_dir = "/".join(dir_list)
                        xml_profile_src_path = f"{self.common.code_root}/application/src/resource/profile/{result}"
                        if not os.path.exists(xml_profile_src_path):
                            self.logger.error(f"{xml_profile_src_path} not exist")
                            continue
                        xml_profile_target_path = f"{self.common.open_code_root}/application/src/resource/profile/{relative_dir}"
                        self.common.check_path(xml_profile_target_path)
                        shutil.copy(f"{xml_profile_src_path}", f"{xml_profile_target_path}")
        return
    
    # v6_img.json 添加语句
    def modify_v6_img_json(self):
        with open(rf'{self.common.code_root}/config/v6_img.json') as content_file:
            content = json.load(content_file)
        with open(rf'{self.common.code_root}/prepare_for_open/config/v6_img.json') as prepare_content_file:
            prepare_content = json.load(prepare_content_file)
        act_list = ['dos2unix', 'mkdir -p', 'cp -a', 'rm -rf', 'mv', 'mv -f', 'touch', "sed", 'ln -s', 'chown', 'chmod', 'strip']
        for act in act_list:
            if prepare_content.get(act, False):
                content.get(act).extend(prepare_content.get(act))
        with open(rf'{self.common.open_code_root}/config/v6_img.json', "w") as f:
            json.dump(content, f, indent=4)
    
    # 递归删除某个目录下所有.c和.h源码里的注释信息
    def del_comment(self, file_path):
        filelist = os.listdir(file_path)
        for file in filelist:
            if os.path.isdir(file_path + '/' + file):
                self.del_comment(f"{file_path}/{file}")
            elif re.search(r'\.(c|h)$', file):
                os.chdir(file_path)
                with open(file, 'r') as f:
                    content = f.read()
                content = re.sub(r'/\*\s.*?\*/', '', content, flags=re.S)
                #content = re.sub(re.compile(r'/\* .*?(dts\d|20\d{2}(-|/|年)\d|wx\d|[a-z]00\d{6}\D|author[ :]|uadp).*?\*/', re.I), '', content)
                content = re.sub(re.compile(r'// .*(dts\d|20\d{2}(-|/|年)\d|wx\d|[a-z]00\d{6}\D|author[ :]|uadp).*', re.I), '', content)
                
                with open(file, 'w') as f:
                    f.write(content)

    # 拷贝之前，给源码打补丁操作
    def patch_src(self):
        patch_src_files = self.get_manufacture_config(self.manifest_file_path, f"patch_src/files")
        # self.logger.info(f"patch_src_files {patch_src_files}")
        for patch in patch_src_files:
            src_file_path = patch.get("src_file")
            patch_file_path = patch.get("patch_file")
            subprocess.run(f"patch {src_file_path} {patch_file_path}", shell=True)

    def run(self):
        self.logger.info(f"copy for open start")
        if os.path.exists(f"{self.common.open_code_root}"):
            self.common.remove_path(f"{self.common.open_code_root}")
        self.common.check_path(f"{self.common.open_code_root}")
        # 拷贝之前，给源码打补丁操作
        self.patch_src()
        # 拷贝裁剪清单的文件
        code_root_application_files = self.get_manufacture_config(self.manifest_file_path, f"code_root/application/files")
        self.copy_manifest_files(code_root_application_files)
        code_root_bin_cmc_files = self.get_manufacture_config(self.manifest_file_path, f"code_root/bin_cmc/files")
        self.copy_manifest_files(code_root_bin_cmc_files)
        code_root_config_files = self.get_manufacture_config(self.manifest_file_path, f"code_root/config/files")
        self.copy_manifest_files(code_root_config_files)
        code_root_platforms_files = self.get_manufacture_config(self.manifest_file_path, f"code_root/platforms/files")
        self.copy_manifest_files(code_root_platforms_files)
        code_root_tools_files = self.get_manufacture_config(self.manifest_file_path, f"code_root/tools/files")
        self.copy_manifest_files(code_root_tools_files)
        code_root_vendor_files = self.get_manufacture_config(self.manifest_file_path, f"code_root/vendor/files")
        self.copy_manifest_files(code_root_vendor_files)
        code_root_files = self.get_manufacture_config(self.manifest_file_path, f"code_root/files")
        self.copy_manifest_files(code_root_files)
        # 拷贝跟单板名相关的文件
        resource_files = self.get_manufacture_config(self.manifest_file_path, f"resource_board")
        board_list = resource_files.keys()
        for board_key in board_list:
            resource_board_files = self.get_manufacture_config(self.manifest_file_path, f"resource_board/{board_key}/files")
            self.copy_manifest_files(resource_board_files)
        # 根据底板profile.txt文件拷贝板卡xml文件
        self.copy_application_src_resource_profile_by_board()
        # 拷贝 web_online_help
        web_online_help_files = self.get_manufacture_config(self.manifest_file_path, f"web_online_help/files")
        self.copy_manifest_files(web_online_help_files)
        # 更改 v6_img.json
        self.modify_v6_img_json()
        # 删除开放组件源码里的注释信息，由于是硬链接形式存在，这里修改会同步到源码
        self.del_comment(f"{self.common.open_code_root}/application/src/apps/card_manage")
        self.del_comment(f"{self.common.open_code_root}/application/src/apps/bios")
        self.del_comment(f"{self.common.open_code_root}/application/src/apps/frudata")
        self.del_comment(f"{self.common.open_code_root}/application/src/apps/payload")
        self.del_comment(f"{self.common.open_code_root}/application/src/apps/redfish")
        self.del_comment(f"{self.common.open_code_root}/application/src/apps/upgrade") # 只有若干头文件
        self.del_comment(f"{self.common.open_code_root}/application/src/apps/bmc_global") # 只有若干头文件
        self.del_comment(f"{self.common.open_code_root}/application/src/apps/sensor_alarm") # 只有若干头文件
        self.del_comment(f"{self.common.open_code_root}/application/src/apps/ui_manage")
        self.del_comment(f"{self.common.open_code_root}/application/src/user_interface/cli")
        # 清理 git 文件
        gitkeep_file_list = self.common.py_find_dir("\.gitkeep", f"{self.common.open_code_root}", True)
        for gitkeep_file in gitkeep_file_list:
            os.remove(gitkeep_file)
        self.logger.info(f"copy for open end")

# 提供脚本单独调试功能，在application/build目录下运行python3 works/build_for_open/work_copy_for_open.py
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="copy for open")
    parser.add_argument("-b", "--board_name", help="Board list must in '' and separated by space, default value is 2488hv6", default="2488hv6")
    args = parser.parse_args()
    cfg = Config(args.board_name)
    wk = WorkCopyForOpen(cfg)
    wk.run()
