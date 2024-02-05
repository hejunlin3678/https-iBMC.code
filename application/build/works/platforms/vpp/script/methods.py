#!/usr/bin/env python
# -*- coding: utf-8 -*-
# version_string 1.1.0
#注释：此脚本中提供常用的小功能函数。
# Copyright Huawei Technologies Co., Ltd. 2010-2021. All rights reserved.

import logging
import os
import platform
import shutil
import stat
import sys
import traceback
from fnmatch import fnmatchcase
from multiprocessing import Pool
from subprocess import PIPE, STDOUT, Popen

logger = logging.getLogger()


def rm_dir(dir_name):
    def _remove_readonly(func, path, _):
        '''Clear the readonly bit and reattempt the removal'''
        os.chmod(path, stat.S_IWRITE)
        func(path)

    if os.path.isdir(dir_name):
        shutil.rmtree(dir_name, onerror=_remove_readonly)


def exec_shell(commond, log_file='', cwd=None):
    if isinstance(commond, list):
        cmd = ' '.join(commond)
    else:
        cmd = commond
    logger.info('{}'.format(cmd))
    if os.name == 'posix':
        p_args = ['/bin/bash', '-c', cmd]
    else:
        p_args = '{} /C "{}"'.format(os.environ.get('COMSPEC'), cmd)
    cmd_out = Popen(p_args,
                    cwd=cwd,
                    stdout=PIPE,
                    stderr=STDOUT,
                    close_fds=False)
    err_flags = [
        'error:', 'Error ', 'warnings being treated as errors',
        'undefined reference to'
    ]
    find_err = 0
    while cmd_out.poll() is None:
        line_out = cmd_out.stdout.readline()
        line_out = line_out.decode(encoding='utf-8', errors='ignore').strip()
        if line_out:
            if any(err in line_out for err in err_flags):
                find_err = 3
            if find_err > 0:
                logger.error(line_out)
                find_err -= 1
            else:
                logger.info(line_out)
    if cmd_out.returncode != 0 and log_file != '':
        logger.error('For details Error logs, see {}'.format(log_file))
    return cmd_out.returncode


def add_env_path(env_path=''):
    connector = ';' if sys.platform.startswith('win') else ':'
    if env_path:
        value = '{}{}{}'.format(env_path, connector, os.environ.get("PATH"))
        os.environ["PATH"] = value


def set_version_environ(version_dict):
    for key, value in version_dict.items():
        if value:
            os.environ[key.upper()] = value
        else:
            logger.error('{} must be set'.format(key))
            return -1
    return 0


def is_exclude_out(name, excludes=None):
    # 检查文件是否在排除列表中, 支持Unix shell风格的通配符, 大小写敏感
    if excludes is None or not excludes:
        return False
    for pattern in excludes:
        if fnmatchcase(name, pattern):
            return True
    return False


def is_filter_out(name, filters=None):
    # 检查文件是否被筛选掉, 支持Unix shell风格的通配符, 大小写敏感
    # 默认包含所有文件, 即未被筛选掉
    if filters is None or not filters:
        return False
    for pattern in filters:
        if fnmatchcase(name, pattern):
            return False
    return True


def get_file_list(src_dir, filters=None, excludes=None):
    '''
    获取给定目录下的所有文件全路径列表. 支持通配符, 大小写敏感.
    filter 指定要包含的文件, 默认包含所有文件.
    exclude 指定要排除的文件夹或文件列表.
    '''
    filelist = []
    for root, dirs, files in os.walk(src_dir):
        # 如果文件夹名称在排除列表中, 不遍历文件夹和其子文件夹
        dir_name = os.path.basename(root)
        if is_exclude_out(dir_name, excludes):
            dirs[:] = []
            files[:] = []
        for file_name in files:
            if is_exclude_out(file_name, excludes):
                continue
            if is_filter_out(file_name, filters):
                continue
            filelist.append(os.path.join(root, file_name))
    return filelist


def cover_file(file_src, file_dest):
    try:
        dest_dir = os.path.dirname(file_dest)
        if not os.path.isdir(dest_dir):
            os.makedirs(dest_dir, exist_ok=True)
        if os.path.isfile(file_dest):
            os.remove(file_dest)
        shutil.copy2(file_src, file_dest)
        ret = 0
    except Exception:
        traceback.print_exc()
        ret = -1
    finally:
        pass
    return ret


def covertree(src_dir, dest_dir, excludes=None):
    #调用多线程拷贝目录文件
    if not os.path.isdir(src_dir):
        logger.error('covertree src not exist: {}'.format(src_dir))
        return -1

    # 获取要拷贝的文件列表
    filelist = get_file_list(src_dir, excludes=excludes)
    if not filelist:
        logger.info('covertree nothing need to copy')
        return 0

    results = []
    for file_src in filelist:
        file_src_rel_path = os.path.relpath(file_src, src_dir)
        file_dest = os.path.realpath(os.path.join(dest_dir, file_src_rel_path))
        cover_file(file_src, file_dest)

    return 0
