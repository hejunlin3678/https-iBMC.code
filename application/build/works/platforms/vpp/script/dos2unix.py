#!/usr/bin/env python
# -*- coding: utf-8 -*-
# version_string 1.0.19
#注释：此脚本执行dos2unix, 进行格式转换。
# Copyright Huawei Technologies Co., Ltd. 2010-2020. All rights reserved.

import logging
import os
import platform
import re
import sys
import traceback
from fnmatch import fnmatchcase
from multiprocessing import Pool
import loggering
from methods import get_file_list, exec_shell

sys.dont_write_bytecode = True

logger = logging.getLogger()


def dos2unix_by_code(file_name):
    try:
        ret_code = 0
        text = None
        # 读取文件数据
        with open(file_name, 'rb') as f:
            text = f.read()
        # 替换文件换行符
        while (b'\r\n' in text):
            text = text.replace(b'\r\n', b'\n')
        # 覆盖写入文件
        if platform.system() == 'Windows':
            flags = os.O_WRONLY | os.O_CREAT | os.O_BINARY
        else:
            flags = os.O_WRONLY | os.O_CREAT
        mode = 0o644
        os.remove(file_name)
        with os.fdopen(os.open(file_name, flags, mode), 'wb') as f:
            f.write(text)
    except Exception:
        traceback.print_exc()
        ret_code = -1
    finally:
        pass
    return ret_code


def dos2unix_by_cmd(file_name):
    # 存在工具时使用工具, 不存在时直接修改文件
    ret_code = exec_shell(['dos2unix', file_name])
    return ret_code


def dos2unix(name, filters=None):
    # 检查dos2unix工具是否存在
    if platform.system() == 'Windows':
        exist_dos2unix = not bool(exec_shell(['where', 'dos2unix.exe']))
    else:
        exist_dos2unix = not bool(exec_shell(['command', '-v', 'dos2unix']))
    # dos2unix工具存在时直接调用工具, 否则直接修改文件编码
    dos2unix_func = dos2unix_by_cmd if exist_dos2unix else dos2unix_by_code

    #多线程dos2unix指定目录下的文件
    filelist = []
    if os.path.isdir(name):
        filelist = get_file_list(name, filters=filters)
    elif os.path.isfile(name):
        filelist.append(name)
    else:
        logger.error('parameter error')
        return -1
    # 获取文件列表
    if not filelist:
        logger.info('no file need to dos2unix')
        return 0

    results = []
    for file_src in filelist:
        if not exist_dos2unix:
            logger.info('dos2unix_bycode: converting file {}'.format(file_src))
        dos2unix_func(file_src, )
    return 0


def main():
    global logger
    logger = loggering.get_logger()
    if len(sys.argv) < 2:
        logger.info('Usage: dos2unix.py <dirname>')
        return -1
    dir_name = os.path.realpath(sys.argv[1])
    filters = ['*.h', '*.x', '*.in']
    ret_code = dos2unix(dir_name, filters)
    return ret_code


if __name__ == "__main__":
    sys.exit(main())
