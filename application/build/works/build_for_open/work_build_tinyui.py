#!/usr/bin/python
# -*- coding: UTF-8 -*-
# Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
'''
功    能：
版权信息：华为技术有限公司，版本所有(C) 2021
修改记录：2021-10-11 创建
'''

import time
import subprocess
import sys
import os
cwd = os.path.dirname(__file__)
sys.path.append(f"{cwd}/../")
sys.path.append(f"{cwd}/../../")
from datetime import datetime, timezone
from works.build.work_build_tinyui import WorkBuildTinyUI
from utils.config import Config
from utils.common import Common


class WorkBuildTinyUI(WorkBuildTinyUI):

    def build_tiny(self):
        self.logger.info('build web begin...')
        os.chdir(self.common.temp_path)
        # 保留tiny_temp和webapp文件夹。
        web_app_str = "webapp"
        self.common.check_path("tiny_temp")
        self.common.check_path(web_app_str)
        platform_dir = os.path.join(self.common.code_root, "platforms")
        self.common.copy_all(os.path.join(platform_dir, web_app_str), 
            os.path.join(self.common.temp_path, "tiny_temp/webapp"))

        os.chdir("tiny_temp/webapp")
        self.judge_board()
        self.run_command("npm run build")

        self.logger.info("build npm successfully")

        # 编译完成后删除node_modules目录，释放空间。裁剪分支不要删除。
        # 切换dist为工作目录
        os.chdir("dist")
        self.common.copy_all(web_app_str, os.path.join(self.common.temp_path, web_app_str))

        resource_id = f"{int(time.time())}{datetime.now(tz=timezone.utc).year}"
        self.run_command(
            f'sed -i "s/\.js/\.js?{resource_id}/g" {self.common.temp_path}/webapp/index.html')
        self.run_command(
            f'sed -i "s/\.css/\.css?{resource_id}/g" {self.common.temp_path}/webapp/index.html')
        self.run_command(
            f'sed -i "s/\.ico/\.ico?{resource_id}/g" {self.common.temp_path}/webapp/index.html')
        if self.config.board_name == "CN90SMMA":
            self.run_command(
                f"sed -i 's/var resourceStr = ''/var resourceStr={resource_id} /' \
                    {self.common.temp_path}/webapp/index.html")

        self.logger.info('build web end!')
        src_path = os.path.realpath(f"{self.common.temp_path}/webapp/")
        self.logger.info("package webapp.tar.gz to %s", os.path.realpath(f"{self.config.work_out}/webapp.tar.gz"))
        self.run_command(f"tar -czf {self.config.work_out}/webapp.tar.gz -C {src_path} .")


# 提供脚本单独调试功能，在application/build目录下运行python3 works/xxxx.py
if __name__ == "__main__":
    cfg = Config("2488hv6", "target_personal")
    wk = WorkBuildTinyUI(cfg)
    wk.run()
    wk.install()
