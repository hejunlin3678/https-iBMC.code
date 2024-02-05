# -*- coding:utf-8 -*-
#-------------------------------------------------------------------------------
# Name:        自动传包,编译, 云上云下传输使用的脚本
# Purpose:
#
# Author:      h00168999
#
# Created:     28/07/2014
# Copyright:   (c) h00168999 2014
# Licence:     <your licence>
#-------------------------------------------------------------------------------

import os , sys, subprocess
sys.path.insert(0, ".\\localscript")
from basicCmdClass import *
from TransferClass import *
from EtransferClass import *
from BuildClass import *

class CMain(CCmdBase):
    def __init__(self):
        CCmdBase.__init__(self)
        self.defAction = "help"
        self.subActList = {
            'atrans' : Transfer,  #自动上传
            'build' : Build,      #编译相应的app,可以通过配置, 上传数传系统.
            'etrans': Etransfer   #传输到数传
        }

    def cmdDes(self):
        return ""

os.chdir("..")
m = CMain()

if True != m.doIt(1):
    exit(1)
else:
    exit(0)
