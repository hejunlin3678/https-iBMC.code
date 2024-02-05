#!/usr/bin/python
# -*- coding: UTF-8 -*-

'''
功    能：下载并安装rtos/hcc工具
版权信息：华为技术有限公司，版本所有(C) 2021
修改记录：2021-10-11 创建
'''
import subprocess
import sys
import os
import argparse
import json
cwd=os.path.dirname(__file__)
sys.path.append(f"{cwd}/../")
sys.path.append(f"{cwd}/../../")
from works.work import Work
from utils.config import Config

class WorkGetFirmware(Work):
    artfactory = ""

    def __init__(self, config, work_name=""):
        super(WorkGetFirmware, self).__init__(config, work_name)
        self.board_version = self.config.board_version

    def set_artfactory(self, artfactory):
        self.artfactory = artfactory
    def set_from_source(self, from_source):
        self.config.from_source = from_source
        self.config.open_local_debug = True

    def read_json(self, json_path):
        if not os.path.exists(json_path):
            raise Exception
        with open(json_path, "r") as json_file:
            return json.load(json_file)
        raise Exception

    def check_need_download(self):
        dependency_file = f"./works/platforms/dependency_extra.json"
        dependency_list = self.read_json(dependency_file)

        if self.artfactory not in dependency_list.keys():
            self.logger.info(f"All boards need {self.artfactory}.")
            return True
        if self.config.board_name in dependency_list[self.artfactory]:
            return True
        else:
            self.logger.info(f"{self.config.board_name} do not need {self.artfactory}.")
            return False

    def download_software(self, dependency_file):
        self.logger.info(f"Download {self.artfactory} begin...")
        # 以temp_path为基准目录
        cmd = f"artget pull -d {dependency_file} -ap {self.common.temp_path}"
        self.run_command(cmd, ignore_error=True, retry=3)
        self.logger.info(f"Download {self.artfactory} end!")

    def make_xml_for_metadata(self, dependency_file):
        # 每个单板将各自依赖的组件xml综合到一个xml文件中，供ci调用artget插件生成元数据，用于刷新结果树。
        output_path = f"{self.common.code_path}/dependency_for_hpm/downxml/board"
        self.common.check_path(output_path)
        output_file = f"{output_path}/{self.config.board_name}_dependency.xml"
        # 删除后两行
        with open(output_file, 'r') as r:
            lines = r.readlines()
        with open(output_file, 'w') as w:
            for li in lines:
                if ('</dependencies>' in li):
                    w.write(li.replace('</dependencies>', ''))
                elif ('</project>' in li):
                    w.write(li.replace('</project>', ''))
                else:
                    w.write(li)
        # 追加新内容
        file = open(output_file, 'a+')
        for line in open(dependency_file):
            if "<project>" not in line and "<dependencies>" not in line:
                file.write(line)
        file.close()

    def run(self):
        if self.config.from_source:
            self.logger.info(f"Compile {self.artfactory} from source.")
            dependency_file = self.get_path(f"works/platforms/{self.artfactory}/srcbaseline.xml")
            # 调用根据构建指导书编写的下载、编译、打包脚本
            if self.artfactory == "vpp":
                if os.path.exists(os.path.join(self.common.code_root, "VPP")):
                    self.logger.info(f"Download {self.artfactory} from source.")
                    subprocess.run(f"sh works/platforms/{self.artfactory}/prebuild.sh", shell=True, check=True)
                else:
                    subprocess.run(f"sh works/platforms/{self.artfactory}/prebuild.sh -o", shell=True, check=True)  
            else:
                if self.config.open_local_debug:
                    self.logger.info(f"Download {self.artfactory} from srcbaseline.")
                    subprocess.run(f"sh works/platforms/{self.artfactory}/prebuild.sh -o", shell=True, check=True)
                else:
                    self.logger.info(f"Download {self.artfactory} from source.")
                    subprocess.run(f"sh works/platforms/{self.artfactory}/prebuild.sh", shell=True, check=True)
            if self.board_version == "V6" and self.artfactory == "vpp":
                subprocess.run(f"sh works/platforms/{self.artfactory}/build64.sh", shell=True, check=True)
                subprocess.run(f"sh works/platforms/{self.artfactory}/postbuild64.sh", shell=True, check=True)
            elif self.artfactory == "pme":
                # pme源码构建需要区分rel/mas，分别对应kernel 4.4和5.10版本
                if self.config.kernel.find("4.4") > 0:
                    typ = "rel"
                    kernel = "4.4"
                else:
                    typ = "mas"
                    kernel = "5.10"
                self.logger.info("pme type: {}, kernel: {}".format(typ, kernel))
                subprocess.run(f"sh works/platforms/{self.artfactory}/build.sh {self.config.board_version} {typ} {kernel}", shell=True, check=True)
                subprocess.run(f"sh works/platforms/{self.artfactory}/postbuild.sh {self.config.board_version}", shell=True, check=True)
            else:
                subprocess.run(f"sh works/platforms/{self.artfactory}/build.sh", shell=True, check=True)
                subprocess.run(f"sh works/platforms/{self.artfactory}/postbuild.sh", shell=True, check=True)
        else:
            # PME在源码pipeline场景下不需要下载，由PME源码构建后CI放入temp目录
            if self.artfactory == "pme" and self.config.full_src_build:
                return
            # 非全量源码的场景，先判断单板是否依赖该组件
            if self.check_need_download() == False:
                return
            dependency_file = self.get_path(f"works/platforms/{self.artfactory}/software.xml")
            if self.config.info_tree:
                # 生成xml文件给ci，用于刷新结果树
                self.logger.info(f"Make {self.artfactory} metadate, no need download!")
                self.make_xml_for_metadata(dependency_file)
                return
            # 直接从CMC下载固件
            self.download_software(dependency_file)
            dependency_file = self.get_path(f"works/platforms/ThirdPartyGroupware/software.xml")
            self.download_software(dependency_file)


# 提供脚本单独调试功能，在application/build目录下运行python3 works/xxxx.py
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="build target")
    parser.add_argument("-b", "--board_name", help="Board list must in '' and separated by space, default value is 2488hv6", default="2488hv6")
    parser.add_argument("-m", "--platform", help="platforms you want to get", default="")
    parser.add_argument("-o", "--open_local_debug", help="open_local_debug", action="store_true")
    args = parser.parse_args()

    cfg = Config(args.board_name)
    cfg.open_local_debug = args.open_local_debug
    wk = WorkGetFirmware(cfg)
    wk.artfactory = args.platform
    wk.run()
