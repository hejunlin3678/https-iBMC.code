# Copyright © Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
# -*- coding: utf-8 -*-
import os , sys, subprocess, time, configparser

def getstatusoutput(cmd):
    pipe = os.popen(cmd + ' 2>&1', 'r')
    text = pipe.read()
    sts = pipe.close()
    if sts is None: sts = 0
    if text[-1:] == '\n': text = text[:-1]
    return sts, text

def mysyscall(cmd):
    sts, text=getstatusoutput(cmd)
    print(text)
    return sts

def getNowStr():
    thisTime = time.localtime(time.time()) #time.strftime('%Y-%m-%d',time.localtime(time.time()))
    return time.strftime('%X', thisTime)


def killProcess(img):
    os.system("cd /d C:\WINDOWS\system32\wbem&taskkill /F /IM %s > nul 2>&1" %img)

def killPid(pid):
    os.system("cd /d C:\WINDOWS\system32\wbem&taskkill /PID %d /T /F" %pid)

def subProcess(cmd, timeout, returnOut=False):
    if returnOut:
        p=subprocess.Popen(cmd, shell=True,stdout=subprocess.PIPE,stdin=subprocess.PIPE,stderr=subprocess.PIPE)
    else:
        p=subprocess.Popen(cmd, shell=True)
    t = 0
    ret = None
    while(t <= timeout):
        ret = p.poll()
        if None != ret: 
            break
        time.sleep(1)
        t +=1

    if None == ret:
        killPid(p.pid)
        if returnOut: 
            return -1, "cmd \"%s\" exec %s second timeout. kill it."
        else: 
            print("子进程执行%s秒超时！" %t)
            return -1
    else:
        if returnOut: return ret, p.stdout.read()
        else: 
            print("子进程正常结束：耗时%d秒, 最大等待时间%d秒" %(t, timeout))
            return ret

def splitFullPath(fullPath):
    idx = fullPath.rfind("\\")
    if idx == -1:
        return "", fullPath
    
    return fullPath[:idx + 1], fullPath[idx + 1:]

#去掉重复部分
def getSvnFileUrl(baseUrl, filename):
    baseUrl = baseUrl.strip("/")
    filename = filename.strip("/")
    dirList = filename.split("/")
    
    matchPrefix = ""
    prefix = ""
    for dirname in dirList:
        tmpPrefix = prefix + "/%s" %dirname
        #print baseUrl[len(baseUrl) - len(tmpPrefix):]
        #print tmpPrefix
        if baseUrl[len(baseUrl) - len(tmpPrefix):] == tmpPrefix:
            matchPrefix = tmpPrefix
            break
        prefix = tmpPrefix

    #print "return: %s + %s" %(baseUrl, filename[len(matchPrefix):])
    return "%s/%s" %(baseUrl, filename[len(matchPrefix):])

def mkDir(fileName):
    fileName = os.path.abspath(fileName)
    dirList = fileName.split("\\")

    dirName = dirList[0]
    # 如果是远程，跳过前2个
    #print "============="
    #print dirList
    if dirName == "":
        beginIdx = 3 # \\\\10.166.159.22\public\agc
    else:
        beginIdx = 1
    for i in range(1, len(dirList)):
        dirName += "\\" + dirList[i]
        if i <= beginIdx: continue #考虑到远程的情况
        #print "dirname:" + dirName
        while(False == os.path.isdir(dirName)):
            try: 
                os.mkdir(dirName)
                break
            except:
                time.sleep(1)


def getPathName(filename):
    rIdx = filename.rfind("\\")
    if rIdx == -1: return ""
    return filename[:rIdx]

def readCfgFile(iniFile):
    cfg = {}
    config = configparser.ConfigParser()
    config.readfp(open(iniFile))
    
    for section in config.sections():
        cfg[section] = {}
        for option in config.options(section):
            cfg[section][option] = config.get(section, option)
    return cfg
    
def readCfg(iniFile):
    cfg = readCfgFile(iniFile)
    # 生成模块列表
    cfg['basic']['modList'] = []
    for k,v in list(cfg.items()):
        if k == "basic": continue
        cfg['basic']['modList'].append(k)
    
    return cfg

