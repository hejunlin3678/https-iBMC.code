#!/usr/bin/env python
# coding: utf-8
# Copyright © Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.

import json
import os
import shutil
import uuid
import subprocess
import psutil
from utils.common import Common
from utils.log import Logger


class Config:
    # RTOS版本号
    rtos_version = "208.5.0"
    arm_dic = {"V3": "arm", "V5": "arm", "V6": "arm64"}

    def __init__(self, board_name = "2488hv6", target="target_personal"):
        # 单板名
        self.board_name = board_name
        # 构建目标的文件名，不包含py尾缀，文件存储在application/build/target目录
        self.target = target
        # 待构建的app名，当前用一work_build_app.py
        self.app_name = ""
        # 固件从源码下载后编译，默认为false，表示从cmc直接下固件
        self.from_source = False
        # 是否开放调试开关 现用于本地调试源码构建配合from_source使用
        self.fuzz_cov = False
        # 是否获取Fuzz测试代码覆盖率
        self.asan_test = False
        # 是否进行asan检测
        self.open_local_debug = False
        # 是否启用maca构建，默认不开启
        self.use_maca = False
        # TODO:待理解
        self.local = ""
        self.common = Common()
        self.logger = Logger().logger
        self.board_version = ""
        self.set_board_name(board_name)
        # step???
        self.step = "ultimate"
        # 是否需要合作伙伴模式，默认值为"",可用方法 set_partner_mode 设置
        self.partner_compile_mode = ""
        # 编译选项，默认值为 False，可用方法 set_enable_arm_gcov 设置
        self.enable_arm_gcov = False
        # 是否需要进行签名
        self.sign = ""
        # 编译 debug 或者 release 或者 装备分离 版本的app包
        self.build_path = ""
        self.rootfs_path = ""
        self.cache_path = ""
        self.work_out = ""
        self.work_config = ""
        self.set_build_type()
        self.debug = False
        self.set_debug(False)
        self.cb_flag = False
        self.build_for_open = False
        self.cpu_count = psutil.cpu_count()
        # info_tree为Ture表示不执行具体下载等命令，仅生成xml文件给ci用于刷新libing结果树数据
        self.info_tree = False
        cb_list = self.common.py_find_dir(".*_cb_flag_productid.list", 
                f"{self.common.code_root}/application/src/resource/board/{self.board_name}")
        if len(cb_list):
            self.cb_flag = True
        full_src_build = os.environ.get("full_src_build")
        print("===============================>>>>>>>>>>>>>>>>>>>>>> From source build flag: {}".format(full_src_build))
        if full_src_build == "true":
            self.full_src_build = True
        else:
            self.full_src_build = False
        self.logger.info("show platforms ")
        subprocess.run("find {} -type f".format(os.path.join(self.common.temp_path, "platforms")), shell=True)

    def set_debug(self, debug):
        '''
        debug 用于标记是否为调试状态
        '''
        self.debug = debug

    def set_step(self, arg_step):
        '''
        参数：arg_step 可选值："build"/"ultimate"/"standard"/""
        '''
        self.step = arg_step

    def set_partner_compile_mode(self, arg_mode):
        '''
        是否需要伙伴模式，默认值为""
        参数：mode 可选值 online，repair，noverify
        '''
        self.partner_compile_mode = arg_mode

    def set_enable_arm_gcov(self, isEnable):
        '''
        是否添加编译选项 -DGCOV=ON
        参数：isEnable 是否添加-DGCOV=ON 可选值：True;False
        '''
        self.enable_arm_gcov = isEnable

    def set_sign(self, arg_sign):
        '''
        是否需要签名
        '''
        self.sign = arg_sign

    def set_app_name(self, app_name="all"):
        if app_name == "all":
            self.app_name = ""
        else:
            self.app_name = app_name

    def set_build_type(self, arg_build_type="debug"):
        '''
        这只编译app的cmake编译选项
        参数：arg_build_type 可选值："debug";"release";"dft_separation"
        '''
        # 可以编译的版本
        build_type_turple = ("debug", "release", "dft_separation")
        if arg_build_type not in build_type_turple:
            self.logger.info("build_type para invalid! please input debug or release or dft_separation.")
            return

        self.build_type = arg_build_type
        # 构建过程文件存储在以下目录
        self.build_path = os.path.join(self.common.temp_path, self.build_type)
        self.rootfs_path = os.path.join(self.build_path, 'tmp_root')
        self.work_out = os.path.join(self.build_path, 'output')
        self.cache_path = os.path.join(self.build_path, 'cache')
        self.buildimg_dir = os.path.join(self.build_path, "buildimg")
        os.makedirs(self.build_path, 0o755, exist_ok=True)
        os.makedirs(self.work_out, 0o755, exist_ok=True)
        os.makedirs(self.rootfs_path, 0o755, exist_ok=True)
        os.makedirs(self.cache_path, 0o755, exist_ok=True)
        os.makedirs(self.buildimg_dir, 0o755, exist_ok=True)
        os.makedirs(self.common.work_out, 0o755, exist_ok=True)

    def set_info_tree(self, arg_info_tree):
        self.info_tree = arg_info_tree

    def deal_conf(self, config_dict):
        if not config_dict:
            return
        for conf in config_dict:
            try:
                method = getattr(self, f"set_{conf}")
                method(config_dict.get(conf))
            except Exception as e:
                raise Exception(f"Invalid conf {conf}")

    def print_config(self):
        print(
            f"board_name={self.board_name} from_source={self.from_source} build_type={self.build_type}")

    def set_board_name(self, board_name):
        # 设置单板板名和基本属性
        self.board_name = board_name
        if self.board_name == "":
            self.board_version = "V5"
        else:
            self.board_version = self.common.get_board_version(self.board_name)
        if self.board_version == "V5":
            self.cross_prefix = "arm32-linux-gnueabi"
            self.cross_host = "arm-linux-gnueabi"
            self.cross_host_alias = "arm32-linux-gnueabi"
            self.cross_build_dir = "arm32-unknown-linux-gnueabi"
            self.cross_compile = "arm32-linux-gnueabi-"
            self.cross_compile_install_path = "/opt/hcc_arm32le"
            self.cpu = "arm32A9le"
            self.sysroot_base = f"/opt/RTOS/{self.rtos_version}/arm32A9le_5.10_ek_preempt_pro"
            self.kernel = "5.10_ek_preempt_pro"
            self.platform = "platform_v5"
            self.strip = f"{self.cross_compile_install_path}/bin/{self.cross_host}-strip"
            self.gcc = f"{self.cross_compile_install_path}/bin/{self.cross_host}-gcc"
            self.gxx = f"{self.cross_compile_install_path}/bin/{self.cross_host}-g++"
        elif self.board_version == "V6":
            self.cross_prefix = "aarch64-target-linux-gnu"
            self.cross_host = "aarch64-linux-gnu"
            self.cross_host_alias = "aarch64-linux-gnu"
            self.cross_build_dir = "aarch64-unknown-linux-gnu"
            self.cross_compile = "aarch64-unknown-linux-gnu-"
            self.cross_compile_install_path = "/opt/hcc_arm64le"
            self.cpu = "arm64le"
            self.sysroot_base = f"/opt/RTOS/{self.rtos_version}/arm64le_5.10_ek_preempt_pro"
            self.kernel = "5.10_ek_preempt_pro"
            self.platform = "platform_v6"
            self.strip = f"{self.cross_compile_install_path}/bin/{self.cross_prefix}-strip"
            self.gcc = f"{self.cross_compile_install_path}/bin/{self.cross_prefix}-gcc"
            self.gxx = f"{self.cross_compile_install_path}/bin/{self.cross_prefix}-g++"

        # build.py 构建app的全局变量，原方式从参数传递
        self.arch = self.arm_dic.get(self.board_version)
        # 是否支持装备分离
        self.support_dft_separation = self.common.get_support_dft_separation_flag(self.board_name)
        # 设置完成boardname后，需要设置编译类型
        self.common.get_env_para(self.board_name, self.board_version)

    def set_env(self, key, value):
        os.environ[key] = value

    def set_env_global(self):
        self.set_env("G_PREFIX", self.common.third_path)
        self.set_env("G_HTTPD_PREFIX", "/opt/pme/web")
        self.set_env("G_BOOST_DIR", self.common.third_path+"/package/boost_1_54_0/stage/lib")
        self.set_env("G_SYSROOT", f"{self.sysroot_base}/sdk")
        self.set_env("G_PKG_CONFIG_PATH", self.common.third_path+"/lib/pkgconfig")
        self.set_env("CC", f"{self.cross_prefix}-gcc")
        self.set_env("CXX", f"{self.cross_prefix}-g++")
        self.set_env("STRIP", f"{self.cross_prefix}-strip")
        self.set_env("G_HOST", self.cross_host)
        self.set_env("CROSS_COMPILE", self.cross_compile)
        self.set_env("G_BUILDDIR", self.cross_host)
        self.set_env("G_HOST_ALIAS", self.cross_host_alias)
        self.set_env("G_NGINX_PREFIX", "/opt/pme/web")
        cmd = f"source /opt/RTOS/{self.rtos_version}/dlsetenv.sh -p {self.cpu}_{self.kernel} --sdk-path={self.cross_compile_install_path}"
        env_file = f"{self.build_path}/env_{os.getpid()}_{str(uuid.uuid4())}.txt"
        subprocess.run(f"{cmd} && env > {env_file}", shell=True)
        self.common.read_file_set_env(env_file)

    def get_flag_need_v3(self):
        if self.board_version == "V5":
            return "NO_V3"

        # 两种场景：v3单板；或者maca替代pme的构建方式
        if self.use_maca:
            return "V3_MACA"
        board_list = ["BM320_V3"]
        if self.board_name in board_list:
            return "V3_BOARD"
        return "NO_V3"
    
    def set_build_for_open(self, isEnable):
        '''
        build_for_open 用于标记是否为开放裁剪流程
        '''
        self.build_for_open = isEnable