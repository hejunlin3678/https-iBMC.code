#!/usr/bin/env python
# coding=utf-8

"""
文件名：build_one_click.py
功能：集中调用构建脚本，一键式构建
版权信息：华为技术有限公司，版本所有(C) 2019-2020
"""
import os
import time
import argparse
import subprocess
from utils.log import Logger
from utils.common import Common

# 参数配置
parser = argparse.ArgumentParser(description="build_one_click.py")
parser.add_argument("-b", "--board_name", help="Board list must in '' and separated by space")
parser.add_argument("-f", "--from_source", help="From source required",
                    const="-f", action="store_const", default="")
parser.add_argument("-s", "--sign", help="Signature required",
                    const="-s", action="store_const", default="")
parser.add_argument("-n", "--no_packet", help="Packer required",
                    action="store_true")
parser.add_argument("-p", "--partner", help="partner mode", action="store_true", default=False)
parser.add_argument("-v3", "--v3_component", help="v3 component convergence", action="store_true", default=False)
parser.add_argument("--enable_arm_gcov", help="enable arm gcov for test", action="store_true", default=False)
parser.add_argument("-t", "--target_name", help=" target_name", default="target_personal")
args = parser.parse_args()


# 声明全局变量
global g_common
# 一个或多个板子的字符串
global g_board_name
global g_board_list
global g_board_version
global g_from_source
global g_sign
global g_no_packet
# 是否有预置签名文件
global prefix_sign_flag
global g_thread_num
global g_arch_dic
global g_arm_dic
# 合作伙伴模式
global g_partner_mode

logger = Logger().logger

def set_env():
    global g_common
    global g_board_name
    global g_board_list
    global g_board_version
    global g_from_source
    global g_sign
    global g_no_packet
    global prefix_sign_flag
    global g_thread_num
    global g_arch_dic
    global g_arm_dic
    global g_partner_mode
    global g_partner_config_file
    global g_convergence_v3
    # 参数读取
    g_common = Common()
    g_board_name = args.board_name
    if ' ' in g_board_name:
        g_board_list = g_board_name.split(' ')
    else:
        g_board_list = [g_board_name]
    g_board_version = g_common.get_board_version(g_board_list[0])
    g_from_source = args.from_source
    g_sign = args.sign
    g_no_packet = args.no_packet
    g_partner_mode = args.partner
    prefix_sign_flag = False

    g_thread_num = 4
    g_convergence_v3 = args.v3_component
    g_common.create_log_content()
    g_arch_dic = {"V3": "arch32", "V5": "arch32", "V6": "arch64"}
    g_arm_dic = {"V3": "arm", "V5": "arm", "V6": "arm64"}
    g_partner_config_file = f"{g_common.code_root}/application/src/resource/board/{g_board_list[0]}/archive_emmc_nand/partner_signature.ini"
    return

def debug_log():
    logger.info(f"g_common = {g_common}")
    logger.info(f"g_board_name = {g_board_name}")
    logger.info(f"g_board_list = {g_board_list}")
    logger.info(f"g_board_version = {g_board_version}")
    logger.info(f"g_from_source = {g_from_source}")
    logger.info(f"g_sign = {g_sign}")
    logger.info(f"g_no_packet = {g_no_packet}")
    logger.info(f"prefix_sign_flag = {prefix_sign_flag}")
    logger.info(f"g_thread_num = {g_thread_num}")
    logger.info(f"arch = {g_arch_dic.get(g_board_version)}")
    logger.info(f"arm = {g_arm_dic.get(g_board_version)}")
    logger.info(f"partner = {g_partner_mode}")
    logger.info(f"target_name = {args.target_name}")
    logger.info(f"sign_pkcs_board_list = {g_common.sign_pkcs_board_list}")
    logger.info(f"sign_pss_board_list = {g_common.sign_pss_board_list}")
    logger.info(f"crl_path = {g_common.crl_path}")
    return

def file_clean():
    # To Do
    return

if __name__ == "__main__":
    """
    在没有明确说明目录变动的情况下，当前目录均在/V2R2_trunk/application/build下
    """
    subprocess.run("pip3 install --trusted-host cmc-cd-mirror.rnd.huawei.com -i https://cmc-cd-mirror.rnd.huawei.com/pypi/simple MarkupSafe==2.0.1", shell=True)
    subprocess.run("pip3 install --trusted-host cmc-cd-mirror.rnd.huawei.com -i https://cmc-cd-mirror.rnd.huawei.com/pypi/simple conan==1.42.1", shell=True)
    subprocess.run("pip3 install numpy", shell=True)
    set_env()
    debug_log()
    if not g_no_packet:
        # 版本构建工程
        if g_from_source == "-f":
            logger.info("python3 frame.py -t target_from_source")
            ret = subprocess.run(f"python3 frame.py -t target_from_source -b {args.board_name} -f", shell=True)
        else:
            logger.info("python3 frame.py -t target_version_compile")
            ret = subprocess.run(f"python3 frame.py -t target_version_compile -b {args.board_name}", shell=True)
        if ret.returncode:
            raise Exception
        g_common.copy_all(f"{g_common.work_out}/packet", f"{g_common.output_path}/packet")
    else:
        # 个人构建工程
        ret = subprocess.run(f"python3 frame.py -t {args.target_name} -b {args.board_name} {g_from_source}", shell=True)
        if ret.returncode:
            raise Exception
        g_common.copy_all(f"{g_common.work_out}/packet", f"{g_common.output_path}/packet")
    os._exit(0)