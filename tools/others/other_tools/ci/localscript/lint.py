# Copyright © Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
# -*- coding: utf-8 -*-
import os , sys, time, glob, configparser
from commScript import *
from basicCmdClass import *

#lint_exe = C:\Program Files\CodingPartner\tools\lint\lint-nt.exe
lint_location = [r'C:\Program Files\CodingPartner\tools\lint', r'C:\Program Files\Avenue SmartIDE\CodingPartner\tools\lint']



def open_file_lint(file_name, mode='r', encoding=None, **kwargs):
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

class lintModuleClass(CMakeBase):
    def __init__(self, cfg, module, filename):
        self.cfg = cfg
        self.modName = module
        self.sourceFile = filename
        self.tempInfoFile = "c:\\%s_lint_temp.txt" %self.modName
        self.lintexe = self.getLintExe()

    def getLintExe(self):
        for dirname in lint_location:
            lint_name = "%s\\lint-nt.exe" %dirname
            if os.path.isfile(lint_name):
                return lint_name
        
        raise AssertionError("Can't find lint-nt.exe")
        
    def lintSingleFile(self, i):
        fileName = self.fileList[i]
        cmdLine = "\"%s\" -u -b" %self.lintexe
        cmdLine += " -i.\\%s" %(self.cfg['basic']['lint_dir'])
        cmdLine += " .\\%s\\%s.lnt" %(self.cfg['basic']['lint_dir'], self.modName)
        cmdLine += " .\\%s" %fileName
        
        #print cmdLine
        ret, text = getstatusoutput(cmdLine)
        self.errNum = text.count("\n") + 1
        if ret != 0: print(text.replace("\n", "\n\n"))
        
        total = len(self.fileList)
        strLen = len(str(total))
        
        msg = "%s/%s %s" %(str(i + 1).rjust(strLen), total, os.path.basename(fileName))
        if ret != 0: 
            print("[ fail ] %s" %msg)
            self.saveFailIdx(i)
            return False
        print("[ pass ] %s" %msg)
        return True
    
    #设置环境变量
    def setEnv(self):
        envList = []
        if 'environ' in self.cfg['basic']:
            envList += self.cfg['basic']['environ'].split(',')
        if 'environ' in self.cfg[self.modName]:
            envList += self.cfg[self.modName]['environ'].split(',')
            
        for item in envList:
            k,v = item.split("=")
            v = v.replace("%PRJ_BASE%", os.path.abspath('.'))
            print("set %s=%s" %(k,v))
            os.environ[k] = v
    
    def saveFailIdx(self, index):
        open_file_lint(self.tempInfoFile, "w").write(str(index))
    
    def readFailIdx(self):
        if not os.path.isfile(self.tempInfoFile): return 0

        fileContent = open_file_lint(self.tempInfoFile).read()
        if not fileContent.isdigit(): return 0
        begIndex = int(fileContent)
        if begIndex > len(self.fileList): begIndex = 0
        return begIndex

    def lintAll(self):
        beginIdx = self.readFailIdx()
        for i in range(beginIdx, len(self.fileList)):
            ret = self.lintSingleFile(i)
            if not ret: return False

        for i in range(0, beginIdx):
            ret = self.lintSingleFile(i)
            if not ret: return False
        self.saveFailIdx(0)

        return True
    def lintFileByName(self, name):
        for i,v in enumerate(self.fileList):
            if os.path.basename(v).lower() == name.lower():
                return self.lintSingleFile(i)
        print("can't find %s in module %s's source files." %(name, self.modName))
        return False

    def runLint(self):
        self.setEnv()
        self.fileList = self.getSourceList(self.modName)
        #print os.path.abspath(self.fileList[0])

        if self.sourceFile == "":
            return self.lintAll()
        else:
            return self.lintFileByName(self.sourceFile)

    def run(self):
        print("=======================")
        print("Bgian lint module: %s "%self.modName)
        ret = self.runLint()
        if ret:
            print("PC-LINT %s success!\n" %self.modName)
        else:
            print("PC-LINT %s failed! Problem number: %s\n" %(self.modName, self.errNum))
        return ret

class CLint():
    def __init__(self, inPara = ""):
        path = os.getcwd()
        print(path)
        os.system("mkdir .\\ci\\sdk")
        os.system("7z e ./ci/../platform/PME_SDK.tar.gz -y -o./ci/sdk")
        os.system("7z x ./ci/sdk/PME_SDK.tar -y -o./ci/sdk")
        os.system("del /Q /F .\\ci\\sdk\\PME_SDK.tar")
        os.system("xcopy /Y /E /S .\\ci\\sdk\\PME\\V100R001C10\\include .\\ci\\include")
        os.system("rd /Q /S .\\ci\\sdk")
        
    def cmdDes(self):
        return "PCLINT检查"

    def setSingleFileName(self):
        print("setSingleFileName lalala")

        
    def parseSubCmd(self):
        self.moduleName = self.getListPara(self.cmd_index)
        if None == self.moduleName:
            return False

        return True

    def lint(self, cmd_index):
        self.preBuild()
        
        restPara = ""
        for idx in range(cmd_index, len(sys.argv)):
            restPara += " " + sys.argv[idx]
        
        if self.moduleName == 'all':
            for mod in self.paraList:
                if False == self.lintOneLib(mod, restPara):
                    return False
            return True
        else :
            return self.lintOneLib(self.moduleName, restPara)

            
    def printHelp(self):
        print("<模块名>： 合法的模块名列表: %s" %str(self.cfg['basic']['modList'])[1:-1])
        print("[文件名]： lint单个文件时使用。")
        print("\nEg: lint.py cim iManaCIM_SoftwareInstallationConcreteJob_Provider.cpp")
        
    def parseArgv(self, cmd_index):
        argv = sys.argv[cmd_index:]

        if len(argv) == 0:
            self.printHelp()
            return False, "", ""
            
        # lint所有模块
        if argv[0] == "all":
            return True, "all", ""
    
        if not argv[0] in self.cfg['basic']['modList']:
            self.printHelp()
            return False, "", ""
        
        # lint单个模块
        if len(argv) == 1:
            return True, argv[0], ""
        # lint指定模块的xx文件
        #if len(sys.argv) == 3:
        return True, argv[0],argv[1]
    
    def lintAllModules(self):
        failModList = []
        for mod in self.cfg['basic']['modList']:
            a = lintModuleClass(self.cfg, mod, "")
            ret = a.run()
            if ret == 0: failModList.append(mod)
    
        if len(failModList) == 0: 
            print("============================")
            print("PC-LINT all modules success!")
            print("============================")
            return True
        
        print("===============================")
        print("PC-LINT '%s' failed!" %str(failModList)[1:-1])
        print("===============================")
        return False


    def doIt(self, cmd_index, pdt_num = 0, lock = None):
        self.cfg = readCfg(".\\ci\\config.ini")
        #os.chdir("..")
        ret, module, filename = self.parseArgv(cmd_index)
        if ret != True: return 1
        
        if module == "all":
            return self.lintAllModules()
        else:
            a = lintModuleClass(self.cfg, module, filename)
            ret = a.run()
        
        if 0 == ret: return True
        return False



if __name__ == "__main__":
    def main():
        cfg = readCfg(".\\ci\\config.ini")
        #os.chdir("..")
        ret, module, filename = parseArgv(cfg)
        if ret != True: return 1
        
        if module == "all":
            return lintAllModules(cfg)
        else:
            a = lintModuleClass(cfg, module, filename)
            ret = a.run()
        
        return ret

    main()