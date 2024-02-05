#!/usr/bin/python
# coding=utf-8
# Copyright © Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.

import xml.dom.minidom
import os,sys,shutil
import json
import json.encoder
import string
from collections import OrderedDict


class evt_information():
    def __init__(self):
        self.objname = ''
        self.name = ''
        self.severity = ''
        self.msg_fmt = ''
        self.evt_id = ''
        self.suggestion = ''
        self.event_type = 0
        self.impact = ''
        self.cause = ''


class registry_info():
    def __init__(self):
        self.major_ver = 0
        self.minor_ver = 0
        self.aux_ver = 0
        self.lang = 'en'

class Redfish_Event_Generator():

    def __init__(self, config):
        self.config = config

    def open_file_redfish_event_registry_generator(self, file_name, mode='r', encoding=None, **kwargs):
        if mode in ['r', 'rt', 'tr'] and encoding is None:
            with open(file_name, 'rb') as f:
                context = f.read()
                for encoding_item in ['UTF-8', 'GBK', 'ISO-8859-1']:
                    try:
                        context.decode(encoding=encoding_item)
                        encoding = encoding_item
                        break
                    except UnicodeDecodeError as e:
                        pass
        return open(file_name, mode=mode, encoding=encoding, **kwargs)

    def get_attrvalue(self, node, attrname):
        return node.getAttribute(attrname) if node else None

    def get_nodevalue(self, node, index=0):
        return node.childNodes[index].nodeValue if node and node.childNodes else None

    def get_xmlnode(self, node, name):
        return node.getElementsByTagName(name) if node and len(node.childNodes) > 0  else []

    #获取注册表版本等信息
    def get_event_registry_info(self, language_xml_path):

        doc = xml.dom.minidom.parse(language_xml_path)
        root = doc.documentElement

        reg_info = registry_info()

        major_ver_node = self.get_xmlnode(root, 'MAJOR_VERSION')
        minor_ver_node = self.get_xmlnode(root, 'MINOR_VERSION')
        aux_ver_node = self.get_xmlnode(root, 'AUX_VERSION')

        reg_info.major_ver = self.get_nodevalue(major_ver_node[0])
        reg_info.minor_ver = self.get_nodevalue(minor_ver_node[0])
        reg_info.aux_ver = self.get_nodevalue(aux_ver_node[0])

        #support en only currently
        reg_info.lang = 'en'

        return reg_info

    #提升运行速度，保存为字典
    def get_lang_item_dict(self, path, lang, item_type):

        doc = xml.dom.minidom.parse(path)
        root = doc.documentElement

        dict = {}

        language_set_array = self.get_xmlnode(root, 'LanguageSet')
        for lang_set_iter in language_set_array:
            if lang == self.get_attrvalue(lang_set_iter, 'NAME'):
                evt_desc_node = self.get_xmlnode(lang_set_iter, item_type)
                item_array = self.get_xmlnode(evt_desc_node[0], 'Item')

                for item_array_iter in item_array:
                    key = self.get_attrvalue(item_array_iter, 'NAME')
                    dict[key] = self.get_nodevalue(item_array_iter)

                return dict

        return []

    #获取严重等级字符串
    def get_severity_string(self, evt_type, severity_level):
        severity_str = ['OK', 'Warning', 'Critical']

        severity_index = int(severity_level)
        evt_type_index = int(evt_type)

        # 事件类型, 0:系统事件(包括SEL和事件), 1:维护事件; 2:运行事件; 3:装备事件
        # 系统事件（故障事件）、装备事件级别, 0:ok, OK; 1:minor, Warning; 2:major, Warning; 3:critical, Critical。
        # 维护事件、运行事件级别：0: info, OK; 1: warning, Warning; 2：error, Critical
        if (evt_type_index == 0 or evt_type_index == 3) and severity_index > 1:
            severity_index = severity_index - 1

        #print 'event_type:%s, severity_index:%d'%(evt_type, severity_index)

        return severity_str[int(severity_index)]

    #获取参数信息
    def get_msg_args_info(self, msg_fmt_str):
        args_array = []

        #print 'input msg fmt:',msg_fmt_str
        if None == msg_fmt_str:
            return 0, args_array

        for i in range(1,100):
            cur_index_str = '%%%d'%(i)
            if -1 == msg_fmt_str.find(cur_index_str):
                #print 'get msg args info, result i:%d, index str:%s'%(i, cur_index_str)
                return (i - 1, args_array)
            else:
                args_array.append('string')

    #生成一条记录
    def  generate_record_item(self, evt_information_obj, desc_item_dict, sugg_item_dict, evt_lang_impact_dict, evt_lang_cause_dict):

        evt_registry_item = OrderedDict()

        msg_fmt = desc_item_dict.get(evt_information_obj.msg_fmt)
        if None != msg_fmt:
            msg_fmt = msg_fmt.replace('@#AB;', ' ')
            #去掉首末尾空格
            msg_fmt = msg_fmt.strip()

        resolution = sugg_item_dict.get(evt_information_obj.suggestion)
        if None != resolution:
            resolution = resolution.replace('@#AB;', ' ')
            #去掉首末尾空格
            resolution = resolution.strip()

        impact =	evt_lang_impact_dict.get(evt_information_obj.impact)
        if None != impact:
            impact = impact.replace('@#AB;', ' ')
            #去掉首末尾空格
            impact = impact.strip()

        cause =	evt_lang_cause_dict.get(evt_information_obj.cause)
        if None != cause:
            cause = cause.replace('@#AB;', ' ')
            #去掉首末尾空格
            cause = cause.strip()

        evt_registry_item['Description'] = None
        evt_registry_item['Message'] = msg_fmt

        evt_registry_item['Severity'] = self.get_severity_string(evt_information_obj.event_type, evt_information_obj.severity)
        (evt_registry_item['NumberOfArgs'], evt_registry_item['ParamTypes']) = self.get_msg_args_info(msg_fmt)
        evt_registry_item['Resolution'] = resolution

        evt_oem_item = OrderedDict()
        evt_huawei_item = OrderedDict()

        evt_huawei_item['@odata.type'] = '#HwBMCEvent.v1_0_0.HwBMCEvent'
        evt_huawei_item['EventId'] = evt_information_obj.evt_id
        evt_huawei_item['EventName'] = evt_information_obj.name
        evt_huawei_item['EventEffect'] = impact
        evt_huawei_item['EventCause'] = cause

        evt_oem_item['Huawei'] = evt_huawei_item
        evt_registry_item['Oem'] = evt_oem_item

        return evt_registry_item

    #生成事件注册表记录
    def generate_event_registry_record(self):

        platform = self.config.board_version.lower()

        platform_xml_path = '%s/application/src/resource/pme_profile/platform.xml'%(self.config.common.code_root)
        if platform != 'v3' :
            platform_xml_path = '%s/application/src/resource/pme_profile/platform_%s.xml'%(self.config.common.code_root, platform)
        event_lang_path = '%s/application/src/rootfs/opt/pme/conf/language/event_lang_template_v2.xml'%(self.config.common.code_root)
        evt_registry_template_path = '%s/application/src/resource/config/redfish/v1/redfish_template/event_registry_template.json'\
                                     %(self.config.common.code_root)

        doc = xml.dom.minidom.parse(platform_xml_path)
        root = doc.documentElement
        objs = root.getElementsByTagName("OBJECT")

        evt_info = evt_information()

        #获取版本信息
        evt_lang_desc_dict = self.get_lang_item_dict(event_lang_path, 'en', 'EventDescription')
        evt_lang_sugg_dict = self.get_lang_item_dict(event_lang_path, 'en', 'EventSuggestion')
        evt_lang_impact_dict = self.get_lang_item_dict(event_lang_path, 'en', 'EventEffect')
        evt_lang_cause_dict = self.get_lang_item_dict(event_lang_path, 'en', 'EventCause')
        reg_info = self.get_event_registry_info(event_lang_path)

        #从模板文件初始化注册表字典
        with self.open_file_redfish_event_registry_generator(evt_registry_template_path) as evt_registry_origin_data:
            evt_registry_jso = json.load(evt_registry_origin_data, object_pairs_hook=OrderedDict)

        if None == evt_registry_jso:
            print('Error open file at:%s'%(evt_registry_template_path))
            exit()

        evt_registry_jso['RegistryVersion'] = '%s.%s.%s'%(reg_info.major_ver, reg_info.minor_ver, reg_info.aux_ver)
        evt_registry_jso['Id'] = 'iBMCEvents.%s.%s.%s'%(reg_info.major_ver, reg_info.minor_ver, reg_info.aux_ver)

        registry_message_dict = evt_registry_jso['Messages']

        for obj_iter in objs:
            if ('EventInformation' == obj_iter.getAttribute('CLASS')):
                props = obj_iter.getElementsByTagName("PROPERTY")
                evt_info.objname = self.get_attrvalue(obj_iter, 'NAME')

                for prop_iter in props:
                    prop_name =  prop_iter.getAttribute('NAME')

                    child_node_array = prop_iter.getElementsByTagName('VALUE')[0].childNodes
                    if child_node_array:
                        prop_value = child_node_array[0].nodeValue
                    else:
                        prop_value = ''
                    if 'EventName' == prop_name:
                        evt_info.name = prop_value
                    if 'Severity' == prop_name:
                        evt_info.severity = prop_value
                    elif 'EventType' == prop_name:
                        evt_info.event_type = prop_value
                    elif 'EventCode' == prop_name:
                        evt_info.evt_id = prop_value
                    elif 'Description' == prop_name:
                        evt_info.msg_fmt = prop_value
                    elif 'Suggestion' == prop_name:
                        evt_info.suggestion = prop_value
                    elif 'EventEffect' == prop_name:
                        evt_info.impact = prop_value
                    elif 'EventCause' == prop_name:
                        evt_info.cause = prop_value

                #添加新的注册表记录
                record_item = self.generate_record_item(evt_info, evt_lang_desc_dict, evt_lang_sugg_dict, evt_lang_impact_dict, evt_lang_cause_dict)

                registry_message_dict[evt_info.name] = record_item

        #生成结果文件名，并写结束文件
        evt_registry_file_path = '%s/redfish/v1/registrystore/messages/en/ibmcevents.v%s_%s_%s.json' \
                                 % (self.config.common.temp_path, reg_info.major_ver, reg_info.minor_ver, reg_info.aux_ver)
        with self.open_file_redfish_event_registry_generator(evt_registry_file_path, 'w') as registry_out_file:
            json.dump(evt_registry_jso, registry_out_file, indent=4, sort_keys=False)
