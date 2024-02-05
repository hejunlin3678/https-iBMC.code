#!/usr/bin/env python3
# coding: utf-8

"""
文件名：parse_to_xml.py
功能：解析json生成xml文件，利用xml文件进行下载
版权信息：华为技术有限公司，版本所有© 2021-2023
"""

import os
import argparse
import json as js
import xml.etree.ElementTree as ET
from configparser import ConfigParser
import sys
cwd=os.path.dirname(__file__)
sys.path.append(f"{cwd}/../")
sys.path.append(f"{cwd}/../../")
from utils.config import Config

from utils.common import Common
from utils.log import Logger
from utils.error_info import EXCEPTION_STAGE
from utils.error_info import EXCEPTION_RESPONSE
from utils.error_info import EXCEPTION_CODE

# logger = Logger().logger
# parser = argparse.ArgumentParser(description="Excute parse_to_xml.py")
# parser.add_argument("-b", "--board_name", help = "Fill in board_name", default = "")
# parser.add_argument("-l", "--llt", action="store_true", help = "LLT mode")
# args = parser.parse_args()

class DependencyXmlParse():
    def __init__(self, board_name, llt_enable, common):
        """
        功能描述：配置必要的全局变量
        参数：无
        返回值：无
        """
        self.logger = common.logger
        self.common = common
        self.board_name = board_name
        self.llt = llt_enable
        self.xml_path = f"{self.common.code_path}/dependency_for_hpm"

    def getJsonContent(self, fileName):
        """
        功能描述：加载json文件到字典类型
        参数：fileName 要加载的json文件
        返回值：传入json生成的字典
        """
        with open(fileName, "r", encoding='UTF-8') as load_f:
            content = js.load(load_f, strict=False)
            load_f.close()
        return content


    def get_version(self):
        """
        功能描述：解析dependency_bin.xml中的版本号并返回
        参数：无
        返回值：dependency_bin.xml中的版本号
        """
        os.chdir(os.path.join(self.common.code_path, "dependency_for_hpm/downxml"))
        domTree = ET.parse("dependency_bin.xml")
        root = domTree.getroot()
        ele_dependencies = root.find("dependencies")
        ele_dependency = ele_dependencies.find("dependency")
        ele_id = ele_dependency.find("id")
        ele_version = ele_id.find("componentVersion")
        os.chdir(os.path.join(self.common.code_path))
        return ele_version.text


    def update_oem_component(self, component_content):
        """
        功能描述：支持OEM定制化修改, 使用`oem_config_path`里面的oem_component_config更新`component_content`
        以oem_component_config中的键值对更新component_content中的键值对

        参数：  component_content 组件json解析后的结果
        返回值：更新后的解析结果
        """
        self.logger.info("adding OEM component_content...")
        oem_component_dir = os.path.join(self.common.oem_config_path,"OEM_component")
        if not os.path.exists(oem_component_dir):
            self.logger.info(f"no OEM component_content found in {oem_component_dir}")
            return component_content
        for file in os.listdir(oem_component_dir):
            with open(os.path.join(oem_component_dir, file)) as jsonfile:
                oem_component_config = js.load(jsonfile)
            for i in oem_component_config.keys():
                component_content[i].update(oem_component_config[i])
        return component_content


    def plat_parse(self, ele_dependencies, plat_components, dependency_content):
        """
        功能：平台文件下载xml格式化
        参数说明：  ele_dependencies     xml配置的ET句柄
                plat_components      平台组件的配置
                dependency_content   当前单板的依赖
        返回值：无
        """
        plat_dep = dependency_content.get("board").get(self.board_name).get("Platform")
        self.logger.info("Parse component platform")
        if plat_components == None or len(plat_dep) == 0:
            self.logger.info(f"Platform part dependency is empty")
            return

        for plat in plat_dep:
            for plat_attr in plat_components.get(plat).get("Artifactory"):
                ele_dependency = ET.Element("dependency")
                ele_versiontype = ET.Element("versionType")
                ele_versiontype.text = plat_attr.get("versionType")
                ele_dependency.append(ele_versiontype)

                ele_repotype = ET.Element("repoType")
                ele_repotype.text = plat_attr.get("repoType")
                ele_dependency.append(ele_repotype)

                ele_id = ET.Element("id")
                ele_offering = ET.Element("offering")
                ele_offering.text = plat_attr.get("offering")
                ele_id.append(ele_offering)
                ele_version = ET.Element("version")
                ele_version.text = plat_attr.get("version")
                ele_id.append(ele_version)
                ele_dependency.append(ele_id)

                ele_according = ET.Element("according")
                ele_according.text = plat_attr.get("according")
                ele_dependency.append(ele_according)

                ele_copies = ET.Element("copies")
                ele_copy = ET.Element("copy")
                ele_source = ET.Element("source")
                ele_source.text = plat_attr.get("source")
                ele_dest = ET.Element("dest")
                ele_dest.text = plat_attr.get("dest")
                ele_copy.append(ele_source)
                if plat_attr.get("dependType"):
                    ele_dependType = ET.Element("dependType")
                    ele_dependType.text = plat_attr.get("dependType")
                    ele_copy.append(ele_dependType)
                ele_copy.append(ele_dest)
                ele_copies.append(ele_copy)
                ele_dependency.append(ele_copies)
                ele_dependencies.append(ele_dependency)
        return


    def third_party_parse(self, ele_dependencies, third_party_components, dependency_content):
        """
        功能：第三方文件下载xml格式化
        参数说明：  ele_dependencies        xml配置的ET句柄
                third_party_components  第三方组件的配置
                dependency_content      当前单板的依赖
        返回值：无
        """
        self.logger.info("Parse component third part")
        third_party_dep = dependency_content.get("board").get(self.board_name).get("ThirdParty")
        if third_party_dep == None:
            self.logger.info("Independent of third-party components")
            return

        if third_party_components == None or len(third_party_dep) == 0:
            self.logger.info(f"Third part dependency is empty")
            return

        for third_attr in third_party_components.get("Artifactory"):
            if third_attr.get("artifactName") not in third_party_dep:
                continue
            ele_dependency = ET.Element("dependency")

            ele_versiontype = ET.Element("versionType")
            ele_versiontype.text = third_attr.get("versionType")
            ele_dependency.append(ele_versiontype)

            ele_repotype = ET.Element("repoType")
            ele_repotype.text = third_attr.get("repoType")
            ele_dependency.append(ele_repotype)

            ele_id = ET.Element("id")
            ele_artifactName = ET.Element("artifactName")
            ele_artifactName.text = third_attr.get("artifactName")
            ele_id.append(ele_artifactName)
            ele_pdmCode = ET.Element("pdmCode")
            ele_pdmCode.text = third_attr.get("pdmCode")
            ele_id.append(ele_pdmCode)
            ele_dependType = ET.Element("dependType")
            ele_dependType.text = third_attr.get("dependType")
            ele_id.append(ele_dependType)
            ele_dependency.append(ele_id)

            ele_copies = ET.Element("copies")
            ele_copy = ET.Element("copy")

            ele_source = ET.Element("source")
            ele_source.text = third_attr.get("source")
            ele_copy.append(ele_source)

            ele_dest = ET.Element("dest")
            ele_dest.text = third_attr.get("dest")
            ele_copy.append(ele_dest)

            ele_parameter = ET.Element("parameter")
            ele_copy.append(ele_parameter)

            ele_clear = ET.Element("clear")
            ele_copy.append(ele_clear)

            ele_copies.append(ele_copy)
            ele_dependency.append(ele_copies)

            ele_dependencies.append(ele_dependency)
        return


    def component_common_parse(self, ele_dependencies, component_name, offering):
        """
        功能：其他组件文件下载xml格式化
        参数说明：  ele_dependencies     xml配置的ET句柄
                component_name       其他组件的配置
                offering             组件来源
        返回值：无
        """
        if component_name == None:
            self.logger.info(f"This component is empty")
            return

        for component in component_name.get("Artifactory"):
            ele_dependency = ET.Element("dependency")

            ele_versiontype = ET.Element("versionType")
            ele_versiontype.text = component.get("versionType")
            ele_dependency.append(ele_versiontype)

            ele_repotype = ET.Element("repoType")
            ele_repotype.text = component.get("repoType")
            ele_dependency.append(ele_repotype)

            # id解析
            ele_id = ET.Element("id")

            ele_offering = ET.Element("offering")
            ele_offering.text = offering
            ele_id.append(ele_offering)

            ele_componentName = ET.Element("componentName")
            ele_componentName.text = component.get("name")
            ele_id.append(ele_componentName)

            ele_componentVersion = ET.Element("componentVersion")
            ele_componentVersion.text = component.get("version")
            ele_id.append(ele_componentVersion)

            ele_dependency.append(ele_id)

            # copies解析
            ele_copies = ET.Element("copies")
            ele_copy = ET.Element("copy")

            ele_source = ET.Element("source")
            ele_source.text = component.get("source")
            ele_copy.append(ele_source)

            ele_dest = ET.Element("dest")
            ele_dest.text = component.get("dest")
            if ele_dest.text == "":
                return
            ele_copy.append(ele_dest)

            ele_copies.append(ele_copy)
            ele_dependency.append(ele_copies)

            # 添加到生成的传入的xml
            ele_dependencies.append(ele_dependency)
        return


    def components_json2xml(self, board_name, conponent_json="component.json", dependency_json="dependency.json"):
        """
        功能：将传入json格式化为xml
        参数说明：  board_name         当前单板名
                conponent_json     组件的配置json
                dependency_json    所有单板的依赖配置json
        返回值：无
        """
        os.chdir(self.xml_path)

        # 获取ini文件配置
        cfg = ConfigParser()
        cfg.read('version.ini')
        offering = cfg.get('3.2.0', 'product')
        version = cfg.get('3.2.0', 'version')

        # 读取原始xml文件，写入ini文件中的配置
        domTree = ET.parse(f"{self.xml_path}/downxml/dependency.xml")
        root = domTree.getroot()
        artifact = root.findall("artifact")
        artifact_versionType = artifact[0].findall("versionType")
        artifact_versionType[0].text = "Component"

        artifact_repoType = artifact[0].findall("repoType")
        artifact_repoType[0].text = "Generic"

        artifact_id = artifact[0].findall("id")
        product = artifact_id[0].findall("offering")
        product[0].text = offering
        componentName = artifact_id[0].findall("componentName")
        componentName[0].text = board_name
        componentVersion = artifact_id[0].findall("componentVersion")
        componentVersion[0].text = version

        # 加载json文件到变量
        component_content = self.getJsonContent(conponent_json)
        dependency_content = self.getJsonContent(dependency_json)

        # 有OEM时解析并覆盖部分组件配置，以用来替换
        component_content = self.update_oem_component(component_content)

        # 所有组件解析(所有组件分为以下几类，新增类型改此处)
        plat_components = component_content.get("PlatComponents")
        third_party = component_content.get("ThirdParty")
        bin_cmc = component_content.get("BinCMC")
        open_source_common = component_content.get("OpenSourceCommon")
        adaptive_lm = component_content.get("AdaptiveLM")
        hostsec = component_content.get("HostSec")
        dft_tools = component_content.get("DftTools")
        third_groupware = component_content.get("ThirdPartyGroupware")
        java_tools = component_content.get("JavaClientTools")
        pangea_atlantic_st_vrd = component_content.get("pangea V6R5C20 pacific_Atlantic ST VRD")
        pangea_atlantic_ifx_vrd = component_content.get("pangea V6R5C20 pacific_atlantic IFX VRD")
        pangea_atlantic_sd5003_vrd = component_content.get("pangea V6R5C20 pacific_atlantic sd5003 VRD")
        pangea_arctic_vrd = component_content.get("pangea V6R5C20 arctic VRD")
        pangea_pacific_au5325 = component_content.get("PANGEA_V600R005C20_Pacific_Au5325")
        pangea_pacific_sa7710 = component_content.get("PANGEA_V600R005C20_Pacific_SA7710")
        pangea_pacific_si5345 = component_content.get("PANGEA_V600R005C20_Pacific_Si5345")
        pangea_pac1220v6_au5325 = component_content.get("PANGEA_V600R005C60_Pac1220V6_Au5325")
        pangea_bios = component_content.get("pangea v6r5c20 bios")
        pangea_pacific_eqm_cpld = component_content.get("PANGEA_V600R005C20_Pacific_Equipment_CPLD")
        pangea_atlantic_smm_eqm_cpld = component_content.get("PANGEA_V600R005C20_Atlantic_Smm_Equipment_CPLD")
        pangea_atlantic_eqm_cpld = component_content.get("PANGEA_V600R005C20_Atlantic_MB_Equipment_CPLD")
        pangea_arctic_eqm_cpld = component_content.get("pangea v6r5c20 ArcticOcean Equipment CPLD")
        pangea_CST0220V6_equip_cpld = component_content.get("pangea v6r3c60 Equipment CPLD for CST0220V6")
        pangea_CST0221V6_equip_cpld = component_content.get("pangea v6r3c60 Equipment CPLD for CST0221V6")
        pangea_SMM0001V6_equip_cpld = component_content.get("pangea v6r3c60 Equipment CPLD for SMM0001V6")
        pangea_CST1220V6_equip_cpld = component_content.get("pangea v6r3c60 Equipment CPLD for CST1220V6")
        pangea_CST1020V6_equip_cpld = component_content.get("pangea v6r3c60 Equipment CPLD for CST1020V6")

        # 预留组件，需要新增类似plat_parse接口
        # board_advance_config = component_content.get("BoardAdvanceConfig")
        # services = component_content.get("Services")
        # micro_services = component_content.get("MicroServices")
        # product = component_content.get("Product")
        # component_sets = component_content.get("ComponentSets")

        # 对各组件解析
        ele_dependencies = root.find("dependencies")
        self.plat_parse(ele_dependencies, plat_components, dependency_content)
        self.third_party_parse(ele_dependencies, third_party, dependency_content)

        # 由于组件独立，缺少外层的判断机制，使用组件表配置
        components_correspond_table = {
            "BinCMC": bin_cmc,
            "OpenSourceCommon": open_source_common,
            "AdaptiveLM": adaptive_lm,
            "HostSec": hostsec,
            "DftTools": dft_tools,
            "ThirdPartyGroupware": third_groupware,
            "JavaClientTools": java_tools,
            "pangea V6R5C20 pacific_Atlantic ST VRD": pangea_atlantic_st_vrd,
            "pangea V6R5C20 pacific_atlantic IFX VRD": pangea_atlantic_ifx_vrd,
            "pangea V6R5C20 pacific_atlantic sd5003 VRD": pangea_atlantic_sd5003_vrd,
            "pangea V6R5C20 arctic VRD": pangea_arctic_vrd,
            "PANGEA_V600R005C20_Pacific_Au5325": pangea_pacific_au5325,
            "PANGEA_V600R005C20_Pacific_SA7710": pangea_pacific_sa7710,
            "PANGEA_V600R005C20_Pacific_Si5345": pangea_pacific_si5345,
            "PANGEA_V600R005C60_Pac1220V6_Au5325": pangea_pac1220v6_au5325,
            "pangea v6r5c20 bios": pangea_bios,
            "PANGEA_V600R005C20_Pacific_Equipment_CPLD": pangea_pacific_eqm_cpld,
            "PANGEA_V600R005C20_Atlantic_Smm_Equipment_CPLD": pangea_atlantic_smm_eqm_cpld,
            "PANGEA_V600R005C20_Atlantic_MB_Equipment_CPLD": pangea_atlantic_eqm_cpld,
            "pangea v6r5c20 ArcticOcean Equipment CPLD": pangea_arctic_eqm_cpld,
            "pangea v6r3c60 Equipment CPLD for CST0220V6": pangea_CST0220V6_equip_cpld,
            "pangea v6r3c60 Equipment CPLD for CST0221V6": pangea_CST0221V6_equip_cpld,
            "pangea v6r3c60 Equipment CPLD for SMM0001V6": pangea_SMM0001V6_equip_cpld,
            "pangea v6r3c60 Equipment CPLD for CST1220V6": pangea_CST1220V6_equip_cpld,
            "pangea v6r3c60 Equipment CPLD for CST1020V6": pangea_CST1020V6_equip_cpld,
        }
        # 扩展使用，请勿随意更改
        offering_table = {
            "AdaptiveLM": "AdaptiveLM",
            "HostSec": "HiSec-ICT NE",
            "DftTools": "iBMC V2",
            "JavaClientTools": "iBMC V2",
            "pangea V6R5C20 pacific_Atlantic ST VRD": "PANGEA",
            "pangea V6R5C20 pacific_atlantic IFX VRD": "PANGEA",
            "pangea V6R5C20 pacific_atlantic sd5003 VRD": "PANGEA",
            "pangea V6R5C20 arctic VRD": "PANGEA",
            "PANGEA_V600R005C20_Pacific_Au5325": "PANGEA",
            "PANGEA_V600R005C20_Pacific_SA7710": "PANGEA",
            "PANGEA_V600R005C20_Pacific_Si5345": "PANGEA",
            "PANGEA_V600R005C60_Pac1220V6_Au5325": "PANGEA",
            "pangea v6r5c20 bios": "PANGEA",
            "PANGEA_V600R005C20_Pacific_Equipment_CPLD": "PANGEA",
            "PANGEA_V600R005C20_Atlantic_Smm_Equipment_CPLD": "PANGEA",
            "PANGEA_V600R005C20_Atlantic_MB_Equipment_CPLD": "PANGEA",
            "pangea v6r5c20 ArcticOcean Equipment CPLD": "PANGEA",
            "pangea v6r3c60 Equipment CPLD for CST0220V6": "PANGEA",
            "pangea v6r3c60 Equipment CPLD for CST0221V6": "PANGEA",
            "pangea v6r3c60 Equipment CPLD for SMM0001V6": "PANGEA",
            "pangea v6r3c60 Equipment CPLD for CST1220V6": "PANGEA",
            "pangea v6r3c60 Equipment CPLD for CST1020V6": "PANGEA",
        }

        # 为减少修改的配置，这里先去获取dependency_bin.xml中的版本号
        components_dep = dependency_content.get("board").get(self.board_name).get("Component")
        for component in components_dep:
            self.logger.info(f"Parse component {component}")
            component_conf = components_correspond_table.get(component)
            if component == "BinCMC" and component_conf != None:
                for bin_cmc_component in component_conf.get("Artifactory"):
                    if bin_cmc_component['name'] == "BinIBMC":
                        bin_cmc_component['version'] = self.get_version()
            self.component_common_parse(ele_dependencies, component_conf, offering_table.get(component, "iBMC V2"))


        # 写入到目标目录
        xml_dest_path = f"{self.xml_path}/downxml/board"
        self.common.check_path(xml_dest_path)
        self.logger.info(f"Generate xml file is {xml_dest_path}/{board_name}_dependency.xml")
        domTree.write(f"{xml_dest_path}/{board_name}_dependency.xml", encoding="utf-8", xml_declaration="<?xml version=\"1.0\" encoding=\"UTF-8\"?>", short_empty_elements=False)

    def parse(self):
        if self.llt == True:
            self.components_json2xml(self.board_name, "llt_component.json", "llt_dependency.json")
        else:
            self.components_json2xml(self.board_name)
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="build target")
    parser.add_argument("-b", "--board_name", help="Board list must in '' and separated by space, default value is 2488hv6", default="2488hv6")
    args = parser.parse_args()
    cfg = Config(args.board_name)
    xml = DependencyXmlParse(cfg.board_name, False, cfg.common)
    xml.parse()
