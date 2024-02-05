# Copyright © Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
# -*- coding: utf-8 -*-
import os , sys
import traceback
from basicCmdClass import *
import string, threading, time
import random
from stat import *
import xml.dom.minidom

ccn_location = [r'C:\Program Files\CodingPartner\tools\SourceMonitor', r'C:\Program Files\Avenue SmartIDE\CodingPartner\tools\SourceMonitor']




def open_file_ccn(file_name, mode='r', encoding=None, **kwargs):
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

class CNcss(CCmdBase):
    def __init__(self):
        self.defAction = "help"
        self.ccnExe = self.getCcnExe()
        self.ccnDir = os.path.dirname(self.ccnExe)
        self.ccnCfg = "%s\\ccn.cfg" %self.ccnDir
        #self.ccnCfg = "c:\\ccn.cfg"
        self.ccnRstXml = "%s\\ccn_result.xml" %self.ccnDir
        #self.ignoreFile = ".\\script\\entry\\ccnIgnoreList.txt"
        #self.ignoreList = self.readIgnoreFile(self.ignoreFile)
        self.maxCcn = 15
        self.oldFuncMap = None

    def getCcnExe(self):
        for dirname in ccn_location:
            lint_name = "%s\\SourceMonitor.exe" %dirname
            if os.path.isfile(lint_name):
                return lint_name
            print(lint_name)
        raise AssertionError("Can't find SourceMonitor.exe")
    

    def getCcnMax(self, modName, func):
        if None == self.oldFuncMap:
            self.oldFuncMap = {}

            for line in open_file_ccn(r".\ci\localscript\special_ccn.txt").readlines():
                line = line.strip("\r")
                line = line.strip("\n")
                tmp = line.split("    ")
                module = tmp[0]
                function = tmp[1]
                ccn = tmp[2]
                if not module in self.oldFuncMap: self.oldFuncMap[module] = {}
                self.oldFuncMap[module][function] = ccn
        
        if not modName in self.oldFuncMap: 
            return int(self.maxCcn)
        
        #获取这个模块的最大圈复杂度
        if None == func:
            maxCcn = self.maxCcn
            for k, v in list(self.oldFuncMap[modName].items()):
                if int(v) > maxCcn: maxCcn = int(v)
            return int(maxCcn)
        
        if not func in self.oldFuncMap[modName]: 
            return int(self.maxCcn)
            
        return int(self.oldFuncMap[modName][func])
        
    def getXmlFuncNodeList(self, resultXml):
        # 如果是空文件，可以遍历到'file'
        nodeList = ['project', 'checkpoints', 'checkpoint', 'files', 'file', 'method_metrics', 'method']
        try:
            xmldoc = xml.dom.minidom.parse(resultXml)
        except:
            traceMsg = traceback.format_exc()
            print(traceMsg)
            print(resultXml)
            if traceMsg.find("not well-formed") != -1:    print("请检查代码工程路径中是否存在中文字符!")
            return False
            
        fNode = xmldoc.childNodes[0]
        subNodeList = None
        for nd in nodeList:
            subNodeList = fNode.getElementsByTagName(nd)
            if len(subNodeList) == 0:
                if nd == 'method_metrics' or nd == 'method':
                    #空文件
                    return []
                else:
                    print("遍历XML节点%s失败" %nd)
                    return [] #解析失败,外面会抛出异常
            fNode = subNodeList[0]

        return subNodeList

    def parseMcssOut(self, resultXml):
        funcInfo = []
        funcNodeList = self.getXmlFuncNodeList(resultXml)
        for func in funcNodeList:
            funcName = str(func.getAttribute("name"))[:-2]
            funcName = funcName.strip()
            complexity = str(func.getElementsByTagName("complexity")[0].firstChild.data)
            complexity = int(complexity)
            #print "%s:%s" %(funcName, complexity)
            funcInfo.append((funcName, complexity))
        return funcInfo

    def mkCcnCfg(self, fullFilePpath):
        filePath, fileName = splitFullPath(fullFilePpath)
        if os.path.isfile("%s\\PTU.smp" %self.ccnDir): os.system("del \"%s\\PTU.smp\"" %self.ccnDir)
        ccnCfg = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n\
<sourcemonitor_commands>\n\
    <write_log>true</write_log>\n\
    <command>\n\
        <project_file>PTU.smp</project_file>\n\
        <project_language>C++</project_language>\n\
       <modified_complexity>true</modified_complexity>\n\
       <source_directory>%s</source_directory>\n\
        <source_subdirectory_list>\n\
            <exclude_subdirectories>true</exclude_subdirectories>\n\
       </source_subdirectory_list>\n\
        <file_extensions>%s</file_extensions>\n\
        <include_subdirectories>false</include_subdirectories>\n\
        <ignore_headers_footers>true</ignore_headers_footers>\n\
        <export>\n\
            <export_file>%s</export_file>\n\
            <export_type>2 (checkpoint details as XML)</export_type>\n\
            <export_option>Include method metrics: option 3</export_option>\n\
        </export>\n\
    </command>\n\
</sourcemonitor_commands>" %(filePath, fileName, self.ccnRstXml)

        f = open_file_ccn(self.ccnCfg, "w")
        f.write(ccnCfg)
        f.close()

    def ncssFile(self, filename):
        # 生成配置文件
        filename = os.path.abspath(filename)
        if not os.path.isfile(filename): return []
        self.mkCcnCfg(filename)
        if os.path.isfile(self.ccnRstXml): os.remove(self.ccnRstXml)
        cwd = os.getcwd()
        os.chdir(self.ccnDir)
        #print "%s /C++ %s" %(self.ccnExe, self.ccnCfg)
        cmdLine = "\"%s\" /C++ %s" %(self.ccnExe, self.ccnCfg)
        #print cmdLine
        ret = os.system(cmdLine)
        #if ret != 0:
        #    raise AssertionError, "ret = %d"  %ret
        os.chdir(cwd)
        return self.parseMcssOut(self.ccnRstXml)

    def ncssNewFuncsMaxCcn(self, baseFile, newFile):
        baseList = self.ncssFile(baseFile)
        newList  = self.ncssFile(newFile)
        return self.ncssNewFuncsMaxCcnByList(baseList, newList)

    def getFileExtName(self, filename):
        lIdx = filename.rfind(".")
        if lIdx == -1: return ""
        return filename[lIdx + 1:]

    def ccnRst(self, modName, ccn):

        if ccn > self.getCcnMax(modName, None): return False
        else: return True

    def findFile(self, pathRoot, filename):
        filename = filename.lower()
        for root, dirs, files in os.walk(pathRoot, True):
            for name in files:
                if name.lower() == filename:
                    return os.path.join(root, name)
        return None

    def checkFile(self, modName, filename, funcName):
        printAll = False
        if funcName != None: printAll = True
        failCount = 0
        passCount = 0
        funcList  = self.ncssFile(os.path.abspath(filename))
        fileMaxCcn = 0
        file_check_pass = True
        for func, ccn in funcList:
            if funcName != None and funcName != func: continue
            functinMaxCcn = self.getCcnMax(modName, func)
            if printAll or (int(ccn) > functinMaxCcn): 
                print("function : \"%s\" ccn : %d > %d" %(func, ccn, functinMaxCcn))
                file_check_pass = False
            if int(ccn) > functinMaxCcn: 
                #if not func in self.ignoreList: 
                failCount += 1
            else:
                passCount += 1

            if int(ccn) > fileMaxCcn: fileMaxCcn = int(ccn)

        msg = "         maxccn = %d, total func = %d, fail func = %d\n" %(fileMaxCcn, (failCount + passCount), failCount)
        
        if file_check_pass:
            print("%s[ pass ] %s\n" %(msg, os.path.basename(filename )))
        else:
            print("%s[ FAIL ] %s\n" %(msg, os.path.basename(filename )))

        return fileMaxCcn, failCount
        
    #对指定的模块、文件名、函数进行检查
    def checkModule(self, modName, fileName, funcName):
        moduleMaxCcn = 0
        self.fileList = self.getSourceList(modName)
        count = 0
        
        if None != fileName:
            pathName = self.basename2pathname(fileName)
            fileList = [pathName]
        else:
            fileList = self.fileList

        for filename in fileList:
            fileMaxCcn, filecount = self.checkFile(modName, filename, funcName)
            count += filecount
            if fileMaxCcn > moduleMaxCcn: moduleMaxCcn = fileMaxCcn

        if count > 0:
            print("\'%s\' module has %s function(s) ccn check FAILED, max ccn is %d" %(modName, count, moduleMaxCcn))
            return False
        else:
            print("\'%s\' ccn check passed." %modName)
            return True
        
        #return moduleMaxCcn

    def checkAllModule(self):
        passList = []
        failList = []
        for modName in self.cfg['basic']['modList']:
            ret = self.checkModule(modName, None, None)
            if ret:
                passList.append(modName)
            else:
                failList.append(modName)
        
        print("modules passed ccn check: %s" %passList)
        if len(failList) > 0:
            print("modules failed ccn check: %s" %failList)
            return False
        return True
        
    def doIt(self, cmd_index, pdt_num = 0, lock = None):
        self.cfg = readCfg(".\\ci\\config.ini")

        if len(sys.argv) <= cmd_index:
            self.helpPrint(cmd_index)
            return False

        #检查指定模块
        modName = sys.argv[cmd_index]
        if "all" == modName:
            return self.checkAllModule()
            
        if not modName in self.cfg['basic']['modList']:
            # 参数错误
            self.helpPrint(cmd_index);
            return False

        if len(sys.argv) == cmd_index + 1:
            # 不指定文件名，检查整个模块本地修改的函数，作为代码上库前的检查
            return self.checkModule(modName, None, None)
            
            #return self.ccnRst(modName, ccn)

        if len(sys.argv) == cmd_index + 2:
            #指定文件名
            fileName = sys.argv[cmd_index + 1]
            return self.checkModule(modName, fileName, None)

        if len(sys.argv) == cmd_index + 3:
            # 指定函数
            fileName = sys.argv[cmd_index + 1]
            func = sys.argv[cmd_index + 2]
            return self.checkModule(modName, fileName, func)

    def cmdDes(self):
        return "圈复杂度检查"

    def helpPrint(self, cmd_index):
        print("<模块名>： 合法的模块名列表: %s" %str(self.cfg['basic']['modList'])[1:-1])
        print("[文件名]： 检查单个文件时使用。")
        print("[函数名]： 检查制定文件的单个函数时使用。")
        print("")
