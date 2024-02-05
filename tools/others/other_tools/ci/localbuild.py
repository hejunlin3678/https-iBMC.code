# Copyright Â© Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
# -*- coding: utf-8 -*-
import os , sys, subprocess
import multiprocessing
from multiprocessing import Pool, freeze_support, Lock

if __name__=='__main__':
    now_path = os.getcwd()
    sys.path.insert(0, ("%s/localscript"%now_path))
from basicCmdClass import *
from lint import *
from ccn import *
from profile_check import CprofileCheck


class CMain(CCmdBase):
    def __init__(self):
        CCmdBase.__init__(self)
        self.defAction = "help"
        self.subActList = { 
            'lint': CLint,
            'ccn': CNcss,
            'xml': CprofileCheck,
        }

    def cmdDes(self):
        return ""

m = CMain()
def for_doIt(i):
    try:
        resulttmp = m.doIt(i,lock)
    except:
        exit(1)
    else:
        return resulttmp
    #return m.doIt(i, lock)

def init(l):
    global lock
    lock = l
    
if __name__=='__main__':
    freeze_support()
    os.chdir("..")
    os.chdir("..")
    os.chdir("..")
    timestart = time.time()
    result_list = []
    num_list = []
    argv_list = []
    k = 2
    if (((sys.argv[1] != 'xml') or (sys.argv[2] != 'all')) and (len(sys.argv)<4)):
        if True != m.doIt(0):
            exit(1)
        else:
            exit(0)
    if len(sys.argv) > 3 :
        while k < len(sys.argv):
            argv_list.append(sys.argv[k])
            k = k + 1
        if True != m.doIt(argv_list): 
            exit(1)
        else:
            print("Checking these boards start...waiting for them checking done.......")
            lock = Lock()
            o = Pool(5, initializer=init, initargs=(lock,))
            result_list = o.map(for_doIt, argv_list)
            o.close()
            o.join()    

            print("check result:%s" % result_list)
            timeuse = time.time() - timestart
            print("eclipse: %s" % timeuse)
            exit(0)
    
    print("Checking all boards start...waiting for them checking done.......")
    num_list = list(range(1, 56))
    lock = Lock()
    p = Pool(9, initializer=init, initargs=(lock,))
    try:
        result_list = p.map(for_doIt, num_list)
    except:
        exit(1)
    #result_list = p.map(for_doIt, num_list)
    p.close()
    p.join()    

    print("check result:%s" % result_list)
    timeuse = time.time() - timestart
    print("eclipse: %s" % timeuse)
    exit(0)


