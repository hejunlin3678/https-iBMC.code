# Copyright © Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
# -*- coding: utf-8 -*-
import os, configparser, time, traceback
import _thread
import sys, glob, configparser
from commScript import *
from basicCmdClass import *
import ctypes

# 彩色打印


def open_file_TransferClass(file_name, mode='r', encoding=None, **kwargs):
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

def error_print(string):
    ctypes.windll.kernel32.SetConsoleTextAttribute(ctypes.windll.kernel32.GetStdHandle(-11), 12)
    print(string)
    ctypes.windll.kernel32.SetConsoleTextAttribute(ctypes.windll.kernel32.GetStdHandle(-11), 7)

def green_print(string):
    ctypes.windll.kernel32.SetConsoleTextAttribute(ctypes.windll.kernel32.GetStdHandle(-11), 10)
    print(string)
    ctypes.windll.kernel32.SetConsoleTextAttribute(ctypes.windll.kernel32.GetStdHandle(-11), 7)


last_flg_file_name = None
transfer_flag = True

def updateFlgFile():
    global last_flg_file_name
    f = open_file_TransferClass(last_flg_file_name, "w")
    f.write("xxx")
    f.close()

def getLastSynTime():
    global last_flg_file_name
    if not os.path.exists(last_flg_file_name):
        return 0
    else:
        return os.stat(last_flg_file_name).st_mtime

base_time = None
def isNeedSyn(filepath, is_update = False):
    global base_time

    base_time = getLastSynTime()

    mtime = os.stat(filepath).st_mtime
    if (mtime > base_time):
        base_time = mtime
        if not is_update: updateFlgFile()
        return True
    else:
        return False

def task_syn_file(cfg, srcfile, desfile, flag):
    syncFile(cfg, srcfile, desfile)
    flag.append("1")

def syncFile(cfg, srcfile, desfile):
    retry_times = 0
    global transfer_flag
    cmdline = "%s -P %s -pw %s %s %s@%s:%s" \
                %(cfg['tool']['pscp'], cfg['upload']['port'], cfg['upload']['password'], \
                srcfile, cfg['upload']['username'], cfg['upload']['ip'], desfile)

    while True:
        ret, text = getstatusoutput(cmdline)
        if ret != 0:
            if retry_times > 3:
                transfer_flag = False
                error_print(">>>>>upload %s to %s@%s failed error_text(%s)" %(srcfile, cfg['upload']['ip'], desfile, text))
                break
            retry_times += 1
            time.sleep(0.2)
            continue
        else:
            green_print(text)
            break

def is_match_file(file_name, filters):
    for filter in filters.split(';'):
        suffix = "." + filter
        if suffix == file_name[-len(suffix):] : return True
    else:
        return False

class Transfer():
    def cmdDes(self):
        return "自动传包工具"

    def onceTrans(self, local_dirs, remote_dir, is_syn = False):
        flag = []
        trans_file_num = 0

        for local_dir in local_dirs:
            for root, dirs, files in os.walk(local_dir, True):
                for filename in files:
                    if not is_match_file(filename, self.info['upload']['filter']) : continue
                    local_file_path = "%s\\%s" %(root, filename)
                    if is_syn and not isNeedSyn(local_file_path, True) : continue
                    remote_file_path = remote_dir + root.replace("\\", "/")
                    if remote_dir[-1] != "/": remote_file_path += "/"
                    _thread.start_new_thread(task_syn_file, (self.info, local_file_path, remote_file_path , flag))
                    trans_file_num += 1

        while len(flag) != trans_file_num:
            time.sleep(0.5)
        updateFlgFile()

        global transfer_flag
        if transfer_flag : green_print(">>>>>transfer all files success, enjoy!~~~~~~~~")

    def atuoTrans(self, local_dirs, remote_dir):
        local_files = {}

        for local_dir in local_dirs:
            if not os.path.isfile(last_flg_file_name):
                green_print("第一次同步，将同步所有文件.......")
                self.onceTrans(local_dir, remote_dir)
                green_print("第一次同步........完成")

            for root, dirs, files in os.walk(local_dir, True):
                for filename in files:
                    if not is_match_file(filename, self.info['upload']['filter']) : continue
                    local_file_path = "%s\\%s" %(root, filename)
                    remote_file_path = remote_dir + root.replace("\\", "/")
                    if remote_dir[-1] != "/": remote_file_path += "/"
                    local_files[local_file_path] = remote_file_path

        while True:
            time.sleep(0.05)

            for l_file_name, r_dir_name in list(local_files.items()) :
                if(isNeedSyn(l_file_name)):
                    syncFile(self.info, l_file_name, r_dir_name)
                    green_print(">>>>>>>>>Transfer %s to %s Finish." %(l_file_name.split("\\")[-1], remote_file_path))

    def printHelp(self):
        error_print("<模块名>： 合法的模块名列表: %s" %str(self.cfg['basic']['modList'])[1:-1])

    def parseArgv(self, cmd_index):
        argv = sys.argv[cmd_index:]

        if len(argv) == 0:
            self.printHelp()
            return False, "", ""

        if not argv[0] in self.cfg['basic']['modList']:
            self.printHelp()
            return False, "", ""

        if len(argv) == 1:
            return True, argv[0], "-a"

        return True, argv[0],argv[1]

    def init_env():
        pass

    def doIt(self, cmd_index):
        self.cfg = readCfg(".\\ci\\config.ini")
        self.info = readCfgFile(".\\ci\\atuoTransBuild.ini")
        ret, module, flag = self.parseArgv(cmd_index)
        if ret != True: return 1

        #初始化记录时间信息的文件名称
        global last_flg_file_name
        last_flg_file_name = ".\\ci\\localscript\\" + module + "mtime"
        tran_dirs = []
        tran_dirs.append(self.cfg[module]["rootdir"].replace("/", "\\"))
        if "depends" in list(self.cfg[module].keys()) :
            print("this is depends")
            for items in self.cfg[module]["depends"].split(","):
                tran_dirs.append(items)

        print(tran_dirs)
        try:
            if flag == "-a" :
                self.atuoTrans(tran_dirs, self.info["upload"]["basepath"])
            elif flag == "-o" :
                self.onceTrans(tran_dirs, self.info["upload"]["basepath"])
            elif flag == "-f" :
                self.onceTrans(tran_dirs, self.info["upload"]["basepath"], True)
            elif flag == "-i" :
                init_env()
            else:
                error_print(">>>>>error parameter, check it!")
                return False
        except KeyboardInterrupt:
            green_print(">>>>>interrupt by user!")
            return 1

        return True

