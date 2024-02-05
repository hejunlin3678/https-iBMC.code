#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Copyright © Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.

import xml.dom.minidom
import os,sys,glob,shutil,copy
#add by b00357649 begin  删除XML里没用的空白字符


def open_file_xml_format(file_name, mode='r', encoding=None, **kwargs):
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

def remove_space(xmlstr):
    strmod=[[' <','<'],['> ','>']
            ,['\t<','<'],['>\t','>']
            ,[' \r','\r'],['\t\r','\r']
            ,[' \n','\n'],['\t\n','\n']
            ,['\r',''],['\n','']
            ,['<PROPERTY ','<P ']
            ,['/PROPERTY>','/P>']
            ,['<VALUE>','<V>']
            ,['</VALUE>','</V>']
            ,['<VALUE/>','<V/>']
            ,['<OBJECT ','<O ']
            ,['/OBJECT>','/O>']]
    for mode in strmod:
        while mode[0] in xmlstr:
            xmlstr = xmlstr.replace(mode[0],mode[1])
    return xmlstr

def covert_file(filename):
    tmp = filename + ".tmp"
    fp = open_file_xml_format(filename,'r')
    outfp = open_file_xml_format(tmp,'w')
    while 1:
        lines = fp.readlines()
        if not lines:
            break
        for line in lines:
            line = remove_space(line)
            outfp.write(line)
    print(filename,'ok')
    fp.close()
    outfp.close()
    shutil.move(tmp, filename)
#add by b00357649 end
    
def replace_keywords(xmlstr):
    strmod=[['<PROPERTY ','<P ']
            ,['/PROPERTY>','/P>']
            ,['<VALUE>','<V>']
            ,['</VALUE>','</V>']
            ,['<VALUE/>','<V/>']
            ,['<OBJECT ','<O ']
            ,['/OBJECT>','/O>']
            ,['\t','']]
    for mode in strmod:
        while mode[0] in xmlstr:
            xmlstr = xmlstr.replace(mode[0],mode[1])
    return xmlstr    

    
def covert_keywords(filename):
    tmp = filename + ".tmp"
    fp = open_file_xml_format(filename,'r')
    outfp = open_file_xml_format(tmp,'w')
    while 1:
        lines = fp.readlines()
        if not lines:
            break
        for line in lines:
            line = replace_keywords(line)
            outfp.write(line)
    print(filename,'ok')
    fp.close()
    outfp.close()
    shutil.move(tmp, filename)

    
def write(filename, xmldoc):
    tmp = filename + ".tmp"
    writeFile = open_file_xml_format(tmp, 'wb')
    writeFile.write(xmldoc.toxml('utf-8')) 
    writeFile.close()
    shutil.move(tmp, filename)

def rmv_comment(xmlNode):
    commentNodeList = []
    for subNode in xmlNode.childNodes:
        if "#comment" == subNode.nodeName: 
            commentNodeList.append(subNode)
            continue
        rmv_comment(subNode)

    for commentNode in commentNodeList:
        xmlNode.removeChild(commentNode)

#入参 1: 需要去掉注释的XML所在的目录名称， 此目录下的所有XML中的注释都会没干掉。
xmlList = glob.glob(os.path.join(sys.argv[1], "*.xml"))
for xmlFile in xmlList:
    print("Cleanup %s ..." %xmlFile)
    try:
        xmldoc = xml.dom.minidom.parse(xmlFile)
    except xml.parsers.expat.ExpatError:
        print(f"----->>>> {xmlFile} can not trans <<<<-----")
        continue
    rmv_comment(xmldoc)
    write(xmlFile, xmldoc)
    #add by b00357649 begin  删除XML里没用的空白字符
    if (len(sys.argv) > 2) and (sys.argv[2] == "remove_space"):
        covert_file(xmlFile)
        
    if (len(sys.argv) > 2) and (sys.argv[2] == "covert_keywords"):
        covert_keywords(xmlFile)        
    #add by b00357649 end
        