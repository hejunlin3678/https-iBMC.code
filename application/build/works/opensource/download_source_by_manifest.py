#!/usr/bin/python
# -*- coding: UTF-8 -*-

'''
功    能：根据传入的manifest文件下载代码
版权信息：华为技术有限公司，版本所有(C) 2021
修改记录：2021-11-13 创建
'''
import sys
import os
import subprocess
import shutil
cwd=os.path.dirname(__file__)
sys.path.append(f"{cwd}/../")
sys.path.append(f"{cwd}/../../")
from works.work import Work
from utils.common import Common
from utils.log import Logger

class DownloadSourceByMenifest():
    def __init__(self, work):
        self.work = work

    def download(self, manifest):
        """
        manifest可选值 "opensource_manifest","vendor_manifest","v3_component_manifest"
        """
        ssdnfs_dir = os.path.realpath(self.work.common.code_root).split("/", 2)[2]
        v3_code_dir = f"{ssdnfs_dir}/../"
        manifest_file = f"{self.work.common.code_path}/dependency_for_hpm/downxml/{manifest}.xml"
        self.work.logger.info(f"download source:{manifest_file}")

        # 获取版本构建的tag
        revision = self.work.common.get_shell_command_result(
            f"cat {manifest_file} | grep 'auto-' | awk -F '\"' '{{print $6}}'")
        self.work.common.revision = revision

        # 创建manifest目录
        manifest_dir = f"{self.work.common.temp_path}/{manifest}"
        shutil.rmtree(manifest_dir, ignore_errors=True)
        os.makedirs(manifest_dir, exist_ok=True)
        os.chdir(manifest_dir)
        subprocess.run("git init .", shell=True)
        subprocess.run(f"cp {manifest_file} .", shell=True)

        # 复制文件
        manifest_dic = {
            "opensource_manifest": "../../",
            "vendor_manifest": "../../",
            "v3_component_manifest": "../../../"
        }

        if os.path.islink(self.work.common.temp_path):
            if "v3_component_manifest" in manifest:
                cmd = f"sed -i 's#path=\"#path=\"../../{v3_code_dir}/#g' {manifest}.xml"
            else:
                cmd = f"sed -i 's#path=\"#path=\"../../{ssdnfs_dir}/#g' {manifest}.xml"
        else:
            cmd = f"sed -i 's#path=\"#path=\"{manifest_dic[manifest]}/#g' {manifest}.xml"

        subprocess.run(cmd, shell=True)

        cmd = f"git add . && git commit -m \"init\""
        subprocess.run(cmd, shell=True)
        # Add to .mm directory
        cmd = f"git mm init -u . -m {manifest}.xml"
        subprocess.run(cmd, shell=True)
        # Download file, only the latest node is pulled.
        cmd = f"git mm sync --depth 1 -j$(nproc)"
        self.work.logger.info(f"start download:{manifest_file}")
        self.work.run_command(cmd)
