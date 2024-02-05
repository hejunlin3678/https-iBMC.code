# Copyright © Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
# coding=utf-8
'''
检查platform.xml和platform_v5.xml中，EventCode重复的EventInformation
调用方式：
form check_event_code import checkEventCode
result = checkEventCode(v3FileXMLPath,v5FileXMLPath)
'''
import os
import sys
import xml.etree.ElementTree as ET

def checkEventCode(pv3Path,pv5or6Path):
    pv3Path = os.path.abspath(pv3Path)
    pv5or6Path = os.path.abspath(pv5or6Path)
    pv3Result = parseXML(pv3Path)
    pv5or6Result = parseXML(pv5or6Path)
    return {os.path.basename(pv3Path):pv3Result,os.path.basename(pv5or6Path):pv5or6Result}

def parseXML(xmlFilePath):
    try:
        tree = ET.parse(xmlFilePath)
        root = tree.getroot()
    except Exception as e:
        print("parse "+xmlFilePath+" fail!")
        return -1
    eventDict = dict()
    numDict = dict()
    for object in root:
        if object.attrib.get('CLASS') == "EventInformation":
            eventName = object.attrib.get('NAME')
            for prop in object:
                if prop.attrib.get('NAME') == "EventCode":
                    EventCode = str(prop[0].text)
            if eventDict.get(EventCode)==None:
                eventDict[EventCode] = list()
                eventDict[EventCode].append(eventName)
            else:
                eventDict[EventCode].append(eventName)
    keys = list(eventDict.keys())
    for key in keys:
        if key != 'None' and len(eventDict[key]) == 1:
            eventDict.pop(key)
    return eventDict
                
if __name__ == "__main__":
    pv3Path = "D:/git/iBMC/application/src/rootfs/opt/pme/conf/profile/platform.xml"
    pv5Path = "D:/git/iBMC/application/src/rootfs/opt/pme/conf/profile/platform_v5.xml"
    print(checkEventCode(pv3Path,pv5Path))
	
