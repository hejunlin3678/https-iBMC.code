#!/usr/bin/env python3
# coding: utf-8
"""
文件名：rootfs_customization_TaiShan2280Mv2.py
功能：单板TaiShan2280Mv2独有配置
版权信息：华为技术有限公司，版本所有(C) 2020-2022
"""
import os
import sys

cwd = os.path.dirname(__file__)
sys.path.append(f"{cwd}/../../../")
from application.build.customization.rootfs_property_customization import rootfs_property_customization


class rootfs_customization_TaiShan2280Mv2(rootfs_property_customization):
    def rootfs_base_cust_deploy(self, work_path: str):
        super(rootfs_customization_TaiShan2280Mv2, self).rootfs_base_cust_deploy(work_path)
        self.hostsec_build(work_path)
