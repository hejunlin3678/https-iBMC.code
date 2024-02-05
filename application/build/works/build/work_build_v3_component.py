#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
# This script build v3 component
# Copyright © Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.

import os
import traceback
import sys
import subprocess
import argparse
import shutil
cwd=os.path.dirname(__file__)
sys.path.append(f"{cwd}/../")
sys.path.append(f"{cwd}/../../")
from works.work import Work
from utils.common import Common
from utils.config import Config
from utils.log import Logger

class WorkV3ComponentBuild(Work):
    timeout = 600

    def run(self):
        try:
            # 由于PYTHONPATH的存在，V2与V3构建会存在类型问题，在此弹出PYTHONPATH
            if os.environ.get("PYTHONPATH", "") != "":
                os.environ.pop("PYTHONPATH")
            os.chdir(f"{self.common.manifest_path}/application/build")
            self.config.set_env_global()
            subprocess.run("conan remove --locks", shell=True)
            if self.config.get_flag_need_v3() == "V3_MACA":
                shutil.copy(f"{self.common.code_root}/application/src/resource/pme_profile/class.xml",
                            f"{self.common.manifest_path}/conan/dfmlite/class.xml")
                self.common.logger.info("build v3 component without pme start")
                subprocess.run("python frame.py -t target_master_app -b TaiShan2280v2_1711", shell=True)
                self.common.logger.info("build v3 component without pme end")
            # 暂且只支持两块单板，TaiShan2280v2_1711和BM320，如有需要，请去v3仓增加单板
            elif self.config.get_flag_need_v3() == "V3_BOARD":
                if self.common.revision.startswith("auto-"):
                    subprocess.run("python3 frame.py -t target_tianchi_app -b v3_TaiShan2280v2_1711 -v {self.common.revision}", shell=True)
                else:
                    subprocess.run("python3 frame.py -t target_tianchi_app -b v3_TaiShan2280v2_1711", shell=True)
            else:
                pass
        except Exception as e:
            raise Exception(f"V3 build error, {e}, {traceback.print_exc()}")
        pass


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="build target")
    parser.add_argument("-b", "--board_name", help="Board name", default="2488hv6")
    args = parser.parse_args()
    cfg = Config(board_name=args.board_name)
    wk = WorkV3ComponentBuild(cfg)
    wk.run()
