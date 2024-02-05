#!/usr/bin/python
# -*- coding: UTF-8 -*-

'''
功    能：下载sp ibma
版权信息：华为技术有限公司，版本所有(C) 2022
'''
import sys
import os
cwd=os.path.dirname(__file__)
sys.path.append(f"{cwd}/../")
sys.path.append(f"{cwd}/../../")
from works.work import Work
from utils.config import Config

class WorkDownloadSpIbma(Work):

    def download(self):
        self.logger.info('download dependency sp ibma begin...')
        agentpath = os.path.join(self.common.temp_path, "spftp")
        self.common.check_path(agentpath)
        dependency_file = self.get_path("dependency_for_bin/dependency.xml")
        cmd = f"artget pull -d {dependency_file} -ap {agentpath}"
        self.run_command(cmd)
        self.logger.info("download sp ibma done")

    def run(self):
        self.download()

# 提供脚本单独调试功能，在application/build目录下运行python3 works/xxxx.py
if __name__ == "__main__":
    cfg = Config("2288hv5", "target_personal")
    wk = WorkDownloadSpIbma(cfg)
    wk.run()
