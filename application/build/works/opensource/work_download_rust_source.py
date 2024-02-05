#!/usr/bin/python
# -*- coding: UTF-8 -*-

'''
功    能：
版权信息：华为技术有限公司，版本所有(C) 2021
修改记录：2021-10-11 创建
'''
import sys
import os
cwd=os.path.dirname(__file__)
sys.path.append(f"{cwd}/../")
sys.path.append(f"{cwd}/../../")
from works.work import Work
from utils.config import Config

class WorkDownloadRust(Work):
    def download_opensource_for_rust(self):
        self.logger.info('download opensource codes begin...')

        dependency_file = self.get_path("dependency_for_hpm/downxml/dependency_opensourcedown.xml")
        os.chdir(self.common.code_path)
        if not os.path.exists(f"{self.common.temp_path}"):
            os.mkdir(f"{self.common.temp_path}")
        shcmd = f"artget pull -os {dependency_file} -at opensource -ap {self.common.temp_path}"
        self.run_command(shcmd)
        self.logger.info('download opensource codes end!')

    def run(self):
        self.download_opensource_for_rust()

# 提供脚本单独调试功能，在application/build目录下运行python3 works/xxxx.py
if __name__ == "__main__":
    cfg = Config()
    wk = WorkDownloadRust(cfg)
    wk.run()
