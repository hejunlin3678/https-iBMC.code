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

class WorkBuildOpensource(Work):
    def __init__(self, config, work_name=""):
        super(WorkBuildOpensource, self).__init__(config, work_name)
        with open(f"{cwd}/work_build_open_source.json", "r") as json_file:
            self.choices = json.load(json_file)
        self.opensource_dir = f"{self.common.code_root}/open_source"
        self.build_dir = f"{self.config.build_path}/open_source/{self.config.cpu}"
        os.makedirs(self.build_dir, exist_ok=True)
        self.package_name = "all"

    def set_package_name(self, package):
        self.package_name = package

    def open_source_output(self, output_dir, real_name):
        # 复制到指定目录
        if self.config.board_version == "V6":
            output_dir = f"{output_dir}/arm64"
        self.run_command(f"mkdir -p {output_dir}/{real_name}")
        self.run_command(f"cp -a ./output/* {output_dir}/{real_name}")

    def open_source_build(self, package_name, config, board_version):
        # 如果有依赖编译项的，先编译依赖
        prev_build = config.get("prev_build")
        if prev_build:
            self.open_source_build(package_name, prev_build, board_version)
        self.logger.info(f"package:{package_name}, build config:{config}")
        # 源码目录
        source = config.get("source")
        if not source:
            self.logger.info(f"package:{package_name}, miss property:source, config now:{config}")
            return
        # 复制源码到编译目录
        src_dir = f"{self.opensource_dir}/{source}"
        dst_dir = f"{self.build_dir}/{source}"
        self.logger.info(f"src:{src_dir}, dst:{dst_dir}")
        shutil.rmtree(dst_dir, ignore_errors=True)
        shutil.copytree(src_dir, dst_dir, symlinks=True)
        os.chdir(self.build_dir)
        # 编译命令
        cmd_list = config.get("cmd_list")
        if not cmd_list:
            self.logger.info(f"package:{package_name}, miss property:cmd_list, config now:{config}")
            return
        # 包名
        real_name = config.get("package_name")
        if not real_name:
            self.logger.info(f"package:{package_name}, miss property:package_name, config now:{config}")
            return

        log_name = f"{self.config.build_path}/opensource_log_{package_name}.log"
        try:
            # 编译前准备工作
            for k in cmd_list:
                self.logger.info(f"package:{package_name}, run command:{k}")
                # 执行cmd_list命令
                if k.startswith("cd "):
                    os.chdir(k[3:])
                    continue
                subprocess.run(f"{k}", shell=True, check=True)
            subprocess.run('find ./ -name configure | xargs -i{} chmod +x {}', shell=True)
            # 开始编译， 如需调试编译过程，请在此屏蔽日志重定向
            self.logger.info(f"package:{package_name}, start, {board_version}")
            build_cmd = "./build64.sh" if board_version == "V6" else "./build.sh"
            # 删除build.sh中的source ./global.sh和./global64.sh字段，由python初始化环境
            subprocess.run(f"sed -i 's/source .\/global.sh//g' ./build64.sh", shell=True)
            subprocess.run(f"sed -i 's/source .\/global64.sh//g' ./build64.sh", shell=True)
            subprocess.run(f"sed -i 's/source .\/global.sh//g' ./build.sh", shell=True)
            subprocess.run(f"sed -i 's/source .\/global64.sh//g' ./build.sh", shell=True)
            self.run_command(f"pwd && bash -x {build_cmd}")

            # 复制到指定目录
            output_dir = f"{self.common.output_path}/open_source"
            self.open_source_output(output_dir, real_name)
            # 额外拷贝到share_path，并和从cmc下载的目录保持一致
            share_dir = f"{self.common.code_root}/share_path/bin_cmc/application/src_bin/open_source/"
            self.open_source_output(share_dir, real_name)
        except Exception as e:
            shutil.rmtree(dst_dir, ignore_errors=True)
            self.logger.error(f"package:{package_name}, failed, exp:{traceback.print_exc()}, log begin:>>>>>>>>>>>>>>>>")
            subprocess.run(f"cat {log_name}", shell=True)
            self.logger.error(f"package:{package_name}, failed, log end<<<<<<<<<<<<<<<<<<<")
            raise e
        shutil.rmtree(dst_dir, ignore_errors=True)
        self.logger.info(f"package:{package_name}, successfully")
        # 回到open_source目录
        os.chdir(self.opensource_dir)

    def run(self):
        self.config.set_env_global()
        os.chdir(self.opensource_dir)
        for package in self.choices:
            if package == self.package_name or self.package_name == "all":
                self.logger.info(f">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> build package: {package}, self:{self.package_name}")
                self.logger.info(f"build package: {package}, self:{self.package_name}")
                self.open_source_build(package, self.choices[package], self.config.board_version)


# 以下为提供脚本单独调试功能示例代码，在application/build目录下运行python3 works/xxxx.py
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="build target")
    parser.add_argument("-p", "--package_name", help="open source package name", default="all")
    parser.add_argument("-b", "--board_name", help="Board list must in '' and separated by space, default value is 2488hv6", default="2488hv6")
    args = parser.parse_args()
    cfg = Config(args.board_name)
    wk = WorkBuildOpensource(cfg)
    wk.set_package_name(args.package_name)
    wk.run()