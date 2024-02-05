#!/usr/bin/python
# -*- coding: UTF-8 -*-

'''
功    能：
版权信息：华为技术有限公司，版本所有(C) 2021
修改记录：2021-10-14 创建
'''
import sys
import os
import json
import subprocess
cwd=os.path.dirname(__file__)
sys.path.append(f"{cwd}/../")
sys.path.append(f"{cwd}/../../")
from works.work import Work
from utils.config import Config
from utils.common import Common
from utils.log import Logger

class WorkHostPrepare(Work):

    def run(self):
        # 修改属主， 用非 root 构建
        pass

# 以下为提供脚本单独调试功能示例代码，在application/build目录下运行python3 works/xxxx.py
if __name__ == "__main__":
    cfg = Config()
    wk = WorkHostPrepare(cfg)
    wk.run()