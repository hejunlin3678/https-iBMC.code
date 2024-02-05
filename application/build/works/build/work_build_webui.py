#!/usr/bin/python
# -*- coding: UTF-8 -*-

'''
功    能：
版权信息：华为技术有限公司，版本所有(C) 2021
修改记录：2021-10-11 创建
'''
import subprocess
import sys
import os
import shutil
cwd=os.path.dirname(__file__)
sys.path.append(f"{cwd}/../")
sys.path.append(f"{cwd}/../../")
from works.work import Work
from utils.config import Config
from datetime import datetime

class WorkBuildWebui(Work):
    def download_code(self):
        # 下载代码
        self.logger.info('download code begin...')
        self.webui_path = f"{self.common.temp_path}/webui"
        shutil.rmtree(self.webui_path, ignore_errors=True)
        os.makedirs(self.webui_path, exist_ok=True)

        os.chdir(self.webui_path)
        subprocess.run(f"cp {self.common.code_path}/component_manifest.xml ./component_manifest.xml", shell=True)

        os.chdir(self.webui_path)
        self.run_command("git init .")
        self.run_command("git add component_manifest.xml")
        self.run_command("git commit -m \"init\"")
        self.run_command(f"git mm init -u {self.webui_path} -m component_manifest.xml")
        self.run_command("git mm sync -d --force-sync --depth 1 -j8")
        self.logger.info('download code end!')

    def build_webui(self):
        self.logger.info('build web begin...')
        os.chdir(f"{self.common.temp_path}/webui/webui")
        self.run_command("npm config set registry https://cmc.centralrepo.rnd.huawei.com/artifactory/api/npm/npm-central-repo/")
        self.run_command("npm ci")
        self.run_command("npm run build")
        self.logger.info('run build end!')
        # 编译完成后删除node_modules目录，释放空间
        shutil.rmtree("./node_modules", False)
        # 切换dist为工作目录
        self.common.copy_all("dist", os.path.join(self.common.temp_path, "webui_dist"))
        self.logger.info('build web end!')

    def run(self):
        self.download_code()
        self.build_webui()

    def install(self):
        dst_path = os.path.realpath(f"{self.config.rootfs_path}/opt/pme/web/htdocs/")
        src_path = os.path.realpath(f"{self.common.temp_path}/webui_dist/")
        self.common.copy_all(src_path, dst_path)


# 提供脚本单独调试功能，在application/build目录下运行python3 works/xxxx.py
if __name__ == "__main__":
    cfg = Config("2488hv6", "target_personal")
    wk = WorkBuildWebui(cfg)
    wk.run()
    wk.install()
