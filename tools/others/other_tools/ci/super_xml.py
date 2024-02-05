# Copyright Â© Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
# -*- coding: utf-8 -*-
import threading
from time import ctime
import os, sys, subprocess, _thread
sys.path.insert(0, "./localscript")
#from xml_check import CprofileCheck, SuperXMLCheck
from profile_check import CprofileCheck



print('start at:', ctime())
os.chdir("..")
os.chdir("..")
os.chdir("..")
m = CprofileCheck()
ret = m.process()
print('all result =', ret)
print('all DONE at:', ctime())
sys.exit(ret)
