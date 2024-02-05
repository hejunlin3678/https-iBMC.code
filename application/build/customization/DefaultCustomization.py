#!/usr/bin/env python
# coding:utf-8

"""
文件名：default.py
功能：打包目录形成
版权信息：华为技术有限公司，版本所有(C) 2020-2021
"""
import os
import sys
import argparse
import subprocess
import json
cwd=os.path.dirname(__file__)
sys.path.append(f"{cwd}/../")
sys.path.append(f"{cwd}/../../")
sys.path.append(f"{cwd}/../../../")
from works.work import Work
from utils.config import Config


class DefaultCustomization(object):
    def __init__(self, work, config):
        self.config = config
        self.work = work

    def rootfs_base_deploy(self, work_path: str):
        # app,pme,rtos压缩包整合
        pass

    def rootfs_base_conf(self, work_path: str):
        # 公共基础包配置
        pass

    def rootfs_ro_deploy(self, work_path: str):
        # 只读包部署
        pass

    def rootfs_rw_deploy(self, work_path: str):
        # 读写包部署
        pass

    def rootfs_dft_deploy(self, work_path: str):
        # 装备包部署
        pass

    def datafs_deploy(self, work_path: str):
        # 数据包部署
        pass

    def ro_cust_conf(self, work_path: str, **version_num):
        # 只读包自定义配置
        pass

    def rootfs_base_cust_deploy(self, work_path: str):
        # 组合包单板自定义配置
        pass

    def rootfs_base_cust_conf(self, work_path: str):
        # 公共包单板自定义配置
        pass

    def rw_cust_conf(self, work_path: str, **version_num):
        # 读写包自定义配置
        pass

    def dft_cust_conf(self, work_path: str):
        # 装备包自定义配置
        pass

    def redfish_conf(self, work_path: str):
        # redfish权限配置
        pass

    def profile_img_conf(self, work_path: str):
        pass

    def web_img_conf(self, work_path: str):
        pass

    def profile_web_img_packet(self, work_path: str, board_id):
        pass

    def app_package_conf(self, work_path: str):
        pass
