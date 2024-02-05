#!/usr/bin/python
# -*- coding: UTF-8 -*-

'''
功    能：下载npm依赖，构建tinyui时会自动下载，此work仅用于调试和开发使用，only down npm open_source for firstboot
版权信息：华为技术有限公司，版本所有(C) 2021
修改记录：2021-10-11 创建
'''
import time
import subprocess
import stat
import shutil
import sys
import os
cwd=os.path.dirname(__file__)
sys.path.append(f"{cwd}/../")
sys.path.append(f"{cwd}/../../")
from works.work import Work
from utils.config import Config
from utils.error_info import EXCEPTION_STAGE
from utils.error_info import EXCEPTION_RESPONSE
from utils.error_info import EXCEPTION_CODE

class WorkDownloadNpmOpensourceCode(Work):
    def download_npm_open_source(self):
        self.logger.info('download npm open_source begin...')

        open_source = os.path.join(self.common.code_root, "open_source")
        node_modules = os.path.join(open_source, "node_modules")

        self.common.remove_path(f"{self.common.temp_path}/tiny_temp")
        self.common.copy_all(f"{self.common.code_root}/platforms/webapp", f"{self.common.temp_path}/tiny_temp/webapp")
        os.chdir(f"{self.common.temp_path}/tiny_temp/webapp")

        shutil.copy(f"{self.common.temp_path}/tiny_temp/webapp/package.json", open_source)
        shutil.copy(f"{self.common.temp_path}/tiny_temp/webapp/package-lock.json", open_source)
        logname = f"{self.common.log_manage_dic.get('npm_log')}/npm_prefix.log"
        self.common.run_command("npm cache clean -f")
        self.common.run_command(f"echo 'y' | npm ci --prefix {open_source} &> {logname}")

        # special文件是需要特殊处理的
        node_modules_files = ["web-animations-js", "css-vars-ponyfill", "echarts",
                            "rxjs", "tslib", "zone.js", "crypto-js", "zrender", "ngx-echarts",
                            "balanced-match", "get-css-data"]
        special_files = ["@ngx-translate/core", "@ngx-translate/http-loader"]

        # 将开源软件从node_modules中移动出来
        for file in node_modules_files:
            self.common.remove_path(os.path.join(open_source, file))
            self.common.copy_all(os.path.join(node_modules, file), os.path.join(open_source, file))

        # 特殊处理angular
        self.common.remove_path(os.path.join(open_source, "angular"))
        angular_files = ("router", "platform-browser-dynamic", "platform-browser",
                        "forms", "core", "compiler", "common", "animations")
        for file in angular_files:
            origin_file_path = os.path.join(node_modules, "@angular")
            target_file_path = os.path.join(open_source, "angular")
            self.common.copy_all(os.path.join(origin_file_path, file), os.path.join(target_file_path, file))

        for i in range(len(special_files)):
            real_file = special_files[i].split('/')[-1]
            # core文件需要改名为ngx-translate-core
            if i == 0:
                real_file = f"ngx-translate-{real_file}"
            self.common.remove_path(os.path.join(open_source, real_file))
            self.common.copy_all(os.path.join(node_modules, special_files[i]), os.path.join(open_source, real_file))

        self.common.remove_path(node_modules)
        os.remove(f"{open_source}/package.json")
        os.remove(f"{open_source}/package-lock.json")
        self.logger.info('download npm open_source end!')
        os.chdir(self.common.code_path)

    def download_npm_v3_open_source(self):
        self.logger.info('download v3 npm open_source begin...')

        open_source = os.path.join(self.common.code_root, "open_source")
        node_modules = os.path.join(open_source, "node_modules")

        os.chmod(f"{self.common.temp_path}/webui", stat.S_IRWXU | stat.S_IRWXG | stat.S_IROTH + stat.S_IXOTH)
        os.chdir(f"{self.common.temp_path}/webui/webui")

        shutil.copy(f"{self.common.temp_path}/webui/webui/package.json", open_source)
        shutil.copy(f"{self.common.temp_path}/webui/webui/package-lock.json", open_source)
        logname = f"{self.common.log_manage_dic.get('npm_log')}/npm_prefix.log"
        self.common.run_command(f"npm cache clean -f")
        self.common.run_command(f"echo 'y' | npm ci --prefix {open_source} &> {logname}")

        # special文件是需要特殊处理的
        node_modules_files = ["axios", "vue-router", "async-validator", "vue-i18n", "vue-devtools",
                            "vue", "@ctrl", "element-plus", "@popperjs", "dayjs", "normalize-wheel-es",
                            "escape-html", "lodash", "lodash-es", "vuex", "zrender"]

        # 将开源软件从node_modules中移动出来
        for file in node_modules_files:
            self.common.remove_path(os.path.join(open_source, file))
            self.common.copy_all(os.path.join(node_modules, file), os.path.join(open_source, file))

        self.common.remove_path(node_modules)
        os.remove(f"{open_source}/package.json")
        os.remove(f"{open_source}/package-lock.json")
        self.logger.info('download v3 npm open_source end!')
        os.chdir(self.common.code_path)


    def run(self):
        self.download_npm_open_source()

# 提供脚本单独调试功能，在application/build目录下运行python3 works/xxxx.py
if __name__ == "__main__":
    cfg = Config("2488hv6", "target_personal")
    wk = WorkDownloadNpmOpensourceCode(cfg)
    wk.run()
