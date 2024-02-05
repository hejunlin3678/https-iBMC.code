#!/usr/bin/env python3
# coding: utf-8
"""
文件名：rootfs_customization_BM320.py
功能：单板BM320独有配置
版权信息：华为技术有限公司，版本所有(C) 2020-2022
"""
import os
import sys

cwd = os.path.dirname(__file__)
sys.path.append(f"{cwd}/../../../")
from application.build.customization.rootfs_property_customization import rootfs_property_customization


class rootfs_customization_BM320(rootfs_property_customization):
    def rootfs_base_cust_deploy(self, work_path: str):
        super(rootfs_customization_BM320, self).rootfs_base_cust_deploy(work_path)
        self.hostsec_build(work_path)
        self.tsb_build(work_path)
        dst_file = f"{work_path}/opt/pme/conf/profile/platform_v6.xml"
        set_copyright = ''
        default_copyright = ''
        doc_support_flag = 0
        set_kvm_link = ''
        self.work.run_command(f'sed -i "/Copyright/s#V>.*</V#V>{set_copyright}</V#" {dst_file}')
        self.work.run_command(f'sed -i "/DefaultCopyright/s#V>.*</V#V>{default_copyright}</V#" {dst_file}')
        self.work.run_command(f'sed -i "/DocSupportFlag/s#V>.*</V#V>{doc_support_flag}</V#" {dst_file}')
        self.work.run_command(f'sed -i "/DownloadKVMLink/s#V>.*</V#V>{set_kvm_link}</V#" {dst_file}')
