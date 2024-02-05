# Copyright © Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
# -*- coding: utf-8 -*-
import os , sys, subprocess, time, threading, glob 
import queue
import _thread
from commScript import *
class CMakeBase():
    
    #扫描目录，获取源代码列表
    def getSourceList(self, modName):
        incFileNameList = []
        maxNum = int(self.cfg['basic']['source_list_num'])
        for i in range(maxNum):
            attrStr = 'sourcefile%d' %i
            if not attrStr in self.cfg[modName]: continue
            incList, excList = self.pasefileString(self.cfg[modName][attrStr])
            incFileNameListTmp = []; excFileNameListTmp = []

            for elm in incList: 
                if 'rootdir' in self.cfg[modName]:
                    srcDir = "%s\\%s" %(self.cfg[modName]['rootdir'], elm)
                else:
                    srcDir = elm
                incFileNameListTmp += glob.glob(srcDir)
            for elm in excList: 
                if 'rootdir' in self.cfg[modName]:
                    srcDir = "%s\\%s" %(self.cfg[modName]['rootdir'], elm)
                else:
                    srcDir = elm
                excFileNameListTmp += glob.glob(srcDir)
            incFileNameList += [x for x in incFileNameListTmp if not x in excFileNameListTmp]
        return incFileNameList
                
    #解析 ini 文件中定义源代码的行
    def pasefileString(self, fileString):
        tmpList = fileString.split(" /")
        incList = [s.strip(' ') for s in tmpList[0].split(',') if s.strip(' ') != ""]
        excList = []
        for elm in tmpList[1:]:
            if elm.find(' ') == -1: 
                #必须包含空格，否则为格式错误。正确的格式应该是“/XF xxx.c”
                raise AssertionError("config.ini parse faied. Error line is:\n'%s'" %fileString)
            key = elm.split(' ')[0]
            if key == "XF":
                excList += [s.strip(' ') for s in elm[len(key) + 1:].split(',') if s.strip(' ') != ""]
        return incList, excList

    def basename2pathname(self, name):
        for i,v in enumerate(self.fileList):
            if os.path.basename(v).lower() == name.lower():
                return v
        return null


class CCmdBase(CMakeBase):

    def __init__(self):
        self.defAction = "help"
        self.subActList = { 'invalid' : 'lalala',
        }

    def getSubCmd(self):
        if len(sys.argv) < (self.cmd_index + 1):
            return self.defAction
        else:
            return sys.argv[self.cmd_index]
    
    def getSubCmdAction(self, sub_cmd):
        for k, v in list(self.subActList.items()):
            if k == sub_cmd:
                c = v()
                return c.doIt
        return None
        
    def isSubCmdValid(self, sub_cmd):
        for k, v in list(self.subActList.items()):
            if k == sub_cmd:
                return True
        return False

    def doIt(self, pdt_num, lock = None):
        self.cmd_index = 1
        # 更新代码，必须制定子参数, 先判断子命令是否合法
        sub_cmd = self.getSubCmd()
        act = self.getSubCmdAction(sub_cmd)

        if None == act :
            self.helpPrint(self.cmd_index)
            return False
        else:
            return act((self.cmd_index + 1), pdt_num, lock)

            
    def cmdDes(self):
        return "功能说明：命令功能说明。。。"
            
    def helpPrint(self, cmd_index):
        mast_cmd = ""
        for i in range(1, cmd_index):
            mast_cmd += sys.argv[i] + " "
    
        print("命令功能：" + self.cmdDes())
        print("")
        helpStr = ""
        if mast_cmd != "" :
            helpStr =  mast_cmd + "命令"
    
        helpStr += "可用的子命令列表如下"
    
        if self.defAction != "":
            helpStr += "(默认 " + self.defAction + "):"
    
        print(helpStr)
        print("help".rjust(10) + " : " + "命令使用帮助")
        for k, v in list(self.subActList.items()):
            c = v()
            print(k.rjust(10) + " : " + c.cmdDes())    
            
            
class thread_pool():
    def __init__(self, max_work_num=0, thread_num=15):
        self.thread_num = thread_num
        self.task_queue = queue.Queue(maxsize = max_work_num)
        self.task_num = 0

    def init_thread_pool(self):
        timein = time.time()
        task_num1 = min(self.task_num, self.thread_num)
        print(">>>>>%d<<<<<<" %task_num1)
        for i in range(task_num1):
            _thread.start_new_thread(self.run, ())
        timeUse = time.time() - timein
        print("eclipse: %s" %timeUse)

    def run(self):
        while True:
            try:
                timein = time.time()
                task_list = self.task_queue.get(False)
                if len(task_list) == 1:
                    task_list[0]()
                elif len(task_list) == 2:
                    task_list[0](task_list[1])
                elif len(task_list) == 3:
                    task_list[0](task_list[1], task_list[2])
                elif len(task_list) == 4:
                    task_list[0](task_list[1], task_list[2], task_list[3])
                else:
                    print("too many args")
                self.task_queue.task_done()
                print("eclipse: %s" %(time.time() - timein))

            except queue.Empty:
                #print "Queue is EmptyQueue is Empty"
                break

    def add_one_task(self, callback, *args):
        task_list = []
        task_list.append(callback)
        for arg in args:
            task_list.append(arg)

        self.task_queue.put_nowait(task_list)
        self.task_num += 1

    def start_work(self):
        self.init_thread_pool()
        self.task_queue.join()

