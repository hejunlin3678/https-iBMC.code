#!/usr/bin/python
# -*- coding: UTF-8 -*-

'''
功    能：
版权信息：华为技术有限公司，版本所有(C) 2021
修改记录：2021-10-11 创建
'''
import sys
import os
import shutil
cwd=os.path.dirname(__file__)
sys.path.append(f"{cwd}/../")
sys.path.append(f"{cwd}/../../")
from works.work import Work
from utils.config import Config
from works.opensource.download_source_by_manifest import DownloadSourceByMenifest


class WorkDownloadV3Component(Work):
    def run(self):
        if not self.config.get_flag_need_v3() == "NO_V3":
            self.logger.info(f"{self.config.board_name} no need v3 components.")
            return
        shutil.rmtree(f"{self.common.code_root}/../manifest", ignore_errors=True)
        ds = DownloadSourceByMenifest(self)
        ds.download("v3_component_manifest")


# 提供脚本单独调试功能，在application/build目录下运行python3 works/xxxx.py
if __name__ == "__main__":
    cfg = Config()
    wk = WorkDownloadV3Component(cfg)
    wk.run()
