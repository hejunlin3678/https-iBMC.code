#!/usr/bin/python
# -*- coding: UTF-8 -*-

'''
功    能：
版权信息：华为技术有限公司，版本所有(C) 2021
修改记录：2021-10-11 创建
'''
import time
import subprocess
import sys
import os
import shutil
import stat
cwd=os.path.dirname(__file__)
sys.path.append(f"{cwd}/../")
sys.path.append(f"{cwd}/../../")
from works.work import Work
from utils.config import Config
from utils.config import Config
from datetime import datetime
from utils.common import Common
from utils.error_info import EXCEPTION_STAGE
from utils.error_info import EXCEPTION_RESPONSE
from utils.error_info import EXCEPTION_CODE

class WorkBuildTinyUI(Work):
    def judge_board(self):
        IRM_board_lists = ["RM210", "RM210_SECURITY_3V0", "RM211"]
        EMM_board_lists = ["CN90SMMA"]
        PANGEA_board_lists = ["PangeaV6_Atlantic", "PangeaV6_Atlantic_Smm", "PangeaV6_Pacific", "PangeaV6_Arctic", "PAC1220V6"]

        target_path = f"{self.common.temp_path}/tiny_temp/webapp/src"
        origin_path = f"{self.common.temp_path}/tiny_temp/webapp/src/assets/bmc-assets"

        if self.config.board_name in IRM_board_lists:
            subprocess.run(f"sed -i 's/bmc-app/irm-app/g' {self.common.temp_path}/tiny_temp/webapp/src/app/app-routing.module.ts",
            shell=True)
            subprocess.run(f"sed -i 's/BmcAppModule/IRMAppModule/g' {self.common.temp_path}/tiny_temp/webapp/src/app/app-routing.module.ts",
            shell=True)
            subprocess.run(f"sed -i 's/bmc-assets/irm-assets/g' {self.common.temp_path}/tiny_temp/webapp/angular.json",
            shell=True)
            origin_path = f"{self.common.temp_path}/tiny_temp/webapp/src/assets/irm-assets"
        elif self.config.board_name in EMM_board_lists:
            subprocess.run(f"sed -i 's/bmc-app/bmc-app/g' {self.common.temp_path}/tiny_temp/webapp/src/app/app-routing.module.ts",
            shell=True)
            subprocess.run(f"sed -i 's/BmcAppModule/BmcAppModule/g' {self.common.temp_path}/tiny_temp/webapp/src/app/app-routing.module.ts",
            shell=True)
            subprocess.run(f"sed -i 's/bmc-assets/bmc-assets/g' {self.common.temp_path}/tiny_temp/webapp/angular.json",
            shell=True)
            origin_path = f"{self.common.temp_path}/tiny_temp/webapp/src/assets/bmc-assets"
        elif self.config.board_name in PANGEA_board_lists:
            subprocess.run(f"sed -i 's/bmc-app/pangea-app/g' {self.common.temp_path}/tiny_temp/webapp/src/app/app-routing.module.ts",
            shell=True)
            subprocess.run(f"sed -i 's/BmcAppModule/PangeaAppModule/g' {self.common.temp_path}/tiny_temp/webapp/src/app/app-routing.module.ts",
            shell=True)
            subprocess.run(f"sed -i 's/bmc-assets/pangea-assets/g' {self.common.temp_path}/tiny_temp/webapp/angular.json",
            shell=True)
            subprocess.run(f"sed -i 's/PangeaV6_Arctic/{self.config.board_name}/g' {self.common.temp_path}/tiny_temp/webapp/src/app/pangea-app/guard/pangea-guard.service.ts",
            shell=True)
            origin_path = f"{self.common.temp_path}/tiny_temp/webapp/src/assets/pangea-assets"
            # 打包前处理 web 文件, 修改_kvmEncryptionConfig，防止非法用户
            if self.common.compability_enable == 1 :
                subprocess.run(f"sed -i 's/_kvmEncryptionConfig = false/_kvmEncryptionConfig = true/g' {self.common.temp_path}/tiny_temp/webapp/src/app/common-app/modules/virtualControl/kvm-h5/models/security.ts",
                shell=True)
        else:
            # 打包前处理 web 文件, 修改_kvmEncryptionConfig，防止非法用户
            if self.common.compability_enable == 1 :
                subprocess.run(f"sed -i 's/_kvmEncryptionConfig = false/_kvmEncryptionConfig = true/g' {self.common.temp_path}/tiny_temp/webapp/src/app/bmc-app/modules/virtualControl/kvm-h5/models/security.ts",
                shell=True)

        subprocess.run(f"mv {origin_path}/favicon.ico {target_path}/favicon.ico", shell=True)

    def get_board_version(self):
        if self.config.board_name == "":
            board_version = "V5"
        else:
            board_version = self.common.get_board_version(self.config.board_name)
        return board_version

    def build_tiny(self):
        self.logger.info('build web begin...')
        os.chdir(self.common.temp_path)
        self.common.remove_path("tiny_temp")
        self.common.remove_path("webapp")
        self.common.check_path("tiny_temp")
        self.common.check_path("webapp")
        platform_dir = os.path.join(self.common.code_root, "platforms")
        self.common.copy_all(os.path.join(platform_dir, "webapp"), os.path.join(self.common.temp_path, "tiny_temp/webapp"))

        os.chdir("tiny_temp/webapp")
        subprocess.run("npm cache clean --force", shell=True)
        self.judge_board()
        self.run_command("echo 'y' | npm ci --loglevel=verbose")
        self.run_command("npm run build")

        self.logger.info("build npm successfully")

        # 编译完成后删除node_modules目录，释放空间。不要删除，后面裁剪的时候需要用到。
        # shutil.rmtree("./node_modules", False)
        # 切换dist为工作目录
        os.chdir("dist")
        # TODO 删除此处的重复复制，由install安装
        self.common.copy_all("webapp", os.path.join(self.common.temp_path, "webapp"))

        resource_id = f"{int(time.time())}{datetime.now().year}"
        subprocess.run(
            f'sed -i "s/\.js/\.js?{resource_id}/g" {self.common.temp_path}/webapp/index.html',
            shell=True)
        subprocess.run(
            f'sed -i "s/\.css/\.css?{resource_id}/g" {self.common.temp_path}/webapp/index.html',
            shell=True)
        subprocess.run(
            f'sed -i "s/\.ico/\.ico?{resource_id}/g" {self.common.temp_path}/webapp/index.html',
            shell=True)
        if self.config.board_name == "CN90SMMA":
            subprocess.run(
                f"sed -i 's/var resourceStr = ''/var resourceStr={resource_id} /' {self.common.temp_path}/webapp/index.html",
                shell=True)

        self.logger.info('build web end!')
        src_path = os.path.realpath(f"{self.common.temp_path}/webapp/")
        self.logger.info("package webapp.tar.gz to {}".format(os.path.realpath(f"{self.config.work_out}/webapp.tar.gz")))
        subprocess.run(f"tar -czf {self.config.work_out}/webapp.tar.gz -C {src_path} .", shell=True)

    def run(self):
        self.build_tiny()
        os.chdir(self.common.code_path)

    def install(self):
        if not self.config.support_dft_separation and self.config.build_type == "dft_separation":
            return
        self.common.remove_file(f"{self.common.temp_path}/webapp/favicon.ico")
        dst_path = os.path.realpath(f"{self.config.rootfs_path}/opt/pme/web/htdocs/")
        src_path = os.path.realpath(f"{self.common.temp_path}/webapp/")
        self.common.copy_all(src_path, dst_path)

# 提供脚本单独调试功能，在application/build目录下运行python3 works/xxxx.py
if __name__ == "__main__":
    cfg = Config("2488hv6", "target_personal")
    wk = WorkBuildTinyUI(cfg)
    wk.run()
    wk.install()
