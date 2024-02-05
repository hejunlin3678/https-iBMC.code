#!/usr/bin/env python
# coding: utf-8

"""
功 能：download_from_cmc类，该类主要涉及将 CMC上与BMC配套的二进制组件 binIBMC 下载到本地代码库中复位
据信息树配置文件下载配套软件包，平台文件
版权信息：华为技术有限公司，版本所有(C) 2019-2020
"""

import os
import sys
import subprocess
import argparse
import stat
import shutil

cwd=os.path.dirname(__file__)
sys.path.append(f"{cwd}/../")
sys.path.append(f"{cwd}/../../")
from utils.common import Common
from works.artget_code_down import ArtgetDownload
from concurrent.futures import ThreadPoolExecutor
from utils.log import Logger
from utils.error_info import EXCEPTION_STAGE
from utils.error_info import EXCEPTION_RESPONSE
from utils.error_info import EXCEPTION_CODE
from works.get_manifest import Get_Manifest_Tree
from xml.dom.minidom import parse, getDOMImplementation

logger = Logger().logger

def get_path(path):
    return os.path.realpath(path)

def set_path():
    global g_common
    g_common = Common()
    global g_produce_name
    g_produce_name = args.Produce
    global g_xml_path
    g_xml_path = args.xml
    global g_no_down_list
    g_no_down_list = args.no_down_list
    global g_key_value
    g_key_value = args.key_value

def create_sourcecode_xml(assemble_dict):
    dom = getDOMImplementation().createDocument(None, "Results", None)
    root = dom.documentElement
    dependentSourceFiles = dom.createElement('dependentSourceFiles')
    root.appendChild(dependentSourceFiles)
    for values in assemble_dict.values():
        fileidentify = dom.createElement('fileidentify')
        dependentSourceFiles.appendChild(fileidentify)
        fileidentify.setAttribute("repoType", "GIT")
        fileidentify.setAttribute("repoBase", values.get_remote_url())
        fileidentify.setAttribute("revision", values.get_revision())
        fileidentify.setAttribute("branch", values.get_branch())
        fileidentify.setAttribute("localpath", values.get_local_path())
    
    with open(f"{g_common.code_root}/{g_produce_name}/sourcecode.xml", "a", encoding="utf-8") as f:
            dom.writexml(f, addindent='\t', newl='\n', encoding='utf-8')

def assemble_xml():
    assemble_dict = {}
    remove_list = []
    for sub_xml in g_common.list_all_file("sourcecode.*", f"{g_common.code_root}/{g_produce_name}"):
        xml_obj = ArtgetDownload()
        repodict = xml_obj.get_repodict(sub_xml)
        for values in repodict.values():
            assemble_dict[values.get_remote_url()] = values
        remove_list.append(sub_xml)
    create_sourcecode_xml(assemble_dict)
    for rm_xml in remove_list:
        os.remove(rm_xml)

def download():
    dependency_file = get_path(g_xml_path)
    logname = f"{g_common.log_manage_dic.get('download_log')}/download_{g_produce_name}_codexml.log"
    logger.info(f"get {g_produce_name} xml conf by artget begin...")
    g_common.remove_path(f"{g_common.code_root}/{g_produce_name}")
    if dependency_file.split('/')[-1].find('*'):
        for d_file in g_common.list_all_file("srcbaseline.*", os.path.dirname(dependency_file)):
            cmd = f"artget pull -sc {d_file} -ap {g_common.code_root}/{g_produce_name}"
            subprocess.run(cmd, shell=True)
        assemble_xml()
    else:
        cmd = f"artget pull -sc {dependency_file} -ap {g_common.code_root}/{g_produce_name}"
        subprocess.run(cmd, shell=True)
    logger.info(f"get {g_produce_name} xml conf by artget end!")
    artget_download = ArtgetDownload()
    for file in os.listdir(f"{g_common.code_root}/{g_produce_name}"):
        if "source" in file:
            if args.open_local_debug:
                logger.info(f'get {g_produce_name} source code begin...')
                artget_download.download(f"{g_common.code_root}/{g_produce_name}/{file}", f"{g_common.code_root}/{g_produce_name}", f"{g_no_down_list}", f"{g_key_value}")
                logger.info(f'get {g_produce_name} source code end!')
                continue

            manifest = Get_Manifest_Tree(f"{g_common.code_root}/{g_produce_name}/{file}", g_produce_name)
            manifest.get_sourcecode_info()
            # 清理目录
            g_common.remove_path(manifest.work_path)
            g_common.check_path(manifest.work_path)
            os.chdir(manifest.work_path)
            manifest.need_xml = list(set(manifest.need_xml))
            os.chdir(manifest.work_path)
            for xml in manifest.need_xml:
                manifest.create_xml(xml)
            if len(manifest.other_d) > 0:
                logger.warning("please check these components !!!")
                for key, values in manifest.other_d.items():
                    print(values)
            
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Excute download_from_cmc_SrcBaseline.py")
    parser.add_argument("-p", "--Produce", help="Product Name", default="")
    parser.add_argument("-x", "--xml", help="Relative path of the .xml file of the original CMC code to be downloaded relative to the .python file.", default="")
    parser.add_argument("-n", "--no_down_list", help="Do not download the module path", default="")
    parser.add_argument("-k", "--key_value", help="Token value of git download code", default="-osRUh8ERTgFWJd_aLg4")
    parser.add_argument("-o", "--open_local_debug", help="open_local_debug", action="store_true")
    args = parser.parse_args()

    set_path()
    download()
