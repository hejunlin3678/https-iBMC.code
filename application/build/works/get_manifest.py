#!/usr/bin/python
# coding=utf-8

"""
功 能：该脚本实现自动转换指定组件 cmc 源码清单文件为manifest格式，入参为组件SrcBaseline.xml路径,产品name
版权信息：华为技术有限公司，版本所有(C) 2022-2023
"""
import os
import sys
import collections
import argparse
import requests
import json
from xml.dom.minidom import parse, getDOMImplementation

cwd=os.path.dirname(__file__)
sys.path.append(f"{cwd}/../")
from utils.common import Common
from utils.log import Logger

global g_common
g_common = Common()
global logger
logger = Logger(log_name="get_manifest").logger

class Get_Manifest_Tree:
    def __init__(self, srcbaselinefile, prod_name):
        self.common = Common()
        self.srcbaselinefile = srcbaselinefile
        self.work_path = srcbaselinefile[:len(srcbaselinefile) - len(srcbaselinefile.split('/')[-1])]
        self.prod_name = prod_name
        self.need_xml = ["dependency.xml", "code.xml"]
        self.other_type = []
        self.remote = []
        self.defult_remote = "codehub-dg-y"
        self.open_source_d = collections.defaultdict()
        self.vendor_d = collections.defaultdict()
        self.code_list_d = collections.defaultdict()
        self.platfrom_d = collections.defaultdict()
        self.other_d = collections.defaultdict()
        # 用来设置组件对应类型（vendor open_source platform...）
        self.defult_d = {
            "kunpeng": "code.xml",
            "hwsecurec_group": "code.xml",
            "HWPlatform_CMS": "code.xml",
            "HWPlatform_HiSecNE": "code.xml",
            "OpenBaize": "code.xml",
            "DOPRA": "code.xml",
            "pme_develop": "code.xml"
        }
        self.prod_group_map = {
            "PME": "pme_develop",
            "RTOS": "RTOS",
            "SiteAI": "DOPRA",
            "SiteData": "DOPRA",
            "HostSec": "HWPlatform_HiSecNE",
            "SDK": "kunpeng",
            "AdaptiveLM": "AdaptiveLM_DevelopGroup",
            "KMC": "HWPlatform_KMC",
            "CBB": "HWPlatform_CMS",
            "VPP": "VPP",
            "HI1880": "it-chip-solution"
        }

    def create_xml(self, file_name):
        dom = getDOMImplementation().createDocument(None, "manifest", None)
        root = dom.documentElement
        if file_name == "dependency.xml":
            # 组装视图xml
            self.get_dependency_xml(root, dom)
        elif file_name == "opensource.xml":
            # 组装 opensource.xml
            self.get_opensource_xml(root, dom)
        elif file_name == "vendor.xml":
            # 组装 vendor.xml
            self.get_vendor_xml(root, dom)
        elif file_name == "code.xml":
            # 组装 code.xml
            self.get_code_xml(root, dom)
        with open(file_name, "a", encoding="utf-8") as f:
            dom.writexml(f, addindent='\t', newl='\n', encoding='utf-8')

    def get_dependency_xml(self, root, dom):
           # 组装 remote
        default = dom.createElement('default')
        default.setAttribute("remote", self.defult_remote)
        default.setAttribute("revision", "master")
        for i_remote in self.remote:
            remote = dom.createElement('remote')
            root.appendChild(remote)
            remote.setAttribute("name", f"{i_remote.split('.huawei.com')[0]}")
            remote.setAttribute("fetch", f"https://{i_remote}")
        root.appendChild(default)
        for xml in self.need_xml:
            if xml != "dependency.xml":
                # 组装 include的xml 
                include = dom.createElement('include')
                root.appendChild(include)
                include.setAttribute("name", f"{self.prod_name}/{xml}")
    
    def get_code_xml(self, root, dom):
        project_list = list()
        for key, values in self.code_list_d.items():
            project = dom.createElement('project')
            if self.defult_remote not in values[0]:
                project.setAttribute("remote", values[0].split('/')[2].split('.huawei.com')[0])
            name_attr = values[0][values[0].find('.com') + 5:]
            project.setAttribute("name", name_attr)
            project.setAttribute("path", f"{self.prod_name}/{values[3]}")
            is_tag = False
            if values[2].startswith("refs/tags/"):
                project.setAttribute("revision", values[2])
                is_tag = True
            elif values[2].startswith("tag_"):
                project.setAttribute("revision", "refs/tags/" + values[2])
                is_tag = True
            else:
                tag = self.get_tag_by_commit(values[1], name_attr)
                if tag:
                    project.setAttribute("revision", "refs/tags/" + tag)
                    is_tag = True
                else:
                    project.setAttribute("revision", values[1])
                    project.setAttribute("upstream", values[2])
            project.setAttribute("groups", f"{self.prod_name}")
            if is_tag:
                root.appendChild(project)
            else:
                project_list.append(project)
        for project in project_list:
            root.appendChild(project)


    def get_opensource_xml(self, root, dom):
        for key, values in self.open_source_d.items():
            project = dom.createElement('project')
            root.appendChild(project)
            if self.defult_remote not in values[0]:
                project.setAttribute("remote", values[0].split('/')[2].split('.huawei.com')[0])
            name_attr = values[0][values[0].find('.com') + 5:]
            if self.prod_name == "HI1880":
                continue
            project.setAttribute("name", name_attr)
            project.setAttribute("path", f"{self.prod_name}/{values[3]}")
            if values[2].startswith("refs/tags/"):
                project.setAttribute("revision", values[2])
            else:
                tag = self.get_tag_by_commit(values[1], name_attr)
                if tag:
                    project.setAttribute("revision", "refs/tags/" + tag)
                else:
                    project.setAttribute("revision", values[1])
                    project.setAttribute("upstream", values[2])
            project.setAttribute("groups", f"{self.prod_name},{self.prod_name}_opensource")

    def get_vendor_xml(self, root, dom):
        for key, values in self.vendor_d.items():
            project = dom.createElement('project')
            root.appendChild(project)
            if self.defult_remote not in values[0]:
                project.setAttribute("remote", values[0].split('/')[2].split('.huawei.com')[0])
            name_attr = values[0][values[0].find('.com') + 5:]
            project.setAttribute("name", name_attr)
            project.setAttribute("path", f"{self.prod_name}/{values[3]}")
            if values[2].startswith("refs/tags/"):
                project.setAttribute("revision", values[2])
            elif values[2].startswith("tag_"):
                project.setAttribute("revision", "refs/tags/" + values[2])
            else:
                tag = self.get_tag_by_commit(values[1], name_attr)
                if tag:
                    project.setAttribute("revision", "refs/tags/" + tag)
                else:
                    project.setAttribute("revision", values[1])
                    project.setAttribute("upstream", values[2])
            project.setAttribute("groups", f"{self.prod_name},{self.prod_name}_vendor")

    def get_sourcecode_info(self):
        tree = parse(self.srcbaselinefile)
        all_project_elem = tree.getElementsByTagName("fileidentify")
        for project_elem in all_project_elem:
            repo_base = project_elem.getAttribute("repoBase")
            # 获取组织名称
            component = repo_base.split('/')[3]
            # 尝试获取组织类型
            type_xml = self.defult_d.get(component)
            group_info = self.prod_group_map.get(self.prod_name)
            # 获取 remote
            remote = repo_base.split('/')[2]
            repo_name = repo_base.split('/')[-1].split('.')[0]
            repo_revision = project_elem.getAttribute("revision")
            repo_branch = project_elem.getAttribute("branch")
            local_path = project_elem.getAttribute("localpath")
            if local_path == "":
                continue
            if remote not in self.remote:
                self.remote.append(remote)
            if "OpenSourceCenter" in component:
                self.need_xml.append("opensource.xml")
                self.open_source_d.setdefault(repo_name, (repo_base, repo_revision, repo_branch, local_path))
            elif component == group_info:  # group信息相同，自研代码
                self.code_list_d.setdefault(repo_name, (repo_base, repo_revision, repo_branch, local_path))
            else:
                # 作为第三方放入
                self.need_xml.append("vendor.xml")
                self.vendor_d.setdefault(repo_name, (repo_base, repo_revision, repo_branch, local_path))

    def get_tag_by_commit(self, commit_id, address):
        if commit_id.startswith('refs/tags/'):
            return commit_id[10:]
        website = 'https://codehub-y.huawei.com/api/v4/projects/'
        encode_adress = address.replace('.git', '').replace('/', '%2F')
        resource2 = '/repository/tags?per_page=100'
        url = website + encode_adress + resource2
        headers = { "PRIVATE-TOKEN": "H5WsKPVVYXGAkWV4tc1F" }
        res = requests.get(url, headers=headers, verify=False)
        resp = res.json()
        try:
            for info in resp:
                if info['commit']['id'] == commit_id:
                    return info['name']
        except:
            print("resp={}".format(resp))
        return None


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="get manifest.xml")
    parser.add_argument("-s", "--srcfile", help="srcbaselinefile", default="V2R2_trunk/SDK/sourcecode.xml")
    parser.add_argument("-p", "--component", help="Component name", default="SDK")
    args = parser.parse_args()
    manifest = Get_Manifest_Tree(args.srcfile, args.component)
    manifest.get_sourcecode_info()
    # 清理目录
    manifest.common.remove_path(manifest.work_path)
    manifest.common.check_path(manifest.work_path)
    os.chdir(manifest.work_path)
    manifest.need_xml = list(set(manifest.need_xml))
    os.chdir(manifest.work_path)
    for xml in manifest.need_xml:
        manifest.create_xml(xml)
    if len(manifest.other_d) > 0:
        logger.warning("please check these components !!!")
        for key, values in manifest.other_d.items():
            print(values)
