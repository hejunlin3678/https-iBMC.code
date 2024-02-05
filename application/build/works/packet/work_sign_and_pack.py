#!/usr/bin/env python
# coding: utf-8
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

cwd=os.path.dirname(__file__)
sys.path.append(f"{cwd}/../")
sys.path.append(f"{cwd}/../../")
sys.path.append(f"{cwd}/../../../")
from works.work import Work
from utils.common import Common
from utils.config import Config
from datetime import datetime
from utils.error_info import EXCEPTION_STAGE
from utils.error_info import EXCEPTION_RESPONSE
from utils.error_info import EXCEPTION_CODE

class WorkSignAndPack(Work):
    def set_env(self):
        self.g_sign = "-s"

    def sign_hpms(self):
        prefix_sign_flag = False
        platform_file_dic = {
            "V5": f"{self.common.code_root}/application/src/resource/pme_profile/platform.xml",
            "V6": f"{self.common.code_root}/application/src/resource/pme_profile/platform_v6.xml",
        }
        os.chdir(self.common.code_path)
        if self.g_sign != "":
            sign_type = self.common.get_sign_algorithm_type(self.config.board_name)
            sing_xml_filename = "sign_pss.xml" if sign_type == 'PSS' else "sign.xml"
            primary_sign_dep_file = f"{self.common.code_root}/{sing_xml_filename}"
            if os.path.exists(primary_sign_dep_file):
                self.logger.info("Signature file exist")
                shutil.copy(primary_sign_dep_file, self.config.build_path)
            else:
                self.logger.info("Signature file doesn't exist")
                self.logger.info(f"Please copy signature dependency file to\
                            {self.common.code_root}")
            self.logger.info(f"###################sign_type: {sign_type}, sing_xml_filename: {sing_xml_filename}")
            # 请勿挪动此语句位置(CI和本地构建均使用到)
            self.common.py_sed(f"{self.config.build_path}/{sing_xml_filename}","(?<=path=\").*(?=\")", self.config.work_out)
            os.chdir(self.config.build_path)
            self.logger.info("Signature online")
            online_signature_file = \
                "/usr/local/signature-jenkins-slave/jenkins-sign.xml"
            # CI要对这里的脚本进行替换，请勿随意换行！
            subprocess.run(f"java -jar /usr/local/signature-jenkins-slave/signature.jar {self.config.build_path}/{sing_xml_filename}", shell=True, check=True)
            # PSS需要手动更新crl文件
            if sign_type == 'PSS':
                self.common.copy_pss_crl(f"{self.config.work_out}/cms.crl")
            else:
                self.common.py_sed(f"{self.config.build_path}/{sing_xml_filename}","(?<=crlfile>)V2R2_trunk/", "")
                shutil.copy(f"{self.config.build_path}/signature/crl/crldata.crl",
                            f"{self.config.work_out}/cms.crl")
            shutil.copy(f"{self.common.code_path}/cms_sign_hpm.sh", self.config.work_out)
            os.chdir(self.config.work_out)
            subprocess.run("sh cms_sign_hpm.sh 2", shell=True)
            # 调试时删除防止误删文件
            self.common.check_path(self.common.work_out)
            self.common.check_path(self.common.output_path)
            # 移动文件到对应目录
            self.logger.info(f"copy compilation results to {self.common.work_out}")
            copy_file_list = self.common.py_find_dir(".*\.(img|hpm)", ".")
            for copy_file in copy_file_list:
                try:
                    shutil.copy(copy_file, self.common.work_out)
                    shutil.copy(copy_file, self.common.output_path)
                except:
                    pass


    def run(self):
        if not self.config.support_dft_separation and self.config.build_type == "dft_separation":
            return
        self.set_env()
        self.sign_hpms()

# 提供脚本单独调试功能，在application/build目录下运行python3 works/xxxx.py
if __name__ == "__main__":
    cfg = Config()
    cfg.set_debug(True)
    wk = WorkSignAndPack(cfg)
    wk.run()
    print("end here")
