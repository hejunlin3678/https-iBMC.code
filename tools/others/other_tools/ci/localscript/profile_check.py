# Copyright © Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
# -*- coding: utf-8 -*-
import re
import xml.dom.minidom
import os,sys,glob,shutil,copy
import types

import xml.etree.ElementTree as et

from basicCmdClass import *
import multiprocessing
from multiprocessing import Pool, Lock, Value 
from check_event_code import checkEventCode
import imp



def open_file_profile_check(file_name, mode='r', encoding=None, **kwargs):
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

def str2int(value):
    value = value.lower()
    if -1 != value.find("0x"):
        base = 16
    else:
        base = 10

    num = None
    try:
        num = int(value, base)
    except:
        return None
    return num

def str2float(value):
    value = value.lower()
    num = None    
    try:
        num = float(value)
    except:
        try:
            num = str2int(value)
        except:
            return None
    return num


    
# XML读取操作的类
class xmlClass():
    def __init__(self, xmlfile):
        self.xmlfile = xmlfile
        self.open()

    def parsexml(self, xmlfile):
        try:
                return xml.dom.minidom.parse(xmlfile)
        except xml.parsers.expat.ExpatError as e:
                print('error xml ', xmlfile, e)
                raise e

    def open(self):        
        if not os.path.isfile(self.xmlfile):
            # 此处报错定位指南
            # 现象1： 报错文件路径为空，或者只有一个符号(,或者.) 
            #   原因1: 检查本文件中，对应单板在pdt2mxl中定义的值，确保list中的文件都是用,隔开并且没有任何空格和换行。
            #   原因2: 检查对应单板的profile.txt中是否存在多余空行，有的话需要删除。
            # 现象2： 报错显示为指定文件路径
            #   原因1: 确保此文件路径在代码库中是否存在/是否存在拼写错误，不存在的文件需要在pdt2mxl中或者profile.txt中删除。
            print("Invalid file path = ",self.xmlfile, ".\n")
            #self.createEmptyXml(self.xmlfile)
        self.xmldoc = self.parsexml(self.xmlfile)
    
    def write(self, filename = None):
        if None == filename:
            filename = self.xmlfile
        tmp = self.xmlfile + ".tmp"
        writeFile = open_file_profile_check(tmp, 'wb')
        writeFile.write(self.xmldoc.toxml('utf-8')) 
        writeFile.close()
        shutil.move(tmp, filename)

    def getXmlNode(self, layerPath):
        nodeList = layerPath.split("/")
        # 根节点
        thisNode = None
        fNode = self.xmldoc
        for nodeName in nodeList:
            thisNode = None
            for child in fNode.childNodes:
                if child.nodeName == nodeName:
                    thisNode = child
                    break
            if None == thisNode: raise AssertionError("")
            fNode = thisNode
        return thisNode

        

class Property:
    def __init__(self, classInfo, xmlName, obj, property, profileMap, err, mergeErr):
        self.classInfo = classInfo
        self.xmlName = xmlName
        self.obj = obj
        self.property = property
        self.property_name = self.property['NAME']
        self.type = property['TYPE']
        self.profileMap = profileMap
        self.err = err
        self.mergeErr = mergeErr


    def check_int_value(self, value):
        num = str2int(value)
        if num == None:
            self.err.printErr_Property("Error", self.obj, self.property_name, "invalid value. type %s, value = %s" %(self.type, value))
            return

        if num < self.min_val or num > self.max_val:
            self.err.printErr_Property("Error", self.obj, self.property_name, "value out of range %s ~ %s. type %s, value = %s" %(self.min_val, self.max_val, self.type, value))

    def check_value(self, value):
        return self.check_int_value(value)

        
    def type_match(self, type_in_obj, type_on_class):
        if "*" == type_on_class: return True
        return (type_in_obj == type_on_class)
        
    def check_type(self):
        if True != self.type_match(self.property['TYPE'], self.classInfo['PROPERTY'][self.property_name]['TYPE']):
            self.err.printErr_Property("Error", self.obj, self.property_name, "property type dismatch with class, type = '%s' should be '%s'." \
                %(self.property['TYPE'], self.classInfo['PROPERTY'][self.property_name]['TYPE']))

    def parseReferenceObject(self, value):
        if value[:2] == "::":
            refSelfXml = False #引用底板
            value = value[2:]
        else:
            refSelfXml = True #引用本文件(xml)中的其他对象属性
    
        return refSelfXml, value


    def check(self):
        if not self.property_name in self.classInfo['PROPERTY']:
            self.err.printErr_Property("Error", self.obj, self.property_name, "can't find this property in class.")
            return

        #非数组类型只能有一个值
        if self.type[0] != 'a' and self.type != 'roa':
            #if not 'a' in self.type[0]:
            if len(self.property['VALUE']) > 1:
                self.err.printErr_Property("Error", self.obj, self.property_name, "type %s value len > 1, value = %s" %(self.type, self.property['VALUE']))
        
        self.check_type()

        for value in self.property['VALUE']:
            #如果值不是全空格，那么前后不能有空格
            if len(value.strip()) > 0 and (value[0] == ' ' or value[-1] == ' '):
                oem_restart_cause = (self.obj['NAME'] != 'SysRestartByBmcUpgPcie' and self.obj['NAME'] != 'SysRestartByBmcFDM'
                    and self.obj['NAME'] != 'SysRestartByBmcFdmDiagSucc')
                if self.property_name != 'DescriptionDynamic' or oem_restart_cause:
                    #盘古产品告warning
                    if pangea_flag == 1:
                        self.err.printErr_Property("Warning", self.obj, self.property_name, "Please check : value start or end with a space, value = '%s' " %(value))
                    else:
                        self.err.printErr_Property("Error", self.obj, self.property_name, "Please check : value start or end with a space, value = '%s' " %(value))
            self.check_value(value)

        if 'RELATION_ACCESSOR' in self.property: 
            refSelfXml, ref_object_name = self.parseReferenceObject(self.property['RELATION_ACCESSOR'])
            if refSelfXml:
                refObjMap = self.profileMap[self.xmlName]
            else:
                refObjMap = self.profileMap['base'] #底板
            self.set_relation_accessor_flag(self.obj, refObjMap, ref_object_name)

    def set_relation_accessor_flag(self, obj, refObjMap, accessor_object_name):
        #允许同一个对象里面的两个属性引用
        if obj['NAME'] == accessor_object_name: return
        
        #不对->进行检查
        if accessor_object_name[:2] == "->": return
        
        if not accessor_object_name in refObjMap:
            # 北冰洋MockAcDownAssor定义在BASEBOARD_90.xml中，BASEBOARD_90.xml和BASEBOARD_90_01.xml不会同时扫描（因为重复对象太多），而检查BASEBOARD_90_01.xml时也会同时扫描接口卡xml，会报MockAcDownAssor的错误，已确认非问题
            if (pdt_local == "PangeaV6_Arctic") and (accessor_object_name == "MockAcDownAssor"): return
            # 太平洋大西洋暂时屏蔽PeriphCpldUpgradeAccessor和PeriphCpldValidateAccessor不存在的错误
            if (accessor_object_name == "PeriphCpldUpgradeAccessor") or (accessor_object_name == "PeriphCpldValidateAccessor") or (accessor_object_name == "CpldBmcUpAccessor"): return
            # 不支持1711设备解复位功能，不配置UnlockResetAccessor，UnlockResetAccessor项的错误属于非问题
            if (accessor_object_name == "UnlockResetAccessor") or (accessor_object_name == "BusAccessor"): return
            self.err.printErr_Object("Error", obj, "error RELATION_ACCESSOR object %s is not exist." %accessor_object_name)
            return
        
        if not 'RELATION' in refObjMap[accessor_object_name]:
            refObjMap[accessor_object_name]['RELATION'] = []
            
        if not obj['NAME'] in refObjMap[accessor_object_name]['RELATION']:
            refObjMap[accessor_object_name]['RELATION'].append(obj['NAME'])

class Property_y(Property):
    min_val = -128
    max_val = 255

class Property_b(Property):
    value_range = ["true", "false"]
    def check_value(self, value):
        if not value in self.value_range:
            self.err.printErr_Property("Error", obj, property_name, "invalid value. type %s, value = %s. Should be one of %s" %(self.type, property['VALUE']), self.value_range)

class Property_n(Property):
    min_val = -32768
    max_val = 32767

class Property_q(Property):
    min_val = 0
    max_val = 65535

class Property_i(Property):
    min_val = -2147483648
    max_val = 2147483647

class Property_u(Property):
    min_val = 0
    max_val = 4294967295

class Property_d(Property):
    def check_value(self, value):
        num = str2float(value)
        if num == None:
            self.err.printErr_Property("Error", obj, property_name, "invalid value. type %s, value = %s" %(self.type, property['VALUE']))
        return

class Property_s(Property):
    def check_value(self, value):
        pass

class Property_ay(Property_y):
    pass
    
class Property_as(Property_s):
    pass

class Property_aq(Property_q):
    pass

class Property_ad(Property_d):
    pass

class Property_ai(Property_i):
    pass

class Property_au(Property_u):
    pass

class Property_an(Property_n):
    pass

class Property_ai(Property_i):
    pass

class Property_rp(Property):

    def check_type(self):
        pass
    
    # rp引用可能是一个单向链表。即:被引用的对象可能又是一个rp指向下一个。。。
    # 获取到最后一个被引用的实体属性
    def get_rp_property_chan(self, xmlName, object_name, property_name, rp_chan = None):
        if None == rp_chan: rp_chan = []
        if [xmlName, object_name, property_name] in rp_chan:
            rp_chan.append([xmlName, object_name, property_name])
            self.err.printErr_Property("Error", self.profileMap[xmlName][object_name], property_name, "error loop rp reference: %s." %rp_chan)
            #print rp_chan
            #raise AssertionError, ""
            return None
        
        rp_chan.append([xmlName, object_name, property_name])
    
        obj = self.profileMap[xmlName][object_name]
        property = obj['PROPERTY'][property_name]
        
        #普通类型直接返回
        if property['TYPE'] != 'rp': 
            #print rp_chan
            #raise AssertionError, ""
            return rp_chan
        
        #允许为空
        if len(property['VALUE']) == 0: return rp_chan
        
        #不对->进行检查
        if property['VALUE'][0][:2] == "->": return rp_chan
    
        #解析rp属性值
        refwarnlist = []
        refSelfXml, ref_object_name, reference_property = self.parseReferenceProperty(property['VALUE'][0])
        if None == refSelfXml:
            self.err.printErr_Property("Error", obj, property_name, "error rp property value number. value = %s" %property['VALUE'][0])
            return None
        if refSelfXml:
            next_xmlName = xmlName
        else:
            next_xmlName = 'base' #底板
        refObjMap = self.profileMap[next_xmlName]
    
        if not ref_object_name in refObjMap:
            # 太平洋ChassisTempWarning定义在BASEBOARD_17.xml中，确认为非问题
            if (pdt_local == "PangeaV6_Pacific") and (ref_object_name == "ChassisTempWarning"): return rp_chan
            refwarnstr = ("Warning file = %s class = %s object = %s property = %s error rp reference.object %s is not exist. value = %s" % (obj['FILE'], obj['CLASS'], obj['NAME'], property_name, ref_object_name, property['VALUE'][0]))
            refwarnlist.append(refwarnstr)
            self.err.printErr_Warning(refwarnlist)
            #self.mergeErr.printErr_Property("Error", obj, property_name, "error rp reference.object %s is not exist. value = %s" %(ref_object_name, property['VALUE'][0]))
            return None
        
        virobjloselist = []
        if not reference_property in refObjMap[ref_object_name]['PROPERTY']:
            if 'VIRTUAL' not in refObjMap[ref_object_name]:
                self.err.printErr_Property("Error", obj, property_name, "error rp reference.property %s.%s is not exist. value = %s" %(ref_object_name, reference_property, property['VALUE'][0]))
            else:
                objlose = ("Warning file = %s class = %s object = %s property = %s error rp reference.property %s.%s is not exist. value = %s." % (obj['FILE'], obj['CLASS'], obj['NAME'], property_name,ref_object_name, reference_property, property['VALUE'][0]))
                virobjloselist.append(objlose)
                #print virobjloselist
                self.err.printErr_Warning(virobjloselist)
            return None
        
        #对被引用的对象设置关联关系
        self.set_relation_accessor_flag(obj, refObjMap, ref_object_name)
        
        return self.get_rp_property_chan(next_xmlName, ref_object_name, reference_property, rp_chan)

    def parseReferenceProperty(self, value):
        if value[:2] == "::":
            refSelfXml = False #引用底板
            value = value[2:]
        else:
            refSelfXml = True #引用本文件(xml)中的其他对象属性
            
        temp = value.split(".")
        if len(temp) != 2:    return None, None, None
        
        ref_object_name = temp[0].strip(".")
        reference_property = temp[1]
        #三大洋产品支持引用对象-编号，例如BackplaneElabel-1，确认非问题
        if pangea_flag == 1:
            tmp = ref_object_name.split("-")
            ref_object_name = tmp[0]
        return refSelfXml, ref_object_name, reference_property

    def get_rp_type(self):

        #获取被引用的属性链表
        rp_chan = self.get_rp_property_chan(self.xmlName, self.obj['NAME'], self.property_name)
        if None == rp_chan: return
        
        #检查类型是否匹配
        ref_xml_name = rp_chan[-1][0]
        tail_object_name = rp_chan[-1][1]
        tail_property_name = rp_chan[-1][2]
    
        tail_type = self.profileMap[ref_xml_name][tail_object_name]['PROPERTY'][tail_property_name]['TYPE']
        
        return tail_type
        
    def check_value(self, value):

        #获取被引用的属性链表
        rp_chan = self.get_rp_property_chan(self.xmlName, self.obj['NAME'], self.property_name)
        if None == rp_chan: return
        
        #检查类型是否匹配
        ref_xml_name = rp_chan[-1][0]
        tail_object_name = rp_chan[-1][1]
        tail_property_name = rp_chan[-1][2]
    
        tail_type = self.profileMap[ref_xml_name][tail_object_name]['PROPERTY'][tail_property_name]['TYPE']
        if (tail_type != 'rp'):
            if not self.type_match(tail_type, self.classInfo['PROPERTY'][self.property_name]['TYPE']):
                self.err.printErr_Property('Error', self.obj, self.property_name, "property type dismatch with class, type = '%s' should be '%s'. reference to: %s" %(tail_type, self.classInfo['PROPERTY'][self.property_name]['TYPE'], rp_chan[1:]))
        return

        self.set_relation_accessor_flag(obj, refObjMap, ref_object_name)

class Property_ro(Property):
    def check_value(self, value):
        objMap = self.profileMap[self.xmlName]
        refwarnlist = []
        #不对->进行检查
        if value[:2] == "->": return
        if value[:2] == "::": return
        
        refSelfXml, ref_object_name = self.parseReferenceObject(value)
        if refSelfXml:
            refObjMap = self.profileMap[self.xmlName]
        else:
            refObjMap = self.profileMap['base'] #底板
    
        if not ref_object_name in refObjMap:
            refwarnstr = ("Warning file = %s class = %s object = %s property = %s error ro reference.object %s is not exist. value = %s" % (self.obj['FILE'], self.obj['CLASS'], self.obj['NAME'], self.property['NAME'], ref_object_name, value))
            refwarnlist.append(refwarnstr)
            self.err.printErr_Warning(refwarnlist)
            #self.mergeErr.printErr_Property("Error", self.obj, self.property['NAME'], "error ro reference.object %s is not exist. value = %s" %(ref_object_name, value))
            return

        self.set_relation_accessor_flag(self.obj, refObjMap, ref_object_name)

class Property_aro(Property_ro):
    pass


propertyMap = {
    'y': Property_y,
    'b': Property_b,
    'n': Property_n,
    'q': Property_q,
    'i': Property_i,
    'u': Property_u,
    'd': Property_d,
    's': Property_s,
    'ay': Property_ay,
    'as': Property_as,
    'aq': Property_aq,
    'ad': Property_ad,
    'au': Property_au,
    'ai': Property_ai,
    'an': Property_an,
    'ai': Property_ai,
    'ro': Property_ro,
    'rp': Property_rp,
    'aro': Property_aro,
    'roa': Property_aro,
};

def printAll(ownerErrMsg = None):
    if None == ownerErrMsg:
        ownerErrMsg = ownerErrMsg
    for owner, msgList in list(ownerErrMsg.items()):
        #print "\n责任人 %s 所属类的错误信息如下:" %owner
        for msg in msgList:
            print(msg)

    #print "\nSummary:"
    #for owner, msgList in ownerErrMsg.items():
        #print "责任人 %s 错误数 %d" %(owner, len(msgList))

class ErrPrint:
    def __init__(self, ownerMap):
        self.ownerErrMsg = {}
        self.ownerMap = ownerMap
        self.ownerWarningMsg = {}
        self.ownerWarningMsg["Warning"] = []

    def printErr(self, level, msg, classname = "unknow_class", fileName = "unknow_xml", object = None, property = None):
        errMsg = ""
        ownerMsg = ""

        errMsg +="[%s] %sfile = %s class = %s" %(level, ownerMsg, fileName, classname)

        if None != object:
            errMsg +=" object = %s" %(object)
        
        if None != property:
            errMsg +=" property = %s" %(property)
        
        errMsg += ", %s" %msg
        #print errMsg
        if classname in self.ownerMap:
            owner = self.ownerMap[classname]
        else:
            owner = "unknow"

        if not owner in self.ownerErrMsg:
            self.ownerErrMsg[owner] = []
        self.ownerErrMsg[owner].append(errMsg)
        
    def printAll(self, ownerWarningMsg = None, ownerErrMsg = None):
        printAll(ownerWarningMsg)
        printAll(ownerErrMsg)

    def printErr_Property(self, level, obj, property, msg, mergeErr = False):
        self.printErr(level, msg, classname = obj['CLASS'], fileName = obj['FILE'], object = obj['NAME'], property = property)

    def printErr_Object(self, level, obj, msg, mergeErr = False):
        self.printErr(level, msg, classname = obj['CLASS'], fileName = obj['FILE'], object = obj['NAME'])
        
    def printErr_Class(self, level, classname, msg, mergeErr = False):
        self.printErr(level, msg, classname = classname, fileName = "class.xml")

    def printErr_xml(self, level, classname, msg, filename, mergeErr = False):
        self.printErr(level, msg, classname=classname, fileName=filename)
    
    def printErr_Warning(self, Warning_msglist):
        for Warning_msg in Warning_msglist:
            if Warning_msg not in self.ownerWarningMsg["Warning"]:
                self.ownerWarningMsg["Warning"].append(Warning_msg)


propertyName = {0: ("LowerNoncritical", "LowerNoncriticalConvert"),
                2: ("LowerCritical", "LowerCriticalConvert"),
                4: ("LowerNonrecoverable", "LowerNonrecoverableConvert"),
                7: ("UpperNoncritical", "UpperNoncriticalConvert"),
                9: ("UpperCritical", "UpperCriticalConvert"),
                11: ("UpperNonrecoverable", "UpperNonrecoverableConvert")}


class ProfileCheck:
    #通过XML的属性节点，读取属性的值，放在一个数组中返回。
    def get_property_value(self, propertyNode):
        valueList = []

        if propertyNode.hasAttribute("VALUE"):
            valueList.append(propertyNode.getAttribute("VALUE"))
        else:
            valueNodeList = propertyNode.getElementsByTagName("VALUE")
            for valueNode in valueNodeList:
                for value in  valueNode.childNodes:
                    valueList.append(value.nodeValue)
        
        return valueList

    def new_prop(self, class_name):
        prop_array = {}
        if class_name not in self.classMap:
            return prop_array
        prop_def = self.classMap[class_name]["PROPERTY"]
        for prop_iter in prop_def:
            prop_array[prop_iter] = {}
            prop_array[prop_iter]["TYPE"] = prop_def[prop_iter]["TYPE"]
            prop_array[prop_iter]["NAME"] = prop_iter
            prop_array[prop_iter]["VALUE"] = ""
        
        return prop_array
        
    #生成一个空的对象节点
    def new_obj(self, name, class_name, prop_array, type, xmlFile):
        obj = {}
        obj["NAME"] = name
        obj["CLASS"] = class_name
        obj['TYPE'] = type
        obj["PROPERTY"] = prop_array
        obj['FILE'] = xmlFile
        return obj

    #获取某虚拟连接器可以加载的配置文件表
    def get_virtual_xml_list_by_bom(self, bom_value, profileDir, xmllist_form_file, baseXmlList):
        virtualXmlList = []
        virtualXmlExistInBaseXmlList = False            

        for xmlPrefix in bom_value:
            #xmlMatch = "%s\\%s_*.xml" %(profileDir, xmlPrefix)
            #xmlList = glob.glob(xmlMatch)
            xmlMatch = "%s_*.xml" %(xmlPrefix)
            
            virtualXmlExistInBaseXmlList = self.get_virtualXml_in_baseXmlList_ExistFlag(baseXmlList, xmlPrefix)
            if virtualXmlExistInBaseXmlList == True:
                continue

            xmlList = self.get_xmlMatch(profileDir, xmlPrefix)
            if len(xmlList) == 0:
                #找不到相应的扣卡的XML，需报错
                print("Info: When load XML for VirtualConnector with Bom = %s, can't find xml %s" %(xmlPrefix, xmlMatch))
                continue
            
            tmpList = []
            matchWithProfilelist = False
            for xmlFileName in xmlList:
                #必须是在profile.txt中的才添加。但是，如果profile.txt中一个符合条件的都没有，就报错miss
                #if not  os.path.basename(xmlFileName) in xmllist_form_file:
                file_name_unix = self.profilename2unixformat(profileDir, xmlFileName)
                if not  file_name_unix in xmllist_form_file:
                    #print "skip %s" %xmlFileName
                    continue
                matchWithProfilelist = True

                tmpList.append(xmlFileName)
            
            #如果profilelist.txt中一个符合条件的都没有，就报错miss
            if not matchWithProfilelist:
                print("Error: When load XML for VirtualConnector with Bom = %s, can't find xml %s in profile.txt" %(xmlPrefix, xmlMatch))
            
            virtualXmlList += tmpList
        
        #去重复
        virtualXmlList = list(set(virtualXmlList))

        return virtualXmlList

    def read_xml(self, filepath):
        """
        :param filepath: xml路径
        :return: 根节点
        """
        try:
            with open(filepath, 'r', encoding="utf-8") as fs:
                data = fs.read()
            root = et.fromstring(data)
            return root
        except Exception as e:
            self.err.printErr_xml("Error", "", "%s can't open the xml" %self.get_filename(filepath), self.get_filename(filepath))


    # 获取目标节点的bit的列表
    def get_index(self, node_list):
        index_list = []
        for node in node_list:
            if node.attrib.get("NAME", None) == "AssertionMask":
                num = list(node)[0].text
                value = bin(int(eval(num)))
                value = value[::-1]
                for i, v in enumerate(value[:12]):
                    if v == "1":
                        index_list.append(i)
        return index_list

    # 判断属于propertyName枚举中的哪一类
    def get_value_node(self, node, key):
        if node.attrib.get("NAME", None) == key:
            if node.attrib.get("TYPE", None) == "rp":
                return "1"
            else:
                value = list(node)[0].text
                return value

    # 判断value值是否是数字(十进制或十六进制的数字)
    def value_different(self, value):
        if not isinstance(value, str):
            value = str(value)
        pattern = re.compile(r"0[\.0-9A-Fa-f]*?")
        if re.search(pattern, value):
            return True

    # 判断该ThresholdSensor类型的object节点是否达到传感器告警门限
    def judge_node(self, node_list, nodename, filename):
        index_list = self.get_index(node_list)
        value1 = ""
        value2 = ""
        value3 = ""
        value4 = ""
        for key, value in propertyName.items():
            for node in node_list:
                if key in index_list:
                    if self.get_value_node(node, value[0]):
                        value1 = self.get_value_node(node, value[0])
                    if self.get_value_node(node, value[1]):
                        value2 = self.get_value_node(node, value[1])
                else:
                    if self.get_value_node(node, value[0]):
                        value3 = self.get_value_node(node, value[0])
                    if self.get_value_node(node, value[1]):
                        value4 = self.get_value_node(node, value[1])

            if value1 and value2:
                if self.value_different(value1):
                    value1 = float(eval(str(value1)))
                if self.value_different(value2):
                    value2 = float(eval(str(value2)))
                if value1 != 0 and value2 == 0:
                    self.err.printErr_xml("Error", "ThresholdSensor", "%s对象中被置位的%s和%s的值不一致" % (
                        nodename, value[0], value[1]), filename=filename)
                elif value1 == 0 and value2 != 0:
                    self.err.printErr_xml("Error", "ThresholdSensor", "%s对象中被置位的%s和%s的值不一致" % (
                        nodename, value[0], value[1]), filename=filename)
            elif value3 and value4:
                if self.value_different(value3):
                    value3 = float(eval(str(value3)))
                if self.value_different(value4):
                    value4 = float(eval(str(value4)))
                if value3 != 0 or value4 != 0:
                    self.err.printErr_xml("Error", "ThresholdSensor", "%s对象中未被置位的%s和%s的值应该为0" % (
                        nodename, value[0], value[1]), filename=filename)
            elif value1 == "" and value2 != '':
                value2 = float(eval(str(value2)))
                if value2 != 0:
                    self.err.printErr_xml("Error", "ThresholdSensor", "%s对象中被置位的%s不能为空" % (
                        nodename, value[0]), filename=filename)
            elif value2 == "" and value1 != "":
                value1 = float(eval(str(value1)))
                if value1 != 0:
                    self.err.printErr_xml("Error", "ThresholdSensor", "%s对象中被置位的%s不能为空" % (
                        nodename, value[1]), filename=filename)
            elif value3 == "" and value4 != '':
                value4 = float(eval(str(value4)))
                if value4 != 0:
                    self.err.printErr_xml("Error", "ThresholdSensor", "%s未被置位的%s不能为空" % (
                        nodename, value[0]), filename=filename)
            elif value4 == "" and value3 != "":
                value3 = float(eval(str(value3)))
                if value3 != 0:
                    self.err.printErr_xml("Error", "ThresholdSensor", "%s未被置位的%s不能为空" % (
                        nodename, value[1]), filename=filename)

    def get_filename(self, name):
        # 辅助打印报错文件名称
        pattern = re.compile(r".*?([^/\\]+\.xml)")
        name = r"%s" %name
        if re.search(pattern, name):
            return re.search(pattern, name).group(1)

    def check_xml_sensor(self, xmlName):
        # 调用sensor门限判断
        root = self.read_xml(xmlName)
        obj_list = list(root)
        for obj in obj_list:
            if obj.attrib.get('CLASS', None) == "ThresholdSensor":
                pro_list = list(obj)
                self.judge_node(pro_list, obj.attrib.get('NAME'), self.get_filename(xmlName))


    #读取对象XML
    #如果loadVirtualFlag为True,profileDir和xmllist_form_file必须指定
    def read_obj_xml(self, xmlFile, objMap = None, loadVirtualFlag = False, profileDir = None, xmllist_form_file = None, baseXmlList = None, need_load = True):
        externVirtualXmlList = []
        if None == objMap: objMap = {}
        #print "Read %s ..." %xmlFile

        srcXml = xmlClass(xmlFile)
        rootNode = srcXml.getXmlNode("OBJECTS_CONFIG")
        for objectNode in rootNode.getElementsByTagName("OBJECT"):
            object_name = objectNode.getAttribute("NAME")
            TianChiRiser_flag = 0
            if object_name in objMap:
                VirtualConnectorLoadXmlList = {
                    'e5':['PCIE_NCSI_00.xml'],'d1':['PSUIDENTIFY_00.xml'],'e6':['OCP_CARD_NCSI_01.xml'],'e7':['OCP_CARD_NCSI_02.xml']
                }
                TianChiRiserList = {
                    "KP_Riser","QZ_Riser"
                }
                for positionnum,filenameList in VirtualConnectorLoadXmlList.items():
                    if os.path.basename(xmlFile) in filenameList:
                        object_name = object_name + positionnum
                        break
                for TianChiRiser in TianChiRiserList:
                    if TianChiRiser in os.path.basename(xmlFile):
                        TianChiRiser_flag = 1
                        break
                if TianChiRiser_flag: continue
                if object_name in objMap:
                    #盘古告Warning
                    if pangea_flag == 1:
                        self.err.printErr_Object("Warning", objMap[object_name], "repeat in %s" %os.path.basename(xmlFile))
                    else:
                        self.err.printErr_Object("Error", objMap[object_name], "repeat in %s" %os.path.basename(xmlFile))
                #可能的原因1：非虚拟连接器加载的XML（比如PCIE网卡）放在板卡的pdt2mxl（即放在了resource/board/机型目录）里，
                #            这里不应该放在板卡的pdt2mxl里，而是放在resource/profile/机型目录里，并在profile.txt里添加
            
            if need_load == False: continue
            
            objMap[object_name] = self.new_obj(object_name, objectNode.getAttribute("CLASS"), {}, objectNode.getAttribute("TYPE"), os.path.basename(xmlFile))

            for property in objectNode.getElementsByTagName("PROPERTY"):
                property_name = property.getAttribute("NAME")
                if property_name in objMap[object_name]["PROPERTY"]:
                    self.err.printErr_Property("Error", objMap[object_name], property_name, "[ERROR] duplicate property.")

                objMap[object_name]["PROPERTY"][property_name] = {}
                objMap[object_name]["PROPERTY"][property_name]["VALUE"] = self.get_property_value(property)
                objMap[object_name]["PROPERTY"][property_name]["TYPE"] =  property.getAttribute("TYPE")
                objMap[object_name]["PROPERTY"][property_name]["NAME"] =  property_name
                if property.hasAttribute("RELATION_ACCESSOR"):
                    objMap[object_name]["PROPERTY"][property_name]["RELATION_ACCESSOR"] =  property.getAttribute("RELATION_ACCESSOR")
                #    set_relation_accessor_flag(objMap, property.getAttribute("RELATION_ACCESSOR"), object_name)
        
            if loadVirtualFlag != True: continue
            
            #本质上虚拟连接器要加载的配置文件是当前配置文件的扩展,因此需要加载
            if objMap[object_name]['CLASS'] != "Connector": continue

            #对部分xml来说，连接时有属于其的VirtualConnector连接器和非VirtualConnector连接器，
            #检测脚本按VirtualConnector连接器去读取xml会有误报产生，按非VirtualConnector连接器去读取则不会
            #所以这里遇到连接这部分xml的VirtualConnector连接器选择跳过，后面会按VirtualConnector连接器去读取xml，不会漏检
            NoneedLoadConnectorList = ["V_RaidCardConnect","V_RaidCard1Connect","V_RaidCard2Connect","OCPConnector","PCIeNCSIVirConnector","VHddBackplaneConnect","VHddBackplane2Connect","VPsuHddBackCardConnect"]
            #由代码创建的XML不需要检测
            NotExistXmlList = ["OSEthConnector","FCCardConnector","FCPortConnector","DrivePartitionConnector","RaidControllerConnector","LogicalDriveDynamicConnector","VLANConnector","DiskArrayDynamicConnector","LogicalDriveConnector","DiskArrayConnector","SMM1Connect","LicenseConnector"]
            if objMap[object_name]['NAME'] in NotExistXmlList or objMap[object_name]['NAME'] in NoneedLoadConnectorList: continue

            if "VirtualConnector" in objMap[object_name]['PROPERTY']['Type']['VALUE']:
                self.virtualConnectorNeeded = True
                tmpList = []
                tmpList = self.get_virtual_xml_list_by_bom(objMap[object_name]['PROPERTY']['Bom']['VALUE'], profileDir, xmllist_form_file, baseXmlList)
                if len(tmpList) != 0:
                    for xmlFileItem in tmpList:
                        filename = os.path.basename(xmlFile)
                        vfilename = os.path.basename(xmlFileItem)
                        #print "Info: Find \"%s\" for \"%s\" in \"%s\" ..." %(vfilename, object_name, filename)
                        #当前某虚拟连接器如果可以加载多个配置文件,这几个配置文件因为部件类型相同所以设置的对象会重名, 因此只加载最后一个进行检测
                        externVirtualXmlList += self.read_obj_xml(xmlFileItem, objMap, True, profileDir, xmllist_form_file, baseXmlList, False)
                        
                    externVirtualXmlList += self.read_obj_xml(xmlFileItem, objMap, True, profileDir, xmllist_form_file, baseXmlList)

                    externVirtualXmlList += tmpList        
                
        #去重复
        externVirtualXmlList = list(set(externVirtualXmlList))
        #大西洋和太平洋sensor对象12_0V_VCC_1  12_0V_VCC_2 VCC_12V_DET FIX_CPU0_0V8有做特殊处理，跳过此项检测
        if (pdt_local != "PangeaV6_Atlantic") and (pdt_local != "PangeaV6_Pacific"):
            self.check_xml_sensor(xmlFile)
        return externVirtualXmlList

    def read_class_node(self, classNode, basicMap = None):
        classname = classNode.getAttribute("NAME")
        if classname in self.classMap:
            self.err.printErr_Class("Error", classname, "repeated class.")

        if None == basicMap:
            self.classMap[classname] = {}
        else:
            self.classMap[classname] = copy.deepcopy(basicMap)
        
        if classname in ["Accessor", "Chip", "Bus"]:
            self.classMap[classname]['TYPE'] = "Physical"
        
        self.classMap[classname]['NAME'] = classname
        if not 'PROPERTY' in self.classMap[classname]:
            self.classMap[classname]["PROPERTY"] = {}
        
        if classname in self.ownerMap:
            self.classMap[classname]["OWNER"] = self.ownerMap[classname]

        for property in classNode.childNodes:
            if not property.nodeName == "PROPERTY": continue
            property_name = property.getAttribute("NAME")
            if property_name in self.classMap[classname]["PROPERTY"]:
                # 盘古产品从单板xml路径下寻找Connector对应的xml文件
                if pangea_flag == 1:
                    self.err.printErr_Class("Warning", classname, "repeated property %s" %(property_name))
                else:
                    self.err.printErr_Class("Error", classname, "repeated property %s" %(property_name))
                
            if not property_name in self.classMap[classname]["PROPERTY"]:
                self.classMap[classname]["PROPERTY"][property_name] = {}

            self.classMap[classname]["PROPERTY"][property_name]["TYPE"] =  property.getAttribute("TYPE")

        for subClass in classNode.childNodes:
            if not subClass.nodeName == "CLASS": continue
            #getElementsByTagName("CLASS")
            self.read_class_node(subClass, self.classMap[classname])

    def read_class_xml(self, xmlFile):
        srcXml = xmlClass(xmlFile)
        rootNode = srcXml.getXmlNode("CLASSTREE")
        for classNode in rootNode.childNodes:
            if not classNode.nodeName == "CLASS": continue
            self.read_class_node(classNode)
        
    #return self.classMap
    def check_property(self, classInfo, xmlName, obj, property):
        property_name = property['NAME']
        if not property_name in classInfo['PROPERTY']:
            self.err.printErr_Property("Error", obj, property_name, "can't find this property in class.")
            return
        
        #类型检查。 rp的属性类型匹配比较复杂, 不在此处理
        if property['TYPE'] not in propertyMap:
            self.err.printErr_Property("Error", obj, property_name, "unknow property type %s" %property['TYPE'])
            return

        property_class = propertyMap[property['TYPE']]
        p = property_class(classInfo, xmlName, obj, property, self.profileMap, self.err, self.mergeErr)
        p.check()
        
    #检查 EventMonitor 类型是否匹配
    def check_type_for_eventmonitor(self, type1, type2):
        #typelist = ["y","q","u","n","i","d"]
        if type1 == type2:
            return True
        return False

    #检查对象是否有不符合规范之处
    def check_object(self, xmlName, obj):
        #查看对象的CLASS是否存在
        if not obj['CLASS'] in self.classMap:
            self.err.printErr_Object("Error", obj, "can't find this class in class.xml.")
            return

        classInfo = self.classMap[obj['CLASS']]            
        #检查对象中填写的TYPE与类中写的TYPE是否一致
        if 'TYPE' in classInfo and obj['TYPE'] != classInfo['TYPE']:
            self.err.printErr_Object("Error", obj, "object type is '%s' should be '%s'." %(obj['TYPE'], classInfo['TYPE']))

        # 虚拟对象，不做进一步检查
        if 'VIRTUAL' in obj: return
        
        # 检查CLASS中的属性，在OBJECT中是否有缺失。
        for property_name in classInfo['PROPERTY']:
            if (not property_name in obj['PROPERTY']) and not (obj['CLASS'] in self.class_whitelist and 
            property_name in self.property_whitelist):
                self.err.printErr_Object("Error", obj, "missing property '%s'." %(property_name))
        
        #逐个检查对象中的属性
        for property_name, property in list(obj['PROPERTY'].items()):
            self.check_property(classInfo, xmlName, obj, property)
            
    #Anchor类对象 Bus属性 的值，就是一个新的I2C对象
    def AnchorPreObj_to_local_object(self, type_prop, class_name):
        for xmlName, objMap in list(self.profileMap.items()):
                #逐个检查object
                for objName, obj in list(objMap.items()):
                    if obj['CLASS'] != "Anchor": continue
                    if not type_prop in obj['PROPERTY']: continue
                    for val in obj['PROPERTY'][type_prop]['VALUE']:
                        if val in objMap:
                            WainingStrList = []
                            WarningStr = ("[Info] file = %s class = %s object = %s Anchor.%s try to add new object %s." % (obj['FILE'], obj['CLASS'], obj['NAME'], type_prop, val))
                            WainingStrList.append(WarningStr)
                            self.err.printErr_Warning(WainingStrList)
                            #self.err.printErr_Object("Error", obj, "Anchor.%s try to add new object %s , collision with exist object." %(type_prop, val))
                            continue
                            
                        default_prop = self.new_prop(class_name)
                        objMap[val] = self.new_obj(val, class_name, default_prop, 'Physical', xmlName)
                        objMap[val]['VIRTUAL'] = True #虚拟对象


    def get_virtualXml_in_baseXmlList_ExistFlag(self, baseXmlList, xmlPrefix):
        virtualXmlExistInBaseXmlList = False
        for baseXml in baseXmlList:
            baseXmlName = os.path.basename(baseXml)
            if baseXmlName[:len(xmlPrefix)] == xmlPrefix:
                virtualXmlExistInBaseXmlList = True
                break
        
        return virtualXmlExistInBaseXmlList
    
    def get_xmlMatch(self, profileDir, xmlPrefix):
        xmlPrefix = xmlPrefix + "_"
        xmlList = []
        for root, dirs, files in os.walk(profileDir, True):
            for name in files:
                tmp = name.split(".")
                if tmp[-1].lower() != "xml": continue
                if tmp[0][:len(xmlPrefix)] == xmlPrefix:
                    xmlList.append(os.path.join(root, name))
        return xmlList

    def get_xmlMatch_from_board_dir(self, xmlPrefix):
        localDir = os.getcwd() # 工具路径为../V2R2_trunk/tools;或者../V2R2_trunk/application，需要分情况拼接文件路径
        fatherDir = localDir[-5:] # 取当前目录名

        if (fatherDir == "tools"): # 当前工具在tools路径下
            workDir = localDir[:-5] + "application" # ../V2R2_trunk/application
        else: # 当前工具在application路径下
            workDir = localDir
        xmlPrefix = xmlPrefix + "_"
        xmlList = []
        for root, dirs, files in os.walk("%s/src/resource/board/%s" %(workDir, pdt_local), True):
            for name in files:
                tmp = name.split(".")
                if tmp[-1].lower() != "xml": continue
                if tmp[0][:len(xmlPrefix)] == xmlPrefix:
                    xmlList.append(os.path.join(root, name))
        return xmlList

    def profilename2unixformat(self, profileDir, xmlFileName):
        return xmlFileName[len(profileDir) + 1:].replace("\\", "/")
    
    # 配置在pdt2xml中、不通过VirtualConnector加载基础的xml，已在baseXmlList中检查
    def is_pdt_base_xml_conn(self, obj):
        # 满足excludedMaps的Connector将返回结果True，随后不在profile目录下找扩展卡XML
        # excludedMaps中key是机型名，value是该机型Connector的Type属性值列表，excludedMaps结构：{"boardName":["typeValue"]}
        excludedMaps = {
            'xh321v6': ["HddBackConnect", "IOBoardConnect", "FanBoardTest"],
            'RM211': ["ToolBoardConnect"]
        }
        filterResult = False
        for typeValue in obj['PROPERTY']['Type']['VALUE']: 
            # 检查Type属性值是否在配置的excludedMaps中
            if typeValue in excludedMaps.get(self.pdt, []):
                filterResult = True
        return filterResult

    def get_externboard_by_Connector(self, externXmlListOld, profileDir, xmllist_form_file):
        externXmlList = []
        
        for xmlKey, xmlObjs in list(self.profileMap.items()):
            for objName, obj in list(xmlObjs.items()):
                if obj['CLASS'] != "Connector": continue
                if not "Bom" in obj['PROPERTY']: 
                    self.err.printErr_Object("Error", obj, "missing property 'Bom'.")
                    continue
                
                #下面这种情况找底板XML，不找扩展卡XML
                #虚拟连接器的Position不是必须为0，当虚拟连接器加载的XML文件中的对象和虚拟连接器所在XML文件的对象在同一层时，Posistion需要配置成0，
                #如果不在同一层时，Position配置成非零值
                #if "VirtualConnector" in obj['PROPERTY']['Type']['VALUE'] and "0x00" in obj['PROPERTY']['Position']['VALUE']:
                if "VirtualConnector" in obj['PROPERTY']['Type']['VALUE']:
                    continue
                if "VirFanConnect" in obj['PROPERTY']['Type']['VALUE'] and "0x00" in obj['PROPERTY']['Position']['VALUE']:
                    continue                     
                if "PTASconnector" in obj['PROPERTY']['Type']['VALUE'] and "0xff" in obj['PROPERTY']['Position']['VALUE']:
                    continue 
                if "VirMultiConnector" in obj['PROPERTY']['Type']['VALUE'] :
                    continue                     
                if self.is_pdt_base_xml_conn(obj):
                    continue

                bomValue = obj['PROPERTY']['Bom']['VALUE']
                for xmlPrefix in bomValue:
                    #xmlMatch = "%s\\%s_*.xml" %(profileDir, xmlPrefix)
                    #xmlList = glob.glob(xmlMatch)
                    xmlMatch = "%s_*.xml" %(xmlPrefix)
                    # 盘古产品从单板xml路径下寻找Connector对应的xml文件
                    if pangea_flag == 1:
                        xmlList_board = self.get_xmlMatch_from_board_dir(xmlPrefix)
                        if len(xmlList_board) != 0: continue
                    # 大西洋没有POWERMGNT_*.xml，属于非问题
                    if (pdt_local == "PangeaV6_Atlantic") and (xmlPrefix == "POWERMGNT"): continue
                    xmlList = self.get_xmlMatch(profileDir, xmlPrefix)
                    if len(xmlList) == 0:
                        #找不到相应的扣卡的XML，需报错
                        self.err.printErr_Object("Error", obj, "Bom = %s, but can't find xml %s" %(xmlPrefix, xmlMatch))
                        continue
                    
                    tmpList = []
                    matchWithProfilelist = False
                    for xmlFileName in xmlList:
                        #必须是在profile.txt中的才添加。但是，如果profile.txt中一个符合条件的都没有，就报错miss
                        #if not  os.path.basename(xmlFileName) in xmllist_form_file:
                        file_name_unix = self.profilename2unixformat(profileDir, xmlFileName)
                        if not  file_name_unix in xmllist_form_file:
                            #print "skip %s" %xmlFileName
                            continue
                        matchWithProfilelist = True
                        if not xmlFileName in externXmlListOld:
                            tmpList.append(xmlFileName)
                    
                    #如果profilelist.txt中一个符合条件的都没有，就报错miss
                    if not matchWithProfilelist:
                        self.err.printErr_Object("Error", obj, "Bom = %s, but can't find xml %s in profile.txt" %(xmlPrefix, xmlMatch))
                    externXmlList += tmpList
        
        #去重复
        externXmlList = list(set(externXmlList))

        return externXmlList


    def read_profile(self, baseXmlList, profileDir, xmllist_form_file, checkedXmlList, profileTempMap, virtualConnectorCheckFlag = False):
        externXmlList = []
        xmlKeyList = []
        xmlKeySkipList = []
        self.profileMap['base'] = {} #底板

        # 先读取底板的XML，底板的XML列表由调用者指定。而扣卡的XML通过底板对象中的connertor获取。
        for objXml in baseXmlList:
            externXmlList += self.read_obj_xml(objXml, self.profileMap['base'], virtualConnectorCheckFlag, profileDir, xmllist_form_file, baseXmlList)

        #扩展卡可能还会继续连接其他扩展卡。。。，所以需要循环查找。直到找不到新的XML为止。
        while(1):
            # 按底板的Connector 决定要加载哪些扣卡的XML
            # 现在添加按Connector获取到的扣卡的xml
            externXmlListTmp = self.get_externboard_by_Connector(externXmlList, profileDir, xmllist_form_file)
            if len(externXmlListTmp) == 0: break

            for objXml in externXmlListTmp:
                basename = os.path.basename(objXml)
                xmlKey = basename #按xml名称进行分类
                if objXml not in checkedXmlList: #已经读取过的xml不再进行重复读取操作
                    self.virtualConnectorNeeded = False
                    self.profileMap[xmlKey] = {}
                    virtualXmlListTmp = self.read_obj_xml(objXml, self.profileMap[xmlKey], virtualConnectorCheckFlag, profileDir, xmllist_form_file, baseXmlList)
                    if self.virtualConnectorNeeded == False:
                        checkedXmlList.append(objXml)
                        xmlKeyList.append(xmlKey)
                    externXmlList += virtualXmlListTmp
                else:
                    xmlKeySkipList.append(xmlKey)

            externXmlList += externXmlListTmp
        
        #通过Anchor类对象 生成虚拟的I2C对象
        self.AnchorPreObj_to_local_object("Bus", "I2c")
        self.AnchorPreObj_to_local_object("Legacy", "Component")
        self.AnchorPreObj_to_local_object("Chip", "Chip")

        #将profileMap数据保存到profileTempMap
        for xmlKey in xmlKeyList:
            profileTempMap[xmlKey] = self.profileMap[xmlKey]
        #从profileTempMap中恢复profileMap数据
        for xmlKey in xmlKeySkipList:
            self.profileMap[xmlKey] = profileTempMap[xmlKey]
        return externXmlList

    #对Connertor对象定制合法性检查
    #1. 必须要有Bom属性， 
    #   在base.xml中的Connertor对象， Bom属性不能有VALUE值，其他xml中的Connertor对象的Bom属性必须有一个VALUE值。
    #2. 必须要有Position属性，且只有一个VALUE值
    #3. 各对象之间，Bom和Position结合生成的KEY不能重复
    def check_connector_object(self):
        positionMap = {}
        #逐个检查object
        for objName, obj in list(self.profileMap['base'].items()):
            if obj['CLASS'] != "Connector": continue
            if not "Position" in obj['PROPERTY']:
                self.err.printErr_Object("Error", obj, "Connector object %s miss property 'Position'." %obj['NAME'])
                continue

            if not "Bom" in obj['PROPERTY']:
                self.err.printErr_Object("Error", obj, "Connector object %s miss property 'Bom'." %obj['NAME'])
                continue

            if "VirtualConnector" in obj['PROPERTY']["Type"]['VALUE']: continue

            bom_val = obj['PROPERTY']["Bom"]['VALUE']
            if obj['FILE'] == "base.xml":
                bom_val_num_should_be = 0
            else:
                bom_val_num_should_be = 1
            
            if len(bom_val) != bom_val_num_should_be:
                self.err.printErr_Object("Error", obj, "Connector object %s property 'Bom' value number %d, should be %d." %(obj['NAME'], len(bom_val), bom_val_num_should_be))
                continue

            position_val = obj['PROPERTY']["Position"]['VALUE']
            if len(position_val) != 1:
                # 北冰洋屏蔽SasFanCpldConnect的postion为空不存在问题
                if (pdt_local == "PangeaV6_Arctic") and (obj['NAME'] == "SasFanCpldConnect"): continue
                self.err.printErr_Object("Error", obj, "Connector object %s property 'Position' value number %d, should be 1." %(obj['NAME'], len(position_val)))
                continue
            
            if 0 == bom_val_num_should_be: continue
                
            key = "%s-%d" %(bom_val[0], str2int(position_val[0]))

            #print "%s %s" %(xmlName, obj['NAME'])
            #print bom_val
            #print position_val
            #print "key: %s" %key
            if key in positionMap:
                #部分标卡和扣卡的Connnector使用相同的Bom和Position，但实际上不可能同时插上，对此不产生告警，只打印提示信息
                ExceptionObj = ["M2TrnsCardSlot1Connect"]
                if obj['NAME'] in ExceptionObj:
                    print("Info: file = %s class = Connector object = %s, Connector object Bom.Position is same with object %s in xml %s." %(obj['FILE'], obj['NAME'], positionMap[key]['NAME'], positionMap[key]['FILE']))
                    continue
                self.err.printErr_Object("Error", obj, "Connector object Bom.Position is same with object %s in xml %s." %(positionMap[key]['NAME'], positionMap[key]['FILE']))
                continue
            positionMap[key] = obj

    def read_profilelist(self, profilelist_file):
        lines = open_file_profile_check(profilelist_file).readlines()
        list_form_file = [e.strip("\r").strip("\n").strip() for e in lines]
        list_form_file.sort()
        return list_form_file
        
    #检查是否有冗余。缺少的XML在之前已经检查过了。
    def get_orphans_object(self, profileDir, externXmlList, list_form_file):
        list_from_object = [self.profilename2unixformat(profileDir, e) for e in externXmlList]
        list_from_object.sort()

        for xmlfile in list_form_file:
            if not xmlfile in list_from_object: 
                #self.orphans_list.append(e)
                if pangea_flag != 1:
                    self.mergeErr.printErr("Error", "profile.txt. Find redunant xml : %s. Please remove this xml files from 'profile.txt'" %xmlfile) #合并后再报错
                else:
                    print("Info: profile.txt. Find redunant xml : %s. Please remove this xml files from 'profile.txt'" %xmlfile)
                
    def get_isolated_object(self):
        # 检查关联必须在完成所有的object.xml检查之后。因为关联关系在检查object.xml的过程中才能生成
        for xmlName, objMap in list(self.profileMap.items()):
            #逐个检查object
            for objName, obj in list(objMap.items()):
                if obj['TYPE'] != "Physical": continue
                #Physical类型的对象必须有其他的类关联,即有其他对象属性包含RELATION_ACCESSOR并指向当前对象
                if not 'RELATION' in obj:
                    self.mergeErr.printErr_Object("Warning", obj, "Isolated 'Physical' object.") #合并后再报错
                    #self.isolated_list.append(obj)
                                    
    #检查告警事件配置是否重复，若有重复，会发生告警事件的覆盖
    #检查规则如下：同一实体且同一事件码的多个告警，没有配置AddInfo进行区分
    #若EventMonitor类型的多个对象中属性SubjectObj和InformationObj的值都相同，则这些对象均需配置属性AddInfo，否则报错并打印出未配置AddInfo的对象信息
    def check_warningevents_repeat(self):
        dict_repeatobj = {}
        for objName, obj in list(self.profileMap['base'].items()):
            if obj['CLASS'] != "EventMonitor": continue 
            # 1406G002_4b 硬盘背板slot0到slot3允许插nvme盘或者普通的硬盘，因此会同时配两个一样的eventmonitor，但是实际上只会产生一个事件，属于例外场景
            if obj['FILE']  == "1406G002_4b.xml":
                NoneedCheckList = ["HDD0InsertMntr","HDD0RemovedMntr","HDD1InsertMntr","HDD1RemovedMntr","HDD2InsertMntr","HDD2RemovedMntr","HDD3InsertMntr","HDD3RemovedMntr"]
                if obj['NAME'] in NoneedCheckList: continue
            addvalue = obj['PROPERTY']["AddInfo"]['VALUE']
            DesDyvalue = obj['PROPERTY']["DescriptionDynamic"]['VALUE']
            for value in obj['PROPERTY']["SubjectObj"]['VALUE']:
                Subvalue = value
            for value in obj['PROPERTY']["InformationObj"]['VALUE']:
                Inforvalue = value
            repeat_tup = (Subvalue, Inforvalue)
            if repeat_tup not in list(dict_repeatobj.keys()):
                dict_repeatobj[repeat_tup] = []
            dict_repeatobj[repeat_tup].append((objName, obj['FILE'], addvalue, DesDyvalue))
        
        for repeatinfo, AddInfo_val_tup in list(dict_repeatobj.items()):
            if len(AddInfo_val_tup) < 2 : continue
            for addinfo_tup in AddInfo_val_tup:
                if (len(addinfo_tup[2]) == 0) and (len(addinfo_tup[3]) != 0):
                    self.err.printErr("Error", "hasn't been configured 'AddInfo' in case of having repeated 'InformationObj' and 'SubjectObj' with other objects.", "EventMonitor", addinfo_tup[1], addinfo_tup[0])
    
        
    def check_profile(self, pdt, baseXmlList, profileDir, profilelist_file, checkedXmlList, profileTempMap, virtualConnectorCheckFlag = False):
        self.pdt = pdt
        xmllist_form_file = self.read_profilelist(profilelist_file)
        externXmlList = self.read_profile(baseXmlList, profileDir, xmllist_form_file, checkedXmlList, profileTempMap, virtualConnectorCheckFlag)
        #逐个检查object.xml
        for xmlName, objMap in list(self.profileMap.items()):
            #逐个检查object
            #print "### check: %s" %xmlName
            for objName, obj in list(objMap.items()):
                self.check_object(xmlName, obj)

        self.get_isolated_object()
        if pangea_flag != 1:
            self.get_orphans_object(profileDir, externXmlList, xmllist_form_file)
        # 检查Connector
        self.check_connector_object()
        # 检查告警事件配置是否有重复
        self.check_warningevents_repeat()
        
    def read_owner(self, filename):
        
        #print "读取CLASS责任人列表"
        for line in open_file_profile_check(filename).readlines():
            line = line.strip()
            tmp = line.split("	")
            self.ownerMap[tmp[0]] = tmp[1]
            #print ("%s    %s" %(tmp[0], tmp[1]))

    def __init__(self, classFile, ownerFile, class_whitelist, property_whitelist):
        self.profileMap = {}
        self.classMap = {}
        self.ownerMap = {}
        self.virtualConnectorNeeded = False
        
        #孤立的物理对象
        #self.isolated_list = []
        #profile.txt中多余的xml
        self.orphans_list  = []

        self.read_owner(ownerFile)
        self.err = ErrPrint(self.ownerMap)
        self.mergeErr = ErrPrint(self.ownerMap)
        self.read_class_xml(classFile)
        self.class_whitelist = class_whitelist
        self.property_whitelist = property_whitelist

                
#全局变量
g_lock = Lock()
g_result = Value('i', 0)                

        
class CprofileCheck(CCmdBase):
    V6List = ["SMM","CM221","CM221S","CN221","RM210","RM210_SECURITY_3V0","RM211","TaiShan5290v2","TaiShan2480v2","TaiShan2480v2Pro","Atlas880","TaiShan2280Ev2","1288hv6","2288hv6","1288hv6_2288hv6_5288v6/2288hv6_5288v6","1288hv6_2288hv6_5288v6/1288hv6","PangeaV6","Atlas500_30X0","SwitchSMM","MM_atlantic","S920XA0","PangeaV6_Atlantic","PangeaV6_Atlantic_Smm","PangeaV6_Pacific","PangeaV6_Arctic","Atlas500_3000","mockup140","TaiShan2280v2_1711","BM320","BM320_V3","S920XB0","PangeaV6_Atlantic_Swmm","TaiShan2280Mv2","S920X05","S920S03","2488hv6","1288hv6_2288hv6_5288v6","Atlas800_9010","xh321v6", "DP1210_DP2210_DP4210/DP2210_DP4210", "DP1210_DP2210_DP4210/DP1210","PangeaV6_Arctic","PangeaV6_Atlantic","PangeaV6_Atlantic_Smm","PangeaV6_Pacific","PAC1220V6","DA122Cv2","2288xv5_1711", "S902X20","Atlas800D_G1","Atlas800D_RM", "SMM0001V6", "CST0220V6", "CST0221V6", "CST1020V6", "CST0210V6", "CST1220V6"]
    PangeaV6List = ["PangeaV6_Arctic","PangeaV6_Atlantic","PangeaV6_Atlantic_Smm","PangeaV6_Pacific", "Atlas200I_SoC_A1", "PAC1220V6", "SMM0001V6", "CST0220V6", "CST0221V6", "CST1020V6", "CST0210V6", "CST1220V6", "ENC0210V6"]
    supportVirtualConnectCheckPdt = ["rh5885hv3","9032","rh8100v5","9008v5base","9008v5advance","MM510","g530v5","g560","g560v5","2488v5","2488hv5_5885hv5","1288hv5","2288hv5","2288v5","ch121v5","ch242v5","ch225v5","TaiShan2280v2","TaiShan2280Mv2","xa320","TaiShan2280Ev2","RM210","RM210_SECURITY_3V0","RM211","Atlas880", "S920X05","TaiShan2280v2_1711","BM320","xr320","TaiShan2480v2","TaiShan2480v2Pro","SMM","CM221","CN221","CM221S","2488hv6","xh321v5","xh628v5","MM650","2298v5","S920S03","2288xv5_5288xv5","1288xv5","Atlas800_9010","Atlas500_3000","2288HV6-16DIMM","xh321v6","1288hv6_2288hv6_5288v6/2288hv6_5288v6","1288hv6_2288hv6_5288v6/1288hv6", "DP1210_DP2210_DP4210/DP2210_DP4210", "DP1210_DP2210_DP4210/DP1210","PangeaV6_Arctic","PangeaV6_Atlantic","PangeaV6_Pacific","DA122Cv2", "Atlas200I_SoC_A1","2288xv5_1711", "S902X20","Atlas800D_G1","Atlas800D_RM", "SMM0001V6", "CST0220V6", "CST0221V6", "CST1020V6", "CST0210V6", "CST1220V6", "Atlas900_PoD_A2"]
    pdt2mxl = {
        'rh8100v3':["_85b.xml,8100_00.xml", "_85b.xml,8100_01.xml", "_85b.xml,8100_02.xml", "_85b.xml,8100_03.xml", "_85b.xml,8100_04.xml", "_85b.xml,8100_05.xml", "_85b.xml,8100_06.xml", "_85b.xml,8100_07.xml", "_8b6.xml,8100_00.xml", "_8b6.xml,8100_01.xml", "_8b6.xml,8100_02.xml", "_8b6.xml,8100_03.xml", "_8b6.xml,8100_04.xml", "_8b6.xml,8100_05.xml", "_8b6.xml,8100_06.xml", "_8b6.xml,8100_07.xml"],
        '9032':["_8b3.xml,9032_SCE_00.xml,9032_BASE_00.xml,9032_logic_02.xml,9032_fdm_00.xml","_8b3.xml,9032_SCE_01.xml,9032_BASE_00.xml,9032_logic_00.xml,9032_fdm_01.xml",           "_8b3.xml,9032_SCE_02.xml,9032_BASE_00.xml,9032_logic_01.xml,9032_fdm_10.xml", "_8b3.xml,9032_SCE_03.xml,9032_BASE_00.xml,9032_physical_00.xml,9032_fdm_11.xml", "_8b3.xml,9032_SCE_04.xml,9032_BASE_00.xml,9032_physical_01.xml,9032_fdm_21.xml","_8b3.xml,9032_SCE_04.xml,9032_BASE_00.xml,9032_logic_10.xml,9032_fdm_31.xml", "_8b3.xml,9032_SCE_04.xml,9032_BASE_00.xml,9032_logic_11.xml,9032_fdm_51.xml","_8b3.xml,9032_SCE_04.xml,9032_BASE_00.xml,9032_logic_11.xml,9032_fdm_51.xml"],
        'ch220v3':["_453.xml,14140130_19e50007_ffffffff.xml,12VSTART_01.xml","_453.xml,14140130_19e50009_ffffffff.xml,12VSTART_02.xml","_453.xml,14140130_10de0ff2_ffffffff.xml"],
        'ch121v3':["_440.xml,SGCA_00.xml,14140228_f0.xml,14140228_f1.xml,12VSTART_01.xml,12VSTART_02.xml,14140130_19e50009_ffffffff.xml", "_440.xml,SGCA_02.xml","_440.xml,SGCA_01.xml"],
        'ch121v5':[
		"_43c.xml,14140228_f0.xml,14140228_f2.xml,12VSTART_01.xml,PCHBASE10G_ff.xml,DS125CFG_da.xml,CPUPKG_01.xml,VRD_MGNT_00.xml,SPCA_00.xml,COOLING_POLICY_00.xml,AEP_01.xml,AEP_02.xml,MEZZCFG_02.xml,14220292_05.xml,MEZZCFG_01.xml,VRD_COOLING_POLICY_00.xml",
        "_43c.xml,14140228_f0.xml,14140228_f2.xml,12VSTART_01.xml,PCHBASE10G_ff.xml,DS125CFG_da.xml,CPUPKG_01.xml,VRD_MGNT_01.xml,SPCA_01.xml,COOLING_POLICY_01.xml,AEP_01.xml,AEP_02.xml,MEZZCFG_02.xml,14220292_05.xml,MEZZCFG_01.xml,VRD_COOLING_POLICY_01.xml"
		],
        'ch242v5':["_459.xml,14140228_f0.xml,14140228_f2.xml,14140228_f4.xml,14140228_f6.xml,12VSTART_01.xml,PCHBASE10G_ff.xml,DS125CFG_da.xml,CPUPKG_01.xml,VRD_MGNT_00.xml,AEP_01.xml,AEP_02.xml,AEP_03.xml,AEP_04.xml,MEZZCFG_01.xml,MEZZCFG_02.xml,MEZZCFG_03.xml,MEZZCFG_04.xml","_459.xml,14140228_f0.xml,14140228_f2.xml,14140228_f4.xml,14140228_f6.xml,12VSTART_01.xml,PCHBASE10G_ff.xml,DS125CFG_da.xml,CPUPKG_01.xml,VRD_MGNT_01.xml,AEP_01.xml,AEP_02.xml,AEP_03.xml,AEP_04.xml,MEZZCFG_01.xml,MEZZCFG_02.xml,MEZZCFG_03.xml,MEZZCFG_04.xml"],
        'ch225v5':["_45a.xml,14140228_f0.xml,14140228_f2.xml,12VSTART_01.xml,PCHBASE10G_ff.xml,DS125CFG_dc.xml,VRD_MGNT_00.xml,MEZZCFG_01.xml,VRD_COOLING_POLICY_00.xml,COOLING_POLICY_01.xml,AEP_01.xml,AEP_02.xml,MEZZCFG_02.xml,MEZZCFG_03.xml,MEZZCFG_04.xml"],
        'rh2288hv3':["_10d.xml,HGSA_I2C_00.xml,HGSA_CPLD_00.xml,HGSA_VRD_05.xml,HGSA_00.xml,14140130_10de15f8_ffffffff.xml,14140130_10de15f7_ffffffff.xml,14140130_80860435_ffffffff.xml,14140130_15b31013_15b30014.xml,14140130_15b31013_15b30013.xml,CustomizeSensor.xml,14140130_177d0012_ffffffff.xml","_10d.xml,HGSA_I2C_01.xml,HGSA_CPLD_00.xml,HGSA_VRD_06.xml,HGSA_01.xml,14140130_10de15f8_ffffffff.xml,14140130_10de15f7_ffffffff.xml,14140130_80860435_ffffffff.xml,14140130_15b31013_15b30014.xml,14140130_15b31013_15b30013.xml,CustomizeSensor.xml,14140130_177d0012_ffffffff.xml"],
        'rh2288hv3_DHL':["_10d.xml,HGSA_I2C_00.xml,HGSA_CPLD_00.xml,HGSA_VRD_05.xml,HGSA_00.xml,14140130_10de15f8_ffffffff.xml,14140130_10de15f7_ffffffff.xml,14140130_80860435_ffffffff.xml,14140130_15b31013_15b30014.xml,14140130_15b31013_15b30013.xml,CustomizeSensor.xml,14140130_177d0012_ffffffff.xml","_10d.xml,HGSA_I2C_01.xml,HGSA_CPLD_00.xml,HGSA_VRD_06.xml,HGSA_01.xml,14140130_10de15f8_ffffffff.xml,14140130_10de15f7_ffffffff.xml,14140130_80860435_ffffffff.xml,14140130_15b31013_15b30014.xml,14140130_15b31013_15b30013.xml,CustomizeSensor.xml,14140130_177d0012_ffffffff.xml"],
        'rh5288v3':["_102.xml,HGSE_I2C_00.xml,1406G002_c3.xml,14140130_15b31013_15b30038.xml,14140130_15b31013_15b30007.xml,14140130_15b31013_15b30041.xml,14140130_80860435_ffffffff.xml,14140130_15b31013_15b30014.xml,14140130_15b31013_15b30073.xml,14140130_15b31013_15b30013.xml,14140130_177d0012_ffffffff.xml,14140130_15b31013_15b30008.xml,14140130_19e50009_ffffffff.xml","_102.xml,HGSE_I2C_00.xml,1406G002_77.xml,14140130_15b31013_15b30038.xml,14140130_15b31013_15b30007.xml,14140130_15b31013_15b30041.xml,14140130_80860435_ffffffff.xml,14140130_15b31013_15b30014.xml,14140130_15b31013_15b30073.xml,14140130_15b31013_15b30013.xml,14140130_177d0012_ffffffff.xml,14140130_15b31013_15b30008.xml,14140130_19e50009_ffffffff.xml",        "_102.xml,HGSE_I2C_00.xml,1406G002_76.xml,14140130_15b31013_15b30038.xml,14140130_15b31013_15b30007.xml,14140130_15b31013_15b30041.xml,14140130_80860435_ffffffff.xml,14140130_15b31013_15b30014.xml,14140130_15b31013_15b30073.xml,14140130_15b31013_15b30013.xml,14140130_177d0012_ffffffff.xml,14140130_15b31013_15b30008.xml,14140130_19e50009_ffffffff.xml","_102.xml,HGSE_I2C_01.xml,1406G002_ca.xml,14140130_15b31013_15b30038.xml,14140130_15b31013_15b30007.xml,14140130_15b31013_15b30041.xml,14140130_80860435_ffffffff.xml,14140130_15b31013_15b30014.xml,14140130_15b31013_15b30073.xml,14140130_15b31013_15b30013.xml,14140130_177d0012_ffffffff.xml,14140130_15b31013_15b30008.xml,14140130_19e50009_ffffffff.xml"],
        'G2500v1':["_291.xml"],
        'rh5885v3':["_1b5.xml,BLCB_04.xml,12VSTART_01.xml,HGSA_PCB_03.xml,HGSA_VRD_02.xml","_1b5.xml,BLCB_04.xml,12VSTART_01.xml,HGSA_PCB_03.xml,HGSA_VRD_03.xml"],
        'rh5885hv3':["_15e.xml,BFSC_1a.xml,Power12_00.xml", "_15e.xml,BFSC_1a.xml,Power12_01.xml,Power34_01.xml"],
        'rh1288v3':["_10f.xml,14140130_80860435_ffffffff.xml,14140130_15b31013_15b30014.xml,14140130_15b31013_15b30013.xml,14140130_177d0012_ffffffff.xml"],
        'rh1288v3_DHL':["_10f.xml,14140130_80860435_ffffffff.xml,14140130_15b31013_15b30014.xml,14140130_15b31013_15b30013.xml,14140130_177d0012_ffffffff.xml"],
        '2288hv5':[
            ("_115.xml,PRODUCT_15_2f.xml,SPSCA_VRD_00.xml,CPUPKG_01.xml,CPUPKG_02.xml,SPSCA_NCSI_00.xml,SPSCA_4U_00.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml,AEP_01.xml,AEP_02.xml,SPSCA_COOLING_00.xml,SPSCA_PCIeAddrInfo_48.xml,SPSCA_LOM_GE_PORT_00.xml"),
            ("_115.xml,PRODUCT_15_2f.xml,SPSCA_VRD_00.xml,CPUPKG_01.xml,CPUPKG_02.xml,SPSCA_NCSI_00.xml,SPSCA_4U_00.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml,AEP_01.xml,AEP_02.xml,SPSCA_COOLING_01.xml,SPSCA_PCIeAddrInfo_48.xml,SPSCA_LOM_GE_PORT_00.xml"),
            ("_115.xml,PRODUCT_15_2f.xml,SPSCA_VRD_00.xml,CPUPKG_01.xml,CPUPKG_02.xml,SPSCA_NCSI_00.xml,SPSCA_4U_00.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml,AEP_01.xml,AEP_02.xml,SPSCA_LIQUKD_COOLING_00.xml,SPSCA_PCIeAddrInfo_48.xml,SPSCA_LOM_GE_PORT_00.xml"),
            ("_115.xml,PRODUCT_15_2f.xml,SPSCA_VRD_00.xml,CPUPKG_01.xml,CPUPKG_02.xml,SPSCA_NCSI_00.xml,SPSCA_4U_00.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml,AEP_01.xml,AEP_02.xml,SPSCA_COOLING_00.xml,SPSCA_PCIeAddrInfo_ff.xml,SPSCA_LOM_GE_PORT_00.xml"),
            ("_115.xml,PRODUCT_15_2f.xml,SPSCA_VRD_00.xml,CPUPKG_01.xml,CPUPKG_02.xml,SPSCA_NCSI_00.xml,SPSCA_4U_00.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml,AEP_01.xml,AEP_02.xml,SPSCA_COOLING_01.xml,SPSCA_PCIeAddrInfo_ff.xml,SPSCA_LOM_GE_PORT_00.xml"),
            ("_115.xml,PRODUCT_15_2f.xml,SPSCA_VRD_00.xml,CPUPKG_01.xml,CPUPKG_02.xml,SPSCA_NCSI_00.xml,SPSCA_4U_00.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml,AEP_01.xml,AEP_02.xml,SPSCA_LIQUKD_COOLING_00.xml,SPSCA_PCIeAddrInfo_ff.xml,SPSCA_LOM_GE_PORT_00.xml"),
            ("_115.xml,PRODUCT_15_3f.xml,SPSCA_VRD_01.xml,CPUPKG_01.xml,CPUPKG_02.xml,SPSCA_NCSI_00.xml,SPSCA_4U_00.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml,AEP_01.xml,AEP_02.xml,SPSCA_COOLING_00.xml,SPSCA_LOM_GE_PORT_00.xml"),
            ("_115.xml,PRODUCT_15_3f.xml,SPSCA_VRD_01.xml,CPUPKG_01.xml,CPUPKG_02.xml,SPSCA_NCSI_00.xml,SPSCA_4U_00.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml,AEP_01.xml,AEP_02.xml,SPSCA_COOLING_01.xml,SPSCA_LOM_GE_PORT_00.xml"),
            ("_115.xml,PRODUCT_15_3f.xml,SPSCA_VRD_01.xml,CPUPKG_01.xml,CPUPKG_02.xml,SPSCA_NCSI_00.xml,SPSCA_4U_00.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml,AEP_01.xml,AEP_02.xml,SPSCA_LIQUKD_COOLING_00.xml,SPSCA_LOM_GE_PORT_00.xml"),
            ("_115.xml,PRODUCT_15_ff.xml,SPSCA_VRD_01.xml,CPUPKG_01.xml,CPUPKG_02.xml,SPSCA_NCSI_00.xml,SPSCA_4U_00.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml,AEP_01.xml,AEP_02.xml,SPSCA_COOLING_00.xml,SPSCA_LOM_GE_PORT_00.xml"),
            ("_115.xml,PRODUCT_15_ff.xml,SPSCA_VRD_01.xml,CPUPKG_01.xml,CPUPKG_02.xml,SPSCA_NCSI_00.xml,SPSCA_4U_00.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml,AEP_01.xml,AEP_02.xml,SPSCA_COOLING_01.xml,SPSCA_LOM_GE_PORT_00.xml"),
            ("_115.xml,PRODUCT_15_ff.xml,SPSCA_VRD_01.xml,CPUPKG_01.xml,CPUPKG_02.xml,SPSCA_NCSI_00.xml,SPSCA_4U_00.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml,AEP_01.xml,AEP_02.xml,SPSCA_LIQUKD_COOLING_00.xml,SPSCA_LOM_GE_PORT_00.xml"),
            ("_117.xml,PRODUCT_17_2e.xml,SPSCA_VRD_00.xml,CPUPKG_01.xml,CPUPKG_02.xml,SPSCA_NCSI_00.xml,SPSCA_4U_00.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml,AEP_01.xml,AEP_02.xml,SPSCA_COOLING_00.xml,SPSCA_PCIeAddrInfo_48.xml,SPSCA_LOM_GE_PORT_00.xml"),
            ("_117.xml,PRODUCT_17_2e.xml,SPSCA_VRD_00.xml,CPUPKG_01.xml,CPUPKG_02.xml,SPSCA_NCSI_00.xml,SPSCA_4U_00.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml,AEP_01.xml,AEP_02.xml,SPSCA_COOLING_01.xml,SPSCA_PCIeAddrInfo_48.xml,SPSCA_LOM_GE_PORT_00.xml"),
            ("_117.xml,PRODUCT_17_2e.xml,SPSCA_VRD_00.xml,CPUPKG_01.xml,CPUPKG_02.xml,SPSCA_NCSI_00.xml,SPSCA_4U_00.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml,AEP_01.xml,AEP_02.xml,SPSCA_LIQUKD_COOLING_00.xml,SPSCA_PCIeAddrInfo_48.xml,SPSCA_LOM_GE_PORT_00.xml"),
            ("_117.xml,PRODUCT_17_2e.xml,SPSCA_VRD_00.xml,CPUPKG_01.xml,CPUPKG_02.xml,SPSCA_NCSI_00.xml,SPSCA_4U_00.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml,AEP_01.xml,AEP_02.xml,SPSCA_COOLING_00.xml,SPSCA_PCIeAddrInfo_ff.xml,SPSCA_LOM_GE_PORT_00.xml"),
            ("_117.xml,PRODUCT_17_2e.xml,SPSCA_VRD_00.xml,CPUPKG_01.xml,CPUPKG_02.xml,SPSCA_NCSI_00.xml,SPSCA_4U_00.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml,AEP_01.xml,AEP_02.xml,SPSCA_COOLING_01.xml,SPSCA_PCIeAddrInfo_ff.xml,SPSCA_LOM_GE_PORT_00.xml"),
            ("_117.xml,PRODUCT_17_2e.xml,SPSCA_VRD_00.xml,CPUPKG_01.xml,CPUPKG_02.xml,SPSCA_NCSI_00.xml,SPSCA_4U_00.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml,AEP_01.xml,AEP_02.xml,SPSCA_LIQUKD_COOLING_00.xml,SPSCA_PCIeAddrInfo_ff.xml,SPSCA_LOM_GE_PORT_00.xml"),
            ("_117.xml,PRODUCT_17_ff.xml,SPSCA_VRD_01.xml,CPUPKG_01.xml,CPUPKG_02.xml,SPSCA_NCSI_00.xml,SPSCA_4U_00.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml,AEP_01.xml,AEP_02.xml,SPSCA_COOLING_00.xml,SPSCA_LOM_GE_PORT_00.xml"), 
            ("_117.xml,PRODUCT_17_ff.xml,SPSCA_VRD_01.xml,CPUPKG_01.xml,CPUPKG_02.xml,SPSCA_NCSI_00.xml,SPSCA_4U_00.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml,AEP_01.xml,AEP_02.xml,SPSCA_COOLING_01.xml,SPSCA_LOM_GE_PORT_00.xml"), 
            ("_117.xml,PRODUCT_17_ff.xml,SPSCA_VRD_01.xml,CPUPKG_01.xml,CPUPKG_02.xml,SPSCA_NCSI_00.xml,SPSCA_4U_00.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml,AEP_01.xml,AEP_02.xml,SPSCA_LIQUKD_COOLING_00.xml,SPSCA_LOM_GE_PORT_00.xml"),
            ("_117.xml,PRODUCT_17_3e.xml,SPSCA_VRD_01.xml,CPUPKG_01.xml,CPUPKG_02.xml,SPSCA_NCSI_00.xml,SPSCA_4U_00.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml,AEP_01.xml,AEP_02.xml,SPSCA_COOLING_00.xml,SPSCA_LOM_GE_PORT_00.xml"),
            ("_117.xml,PRODUCT_17_3e.xml,SPSCA_VRD_01.xml,CPUPKG_01.xml,CPUPKG_02.xml,SPSCA_NCSI_00.xml,SPSCA_4U_00.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml,AEP_01.xml,AEP_02.xml,SPSCA_COOLING_01.xml,SPSCA_LOM_GE_PORT_00.xml"),
            ("_117.xml,PRODUCT_17_3e.xml,SPSCA_VRD_01.xml,CPUPKG_01.xml,CPUPKG_02.xml,SPSCA_NCSI_00.xml,SPSCA_4U_00.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml,AEP_01.xml,AEP_02.xml,SPSCA_LIQUKD_COOLING_00.xml,SPSCA_LOM_GE_PORT_00.xml"),
            ("_1ba.xml,PRODUCT_ba_2d.xml,SPSCD_VRD_00.xml,CPUPKG_01.xml,CPUPKG_02.xml,SPSCA_NCSI_00.xml,SPSCA_4U_00.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml,AEP_01.xml,AEP_02.xml,SPSCA_COOLING_00.xml,SPSCA_PCIeAddrInfo_48.xml"),
            ("_1ba.xml,PRODUCT_ba_2d.xml,SPSCD_VRD_00.xml,CPUPKG_01.xml,CPUPKG_02.xml,SPSCA_NCSI_00.xml,SPSCA_4U_00.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml,AEP_01.xml,AEP_02.xml,SPSCA_COOLING_01.xml,SPSCA_PCIeAddrInfo_48.xml"),
            ("_1ba.xml,PRODUCT_ba_2d.xml,SPSCD_VRD_00.xml,CPUPKG_01.xml,CPUPKG_02.xml,SPSCA_NCSI_00.xml,SPSCA_4U_00.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml,AEP_01.xml,AEP_02.xml,SPSCA_LIQUKD_COOLING_00.xml,SPSCA_PCIeAddrInfo_48.xml"),
            ("_1ba.xml,PRODUCT_ba_2d.xml,SPSCD_VRD_00.xml,CPUPKG_01.xml,CPUPKG_02.xml,SPSCA_NCSI_00.xml,SPSCA_4U_00.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml,AEP_01.xml,AEP_02.xml,SPSCA_COOLING_00.xml,SPSCA_PCIeAddrInfo_ff.xml"),
            ("_1ba.xml,PRODUCT_ba_2d.xml,SPSCD_VRD_00.xml,CPUPKG_01.xml,CPUPKG_02.xml,SPSCA_NCSI_00.xml,SPSCA_4U_00.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml,AEP_01.xml,AEP_02.xml,SPSCA_COOLING_01.xml,SPSCA_PCIeAddrInfo_ff.xml"),       
            ("_1ba.xml,PRODUCT_ba_2d.xml,SPSCD_VRD_00.xml,CPUPKG_01.xml,CPUPKG_02.xml,SPSCA_NCSI_00.xml,SPSCA_4U_00.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml,AEP_01.xml,AEP_02.xml,SPSCA_LIQUKD_COOLING_00.xml,SPSCA_PCIeAddrInfo_ff.xml")
        ],
        '2288v5':[
            ("_1b1.xml,CPUPKG_01.xml,CPUPKG_02.xml,SPSCA_4U_00.xml,CustomizeSensor.xml,SPSCA_COOLING_00.xml,SPSEA_VRD_00.xml,SPSCA_PCIeAddrInfo_48.xml"),
            ("_1b1.xml,CPUPKG_01.xml,CPUPKG_02.xml,SPSCA_4U_00.xml,CustomizeSensor.xml,SPSCA_COOLING_01.xml,SPSEA_VRD_01.xml,SPSCA_PCIeAddrInfo_ff.xml"),
            ("_1b3.xml,CPUPKG_01.xml,CPUPKG_02.xml,SPSCA_4U_00.xml,CustomizeSensor.xml,SPSCA_COOLING_00.xml,PRODUCT_b3_2e.xml,SPSEA_VRD_00.xml,SPSCA_PCIeAddrInfo_48.xml"),
            ("_1b3.xml,CPUPKG_01.xml,CPUPKG_02.xml,SPSCA_4U_00.xml,CustomizeSensor.xml,SPSCA_COOLING_01.xml,PRODUCT_b3_2e.xml,SPSEA_VRD_01.xml,SPSCA_PCIeAddrInfo_ff.xml"),
            ("_1b3.xml,CPUPKG_01.xml,CPUPKG_02.xml,SPSCA_4U_00.xml,CustomizeSensor.xml,SPSCA_COOLING_00.xml,PRODUCT_b3_3e.xml,SPSCA_JDM_VRD_00.xml"),
            ("_1b3.xml,CPUPKG_01.xml,CPUPKG_02.xml,SPSCA_4U_00.xml,CustomizeSensor.xml,SPSCA_COOLING_01.xml,PRODUCT_b3_3e.xml,SPSCA_JDM_VRD_01.xml"),
            ("_1b3.xml,CPUPKG_01.xml,CPUPKG_02.xml,SPSCA_4U_00.xml,CustomizeSensor.xml,SPSCA_COOLING_00.xml,PRODUCT_b3_ff.xml,SPSCA_JDM_VRD_01.xml"),
            ("_1b3.xml,CPUPKG_01.xml,CPUPKG_02.xml,SPSCA_4U_00.xml,CustomizeSensor.xml,SPSCA_COOLING_01.xml,PRODUCT_b3_ff.xml,SPSCA_JDM_VRD_01.xml"),
            ("_1b4.xml,CPUPKG_01.xml,CPUPKG_02.xml,SPSCA_4U_00.xml,CustomizeSensor.xml,SPSCA_COOLING_00.xml,PRODUCT_b4_2f.xml,SPSEA_VRD_00.xml,SPSCA_PCIeAddrInfo_48.xml"),
            ("_1b4.xml,CPUPKG_01.xml,CPUPKG_02.xml,SPSCA_4U_00.xml,CustomizeSensor.xml,SPSCA_COOLING_01.xml,PRODUCT_b4_2f.xml,SPSEA_VRD_00.xml,SPSCA_PCIeAddrInfo_ff.xml"),
            ("_1b4.xml,CPUPKG_01.xml,CPUPKG_02.xml,SPSCA_4U_00.xml,CustomizeSensor.xml,SPSCA_COOLING_00.xml,PRODUCT_b4_3f.xml,SPSCA_JDM_VRD_00.xml"),
            ("_1b4.xml,CPUPKG_01.xml,CPUPKG_02.xml,SPSCA_4U_00.xml,CustomizeSensor.xml,SPSCA_COOLING_01.xml,PRODUCT_b4_3f.xml,SPSCA_JDM_VRD_01.xml"),
            ("_1b4.xml,CPUPKG_01.xml,CPUPKG_02.xml,SPSCA_4U_00.xml,CustomizeSensor.xml,SPSCA_COOLING_00.xml,PRODUCT_b4_ff.xml,SPSCA_JDM_VRD_00.xml"),
            ("_1b4.xml,CPUPKG_01.xml,CPUPKG_02.xml,SPSCA_4U_00.xml,CustomizeSensor.xml,SPSCA_COOLING_01.xml,PRODUCT_b4_ff.xml,SPSCA_JDM_VRD_01.xml")
        ],
        '1288hv5':[
            ("_118.xml,CPUPKG_01.xml,CPUPKG_02.xml,AEP_01.xml,AEP_02.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml,SPSCA_NCSI_00.xml,SPSCC_VRD_00.xml,SPSCC_PCIeAddrInfo_43.xml,SPSCA_LOM_GE_PORT_00.xml"),
            ("_118.xml,CPUPKG_01.xml,CPUPKG_02.xml,AEP_01.xml,AEP_02.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml,SPSCA_NCSI_00.xml,SPSCC_VRD_01.xml,SPSCC_PCIeAddrInfo_ff.xml,SPSCA_LOM_GE_PORT_00.xml"),
            ("_119.xml,CPUPKG_01.xml,CPUPKG_02.xml,AEP_01.xml,AEP_02.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml,SPSCA_NCSI_00.xml,SPSCC_VRD_00.xml,SPSCC_PCIeAddrInfo_43.xml,SPSCA_LOM_GE_PORT_00.xml"),
            ("_119.xml,CPUPKG_01.xml,CPUPKG_02.xml,AEP_01.xml,AEP_02.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml,SPSCA_NCSI_00.xml,SPSCC_VRD_01.xml,SPSCC_PCIeAddrInfo_ff.xml,SPSCA_LOM_GE_PORT_00.xml"),
            ("_1b6.xml,CPUPKG_01.xml,CPUPKG_02.xml,AEP_01.xml,AEP_02.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml,SPSCA_NCSI_00.xml,SPSCC_JDM_VRD_00.xml,SPSCA_LOM_GE_PORT_00.xml"),
		    ("_1b6.xml,CPUPKG_01.xml,CPUPKG_02.xml,AEP_01.xml,AEP_02.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml,SPSCA_NCSI_00.xml,SPSCA_LOM_GE_PORT_00.xml,SPSCC_JDM_VRD_01.xml"),
		    ("_1b7.xml,CPUPKG_01.xml,CPUPKG_02.xml,AEP_01.xml,AEP_02.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml,SPSCA_NCSI_00.xml,SPSCC_JDM_VRD_00.xml"),
		    ("_1b7.xml,CPUPKG_01.xml,CPUPKG_02.xml,AEP_01.xml,AEP_02.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml,SPSCA_NCSI_00.xml,SPSCC_JDM_VRD_01.xml"),
            ("_1bb.xml,CPUPKG_01.xml,CPUPKG_02.xml,AEP_01.xml,AEP_02.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml,SPSCA_NCSI_00.xml,SPSCC_PCIeAddrInfo_43.xml,SPSCE_VRD_00.xml"),
            ("_1bb.xml,CPUPKG_01.xml,CPUPKG_02.xml,AEP_01.xml,AEP_02.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml,SPSCA_NCSI_00.xml,SPSCC_PCIeAddrInfo_ff.xml,SPSCE_VRD_00.xml")
        ],
        '2488v5':["_802.xml,MBLA_VRD_00.xml,CustomizeSensor.xml,M2_SATA_SSD_00.xml","_802.xml,MBLA_VRD_01.xml,CustomizeSensor.xml,M2_SATA_SSD_00.xml"],
        '2488hv5_5885hv5':[
		"_803.xml,2488H_VRD_CPU1_2_00.xml,2488H_VRD_CPU3_4_00.xml,ServerType_00.xml,AEP_CPU1_01.xml,M2_SATA_SSD_00.xml,CustomizeSensor.xml",
		"_803.xml,2488H_VRD_CPU1_2_01.xml,2488H_VRD_CPU3_4_01.xml,ServerType_00.xml,AEP_CPU3_03.xml,M2_SATA_SSD_00.xml,CustomizeSensor.xml",
		"_803.xml,2488H_VRD_CPU1_2_00.xml,2488H_VRD_CPU3_4_00.xml,ServerType_01.xml,AEP_CPU2_02.xml,BC62MB01_PCIeAddrInfo_78.xml,M2_SATA_SSD_00.xml,CustomizeSensor.xml",
		"_803.xml,2488H_VRD_CPU1_2_01.xml,2488H_VRD_CPU3_4_01.xml,ServerType_01.xml,AEP_CPU4_04.xml,BC62MB01_PCIeAddrInfo_78.xml,M2_SATA_SSD_00.xml,CustomizeSensor.xml",
		"_803.xml,2488H_VRD_CPU1_2_00.xml,2488H_VRD_CPU3_4_00.xml,ServerType_02.xml,M2_SATA_SSD_00.xml,CustomizeSensor.xml",
		"_803.xml,2488H_VRD_CPU1_2_01.xml,2488H_VRD_CPU3_4_01.xml,ServerType_02.xml,M2_SATA_SSD_00.xml,CustomizeSensor.xml"
		],
        'rh1288v3_FranceSystemU':["_10f.xml,14140130_80860435_ffffffff.xml,14140130_15b31013_15b30014.xml,14140130_15b31013_15b30013.xml,14140130_177d0012_ffffffff.xml"],
        'rh2288v3':["_10e.xml,1010101V_80.xml,HGSB_I2C_00.xml,HGSB_PRODUCT_00.xml,HGSB_0d.xml,HGSB_04.xml,HGSB_0b.xml,HGSB_03.xml,HGSB_05.xml,HGSB_0c.xml,HGSB_00.xml,14140130_80860435_ffffffff.xml,14140130_15b31013_15b30014.xml,HGSB_07.xml,14140130_15b31013_15b30013.xml,HGSB_0a.xml,HGSB_08.xml,CustomizeSensor.xml,14140130_177d0012_ffffffff.xml,HGSB_02.xml,HGSB_06.xml,HGSB_01.xml,HGSB_09.xml","_10e.xml,1010101V_80.xml,HGSB_I2C_01.xml,HGSB_PRODUCT_00.xml,HGSB_0d.xml,HGSB_04.xml,HGSB_0b.xml,HGSB_03.xml,HGSB_05.xml,HGSB_0c.xml,HGSB_00.xml,14140130_80860435_ffffffff.xml,14140130_15b31013_15b30014.xml,HGSB_07.xml,14140130_15b31013_15b30013.xml,HGSB_0a.xml,HGSB_08.xml,CustomizeSensor.xml,14140130_177d0012_ffffffff.xml,HGSB_02.xml,HGSB_06.xml,HGSB_01.xml,HGSB_09.xml","_101.xml,1010101V_80.xml,HGSB_I2C_00.xml,HGSB_PRODUCT_00.xml,HGSB_0d.xml,HGSB_04.xml,HGSB_0b.xml,HGSB_03.xml,HGSB_05.xml,HGSB_0c.xml,HGSB_00.xml,14140130_80860435_ffffffff.xml,14140130_15b31013_15b30014.xml,HGSB_07.xml,14140130_15b31013_15b30013.xml,HGSB_0a.xml,HGSB_08.xml,CustomizeSensor.xml,14140130_177d0012_ffffffff.xml,HGSB_02.xml,HGSB_06.xml,HGSB_01.xml,HGSB_09.xml","_101.xml,1010101V_80.xml,HGSB_I2C_01.xml,HGSB_PRODUCT_00.xml,HGSB_0d.xml,HGSB_04.xml,HGSB_0b.xml,HGSB_03.xml,HGSB_05.xml,HGSB_0c.xml,HGSB_00.xml,14140130_80860435_ffffffff.xml,14140130_15b31013_15b30014.xml,HGSB_07.xml,14140130_15b31013_15b30013.xml,HGSB_0a.xml,HGSB_08.xml,CustomizeSensor.xml,14140130_177d0012_ffffffff.xml,HGSB_02.xml,HGSB_06.xml,HGSB_01.xml,HGSB_09.xml"],
        'cx712':["_418.xml","_409.xml"], 
        'cx710':["_417.xml,12VSTART_01.xml","_417.xml,12VSTART_02.xml"], 
        'x6800':["_30d.xml,HGSA_03.xml,SPSCA_COOLING_00.xml,14140130_19e50123_ffffffff.xml,14140130_10de15f8_ffffffff.xml,14140130_10de15f7_ffffffff.xml,14140130_80860435_ffffffff.xml,14140130_15b31013_15b30014.xml,14140130_15b31015_19e5d11b.xml,14140130_15b31013_15b30013.xml,14140130_80861563_19e5d11a.xml,14140130_80861572_19e5d11c.xml,14140130_177d0012_ffffffff.xml,14140130_19e50009_ffffffff.xml","_30d.xml,HGSA_02.xml,SPSCA_COOLING_00.xml,14140130_19e50123_ffffffff.xml,14140130_10de15f8_ffffffff.xml,14140130_10de15f7_ffffffff.xml,14140130_80860435_ffffffff.xml,14140130_15b31013_15b30014.xml,14140130_15b31015_19e5d11b.xml,14140130_15b31013_15b30013.xml,14140130_80861563_19e5d11a.xml,14140130_80861572_19e5d11c.xml,14140130_177d0012_ffffffff.xml,14140130_19e50009_ffffffff.xml","_30d.xml,HGSA_01.xml,SPSCA_COOLING_00.xml,14140130_19e50123_ffffffff.xml,14140130_10de15f8_ffffffff.xml,14140130_10de15f7_ffffffff.xml,14140130_80860435_ffffffff.xml,14140130_15b31013_15b30014.xml,14140130_15b31015_19e5d11b.xml,14140130_15b31013_15b30013.xml,14140130_80861563_19e5d11a.xml,14140130_80861572_19e5d11c.xml,14140130_177d0012_ffffffff.xml,14140130_19e50009_ffffffff.xml","_30d.xml,HGSA_00.xml,SPSCA_COOLING_00.xml,14140130_19e50123_ffffffff.xml,14140130_10de15f8_ffffffff.xml,14140130_10de15f7_ffffffff.xml,14140130_80860435_ffffffff.xml,14140130_15b31013_15b30014.xml,14140130_15b31015_19e5d11b.xml,14140130_15b31013_15b30013.xml,14140130_80861563_19e5d11a.xml,14140130_80861572_19e5d11c.xml,14140130_177d0012_ffffffff.xml,14140130_19e50009_ffffffff.xml","_30e.xml,HGSB_01.xml,SPSCA_COOLING_00.xml,14140130_19e50123_ffffffff.xml,14140130_10de15f8_ffffffff.xml,14140130_10de15f7_ffffffff.xml,14140130_80860435_ffffffff.xml,14140130_15b31013_15b30014.xml,14140130_15b31015_19e5d11b.xml,14140130_15b31013_15b30013.xml,14140130_80861563_19e5d11a.xml,14140130_80861572_19e5d11c.xml,14140130_177d0012_ffffffff.xml,14140130_19e50009_ffffffff.xml"],
        'cx220':["_41b.xml,12VSTART_01.xml","_41b.xml,12VSTART_02.xml"],
        #'rh2288a':"_197.xml",
        #'rh1288a':"_198.xml",
        'MM810':"_365.xml",
        'ch140v3':["_442.xml,12VSTART_01.xml","_442.xml,12VSTART_02.xml"],
        'ch242v3':["_454.xml,14140130_19e50009_ffffffff.xml,NvmeDriveConfig_f0.xml,NvmeDriveConfig_f1.xml,NvmeDriveConfig_f2.xml,NvmeDriveConfig_f3.xml,12VSTART_01.xml","_454.xml,14140130_19e50009_ffffffff.xml,NvmeDriveConfig_f0.xml,NvmeDriveConfig_f1.xml,NvmeDriveConfig_f2.xml,NvmeDriveConfig_f3.xml,12VSTART_02.xml"],
        'ch226v3':["_456.xml,SGSA_00.xml"],
        'xh310v3':["_30f.xml,DUSA_00.xml","_30f.xml,DUSA_01.xml"],
        'ch225v3':["_458.xml,SGSD_00.xml,14140228_19e50123.xml","_458.xml,SGSD_00.xml,14140228_80860953.xml"],
        #'rh1288a_rh2288a':["_197.xml", "_198.xml"],
        'cx318':["_4b2.xml,LIUC_00.xml,12VSTART_01.xml,12VSTART_02.xml"],
        'cx620':["_41d.xml,12VSTART_01.xml,12VSTART_02.xml,XCUL_00.xml","_41d.xml,12VSTART_01.xml,12VSTART_02.xml,XCUL_01.xml"],
        'cx920':["_41a.xml,12VSTART_01.xml,12VSTART_02.xml"],
        #'ch121hv3':["_441.xml,SGCB_00.xml,14140228_f0.xml,14140228_f1.xml,14140228_80860953.xml","_441.xml,SGCB_01.xml,14140228_f0.xml,14140228_f1.xml,14140228_80860953.xml"],
        'ch121hv3':["_441.xml,SGCB_00.xml,14140228_f0.xml,14140228_f1.xml,12VSTART_01.xml,12VSTART_02.xml,14140130_19e50009_ffffffff.xml","_441.xml,SGCB_01.xml,14140228_f0.xml,14140228_f1.xml,12VSTART_01.xml,12VSTART_02.xml,14140130_19e50009_ffffffff.xml"],
        'cx320':["_41c.xml,12VSTART_01.xml,12VSTART_02.xml"],
        'xh321v3':["_3a0.xml,HGSCA_00.xml,14140228_80860953.xml,14140228_19e50123.xml,14140130_80860435_ffffffff.xml,14140130_15b31013_15b30014.xml,14140130_15b31013_15b30013.xml,14140130_177d0012_ffffffff.xml","_3a3.xml,HGSCB_00.xml,14140228_80860953.xml,14140228_19e50123.xml,14140130_80860435_ffffffff.xml,14140130_15b31013_15b30014.xml,14140130_15b31013_15b30013.xml,14140130_177d0012_ffffffff.xml"],
        'xh321v3_Loughborough':["_3a0.xml,HGSCA_00.xml","_3a3.xml,HGSCB_00.xml"],
        'xh321v5':["_30c.xml,SPSA_VRD_00.xml,SPSCA_COOLING_00.xml,SPSCA_COOLING_02.xml,AEP_01.xml,AEP_02.xml,CPUPKG_01.xml,CPUPKG_02.xml,CustomizeSensor.xml,SPSCA_NCSI_00.xml",
                    "_30b.xml,SPSA_VRD_01.xml,SPSCA_COOLING_01.xml,CPUPKG_01.xml,CPUPKG_02.xml,CustomizeSensor.xml"],
        'xh628v5':["_309.xml,MBSA_VRD_00.xml,MBSA_COOLING_00.xml,MBSA_01.xml,CPUPKG_02.xml,CPUPKG_01.xml,CustomizeSensor.xml,AEP_01.xml,AEP_02.xml","_309.xml,MBSA_VRD_01.xml"],
        'MM630':"_366.xml",
        'MM650':["_368.xml,LSW_00.xml"],
        'rh8100v5':["_8b5.xml,8100_03.xml,8100_BASE_00.xml", "_8b5.xml,8100_01.xml", "_8b5.xml,8100_00.xml,8100_BASE_00.xml","_8b5.xml,8100_02.xml,8100_BASE_00.xml","_8b5.xml,8100_04.xml,8100_BASE_00.xml","_8b5.xml,8100_05.xml,8100_BASE_00.xml"],
        '9008v5base':["_8bf.xml,8100_03.xml,8100_BASE_00.xml", "_8bf.xml,8100_01.xml", "_8bf.xml,8100_00.xml,8100_BASE_00.xml","_8bf.xml,8100_02.xml,8100_BASE_00.xml","_8bf.xml,8100_04.xml,8100_BASE_00.xml","_8bf.xml,8100_05.xml,8100_BASE_00.xml"],
        '9008v5advance':[
        "_8bd.xml,9008_SCE_00.xml,9008_00.xml,9008_BASE_00.xml",
        "_8bd.xml,9008_SCE_00.xml,9008_01.xml,9008_BASE_00.xml",
        "_8bd.xml,9008_SCE_00.xml,9008_02.xml,9008_BASE_00.xml",
        "_8bd.xml,9008_SCE_00.xml,9008_03.xml,9008_BASE_00.xml,PRODUCT_M_03.xml",
        "_8bd.xml,9008_SCE_00.xml,9008_03.xml,9008_BASE_00.xml,PRODUCT_M_ff.xml",
        "_8bd.xml,9008_SCE_00.xml,9008_04.xml,9008_BASE_00.xml",
        "_8bd.xml,9008_SCE_00.xml,9008_05.xml,9008_BASE_00.xml",
        "_8bd.xml,9008_SCE_01.xml,9008_00.xml,9008_BASE_00.xml",
        "_8bd.xml,9008_SCE_01.xml,9008_01.xml,9008_BASE_00.xml",
        "_8bd.xml,9008_SCE_01.xml,9008_02.xml,9008_BASE_00.xml",
        "_8bd.xml,9008_SCE_01.xml,9008_03.xml,9008_BASE_00.xml,PRODUCT_M_03.xml",
        "_8bd.xml,9008_SCE_01.xml,9008_03.xml,9008_BASE_00.xml,PRODUCT_M_ff.xml",
        "_8bd.xml,9008_SCE_01.xml,9008_04.xml,9008_BASE_00.xml",
        "_8bd.xml,9008_SCE_01.xml,9008_05.xml,9008_BASE_00.xml",
        "_8bd.xml,9008_SCE_02.xml,9008_00.xml,9008_BASE_00.xml",
        "_8bd.xml,9008_SCE_02.xml,9008_01.xml,9008_BASE_00.xml",
        "_8bd.xml,9008_SCE_02.xml,9008_02.xml,9008_BASE_00.xml",
        "_8bd.xml,9008_SCE_02.xml,9008_03.xml,9008_BASE_00.xml,PRODUCT_M_03.xml",
        "_8bd.xml,9008_SCE_02.xml,9008_03.xml,9008_BASE_00.xml,PRODUCT_M_ff.xml",
        "_8bd.xml,9008_SCE_02.xml,9008_04.xml,9008_BASE_00.xml",
        "_8bd.xml,9008_SCE_02.xml,9008_05.xml,9008_BASE_00.xml",
        "_8bd.xml,9008_SCE_03.xml,9008_00.xml,9008_BASE_00.xml",
        "_8bd.xml,9008_SCE_03.xml,9008_01.xml,9008_BASE_00.xml",
        "_8bd.xml,9008_SCE_03.xml,9008_02.xml,9008_BASE_00.xml",
        "_8bd.xml,9008_SCE_03.xml,9008_03.xml,9008_BASE_00.xml,PRODUCT_M_03.xml",
        "_8bd.xml,9008_SCE_03.xml,9008_03.xml,9008_BASE_00.xml,PRODUCT_M_ff.xml",
        "_8bd.xml,9008_SCE_03.xml,9008_04.xml,9008_BASE_00.xml",
        "_8bd.xml,9008_SCE_03.xml,9008_05.xml,9008_BASE_00.xml",
        "_8bd.xml,9008_SCE_00.xml,9008_00.xml,9008_BASE_00.xml",
        "_8bd.xml,9008_SCE_04.xml,9008_01.xml,9008_BASE_00.xml",
        "_8bd.xml,9008_SCE_04.xml,9008_02.xml,9008_BASE_00.xml",
        "_8bd.xml,9008_SCE_04.xml,9008_03.xml,9008_BASE_00.xml,PRODUCT_M_03.xml",
        "_8bd.xml,9008_SCE_04.xml,9008_03.xml,9008_BASE_00.xml,PRODUCT_M_ff.xml",
        "_8bd.xml,9008_SCE_04.xml,9008_04.xml,9008_BASE_00.xml",
        "_8bd.xml,9008_SCE_04.xml,9008_05.xml,9008_BASE_00.xml",
        ],
        'cx916':["_414.xml,12VSTART_01.xml,14050320_04.xml,XCUN_01.xml","_414.xml,12VSTART_01.xml,14050320_04.xml,XCUN_02.xml"],
        'cx930':["_41e.xml,12VSTART_01.xml"],
        'cx820':["_416.xml,12VSTART_01.xml"],
        'TaiShan2280_5280':["_106.xml,baseboard_01.xml,CustomizeSensor.xml,SPCD_00.xml","_106.xml,baseboard_02.xml,CustomizeSensor.xml,SPCD_00.xml",
        "_106.xml,baseboard_00.xml,CustomizeSensor.xml,SPCD_00.xml"],
        'xr320':["_1a5.xml,SPSA_VRD_00.xml,CustomizeSensor.xml","_1a5.xml,SPSA_VRD_01.xml,CustomizeSensor.xml","_3a5.xml,SPSA_VRD_00.xml,CustomizeSensor.xml","_3a5.xml,SPSA_VRD_01.xml,CustomizeSensor.xml"],
        'g560':["_250.xml,SGSD_00.xml"],
        'MM510': [
            ("_200.xml,PHY_CHIP_CFG_00.xml,PHY_CFG_00.xml,ACTIVE_SMM_CFG_ff.xml"),
            ("_200.xml,PHY_CHIP_CFG_00.xml,PHY_CFG_00.xml,STANDBY_SMM_CFG_ff.xml")
        ],
        'g530v5': [
            ("_23c.xml,12VSTART_01.xml,PCHBASE10G_ff.xml,DS125CFG_da.xml,CPUPKG_01.xml,COOLING_POLICY_01.xml,"
                "VRD_MGNT_00.xml,VRD_COOLING_POLICY_00.xml,LOM_MGNT_01.xml,AEP_01.xml,AEP_02.xml"),
            ("_23c.xml,12VSTART_01.xml,PCHBASE10G_ff.xml,DS125CFG_da.xml,CPUPKG_01.xml,COOLING_POLICY_01.xml,"
                "VRD_MGNT_01.xml,VRD_COOLING_POLICY_00.xml,LOM_MGNT_01.xml,AEP_01.xml,AEP_02.xml")
        ],
        'g560v5': [
            ("_251.xml,AEP_01.xml,AEP_02.xml,12VSTART_01.xml,PCHBASE10G_ff.xml,DS125CFG_da.xml,COOLING_POLICY_01.xml,"
                "VRD_MGNT_00.xml,VRD_COOLING_POLICY_00.xml"),
            ("_251.xml,AEP_01.xml,AEP_02.xml,12VSTART_01.xml,PCHBASE10G_ff.xml,DS125CFG_dc.xml,COOLING_POLICY_01.xml,"
                "VRD_MGNT_00.xml,VRD_COOLING_POLICY_00.xml")
        ],
        'MM920':["_401.xml"],
        'MM921':["_403.xml"],
        'FDController':["_402.xml"],
        'SMM':[
        "_608.xml,PRODUCT_608_01.xml,LswComm_08.xml,LSW_608_01.xml,BackPlane_601_07.xml,CustomizeSensor.xml",
        "_608.xml,PRODUCT_608_02.xml,LswComm_08.xml,LSW_608_02.xml,BackPlane_602_07.xml,CustomizeSensor.xml",
        "_609.xml,PRODUCT_609_01.xml,LswComm_09.xml,LSW_609_01.xml,BackPlane_601_07.xml,CustomizeSensor.xml",
        "_609.xml,PRODUCT_609_02.xml,LswComm_09.xml,LSW_609_02.xml,BackPlane_602_07.xml,CustomizeSensor.xml",
        ],
        'RM210':["_76d.xml,VirtualALBoard_ff.xml"],
        'RM210_SECURITY_3V0':["_76d.xml,VirtualALBoard_ff.xml"],
        'RM211':["_76c.xml,VirtualALBoard_ff.xml,ToolBoard_ff.xml"],
		'TaiShan2280v2':[
		"_1b9.xml,BASEBOARD_b9.xml,ADC_b9.xml,VRD_b9_01.xml,COOLING_00.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_00.xml,SPSCA_4U_00.xml,PRODUCT_00.xml,EXCLUDE_HDD_00.xml,PCIeAddrInfo_ff.xml,Riser2_31.xml,RearBp_4e.xml,M2_SATA_SSD_00.xml",
		"_1b9.xml,BASEBOARD_b9_02.xml,ADC_b9.xml,VRD_b9_02.xml,COOLING_00_02.xml,FAN_b9_02.xml,PSevent_b9.xml,CLOCK_01.xml,SPSCA_4U_00.xml,PRODUCT_00_10.xml,PCIeAddrInfo_02.xml,Riser1_30.xml,M2_SATA_SSD_00.xml",
		"_1b9.xml,BASEBOARD_b9_01.xml,ADC_b9.xml,VRD1P_b9_01.xml,COOLING_00_01.xml,FAN_b9.xml,PSevent_b9_01.xml,CLOCK_00.xml,SPSCA_4U_00.xml,PRODUCT_00_20.xml,EXCLUDE_HDD_01.xml,PCIeAddrInfo_01.xml,Riser1_31.xml,M2_SATA_SSD_00.xml",
		"_1b9.xml,BASEBOARD_b9_01.xml,ADC_b9.xml,VRD1P_b9_02.xml,COOLING_00_01.xml,FAN_b9.xml,PSevent_b9_01.xml,CLOCK_00.xml,SPSCA_4U_00.xml,PRODUCT_00_40.xml,PCIeAddrInfo_01.xml,Riser1_3b.xml,M2_SATA_SSD_00.xml",
		"_1b9.xml,BASEBOARD_b9_01.xml,ADC_b9.xml,VRD1P_b9_05.xml,COOLING_00_01.xml,FAN_b9.xml,PSevent_b9_01.xml,CLOCK_00.xml,SPSCA_4U_00.xml,PRODUCT_00_60.xml,PCIeAddrInfo_01.xml,Riser1_32.xml,M2_SATA_SSD_00.xml",
		"_1b9.xml,BASEBOARD_b9.xml,ADC_b9.xml,VRD_b9_01.xml,COOLING_01.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_01.xml,SPSCA_4U_00.xml,PRODUCT_01.xml,CustomizeSensor.xml,PCIeAddrInfo_ff.xml,Riser2_3b.xml,M2_SATA_SSD_00.xml",
		"_1b9.xml,BASEBOARD_b9.xml,ADC_b9.xml,VRD_b9_05.xml,COOLING_01.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_00.xml,SPSCA_4U_00.xml,PRODUCT_01_10.xml,PCIeAddrInfo_ff.xml,Riser2_9f.xml,M2_SATA_SSD_00.xml",
		"_1b9.xml,BASEBOARD_b9_01.xml,ADC_b9.xml,VRD1P_b9_05.xml,COOLING_01_01.xml,FAN_b9.xml,PSevent_b9_01.xml,CLOCK_01.xml,SPSCA_4U_00.xml,PRODUCT_01_20.xml,PCIeAddrInfo_01.xml,Riser1_38.xml,M2_SATA_SSD_00.xml",
		"_1b9.xml,BASEBOARD_b9.xml,ADC_b9.xml,VRD_b9_05.xml,COOLING_01.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_00.xml,SPSCA_4U_00.xml,PRODUCT_01_40.xml,PCIeAddrInfo_ff.xml,Riser2_35.xml,M2_SATA_SSD_00.xml",
		"_1a9.xml,BASEBOARD_b9.xml,ADC_b9.xml,VRD_b9_01.xml,COOLING_00.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_00.xml,SPSCA_4U_00.xml,PRODUCT_02.xml,PCIeAddrInfo_ff.xml,Riser2_94.xml,M2_SATA_SSD_00.xml",
		"_1a9.xml,BASEBOARD_b9_02.xml,ADC_b9.xml,VRD_b9_01.xml,COOLING_00_02.xml,FAN_b9_02.xml,PSevent_b9.xml,CLOCK_01.xml,SPSCA_4U_00.xml,PRODUCT_02_10.xml,PCIeAddrInfo_02.xml,Riser1_30.xml,M2_SATA_SSD_00.xml",
		"_1a9.xml,BASEBOARD_b9_01.xml,ADC_b9.xml,VRD1P_b9_01.xml,COOLING_00_01.xml,FAN_b9.xml,PSevent_b9_01.xml,CLOCK_00.xml,SPSCA_4U_00.xml,PRODUCT_02_20.xml,PCIeAddrInfo_01.xml,Riser1_90.xml,M2_SATA_SSD_00.xml",
		"_1a9.xml,BASEBOARD_b9_01.xml,ADC_b9.xml,VRD1P_b9_05.xml,COOLING_00_01.xml,FAN_b9.xml,PSevent_b9_01.xml,CLOCK_00.xml,SPSCA_4U_00.xml,PRODUCT_02_40.xml,PCIeAddrInfo_01.xml,Riser1_91.xml,M2_SATA_SSD_00.xml",
		"_1a9.xml,BASEBOARD_b9_01.xml,ADC_b9.xml,VRD1P_b9_01.xml,COOLING_00_01.xml,FAN_b9.xml,PSevent_b9_01.xml,CLOCK_00.xml,SPSCA_4U_00.xml,PRODUCT_02_60.xml,PCIeAddrInfo_01.xml,Riser1_94.xml,M2_SATA_SSD_00.xml",
		"_1a9.xml,BASEBOARD_b9.xml,ADC_b9.xml,VRD_b9_01.xml,COOLING_01.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_01.xml,SPSCA_4U_00.xml,PRODUCT_03.xml,PCIeAddrInfo_ff.xml,Riser2_95.xml,M2_SATA_SSD_00.xml",
		"_1a9.xml,BASEBOARD_b9_01.xml,ADC_b9.xml,VRD1P_b9_05.xml,COOLING_01_01.xml,FAN_b9.xml,PSevent_b9_01.xml,CLOCK_01.xml,SPSCA_4U_00.xml,PRODUCT_03_10.xml,PCIeAddrInfo_01.xml,Riser1_95.xml,M2_SATA_SSD_00.xml",
		"_1a9.xml,BASEBOARD_b9_01.xml,ADC_b9.xml,VRD1P_b9_05.xml,COOLING_01_01.xml,FAN_b9.xml,PSevent_b9_01.xml,CLOCK_01.xml,SPSCA_4U_00.xml,PRODUCT_03_20.xml,PCIeAddrInfo_01.xml,Riser1_30.xml,Riser1_30_02.xml,M2_SATA_SSD_00.xml",
		"_1a9.xml,BASEBOARD_b9_01.xml,ADC_b9.xml,VRD1P_b9_05.xml,COOLING_01_01.xml,FAN_b9.xml,PSevent_b9_01.xml,CLOCK_01.xml,SPSCA_4U_00.xml,PRODUCT_03_40.xml,PCIeAddrInfo_01.xml,Riser1_30.xml,Riser1_30_01.xml,M2_SATA_SSD_00.xml",
		"_aa9.xml,BASEBOARD_b9.xml,ADC_b9.xml,VRD_b9_01.xml,COOLING_00.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_00.xml,SPSCA_4U_00.xml,PRODUCT_04.xml,PCIeAddrInfo_ff.xml,M2_SATA_SSD_00.xml",
		"_aa9.xml,BASEBOARD_b9.xml,ADC_b9.xml,VRD_b9_01.xml,COOLING_00.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_00.xml,SPSCA_4U_00.xml,PRODUCT_04_10.xml,PCIeAddrInfo_ff.xml,Riser3_37.xml,M2_SATA_SSD_00.xml",
		"_aa9.xml,BASEBOARD_b9.xml,ADC_b9.xml,VRD_b9_01.xml,COOLING_00.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_00.xml,SPSCA_4U_00.xml,PRODUCT_04_20.xml,PCIeAddrInfo_ff.xml,Riser2_31.xml,M2_SATA_SSD_00.xml",
		"_aa9.xml,BASEBOARD_b9.xml,ADC_b9.xml,VRD_b9_01.xml,COOLING_00.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_00.xml,SPSCA_4U_00.xml,PRODUCT_04_40.xml,PCIeAddrInfo_ff.xml,Riser2_90.xml,M2_SATA_SSD_00.xml",
		"_aa9.xml,BASEBOARD_b9.xml,ADC_b9.xml,VRD_b9_01.xml,COOLING_00.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_00.xml,SPSCA_4U_00.xml,PRODUCT_04_60.xml,PCIeAddrInfo_ff.xml,Riser2_31.xml,M2_SATA_SSD_00.xml",
		"_aa9.xml,BASEBOARD_b9.xml,ADC_b9.xml,VRD_b9_01.xml,COOLING_00.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_00.xml,SPSCA_4U_00.xml,PRODUCT_05.xml,PCIeAddrInfo_ff.xml,Riser2_31.xml,M2_SATA_SSD_00.xml",
		"_aa9.xml,BASEBOARD_b9.xml,ADC_b9.xml,VRD_b9_01.xml,COOLING_00.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_00.xml,SPSCA_4U_00.xml,PRODUCT_05_10.xml,PCIeAddrInfo_ff.xml,Riser2_31.xml,M2_SATA_SSD_00.xml",
		"_aa9.xml,BASEBOARD_b9.xml,ADC_b9.xml,VRD_b9_01.xml,COOLING_00.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_00.xml,SPSCA_4U_00.xml,PRODUCT_05_20.xml,PCIeAddrInfo_ff.xml,Riser2_31.xml,M2_SATA_SSD_00.xml",
		"_aa9.xml,BASEBOARD_b9.xml,ADC_b9.xml,VRD_b9_01.xml,COOLING_00.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_00.xml,SPSCA_4U_00.xml,PRODUCT_05_40.xml,PCIeAddrInfo_ff.xml,Riser3_3c.xml,M2_SATA_SSD_00.xml",
        "_1a5.xml,BASEBOARD_a5.xml,ADC_a5.xml,VRD_a5_01.xml,COOLING_00.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_00.xml,SPSCA_4U_00.xml,PRODUCT_A5_00.xml,PCIeAddrInfo_ff.xml,Riser2_38.xml,M2_SATA_SSD_00.xml",
		"_1a5.xml,BASEBOARD_a5_02.xml,ADC_a5.xml,VRD_a5_05.xml,COOLING_00_02.xml,FAN_b9_02.xml,PSevent_b9.xml,CLOCK_00.xml,SPSCA_4U_00.xml,PRODUCT_A5_00_10.xml,PCIeAddrInfo_02.xml,Riser2_31.xml,M2_SATA_SSD_00.xml",
        "_1a5.xml,BASEBOARD_a5.xml,ADC_a5.xml,VRD_a5_01.xml,COOLING_00.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_00.xml,SPSCA_4U_00.xml,PRODUCT_A5_00_20.xml,PCIeAddrInfo_ff.xml,Riser2_31.xml,M2_SATA_SSD_00.xml",
		"_1a5.xml,BASEBOARD_a5_01.xml,ADC_a5.xml,VRD1P_a5_01.xml,COOLING_00_01.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_00.xml,SPSCA_4U_00.xml,PRODUCT_A5_00_40.xml,PCIeAddrInfo_01.xml,Riser2_31.xml,M2_SATA_SSD_00.xml",
        "_1a5.xml,BASEBOARD_a5_01.xml,ADC_a5.xml,VRD1P_a5_05.xml,COOLING_00_01.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_00.xml,SPSCA_4U_00.xml,PRODUCT_A5_00_60.xml,PCIeAddrInfo_01.xml,Riser2_31.xml,M2_SATA_SSD_00.xml",
        "_1a5.xml,BASEBOARD_a5.xml,ADC_a5.xml,VRD_a5_01.xml,COOLING_01.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_00.xml,SPSCA_4U_00.xml,PRODUCT_A5_01.xml,PCIeAddrInfo_ff.xml,Riser2_31.xml,M2_SATA_SSD_00.xml",
		"_1a5.xml,BASEBOARD_a5.xml,ADC_a5.xml,VRD_a5_01.xml,COOLING_01.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_00.xml,SPSCA_4U_00.xml,PRODUCT_A5_01_10.xml,PCIeAddrInfo_ff.xml,Riser2_31.xml,M2_SATA_SSD_00.xml",
		"_1a5.xml,BASEBOARD_a5.xml,ADC_a5.xml,VRD_a5_01.xml,COOLING_01.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_00.xml,SPSCA_4U_00.xml,PRODUCT_A5_01_20.xml,PCIeAddrInfo_ff.xml,Riser2_31.xml,M2_SATA_SSD_00.xml",
		"_1a5.xml,BASEBOARD_a5.xml,ADC_a5.xml,VRD_a5_01.xml,COOLING_01.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_00.xml,SPSCA_4U_00.xml,PRODUCT_A5_01_40.xml,PCIeAddrInfo_ff.xml,Riser2_31.xml,M2_SATA_SSD_00.xml",
		"_1a5.xml,BASEBOARD_a5.xml,ADC_a5.xml,VRD_a5_01.xml,COOLING_01.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_00.xml,SPSCA_4U_00.xml,PRODUCT_A5_01_60.xml,PCIeAddrInfo_ff.xml,Riser2_31.xml,M2_SATA_SSD_00.xml",
		"_aa5.xml,BASEBOARD_a5_01.xml,ADC_a5.xml,VRD1P_a5_01.xml,COOLING_00_01.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_00.xml,SPSCA_4U_00.xml,PRODUCT_A5_04.xml,PCIeAddrInfo_01.xml,Riser2_31.xml,M2_SATA_SSD_00.xml",
		"_aa5.xml,BASEBOARD_a5_01.xml,ADC_a5.xml,VRD1P_a5_01.xml,COOLING_00_01.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_00.xml,SPSCA_4U_00.xml,PRODUCT_A5_04_10.xml,PCIeAddrInfo_01.xml,Riser2_31.xml,M2_SATA_SSD_00.xml",
		"_aa5.xml,BASEBOARD_a5_01.xml,ADC_a5.xml,VRD1P_a5_01.xml,COOLING_00_01.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_00.xml,SPSCA_4U_00.xml,PRODUCT_A5_04_20.xml,PCIeAddrInfo_01.xml,Riser2_31.xml,M2_SATA_SSD_00.xml",
		"_aa5.xml,BASEBOARD_a5_01.xml,ADC_a5.xml,VRD1P_a5_01.xml,COOLING_00_01.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_00.xml,SPSCA_4U_00.xml,PRODUCT_A5_04_40.xml,PCIeAddrInfo_01.xml,Riser2_31.xml,M2_SATA_SSD_00.xml",
		"_aa5.xml,BASEBOARD_a5_01.xml,ADC_a5.xml,VRD1P_a5_01.xml,COOLING_00_01.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_00.xml,SPSCA_4U_00.xml,PRODUCT_A5_05.xml,PCIeAddrInfo_01.xml,Riser2_31.xml,M2_SATA_SSD_00.xml",
		"_aa5.xml,BASEBOARD_a5_01.xml,ADC_a5.xml,VRD1P_a5_01.xml,COOLING_00_01.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_01.xml,SPSCA_4U_00.xml,PRODUCT_A5_05_10.xml,PCIeAddrInfo_01.xml,Riser2_31.xml,M2_SATA_SSD_00.xml",
		"_aa5.xml,BASEBOARD_a5_01.xml,ADC_a5.xml,VRD1P_a5_01.xml,COOLING_00_01.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_00.xml,SPSCA_4U_00.xml,PRODUCT_A5_05_20.xml,PCIeAddrInfo_01.xml,Riser2_31.xml,M2_SATA_SSD_00.xml",
		"_aa5.xml,BASEBOARD_a5_01.xml,ADC_a5.xml,VRD1P_a5_05.xml,COOLING_00_01.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_00.xml,SPSCA_4U_00.xml,PRODUCT_A5_05_40.xml,PCIeAddrInfo_01.xml,Riser2_31.xml,M2_SATA_SSD_00.xml",
		],
		'TaiShan2280Mv2':[
		#2280 2.6G
		"_ea9.xml,BASEBOARD_b9.xml,ADC_b9.xml,VRD_b9_01.xml,COOLING_00.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_00.xml,SPSCA_4U_00.xml,PRODUCT_02.xml,PCIeAddrInfo_ff.xml,Riser1_3b.xml,Riser2_3d.xml,Riser3_3c.xml,RearBp_4e.xml,M2_SATA_SSD_00.xml,CustomizeSensor.xml",
		#2280 2.6G ST VRD
		"_ea9.xml,BASEBOARD_b9.xml,ADC_b9.xml,VRD_b9_05.xml,COOLING_00.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_01.xml,SPSCA_4U_00.xml,PRODUCT_02.xml,PCIeAddrInfo_ff.xml,Riser1_30.xml,Riser1_30_01.xml,Riser2_9f.xml,RearBp_4e.xml,M2_SATA_SSD_00.xml,CustomizeSensor.xml",
		#5280 3.0G
		"_ea9.xml,BASEBOARD_b9.xml,ADC_b9.xml,VRD_b9_02.xml,COOLING_01.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_00.xml,SPSCA_4U_00.xml,PRODUCT_03.xml,PCIeAddrInfo_ff.xml,Riser1_3d.xml,Riser2_31.xml,Riser3_37.xml,RearBp_47.xml,M2_SATA_SSD_00.xml,CustomizeSensor.xml",
		#2180
		"_ea9.xml,BASEBOARD_b9_01.xml,ADC_b9.xml,VRD1P_b9_01.xml,COOLING_00_01.xml,FAN_b9.xml,PSevent_b9_01.xml,CLOCK_00.xml,SPSCA_4U_00.xml,PRODUCT_02_40.xml,PCIeAddrInfo_01.xml,Riser1_31.xml,M2_SATA_SSD_00.xml,CustomizeSensor.xml",
		#2180 2.6G ST VRD
		"_ea9.xml,BASEBOARD_b9_01.xml,ADC_b9.xml,VRD1P_b9_05.xml,COOLING_00_01.xml,FAN_b9.xml,PSevent_b9_01.xml,CLOCK_00.xml,SPSCA_4U_00.xml,PRODUCT_02_40.xml,PCIeAddrInfo_01.xml,Riser1_32.xml,M2_SATA_SSD_00.xml,CustomizeSensor.xml",
		#5280 1P 3.0G
		"_ea9.xml,BASEBOARD_b9_01.xml,ADC_b9.xml,VRD1P_b9_02.xml,COOLING_01_01.xml,FAN_b9.xml,PSevent_b9_01.xml,CLOCK_01.xml,SPSCA_4U_00.xml,PRODUCT_03_40.xml,PCIeAddrInfo_01.xml,Riser1_38.xml,M2_SATA_SSD_00.xml,CustomizeSensor.xml",
		#2280M 2.6G
		"_ea9.xml,BASEBOARD_b9.xml,ADC_b9.xml,VRD_b9_05.xml,COOLING_00.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_01.xml,SPSCA_4U_00.xml,PRODUCT_04.xml,PCIeAddrInfo_ff.xml,Riser1_3b.xml,Riser2_3b.xml,RearBp_4e.xml,M2_SATA_SSD_00.xml,CustomizeSensor.xml",
		#2280K 2.6G ST VRD
		"_ea9.xml,BASEBOARD_b9.xml,ADC_b9.xml,VRD_b9_05.xml,COOLING_00.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_01.xml,SPSCA_4U_00.xml,PRODUCT_02_20.xml,PCIeAddrInfo_ff.xml,Riser1_30.xml,Riser1_30_02.xml,Riser2_94.xml,RearBp_4e.xml,M2_SATA_SSD_00.xml,CustomizeSensor.xml",
		#1280
		"_ea9.xml,BASEBOARD_b9_02.xml,ADC_b9.xml,VRD_b9_01.xml,COOLING_00_02.xml,FAN_b9_02.xml,PSevent_b9.xml,CLOCK_01.xml,SPSCA_4U_00.xml,PRODUCT_03_10.xml,PCIeAddrInfo_02.xml,Riser1_39.xml,Riser2_32.xml,Riser3_3c.xml,Raid_3e.xml,M2_SATA_SSD_00.xml,CustomizeSensor.xml",
		#1280 pro MPS VRD
        "_ea9.xml,BASEBOARD_b9_02.xml,ADC_b9.xml,VRD_b9_51.xml,COOLING_00_02.xml,FAN_b9_02.xml,PSevent_b9.xml,CLOCK_00.xml,PRODUCT_02_80.xml,SPSCA_4U_00.xml,PCIeAddrInfo_02.xml,Riser1_30.xml,Riser2_90.xml,M2_SATA_SSD_00.xml,CustomizeSensor.xml",
		#1280 pro PXP1600 VRD
		"_ea9.xml,BASEBOARD_b9_02.xml,ADC_b9.xml,VRD_b9_0a.xml,COOLING_00_02.xml,FAN_b9_02.xml,PSevent_b9.xml,CLOCK_00.xml,PRODUCT_02_80.xml,SPSCA_4U_00.xml,PCIeAddrInfo_02.xml,Riser1_30.xml,Riser2_90.xml,M2_SATA_SSD_00.xml,CustomizeSensor.xml",
        #2280 1620s
		"_ea5.xml,BASEBOARD_a5.xml,ADC_a5.xml,VRD_a5_01.xml,COOLING_00.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_00.xml,SPSCA_4U_00.xml,PRODUCT_A5_00.xml,PCIeAddrInfo_ff.xml,Riser1_90.xml,Riser2_35.xml,Riser3_3c.xml,RearBp_4e.xml,M2_SATA_SSD_00.xml,CustomizeSensor.xml",
		#2280 1620s ST VRD
		"_ea5.xml,BASEBOARD_a5.xml,ADC_a5.xml,VRD_a5_05.xml,COOLING_00.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_01.xml,SPSCA_4U_00.xml,PRODUCT_A5_00.xml,PCIeAddrInfo_ff.xml,Riser1_30.xml,Riser1_30_02.xml,Riser2_38.xml,RearBp_4e.xml,M2_SATA_SSD_00.xml,CustomizeSensor.xml",
		#5280 1620s INF UPI MOS
		"_ea5.xml,BASEBOARD_a5.xml,ADC_a5.xml,VRD_a5_21.xml,COOLING_01.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_01.xml,SPSCA_4U_00.xml,PRODUCT_A5_01.xml,PCIeAddrInfo_ff.xml,Riser1_91.xml,Riser2_39.xml,Riser3_37.xml,RearBp_47.xml,M2_SATA_SSD_00.xml,CustomizeSensor.xml",
		#2180 1620s
		"_ea5.xml,BASEBOARD_a5_01.xml,ADC_a5_01.xml,VRD1P_a5_01.xml,COOLING_00_01.xml,FAN_b9.xml,PSevent_b9_01.xml,CLOCK_00.xml,SPSCA_4U_00.xml,PRODUCT_A5_00_40.xml,PCIeAddrInfo_01.xml,Riser1_94.xml,M2_SATA_SSD_00.xml,CustomizeSensor.xml",
		#5280 1620s 1P
		"_ea5.xml,BASEBOARD_a5_01.xml,ADC_a5_01.xml,VRD1P_a5_01.xml,COOLING_01_01.xml,FAN_b9.xml,PSevent_b9_01.xml,CLOCK_00.xml,SPSCA_4U_00.xml,PRODUCT_A5_01_40.xml,PCIeAddrInfo_01.xml,Riser1_95.xml,M2_SATA_SSD_00.xml,CustomizeSensor.xml",
		#2280M 1620s
		"_ea5.xml,BASEBOARD_a5.xml,ADC_a5.xml,VRD_a5_01.xml,COOLING_00.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_00.xml,SPSCA_4U_00.xml,PRODUCT_A5_00.xml,PCIeAddrInfo_ff.xml,Riser1_90.xml,Riser2_90.xml,Riser3_3c.xml,RearBp_4e.xml,M2_SATA_SSD_00.xml,CustomizeSensor.xml",
		#2280M 1P 1620s
		"_ea5.xml,BASEBOARD_a5_01.xml,ADC_a5_01.xml,VRD1P_a5_01.xml,COOLING_00_01.xml,FAN_b9.xml,PSevent_b9_01.xml,CLOCK_00.xml,SPSCA_4U_00.xml,PRODUCT_A5_04_40.xml,PCIeAddrInfo_01.xml,Riser1_94.xml,M2_SATA_SSD_00.xml,CustomizeSensor.xml",
		#5280K 1620s INF UPI MOS
		"_ea5.xml,BASEBOARD_a5.xml,ADC_a5.xml,VRD_a5_21.xml,COOLING_01.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_01.xml,SPSCA_4U_00.xml,PRODUCT_A5_01.xml,PCIeAddrInfo_ff.xml,Riser1_91.xml,Riser2_95.xml,Riser3_37.xml,RearBp_47.xml,M2_SATA_SSD_00.xml,CustomizeSensor.xml",
		#Atlas 800 3000
		"_ea9.xml,BASEBOARD_b9.xml,ADC_b9.xml,VRD_b9_09.xml,COOLING_00.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_00.xml,SPSCA_4U_00.xml,PRODUCT_02.xml,PCIeAddrInfo_ff.xml,Riser1_94.xml,Riser2_94.xml,Riser3_36.xml,Riser4_33.xml,M2_SATA_SSD_00.xml,CustomizeSensor.xml,UPGRADE_CPLD_00.xml,TENCENTDATA_00.xml"
		],
		'xa320':[
		"_3a6.xml,SPSA_VRD_01.xml,BASEBOARD_00.xml,LOM_a6.xml,COOLING_00.xml,CustomizeSensor.xml,PCIeAddrInfo_ff.xml,PSUCustomizeSensor.xml,Riser2_97.xml",
        "_3a6.xml,SPSA_VRD_01.xml,BASEBOARD_00.xml,LOM_a6.xml,COOLING_01.xml,CustomizeSensor.xml,PCIeAddrInfo_ff.xml,PSUCustomizeSensor.xml,Riser2_ff.xml",
        "_3b0.xml,SPSA_VRD_01.xml,BASEBOARD_00.xml,LOM_b0.xml,COOLING_00.xml,CustomizeSensor.xml,PCIeAddrInfo_ff.xml,PSUCustomizeSensor.xml,Riser2_97.xml",
        "_3b0.xml,SPSA_VRD_01.xml,BASEBOARD_00.xml,LOM_b0.xml,COOLING_01.xml,CustomizeSensor.xml,PCIeAddrInfo_ff.xml,PSUCustomizeSensor.xml,Riser2_ff.xml",
        "_3b1.xml,SPSA_VRD_01.xml,BASEBOARD_00.xml,LOM_b1.xml,COOLING_00.xml,CustomizeSensor.xml,PCIeAddrInfo_ff.xml,PSUCustomizeSensor.xml,Riser2_97.xml",
        "_3b1.xml,SPSA_VRD_01.xml,BASEBOARD_00.xml,LOM_b1.xml,COOLING_01.xml,CustomizeSensor.xml,PCIeAddrInfo_ff.xml,PSUCustomizeSensor.xml,Riser2_ff.xml",
        "_3b2.xml,SPSA_VRD_01.xml,BASEBOARD_00.xml,LOM_b2.xml,COOLING_00.xml,CustomizeSensor.xml,PCIeAddrInfo_ff.xml,PSUCustomizeSensor.xml,Riser2_97.xml",
        "_3b2.xml,SPSA_VRD_01.xml,BASEBOARD_00.xml,LOM_b2.xml,COOLING_01.xml,CustomizeSensor.xml,PCIeAddrInfo_ff.xml,PSUCustomizeSensor.xml,Riser2_ff.xml",
        "_3b3.xml,SPSA_VRD_03.xml,BASEBOARD_00.xml,LOM_b3.xml,COOLING_00.xml,CustomizeSensor.xml,PCIeAddrInfo_ff.xml,PSUCustomizeSensor.xml,Riser2_97.xml",
        "_3b3.xml,SPSA_VRD_03.xml,BASEBOARD_00.xml,LOM_b3.xml,COOLING_01.xml,CustomizeSensor.xml,PCIeAddrInfo_ff.xml,PSUCustomizeSensor.xml,Riser2_ff.xml",
        "_3b4.xml,SPSA_VRD_03.xml,BASEBOARD_00.xml,LOM_b4.xml,COOLING_00.xml,CustomizeSensor.xml,PCIeAddrInfo_ff.xml,PSUCustomizeSensor.xml,Riser2_97.xml",
        "_3b4.xml,SPSA_VRD_03.xml,BASEBOARD_00.xml,LOM_b4.xml,COOLING_01.xml,CustomizeSensor.xml,PCIeAddrInfo_ff.xml,PSUCustomizeSensor.xml,Riser2_ff.xml",
        "_3b5.xml,SPSA_VRD_03.xml,BASEBOARD_00.xml,LOM_b5.xml,COOLING_00.xml,CustomizeSensor.xml,PCIeAddrInfo_ff.xml,PSUCustomizeSensor.xml,Riser2_97.xml",
        "_3b5.xml,SPSA_VRD_03.xml,BASEBOARD_00.xml,LOM_b5.xml,COOLING_01.xml,CustomizeSensor.xml,PCIeAddrInfo_ff.xml,PSUCustomizeSensor.xml,Riser2_ff.xml"
		],
        'TaiShan2480v2':[
        "_701.xml,ADC_01.xml,BASEBOARD_01.xml,CHASSIS_INTRUSION_00.xml,COOLING_00.xml,CustomizeSensor.xml,FAN_01.xml,M2_SATA_SSD_00.xml,OCPBoard_6f.xml,PCIeAddrInfo_ff.xml,PRODUCT_00.xml,PSevent_01.xml,PSUCustomizeSensor.xml,Riser1_3d.xml,Riser2_9f.xml,VRD_00_01.xml",
		"_701.xml,ADC_01.xml,BASEBOARD_01.xml,CHASSIS_INTRUSION_00.xml,COOLING_00.xml,CustomizeSensor.xml,FAN_01.xml,M2_SATA_SSD_00.xml,OCPBoard_6f.xml,PCIeAddrInfo_ff.xml,PRODUCT_00.xml,PSevent_01.xml,PSUCustomizeSensor.xml,Riser1_3e.xml,Riser2_38.xml,VRD_00_01.xml",
        "_701.xml,ADC_01.xml,BASEBOARD_01.xml,CHASSIS_INTRUSION_00.xml,COOLING_00.xml,CustomizeSensor.xml,FAN_01.xml,M2_SATA_SSD_00.xml,OCPBoard_6f.xml,PCIeAddrInfo_ff.xml,PRODUCT_00.xml,PSevent_01.xml,PSUCustomizeSensor.xml,Riser1_34.xml,Riser2_33.xml,VRD_00_01.xml",
        "_701.xml,ADC_01.xml,BASEBOARD_01.xml,CHASSIS_INTRUSION_00.xml,COOLING_00.xml,CustomizeSensor.xml,FAN_01.xml,M2_SATA_SSD_00.xml,OCPBoard_6f.xml,PCIeAddrInfo_ff.xml,PRODUCT_00.xml,PSevent_01.xml,PSUCustomizeSensor.xml,Riser1_3e.xml,Riser2_39.xml,VRD_00_01.xml",
        "_701.xml,ADC_01.xml,BASEBOARD_01.xml,CHASSIS_INTRUSION_00.xml,COOLING_00.xml,CustomizeSensor.xml,FAN_01.xml,M2_SATA_SSD_00.xml,OCPBoard_6f.xml,PCIeAddrInfo_ff.xml,PRODUCT_00.xml,PSevent_01.xml,PSUCustomizeSensor.xml,Riser1_3d.xml,Riser2_9f.xml,VRD_02_01.xml",
        "_701.xml,ADC_01.xml,BASEBOARD_01.xml,CHASSIS_INTRUSION_00.xml,COOLING_00.xml,CustomizeSensor.xml,FAN_01.xml,M2_SATA_SSD_00.xml,OCPBoard_6f.xml,PCIeAddrInfo_ff.xml,PRODUCT_00.xml,PSevent_01.xml,PSUCustomizeSensor.xml,Riser1_3e.xml,Riser2_38.xml,VRD_02_01.xml",
        "_701.xml,ADC_01.xml,BASEBOARD_01.xml,CHASSIS_INTRUSION_00.xml,COOLING_00.xml,CustomizeSensor.xml,FAN_01.xml,M2_SATA_SSD_00.xml,OCPBoard_6f.xml,PCIeAddrInfo_ff.xml,PRODUCT_00.xml,PSevent_01.xml,PSUCustomizeSensor.xml,Riser1_34.xml,Riser2_33.xml,VRD_02_01.xml",
        "_701.xml,ADC_01.xml,BASEBOARD_01.xml,CHASSIS_INTRUSION_00.xml,COOLING_00.xml,CustomizeSensor.xml,FAN_01.xml,M2_SATA_SSD_00.xml,OCPBoard_6f.xml,PCIeAddrInfo_ff.xml,PRODUCT_00.xml,PSevent_01.xml,PSUCustomizeSensor.xml,Riser1_3e.xml,Riser2_39.xml,VRD_02_01.xml",
		"_701.xml,ADC_01_01.xml,BASEBOARD_01.xml,CHASSIS_INTRUSION_00.xml,COOLING_00.xml,CustomizeSensor.xml,FAN_01.xml,M2_SATA_SSD_00.xml,OCPBoard_6f.xml,PCIeAddrInfo_ff.xml,PRODUCT_00.xml,PSevent_01.xml,PSUCustomizeSensor.xml,Riser1_3e.xml,Riser2_39.xml,VRD_02_01.xml",
		"_701.xml,ADC_01.xml,BASEBOARD_01.xml,CHASSIS_INTRUSION_00.xml,COOLING_00.xml,CustomizeSensor.xml,FAN_01.xml,M2_SATA_SSD_00.xml,OCPBoard_6f.xml,PCIeAddrInfo_ff.xml,PRODUCT_00_01.xml,PSevent_01.xml,PSUCustomizeSensor.xml,Riser1_34.xml,Riser2_33.xml,VRD_01_01.xml"
        ],
		'TaiShan5290v2':[
        "_704.xml,BASEBOARD_04.xml,PCIE_NCSI_00.xml,ADC_04.xml,VRD_00_04.xml,PCIeAddrInfo_ff.xml,RearBp_4e.xml,FAN_04.xml,PSevent_04.xml,CablePrsntEvent_04.xml,PRODUCT_00.xml,COOLING_00.xml,CLOCK_01.xml",
		"_704.xml,BASEBOARD_04.xml,PCIE_NCSI_00.xml,ADC_04.xml,VRD_01_04.xml,PCIeAddrInfo_ff.xml,RearBp_4e.xml,FAN_04.xml,PSevent_04.xml,CablePrsntEvent_04.xml,PRODUCT_00.xml,COOLING_00.xml,CLOCK_00.xml"
        ],
		'TaiShan2480v2Pro':[
        "_701.xml,ADC_01.xml,BASEBOARD_01.xml,CHASSIS_INTRUSION_00.xml,COOLING_00.xml,CustomizeSensor.xml,FAN_01.xml,M2_SATA_SSD_00.xml,OCPBoard_6f.xml,PCIeAddrInfo_ff.xml,PRODUCT_01.xml,PSevent_01.xml,PSUCustomizeSensor.xml,Riser1_3d.xml,Riser2_9f.xml,VRD_00_01.xml",
		"_701.xml,ADC_01.xml,BASEBOARD_01.xml,CHASSIS_INTRUSION_00.xml,COOLING_00.xml,CustomizeSensor.xml,FAN_01.xml,M2_SATA_SSD_00.xml,OCPBoard_6f.xml,PCIeAddrInfo_ff.xml,PRODUCT_01.xml,PSevent_01.xml,PSUCustomizeSensor.xml,Riser1_3e.xml,Riser2_38.xml,VRD_00_01.xml",
        "_701.xml,ADC_01.xml,BASEBOARD_01.xml,CHASSIS_INTRUSION_00.xml,COOLING_00.xml,CustomizeSensor.xml,FAN_01.xml,M2_SATA_SSD_00.xml,OCPBoard_6f.xml,PCIeAddrInfo_ff.xml,PRODUCT_01.xml,PSevent_01.xml,PSUCustomizeSensor.xml,Riser1_34.xml,Riser2_33.xml,VRD_00_01.xml",
        "_701.xml,ADC_01.xml,BASEBOARD_01.xml,CHASSIS_INTRUSION_00.xml,COOLING_00.xml,CustomizeSensor.xml,FAN_01.xml,M2_SATA_SSD_00.xml,OCPBoard_6f.xml,PCIeAddrInfo_ff.xml,PRODUCT_01.xml,PSevent_01.xml,PSUCustomizeSensor.xml,Riser1_3e.xml,Riser2_39.xml,VRD_00_01.xml",
        "_701.xml,ADC_01.xml,BASEBOARD_01.xml,CHASSIS_INTRUSION_00.xml,COOLING_00.xml,CustomizeSensor.xml,FAN_01.xml,M2_SATA_SSD_00.xml,OCPBoard_6f.xml,PCIeAddrInfo_ff.xml,PRODUCT_01.xml,PSevent_01.xml,PSUCustomizeSensor.xml,Riser1_3d.xml,Riser2_9f.xml,VRD_02_01.xml",
        "_701.xml,ADC_01.xml,BASEBOARD_01.xml,CHASSIS_INTRUSION_00.xml,COOLING_00.xml,CustomizeSensor.xml,FAN_01.xml,M2_SATA_SSD_00.xml,OCPBoard_6f.xml,PCIeAddrInfo_ff.xml,PRODUCT_01.xml,PSevent_01.xml,PSUCustomizeSensor.xml,Riser1_3e.xml,Riser2_38.xml,VRD_02_01.xml",
        "_701.xml,ADC_01.xml,BASEBOARD_01.xml,CHASSIS_INTRUSION_00.xml,COOLING_00.xml,CustomizeSensor.xml,FAN_01.xml,M2_SATA_SSD_00.xml,OCPBoard_6f.xml,PCIeAddrInfo_ff.xml,PRODUCT_01.xml,PSevent_01.xml,PSUCustomizeSensor.xml,Riser1_34.xml,Riser2_33.xml,VRD_02_01.xml",
        "_701.xml,ADC_01.xml,BASEBOARD_01.xml,CHASSIS_INTRUSION_00.xml,COOLING_00.xml,CustomizeSensor.xml,FAN_01.xml,M2_SATA_SSD_00.xml,OCPBoard_6f.xml,PCIeAddrInfo_ff.xml,PRODUCT_01.xml,PSevent_01.xml,PSUCustomizeSensor.xml,Riser1_3e.xml,Riser2_39.xml,VRD_02_01.xml",
		"_701.xml,ADC_01_01.xml,BASEBOARD_01.xml,CHASSIS_INTRUSION_00.xml,COOLING_00.xml,CustomizeSensor.xml,FAN_01.xml,M2_SATA_SSD_00.xml,OCPBoard_6f.xml,PCIeAddrInfo_ff.xml,PRODUCT_01.xml,PSevent_01.xml,PSUCustomizeSensor.xml,Riser1_3e.xml,Riser2_39.xml,VRD_02_01.xml",
		"_701.xml,ADC_01.xml,BASEBOARD_01.xml,CHASSIS_INTRUSION_00.xml,COOLING_00.xml,CustomizeSensor.xml,FAN_01.xml,M2_SATA_SSD_00.xml,OCPBoard_6f.xml,PCIeAddrInfo_ff.xml,PRODUCT_01.xml,PSevent_01.xml,PSUCustomizeSensor.xml,Riser1_34.xml,Riser2_33.xml,VRD_01_01.xml"
        ],
		'TaiShan2280v2Pro':[
		"_ea9.xml,BASEBOARD_a9.xml,ADC_a9.xml,VRD_a9.xml,COOLING_00.xml,FAN_a9.xml,PSevent_a9.xml,CLOCK_00.xml,PCIeAddrInfo_ff.xml,CHASSIS_INTRUSION_00.xml,CustomizeSensor.xml,SPSCA_4U_00.xml,PRODUCT_A9_00.xml,M2_SATA_SSD_00.xml,PSUCustomizeSensor.xml,Raid_3e.xml,RearBp_4e.xml,Riser1_3b.xml,Riser2_3b.xml,Riser3_3c.xml,UPGRADE_CPLD_00.xml",
		"_ea1.xml,BASEBOARD_a1.xml,ADC_a1.xml,VRD_a1_01.xml,COOLING_00.xml,FAN_a9.xml,PSevent_a9.xml,CLOCK_01.xml,PCIeAddrInfo_ff.xml,CHASSIS_INTRUSION_00.xml,CustomizeSensor.xml,SPSCA_4U_00.xml,PRODUCT_A1_00.xml,M2_SATA_SSD_00.xml,PSUCustomizeSensor.xml,Raid_3e.xml,RearBp_47.xml,Riser1_32.xml,Riser2_32.xml,UPGRADE_CPLD_00.xml",
		"_ea1.xml,BASEBOARD_a1.xml,ADC_a1.xml,VRD_a1_21.xml,COOLING_00.xml,FAN_a9.xml,PSevent_a9.xml,CLOCK_01.xml,PCIeAddrInfo_ff.xml,CHASSIS_INTRUSION_00.xml,CustomizeSensor.xml,SPSCA_4U_00.xml,PRODUCT_A1_00.xml,M2_SATA_SSD_00.xml,PSUCustomizeSensor.xml,Raid_3e.xml,RearBp_4e.xml,Riser1_3d.xml,Riser2_3d.xml,Riser3_37.xml,UPGRADE_CPLD_00.xml",
        "_ea1.xml,BASEBOARD_a1.xml,ADC_a1.xml,VRD_a1_25.xml,COOLING_00.xml,FAN_a9.xml,PSevent_a9.xml,CLOCK_01.xml,PCIeAddrInfo_ff.xml,CHASSIS_INTRUSION_00.xml,CustomizeSensor.xml,SPSCA_4U_00.xml,PRODUCT_A1_00.xml,M2_SATA_SSD_00.xml,PSUCustomizeSensor.xml,Raid_3e.xml,RearBp_4e.xml,Riser1_3d.xml,Riser2_3d.xml,Riser3_37.xml,UPGRADE_CPLD_00.xml"
		],
        'Atlas880':["_292.xml,ADC_92.xml,BASEBOARD_92.xml,CHASSIS_INTRUSION_00.xml,COOLING_00.xml," + 
                    "CustomizeSensor.xml,FAN_92.xml,HddPcieAddr_00.xml,M2_SATA_SSD_00.xml,PCIeAddrInfo_ff.xml," +
                    "PCIeLomCard_fe.xml,PRODUCT_00.xml,PSevent_92.xml,PSUCustomizeSensor.xml," +
                    "Riser1_3d.xml,Riser1_3e.xml,VRD_00_92.xml",
                    "_292.xml,ADC_92.xml,BASEBOARD_92.xml,CHASSIS_INTRUSION_00.xml,COOLING_01.xml," + 
                    "CustomizeSensor.xml,FAN_92.xml,HddPcieAddr_01.xml,M2_SATA_SSD_00.xml,PCIeAddrInfo_ff.xml," +
                    "PCIeLomCard_ff.xml,PRODUCT_00.xml,PSevent_92.xml,PSUCustomizeSensor.xml," +
                    "Riser1_3d.xml,Riser1_3e.xml,VRD_00_92.xml"],
        'CM221':[
        "_630.xml,BASEBOARD_ff.xml,VRD_30.xml,CLOCK_30.xml,ADC_30.xml,COOLING_POLICY_30.xml,PSevent_30.xml,PCIeAddrInfo_01.xml,LOMNIC1_30.xml,LOMNIC2_30.xml,Hdd_ff_01.xml,Riser2_a1.xml,NPCARD_ff.xml,CustomizeSensor.xml",
        "_630.xml,BASEBOARD_ff.xml,VRD_30.xml,CLOCK_30_01.xml,ADC_30.xml,COOLING_POLICY_30.xml,PSevent_30.xml,PCIeAddrInfo_01.xml,LOMNIC1_30.xml,LOMNIC2_30.xml,Hdd_ff_01.xml,Riser2_a2.xml,Riser3_a1.xml,NPCARD_ff.xml,CustomizeSensor.xml",
        "_631.xml,BASEBOARD_ff.xml,VRD_31.xml,CLOCK_31.xml,ADC_31.xml,COOLING_POLICY_31.xml,PSevent_31.xml,PCIeAddrInfo_01.xml,LOMNIC1_31.xml,LOMNIC2_31.xml,Hdd_ff_01.xml,Riser2_a3.xml,Riser3_a2.xml,NPCARD_ff.xml,CustomizeSensor.xml",
        "_631.xml,BASEBOARD_ff.xml,VRD_31.xml,CLOCK_31_01.xml,ADC_31.xml,COOLING_POLICY_31.xml,PSevent_31.xml,PCIeAddrInfo_01.xml,LOMNIC1_31.xml,LOMNIC2_31.xml,Hdd_ff_01.xml,Riser2_36.xml,Riser3_a3.xml,NPCARD_ff.xml,CustomizeSensor.xml",
        "_630.xml,BASEBOARD_ff.xml,VRD_30_01.xml,CLOCK_30.xml,ADC_30.xml,COOLING_POLICY_30.xml,PSevent_30.xml,PCIeAddrInfo_01.xml,LOMNIC1_30.xml,LOMNIC2_30.xml,Hdd_ff_01.xml,Riser2_36.xml,Riser3_a3.xml,NPCARD_ff.xml,CustomizeSensor.xml",
        "_630.xml,BASEBOARD_ff.xml,VRD_30_01.xml,CLOCK_30_01.xml,ADC_30.xml,COOLING_POLICY_30.xml,PSevent_30.xml,PCIeAddrInfo_01.xml,LOMNIC1_30.xml,LOMNIC2_30.xml,Hdd_ff_01.xml,Riser2_a3.xml,Riser3_a2.xml,NPCARD_ff.xml,CustomizeSensor.xml",
        "_631.xml,BASEBOARD_ff.xml,VRD_31_01.xml,CLOCK_31.xml,ADC_31.xml,COOLING_POLICY_31.xml,PSevent_31.xml,PCIeAddrInfo_01.xml,LOMNIC1_31.xml,LOMNIC2_31.xml,Hdd_ff_01.xml,Riser2_a2.xml,Riser3_a1.xml,NPCARD_ff.xml,CustomizeSensor.xml",
        "_631.xml,BASEBOARD_ff.xml,VRD_31_01.xml,CLOCK_31_01.xml,ADC_31.xml,COOLING_POLICY_31.xml,PSevent_31.xml,PCIeAddrInfo_01.xml,LOMNIC1_31.xml,LOMNIC2_31.xml,Hdd_ff_01.xml,Riser2_a1.xml,NPCARD_ff.xml,CustomizeSensor.xml"
        ],
        'CM221S': [
            "_632.xml,PRODUCT_00.xml,BASEBOARD_32.xml,Riser_32.xml,PsuFanBACKPLANE_32.xml,VRD_32.xml,CLOCK_00.xml,ADC_32.xml,COOLING_POLICY_01.xml,PSevent_32.xml,Fantray_ff.xml,PCIeAddrInfo_01.xml,LOMNIC1_32.xml,LOMNIC2_32.xml,SYNC_ff.xml,Hdd_32_01.xml,VSMMFantray_ff.xml,CustomizeSensor.xml,LSW_632.xml",
            "_632.xml,PRODUCT_00.xml,BASEBOARD_32.xml,Riser_32.xml,PsuFanBACKPLANE_32_01.xml,VRD_32.xml,CLOCK_00.xml,ADC_32.xml,COOLING_POLICY_01.xml,PSevent_32.xml,Fantray_ff.xml,PCIeAddrInfo_01.xml,LOMNIC1_32.xml,LOMNIC2_32.xml,SYNC_ff.xml,Hdd_32_01.xml,VSMMFantray_ff.xml,CustomizeSensor.xml,LSW_632.xml",
            "_632.xml,PRODUCT_00.xml,BASEBOARD_32.xml,Riser_32.xml,PsuFanBACKPLANE_32_02.xml,VRD_32.xml,CLOCK_00.xml,ADC_32.xml,COOLING_POLICY_01.xml,PSevent_32.xml,Fantray_ff.xml,PCIeAddrInfo_01.xml,LOMNIC1_32.xml,LOMNIC2_32.xml,SYNC_ff.xml,Hdd_32_01.xml,VSMMFantray_ff.xml,CustomizeSensor.xml,LSW_632.xml",
            "_632.xml,PRODUCT_00.xml,BASEBOARD_32.xml,Riser_32.xml,PsuFanBACKPLANE_32.xml,VRD_32.xml,CLOCK_01.xml,ADC_32.xml,COOLING_POLICY_01.xml,PSevent_32.xml,Fantray_ff.xml,PCIeAddrInfo_01.xml,LOMNIC1_32.xml,LOMNIC2_32.xml,SYNC_ff.xml,Hdd_32_01.xml,VSMMFantray_ff.xml,CustomizeSensor.xml,LSW_632.xml",
            "_632.xml,PRODUCT_00.xml,BASEBOARD_32.xml,Riser_32.xml,PsuFanBACKPLANE_32_01.xml,VRD_32.xml,CLOCK_01.xml,ADC_32.xml,COOLING_POLICY_01.xml,PSevent_32.xml,Fantray_ff.xml,PCIeAddrInfo_01.xml,LOMNIC1_32.xml,LOMNIC2_32.xml,SYNC_ff.xml,Hdd_32_01.xml,VSMMFantray_ff.xml,CustomizeSensor.xml,LSW_632.xml",
            "_632.xml,PRODUCT_00.xml,BASEBOARD_32.xml,Riser_32.xml,PsuFanBACKPLANE_32_02.xml,VRD_32.xml,CLOCK_01.xml,ADC_32.xml,COOLING_POLICY_01.xml,PSevent_32.xml,Fantray_ff.xml,PCIeAddrInfo_01.xml,LOMNIC1_32.xml,LOMNIC2_32.xml,SYNC_ff.xml,Hdd_32_01.xml,VSMMFantray_ff.xml,CustomizeSensor.xml,LSW_632.xml",
        ],
        'CN221': [
            "_651.xml,PRODUCT_00.xml,BASEBOARD_ff.xml,Riser_ff.xml,VRD_51.xml,CLOCK_00.xml,ADC_ff.xml,COOLING_POLICY_51.xml,PSevent_ff.xml,PCIeAddrInfo_01.xml,Riser1_PcieAddrInfo_a8.xml,Riser2_PcieAddrInfo_a8.xml,Riser4_PcieAddrInfo_a1.xml,Hdd_ff_01.xml,LOMNIC_ff.xml,NPCARD_ff.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml",
            "_651.xml,PRODUCT_00.xml,BASEBOARD_ff.xml,Riser_ff.xml,VRD_51.xml,CLOCK_01.xml,ADC_ff.xml,COOLING_POLICY_51.xml,PSevent_ff.xml,PCIeAddrInfo_01.xml,Riser1_PcieAddrInfo_a8.xml,Riser2_PcieAddrInfo_a8.xml,Riser4_PcieAddrInfo_a1.xml,Hdd_ff_01.xml,LOMNIC_ff.xml,NPCARD_ff.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml",
            "_651.xml,PRODUCT_00.xml,BASEBOARD_ff.xml,Riser_ff.xml,VRD_51_01.xml,CLOCK_00.xml,ADC_ff.xml,COOLING_POLICY_51.xml,PSevent_ff.xml,PCIeAddrInfo_01.xml,Riser1_PcieAddrInfo_a8.xml,Riser2_PcieAddrInfo_a8.xml,Riser4_PcieAddrInfo_a1.xml,Hdd_ff_01.xml,LOMNIC_ff.xml,NPCARD_ff.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml",
            "_651.xml,PRODUCT_00.xml,BASEBOARD_ff.xml,Riser_ff.xml,VRD_51_01.xml,CLOCK_01.xml,ADC_ff.xml,COOLING_POLICY_51.xml,PSevent_ff.xml,PCIeAddrInfo_01.xml,Riser1_PcieAddrInfo_a8.xml,Riser2_PcieAddrInfo_a8.xml,Riser4_PcieAddrInfo_a1.xml,Hdd_ff_01.xml,LOMNIC_ff.xml,NPCARD_ff.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml",
            "_652.xml,PRODUCT_00.xml,BASEBOARD_ff.xml,Riser_ff.xml,VRD_52.xml,CLOCK_00.xml,ADC_ff.xml,COOLING_POLICY_52.xml,PSevent_ff.xml,PCIeAddrInfo_01.xml,Riser1_PcieAddrInfo_a8.xml,Riser2_PcieAddrInfo_a8.xml,Riser4_PcieAddrInfo_a1.xml,Hdd_ff_01.xml,LOMNIC_ff.xml,NPCARD_ff.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml",
            "_652.xml,PRODUCT_00.xml,BASEBOARD_ff.xml,Riser_ff.xml,VRD_52.xml,CLOCK_01.xml,ADC_ff.xml,COOLING_POLICY_52.xml,PSevent_ff.xml,PCIeAddrInfo_01.xml,Riser1_PcieAddrInfo_a8.xml,Riser2_PcieAddrInfo_a8.xml,Riser4_PcieAddrInfo_a1.xml,Hdd_ff_01.xml,LOMNIC_ff.xml,NPCARD_ff.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml",
            "_652.xml,PRODUCT_00.xml,BASEBOARD_ff.xml,Riser_ff.xml,VRD_52_01.xml,CLOCK_00.xml,ADC_ff.xml,COOLING_POLICY_52.xml,PSevent_ff.xml,PCIeAddrInfo_01.xml,Riser1_PcieAddrInfo_a8.xml,Riser2_PcieAddrInfo_a8.xml,Riser4_PcieAddrInfo_a1.xml,Hdd_ff_01.xml,LOMNIC_ff.xml,NPCARD_ff.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml",
            "_652.xml,PRODUCT_00.xml,BASEBOARD_ff.xml,Riser_ff.xml,VRD_52_01.xml,CLOCK_01.xml,ADC_ff.xml,COOLING_POLICY_52.xml,PSevent_ff.xml,PCIeAddrInfo_01.xml,Riser1_PcieAddrInfo_a8.xml,Riser2_PcieAddrInfo_a8.xml,Riser4_PcieAddrInfo_a1.xml,Hdd_ff_01.xml,LOMNIC_ff.xml,NPCARD_ff.xml,CustomizeSensor.xml,UPGRADE_REVISION_00.xml",
        ],
		'2298v5':["_1b6.xml,MTSA_01.xml,MTSA_VRD_00.xml,CPUPKG_01.xml,COOLING_00.xml,CustomizeSensor.xml,SPSF_NCSI_00.xml",
                  "_109.xml,MTSA_00.xml,MTSA_VRD_00.xml,CPUPKG_01.xml,COOLING_00.xml,CustomizeSensor.xml,SPSF_NCSI_01.xml"],
        '2288xv5_5288xv5':["_1bf.xml,PRODUCT_bf_2e.xml,AEP_01.xml,AEP_02.xml,SPSF_COOLING_00.xml,SPSF_NCSI_00.xml,SPSF_VRD_00.xml,CHASSIS_INTRUSION_00.xml,CustomizeSensor.xml,PSUCustomizeSensor.xml,UPGRADE_REVISION_00.xml",
				   "_1bf.xml,PRODUCT_bf_3e.xml,AEP_01.xml,AEP_02.xml,SPSF_COOLING_01.xml,SPSF_NCSI_01.xml,SPSF_VRD_01.xml,CHASSIS_INTRUSION_00.xml,CustomizeSensor.xml,PSUCustomizeSensor.xml,UPGRADE_REVISION_00.xml",
                   "_1bf.xml,PRODUCT_bf_3e.xml,AEP_01.xml,AEP_02.xml,SPSF_COOLING_01.xml,SPSF_NCSI_01.xml,SPSF_VRD_02.xml,CHASSIS_INTRUSION_00.xml,CustomizeSensor.xml,PSUCustomizeSensor.xml,UPGRADE_REVISION_00.xml",
				   "_1bf.xml,PRODUCT_bf_ff.xml,AEP_01.xml,AEP_02.xml,SPSF_COOLING_00.xml,SPSF_NCSI_01.xml,SPSF_VRD_03.xml,CHASSIS_INTRUSION_00.xml,CustomizeSensor.xml,PSUCustomizeSensor.xml,UPGRADE_REVISION_00.xml"],
        '2288xv5_1711':["_1b0.xml,PRODUCT_b0_2e.xml,AEP_01.xml,AEP_02.xml,SPSF_COOLING_00.xml,SPSF_NCSI_00.xml,SPSF_VRD_00.xml,CHASSIS_INTRUSION_00.xml,CustomizeSensor.xml,PSUCustomizeSensor.xml",
				   "_1b0.xml,PRODUCT_b0_3e.xml,AEP_01.xml,AEP_02.xml,SPSF_COOLING_01.xml,SPSF_NCSI_01.xml,SPSF_VRD_01.xml,CHASSIS_INTRUSION_00.xml,CustomizeSensor.xml,PSUCustomizeSensor.xml",
                   "_1b0.xml,PRODUCT_b0_3e.xml,AEP_01.xml,AEP_02.xml,SPSF_COOLING_01.xml,SPSF_NCSI_01.xml,SPSF_VRD_00.xml,CHASSIS_INTRUSION_00.xml,CustomizeSensor.xml,PSUCustomizeSensor.xml",
				   "_1b0.xml,PRODUCT_b0_ff.xml,AEP_01.xml,AEP_02.xml,SPSF_COOLING_00.xml,SPSF_NCSI_01.xml,SPSF_VRD_01.xml,CHASSIS_INTRUSION_00.xml,CustomizeSensor.xml,PSUCustomizeSensor.xml",
                   "_1b4.xml,PRODUCT_b4_2e.xml,AEP_01.xml,AEP_02.xml,SPSF_COOLING_00.xml,SPSF_NCSI_00.xml,SPSF_VRD_00.xml,CHASSIS_INTRUSION_00.xml,CustomizeSensor.xml,PSUCustomizeSensor.xml",
				   "_1b4.xml,PRODUCT_b4_ff.xml,AEP_01.xml,AEP_02.xml,SPSF_COOLING_01.xml,SPSF_NCSI_01.xml,SPSF_VRD_01.xml,CHASSIS_INTRUSION_00.xml,CustomizeSensor.xml,PSUCustomizeSensor.xml",
                   "_1b4.xml,PRODUCT_b4_2e.xml,AEP_01.xml,AEP_02.xml,SPSF_COOLING_01.xml,SPSF_NCSI_01.xml,SPSF_VRD_00.xml,CHASSIS_INTRUSION_00.xml,CustomizeSensor.xml,PSUCustomizeSensor.xml",
				   "_1b4.xml,PRODUCT_b4_ff.xml,AEP_01.xml,AEP_02.xml,SPSF_COOLING_00.xml,SPSF_NCSI_01.xml,SPSF_VRD_01.xml,CHASSIS_INTRUSION_00.xml,CustomizeSensor.xml,PSUCustomizeSensor.xml"],
        '1288xv5':["_1bc.xml,AEP_01.xml,AEP_02.xml,SPSF_NCSI_00.xml,SPSF_VRD_00.xml,CHASSIS_INTRUSION_00.xml,CustomizeSensor.xml,PSUCustomizeSensor.xml,UPGRADE_REVISION_00.xml",
                   "_1bc.xml,AEP_01.xml,AEP_02.xml,SPSF_NCSI_01.xml,SPSF_VRD_02.xml,CHASSIS_INTRUSION_00.xml,CustomizeSensor.xml,PSUCustomizeSensor.xml,UPGRADE_REVISION_00.xml"],
		'TaiShan2280Ev2':[
		"_705.xml,BASEBOARD_05.xml,ADC_05.xml,VRD_00_05.xml,COOLING_00.xml,FAN_05.xml,PSevent_05.xml,CLOCK_00.xml,PCIeAddrInfo_ff.xml,CHASSIS_INTRUSION_00.xml,CustomizeSensor.xml,PCIE_NCSI_00.xml"
		],
		'TaiShan2280v2_1711':[
		"_eb9.xml,BASEBOARD_b9.xml,ADC_b9.xml,VRD_b9.xml,COOLING_00.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_00.xml,PCIeAddrInfo_ff.xml,CHASSIS_INTRUSION_00.xml,CustomizeSensor.xml,SPSCA_4U_00.xml,PRODUCT_00.xml,M2_SATA_SSD_00.xml,PSUCustomizeSensor.xml,Raid_3e.xml,RearBp_4e.xml,Riser1_3b.xml,Riser2_3b.xml,Riser3_3c.xml,UPGRADE_CPLD_00.xml",
		"_eb9.xml,BASEBOARD_b9.xml,ADC_b9.xml,VRD_b9.xml,COOLING_01.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_01.xml,PCIeAddrInfo_ff.xml,CHASSIS_INTRUSION_00.xml,CustomizeSensor.xml,SPSCA_4U_00.xml,PRODUCT_01.xml,M2_SATA_SSD_00.xml,PSUCustomizeSensor.xml,Raid_3e.xml,RearBp_4e.xml,Riser1_32.xml,Riser2_32.xml,UPGRADE_CPLD_01.xml",
		"_eb9.xml,BASEBOARD_b9.xml,ADC_b9.xml,VRD_b9.xml,COOLING_01.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_01.xml,PCIeAddrInfo_ff.xml,CHASSIS_INTRUSION_00.xml,CustomizeSensor.xml,SPSCA_4U_00.xml,PRODUCT_01.xml,M2_SATA_SSD_00.xml,PSUCustomizeSensor.xml,Raid_3e.xml,RearBp_4e.xml,Riser1_90.xml,Riser2_90.xml,Riser3_37.xml,UPGRADE_CPLD_01.xml",
		"_ffb9.xml,BASEBOARD_b9.xml,ADC_b9.xml,VRD_b9.xml,COOLING_01.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_01.xml,PCIeAddrInfo_ff.xml,CHASSIS_INTRUSION_00.xml,CustomizeSensor.xml,SPSCA_4U_00.xml,PRODUCT_01.xml,M2_SATA_SSD_00.xml,PSUCustomizeSensor.xml,Raid_3e.xml,RearBp_4e.xml,Riser1_94.xml,Riser2_94.xml,UPGRADE_CPLD_01.xml",
		"_ffb9.xml,BASEBOARD_b9.xml,ADC_b9.xml,VRD_b9.xml,COOLING_01.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_01.xml,PCIeAddrInfo_ff.xml,CHASSIS_INTRUSION_00.xml,CustomizeSensor.xml,SPSCA_4U_00.xml,PRODUCT_01.xml,M2_SATA_SSD_00.xml,PSUCustomizeSensor.xml,Raid_3e.xml,RearBp_4e.xml,Riser1_95.xml,Riser2_95.xml,UPGRADE_CPLD_01.xml",
		"_ffb9.xml,BASEBOARD_b9.xml,ADC_b9.xml,VRD_b9.xml,COOLING_00.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_00.xml,PCIeAddrInfo_ff.xml,CHASSIS_INTRUSION_00.xml,CustomizeSensor.xml,SPSCA_4U_00.xml,PRODUCT_00.xml,M2_SATA_SSD_00.xml,PSUCustomizeSensor.xml,Raid_3e.xml,RearBp_4e.xml,Riser1_99.xml,Riser2_99.xml,UPGRADE_CPLD_00.xml",
		"_ea1.xml,BASEBOARD_a1.xml,ADC_a1.xml,VRD_a1_21.xml,COOLING_01.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_01.xml,PCIeAddrInfo_ff.xml,CHASSIS_INTRUSION_00.xml,CustomizeSensor.xml,SPSCA_4U_00.xml,PRODUCT_A1_00.xml,M2_SATA_SSD_00.xml,RAIDSASCable_00.xml",
		"_ea1.xml,BASEBOARD_a1.xml,ADC_a1.xml,VRD_a1_01.xml,COOLING_01.xml,FAN_b9.xml,PSevent_b9.xml,CLOCK_01.xml,PCIeAddrInfo_ff.xml,CHASSIS_INTRUSION_00.xml,CustomizeSensor.xml,SPSCA_4U_00.xml,PRODUCT_A1_01.xml,M2_SATA_SSD_00.xml,RAIDSASCable_01.xml"
		],
        'BM320':[
		"_ff.xml,BASEBOARD_ff.xml,COOLING_00.xml,MPC_00.xml,CustomizeSensor.xml,PRODUCT_FF_00.xml,M2_SATA_SSD_00.xml,Eth_a1.xml,InletOutlet_a1.xml,Serial_a1.xml"
		],
        'S920S03':[
        "_708.xml,ADC_08.xml,BASEBOARD_08.xml,CHASSIS_INTRUSION_00.xml,CLOCK_00.xml,COOLING_00.xml,CustomizeSensor.xml,FAN_08.xml,PCIE_NCSI_00.xml,PCIeAddrInfo_ff.xml,PRODUCT_00.xml,PSevent_08.xml,PSUIDENTIFY_00.xml,RISER_00.xml,VRD_00_08.xml,RISER2CONNECTOR_02.xml",
        "_708.xml,ADC_08.xml,BASEBOARD_08.xml,CHASSIS_INTRUSION_00.xml,CLOCK_00.xml,COOLING_00.xml,CustomizeSensor.xml,FAN_08.xml,PCIE_NCSI_00.xml,PCIeAddrInfo_ff.xml,PRODUCT_00.xml,PSevent_08.xml,PSUIDENTIFY_00.xml,RISER_00.xml,VRD_00_08.xml,RISER2CONNECTOR_01.xml",
        "_708.xml,ADC_08.xml,BASEBOARD_08.xml,CHASSIS_INTRUSION_00.xml,CLOCK_01.xml,COOLING_01.xml,CustomizeSensor.xml,FAN_08.xml,PCIE_NCSI_00.xml,PCIeAddrInfo_ff.xml,PRODUCT_00.xml,PSevent_08.xml,PSUIDENTIFY_00.xml,RISER_00.xml,VRD_01_08.xml,RISER2CONNECTOR_02.xml"
       ],
        'S920X05':[
        "_af1.xml,ADC_f1.xml,BASEBOARD_f1.xml,CHASSIS_INTRUSION_00.xml,BC13MBSA_OCP_NCSI_00.xml,CLOCK_00.xml,COOLING_00.xml,CustomizeSensor.xml,FAN_f1.xml,M2_SATA_SSD_00.xml,PCIeAddrInfo_ff.xml,PRODUCT_00.xml,PSevent_f1.xml,PSUCustomizeSensor.xml,Raid_3e.xml,Riser1_3b.xml,Riser2_3b.xml,SPSCA_4U_00.xml,UPGRADE_CPLD_00.xml,VRD_f1.xml",
        "_af1.xml,ADC_f1.xml,BASEBOARD_f1.xml,CHASSIS_INTRUSION_00.xml,BC13MBSA_OCP_NCSI_00.xml,CLOCK_01.xml,COOLING_00.xml,CustomizeSensor.xml,FAN_f1.xml,M2_SATA_SSD_00.xml,PCIeAddrInfo_ff.xml,PRODUCT_00_40.xml,PSevent_f1.xml,PSUCustomizeSensor.xml,Raid_3e.xml,Riser1_32.xml,Riser2_32.xml,SPSCA_4U_00.xml,UPGRADE_CPLD_01.xml,VRD_f1.xml",
        "_af1.xml,ADC_f1.xml,BASEBOARD_f1.xml,CHASSIS_INTRUSION_00.xml,BC13MBSA_OCP_NCSI_00.xml,CLOCK_01.xml,COOLING_00.xml,CustomizeSensor.xml,FAN_f1.xml,M2_SATA_SSD_00.xml,PCIeAddrInfo_ff.xml,PRODUCT_00_ff.xml,PSevent_f1.xml,PSUCustomizeSensor.xml,Raid_3e.xml,Riser1_90.xml,Riser2_90.xml,SPSCA_4U_00.xml,UPGRADE_CPLD_00.xml,VRD_f1.xml",
        "_af1.xml,ADC_f1.xml,BASEBOARD_f1.xml,CHASSIS_INTRUSION_00.xml,BC13MBSA_OCP_NCSI_00.xml,CLOCK_01.xml,COOLING_00.xml,CustomizeSensor.xml,FAN_f1.xml,M2_SATA_SSD_00.xml,PCIeAddrInfo_ff.xml,PRODUCT_00.xml,PSevent_f1.xml,PSUCustomizeSensor.xml,Raid_3e.xml,Riser1_94.xml,Riser2_94.xml,Riser3_37.xml,SPSCA_4U_00.xml,UPGRADE_CPLD_01.xml,VRD_f1.xml",
        "_af1.xml,ADC_f1.xml,BASEBOARD_f1.xml,CHASSIS_INTRUSION_00.xml,BC13MBSA_OCP_NCSI_00.xml,CLOCK_01.xml,COOLING_00.xml,CustomizeSensor.xml,FAN_f1.xml,M2_SATA_SSD_00.xml,PCIeAddrInfo_ff.xml,PRODUCT_00_40.xml,PSevent_f1.xml,PSUCustomizeSensor.xml,Raid_3e.xml,Riser1_95.xml,Riser2_95.xml,SPSCA_4U_00.xml,UPGRADE_CPLD_00.xml,VRD_f1.xml",
        "_af1.xml,ADC_f1.xml,BASEBOARD_f1.xml,CHASSIS_INTRUSION_00.xml,BC13MBSA_OCP_NCSI_00.xml,CLOCK_01.xml,COOLING_00.xml,CustomizeSensor.xml,FAN_f1.xml,M2_SATA_SSD_00.xml,PCIeAddrInfo_ff.xml,PRODUCT_00_ff.xml,PSevent_f1.xml,PSUCustomizeSensor.xml,Raid_3e.xml,Riser1_99.xml,Riser2_99.xml,Riser3_3c.xml,SPSCA_4U_00.xml,UPGRADE_CPLD_01.xml,VRD_f1.xml"
        ],
        'bmc_card/2288hv6':[
        "_ffb5.xml,CustomizeSensor.xml,BC13MBSA_COOLING_00.xml,BC13MBSA_VRD_00.xml,BC13MBSA_FAN_00.xml",
        "_ffb5.xml,CustomizeSensor.xml,BC13MBSA_COOLING_00.xml,BC13MBSA_VRD_00.xml,BC13MBSA_FAN_01.xml"],
        '2488hv6':[
        "_804.xml,BC13MBHA_OCP_NCSI_01.xml,UPGRADE_REVISION_00.xml,VRD_Firmware_01.xml,BC13MBHA_VRD_00.xml,BPS_CPU1_01.xml,BPS_CPU2_02.xml,BPS_CPU3_03.xml,BPS_CPU4_04.xml,COOLING_00.xml,CustomizeSensor.xml,DISKBP_4b.xml,FAN_04.xml,M2_SATA_SSD_00.xml,PCIeAddrInfo_ff.xml,Riser1_3f.xml,Riser1_38.xml,Riser1_39.xml,Riser2_3f.xml,Riser2_38.xml,Riser2_39.xml,SLIM_Cable_4b.xml",
        "_804.xml,BC13MBHA_OCP_NCSI_01.xml,UPGRADE_REVISION_00.xml,BC13MBHA_VRD_01.xml,BPS_CPU1_01.xml,BPS_CPU2_02.xml,BPS_CPU3_03.xml,BPS_CPU4_04.xml,COOLING_00.xml,CustomizeSensor.xml,DISKBP_45.xml,FAN_04.xml,M2_SATA_SSD_00.xml,PCIeAddrInfo_ff.xml,Riser1_3f.xml,Riser1_38.xml,Riser1_39.xml,Riser2_3f.xml,Riser2_38.xml,Riser2_39.xml,SLIM_Cable_4b.xml",
        "_804.xml,BC13MBHA_OCP_NCSI_01.xml,UPGRADE_REVISION_00.xml,BC13MBHA_VRD_02.xml,BPS_CPU1_01.xml,BPS_CPU2_02.xml,BPS_CPU3_03.xml,BPS_CPU4_04.xml,COOLING_00.xml,CustomizeSensor.xml,DISKBP_4b.xml,FAN_04.xml,M2_SATA_SSD_00.xml,PCIeAddrInfo_ff.xml,Riser1_3f.xml,Riser1_38.xml,Riser1_39.xml,Riser2_3f.xml,Riser2_38.xml,Riser2_39.xml,SLIM_Cable_45.xml",
        "_804.xml,BC13MBHA_OCP_NCSI_01.xml,UPGRADE_REVISION_00.xml,VRD_Firmware_02.xml,BC13MBHA_VRD_00.xml,BPS_CPU1_01.xml,BPS_CPU2_02.xml,BPS_CPU3_03.xml,BPS_CPU4_04.xml,COOLING_01.xml,CustomizeSensor.xml,DISKBP_45.xml,FAN_04.xml,M2_SATA_SSD_00.xml,PCIeAddrInfo_ff.xml,Riser1_3f.xml,Riser1_38.xml,Riser1_39.xml,Riser2_3f.xml,Riser2_38.xml,Riser2_39.xml,SLIM_Cable_45.xml"
        ],
        'Atlas800_9010':[
        "_252.xml,ADC_52.xml,BASEBOARD_52.xml,DBoardSLIMSlot1_93.xml,CHASSIS_INTRUSION_00.xml,COOLING_00.xml," + 
        "CustomizeSensor.xml,FAN_52.xml,M2_SATA_SSD_00.xml,PCIeAddrInfo_ff.xml," +
        "PRODUCT_00.xml,PSevent_52.xml,PSUCustomizeSensor.xml," +
        "VRD_52_01.xml",
        "_252.xml,ADC_52.xml,BASEBOARD_52.xml,DBoardSLIMSlot2_93.xml,CHASSIS_INTRUSION_00.xml,COOLING_00.xml," + 
        "CustomizeSensor.xml,FAN_52.xml,M2_SATA_SSD_00.xml,PCIeAddrInfo_ff.xml," +
        "PRODUCT_00.xml,PSevent_52.xml,PSUCustomizeSensor.xml," +
        "VRD_52_02.xml"
        ],
        'Atlas900_PoD_A2':[
        "_29c.xml,ADC_00.xml,BASEBOARD_00.xml,CHASSIS_INTRUSION_00.xml,COOLING_01.xml," + 
        "CustomizeSensor.xml,FAN_00.xml,M2_SATA_SSD_00.xml,PCIeAddrInfo_ff.xml," +
        "PCIeLomCard_fe.xml,PRODUCT_00.xml,PSevent_00.xml,PSUCustomizeSensor.xml," +
        "Riser1_3d.xml,Riser1_3e.xml,Riser1_3f.xml",
        "_29f.xml,ADC_00.xml,BASEBOARD_00.xml,CHASSIS_INTRUSION_00.xml,COOLING_01.xml," + 
        "CustomizeSensor.xml,FAN_00.xml,M2_SATA_SSD_00.xml,PCIeAddrInfo_ff.xml," +
        "PCIeLomCard_fe.xml,PRODUCT_00.xml,PSevent_00.xml,PSUCustomizeSensor.xml," +
        "Riser1_3d.xml,Riser1_3e.xml,Riser1_3f.xml,VRD_00.xml"
		],
        'Atlas500_3000':[
        "_294.xml,CLOCK_00.xml,PRODUCT_00.xml,ADC_94.xml,CHASSIS_INTRUSION_00.xml,CustomizeSensor.xml,FAN_94.xml,PCIeAddrInfo_ff.xml,COOLING_00.xml,BASEBOARD_94.xml,PCIE_NCSI_00.xml,PSevent_94.xml,VRD_00_94.xml,UPGRADE_REVISION_00.xml,PSUCustomizeSensor.xml" ,
        "_294.xml,CLOCK_00.xml,PRODUCT_01.xml,ADC_94.xml,CHASSIS_INTRUSION_00.xml,CustomizeSensor.xml,FAN_94.xml,PCIeAddrInfo_ff.xml,COOLING_00.xml,BASEBOARD_94.xml,PCIE_NCSI_00.xml,PSevent_94.xml,VRD_00_94.xml,UPGRADE_REVISION_00.xml,PSUCustomizeSensor.xml" ,
		"_294.xml,CLOCK_01.xml,PRODUCT_00.xml,ADC_94.xml,CHASSIS_INTRUSION_00.xml,CustomizeSensor.xml,FAN_94.xml,PCIeAddrInfo_ff.xml,COOLING_00.xml,BASEBOARD_94.xml,PCIE_NCSI_00.xml,PSevent_94.xml,VRD_00_94.xml,UPGRADE_REVISION_00.xml,PSUCustomizeSensor.xml" ,
		"_294.xml,CLOCK_01.xml,PRODUCT_01.xml,ADC_94.xml,CHASSIS_INTRUSION_00.xml,CustomizeSensor.xml,FAN_94.xml,PCIeAddrInfo_ff.xml,COOLING_00.xml,BASEBOARD_94.xml,PCIE_NCSI_00.xml,PSevent_94.xml,VRD_00_94.xml,UPGRADE_REVISION_00.xml,PSUCustomizeSensor.xml" ,
        "_294.xml,CLOCK_00.xml,PRODUCT_02.xml,ADC_94.xml,CHASSIS_INTRUSION_00.xml,CustomizeSensor.xml,FAN_94.xml,PCIeAddrInfo_ff.xml,COOLING_00.xml,BASEBOARD_94.xml,PCIE_NCSI_00.xml,PSevent_94.xml,VRD_00_94.xml,UPGRADE_REVISION_00.xml,PSUCustomizeSensor.xml" ,
		"_294.xml,CLOCK_01.xml,PRODUCT_02.xml,ADC_94.xml,CHASSIS_INTRUSION_00.xml,CustomizeSensor.xml,FAN_94.xml,PCIeAddrInfo_ff.xml,COOLING_00.xml,BASEBOARD_94.xml,PCIE_NCSI_00.xml,PSevent_94.xml,VRD_00_94.xml,UPGRADE_REVISION_00.xml,PSUCustomizeSensor.xml" ,
        "_294.xml,CLOCK_01.xml,PRODUCT_02.xml,ADC_94.xml,CHASSIS_INTRUSION_00.xml,CustomizeSensor.xml,FAN_94.xml,PCIeAddrInfo_ff.xml,COOLING_00.xml,BASEBOARD_94.xml,PCIE_NCSI_00.xml,PSevent_94.xml,VRD_00_03.xml,UPGRADE_REVISION_00.xml,PSUCustomizeSensor.xml"  
        ],
        '2288HV6-16DIMM':[
        "_1b8.xml,BC13MBSC_COOLING_00.xml,BC13MBSC_FAN_00.xml,BC13MBSC_OCP_NCSI_01.xml,BC13MBSC_VRD_00.xml,CustomizeSensor.xml,PcieCardCooling_00.xml"
        ],
        'xh321v6':[
        "_3b5.xml,ADC_00.xml,BC3SPSA_BPS_01.xml,BC3SPSA_CPU_MEM_00.xml,BC3SPSA_FAN_01.xml,BC3SPSA_FANBOARD_CPLD_00.xml,BC3SPSA_FANBOARD_LSW_01.xml,BC3SPSA_FANBOARD_PHY_01.xml,BC3SPSA_FANBOARD_TEST_00.xml,BC3SPSA_FANBOARD_TEST_NVME_00.xml,BC3SPSA_IOBOARD_00.xml,BC3SPSA_OCP_NCSI_01.xml,BC3SPSA_SMM_POWER_00.xml,BC3SPSA_VRD_01.xml,COOLING_POLICY_00.xml,CustomizeSensor.xml,HDD_CABLE_00.xml,M2_SATA_SSD_00.xml,PCIE_ADDR_01.xml,PS_EVENT_01.xml",
        "_3b5.xml,ADC_00.xml,BC3SPSA_BPS_02.xml,BC3SPSA_CPU_MEM_00.xml,BC3SPSA_FAN_01.xml,BC3SPSA_FANBOARD_CPLD_01.xml,BC3SPSA_FANBOARD_LSW_01.xml,BC3SPSA_FANBOARD_PHY_01.xml,BC3SPSA_FANBOARD_TEST_00.xml,BC3SPSA_FANBOARD_TEST_NVME_00.xml,BC3SPSA_IOBOARD_00.xml,BC3SPSA_OCP_NCSI_01.xml,BC3SPSA_SMM_POWER_00.xml,BC3SPSA_VRD_01.xml,COOLING_POLICY_01.xml,CustomizeSensor.xml,HDD_CABLE_00.xml,M2_SATA_SSD_00.xml,PCIE_ADDR_01.xml,PS_EVENT_01.xml",
        "_3b5.xml,ADC_00.xml,BC3SPSA_BPS_01.xml,BC3SPSA_CPU_MEM_00.xml,BC3SPSA_FAN_01.xml,BC3SPSA_FANBOARD_CPLD_00.xml,BC3SPSA_FANBOARD_LSW_01.xml,BC3SPSA_FANBOARD_PHY_01.xml,BC3SPSA_FANBOARD_TEST_00.xml,BC3SPSA_FANBOARD_TEST_NVME_00.xml,BC3SPSA_IOBOARD_00.xml,BC3SPSA_OCP_NCSI_01.xml,BC3SPSA_SMM_POWER_00.xml,BC3SPSA_VRD_01.xml,COOLING_POLICY_00.xml,CustomizeSensor.xml,HDD_CABLE_00.xml,M2_SATA_SSD_00.xml,PCIE_ADDR_01.xml,PS_EVENT_01.xml",
        "_3b5.xml,ADC_00.xml,BC3SPSA_BPS_02.xml,BC3SPSA_CPU_MEM_00.xml,BC3SPSA_FAN_01.xml,BC3SPSA_FANBOARD_CPLD_01.xml,BC3SPSA_FANBOARD_LSW_01.xml,BC3SPSA_FANBOARD_PHY_01.xml,BC3SPSA_FANBOARD_TEST_00.xml,BC3SPSA_FANBOARD_TEST_NVME_00.xml,BC3SPSA_IOBOARD_00.xml,BC3SPSA_OCP_NCSI_01.xml,BC3SPSA_SMM_POWER_00.xml,BC3SPSA_VRD_01.xml,COOLING_POLICY_01.xml,CustomizeSensor.xml,HDD_CABLE_00.xml,M2_SATA_SSD_00.xml,PCIE_ADDR_01.xml,PS_EVENT_01.xml",
        "_3b5.xml,ADC_00.xml,BC3SPSA_BPS_01.xml,BC3SPSA_CPU_MEM_00.xml,BC3SPSA_FAN_01.xml,BC3SPSA_FANBOARD_CPLD_00.xml,BC3SPSA_FANBOARD_LSW_01.xml,BC3SPSA_FANBOARD_PHY_01.xml,BC3SPSA_FANBOARD_TEST_00.xml,BC3SPSA_FANBOARD_TEST_NVME_00.xml,BC3SPSA_IOBOARD_00.xml,BC3SPSA_OCP_NCSI_01.xml,BC3SPSA_SMM_POWER_00.xml,BC3SPSA_VRD_01.xml,COOLING_POLICY_00.xml,CustomizeSensor.xml,HDD_CABLE_00.xml,M2_SATA_SSD_00.xml,PCIE_ADDR_01.xml,PS_EVENT_01.xml",
        "_3b5.xml,ADC_00.xml,BC3SPSA_BPS_02.xml,BC3SPSA_CPU_MEM_00.xml,BC3SPSA_FAN_01.xml,BC3SPSA_FANBOARD_CPLD_01.xml,BC3SPSA_FANBOARD_LSW_01.xml,BC3SPSA_FANBOARD_PHY_01.xml,BC3SPSA_FANBOARD_TEST_00.xml,BC3SPSA_FANBOARD_TEST_NVME_00.xml,BC3SPSA_IOBOARD_00.xml,BC3SPSA_OCP_NCSI_01.xml,BC3SPSA_SMM_POWER_00.xml,BC3SPSA_VRD_01.xml,COOLING_POLICY_01.xml,CustomizeSensor.xml,HDD_CABLE_00.xml,M2_SATA_SSD_00.xml,PCIE_ADDR_01.xml,PS_EVENT_01.xml",
        "_3b5.xml,ADC_00.xml,BC3SPSA_BPS_01.xml,BC3SPSA_CPU_MEM_00.xml,BC3SPSA_FAN_01.xml,BC3SPSA_FANBOARD_CPLD_00.xml,BC3SPSA_FANBOARD_LSW_01.xml,BC3SPSA_FANBOARD_PHY_01.xml,BC3SPSA_FANBOARD_TEST_00.xml,BC3SPSA_FANBOARD_TEST_NVME_00.xml,BC3SPSA_IOBOARD_00.xml,BC3SPSA_OCP_NCSI_01.xml,BC3SPSA_SMM_POWER_00.xml,BC3SPSA_VRD_01.xml,COOLING_POLICY_00.xml,CustomizeSensor.xml,HDD_CABLE_00.xml,M2_SATA_SSD_00.xml,PCIE_ADDR_01.xml,PS_EVENT_01.xml",
        "_3b5.xml,ADC_00.xml,BC3SPSA_BPS_02.xml,BC3SPSA_CPU_MEM_00.xml,BC3SPSA_FAN_01.xml,BC3SPSA_FANBOARD_CPLD_01.xml,BC3SPSA_FANBOARD_LSW_01.xml,BC3SPSA_FANBOARD_PHY_01.xml,BC3SPSA_FANBOARD_TEST_00.xml,BC3SPSA_FANBOARD_TEST_NVME_00.xml,BC3SPSA_IOBOARD_00.xml,BC3SPSA_OCP_NCSI_01.xml,BC3SPSA_SMM_POWER_00.xml,BC3SPSA_VRD_01.xml,COOLING_POLICY_01.xml,CustomizeSensor.xml,HDD_CABLE_00.xml,M2_SATA_SSD_00.xml,PCIE_ADDR_01.xml,PS_EVENT_01.xml",
        "_3b6.xml,ADC_00.xml,BC3SPSA_BPS_01.xml,BC3SPSA_CPU_MEM_00.xml,BC3SPSA_FAN_01.xml,BC3SPSA_FANBOARD_CPLD_00.xml,BC3SPSA_FANBOARD_LSW_01.xml,BC3SPSA_FANBOARD_PHY_01.xml,BC3SPSA_FANBOARD_TEST_00.xml,BC3SPSA_FANBOARD_TEST_NVME_00.xml,BC3SPSA_IOBOARD_00.xml,BC3SPSA_OCP_NCSI_01.xml,BC3SPSA_SMM_POWER_00.xml,BC3SPSA_VRD_01.xml,COOLING_POLICY_00.xml,CustomizeSensor.xml,HDD_CABLE_00.xml,M2_SATA_SSD_00.xml,PCIE_ADDR_01.xml,PS_EVENT_01.xml",
        "_3b6.xml,ADC_00.xml,BC3SPSA_BPS_02.xml,BC3SPSA_CPU_MEM_00.xml,BC3SPSA_FAN_01.xml,BC3SPSA_FANBOARD_CPLD_01.xml,BC3SPSA_FANBOARD_LSW_01.xml,BC3SPSA_FANBOARD_PHY_01.xml,BC3SPSA_FANBOARD_TEST_00.xml,BC3SPSA_FANBOARD_TEST_NVME_00.xml,BC3SPSA_IOBOARD_00.xml,BC3SPSA_OCP_NCSI_01.xml,BC3SPSA_SMM_POWER_00.xml,BC3SPSA_VRD_01.xml,COOLING_POLICY_01.xml,CustomizeSensor.xml,HDD_CABLE_00.xml,M2_SATA_SSD_00.xml,PCIE_ADDR_01.xml,PS_EVENT_01.xml",
        "_3b6.xml,ADC_00.xml,BC3SPSA_BPS_01.xml,BC3SPSA_CPU_MEM_00.xml,BC3SPSA_FAN_01.xml,BC3SPSA_FANBOARD_CPLD_00.xml,BC3SPSA_FANBOARD_LSW_01.xml,BC3SPSA_FANBOARD_PHY_01.xml,BC3SPSA_FANBOARD_TEST_00.xml,BC3SPSA_FANBOARD_TEST_NVME_00.xml,BC3SPSA_IOBOARD_00.xml,BC3SPSA_OCP_NCSI_01.xml,BC3SPSA_SMM_POWER_00.xml,BC3SPSA_VRD_01.xml,COOLING_POLICY_00.xml,CustomizeSensor.xml,HDD_CABLE_00.xml,M2_SATA_SSD_00.xml,PCIE_ADDR_01.xml,PS_EVENT_01.xml",
        "_3b6.xml,ADC_00.xml,BC3SPSA_BPS_02.xml,BC3SPSA_CPU_MEM_00.xml,BC3SPSA_FAN_01.xml,BC3SPSA_FANBOARD_CPLD_01.xml,BC3SPSA_FANBOARD_LSW_01.xml,BC3SPSA_FANBOARD_PHY_01.xml,BC3SPSA_FANBOARD_TEST_00.xml,BC3SPSA_FANBOARD_TEST_NVME_00.xml,BC3SPSA_IOBOARD_00.xml,BC3SPSA_OCP_NCSI_01.xml,BC3SPSA_SMM_POWER_00.xml,BC3SPSA_VRD_01.xml,COOLING_POLICY_01.xml,CustomizeSensor.xml,HDD_CABLE_00.xml,M2_SATA_SSD_00.xml,PCIE_ADDR_01.xml,PS_EVENT_01.xml",
        "_3b6.xml,ADC_00.xml,BC3SPSA_BPS_01.xml,BC3SPSA_CPU_MEM_00.xml,BC3SPSA_FAN_01.xml,BC3SPSA_FANBOARD_CPLD_00.xml,BC3SPSA_FANBOARD_LSW_01.xml,BC3SPSA_FANBOARD_PHY_01.xml,BC3SPSA_FANBOARD_TEST_00.xml,BC3SPSA_FANBOARD_TEST_NVME_00.xml,BC3SPSA_IOBOARD_00.xml,BC3SPSA_OCP_NCSI_01.xml,BC3SPSA_SMM_POWER_00.xml,BC3SPSA_VRD_01.xml,COOLING_POLICY_00.xml,CustomizeSensor.xml,HDD_CABLE_00.xml,M2_SATA_SSD_00.xml,PCIE_ADDR_01.xml,PS_EVENT_01.xml",
        "_3b6.xml,ADC_00.xml,BC3SPSA_BPS_02.xml,BC3SPSA_CPU_MEM_00.xml,BC3SPSA_FAN_01.xml,BC3SPSA_FANBOARD_CPLD_01.xml,BC3SPSA_FANBOARD_LSW_01.xml,BC3SPSA_FANBOARD_PHY_01.xml,BC3SPSA_FANBOARD_TEST_00.xml,BC3SPSA_FANBOARD_TEST_NVME_00.xml,BC3SPSA_IOBOARD_00.xml,BC3SPSA_OCP_NCSI_01.xml,BC3SPSA_SMM_POWER_00.xml,BC3SPSA_VRD_01.xml,COOLING_POLICY_01.xml,CustomizeSensor.xml,HDD_CABLE_00.xml,M2_SATA_SSD_00.xml,PCIE_ADDR_01.xml,PS_EVENT_01.xml",
        "_3b6.xml,ADC_00.xml,BC3SPSA_BPS_01.xml,BC3SPSA_CPU_MEM_00.xml,BC3SPSA_FAN_01.xml,BC3SPSA_FANBOARD_CPLD_00.xml,BC3SPSA_FANBOARD_LSW_01.xml,BC3SPSA_FANBOARD_PHY_01.xml,BC3SPSA_FANBOARD_TEST_00.xml,BC3SPSA_FANBOARD_TEST_NVME_00.xml,BC3SPSA_IOBOARD_00.xml,BC3SPSA_OCP_NCSI_01.xml,BC3SPSA_SMM_POWER_00.xml,BC3SPSA_VRD_01.xml,COOLING_POLICY_00.xml,CustomizeSensor.xml,HDD_CABLE_00.xml,M2_SATA_SSD_00.xml,PCIE_ADDR_01.xml,PS_EVENT_01.xml",
        "_3b6.xml,ADC_00.xml,BC3SPSA_BPS_02.xml,BC3SPSA_CPU_MEM_00.xml,BC3SPSA_FAN_01.xml,BC3SPSA_FANBOARD_CPLD_01.xml,BC3SPSA_FANBOARD_LSW_01.xml,BC3SPSA_FANBOARD_PHY_01.xml,BC3SPSA_FANBOARD_TEST_00.xml,BC3SPSA_FANBOARD_TEST_NVME_00.xml,BC3SPSA_IOBOARD_00.xml,BC3SPSA_OCP_NCSI_01.xml,BC3SPSA_SMM_POWER_00.xml,BC3SPSA_VRD_01.xml,COOLING_POLICY_01.xml,CustomizeSensor.xml,HDD_CABLE_00.xml,M2_SATA_SSD_00.xml,PCIE_ADDR_01.xml,PS_EVENT_01.xml",
        ],
        '1288hv6_2288hv6_5288v6/1288hv6':[
            "_ead.xml,BASEBOARD_00.xml,BC13MBSAA_CFG_00.xml,BC13MBSAA_COOLING_00.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_8c.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_cb.xml,SYSPOWER_SENSOR_ff.xml",
            "_ead.xml,BASEBOARD_00.xml,BC13MBSAA_CFG_01.xml,BC13MBSAA_COOLING_00.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_8c.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_cb.xml,SYSPOWER_SENSOR_ff.xml",
            "_ebd.xml,BASEBOARD_00.xml,BC13MBSAA_CFG_00.xml,BC13MBSAA_COOLING_00.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_8c.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_cb.xml,SYSPOWER_SENSOR_ff.xml",
            "_ebd.xml,BASEBOARD_00.xml,BC13MBSAA_CFG_01.xml,BC13MBSAA_COOLING_00.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_8c.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_cb.xml,SYSPOWER_SENSOR_ff.xml",
            "_ffbd.xml,BASEBOARD_00.xml,BC13MBSAA_CFG_00.xml,BC13MBSAA_COOLING_00.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_8c.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_cb.xml,SYSPOWER_SENSOR_ff.xml",
            "_ffbd.xml,BASEBOARD_00.xml,BC13MBSAA_CFG_01.xml,BC13MBSAA_COOLING_00.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_8c.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_cb.xml,SYSPOWER_SENSOR_ff.xml"
        ],
        '1288hv6_2288hv6_5288v6/2288hv6_5288v6':[
            "_eb1.xml,BASEBOARD_00.xml,BC13MBSA_CFG_00.xml,BC13MBSA_COOLING_00.xml,BC13MBSA_FAN_00.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_4b.xml,DISKBP_PcieAddrInfo_4d.xml,DISKBP_PcieAddrInfo_7c.xml,DISKBP_PcieAddrInfo_7d.xml,DISKBP_PcieAddrInfo_8d.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_45.xml,DISKBP_PcieAddrInfo_74.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_82.xml,DISKBP_PcieAddrInfo_83.xml,DISKBP_PcieAddrInfo_88.xml,DPU_EXTRA_01.xml,PcieCardCooling_00.xml,PRODUCT_b1_02.xml,RISER_PcieAddrInfo_3b.xml,RISER_PcieAddrInfo_3c.xml,RISER_PcieAddrInfo_3d.xml,RISER_PcieAddrInfo_3e.xml,RISER_PcieAddrInfo_3f.xml,RISER_PcieAddrInfo_9a.xml,RISER_PcieAddrInfo_9b.xml,RISER_PcieAddrInfo_9c.xml,RISER_PcieAddrInfo_9d.xml,RISER_PcieAddrInfo_9e.xml,RISER_PcieAddrInfo_38.xml,RISER_PcieAddrInfo_39.xml,RISER_PcieAddrInfo_40.xml,RISER_PcieAddrInfo_46.xml,RISER_PcieAddrInfo_90.xml,RISER_PcieAddrInfo_92.xml,RISER_PcieAddrInfo_93.xml,RISER_PcieAddrInfo_95.xml,RISER_PcieAddrInfo_96.xml,RISER_PcieAddrInfo_97.xml,RISER_PcieAddrInfo_98.xml,RISER_PcieAddrInfo_99.xml,RISER3_CONFIG_00.xml,RISER9_CONFIG_00.xml,SYSPOWER_SENSOR_ff.xml",
            "_eb1.xml,BASEBOARD_00.xml,BC13MBSA_CFG_01.xml,BC13MBSA_COOLING_00.xml,BC13MBSA_FAN_00.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_4b.xml,DISKBP_PcieAddrInfo_4d.xml,DISKBP_PcieAddrInfo_7c.xml,DISKBP_PcieAddrInfo_7d.xml,DISKBP_PcieAddrInfo_8d.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_45.xml,DISKBP_PcieAddrInfo_74.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_82.xml,DISKBP_PcieAddrInfo_83.xml,DISKBP_PcieAddrInfo_88.xml,DPU_EXTRA_01.xml,PcieCardCooling_00.xml,PRODUCT_b1_02.xml,RISER_PcieAddrInfo_3b.xml,RISER_PcieAddrInfo_3c.xml,RISER_PcieAddrInfo_3d.xml,RISER_PcieAddrInfo_3e.xml,RISER_PcieAddrInfo_3f.xml,RISER_PcieAddrInfo_9a.xml,RISER_PcieAddrInfo_9b.xml,RISER_PcieAddrInfo_9c.xml,RISER_PcieAddrInfo_9d.xml,RISER_PcieAddrInfo_9e.xml,RISER_PcieAddrInfo_38.xml,RISER_PcieAddrInfo_39.xml,RISER_PcieAddrInfo_40.xml,RISER_PcieAddrInfo_46.xml,RISER_PcieAddrInfo_90.xml,RISER_PcieAddrInfo_92.xml,RISER_PcieAddrInfo_93.xml,RISER_PcieAddrInfo_95.xml,RISER_PcieAddrInfo_96.xml,RISER_PcieAddrInfo_97.xml,RISER_PcieAddrInfo_98.xml,RISER_PcieAddrInfo_99.xml,RISER3_CONFIG_00.xml,RISER9_CONFIG_00.xml,SYSPOWER_SENSOR_ff.xml",
            "_eb1.xml,BASEBOARD_00.xml,BC13MBSA_CFG_00.xml,BC13MBSA_COOLING_00.xml,BC13MBSA_FAN_01.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_4b.xml,DISKBP_PcieAddrInfo_4d.xml,DISKBP_PcieAddrInfo_7c.xml,DISKBP_PcieAddrInfo_7d.xml,DISKBP_PcieAddrInfo_8d.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_45.xml,DISKBP_PcieAddrInfo_74.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_82.xml,DISKBP_PcieAddrInfo_83.xml,DISKBP_PcieAddrInfo_88.xml,DPU_EXTRA_01.xml,PcieCardCooling_00.xml,PRODUCT_b1_02.xml,RISER_PcieAddrInfo_3b.xml,RISER_PcieAddrInfo_3c.xml,RISER_PcieAddrInfo_3d.xml,RISER_PcieAddrInfo_3e.xml,RISER_PcieAddrInfo_3f.xml,RISER_PcieAddrInfo_9a.xml,RISER_PcieAddrInfo_9b.xml,RISER_PcieAddrInfo_9c.xml,RISER_PcieAddrInfo_9d.xml,RISER_PcieAddrInfo_9e.xml,RISER_PcieAddrInfo_38.xml,RISER_PcieAddrInfo_39.xml,RISER_PcieAddrInfo_40.xml,RISER_PcieAddrInfo_46.xml,RISER_PcieAddrInfo_90.xml,RISER_PcieAddrInfo_92.xml,RISER_PcieAddrInfo_93.xml,RISER_PcieAddrInfo_95.xml,RISER_PcieAddrInfo_96.xml,RISER_PcieAddrInfo_97.xml,RISER_PcieAddrInfo_98.xml,RISER_PcieAddrInfo_99.xml,RISER3_CONFIG_00.xml,RISER9_CONFIG_00.xml,SYSPOWER_SENSOR_ff.xml",
            "_eb1.xml,BASEBOARD_00.xml,BC13MBSA_CFG_00.xml,BC13MBSA_COOLING_00.xml,BC13MBSA_FAN_00.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_4b.xml,DISKBP_PcieAddrInfo_4d.xml,DISKBP_PcieAddrInfo_7c.xml,DISKBP_PcieAddrInfo_7d.xml,DISKBP_PcieAddrInfo_8d.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_45.xml,DISKBP_PcieAddrInfo_74.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_82.xml,DISKBP_PcieAddrInfo_83.xml,DISKBP_PcieAddrInfo_88.xml,DPU_EXTRA_02.xml,PcieCardCooling_00.xml,PRODUCT_b1_02.xml,RISER_PcieAddrInfo_3b.xml,RISER_PcieAddrInfo_3c.xml,RISER_PcieAddrInfo_3d.xml,RISER_PcieAddrInfo_3e.xml,RISER_PcieAddrInfo_3f.xml,RISER_PcieAddrInfo_9a.xml,RISER_PcieAddrInfo_9b.xml,RISER_PcieAddrInfo_9c.xml,RISER_PcieAddrInfo_9d.xml,RISER_PcieAddrInfo_9e.xml,RISER_PcieAddrInfo_38.xml,RISER_PcieAddrInfo_39.xml,RISER_PcieAddrInfo_40.xml,RISER_PcieAddrInfo_46.xml,RISER_PcieAddrInfo_90.xml,RISER_PcieAddrInfo_92.xml,RISER_PcieAddrInfo_93.xml,RISER_PcieAddrInfo_95.xml,RISER_PcieAddrInfo_96.xml,RISER_PcieAddrInfo_97.xml,RISER_PcieAddrInfo_98.xml,RISER_PcieAddrInfo_99.xml,RISER3_CONFIG_00.xml,RISER9_CONFIG_00.xml,SYSPOWER_SENSOR_ff.xml",
            "_eb1.xml,BASEBOARD_00.xml,BC13MBSA_CFG_00.xml,BC13MBSA_COOLING_00.xml,BC13MBSA_FAN_00.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_4b.xml,DISKBP_PcieAddrInfo_4d.xml,DISKBP_PcieAddrInfo_7c.xml,DISKBP_PcieAddrInfo_7d.xml,DISKBP_PcieAddrInfo_8d.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_45.xml,DISKBP_PcieAddrInfo_74.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_82.xml,DISKBP_PcieAddrInfo_83.xml,DISKBP_PcieAddrInfo_88.xml,DPU_EXTRA_01.xml,PcieCardCooling_01.xml,PRODUCT_b1_02.xml,RISER_PcieAddrInfo_3b.xml,RISER_PcieAddrInfo_3c.xml,RISER_PcieAddrInfo_3d.xml,RISER_PcieAddrInfo_3e.xml,RISER_PcieAddrInfo_3f.xml,RISER_PcieAddrInfo_9a.xml,RISER_PcieAddrInfo_9b.xml,RISER_PcieAddrInfo_9c.xml,RISER_PcieAddrInfo_9d.xml,RISER_PcieAddrInfo_9e.xml,RISER_PcieAddrInfo_38.xml,RISER_PcieAddrInfo_39.xml,RISER_PcieAddrInfo_40.xml,RISER_PcieAddrInfo_46.xml,RISER_PcieAddrInfo_90.xml,RISER_PcieAddrInfo_92.xml,RISER_PcieAddrInfo_93.xml,RISER_PcieAddrInfo_95.xml,RISER_PcieAddrInfo_96.xml,RISER_PcieAddrInfo_97.xml,RISER_PcieAddrInfo_98.xml,RISER_PcieAddrInfo_99.xml,RISER3_CONFIG_00.xml,RISER9_CONFIG_00.xml,SYSPOWER_SENSOR_ff.xml",
            "_eb1.xml,BASEBOARD_00.xml,BC13MBSA_CFG_00.xml,BC13MBSA_COOLING_00.xml,BC13MBSA_FAN_00.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_4b.xml,DISKBP_PcieAddrInfo_4d.xml,DISKBP_PcieAddrInfo_7c.xml,DISKBP_PcieAddrInfo_7d.xml,DISKBP_PcieAddrInfo_8d.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_45.xml,DISKBP_PcieAddrInfo_74.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_82.xml,DISKBP_PcieAddrInfo_83.xml,DISKBP_PcieAddrInfo_88.xml,DPU_EXTRA_01.xml,PcieCardCooling_02.xml,PRODUCT_b1_02.xml,RISER_PcieAddrInfo_3b.xml,RISER_PcieAddrInfo_3c.xml,RISER_PcieAddrInfo_3d.xml,RISER_PcieAddrInfo_3e.xml,RISER_PcieAddrInfo_3f.xml,RISER_PcieAddrInfo_9a.xml,RISER_PcieAddrInfo_9b.xml,RISER_PcieAddrInfo_9c.xml,RISER_PcieAddrInfo_9d.xml,RISER_PcieAddrInfo_9e.xml,RISER_PcieAddrInfo_38.xml,RISER_PcieAddrInfo_39.xml,RISER_PcieAddrInfo_40.xml,RISER_PcieAddrInfo_46.xml,RISER_PcieAddrInfo_90.xml,RISER_PcieAddrInfo_92.xml,RISER_PcieAddrInfo_93.xml,RISER_PcieAddrInfo_95.xml,RISER_PcieAddrInfo_96.xml,RISER_PcieAddrInfo_97.xml,RISER_PcieAddrInfo_98.xml,RISER_PcieAddrInfo_99.xml,RISER3_CONFIG_00.xml,RISER9_CONFIG_00.xml,SYSPOWER_SENSOR_ff.xml",
            "_eb1.xml,BASEBOARD_00.xml,BC13MBSA_CFG_00.xml,BC13MBSA_COOLING_00.xml,BC13MBSA_FAN_00.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_4b.xml,DISKBP_PcieAddrInfo_4d.xml,DISKBP_PcieAddrInfo_7c.xml,DISKBP_PcieAddrInfo_7d.xml,DISKBP_PcieAddrInfo_8d.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_45.xml,DISKBP_PcieAddrInfo_74.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_82.xml,DISKBP_PcieAddrInfo_83.xml,DISKBP_PcieAddrInfo_88.xml,DPU_EXTRA_01.xml,PcieCardCooling_00.xml,PRODUCT_b1_03.xml,RISER_PcieAddrInfo_3b.xml,RISER_PcieAddrInfo_3c.xml,RISER_PcieAddrInfo_3d.xml,RISER_PcieAddrInfo_3e.xml,RISER_PcieAddrInfo_3f.xml,RISER_PcieAddrInfo_9a.xml,RISER_PcieAddrInfo_9b.xml,RISER_PcieAddrInfo_9c.xml,RISER_PcieAddrInfo_9d.xml,RISER_PcieAddrInfo_9e.xml,RISER_PcieAddrInfo_38.xml,RISER_PcieAddrInfo_39.xml,RISER_PcieAddrInfo_40.xml,RISER_PcieAddrInfo_46.xml,RISER_PcieAddrInfo_90.xml,RISER_PcieAddrInfo_92.xml,RISER_PcieAddrInfo_93.xml,RISER_PcieAddrInfo_95.xml,RISER_PcieAddrInfo_96.xml,RISER_PcieAddrInfo_97.xml,RISER_PcieAddrInfo_98.xml,RISER_PcieAddrInfo_99.xml,RISER3_CONFIG_00.xml,RISER9_CONFIG_00.xml,SYSPOWER_SENSOR_ff.xml",
            "_eb1.xml,BASEBOARD_00.xml,BC13MBSA_CFG_00.xml,BC13MBSA_COOLING_00.xml,BC13MBSA_FAN_00.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_4b.xml,DISKBP_PcieAddrInfo_4d.xml,DISKBP_PcieAddrInfo_7c.xml,DISKBP_PcieAddrInfo_7d.xml,DISKBP_PcieAddrInfo_8d.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_45.xml,DISKBP_PcieAddrInfo_74.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_82.xml,DISKBP_PcieAddrInfo_83.xml,DISKBP_PcieAddrInfo_88.xml,DPU_EXTRA_01.xml,PcieCardCooling_00.xml,RISER_PcieAddrInfo_3b.xml,RISER_PcieAddrInfo_3c.xml,RISER_PcieAddrInfo_3d.xml,RISER_PcieAddrInfo_3e.xml,RISER_PcieAddrInfo_3f.xml,RISER_PcieAddrInfo_9a.xml,RISER_PcieAddrInfo_9b.xml,RISER_PcieAddrInfo_9c.xml,RISER_PcieAddrInfo_9d.xml,RISER_PcieAddrInfo_9e.xml,RISER_PcieAddrInfo_38.xml,RISER_PcieAddrInfo_39.xml,RISER_PcieAddrInfo_40.xml,RISER_PcieAddrInfo_46.xml,RISER_PcieAddrInfo_90.xml,RISER_PcieAddrInfo_92.xml,RISER_PcieAddrInfo_93.xml,RISER_PcieAddrInfo_95.xml,RISER_PcieAddrInfo_96.xml,RISER_PcieAddrInfo_97.xml,RISER_PcieAddrInfo_98.xml,RISER_PcieAddrInfo_99.xml,RISER3_CONFIG_00.xml,RISER9_CONFIG_00.xml,SYSPOWER_SENSOR_ff.xml",
            "_eb1.xml,BASEBOARD_00.xml,BC13MBSA_CFG_00.xml,BC13MBSA_COOLING_00.xml,BC13MBSA_FAN_00.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_4b.xml,DISKBP_PcieAddrInfo_4d.xml,DISKBP_PcieAddrInfo_7c.xml,DISKBP_PcieAddrInfo_7d.xml,DISKBP_PcieAddrInfo_8d.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_45.xml,DISKBP_PcieAddrInfo_74.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_82.xml,DISKBP_PcieAddrInfo_83.xml,DISKBP_PcieAddrInfo_88.xml,DPU_EXTRA_01.xml,PcieCardCooling_00.xml,PRODUCT_b5_02.xml,RISER_PcieAddrInfo_3b.xml,RISER_PcieAddrInfo_3c.xml,RISER_PcieAddrInfo_3d.xml,RISER_PcieAddrInfo_3e.xml,RISER_PcieAddrInfo_3f.xml,RISER_PcieAddrInfo_9a.xml,RISER_PcieAddrInfo_9b.xml,RISER_PcieAddrInfo_9c.xml,RISER_PcieAddrInfo_9d.xml,RISER_PcieAddrInfo_9e.xml,RISER_PcieAddrInfo_38.xml,RISER_PcieAddrInfo_39.xml,RISER_PcieAddrInfo_40.xml,RISER_PcieAddrInfo_46.xml,RISER_PcieAddrInfo_90.xml,RISER_PcieAddrInfo_92.xml,RISER_PcieAddrInfo_93.xml,RISER_PcieAddrInfo_95.xml,RISER_PcieAddrInfo_96.xml,RISER_PcieAddrInfo_97.xml,RISER_PcieAddrInfo_98.xml,RISER_PcieAddrInfo_99.xml,RISER3_CONFIG_00.xml,RISER9_CONFIG_00.xml,SYSPOWER_SENSOR_ff.xml",
            "_eb1.xml,BASEBOARD_00.xml,BC13MBSA_CFG_00.xml,BC13MBSA_COOLING_00.xml,BC13MBSA_FAN_00.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_4b.xml,DISKBP_PcieAddrInfo_4d.xml,DISKBP_PcieAddrInfo_7c.xml,DISKBP_PcieAddrInfo_7d.xml,DISKBP_PcieAddrInfo_8d.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_45.xml,DISKBP_PcieAddrInfo_74.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_82.xml,DISKBP_PcieAddrInfo_83.xml,DISKBP_PcieAddrInfo_88.xml,DPU_EXTRA_01.xml,PcieCardCooling_00.xml,PRODUCT_b5_03.xml,RISER_PcieAddrInfo_3b.xml,RISER_PcieAddrInfo_3c.xml,RISER_PcieAddrInfo_3d.xml,RISER_PcieAddrInfo_3e.xml,RISER_PcieAddrInfo_3f.xml,RISER_PcieAddrInfo_9a.xml,RISER_PcieAddrInfo_9b.xml,RISER_PcieAddrInfo_9c.xml,RISER_PcieAddrInfo_9d.xml,RISER_PcieAddrInfo_9e.xml,RISER_PcieAddrInfo_38.xml,RISER_PcieAddrInfo_39.xml,RISER_PcieAddrInfo_40.xml,RISER_PcieAddrInfo_46.xml,RISER_PcieAddrInfo_90.xml,RISER_PcieAddrInfo_92.xml,RISER_PcieAddrInfo_93.xml,RISER_PcieAddrInfo_95.xml,RISER_PcieAddrInfo_96.xml,RISER_PcieAddrInfo_97.xml,RISER_PcieAddrInfo_98.xml,RISER_PcieAddrInfo_99.xml,RISER3_CONFIG_00.xml,RISER9_CONFIG_00.xml,SYSPOWER_SENSOR_ff.xml",
            "_eb1.xml,BASEBOARD_00.xml,BC13MBSA_CFG_00.xml,BC13MBSA_COOLING_00.xml,BC13MBSA_FAN_00.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_4b.xml,DISKBP_PcieAddrInfo_4d.xml,DISKBP_PcieAddrInfo_7c.xml,DISKBP_PcieAddrInfo_7d.xml,DISKBP_PcieAddrInfo_8d.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_45.xml,DISKBP_PcieAddrInfo_74.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_82.xml,DISKBP_PcieAddrInfo_83.xml,DISKBP_PcieAddrInfo_88.xml,DPU_EXTRA_01.xml,PcieCardCooling_00.xml,RISER_PcieAddrInfo_3b.xml,RISER_PcieAddrInfo_3c.xml,RISER_PcieAddrInfo_3d.xml,RISER_PcieAddrInfo_3e.xml,RISER_PcieAddrInfo_3f.xml,RISER_PcieAddrInfo_9a.xml,RISER_PcieAddrInfo_9b.xml,RISER_PcieAddrInfo_9c.xml,RISER_PcieAddrInfo_9d.xml,RISER_PcieAddrInfo_9e.xml,RISER_PcieAddrInfo_38.xml,RISER_PcieAddrInfo_39.xml,RISER_PcieAddrInfo_40.xml,RISER_PcieAddrInfo_46.xml,RISER_PcieAddrInfo_90.xml,RISER_PcieAddrInfo_92.xml,RISER_PcieAddrInfo_93.xml,RISER_PcieAddrInfo_95.xml,RISER_PcieAddrInfo_96.xml,RISER_PcieAddrInfo_97.xml,RISER_PcieAddrInfo_98.xml,RISER_PcieAddrInfo_99.xml,RISER3_CONFIG_00.xml,RISER9_CONFIG_00.xml,SYSPOWER_SENSOR_ff.xml",
            "_eb1.xml,BASEBOARD_00.xml,BC13MBSA_CFG_00.xml,BC13MBSA_COOLING_00.xml,BC13MBSA_FAN_00.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_4b.xml,DISKBP_PcieAddrInfo_4d.xml,DISKBP_PcieAddrInfo_7c.xml,DISKBP_PcieAddrInfo_7d.xml,DISKBP_PcieAddrInfo_8d.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_45.xml,DISKBP_PcieAddrInfo_74.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_82.xml,DISKBP_PcieAddrInfo_83.xml,DISKBP_PcieAddrInfo_88.xml,DPU_EXTRA_01.xml,PcieCardCooling_00.xml,PRODUCT_b1_02.xml,RISER_PcieAddrInfo_3b.xml,RISER_PcieAddrInfo_3c.xml,RISER_PcieAddrInfo_3d.xml,RISER_PcieAddrInfo_3e.xml,RISER_PcieAddrInfo_3f.xml,RISER_PcieAddrInfo_9a.xml,RISER_PcieAddrInfo_9b.xml,RISER_PcieAddrInfo_9c.xml,RISER_PcieAddrInfo_9d.xml,RISER_PcieAddrInfo_9e.xml,RISER_PcieAddrInfo_38.xml,RISER_PcieAddrInfo_39.xml,RISER_PcieAddrInfo_40.xml,RISER_PcieAddrInfo_46.xml,RISER_PcieAddrInfo_90.xml,RISER_PcieAddrInfo_92.xml,RISER_PcieAddrInfo_93.xml,RISER_PcieAddrInfo_95.xml,RISER_PcieAddrInfo_96.xml,RISER_PcieAddrInfo_97.xml,RISER_PcieAddrInfo_98.xml,RISER_PcieAddrInfo_99.xml,RISER3_CONFIG_01.xml,RISER9_CONFIG_00.xml,SYSPOWER_SENSOR_ff.xml",
            "_eb5.xml,BASEBOARD_00.xml,BC13MBSA_CFG_00.xml,BC13MBSA_COOLING_00.xml,BC13MBSA_FAN_00.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_4b.xml,DISKBP_PcieAddrInfo_4d.xml,DISKBP_PcieAddrInfo_7c.xml,DISKBP_PcieAddrInfo_7d.xml,DISKBP_PcieAddrInfo_8d.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_45.xml,DISKBP_PcieAddrInfo_74.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_82.xml,DISKBP_PcieAddrInfo_83.xml,DISKBP_PcieAddrInfo_88.xml,DPU_EXTRA_01.xml,PcieCardCooling_00.xml,PRODUCT_b1_02.xml,RISER_PcieAddrInfo_3b.xml,RISER_PcieAddrInfo_3c.xml,RISER_PcieAddrInfo_3d.xml,RISER_PcieAddrInfo_3e.xml,RISER_PcieAddrInfo_3f.xml,RISER_PcieAddrInfo_9a.xml,RISER_PcieAddrInfo_9b.xml,RISER_PcieAddrInfo_9c.xml,RISER_PcieAddrInfo_9d.xml,RISER_PcieAddrInfo_9e.xml,RISER_PcieAddrInfo_38.xml,RISER_PcieAddrInfo_39.xml,RISER_PcieAddrInfo_40.xml,RISER_PcieAddrInfo_46.xml,RISER_PcieAddrInfo_90.xml,RISER_PcieAddrInfo_92.xml,RISER_PcieAddrInfo_93.xml,RISER_PcieAddrInfo_95.xml,RISER_PcieAddrInfo_96.xml,RISER_PcieAddrInfo_97.xml,RISER_PcieAddrInfo_98.xml,RISER_PcieAddrInfo_99.xml,RISER3_CONFIG_00.xml,RISER9_CONFIG_00.xml,SYSPOWER_SENSOR_ff.xml",
            "_ffb5.xml,BASEBOARD_00.xml,BC13MBSA_CFG_00.xml,BC13MBSA_COOLING_00.xml,BC13MBSA_FAN_00.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_4b.xml,DISKBP_PcieAddrInfo_4d.xml,DISKBP_PcieAddrInfo_7c.xml,DISKBP_PcieAddrInfo_7d.xml,DISKBP_PcieAddrInfo_8d.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_45.xml,DISKBP_PcieAddrInfo_74.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_82.xml,DISKBP_PcieAddrInfo_83.xml,DISKBP_PcieAddrInfo_88.xml,DPU_EXTRA_01.xml,PcieCardCooling_00.xml,PRODUCT_b1_02.xml,RISER_PcieAddrInfo_3b.xml,RISER_PcieAddrInfo_3c.xml,RISER_PcieAddrInfo_3d.xml,RISER_PcieAddrInfo_3e.xml,RISER_PcieAddrInfo_3f.xml,RISER_PcieAddrInfo_9a.xml,RISER_PcieAddrInfo_9b.xml,RISER_PcieAddrInfo_9c.xml,RISER_PcieAddrInfo_9d.xml,RISER_PcieAddrInfo_9e.xml,RISER_PcieAddrInfo_38.xml,RISER_PcieAddrInfo_39.xml,RISER_PcieAddrInfo_40.xml,RISER_PcieAddrInfo_46.xml,RISER_PcieAddrInfo_90.xml,RISER_PcieAddrInfo_92.xml,RISER_PcieAddrInfo_93.xml,RISER_PcieAddrInfo_95.xml,RISER_PcieAddrInfo_96.xml,RISER_PcieAddrInfo_97.xml,RISER_PcieAddrInfo_98.xml,RISER_PcieAddrInfo_99.xml,RISER3_CONFIG_00.xml,RISER9_CONFIG_00.xml,SYSPOWER_SENSOR_ff.xml"
        ],
        'DP1210_DP2210_DP4210/DP1210':[
            "_ead.xml,BASEBOARD_00.xml,BC13MBSAA_CFG_00.xml,BC13MBSAA_COOLING_00.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_8c.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_cb.xml,SYSPOWER_SENSOR_ff.xml",
            "_ead.xml,BASEBOARD_00.xml,BC13MBSAA_CFG_01.xml,BC13MBSAA_COOLING_00.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_8c.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_cb.xml,SYSPOWER_SENSOR_ff.xml",
            "_ebd.xml,BASEBOARD_00.xml,BC13MBSAA_CFG_00.xml,BC13MBSAA_COOLING_00.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_8c.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_cb.xml,SYSPOWER_SENSOR_ff.xml",
            "_ebd.xml,BASEBOARD_00.xml,BC13MBSAA_CFG_01.xml,BC13MBSAA_COOLING_00.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_8c.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_cb.xml,SYSPOWER_SENSOR_ff.xml",
            "_ffbd.xml,BASEBOARD_00.xml,BC13MBSAA_CFG_00.xml,BC13MBSAA_COOLING_00.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_8c.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_cb.xml,SYSPOWER_SENSOR_ff.xml",
            "_ffbd.xml,BASEBOARD_00.xml,BC13MBSAA_CFG_01.xml,BC13MBSAA_COOLING_00.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_8c.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_cb.xml,SYSPOWER_SENSOR_ff.xml"
        ],
        'DP1210_DP2210_DP4210/DP2210_DP4210':[
            "_eb1.xml,BASEBOARD_00.xml,BC13MBSA_CFG_00.xml,BC13MBSA_COOLING_00.xml,BC13MBSA_FAN_00.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_4b.xml,DISKBP_PcieAddrInfo_4d.xml,DISKBP_PcieAddrInfo_7c.xml,DISKBP_PcieAddrInfo_7d.xml,DISKBP_PcieAddrInfo_8d.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_45.xml,DISKBP_PcieAddrInfo_74.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_82.xml,DISKBP_PcieAddrInfo_83.xml,DISKBP_PcieAddrInfo_88.xml,DPU_EXTRA_01.xml,PcieCardCooling_00.xml,PRODUCT_b1_02.xml,RISER_PcieAddrInfo_3b.xml,RISER_PcieAddrInfo_3c.xml,RISER_PcieAddrInfo_3d.xml,RISER_PcieAddrInfo_3e.xml,RISER_PcieAddrInfo_3f.xml,RISER_PcieAddrInfo_9a.xml,RISER_PcieAddrInfo_9b.xml,RISER_PcieAddrInfo_9c.xml,RISER_PcieAddrInfo_9d.xml,RISER_PcieAddrInfo_9e.xml,RISER_PcieAddrInfo_38.xml,RISER_PcieAddrInfo_39.xml,RISER_PcieAddrInfo_40.xml,RISER_PcieAddrInfo_46.xml,RISER_PcieAddrInfo_90.xml,RISER_PcieAddrInfo_92.xml,RISER_PcieAddrInfo_93.xml,RISER_PcieAddrInfo_95.xml,RISER_PcieAddrInfo_96.xml,RISER_PcieAddrInfo_97.xml,RISER_PcieAddrInfo_98.xml,RISER_PcieAddrInfo_99.xml,RISER3_CONFIG_00.xml,RISER9_CONFIG_00.xml,SYSPOWER_SENSOR_ff.xml",
            "_eb1.xml,BASEBOARD_00.xml,BC13MBSA_CFG_01.xml,BC13MBSA_COOLING_00.xml,BC13MBSA_FAN_00.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_4b.xml,DISKBP_PcieAddrInfo_4d.xml,DISKBP_PcieAddrInfo_7c.xml,DISKBP_PcieAddrInfo_7d.xml,DISKBP_PcieAddrInfo_8d.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_45.xml,DISKBP_PcieAddrInfo_74.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_82.xml,DISKBP_PcieAddrInfo_83.xml,DISKBP_PcieAddrInfo_88.xml,DPU_EXTRA_01.xml,PcieCardCooling_00.xml,PRODUCT_b1_02.xml,RISER_PcieAddrInfo_3b.xml,RISER_PcieAddrInfo_3c.xml,RISER_PcieAddrInfo_3d.xml,RISER_PcieAddrInfo_3e.xml,RISER_PcieAddrInfo_3f.xml,RISER_PcieAddrInfo_9a.xml,RISER_PcieAddrInfo_9b.xml,RISER_PcieAddrInfo_9c.xml,RISER_PcieAddrInfo_9d.xml,RISER_PcieAddrInfo_9e.xml,RISER_PcieAddrInfo_38.xml,RISER_PcieAddrInfo_39.xml,RISER_PcieAddrInfo_40.xml,RISER_PcieAddrInfo_46.xml,RISER_PcieAddrInfo_90.xml,RISER_PcieAddrInfo_92.xml,RISER_PcieAddrInfo_93.xml,RISER_PcieAddrInfo_95.xml,RISER_PcieAddrInfo_96.xml,RISER_PcieAddrInfo_97.xml,RISER_PcieAddrInfo_98.xml,RISER_PcieAddrInfo_99.xml,RISER3_CONFIG_00.xml,RISER9_CONFIG_00.xml,SYSPOWER_SENSOR_ff.xml",
            "_eb1.xml,BASEBOARD_00.xml,BC13MBSA_CFG_00.xml,BC13MBSA_COOLING_00.xml,BC13MBSA_FAN_01.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_4b.xml,DISKBP_PcieAddrInfo_4d.xml,DISKBP_PcieAddrInfo_7c.xml,DISKBP_PcieAddrInfo_7d.xml,DISKBP_PcieAddrInfo_8d.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_45.xml,DISKBP_PcieAddrInfo_74.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_82.xml,DISKBP_PcieAddrInfo_83.xml,DISKBP_PcieAddrInfo_88.xml,DPU_EXTRA_01.xml,PcieCardCooling_00.xml,PRODUCT_b1_02.xml,RISER_PcieAddrInfo_3b.xml,RISER_PcieAddrInfo_3c.xml,RISER_PcieAddrInfo_3d.xml,RISER_PcieAddrInfo_3e.xml,RISER_PcieAddrInfo_3f.xml,RISER_PcieAddrInfo_9a.xml,RISER_PcieAddrInfo_9b.xml,RISER_PcieAddrInfo_9c.xml,RISER_PcieAddrInfo_9d.xml,RISER_PcieAddrInfo_9e.xml,RISER_PcieAddrInfo_38.xml,RISER_PcieAddrInfo_39.xml,RISER_PcieAddrInfo_40.xml,RISER_PcieAddrInfo_46.xml,RISER_PcieAddrInfo_90.xml,RISER_PcieAddrInfo_92.xml,RISER_PcieAddrInfo_93.xml,RISER_PcieAddrInfo_95.xml,RISER_PcieAddrInfo_96.xml,RISER_PcieAddrInfo_97.xml,RISER_PcieAddrInfo_98.xml,RISER_PcieAddrInfo_99.xml,RISER3_CONFIG_00.xml,RISER9_CONFIG_00.xml,SYSPOWER_SENSOR_ff.xml",
            "_eb1.xml,BASEBOARD_00.xml,BC13MBSA_CFG_00.xml,BC13MBSA_COOLING_00.xml,BC13MBSA_FAN_00.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_4b.xml,DISKBP_PcieAddrInfo_4d.xml,DISKBP_PcieAddrInfo_7c.xml,DISKBP_PcieAddrInfo_7d.xml,DISKBP_PcieAddrInfo_8d.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_45.xml,DISKBP_PcieAddrInfo_74.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_82.xml,DISKBP_PcieAddrInfo_83.xml,DISKBP_PcieAddrInfo_88.xml,DPU_EXTRA_02.xml,PcieCardCooling_00.xml,PRODUCT_b1_02.xml,RISER_PcieAddrInfo_3b.xml,RISER_PcieAddrInfo_3c.xml,RISER_PcieAddrInfo_3d.xml,RISER_PcieAddrInfo_3e.xml,RISER_PcieAddrInfo_3f.xml,RISER_PcieAddrInfo_9a.xml,RISER_PcieAddrInfo_9b.xml,RISER_PcieAddrInfo_9c.xml,RISER_PcieAddrInfo_9d.xml,RISER_PcieAddrInfo_9e.xml,RISER_PcieAddrInfo_38.xml,RISER_PcieAddrInfo_39.xml,RISER_PcieAddrInfo_40.xml,RISER_PcieAddrInfo_46.xml,RISER_PcieAddrInfo_90.xml,RISER_PcieAddrInfo_92.xml,RISER_PcieAddrInfo_93.xml,RISER_PcieAddrInfo_95.xml,RISER_PcieAddrInfo_96.xml,RISER_PcieAddrInfo_97.xml,RISER_PcieAddrInfo_98.xml,RISER_PcieAddrInfo_99.xml,RISER3_CONFIG_00.xml,RISER9_CONFIG_00.xml,SYSPOWER_SENSOR_ff.xml",
            "_eb1.xml,BASEBOARD_00.xml,BC13MBSA_CFG_00.xml,BC13MBSA_COOLING_00.xml,BC13MBSA_FAN_00.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_4b.xml,DISKBP_PcieAddrInfo_4d.xml,DISKBP_PcieAddrInfo_7c.xml,DISKBP_PcieAddrInfo_7d.xml,DISKBP_PcieAddrInfo_8d.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_45.xml,DISKBP_PcieAddrInfo_74.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_82.xml,DISKBP_PcieAddrInfo_83.xml,DISKBP_PcieAddrInfo_88.xml,DPU_EXTRA_01.xml,PcieCardCooling_01.xml,PRODUCT_b1_02.xml,RISER_PcieAddrInfo_3b.xml,RISER_PcieAddrInfo_3c.xml,RISER_PcieAddrInfo_3d.xml,RISER_PcieAddrInfo_3e.xml,RISER_PcieAddrInfo_3f.xml,RISER_PcieAddrInfo_9a.xml,RISER_PcieAddrInfo_9b.xml,RISER_PcieAddrInfo_9c.xml,RISER_PcieAddrInfo_9d.xml,RISER_PcieAddrInfo_9e.xml,RISER_PcieAddrInfo_38.xml,RISER_PcieAddrInfo_39.xml,RISER_PcieAddrInfo_40.xml,RISER_PcieAddrInfo_46.xml,RISER_PcieAddrInfo_90.xml,RISER_PcieAddrInfo_92.xml,RISER_PcieAddrInfo_93.xml,RISER_PcieAddrInfo_95.xml,RISER_PcieAddrInfo_96.xml,RISER_PcieAddrInfo_97.xml,RISER_PcieAddrInfo_98.xml,RISER_PcieAddrInfo_99.xml,RISER3_CONFIG_00.xml,RISER9_CONFIG_00.xml,SYSPOWER_SENSOR_ff.xml",
            "_eb1.xml,BASEBOARD_00.xml,BC13MBSA_CFG_00.xml,BC13MBSA_COOLING_00.xml,BC13MBSA_FAN_00.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_4b.xml,DISKBP_PcieAddrInfo_4d.xml,DISKBP_PcieAddrInfo_7c.xml,DISKBP_PcieAddrInfo_7d.xml,DISKBP_PcieAddrInfo_8d.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_45.xml,DISKBP_PcieAddrInfo_74.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_82.xml,DISKBP_PcieAddrInfo_83.xml,DISKBP_PcieAddrInfo_88.xml,DPU_EXTRA_01.xml,PcieCardCooling_02.xml,PRODUCT_b1_02.xml,RISER_PcieAddrInfo_3b.xml,RISER_PcieAddrInfo_3c.xml,RISER_PcieAddrInfo_3d.xml,RISER_PcieAddrInfo_3e.xml,RISER_PcieAddrInfo_3f.xml,RISER_PcieAddrInfo_9a.xml,RISER_PcieAddrInfo_9b.xml,RISER_PcieAddrInfo_9c.xml,RISER_PcieAddrInfo_9d.xml,RISER_PcieAddrInfo_9e.xml,RISER_PcieAddrInfo_38.xml,RISER_PcieAddrInfo_39.xml,RISER_PcieAddrInfo_40.xml,RISER_PcieAddrInfo_46.xml,RISER_PcieAddrInfo_90.xml,RISER_PcieAddrInfo_92.xml,RISER_PcieAddrInfo_93.xml,RISER_PcieAddrInfo_95.xml,RISER_PcieAddrInfo_96.xml,RISER_PcieAddrInfo_97.xml,RISER_PcieAddrInfo_98.xml,RISER_PcieAddrInfo_99.xml,RISER3_CONFIG_00.xml,RISER9_CONFIG_00.xml,SYSPOWER_SENSOR_ff.xml",
            "_eb1.xml,BASEBOARD_00.xml,BC13MBSA_CFG_00.xml,BC13MBSA_COOLING_00.xml,BC13MBSA_FAN_00.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_4b.xml,DISKBP_PcieAddrInfo_4d.xml,DISKBP_PcieAddrInfo_7c.xml,DISKBP_PcieAddrInfo_7d.xml,DISKBP_PcieAddrInfo_8d.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_45.xml,DISKBP_PcieAddrInfo_74.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_82.xml,DISKBP_PcieAddrInfo_83.xml,DISKBP_PcieAddrInfo_88.xml,DPU_EXTRA_01.xml,PcieCardCooling_00.xml,PRODUCT_b1_03.xml,RISER_PcieAddrInfo_3b.xml,RISER_PcieAddrInfo_3c.xml,RISER_PcieAddrInfo_3d.xml,RISER_PcieAddrInfo_3e.xml,RISER_PcieAddrInfo_3f.xml,RISER_PcieAddrInfo_9a.xml,RISER_PcieAddrInfo_9b.xml,RISER_PcieAddrInfo_9c.xml,RISER_PcieAddrInfo_9d.xml,RISER_PcieAddrInfo_9e.xml,RISER_PcieAddrInfo_38.xml,RISER_PcieAddrInfo_39.xml,RISER_PcieAddrInfo_40.xml,RISER_PcieAddrInfo_46.xml,RISER_PcieAddrInfo_90.xml,RISER_PcieAddrInfo_92.xml,RISER_PcieAddrInfo_93.xml,RISER_PcieAddrInfo_95.xml,RISER_PcieAddrInfo_96.xml,RISER_PcieAddrInfo_97.xml,RISER_PcieAddrInfo_98.xml,RISER_PcieAddrInfo_99.xml,RISER3_CONFIG_00.xml,RISER9_CONFIG_00.xml,SYSPOWER_SENSOR_ff.xml",
            "_eb1.xml,BASEBOARD_00.xml,BC13MBSA_CFG_00.xml,BC13MBSA_COOLING_00.xml,BC13MBSA_FAN_00.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_4b.xml,DISKBP_PcieAddrInfo_4d.xml,DISKBP_PcieAddrInfo_7c.xml,DISKBP_PcieAddrInfo_7d.xml,DISKBP_PcieAddrInfo_8d.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_45.xml,DISKBP_PcieAddrInfo_74.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_82.xml,DISKBP_PcieAddrInfo_83.xml,DISKBP_PcieAddrInfo_88.xml,DPU_EXTRA_01.xml,PcieCardCooling_00.xml,RISER_PcieAddrInfo_3b.xml,RISER_PcieAddrInfo_3c.xml,RISER_PcieAddrInfo_3d.xml,RISER_PcieAddrInfo_3e.xml,RISER_PcieAddrInfo_3f.xml,RISER_PcieAddrInfo_9a.xml,RISER_PcieAddrInfo_9b.xml,RISER_PcieAddrInfo_9c.xml,RISER_PcieAddrInfo_9d.xml,RISER_PcieAddrInfo_9e.xml,RISER_PcieAddrInfo_38.xml,RISER_PcieAddrInfo_39.xml,RISER_PcieAddrInfo_40.xml,RISER_PcieAddrInfo_46.xml,RISER_PcieAddrInfo_90.xml,RISER_PcieAddrInfo_92.xml,RISER_PcieAddrInfo_93.xml,RISER_PcieAddrInfo_95.xml,RISER_PcieAddrInfo_96.xml,RISER_PcieAddrInfo_97.xml,RISER_PcieAddrInfo_98.xml,RISER_PcieAddrInfo_99.xml,RISER3_CONFIG_00.xml,RISER9_CONFIG_00.xml,SYSPOWER_SENSOR_ff.xml",
            "_eb1.xml,BASEBOARD_00.xml,BC13MBSA_CFG_00.xml,BC13MBSA_COOLING_00.xml,BC13MBSA_FAN_00.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_4b.xml,DISKBP_PcieAddrInfo_4d.xml,DISKBP_PcieAddrInfo_7c.xml,DISKBP_PcieAddrInfo_7d.xml,DISKBP_PcieAddrInfo_8d.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_45.xml,DISKBP_PcieAddrInfo_74.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_82.xml,DISKBP_PcieAddrInfo_83.xml,DISKBP_PcieAddrInfo_88.xml,DPU_EXTRA_01.xml,PcieCardCooling_00.xml,PRODUCT_b5_02.xml,RISER_PcieAddrInfo_3b.xml,RISER_PcieAddrInfo_3c.xml,RISER_PcieAddrInfo_3d.xml,RISER_PcieAddrInfo_3e.xml,RISER_PcieAddrInfo_3f.xml,RISER_PcieAddrInfo_9a.xml,RISER_PcieAddrInfo_9b.xml,RISER_PcieAddrInfo_9c.xml,RISER_PcieAddrInfo_9d.xml,RISER_PcieAddrInfo_9e.xml,RISER_PcieAddrInfo_38.xml,RISER_PcieAddrInfo_39.xml,RISER_PcieAddrInfo_40.xml,RISER_PcieAddrInfo_46.xml,RISER_PcieAddrInfo_90.xml,RISER_PcieAddrInfo_92.xml,RISER_PcieAddrInfo_93.xml,RISER_PcieAddrInfo_95.xml,RISER_PcieAddrInfo_96.xml,RISER_PcieAddrInfo_97.xml,RISER_PcieAddrInfo_98.xml,RISER_PcieAddrInfo_99.xml,RISER3_CONFIG_00.xml,RISER9_CONFIG_00.xml,SYSPOWER_SENSOR_ff.xml",
            "_eb1.xml,BASEBOARD_00.xml,BC13MBSA_CFG_00.xml,BC13MBSA_COOLING_00.xml,BC13MBSA_FAN_00.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_4b.xml,DISKBP_PcieAddrInfo_4d.xml,DISKBP_PcieAddrInfo_7c.xml,DISKBP_PcieAddrInfo_7d.xml,DISKBP_PcieAddrInfo_8d.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_45.xml,DISKBP_PcieAddrInfo_74.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_82.xml,DISKBP_PcieAddrInfo_83.xml,DISKBP_PcieAddrInfo_88.xml,DPU_EXTRA_01.xml,PcieCardCooling_00.xml,PRODUCT_b5_03.xml,RISER_PcieAddrInfo_3b.xml,RISER_PcieAddrInfo_3c.xml,RISER_PcieAddrInfo_3d.xml,RISER_PcieAddrInfo_3e.xml,RISER_PcieAddrInfo_3f.xml,RISER_PcieAddrInfo_9a.xml,RISER_PcieAddrInfo_9b.xml,RISER_PcieAddrInfo_9c.xml,RISER_PcieAddrInfo_9d.xml,RISER_PcieAddrInfo_9e.xml,RISER_PcieAddrInfo_38.xml,RISER_PcieAddrInfo_39.xml,RISER_PcieAddrInfo_40.xml,RISER_PcieAddrInfo_46.xml,RISER_PcieAddrInfo_90.xml,RISER_PcieAddrInfo_92.xml,RISER_PcieAddrInfo_93.xml,RISER_PcieAddrInfo_95.xml,RISER_PcieAddrInfo_96.xml,RISER_PcieAddrInfo_97.xml,RISER_PcieAddrInfo_98.xml,RISER_PcieAddrInfo_99.xml,RISER3_CONFIG_00.xml,RISER9_CONFIG_00.xml,SYSPOWER_SENSOR_ff.xml",
            "_eb1.xml,BASEBOARD_00.xml,BC13MBSA_CFG_00.xml,BC13MBSA_COOLING_00.xml,BC13MBSA_FAN_00.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_4b.xml,DISKBP_PcieAddrInfo_4d.xml,DISKBP_PcieAddrInfo_7c.xml,DISKBP_PcieAddrInfo_7d.xml,DISKBP_PcieAddrInfo_8d.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_45.xml,DISKBP_PcieAddrInfo_74.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_82.xml,DISKBP_PcieAddrInfo_83.xml,DISKBP_PcieAddrInfo_88.xml,DPU_EXTRA_01.xml,PcieCardCooling_00.xml,RISER_PcieAddrInfo_3b.xml,RISER_PcieAddrInfo_3c.xml,RISER_PcieAddrInfo_3d.xml,RISER_PcieAddrInfo_3e.xml,RISER_PcieAddrInfo_3f.xml,RISER_PcieAddrInfo_9a.xml,RISER_PcieAddrInfo_9b.xml,RISER_PcieAddrInfo_9c.xml,RISER_PcieAddrInfo_9d.xml,RISER_PcieAddrInfo_9e.xml,RISER_PcieAddrInfo_38.xml,RISER_PcieAddrInfo_39.xml,RISER_PcieAddrInfo_40.xml,RISER_PcieAddrInfo_46.xml,RISER_PcieAddrInfo_90.xml,RISER_PcieAddrInfo_92.xml,RISER_PcieAddrInfo_93.xml,RISER_PcieAddrInfo_95.xml,RISER_PcieAddrInfo_96.xml,RISER_PcieAddrInfo_97.xml,RISER_PcieAddrInfo_98.xml,RISER_PcieAddrInfo_99.xml,RISER3_CONFIG_00.xml,RISER9_CONFIG_00.xml,SYSPOWER_SENSOR_ff.xml",
            "_eb1.xml,BASEBOARD_00.xml,BC13MBSA_CFG_00.xml,BC13MBSA_COOLING_00.xml,BC13MBSA_FAN_00.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_4b.xml,DISKBP_PcieAddrInfo_4d.xml,DISKBP_PcieAddrInfo_7c.xml,DISKBP_PcieAddrInfo_7d.xml,DISKBP_PcieAddrInfo_8d.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_45.xml,DISKBP_PcieAddrInfo_74.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_82.xml,DISKBP_PcieAddrInfo_83.xml,DISKBP_PcieAddrInfo_88.xml,DPU_EXTRA_01.xml,PcieCardCooling_00.xml,PRODUCT_b1_02.xml,RISER_PcieAddrInfo_3b.xml,RISER_PcieAddrInfo_3c.xml,RISER_PcieAddrInfo_3d.xml,RISER_PcieAddrInfo_3e.xml,RISER_PcieAddrInfo_3f.xml,RISER_PcieAddrInfo_9a.xml,RISER_PcieAddrInfo_9b.xml,RISER_PcieAddrInfo_9c.xml,RISER_PcieAddrInfo_9d.xml,RISER_PcieAddrInfo_9e.xml,RISER_PcieAddrInfo_38.xml,RISER_PcieAddrInfo_39.xml,RISER_PcieAddrInfo_40.xml,RISER_PcieAddrInfo_46.xml,RISER_PcieAddrInfo_90.xml,RISER_PcieAddrInfo_92.xml,RISER_PcieAddrInfo_93.xml,RISER_PcieAddrInfo_95.xml,RISER_PcieAddrInfo_96.xml,RISER_PcieAddrInfo_97.xml,RISER_PcieAddrInfo_98.xml,RISER_PcieAddrInfo_99.xml,RISER3_CONFIG_01.xml,RISER9_CONFIG_00.xml,SYSPOWER_SENSOR_ff.xml",
            "_eb5.xml,BASEBOARD_00.xml,BC13MBSA_CFG_00.xml,BC13MBSA_COOLING_00.xml,BC13MBSA_FAN_00.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_4b.xml,DISKBP_PcieAddrInfo_4d.xml,DISKBP_PcieAddrInfo_7c.xml,DISKBP_PcieAddrInfo_7d.xml,DISKBP_PcieAddrInfo_8d.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_45.xml,DISKBP_PcieAddrInfo_74.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_82.xml,DISKBP_PcieAddrInfo_83.xml,DISKBP_PcieAddrInfo_88.xml,DPU_EXTRA_01.xml,PcieCardCooling_00.xml,PRODUCT_b1_02.xml,RISER_PcieAddrInfo_3b.xml,RISER_PcieAddrInfo_3c.xml,RISER_PcieAddrInfo_3d.xml,RISER_PcieAddrInfo_3e.xml,RISER_PcieAddrInfo_3f.xml,RISER_PcieAddrInfo_9a.xml,RISER_PcieAddrInfo_9b.xml,RISER_PcieAddrInfo_9c.xml,RISER_PcieAddrInfo_9d.xml,RISER_PcieAddrInfo_9e.xml,RISER_PcieAddrInfo_38.xml,RISER_PcieAddrInfo_39.xml,RISER_PcieAddrInfo_40.xml,RISER_PcieAddrInfo_46.xml,RISER_PcieAddrInfo_90.xml,RISER_PcieAddrInfo_92.xml,RISER_PcieAddrInfo_93.xml,RISER_PcieAddrInfo_95.xml,RISER_PcieAddrInfo_96.xml,RISER_PcieAddrInfo_97.xml,RISER_PcieAddrInfo_98.xml,RISER_PcieAddrInfo_99.xml,RISER3_CONFIG_00.xml,RISER9_CONFIG_00.xml,SYSPOWER_SENSOR_ff.xml",
            "_ffb5.xml,BASEBOARD_00.xml,BC13MBSA_CFG_00.xml,BC13MBSA_COOLING_00.xml,BC13MBSA_FAN_00.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_4b.xml,DISKBP_PcieAddrInfo_4d.xml,DISKBP_PcieAddrInfo_7c.xml,DISKBP_PcieAddrInfo_7d.xml,DISKBP_PcieAddrInfo_8d.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_45.xml,DISKBP_PcieAddrInfo_74.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_82.xml,DISKBP_PcieAddrInfo_83.xml,DISKBP_PcieAddrInfo_88.xml,DPU_EXTRA_01.xml,PcieCardCooling_00.xml,PRODUCT_b1_02.xml,RISER_PcieAddrInfo_3b.xml,RISER_PcieAddrInfo_3c.xml,RISER_PcieAddrInfo_3d.xml,RISER_PcieAddrInfo_3e.xml,RISER_PcieAddrInfo_3f.xml,RISER_PcieAddrInfo_9a.xml,RISER_PcieAddrInfo_9b.xml,RISER_PcieAddrInfo_9c.xml,RISER_PcieAddrInfo_9d.xml,RISER_PcieAddrInfo_9e.xml,RISER_PcieAddrInfo_38.xml,RISER_PcieAddrInfo_39.xml,RISER_PcieAddrInfo_40.xml,RISER_PcieAddrInfo_46.xml,RISER_PcieAddrInfo_90.xml,RISER_PcieAddrInfo_92.xml,RISER_PcieAddrInfo_93.xml,RISER_PcieAddrInfo_95.xml,RISER_PcieAddrInfo_96.xml,RISER_PcieAddrInfo_97.xml,RISER_PcieAddrInfo_98.xml,RISER_PcieAddrInfo_99.xml,RISER3_CONFIG_00.xml,RISER9_CONFIG_00.xml,SYSPOWER_SENSOR_ff.xml"
        ], 
		'PangeaV6_Arctic':[
		"_d90.xml,BASEBOARD_90.xml,ADC_90.xml,ADC_ff.xml,ARCard_00.xml,BackPlane_90.xml,BBUModule_90.xml,COOLING_90.xml,CX4_90.xml,EXPBOARD_11.xml,FAN_90.xml,FAN_90_11.xml,FANBOARD_90.xml,FANEXPBOARD_90.xml,POWERMGNT_90.xml,PSevent_90.xml,SAS_cpld_01.xml,VRD_06.xml",
		"_d90.xml,BASEBOARD_90.xml,ADC_90.xml,ADC_ff.xml,ARCard_00.xml,BackPlane_90.xml,BBUModule_90.xml,COOLING_90.xml,CX4_90.xml,EXPBOARD_11.xml,EXPBOARD_PG_11.xml,FAN_90.xml,FAN_90_11.xml,FANBOARD_90.xml,FANEXPBOARD_90.xml,POWERMGNT_90.xml,PSevent_90.xml,SAS_cpld_01.xml,VRD_06.xml",	
		"_d93.xml,BASEBOARD_90.xml,ADC_90.xml,ADC_ff.xml,ARCard_00.xml,BackPlane_90.xml,BBUModule_90.xml,COOLING_90.xml,CX4_90.xml,EXPBOARD_12.xml,FAN_90.xml,FAN_90_12.xml,FANBOARD_90.xml,FANEXPBOARD_90.xml,POWERMGNT_90.xml,PSevent_90.xml,SAS_cpld_01.xml,VRD_06.xml",
		"_d93.xml,BASEBOARD_90.xml,ADC_90.xml,ADC_ff.xml,ARCard_00.xml,BackPlane_90.xml,BBUModule_90.xml,COOLING_90.xml,CX4_90.xml,EXPBOARD_12.xml,FAN_90.xml,FAN_90_12.xml,FANBOARD_90.xml,FANEXPBOARD_90.xml,POWERMGNT_90.xml,PSevent_90.xml,SAS_cpld_01.xml,VRD_06.xml",
		"_d90.xml,BASEBOARD_90_01.xml,ADC_90.xml,ADC_ff.xml,ARCard_00.xml,BackPlane_90_01.xml,BBUModule_90.xml,COOLING_90_01.xml,CX4_90.xml,EXPBOARD_11.xml,FANBOARD_90.xml,FANEXPBOARD_90.xml,POWERMGNT_90.xml,PSevent_90.xml,SAS_cpld_01.xml,VRD_06.xml",
		"_d90.xml,BASEBOARD_90_01.xml,ADC_90.xml,ADC_ff.xml,ARCard_00.xml,BackPlane_90_01.xml,BBUModule_90.xml,COOLING_90_01.xml,CX4_90.xml,EXPBOARD_11.xml,EXPBOARD_PG_11.xml,FANBOARD_90.xml,FANEXPBOARD_90.xml,POWERMGNT_90.xml,PSevent_90.xml,SAS_cpld_01.xml,VRD_06.xml",
		"_d93.xml,BASEBOARD_90_01.xml,ADC_90.xml,ADC_ff.xml,ARCard_00.xml,BackPlane_90_01.xml,BBUModule_90.xml,COOLING_90_01.xml,CX4_90.xml,EXPBOARD_12.xml,FANBOARD_90.xml,FANEXPBOARD_90.xml,POWERMGNT_90.xml,PSevent_90.xml,SAS_cpld_01.xml,VRD_06.xml",
		"_d93.xml,BASEBOARD_90_01.xml,ADC_90.xml,ADC_ff.xml,ARCard_00.xml,BackPlane_90_01.xml,BBUModule_90.xml,COOLING_90_01.xml,CX4_90.xml,EXPBOARD_13.xml,EXPBOARD_PG_13.xml,FANBOARD_90.xml,FANEXPBOARD_90.xml,POWERMGNT_90.xml,PSevent_90.xml,SAS_cpld_01.xml,VRD_06.xml",
		],
		'PangeaV6_Atlantic':[
		"_d1b.xml,ADC_ff.xml,BackPlane_1b_01.xml,BASEBOARD_1b.xml,BBUModule_1b.xml,CLOCK_00.xml,COOLING_1b.xml,FAN_1b.xml,FANBOARD_1b.xml,PSevent_1b.xml,VRD_00.xml", 
		"_d1b.xml,ADC_ff.xml,BackPlane_1b_01.xml,BASEBOARD_1b.xml,BBUModule_1b.xml,CLOCK_01.xml,COOLING_1b.xml,FAN_1b.xml,FANBOARD_1b.xml,PSevent_1b.xml,VRD_01.xml", 
		"_d1b.xml,ADC_ff.xml,BackPlane_1b_01.xml,BASEBOARD_1b.xml,BBUModule_1b.xml,CLOCK_00.xml,COOLING_1b.xml,FAN_1b.xml,FANBOARD_1b.xml,PSevent_1b.xml,VRD_02.xml", 
		"_d1b.xml,ADC_ff.xml,BackPlane_1b_01.xml,BASEBOARD_1b_01.xml,BBUModule_1b.xml,CLOCK_00.xml,COOLING_1b_01.xml,FAN_1b.xml,FANBOARD_1b.xml,PSevent_1b.xml,VRD_00.xml"
		],
		'PangeaV6_Atlantic_Smm':[
        "_d30.xml,ADC_30.xml,ADC_CDR_00.xml,ADC_CLOCK_30.xml,BackPlane_30.xml,BASEBOARD_30.xml,CLOCK_01.xml,COOLING_30.xml,FANBOARD_30.xml,POWERMGNT_30.xml,PSevent_30.xml,PSevent_CDR_00.xml,RETIMER_00.xml",
        "_d30.xml,ADC_30.xml,ADC_CDR_01.xml,ADC_CLOCK_30.xml,BackPlane_30_01.xml,BASEBOARD_30_01.xml,CLOCK_01.xml,COOLING_30.xml,FANBOARD_30.xml,POWERMGNT_30.xml,PSevent_30.xml,PSevent_CDR_01.xml,RETIMER_01.xml",
        "_d30.xml,ADC_30.xml,ADC_CDR_00.xml,ADC_CLOCK_30.xml,BackPlane_30_01.xml,BASEBOARD_30_01.xml,CLOCK_01.xml,COOLING_30.xml,FANBOARD_30.xml,POWERMGNT_30.xml,PSevent_30.xml,PSevent_CDR_00.xml,RETIMER_01.xml",
        "_d30.xml,BackPlane_30_01.xml,BASEBOARD_30_02.xml,CLOCK_01.xml,COOLING_30.xml,FANBOARD_30.xml,POWERMGNT_30.xml"
		],
		'PangeaV6_Pacific':[
        "_d17.xml,ADC_ff.xml,BackPlane_17.xml,BASEBOARD_17.xml,BBUModule_17.xml,COOLING_17.xml,CLOCK_00.xml,EXPBOARD_00.xml,EXPBOARD_EXTRA_07.xml,FANBOARD_17.xml,POWERMGNT_17.xml,PSevent_ff.xml,VRD_00.xml",
		"_d17.xml,ADC_ff.xml,BackPlane_17.xml,BASEBOARD_17.xml,BBUModule_17.xml,COOLING_17.xml,CLOCK_01.xml,EXPBOARD_00.xml,EXPBOARD_EXTRA_07.xml,FANBOARD_17.xml,POWERMGNT_17.xml,PSevent_ff.xml,VRD_01.xml",
		"_d17.xml,ADC_ff.xml,BackPlane_17.xml,BASEBOARD_17.xml,BBUModule_17.xml,COOLING_17.xml,CLOCK_02.xml,EXPBOARD_00.xml,EXPBOARD_EXTRA_07.xml,FANBOARD_17.xml,POWERMGNT_17.xml,PSevent_ff.xml,VRD_02.xml",
		"_d17.xml,ADC_ff.xml,BackPlane_17.xml,BASEBOARD_17_01.xml,COOLING_17_01.xml,CLOCK_00.xml,PSevent_ff.xml,VRD_00.xml"
		],
        'PAC1220V6':[
        "_df3.xml,BASEBOARD_01.xml,BBUModule_f3.xml,BackPlane_f3.xml,POWERMGNT_f3.xml,PSevent_ff.xml,CLOCK_f3.xml,EXPBOARD_00.xml,EXPBOARD_EXTRA_0a.xml,VRD_01.xml,VRD_02.xml",
        "_df3.xml,BASEBOARD_01.xml,BBUModule_f3.xml,BackPlane_f3.xml,POWERMGNT_f3.xml,PSevent_ff.xml,CLOCK_f3.xml,EXPBOARD_00.xml,EXPBOARD_EXTRA_0a.xml,VRD_01.xml,VRD_01_01.xml,VRD_01_02.xml",
        "_df3.xml,BASEBOARD_01.xml,BBUModule_f3.xml,BackPlane_f3.xml,POWERMGNT_f3.xml,PSevent_ff.xml,CLOCK_f3.xml,EXPBOARD_00.xml,EXPBOARD_EXTRA_0a.xml,VRD_02.xml,VRD_02_01.xml,VRD_02_02.xml",
        "_df3.xml,BASEBOARD_01.xml,BBUModule_f3.xml,BackPlane_f3.xml,POWERMGNT_f3.xml,PSevent_ff.xml",
		],
        'DA121Cv2':[
            "_709.xml,BASEBOARD_09.xml,ADC_09.xml,CustomizeSensor.xml,PcieAddrInfo_ff.xml,PRODUCT_00.xml,CLOCK_00.xml,COOLING_00.xml,EQUIP_00.xml,FAN_09.xml,Disk_00.xml,PSevent_09.xml,VRD_00.xml,VRD1601_01.xml,PSUCustomizeSensor.xml"
        ],
        'DA122Cv2':[
        "_ff.xml,BASEBOARD_ff.xml,COOLING_00.xml,CustomizeSensor.xml,PRODUCT_FF_00.xml,M2_SATA_SSD_00.xml,Cable_a1.xml,Eth_a1.xml,InletOutlet_a1.xml,Serial_a1.xml"
        ],
        'Atlas200I_SoC_A1':["_203.xml,BASEBOARD_03.xml,PRODUCT_00.xml,FAN_03.xml,ADC_03.xml,PSevent_03.xml,COOLING_00.xml"],
        'S902X20':[
        "_af3.xml,ADC_f3.xml,BASEBOARD_f3.xml,Physical_f3.xml,CHASSIS_INTRUSION_00.xml,BC13MBSA_OCP_NCSI_00.xml,CLOCK_00.xml,COOLING_00.xml,CustomizeSensor.xml,FAN_f3.xml,SPSCA_4U_00.xml,PRODUCT_00.xml,PSevent_f3.xml,PSUCustomizeSensor.xml,UPGRADE_CPLD_00.xml,VRD_00.xml",
		"_af3.xml,ADC_f3.xml,BASEBOARD_f3.xml,Physical_f3.xml,CHASSIS_INTRUSION_00.xml,BC13MBSA_OCP_NCSI_00.xml,CLOCK_00.xml,COOLING_00.xml,CustomizeSensor.xml,FAN_f3.xml,SPSCA_4U_00.xml,PRODUCT_00.xml,PSevent_f3.xml,PSUCustomizeSensor.xml,UPGRADE_CPLD_00.xml,VRD_02.xml",
		"_af3.xml,ADC_f3.xml,BASEBOARD_f3.xml,Physical_f3.xml,CHASSIS_INTRUSION_00.xml,BC13MBSA_OCP_NCSI_00.xml,CLOCK_00.xml,COOLING_00.xml,CustomizeSensor.xml,FAN_f3.xml,SPSCA_4U_00.xml,PRODUCT_01.xml,PSevent_f3.xml,PSUCustomizeSensor.xml,UPGRADE_CPLD_00.xml,VRD_00.xml",
        "_af3.xml,ADC_f3.xml,BASEBOARD_f3.xml,Physical_f3.xml,CHASSIS_INTRUSION_00.xml,BC13MBSA_OCP_NCSI_00.xml,CLOCK_00.xml,COOLING_00.xml,CustomizeSensor.xml,FAN_f3.xml,SPSCA_4U_00.xml,PRODUCT_01.xml,PSevent_f3.xml,PSUCustomizeSensor.xml,UPGRADE_CPLD_00.xml,VRD_02.xml"
        ],
        'Atlas800D_G1':["_201.xml,BASEBOARD_01.xml,ADC_01.xml,COOLING_00.xml,FAN_01.xml,PSevent_01.xml,PCIeAddrInfo_ff.xml,CustomizeSensor.xml,PRODUCT_00.xml,PSUCustomizeSensor.xml,Riser1_ff.xml,LSW_00.xml,DFT_Tools_Board_00.xml,FRUID_ADJUST_01.xml"],
        'Atlas800D_RM':["_2be.xml,BASEBOARD_01.xml,CustomizeSensor.xml,PRODUCT_00.xml,PSUCustomizeSensor.xml,LSW_00.xml,DFT_Tools_Board_00.xml,FRUID_ADJUST_01.xml"],
        'ENC0210V6':[
            "_df5.xml,BASEBOARD_f5.xml,BBUModule_f5.xml,BackPlane_f5.xml,PSevent_ff.xml"
		],
        'SMM0001V6':[
            "_daf.xml,ADC_af.xml,BackPlane_af.xml,BASEBOARD_af.xml,CLOCK_af.xml,COOLING_af.xml,FANBOARD_af.xml,POWERMGNT_af.xml,NETWORK_CONFIG_00.xml,Equipment_af_01.xml,HddBackPlane_0a.xml",
            "_daf.xml,ADC_af.xml,BackPlane_af.xml,BASEBOARD_af.xml,CLOCK_af.xml,COOLING_af.xml,FANBOARD_af.xml,POWERMGNT_af.xml,NETWORK_CONFIG_01.xml,Equipment_af_02.xml,HddBackPlane_0a.xml",
            "_daf.xml,ADC_af.xml,BackPlane_af_01.xml,BASEBOARD_af.xml,CLOCK_af.xml,COOLING_af.xml,FANBOARD_af.xml,POWERMGNT_af.xml,NETWORK_CONFIG_00.xml,Equipment_af_01.xml,HddBackPlane_0a.xml",
            "_daf.xml,ADC_af.xml,BackPlane_af_01.xml,BASEBOARD_af.xml,CLOCK_af.xml,COOLING_af.xml,FANBOARD_af.xml,POWERMGNT_af.xml,NETWORK_CONFIG_00.xml,Equipment_af_02.xml,HddBackPlane_0a.xml",
            "_daf.xml,ADC_af.xml,BackPlane_af_01.xml,BASEBOARD_af.xml,CLOCK_af.xml,COOLING_af.xml,FANBOARD_af.xml,POWERMGNT_af.xml,NETWORK_CONFIG_01.xml,Equipment_af_01.xml,HddBackPlane_0a.xml",
            "_daf.xml,ADC_af.xml,BackPlane_af_01.xml,BASEBOARD_af.xml,CLOCK_af.xml,COOLING_af.xml,FANBOARD_af.xml,POWERMGNT_af.xml,NETWORK_CONFIG_01.xml,Equipment_af_02.xml,HddBackPlane_0a.xml",
		],
        'CST0220V6':[
            "_da6.xml,ADC_a6.xml,BASEBOARD_a6.xml,CLOCK_a6.xml,PRODUCT_a6.xml,COOLING_PUBLIC_a6.xml,CustomizeSensor.xml,FAN_a6.xml,HI1823_a6.xml,SYSPOWER_SENSOR_ff.xml,VRD_02.xml,COOLING_NODE_00.xml,NETWORK_CONFIG_00.xml,HalfPalm_a6.xml",
            "_da6.xml,ADC_a6.xml,BASEBOARD_a6.xml,CLOCK_a6.xml,PRODUCT_a6.xml,COOLING_PUBLIC_a6.xml,CustomizeSensor.xml,FAN_a6.xml,HI1823_a6.xml,SYSPOWER_SENSOR_ff.xml,VRD_02.xml,COOLING_NODE_02.xml,NETWORK_CONFIG_00.xml,HalfPalm_a6.xml",
            "_da6.xml,ADC_a6.xml,BASEBOARD_a6.xml,CLOCK_a6.xml,PRODUCT_a6.xml,COOLING_PUBLIC_a6.xml,CustomizeSensor.xml,FAN_a6.xml,HI1823_a6.xml,SYSPOWER_SENSOR_ff.xml,VRD_02.xml,COOLING_NODE_00.xml,NETWORK_CONFIG_01.xml,HalfPalm_a6.xml",
            "_da6.xml,ADC_a6.xml,BASEBOARD_a6.xml,CLOCK_a6.xml,PRODUCT_a6.xml,COOLING_PUBLIC_a6.xml,CustomizeSensor.xml,FAN_a6.xml,HI1823_a6.xml,SYSPOWER_SENSOR_ff.xml,VRD_02.xml,COOLING_NODE_02.xml,NETWORK_CONFIG_01.xml,HalfPalm_a6.xml",
            "_da6.xml,ADC_a6.xml,BASEBOARD_a6.xml,CLOCK_a6.xml,PRODUCT_a6.xml,COOLING_PUBLIC_a6.xml,CustomizeSensor.xml,FAN_a6.xml,HI1823_a6.xml,SYSPOWER_SENSOR_ff.xml,VRD_02.xml,COOLING_NODE_00.xml,NETWORK_CONFIG_02.xml,HalfPalm_a6.xml",
            "_da6.xml,ADC_a6.xml,BASEBOARD_a6.xml,CLOCK_a6.xml,PRODUCT_a6.xml,COOLING_PUBLIC_a6.xml,CustomizeSensor.xml,FAN_a6.xml,HI1823_a6.xml,SYSPOWER_SENSOR_ff.xml,VRD_02.xml,COOLING_NODE_02.xml,NETWORK_CONFIG_02.xml,HalfPalm_a6.xml",
        ],
        'CST0221V6':[
            "_da9.xml,ADC_a9.xml,BASEBOARD_a9.xml,CLOCK_a9.xml,COOLING_NODE_02.xml,COOLING_PUBLIC_a9.xml,DiskModule_a9.xml,FAN_a9.xml,HI1823_a9.xml,SYSPOWER_SENSOR_ff.xml,VRD_02.xml,NETWORK_CONFIG_00.xml,Equipment_a9_01.xml,HDD_a9.xml,BBUModule_a9.xml,PangeaCardModule_a9.xml",
            "_da9.xml,ADC_a9.xml,BASEBOARD_a9.xml,CLOCK_a9.xml,COOLING_NODE_02.xml,COOLING_PUBLIC_a9.xml,DiskModule_a9.xml,FAN_a9.xml,HI1823_a9.xml,SYSPOWER_SENSOR_ff.xml,VRD_02.xml,NETWORK_CONFIG_01.xml,Equipment_a9_01.xml,HDD_a9.xml,BBUModule_a9.xml,PangeaCardModule_a9.xml",
            "_da9.xml,ADC_a9.xml,BASEBOARD_a9.xml,CLOCK_a9.xml,COOLING_NODE_02.xml,COOLING_PUBLIC_a9.xml,DiskModule_a9.xml,FAN_a9.xml,HI1823_a9.xml,SYSPOWER_SENSOR_ff.xml,VRD_02.xml,NETWORK_CONFIG_02.xml,Equipment_a9_01.xml,HDD_a9.xml,BBUModule_a9.xml,PangeaCardModule_a9.xml",
        ],
        'CST1020V6':[
            "_d9b.xml,BASEBOARD_9c.xml,BC13MBSAA_CFG_00.xml,COOLING_NODE_00.xml,COOLING_PUBLIC_9c.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_8c.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_cb.xml,FAN_9c.xml,SYSPOWER_SENSOR_ff.xml,Equipment_9c.xml",
            "_d9b.xml,BASEBOARD_9c.xml,BC13MBSAA_CFG_00.xml,COOLING_NODE_02.xml,COOLING_PUBLIC_9c.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_8c.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_cb.xml,FAN_9c.xml,SYSPOWER_SENSOR_ff.xml,Equipment_9c.xml",
            "_d9b.xml,BASEBOARD_9c.xml,BC13MBSAA_CFG_01.xml,COOLING_NODE_00.xml,COOLING_PUBLIC_9c.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_8c.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_cb.xml,FAN_9c.xml,SYSPOWER_SENSOR_ff.xml,Equipment_9c.xml",
            "_d9b.xml,BASEBOARD_9c.xml,BC13MBSAA_CFG_01.xml,COOLING_NODE_02.xml,COOLING_PUBLIC_9c.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_8c.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_cb.xml,FAN_9c.xml,SYSPOWER_SENSOR_ff.xml,Equipment_9c.xml",
            "_d9c.xml,BASEBOARD_9c.xml,BC13MBSAA_CFG_00.xml,COOLING_NODE_00.xml,COOLING_PUBLIC_9c.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_8c.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_cb.xml,FAN_9c.xml,SYSPOWER_SENSOR_ff.xml,Equipment_9c.xml",
            "_d9c.xml,BASEBOARD_9c.xml,BC13MBSAA_CFG_00.xml,COOLING_NODE_02.xml,COOLING_PUBLIC_9c.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_8c.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_cb.xml,FAN_9c.xml,SYSPOWER_SENSOR_ff.xml,Equipment_9c.xml",
            "_d9c.xml,BASEBOARD_9c.xml,BC13MBSAA_CFG_01.xml,COOLING_NODE_00.xml,COOLING_PUBLIC_9c.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_8c.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_cb.xml,FAN_9c.xml,SYSPOWER_SENSOR_ff.xml,Equipment_9c.xml",
            "_d9c.xml,BASEBOARD_9c.xml,BC13MBSAA_CFG_01.xml,COOLING_NODE_02.xml,COOLING_PUBLIC_9c.xml,CustomizeSensor.xml,DISKBP_PcieAddrInfo_8c.xml,DISKBP_PcieAddrInfo_8e.xml,DISKBP_PcieAddrInfo_81.xml,DISKBP_PcieAddrInfo_cb.xml,FAN_9c.xml,SYSPOWER_SENSOR_ff.xml,Equipment_9c.xml",
        ],
        'CST0210V6':[
            "_da3.xml,ADC_ff.xml,BackPlane_a3.xml,BASEBOARD_a3.xml,BBUModule_a3.xml,CLOCK_a3.xml,COOLING_a3.xml,DiskModule_ff.xml,FANBOARD_a3.xml,Equipment_a3_01.xml,VRD_00.xml,POWERMGNT_a3.xml",
            "_da3.xml,ADC_ff.xml,BackPlane_a3.xml,BASEBOARD_a3.xml,BBUModule_a3.xml,CLOCK_a3.xml,COOLING_a3.xml,DiskModule_ff.xml,FANBOARD_a3.xml,Equipment_a3_01.xml,VRD_02.xml,POWERMGNT_a3.xml",
            "_da3.xml,ADC_ff.xml,BackPlane_a3.xml,BASEBOARD_a3.xml,BBUModule_a3.xml,CLOCK_a3.xml,COOLING_a3.xml,DiskModule_ff.xml,FANBOARD_a3.xml,Equipment_a3_02.xml,VRD_00.xml,POWERMGNT_a3.xml",
            "_da3.xml,ADC_ff.xml,BackPlane_a3.xml,BASEBOARD_a3.xml,BBUModule_a3.xml,CLOCK_a3.xml,COOLING_a3.xml,DiskModule_ff.xml,FANBOARD_a3.xml,Equipment_a3_02.xml,VRD_02.xml,POWERMGNT_a3.xml",
            "_da3.xml,ADC_ff.xml,BackPlane_a3.xml,BASEBOARD_a3.xml,BBUModule_a3.xml,CLOCK_a3.xml,COOLING_a3_01.xml,DiskModule_ff.xml,FANBOARD_a3.xml,Equipment_a3_01.xml,VRD_00.xml,POWERMGNT_a3.xml",
            "_da3.xml,ADC_ff.xml,BackPlane_a3.xml,BASEBOARD_a3.xml,BBUModule_a3.xml,CLOCK_a3.xml,COOLING_a3_01.xml,DiskModule_ff.xml,FANBOARD_a3.xml,Equipment_a3_01.xml,VRD_02.xml,POWERMGNT_a3.xml",
            "_da3.xml,ADC_ff.xml,BackPlane_a3.xml,BASEBOARD_a3.xml,BBUModule_a3.xml,CLOCK_a3.xml,COOLING_a3_01.xml,DiskModule_ff.xml,FANBOARD_a3.xml,Equipment_a3_02.xml,VRD_00.xml,POWERMGNT_a3.xml",
            "_da3.xml,ADC_ff.xml,BackPlane_a3.xml,BASEBOARD_a3.xml,BBUModule_a3.xml,CLOCK_a3.xml,COOLING_a3_01.xml,DiskModule_ff.xml,FANBOARD_a3.xml,Equipment_a3_02.xml,VRD_02.xml,POWERMGNT_a3.xml",
            "_da3.xml,ADC_ff.xml,BackPlane_a3_01.xml,BASEBOARD_a3.xml,BBUModule_a3.xml,CLOCK_a3.xml,COOLING_a3.xml,DiskModule_ff.xml,FANBOARD_a3.xml,Equipment_a3_01.xml,VRD_00.xml,POWERMGNT_a3.xml",
            "_da3.xml,ADC_ff.xml,BackPlane_a3_01.xml,BASEBOARD_a3.xml,BBUModule_a3.xml,CLOCK_a3.xml,COOLING_a3.xml,DiskModule_ff.xml,FANBOARD_a3.xml,Equipment_a3_01.xml,VRD_02.xml,POWERMGNT_a3.xml",
            "_da3.xml,ADC_ff.xml,BackPlane_a3_01.xml,BASEBOARD_a3.xml,BBUModule_a3.xml,CLOCK_a3.xml,COOLING_a3.xml,DiskModule_ff.xml,FANBOARD_a3.xml,Equipment_a3_02.xml,VRD_00.xml,POWERMGNT_a3.xml",
            "_da3.xml,ADC_ff.xml,BackPlane_a3_01.xml,BASEBOARD_a3.xml,BBUModule_a3.xml,CLOCK_a3.xml,COOLING_a3.xml,DiskModule_ff.xml,FANBOARD_a3.xml,Equipment_a3_02.xml,VRD_02.xml,POWERMGNT_a3.xml",
            "_da3.xml,ADC_ff.xml,BackPlane_a3_01.xml,BASEBOARD_a3.xml,BBUModule_a3.xml,CLOCK_a3.xml,COOLING_a3_01.xml,DiskModule_ff.xml,FANBOARD_a3.xml,Equipment_a3_01.xml,VRD_00.xml,POWERMGNT_a3.xml",
            "_da3.xml,ADC_ff.xml,BackPlane_a3_01.xml,BASEBOARD_a3.xml,BBUModule_a3.xml,CLOCK_a3.xml,COOLING_a3_01.xml,DiskModule_ff.xml,FANBOARD_a3.xml,Equipment_a3_01.xml,VRD_02.xml,POWERMGNT_a3.xml",
            "_da3.xml,ADC_ff.xml,BackPlane_a3_01.xml,BASEBOARD_a3.xml,BBUModule_a3.xml,CLOCK_a3.xml,COOLING_a3_01.xml,DiskModule_ff.xml,FANBOARD_a3.xml,Equipment_a3_02.xml,VRD_00.xml,POWERMGNT_a3.xml",
            "_da3.xml,ADC_ff.xml,BackPlane_a3_01.xml,BASEBOARD_a3.xml,BBUModule_a3.xml,CLOCK_a3.xml,COOLING_a3_01.xml,DiskModule_ff.xml,FANBOARD_a3.xml,Equipment_a3_02.xml,VRD_02.xml,POWERMGNT_a3.xml",
        ],
        'CST1220V6':[
            "_d9f.xml,ADC_9f.xml,BASEBOARD_9f.xml,CLOCK_9f.xml,COOLING_NODE_00.xml,COOLING_PUBLIC_9f.xml,CustomizeSensor.xml,FAN_9f.xml,PCIeAddrInfo_01.xml,PCIeAddrInfo_02.xml,PCIeAddrInfo_ff.xml,PRODUCT_9f.xml,PSevent_9f.xml,PSUCustomizeSensor.xml,RAIDSASCable_00.xml,UPGRADE_CPLD_00.xml",
            "_d9f.xml,ADC_9f.xml,BASEBOARD_9f.xml,CLOCK_9f.xml,COOLING_NODE_00.xml,COOLING_PUBLIC_9f.xml,CustomizeSensor.xml,FAN_9f.xml,PCIeAddrInfo_01.xml,PCIeAddrInfo_02.xml,PCIeAddrInfo_ff.xml,PRODUCT_9f.xml,PSevent_9f.xml,PSUCustomizeSensor.xml,RAIDSASCable_00.xml,UPGRADE_CPLD_01.xml",
            "_d9f.xml,ADC_9f.xml,BASEBOARD_9f.xml,CLOCK_9f.xml,COOLING_NODE_00.xml,COOLING_PUBLIC_9f.xml,CustomizeSensor.xml,FAN_9f.xml,PCIeAddrInfo_01.xml,PCIeAddrInfo_02.xml,PCIeAddrInfo_ff.xml,PRODUCT_9f.xml,PSevent_9f.xml,PSUCustomizeSensor.xml,RAIDSASCable_01.xml,UPGRADE_CPLD_00.xml",
            "_d9f.xml,ADC_9f.xml,BASEBOARD_9f.xml,CLOCK_9f.xml,COOLING_NODE_00.xml,COOLING_PUBLIC_9f.xml,CustomizeSensor.xml,FAN_9f.xml,PCIeAddrInfo_01.xml,PCIeAddrInfo_02.xml,PCIeAddrInfo_ff.xml,PRODUCT_9f.xml,PSevent_9f.xml,PSUCustomizeSensor.xml,RAIDSASCable_01.xml,UPGRADE_CPLD_01.xml",
            "_d9f.xml,ADC_9f.xml,BASEBOARD_9f.xml,CLOCK_9f.xml,COOLING_NODE_02.xml,COOLING_PUBLIC_9f.xml,CustomizeSensor.xml,FAN_9f.xml,PCIeAddrInfo_01.xml,PCIeAddrInfo_02.xml,PCIeAddrInfo_ff.xml,PRODUCT_9f.xml,PSevent_9f.xml,PSUCustomizeSensor.xml,RAIDSASCable_00.xml,UPGRADE_CPLD_00.xml",
            "_d9f.xml,ADC_9f.xml,BASEBOARD_9f.xml,CLOCK_9f.xml,COOLING_NODE_02.xml,COOLING_PUBLIC_9f.xml,CustomizeSensor.xml,FAN_9f.xml,PCIeAddrInfo_01.xml,PCIeAddrInfo_02.xml,PCIeAddrInfo_ff.xml,PRODUCT_9f.xml,PSevent_9f.xml,PSUCustomizeSensor.xml,RAIDSASCable_00.xml,UPGRADE_CPLD_01.xml",
            "_d9f.xml,ADC_9f.xml,BASEBOARD_9f.xml,CLOCK_9f.xml,COOLING_NODE_02.xml,COOLING_PUBLIC_9f.xml,CustomizeSensor.xml,FAN_9f.xml,PCIeAddrInfo_01.xml,PCIeAddrInfo_02.xml,PCIeAddrInfo_ff.xml,PRODUCT_9f.xml,PSevent_9f.xml,PSUCustomizeSensor.xml,RAIDSASCable_01.xml,UPGRADE_CPLD_00.xml",
            "_d9f.xml,ADC_9f.xml,BASEBOARD_9f.xml,CLOCK_9f.xml,COOLING_NODE_02.xml,COOLING_PUBLIC_9f.xml,CustomizeSensor.xml,FAN_9f.xml,PCIeAddrInfo_01.xml,PCIeAddrInfo_02.xml,PCIeAddrInfo_ff.xml,PRODUCT_9f.xml,PSevent_9f.xml,PSUCustomizeSensor.xml,RAIDSASCable_01.xml,UPGRADE_CPLD_01.xml",
        ]
	}
    
    #只有在所有的组合中都孤立的对象，才报错。
    def merge_lists(self, ownerErrList):
        mergeRst = ownerErrList[0]

        for ownerErr in ownerErrList[1:]:
            for owner, errMsgList in list(mergeRst.items()):
                if not owner in ownerErr:
                    del mergeRst[owner]
                    continue
                delmsgList = []
                for errMsg in errMsgList:
                    if not errMsg in ownerErr[owner]:
                        delmsgList.append(errMsg)
                
                for delmsg in delmsgList:
                    mergeRst[owner].remove(delmsg)

                if len(mergeRst[owner]) == 0:
                    del mergeRst[owner]

        return mergeRst
    

    def doIt(self, cmd_index, pdt_num, lock = None):
        imp.reload(sys)
        
        pdtlist = []
        if len(sys.argv) <= cmd_index:
            self.helpPrint(cmd_index)
            return False
        
        pdt = sys.argv[cmd_index]
        if pdt_num == 0:
            if not pdt in self.pdt2mxl:
                self.helpPrint(cmd_index)
                return False
            self.check_pdt(pdt)    
            return True
        #all单板xml检查，返回单板检查结果
        else:
            if lock == None:
                for single in pdt_num:
                    if single not in self.pdt2mxl:
                        self.helpPrint(cmd_index)
                        return False
                return True
            if not isinstance(pdt_num, int):
                return self.check_pdt(pdt_num, lock)
                
            for elm in self.pdt2mxl:
                pdtlist.append(elm) 
            
            return self.check_pdt(pdtlist[pdt_num-1], lock)

    def get_whitelist(self, class_xml_path):
        class_tree = et.parse(class_xml_path).getroot()
        class_whitelist = ["Chip"]
        for item in class_tree.findall(".//CLASS[@NAME=\"Chip\"]//CLASS"):
            class_whitelist.append(item.attrib["NAME"])
        property_whitelist = ['WriteTmout','ReadTmout','CacheBuf', 'CacheSuspended']
        return class_whitelist, property_whitelist

    def check_pdt(self, pdt, lock = None):

        localDir = os.getcwd() # 工具路径为../V2R2_trunk/tools;或者../V2R2_trunk/application，需要分情况拼接文件路径
        fatherDir = localDir[-5:] # 取当前目录名

        if (fatherDir == "tools"): # 当前工具在tools路径下
            workDir = localDir[:-5] + "application" # ../V2R2_trunk/application
            baseXml = "%s/others/other_tools/ci/xml/base.xml" %localDir
            classOwnwerXml = "%s/others/other_tools/ci/localscript/class_owner.txt" %localDir
        else: # 当前工具在application路径下
            workDir = localDir
            baseXml = "%s/src/include/ci/xml/base.xml" %localDir
            classOwnwerXml = "%s/src/include/ci/localscript/class_owner.txt" %workDir
        
        #读取class.xml
        classXml = "%s/src/resource/pme_profile/class.xml" %workDir
        if pdt in self.V6List:
            platformXml = "%s/src/resource/pme_profile/platform_v6.xml" %workDir
        else: 
            platformXml = "%s/src/resource/pme_profile/platform_v5.xml" %workDir
        platform_Xml = "%s/src/resource/pme_profile/platform.xml" %workDir  # 添加检查platform_v5.xml和platform.xml文件模块
        profilelist_file = "%s/src/resource/board/%s/profile.txt" %(workDir, pdt)

        class_whitelist, property_whitelist = self.get_whitelist(classXml)
        
        #枚举各种产品形态，例如全框，半框
        if type(self.pdt2mxl[pdt]) is list:
            pdtCfgList = self.pdt2mxl[pdt]
        else:
            pdtCfgList = [self.pdt2mxl[pdt]]
        
        virtualConnectCheckFlag = False
        if pdt in self.supportVirtualConnectCheckPdt:
            virtualConnectCheckFlag = True
        
        #isolated_list_list = []
        #orphans_list_list = []
        mergeErrList = []
        objoverlenthlist = []
        ErrString = ""
        result_tmp = {}
        ownerErrMsg = {}
        ownerWarningMsg = {}
        ownerWarningMsg["Warning"] = []
        CheckEventCode_result = {}
        checkedXmlList = []
        profileTempMap = {}
        global pdt_local
        pdt_local = "%s" %pdt
        global pangea_flag
        # 计算使用该告警码为紧急等级，盘古使用该告警码为严重等级，共用告警码，需要屏蔽
        ExceptEventCodeTuple = ("0x0300000D")
        if pdt in self.PangeaV6List:
            pangea_flag = 1
        else:
            pangea_flag = 0        #检查platform.xml和platform_v5.xml/platform_v6.xml中，EventCode重复的EventInformation
        CheckEventCode_result = checkEventCode(platform_Xml, platformXml)
        for file_name, ErrMsg in list(CheckEventCode_result.items()):
            for EventCode, ErrObjlist in list(ErrMsg.items()):
                if EventCode in ExceptEventCodeTuple:
                    continue
                if EventCode == 'None':
                    ErrString = ("Warning file = %s, class = EventInformation, object %s missing EventCode.Value." % (file_name, ErrObjlist))
                else:
                    ErrString = ("Warning file = %s, class = EventInformation, object %s have same EventCode.Value %s." % (file_name, ErrObjlist, EventCode))
                if ErrString not in ownerWarningMsg["Warning"]:
                    ownerWarningMsg["Warning"].append(ErrString)
        for pdtCfg in pdtCfgList:
            a = ProfileCheck(classXml, classOwnwerXml, class_whitelist, property_whitelist)
            mainboardXmlList = pdtCfg.split(",")
            mainboardXmlList = ["%s/src/resource/board/%s/cfgfs/profile/%s" %(workDir, pdt, xmlFile) for xmlFile in mainboardXmlList]
            a.check_profile(pdt, [platformXml, baseXml] + mainboardXmlList, "%s/src/resource/profile" %workDir, profilelist_file, checkedXmlList, profileTempMap, virtualConnectCheckFlag)
            #isolated_list_list.append(a.isolated_list)
            #orphans_list_list.append(a.orphans_list)
            for xmlName, objMap in list(a.profileMap.items()):
                if xmlName == 'base':
                    object_length_max = 31
                else:
                    object_length_max = 22
                    
                for objKey, obj in list(objMap.items()):
                    if len(obj['NAME'].strip()) > object_length_max:
                        #print("Error %s object %s length %d over." % (obj['FILE'], obj['NAME'].strip(), len(obj['NAME'].strip())))
                        ErrString = ("Warning %s length of object name %s %d is over then set value(%d)." % (obj['FILE'], obj['NAME'].strip(), len(obj['NAME'].strip()), object_length_max))
                        objoverlenthlist.append(ErrString)
            a.err.printErr_Warning(objoverlenthlist)
            mergeErrList.append(a.mergeErr.ownerErrMsg)
            #a.err.printAll()
            #错误信息放入字典WarningMsg和ownerErrMsg中，所有xml文件检查完统一打印
            for WarningMsg in a.err.ownerWarningMsg["Warning"]:
                if WarningMsg not in ownerWarningMsg["Warning"]:
                    ownerWarningMsg["Warning"].append(WarningMsg)
            for owner, ErrMsg in list(a.err.ownerErrMsg.items()):
                if owner in ownerErrMsg:
                    for OneErrMsg in ErrMsg:
                        if OneErrMsg not in ownerErrMsg[owner]:
                            ownerErrMsg[owner].append(OneErrMsg)
                else:
                    ownerErrMsg[owner] = ErrMsg
            #ownerErrMsg.update(a.err.ownerErrMsg)
            #if len(a.err.ownerErrMsg) > 0: break
            if lock != None: lock.acquire()
            print("%s success continue." % pdt)
            if lock != None: lock.release()
        
        result_list = self.merge_lists(mergeErrList)
        if lock != None: 
            lock.acquire()
            if (ownerWarningMsg["Warning"] or ownerErrMsg):
                print("%s starts printing error informations" % pdt)
        a.err.printAll(ownerWarningMsg, ownerErrMsg)
        printAll(result_list)
        if lock != None:
            if (ownerWarningMsg["Warning"] or ownerErrMsg):
                print("%s ends printing error informations" % pdt)
        rst = False
        if len(ownerErrMsg) == 0: 
            print("all result is success")
            #if len(result_list) == 0: 
            rst = True
        #print self.errMap

        cnt = 0
        skipFlag = False
        for value in ownerErrMsg.values():
            if "Warning" in str(value):
                cnt = cnt + 1
        if (pdt in self.PangeaV6List) and (len(ownerErrMsg) == cnt):
            skipFlag = True

        if rst or skipFlag:
            print("%s check xml success!" %pdt)
            result_tmp[pdt] = "success"
        else:
            print("%s check xml failed!" %pdt)
            result_tmp[pdt] = "failed"
        if lock != None: lock.release()
        return result_tmp

    def helpPrint(self, cmd_index):
        brdList = ""
        for k, v in list(self.pdt2mxl.items()):
            brdList += "%s, " %k
        
        print("<boardname>： supported board: %s" %brdList[:-2])
        print("")
    
    def cmdDes(self):
        return "check class.xml and product profile file"
        
    #下面为新增加代码，并行检查    2016-1-20
    def start_check(self, pdt, pdtCfgList):
        workDir = os.getcwd()

        framworkDir =  "%s/src/include/ci/xml" %workDir
        #读取class.xml
        classXml = "%s/src/resource/pme_profile/class.xml" %workDir
        if pdt in self.V6List:
            platformXml = "%s/src/resource/pme_profile/platform_v6.xml" %workDir
        else:
            platformXml = "%s/src/resource/pme_profile/platform_v5.xml" %workDir
        baseXml = "%s/base.xml" %framworkDir
        profilelist_file = "%s/src/resource/board/%s/profile.txt" %(workDir, pdt)

        class_whitelist, property_whitelist = self.get_whitelist(classXml)
        
        virtualConnectCheckFlag = False
        if pdt in self.supportVirtualConnectCheckPdt:
            virtualConnectCheckFlag = True
        
        #isolated_list_list = []
        #orphans_list_list = []
        mergeErrList = []
        checkedXmlList = []
        profileTempMap = {}
        for pdtCfg in pdtCfgList:

            a = ProfileCheck(classXml, "%s/src/include/ci/localscript/class_owner.txt" %workDir, class_whitelist, property_whitelist)
            mainboardXmlList = pdtCfg.split(",")
            mainboardXmlList = ["%s/resource/board/%s/cfgfs/profile/%s" %(workDir, pdt, xmlFile) for xmlFile in mainboardXmlList]
            a.check_profile(pdt, [platformXml, baseXml] + mainboardXmlList, "%s/src/resource/profile" %workDir, profilelist_file, checkedXmlList, profileTempMap, virtualConnectCheckFlag)
            #isolated_list_list.append(a.isolated_list)
            #orphans_list_list.append(a.orphans_list)
            mergeErrList.append(a.mergeErr.ownerErrMsg)
            a.err.printAll()
            if len(a.err.ownerErrMsg) > 0: break
            #print "success continue."
            
        rst = False
        if len(a.err.ownerErrMsg) == 0: 
            #print "all result is success"
            result_list = self.merge_lists(mergeErrList)
            printAll(result_list)
            if len(result_list) == 0: rst = True

        #print self.errMap
        if rst:
            print("%s check xml success!" %pdt)
            return True
        else:
            print("%s check xml failed!" %pdt)
            return False
        
        
    def xml_thread(self, pdt, pdtCfgList):
        ret = self.start_check(pdt, pdtCfgList)
        if False == ret:
            with g_lock:
                g_result.value = 1
        
    def process(self):
        imp.reload(sys)
        sys.setdefaultencoding('utf-8')

        jobs = []
        
        nlists = list(range(len(self.pdt2mxl)))
        for pdt in self.pdt2mxl:
            #枚举各种产品形态，例如全框，半框
            if type(self.pdt2mxl[pdt]) is list:
                pdtCfgList = self.pdt2mxl[pdt]
            else:
                pdtCfgList = [self.pdt2mxl[pdt]]
                    
            p = multiprocessing.Process(target=self.xml_thread, args=(pdt,pdtCfgList))
            jobs.append(p)
            p.start()
        
        
        njobs = list(range(len(jobs)))
        for i in njobs:
            jobs[i].join()
        
        return g_result.value





