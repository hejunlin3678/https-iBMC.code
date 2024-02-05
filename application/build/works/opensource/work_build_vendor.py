#!/usr/bin/python
# -*- coding: UTF-8 -*-

'''
功    能：
版权信息：华为技术有限公司，版本所有(C) 2021
修改记录：2021-10-14 创建
自定义参数：package_name 可选all或opensource_build.json中定义的软件包名
'''
import sys
import os
import json
import subprocess
import argparse
import traceback
import shutil
cwd=os.path.dirname(__file__)
sys.path.append(f"{cwd}/../")
sys.path.append(f"{cwd}/../../")
from works.work import Work
from utils.config import Config
from utils.common import Common
from utils.log import Logger

class WorkBuildVendor(Work):
    def __init__(self, config, work_name=""):
        super(WorkBuildVendor, self).__init__(config, work_name)
        with open(f"{cwd}/work_build_vendor.json", "r") as json_file:
            self.choices = json.load(json_file)
        self.vendor_dir = f"{self.common.code_root}/vendor"
        self.package_name = "all"

    def set_package_name(self, package):
        self.package_name = package

    def vender_build(self, package_name, config, board_version):
        prev_build = config.get("prev_build")
        if prev_build:
            self.open_source_build(package_name, prev_build, board_version)
        # 获取package_name和cmd_list字段
        self.logger.info(f"package:{package_name}, build config:{config}")
        cmd_list = config.get("cmd_list")
        if not cmd_list:
            self.logger.info(f"package:{package_name}, miss property:cmd_list, config now:{config}")
            return
        log_name = f"{self.config.build_path}/opensource_log_{package_name}.log"

        # 复制文件到指定目录
        dst = f"{self.config.build_path}/vendor/{self.config.cpu}"
        os.makedirs(dst, exist_ok = True)
        self.common.remove_path(dst)
        shutil.copytree(f"{self.common.code_root}/vendor/{package_name}", dst)
        # 清理无用
        self.common.del_head(dst, "MR_")
        self.common.del_head(dst, "SAS3_")
        os.chdir(dst)

        try:
            subprocess.run('find ./ -name configure | xargs -i{} chmod +x {}', shell=True)

            # 开始编译， 如需调试编译过程，请在此屏蔽日志重定向
            self.logger.info(f"package:{package_name}, start, {board_version}")
            build_cmd = "build64.sh" if board_version == "V6" else "build.sh"
            subprocess.run(f"sed -i 's/source .\/global.sh//g' {build_cmd}", shell=True)
            subprocess.run(f"sed -i 's/source .\/global64.sh//g' {build_cmd}", shell=True)
            subprocess.run(f"bash -x {build_cmd}", shell=True, check=True)

            output_dir = f"{self.common.code_root}/output/vendor"
            if board_version == "V6":
                output_dir = f"{output_dir}/arm64"
            output_dir = f"{output_dir}/{package_name}"
            subprocess.run(f"rm -rf {output_dir}", shell=True)
            subprocess.run(f"mkdir -p {output_dir}", shell=True)
            subprocess.run(f"cp -a ./output/* {output_dir}", shell=True, check=True)
        except Exception as e:
            self.logger.error(f"package:{package_name}, failed, exp:{traceback.print_exc()}, log begin:>>>>>>>>>>>>>>>>")
            subprocess.run(f"cat {log_name}", shell=True)
            self.logger.error(f"package:{package_name}, failed, log end<<<<<<<<<<<<<<<<<<<")
            raise e
        self.logger.info(f"package:{package_name}, successfully")
        # 回到open_source目录
        os.chdir(self.vendor_dir)

    def run(self):
        self.config.set_env_global()
        os.chdir(self.vendor_dir)

        for package in self.choices:
            if package == self.package_name or self.package_name == "all":
                self.logger.info(f">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> build package: {package}, self:{self.package_name}")
                self.vender_build(package, self.choices[package], self.config.board_version)

# 以下为提供脚本单独调试功能示例代码，在application/build目录下运行python3 works/xxxx.py
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="build target")
    parser.add_argument("-p", "--package_name", help="open source package name", default="all")
    parser.add_argument("-b", "--board_name", help="Board list must in '' and separated by space, default value is 2488hv6", default="2488hv6")
    args = parser.parse_args()
    cfg = Config(args.board_name)
    wk = WorkBuildVendor(cfg)
    wk.set_package_name(args.package_name)
    wk.run()