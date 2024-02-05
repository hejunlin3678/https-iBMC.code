#!/usr/bin/env python
# coding: utf-8
# Copyright © Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.

import os

from utils.common import Common
from utils.log import Logger
from utils.error_info import EXCEPTION_STAGE
from utils.error_info import EXCEPTION_RESPONSE
from utils.error_info import EXCEPTION_CODE

logger = Logger().logger
class CheckFileList:
    """
    功能描述: check file about security
    接口：无
    """
    g_black_list = ["bin/busybox310",
                    "usr/bin/telnet",
                    "conf/smbios/smbios.bin",
                    "var/spool/lpd",
                    "var/spool/mail",
                    "etc/pam.d/su",
                    "usr/bin/su",
                    "etc/pam.d/openwsman",
                    "etc/pam.d/su-l"]

    g_white_list = ["fsync",
                    "maint_debug_mod",
                    "maint_debug_mod2",
                    "maint_debug_mod3"]

    g_common = Common()

    def dl_black_list(self, path):
        """
        功能描述：删除指定路径下不该存在的文件
        参数：path 路径
        返回值：无
        """
        for file in self.g_black_list:
            logger.info(f"{path}/{file}")
            self.g_common.remove_path(os.path.join(path, file))

    def is_in_white_list(self, input_file):
        """
        功能描述：判断文件是否该存在
        参数：input_file 待判断的文件
        返回值：True or False
        """
        return input_file in self.g_white_list
