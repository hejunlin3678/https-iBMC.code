#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
功能：日志模块等
版权信息：Copyright Huawei Technologies Co., Ltd. 2010-2022. All rights reserved.
"""
import logging
import sys
import os


class Logger(object):
    def __init__(self, log_name="", file_name=""):
        if log_name == "":
            log_name = sys._getframe().f_back.f_code.co_filename.rsplit('.', 1)[0]
        logger = logging.getLogger(log_name)
        logger.setLevel(logging.DEBUG)
        formatter = logging.Formatter(
            fmt=' [{levelname} {filename}:{lineno} {funcName:4}] {message}',
            style='{')
        if file_name != "":
            if os.path.exists(file_name):
                os.remove(file_name)
            handler = logging.FileHandler(file_name, mode='a', encoding=None, delay=False)
        else:
            handler = logging.StreamHandler(sys.stdout)
        handler.setFormatter(formatter)
        logger.addHandler(handler)
        self.logger = logger
        self.__disable_print()

    def __disable_print(self):
        global print
        self.print_fun = print

        def fake_print(*args, **kwargs):
            raise UserWarning(
                "please use `self.logger.info` instead of `print`")

        print = fake_print
        return self