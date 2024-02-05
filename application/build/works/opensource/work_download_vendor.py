#!/usr/bin/python
# -*- coding: UTF-8 -*-

'''
功    能：
版权信息：华为技术有限公司，版本所有(C) 2021
修改记录：2021-10-11 创建
'''
import time
import subprocess
import sys
import os
import shutil
cwd=os.path.dirname(__file__)
sys.path.append(f"{cwd}/../")
sys.path.append(f"{cwd}/../../")
from works.work import Work
from works.opensource.download_source_by_manifest import DownloadSourceByMenifest
from utils.config import Config

class WorkDownloadVendor(Work):

    def cur_batch_rename_dir(self, cur_str, target_str):
        temp_list_dir = os.listdir(os.getcwd())
        for cur_name_dir in temp_list_dir:
            if os.path.isdir(f"{os.getcwd()}/{cur_name_dir}"):
                temp_name_dir = re.sub(cur_str, target_str, cur_name_dir)
                os.rename(cur_name_dir, temp_name_dir)
        return

    def check_dir(self, dir_name):
        """
        确认目录是否存在，存在则删除重建，不存在则创建
        """
        if os.path.isdir(dir_name):
            shutil.rmtree(dir_name)
            os.makedirs(dir_name)
        else:
            os.makedirs(dir_name)
        return

    def move_third_party_groupware(self):
        """
        移动第三方文件组
        """
        self.logger.info(f"jre dir: {self.common.code_root}/vendor/bin/kvm_client")

        self.check_dir(f"{self.common.code_root}/vendor/bin/kvm_client/jre_linux")
        self.check_dir(f"{self.common.code_root}/vendor/bin/kvm_client/jre_mac")
        self.check_dir(f"{self.common.code_root}/vendor/bin/kvm_client/jre_windows")

        os.chdir(f"{self.common.code_root}/vendor/bin/kvm_client/jre_linux")
        self.cur_batch_rename_dir("jdk8u222-b10-jre", "jre")

        os.chdir(f"{self.common.code_root}/vendor/bin/kvm_client/jre_mac")
        self.cur_batch_rename_dir("jdk8u222-b10-jre", "jre")

        os.chdir(f"{self.common.code_root}/vendor/bin/kvm_client/jre_windows")
        self.cur_batch_rename_dir("jdk8u222-b10-jre", "jre")

        self.logger.info("move thir_party_groupware from cmc done")
        return

    def move_cs4343_from_codehub(self):
        file_name_list = self.common.list_all_file("cs.*", f"{self.common.code_root}/vendor/CS4343_PHY_API/450940-API_CS4227_CS4223_CS4343_PHY_API_Release_3.7.8/CS4343_Release_3.7.8/api")
        file_name_list += self.common.list_all_file("cs.*", f"{self.common.code_root}/vendor/CS4343_PHY_API/450940-API_CS4227_CS4223_CS4343_PHY_API_Release_3.7.8/CS4343_Release_3.7.8/platform")

        self.common.remove_path(f"{self.common.code_root}/vendor/CS4343_PHY_API/cs4343_release/")
        os.mkdir(f"{self.common.code_root}/vendor/CS4343_PHY_API/cs4343_release")

        for file_name in file_name_list:
            shutil.copy(file_name, f"{self.common.code_root}/vendor/CS4343_PHY_API/cs4343_release/")

    def download_open_source_from_codehub(self):
        """
        下载开源软件opensource和第三方vendor
        下载后将文件移动至目标目录
        """
        # 将vender目录移动到指定位置
        if not self.config.from_source or self.config.open_local_debug:
            self.logger.info('download vendor from codehub begin...')
            ds = DownloadSourceByMenifest(self)
            ds.download("vendor_manifest")
            self.logger.info('download vendor from codehub end!')
        vendor=f"{self.common.code_root}/vendor"
        cmd = "rm -rf {vendor}/StoreLib/versionChangeSet/common_storelib_unf_rel"
        cmd = f"{cmd}/app_util/common_storelib_unf/src/common/opensource/zlib"
        self.run_command(cmd)
        self.move_third_party_groupware()
        self.move_cs4343_from_codehub()
        return True

    def run(self):
        self.download_open_source_from_codehub()

# 提供脚本单独调试功能，在application/build目录下运行python3 works/xxxx.py
if __name__ == "__main__":
    cfg = Config("2488hv6", "target_personal")
    wk = WorkDownloadVendor(cfg)
    wk.run()
