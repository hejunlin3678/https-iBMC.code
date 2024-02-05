#!/usr/bin/python
# coding=utf-8

"""
功 能：该脚本实现差异化告警，入参为差异化event_lang_template_v2.xml路径
版权信息：华为技术有限公司，版本所有(C) 2019-2020
"""

import os
import sys
import xml.dom.minidom
from xml.dom.minidom import Node


class Differentiated_Alarm:

    def __init__(self, config):
        self.config = config
        self.event_attributes = ['EventDescription', 'EventSuggestion', 'EventEffect', 'EventCause']
        self.languages = ['en', 'zh']

    # xml格式调整
    def xml_min(self, node):
        for n in node.childNodes:
            if n.nodeType == Node.TEXT_NODE and n.nodeValue:
                n.nodeValue = n.nodeValue.strip()
            elif n.nodeType == Node.ELEMENT_NODE:
                self.xml_min(n)


    def get_attr_value(self, node, attr_name):
        return node.getAttribute(attr_name) if node else None


    def get_node_value(self, node, index=0):
        return node.childNodes[index].nodeValue if node and node.childNodes else None


    def get_xml_node(self,node, name):
        return node.getElementsByTagName(name) if node and len(node.childNodes) > 0 else []


    def write_item_into_doc(self, key, value, lang_set_iter, doc, item_type):
        evt_desc_node = self.get_xml_node(lang_set_iter, item_type)
        item_array = self.get_xml_node(evt_desc_node[0], 'Item')
        # 泰山修改的描述，在原处修改
        for item_array_iter in item_array:
            lang_str = self.get_attr_value(item_array_iter, 'NAME')
            if lang_str == key:
                if item_array_iter.childNodes:
                    item_array_iter.childNodes[0].data = value
                else:
                    text_node = doc.createTextNode(value)
                    item_array_iter.appendChild(text_node)
                return
        # 新增描述添加在最后
        item_node = doc.createElement('Item')
        item_node.setAttribute('NAME', key)
        text_node = doc.createTextNode(value)
        item_node.appendChild(text_node)
        evt_desc_node[0].appendChild(item_node)
        return


    def get_lang_item_dict(self, path, lang, item_type):
        if not os.path.exists(path):
            return

        evt_registry_item = {}
        doc = xml.dom.minidom.parse(path)
        root = doc.documentElement

        language_set_array = self.get_xml_node(root, 'LanguageSet')
        for lang_set_iter in language_set_array:
            if lang != self.get_attr_value(lang_set_iter, 'NAME'):
                continue
            evt_desc_node = self.get_xml_node(lang_set_iter, item_type)
            item_array = self.get_xml_node(evt_desc_node[0], 'Item')
            for item_array_iter in item_array:
                lang_str = self.get_attr_value(item_array_iter, 'NAME')
                evt_attribute = self.get_node_value(item_array_iter)
                if evt_attribute is None:
                    evt_attribute = ''
                evt_registry_item[lang_str] = evt_attribute

        return evt_registry_item


    def set_lang_to_xml(self, path, target_lang_path, evt_increment_dict, lang):
        doc = xml.dom.minidom.parse(path)
        root = doc.documentElement

        for attribute in self.event_attributes:
            language_set_array = self.get_xml_node(root, 'LanguageSet')
            for lang_set_iter in language_set_array:
                if lang != self.get_attr_value(lang_set_iter, 'NAME'):
                    continue
                for info_item in evt_increment_dict[attribute].keys():
                    self.write_item_into_doc(info_item, evt_increment_dict[attribute][info_item], lang_set_iter, doc, attribute)
        try:
            with open(path, 'w', encoding='UTF-8') as redfish_event_fh:
                self.xml_min(doc)
                doc.normalize()
                doc.writexml(redfish_event_fh, indent='', addindent='\t', newl='\n', encoding='UTF-8')

        except Exception as err:
            print('err：{err}'.format(err=err))

        try:
            with open(target_lang_path, 'w', encoding='UTF-8') as target_fh:
                doc.writexml(target_fh, indent='', addindent='\t', newl='\n', encoding='UTF-8')

        except Exception as err:
            print('err：{err}'.format(err=err))
        return


    def generate_event_registry_record(self, event_lang_diff_path):
        print(event_lang_diff_path)
        if not os.path.exists(event_lang_diff_path):
            print('can not find event_lang_arm_template_v2.xml nor event_lang_tianchi_template_v2.xml')
            return
        event_lang_path = '%s/application/src/rootfs/opt/pme/conf/language/event_lang_template_v2.xml' % self.config.common.code_root
        self.config.common.check_path(f"{self.config.rootfs_path}/opt/pme/conf/language")
        target_lang_path = '%s/opt/pme/conf/language/event_lang_template_v2.xml' % self.config.rootfs_path
        evt_increment_dict = {}

        for language in self.languages:
        # 获取属性信息
            for event_attribute in self.event_attributes:
                pangea_attribute_dict = self.get_lang_item_dict(event_lang_diff_path, language, event_attribute)
                trunk_attribute_dict = self.get_lang_item_dict(event_lang_path, language, event_attribute)
                increment_attribute_dict = {}
                for info_item in pangea_attribute_dict.keys():
                    # 新增部分
                    if trunk_attribute_dict.get(info_item) is None:
                        if pangea_attribute_dict.get(info_item) is not None and pangea_attribute_dict[info_item] != '':
                            increment_attribute_dict[info_item] = pangea_attribute_dict[info_item]
                        continue
                    # 修改部分
                    if trunk_attribute_dict[info_item] != pangea_attribute_dict[info_item]:
                        increment_attribute_dict[info_item] = pangea_attribute_dict[info_item]
                evt_increment_dict[event_attribute] = increment_attribute_dict

            # 增量部分写入event_lang_template_v2.xml
            self.set_lang_to_xml(event_lang_path, target_lang_path, evt_increment_dict, language)
            evt_increment_dict.clear()
