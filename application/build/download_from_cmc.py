#!/usr/bin/env python
# coding: utf-8

"""
功 能：download_from_cmc类，该类主要涉及将 CMC上与BMC配套的二进制组件 binIBMC 下载到本地代码库中复位
据信息树配置文件下载配套软件包，平台文件
版权信息：华为技术有限公司，版本所有(C) 2019-2020
"""

import os
import subprocess
import argparse
import stat
import shutil
from utils.common import Common

from concurrent.futures import ThreadPoolExecutor
from utils.log import Logger
from utils.error_info import EXCEPTION_STAGE
from utils.error_info import EXCEPTION_RESPONSE
from utils.error_info import EXCEPTION_CODE
from works.dependency.parse_to_xml import DependencyXmlParse
from works.artget_code_down import ArtgetDownload
logger = Logger().logger

parser = argparse.ArgumentParser(description="Excute download_from_cmc.py")
parser.add_argument("-n", "--npm", help="only down npm open_source for firstboot", action='store_true')
parser.add_argument("-b", "--board_name", help="Fill in board_name", default="")
parser.add_argument("-npk", "--no_packet", help="Packer required", action="store_true")
parser.add_argument("-l", "--llt", action="store_true", help="LLT mode")
parser.add_argument("-o", "--open_source", action="store_true", help="open_source mode")
args = parser.parse_args()


def get_path(path):
    return os.path.realpath(os.path.join(current_script_path, path))


def set_path():
    global g_common
    global pre_flag
    global g_thread_num
    global g_llt_mode
    g_common = Common()
    g_thread_num = 3
    g_common.create_log_content()
    g_llt_mode = args.llt

    global g_board_name
    global g_no_packet
    g_board_name = args.board_name
    g_no_packet = args.no_packet
    if g_board_name == "":
        logger.info("no board input")
        g_board_name = 'TaiShan2280v2'

    logger.info(f"g_board_name : {g_board_name}")
    global current_script_path
    current_script_path = g_common.code_path

    pre_flag = False
    if os.path.exists(f"{g_common.code_root}/prefix_language_ja"):
        pre_flag = True


def clear_env():
    # 清理上次构建遗留的 RTOS 文件
    logger.info('clear_env begin...')

    g_common.remove_path(os.path.join(g_common.temp_path, "build_tools/rtos-sdk"))
    g_common.remove_path(os.path.join(g_common.temp_path, "build_tools/rtos-sdk-arm64"))
    g_common.remove_path(os.path.join(g_common.temp_path, "tools/build_tools/rtos-sdk"))
    g_common.remove_path(os.path.join(g_common.temp_path, "tools/build_tools/rtos-sdk-arm64"))

    # 清理上次下载的中间二进制库目录
    g_common.remove_path(os.path.join(g_common.temp_path, "bin_cmc"))
    g_common.remove_path(os.path.join(g_common.temp_path, "open_source"))
    g_common.remove_path(os.path.join(g_common.temp_path, "build_tools"))
    g_common.remove_path(os.path.join(g_common.temp_path, "platforms"))
    g_common.remove_path(os.path.join(g_common.temp_path, "jardown_cmc"))
    g_common.remove_path(os.path.join(g_common.temp_path, "opendown_cmc"))

    remove_dir_tuple = ("build_tools", "platforms", "bin_cmc", "jardown_cmc", "opendown_cmc")
    for dir_name in remove_dir_tuple:
        subprocess.run(f"LD_PRELOAD= sudo LD_PRELOAD=${{LD_PRELOAD}} -E rm -rf {g_common.temp_path}/{dir_name}/*",
                       shell=True)

    logger.info('clear_env end!')




def download_opensource():
    logger.info('download opensource codes begin...')

    dependency_file = get_path("dependency_for_hpm/downxml/dependency_opensourcedown.xml")
    os.chdir(current_script_path)
    logname = f"{g_common.log_manage_dic.get('download_log')}/download_opensource.log"
    if not os.path.exists(f"{g_common.temp_path}"):
        os.mkdir(f"{g_common.temp_path}")
    shcmd = f"artget pull -os {dependency_file} -at opensource -ap {g_common.temp_path} >{logname} 2>&1"
    subprocess.run(shcmd, shell=True, check=True)
    logger.info('download opensource codes end!')


def get_oem_buildtools(dependency_file):
    logger.info('get OEM buildtools begin...')
    # 支持OEM 定制化修改, 使用build_tree里面的dependency
    oem_dependency_file = os.path.join(g_common.oem_config_path, "dependency_buildtools.xml")
    if os.path.exists(oem_dependency_file):
        dependency_file = os.path.realpath(oem_dependency_file)
        logger.info('use OEM buildtools config...')

    subprocess.run(f"cat {dependency_file}", shell=True)
    return dependency_file


def download_dependency():
    logger.info('download dependency tree begin...')
    agentpath = os.path.join(g_common.temp_path, "bin_cmc")
    remove_dir_tuple = ("build_tools", "platforms", "bin_cmc", "jardown_cmc", "opendown_cmc")
    for dir_name in remove_dir_tuple:
        subprocess.run(f"LD_PRELOAD= sudo LD_PRELOAD=${{LD_PRELOAD}} -E rm -rf {g_common.temp_path}/{dir_name}/*",
                       shell=True)
    dependency_file = get_path("dependency_for_hpm/downxml/board/" + g_board_name + "_dependency.xml")
    subprocess.run(f"cat {dependency_file}", shell=True)
    logname = f"{g_common.log_manage_dic.get('download_log')}/download_board_package.log"
    cmd = f"artget pull -d {dependency_file} -ap {agentpath}"
    subprocess.run(cmd, shell=True)
    logger.info('download dependency tree end!')

def download_tools():
    logger.info('download dependency tools begin...')
    agentpath = os.path.join(g_common.temp_path, "bin_cmc")
    g_common.check_path(agentpath)
    logname = f"{g_common.log_manage_dic.get('download_log')}/download_buildtools.log"
    dependency_file = get_path("dependency_for_hpm/downxml/dependency_buildtools.xml")
    dependency_file = get_oem_buildtools(dependency_file)
    cmd = f"artget pull -d {dependency_file} -ap {agentpath} >{logname} 2>&1"
    subprocess.run(cmd, shell=True)
    os.chdir(current_script_path)
    logger.info('download dependency tree end!')


def download_open_source_from_codehub():
    logger.info('download open_source from codehub begin...')

    logname = f"{g_common.log_manage_dic.get('download_log')}/download_from_codehub.log"
    subprocess.run(f"sh download_from_codehub.sh > {logname} 2>&1 ", shell=True)
    log_check_list = g_common.file_find_text("^autogen.sh:", logname)
    if len(log_check_list) == 0:
        logger.error(f"{EXCEPTION_CODE[503]}{EXCEPTION_STAGE['D']}{EXCEPTION_RESPONSE[2]}下载异常详见{logname},退出!")
    logger.info('download open_source from codehub end!')


def download_kmc_from_cmc():
    artget_download = ArtgetDownload()
    kmc_dependency_file = get_path("works/platforms/kmc/srcbaseline.xml")
    logname = f"{g_common.log_manage_dic.get('download_log')}/download_kmc_cbb_codexml.log"

    logger.info('get kmc xml conf by artget begin...')
    g_common.remove_path(f"{g_common.temp_path}/kmc")
    kmc_cmd = f"artget pull -sc {kmc_dependency_file} -ap {g_common.temp_path}/kmc >{logname} 2>&1"
    subprocess.run(kmc_cmd, shell=True)
    logger.info('get kmc xml conf by artget end!')
    for file in os.listdir(f"{g_common.temp_path}/kmc"):
        if "source" in file:
            logger.info('get kmc source code begin...')
            artget_download.download(f"{g_common.temp_path}/kmc/{file}", f"{g_common.temp_path}/kmc","KMC_Java","")
            g_common.run_command(f"find {g_common.temp_path}/kmc/ -type f | xargs dos2unix")
            if os.path.isdir(f"{g_common.temp_path}/kmc/src/kmc"):
                g_common.remove_path(f"{g_common.code_root}/platforms/kmc")
                g_common.copy_all(f"{g_common.temp_path}/kmc/src/kmc", f"{g_common.code_root}/platforms/kmc")
                os.chdir(f"{g_common.code_path}")
                subprocess.run("sh patch_for_platforms.sh", shell=True)
            logger.info('get kmc source code end!')


def download_cbb_from_cmc():
    artget_download = ArtgetDownload()
    cbb_dependency_file = get_path("works/platforms/cbb/srcbaseline.xml")
    logname = f"{g_common.log_manage_dic.get('download_log')}/download_kmc_cbb_codexml.log"

    logger.info('get cbb xml conf by artget begin...')
    g_common.remove_path(f"{g_common.temp_path}/cbb")
    cbb_cmd = f"artget pull -sc {cbb_dependency_file} -ap {g_common.temp_path}/cbb >>{logname} 2>&1"
    subprocess.run(cbb_cmd, shell=True)
    logger.info('get cbb xml conf by artget end!')
    for file in os.listdir(f"{g_common.temp_path}/cbb"):
        if "source" in file:
            logger.info('get cbb source code begin...')
            artget_download.download(f"{g_common.temp_path}/cbb/{file}", f"{g_common.temp_path}/cbb","CMS-JAVA","")
            g_common.run_command(f"find {g_common.temp_path}/cbb/ -type f | xargs dos2unix")
            if os.path.isdir(f"{g_common.temp_path}/cbb/CMSCBB_SourceCodeList/CMS-C"):
                g_common.remove_path(f"{g_common.code_root}/platforms/cms_vrf_cbb")
                g_common.copy_all(f"{g_common.temp_path}/cbb/CMSCBB_SourceCodeList/CMS-C", f"{g_common.code_root}/platforms/cms_vrf_cbb")
            logger.info('get cbb source code end!')


def download_xml_parse():
    logger.info('parse_to_xml begin...')
    logname = f"{g_common.log_manage_dic.get('download_log')}/parse_to_xml.log"
    if g_llt_mode == True:
        xml = DependencyXmlParse(g_board_name, True, g_common)
    elif g_no_packet:
        g_common.run_command(f"python3 parse_to_xml.py -b {g_board_name} -npk > {logname} 2>&1")
    else:
        xml = DependencyXmlParse(g_board_name, False, g_common)
    xml.parse()
    logger.info('parse_to_xml end...')


def download_npm_open_source():
    logger.info('download npm open_source begin...')

    open_source = os.path.join(g_common.code_root, "open_source")
    node_modules = os.path.join(open_source, "node_modules")

    g_common.remove_path(f"{g_common.temp_path}/tiny_temp")
    g_common.copy_all(f"{g_common.code_root}/platforms/webapp", f"{g_common.temp_path}/tiny_temp/webapp")
    os.chdir(f"{g_common.temp_path}/tiny_temp/webapp")

    shutil.copy(f"{g_common.temp_path}/tiny_temp/webapp/package.json", open_source)
    shutil.copy(f"{g_common.temp_path}/tiny_temp/webapp/package-lock.json", open_source)
    logname = f"{g_common.log_manage_dic.get('npm_log')}/npm_prefix.log"
    g_common.run_command(f"npm cache clean -f")
    g_common.run_command(f"echo 'y' | npm ci --prefix {open_source} &> {logname}")

    # special文件是需要特殊处理的
    node_modules_files = ["web-animations-js", "css-vars-ponyfill", "echarts",
                          "rxjs", "tslib", "zone.js", "crypto-js", "zrender", "ngx-echarts",
                          "balanced-match", "get-css-data"]
    special_files = ["@ngx-translate/core", "@ngx-translate/http-loader"]

    # 将开源软件从node_modules中移动出来
    for file in node_modules_files:
        g_common.remove_path(os.path.join(open_source, file))
        g_common.copy_all(os.path.join(node_modules, file), os.path.join(open_source, file))

    # 特殊处理angular
    g_common.remove_path(os.path.join(open_source, "angular"))
    angular_files = ("router", "platform-browser-dynamic", "platform-browser",
                     "forms", "core", "compiler", "common", "animations")
    for file in angular_files:
        origin_file_path = os.path.join(node_modules, "@angular")
        target_file_path = os.path.join(open_source, "angular")
        g_common.copy_all(os.path.join(origin_file_path, file), os.path.join(target_file_path, file))

    for i in range(len(special_files)):
        real_file = special_files[i].split('/')[-1]
        # core文件需要改名为ngx-translate-core
        if i == 0:
            real_file = f"ngx-translate-{real_file}"
        g_common.remove_path(os.path.join(open_source, real_file))
        g_common.copy_all(os.path.join(node_modules, special_files[i]), os.path.join(open_source, real_file))

    g_common.remove_path(node_modules)
    os.remove(f"{open_source}/package.json")
    os.remove(f"{open_source}/package-lock.json")
    logger.info('download npm open_source end!')
    os.chdir(current_script_path)


def download_v3_component():
    ret = subprocess.run(f"python3 {g_common.code_path}/works/opensource/work_download_v3_component.py", shell=True)
    if ret.returncode:
        logger.error(f"{EXCEPTION_CODE[504]}{EXCEPTION_STAGE['C']}{EXCEPTION_RESPONSE[2]}下载异常详见{logname},退出!")
        os._exit(1)
    else:
        logger.info("download v3 component success")

def copy_common_file():
    vrd_source = f"{g_common.temp_path}/bin_cmc/application/src_bin/rootfs/opt/pme/conf/vrd_mgnt/pangea_cmc"
    vrd_dest_atl = f"{g_common.temp_path}/bin_cmc/application/src_bin/rootfs/opt/pme/conf/vrd_mgnt/PangeaV6_Atlantic"
    vrd_dest_pac = f"{g_common.temp_path}/bin_cmc/application/src_bin/rootfs/opt/pme/conf/vrd_mgnt/PangeaV6_Pacific"

    if not os.path.isdir(vrd_source):
        logger.info('vrd_source is not exit!')
        return

def untar_bios():
    bios_source = f"{g_common.temp_path}/bin_cmc/application/src_bin/rootfs/opt/pme/conf/bios/pangea_cmc"
    bios_dest_atl = f"{g_common.temp_path}/bin_cmc/application/src_bin/rootfs/opt/pme/conf/bios/PangeaV6_Atlantic"
    bios_dest_pac = f"{g_common.temp_path}/bin_cmc/application/src_bin/rootfs/opt/pme/conf/bios/PangeaV6_Pacific"
    bios_hpm_name = f"bios.hpm"

    if not os.path.isdir(bios_source):
        logger.info('bios_source is not exit!')
        return

    bios_file_list = g_common.py_find_dir(".*\.zip", bios_source)
    for bios_file in bios_file_list:
        g_common.unzip_to_dir(f"{bios_file}", f"{bios_source}")

    if not os.path.isdir(bios_dest_atl):
        os.makedirs(bios_dest_atl)
    if not os.path.isdir(bios_dest_pac):
        os.makedirs(bios_dest_pac)
    # 拷贝后，删除该目录
    g_common.copy(f"{bios_source}/{bios_hpm_name}", f"{bios_dest_atl}/{bios_hpm_name}")
    g_common.copy(f"{bios_source}/{bios_hpm_name}", f"{bios_dest_pac}/{bios_hpm_name}")
    g_common.remove_path(f"{bios_source}")
    

def cpld_change_name():
    cpld_source = f"{g_common.temp_path}/bin_cmc/application/src_bin/rootfs/opt/pme/conf/cpld"
    board_name_lists = ["PangeaV6_Pacific", "PangeaV6_Atlantic_Smm", "PangeaV6_Atlantic", "PangeaV6_Arctic"]
    for board_name in board_name_lists:
        if os.path.isdir(f"{cpld_source}/{board_name}"):
            os.rename(f"{cpld_source}/{board_name}/cpld.hpm", f"{cpld_source}/{board_name}/cpld_resume.hpm")
            # 多板子是分配到不同机器分别执行，这里可以及时返回
            return


def downlaod_sequence():
    if g_llt_mode == False:
        download_open_source_from_codehub()
    download_kmc_from_cmc()
    download_cbb_from_cmc()
    download_dependency()
    download_tools()
    download_v3_component()
    copy_common_file()
    # 如果有bios，需要解压缩
    untar_bios()
    # cpld需要改名为cpld_resume.hpm
    cpld_change_name()

if __name__ == "__main__":
    set_path()
    clear_env()
    download_xml_parse()
    if args.npm:
        download_npm_open_source()
    elif args.open_source:
        download_open_source_from_codehub()
        download_tools()
    else:
        thread_pool = ThreadPoolExecutor(g_thread_num)
        task1 = thread_pool.submit(downlaod_sequence)
        logger.info("Download task is running --->>>")
        g_common.progress_print()
        while not task1.done():
            g_common.progress_print([task1.done()])
        logger.info("Download task finished --->>>")
