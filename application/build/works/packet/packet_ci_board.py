#!/usr/bin/env python
# coding: utf-8
"""
功 能：packet_ci_board，该脚本 发布件打包，包括 supportE 以及 GDP
版权信息：华为技术有限公司，版本所有(C) 2019-2020
"""

import os
import shutil
import subprocess
import sys

cwd=os.path.dirname(__file__)
sys.path.append(f"{cwd}/../")
sys.path.append(f"{cwd}/../../")
sys.path.append(f"{cwd}/../../../")
from utils.common import Common
from utils.log import Logger
from utils.error_info import EXCEPTION_STAGE
from utils.error_info import EXCEPTION_RESPONSE
from utils.error_info import EXCEPTION_CODE
from works.work import Work
from works.packet.buildpkg_ext4 import PacketPkgExt4

class WorkPacketCIBoard(Work):
    def __init__(self, config, common, logger):
        self.config = config
        self.common = common
        self.logger = logger


    def build_pack(self):
        board = self.config.board_name
        self.logger.info(f"Current pack board name: {board}")
        os.chdir( self.common.code_path)
        ro_hpm_prefix = f"{self.config.work_out}/rootfs_{board}"
        ro_hpm_temp = f"{self.config.work_out}/rootfs_{board}_temp.hpm"
        resource_dir = os.path.join( self.common.code_root, 'application/src/resource')

        pkt_ext4 = PacketPkgExt4(self.config, self.common, self.logger)
        pkt_ext4.run()

        os.chdir(f"{self.common.code_path}/works/packet")
        ret = subprocess.run(f"python3 -B packet.py -b {board}", shell=True)
        if ret.returncode:
            err_msg = f"{EXCEPTION_CODE[610]}{EXCEPTION_STAGE['C']}{EXCEPTION_RESPONSE[2]}"
            self.logger.error(f"{err_msg} packet error!!!")
            os._exit(1)

    def run(self):
        self.build_pack()
