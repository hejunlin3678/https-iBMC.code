#!/usr/bin/python
# -*- coding: UTF-8 -*-

'''
功    能：转换平台srcbaseline.xml 为manifest格式xml
版权信息：华为技术有限公司，版本所有(C) 2022-2023
修改记录：2022-08-22 创建
'''
import subprocess
import sys
import os
import argparse
cwd=os.path.dirname(__file__)
sys.path.append(f"{cwd}/../")
sys.path.append(f"{cwd}/../../")
from works.work import Work
from utils.config import Config

class WorkGetManifest(Work):

    def __init__(self, config, work_name=""):
        super(WorkGetManifest, self).__init__(config, work_name)
        self.defult_d = {
            "pme" : "PME",
            "rtos": "RTOS",
            "site_ai": "SiteAI",
            "site_data": "SiteData",
            "hostsec": "HostSec",
            "sdk": "SDK",
            "adaptive_lm": "AdaptiveLM",
            "kmc": "KMC",
            "cbb": "CBB",
            "vpp": "VPP",
            "hi1880": "HI1880"
        }
        self.artfactory = ""

    def set_artfactory(self, artfactory):
        self.artfactory = artfactory

    def run(self):
        os.chdir(cwd)
        if self.artfactory == "vpp":
            cmd = f"python3 download_from_cmc_SrcBaseline.py " \
                f"-p {self.defult_d.get(self.artfactory)} " \
                f"-x {self.artfactory}/'srcbaseline*.xml' "
        else:
            cmd = f"python3 download_from_cmc_SrcBaseline.py " \
                f"-p {self.defult_d.get(self.artfactory)} " \
                f"-x {self.artfactory}/srcbaseline.xml"
        self.run_command(cmd)


# 提供脚本单独调试功能，在application/build目录下运行python3 works/xxxx.py
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="build target")
    parser.add_argument("-m", "--platform", help="platforms you want to get", default="")
    args = parser.parse_args()
    cfg = Config()
    wk = WorkGetManifest(cfg)
    wk.artfactory = args.platform
    wk.run()
