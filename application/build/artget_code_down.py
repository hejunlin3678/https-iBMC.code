#!/usr/bin/env python
# coding=utf-8

"""
文件名：artget_code_down.py
功能：解析 artget源码清单 并下载源码
版权信息：华为技术有限公司，版本所有(C) 2019-2020
"""
import xml.sax
import os
import sys
import subprocess
from utils.common import Common
from utils.log import Logger

global repodict
repodict={}
global logger
logger = Logger(log_name="artget_code_down").logger
global g_common
g_common = Common()

class Repo:
    def __init__(self, repoBase, localPath, revision, repoType, branch):
        self.repoUrl = repoBase
        self.localPath = localPath
        self.revision = revision
        self.repoType = repoType
        self.branch = branch
        # 默认不分group
        self.group = "all"

    def get_remote_url(self):
        return self.repoUrl
    def get_repo_type(self):
        return self.repoType
    def get_local_path(self):
        return self.localPath
    def get_revision(self):
        return self.revision
    def get_branch(self):
        return self.branch


class FileHandler(xml.sax.ContentHandler):
    def __init__(self):
        self.currentOp = "invalid"
        self.CurrentData = ""

    # 元素开始事件处理
    def startElement(self, tag, attributes):
        self.CurrentData = tag
        if tag == "fileidentify":
            logger.info("start hanle repo ...")
        else:#不处理其他tag
            return
        newrepo = Repo(attributes["repoBase"], attributes["localpath"], attributes["revision"], attributes["repoType"], attributes["branch"])
        if(repodict != None and repodict.get("repoBase") != None):
            logger.info("Duplicate repo! %s"%(attributes["repoBase"]))
            os._exit(1)
        repodict[attributes["localpath"]] = newrepo

# 解析源码清单，生成所有的repo对象
def parse_src_list(srclistpath):
    if not os.path.exists(srclistpath):
        logger.error(f"{srclistpath} not found")
        os._exit(1)
    # 创建一个 XMLReader
    parser = xml.sax.make_parser()
    handler = FileHandler()
    parser.setContentHandler(handler)
    parser.parse(srclistpath)

# 下载源码到dest_path
def download(dest_path):
    for key in repodict.keys():
        if "java" not in key and "JAVA" not in key:
            remoteUrl = repodict[key].get_remote_url()
            branch = repodict[key].get_branch()
            g_common.remove_path(dest_path)
            g_common.check_path(dest_path)
            os.chdir(dest_path)
            subprocess.run(f"git clone {remoteUrl} -b {branch}", shell=True)
            os.chdir(repodict[key].get_local_path().split('/')[-1].upper())
            subprocess.run(f"git reset --hard {repodict[key].get_revision()}", shell=True)

if (__name__ == "__main__"):
    srclistPath = sys.argv[1]
    parse_src_list(srclistPath)
    download(sys.argv[2])
