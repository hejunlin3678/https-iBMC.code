#!/usr/bin/env python
# coding: utf-8
# Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
"""
功 能：buildimg_ext4脚本，该脚本 make ext4 image
版权信息：华为技术有限公司，版本所有(C) 2019-2020
"""

import os
import shutil
import subprocess
import sys
import time
import json
from pathlib import Path
from datetime import datetime
cwd = os.path.dirname(__file__)
sys.path.append(f"{cwd}/../")
sys.path.append(f"{cwd}/../../")
sys.path.append(f"{cwd}/../../../")
from works.packet.work_sign_and_pack import WorkSignAndPack
from utils.common import Common
from utils.config import Config
from utils.error_info import EXCEPTION_STAGE
from utils.error_info import EXCEPTION_RESPONSE
from utils.error_info import EXCEPTION_CODE


class WorkSignAndPack(WorkSignAndPack):
    def sign_hpms(self):
        os.chdir(self.config.work_out)
        self.logger.info("Use virtual signature file!")
        virtual_cms_file = f"{self.common.code_root}/application/sign/hpm.filelist.cms"
        virtual_crl_file = f"{self.common.code_root}/application/sign/cms.crl"
        # 复制下载的crldata.crl文件。
        self.common.copy_pss_crl(virtual_crl_file)
        filelist_list = self.common.py_find_dir(".*\.filelist", ".")
        for filelist in filelist_list:
            shutil.copy(virtual_cms_file, f"{filelist}.cms")
        shutil.copy(virtual_crl_file, "cms.crl")
        shutil.copy(f"{self.common.code_path}/cms_sign_hpm.sh", self.config.work_out)
        os.chdir(self.config.work_out)
        self.run_command(f"sh {self.config.work_out}/cms_sign_hpm.sh 2")
        # 调试时删除防止误删文件
        self.common.check_path(self.common.work_out)
        self.common.check_path(self.common.output_path)
        # 移动文件到对应目录
        self.logger.info("copy compilation results to %s", self.common.work_out)
        copy_file_list = self.common.py_find_dir(".*\.(img|hpm)", ".")
        for copy_file in copy_file_list:
            try:
                shutil.copy(copy_file, self.common.work_out)
                shutil.copy(copy_file, self.common.output_path)
            except Exception as e:
                self.logger.error("copy compilation results error")
                raise e
        # 清理 work_out 目录下src文件
        if os.path.exists(f"{self.common.work_out}/src"):
            shutil.rmtree(f"{self.common.work_out}/src")

# 提供脚本单独调试功能，在application/build目录下运行python3 works/xxxx.py
if __name__ == "__main__":
    cfg = Config()
    cfg.set_debug(True)
    wk = WorkSignAndPack(cfg)
    wk.run()
