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
import time
import subprocess
from utils.common import Common
from utils.log import Logger
from multiprocessing import Process

global logger
logger = Logger(log_name="artget_code_down").logger
global g_common
g_common = Common()

class Repo:
    def __init__(self, repoBase, localPath, revision, branch):
        self.repoUrl = repoBase
        self.localPath = localPath
        self.revision = revision
        self.branch = branch
        # 默认不分group
        self.group = "all"

    def get_remote_url(self):
        return self.repoUrl
    def get_local_path(self):
        return self.localPath
    def get_revision(self):
        return self.revision
    def get_branch(self):
        return self.branch


class FileHandler(xml.sax.ContentHandler):
    def __init__(self, repodict):
        self.currentOp = "invalid"
        self.CurrentData = ""
        self.repodict = repodict
        self.id = 1

    # 元素开始事件处理
    def startElement(self, tag, attributes):
        self.CurrentData = tag
        if tag == "fileidentify":
            logger.info("start hanle repo ...")
        else:#不处理其他tag
            return
        newrepo = Repo(attributes["repoBase"], attributes["localpath"], attributes["revision"], attributes["branch"])
        if(self.repodict != None and self.repodict.get("repoBase") != None):
            logger.info("Duplicate repo! %s"%(attributes["repoBase"]))
            return
        # 下载地址为空的不要
        if newrepo.get_local_path() == "":
            return
        # 以id做为key值
        self.repodict[self.id] = newrepo
        self.id += 1

class ArtgetDownload():
    # 解析源码清单，生成所有的repo对象
    def __parse_src_list(self, srclistpath, repodict):
        if not os.path.exists(srclistpath):
            logger.error(f"{srclistpath} not found")
            os._exit(1)
        # 创建一个 XMLReader
        parser = xml.sax.make_parser()
        handler = FileHandler(repodict)
        parser.setContentHandler(handler)
        parser.parse(srclistpath)

    # 调用git ls-remote获取git仓远端commit id
    def git_clone(self, clone_cmd, reset_cmd):
        time.sleep(1) # 延时1s等任务创建结束，防止偶现文件被锁住导致的git下载失败
        subprocess.run(clone_cmd, shell=True, check=True)
        if reset_cmd is not None:
            subprocess.run(reset_cmd, shell=True, check=True)

    # 下载源码到dest_path
    def __download(self, dest_path, listcon, key_value, repodict):
        if listcon == "":
            list = []
        else:
            list = listcon.split(' ')
        print(f"no_down_list={list}")

        g_common.remove_path(dest_path)
        g_common.check_path(dest_path)

        req_process_list = []

        for key in repodict.keys():
            flag = False
            # 遍历一次不下载的对象列表
            for module in list:
                if module in repodict[key].get_remote_url():
                    flag = True
                    break
            if flag:
                continue

            # 下载流程
            remoteUrl = repodict[key].get_remote_url().replace("ssh://git@",f"https://")
            remoteUrl = remoteUrl.replace(":2222",f"")

            # 开源的仓,只能用个人账号,不能用token
            if "open.codehub.huawei.com" not in repodict[key].get_remote_url() and key_value != "":
                remoteUrl = remoteUrl.replace("//",f"//{key_value}:{key_value}@")

            branch = repodict[key].get_branch().replace("refs/tags/","")
            os.chdir(dest_path)
            if repodict[key].get_local_path() != "." or repodict[key].get_local_path() != "./":
                g_common.remove_path(repodict[key].get_local_path())

            if repodict[key].get_branch().find("refs/tags/") != -1:
                clone_cmd = f"git clone {remoteUrl} -b {branch} --depth=1 {key}"
                reset_cmd = None
            else:
                clone_cmd = f"git clone {remoteUrl} -b {branch} {key}"
                reset_cmd = "cd {} && git reset --hard {}".format(key, repodict[key].get_revision())
            req_process = Process(target=self.git_clone, args=(clone_cmd, reset_cmd))
            req_process.start()
            req_process_list.append(req_process)
        # 等待线程结束

        for req_process in req_process_list:
            req_process.join()

    def after_download(self, repodict):
        # 并行下载到不同目录，最终归档到一起
        for key in repodict.keys():
            local_path = repodict[key].get_local_path()
            if os.path.exists(str(key)):
                subprocess.run(f"mkdir -p {local_path}", shell=True)
                subprocess.run(f"cp -rf {key}/. {local_path}", shell=True)
                subprocess.run(f"rm -rf {key}", shell=True)
            else:
                print(f"Not download {local_path}")

    def download(self, src_xml_path, dest_path, listcon, key_value="-osRUh8ERTgFWJd_aLg4"):
        repodict={}
        self.__parse_src_list(src_xml_path, repodict)
        self.__download(dest_path, listcon, key_value, repodict)
        self.after_download(repodict)
    
    def get_repodict(self, src_xml_path):
        repodict = {}
        self.__parse_src_list(src_xml_path, repodict)
        return repodict

if (__name__ == "__main__"):
    srclistPath = sys.argv[1]
    parse_src_list(srclistPath)
    download(sys.argv[2], sys.argv[3], sys.argv[4])
