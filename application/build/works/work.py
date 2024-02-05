#!/usr/bin/python
# -*- coding: UTF-8 -*-

'''
功    能：
版权信息：华为技术有限公司，版本所有(C) 2021
修改记录：2021-10-14 创建
'''
import sys
import os
import subprocess
import traceback
import json
import importlib
import shutil
from string import Template
from multiprocessing import Process
cwd=os.path.dirname(__file__)
sys.path.append(f"{cwd}/../")
sys.path.append(f"{cwd}/../../")
sys.path.append(f"{cwd}/../../../")
from utils.common import Common
from utils.log import Logger

class Work(Process):
    name = "WorkBase"
    timeout = 1200
    description="基础类"
    common = Common()
    super_user_run = False
    logger = Logger().logger
    manifest_obj = None

    def __init__(self, config, work_name=""):
        Process.__init__(self)
        self.config = config
        # work中，编译选项初始化
        self.common.get_env_para(self.config.board_name, self.config.board_version)
        self.log_name = f"{self.config.build_path}/{self.__class__.__name__}_{work_name}.log"
        self.work_name = work_name
        self.rootfs_customization()
        pass

    def rootfs_customization(self):
        with open(f"{self.common.code_path}/customization/config.json") as json_file:
            cust = json.load(json_file)
            json_file.close()
        cfg = cust.get(self.config.board_name, None)
        if cfg is None:
            cfg = cust.get("default", None)
        cls = cfg.get("class_name", None)
        path = cfg.get("path", None)
        if cls is None or path is None:
            return
        work_py_file = importlib.import_module(f"{path}.{cls}")
        work_class = getattr(work_py_file, cls)
        self.customization = work_class(self, self.config)
        self.logger.info("load customization script, file:{}, class:{}".format(path, cls))

    def get_path(self, path):
        return os.path.realpath(os.path.join(self.common.code_path, path))

    # 子类必须实现run方法,未实现时异常
    def run(self):
        self.logger.info(f"{__file__} error")
        raise Exception

    # 安装函数，将各work的产出安装到config.rootfs目录中
    def install(self):
        pass

    def deal_conf(self, config_dict):
        """
        处理每个work的私有配置"work_config"，只有work类有set_xxx类方法的属性可以设置
        """
        if not config_dict:
            return
        for conf in config_dict:
            try:
                exist = getattr(self, f"set_{conf}")
                val = config_dict.get(conf)
                exist(val)
            except Exception as e:
                raise Exception(f"Invalid conf {conf}, {e}, {traceback.print_exc()}")

    def run_command(self, command, ignore_error=False, return_result=False, super_user=False, retry=0):
        """
        参数:
        command: 传入的命令，命令请逐条写，不要使用&&语法
        ignore_error: 是否忽略命令执行结果
        return_result： 是否返回执行结果
        super_user: 是否使用root用户运行
        如果ignore_error为False，命令返回码非0时则打印堆栈和日志并触发异常，中断构建
        retry 重试次数 配合 ignore_error为True 使用
        """
        call_file = os.path.basename(sys._getframe(1).f_code.co_filename)
        call_line = sys._getframe(1).f_lineno
        if "&&" in command and (super_user is True or self.super_user_run is True):
            self.logger.error(f"{call_file}:{call_line}: {command}\n===>>>>> Command concatenation is not allowed <<<<<===")

        if self.super_user_run is True or super_user is True:
            command = "{} {}".format("LD_PRELOAD= sudo LD_PRELOAD=${LD_PRELOAD} -E", command)

        log_command = command
        if return_result is False:
            log_command = "{} >> {}".format(command, os.path.basename(self.log_name))
            command = f"{command} >> {self.log_name} 2>&1"
        try:
            ret = subprocess.run(command, shell=True, check=True, capture_output=True)
            self.logger.info(f"{call_file}:{call_line}: {log_command} successfully")
            if return_result:
                return ret.stdout.decode().strip('\n').split('\t')[0]
        except Exception as e:
            if not ignore_error:
                self.logger.info(f"{call_file}:{call_line}: command({log_command}) error, log:")
                subprocess.run(f"cat {self.log_name}", shell=True)
                raise e
            else:
                if retry > 0:
                    retry = retry - 1
                    if retry == 0:
                        ignore_error=False
                    self.logger.info(f"retry command is {command} and left {retry} times")
                    self.run_command(command, ignore_error, return_result, super_user, retry)

                self.logger.info(f"{call_file}:{call_line}: command: {log_command} executed exception, {e}")

    # 只拷贝目录下的文件，排除子文件夹。
    def copy_file_exclude_subdir(self, src_dir, dst_dir):
        if not os.path.isdir(src_dir):
            self.logger.error(f"{src_dir} is not directory")
        self.common.check_path(dst_dir)
        for file in os.listdir(src_dir):
            if os.path.isfile(f"{src_dir}/{file}"):
                shutil.copy(f"{src_dir}/{file}", dst_dir)
        return
    
    # 创建硬链接
    def link(self, src, dst):
        if os.path.isfile(dst):
            os.unlink(dst)
        if os.path.dirname(dst) != "":
            os.makedirs(os.path.dirname(dst), exist_ok=True)
        call_file = os.path.basename(sys._getframe(1).f_code.co_filename)
        call_line = sys._getframe(1).f_lineno
        self.logger.info(f"{call_file}:{call_line} ln {src} {dst}")
        os.link(src, dst)

    # 模板化，manifest.yml替换时使用
    def get_template(self) -> dict:
        template = {
            "code_root": self.common.code_root,
            "open_code_root": self.common.open_code_root,
            "product": f"{self.common.code_path}/product",
            "manufacture": f"{self.common.code_path}/manufacture",
            "hpm_build_dir": f"{self.config.build_path}/hpm_build",
            "board_name": self.config.board_name,
            "work_out": self.config.work_out,
            "build_type": self.config.build_type,
            "output_path": self.common.output_path,
            "sdk_path": f"{self.config.buildimg_dir}/sdk"
        }
        return template

    # 依赖manifest的files复制文件
    def copy_manifest_files(self, files):
        if files is None:
            return
        for file in files:
            filename = file.get("file")
            if filename is None:
                continue
            if not os.path.isfile(filename) and not os.path.isdir(filename):
                self.logger.error(f"File {filename} is neither file nor dir, maybe not exist")
                continue
            dst = file.get("dst")
            if dst is None:
                dst = os.path.basename(filename)
            elif "/" in dst:
                dst = str(Template(dst).safe_substitute(self.get_template()))
                dirname = os.path.dirname(dst)
                os.makedirs(dirname, exist_ok=True)

            condition = file.get("condition")
            match = True
            if condition is not None:
                for key, val in condition.items():
                    if key == "build_type":
                        if self.config.build_type != val:
                            match = False
                            break
                    else:
                        self.logger.warning(f"Unkown condition: {key}, exit")
                        sys.exit(-1)
            if not match:
                continue
            if file.get("template"):
                # 模板文件替换内容后复制
                with open(filename, "r") as fp:
                    content = fp.read()
                    fp.close()
                content = Template(content)
                content = content.safe_substitute(self.get_template())
                with open(dst, "w") as fp:
                    fp.write(content)
                    fp.close()
            else:
                # 非模板文件直接复制
                if os.path.isdir(filename):
                    copy_type = file.get("copy_type")
                    if copy_type == "copy_file_exclude_subdir":
                        self.copy_file_exclude_subdir(filename, dst)
                    else:
                        self.logger.info(f"copy dir ({filename}) to ({dst})")
                        ignore_patterns = file.get("ignore_patterns")
                        dirs_exist_ok = file.get("dirs_exist_ok")
                        symlinks = file.get("symlinks")
                        if ignore_patterns is not None:
                            ignore_patterns = ignore_patterns.split(' ')
                            ignore_patterns = shutil.ignore_patterns(*ignore_patterns)
                        if dirs_exist_ok is not None:
                            dirs_exist_ok = True
                        if symlinks is not None:
                            symlinks = True
                        try:
                            shutil.copytree(filename, dst, dirs_exist_ok=dirs_exist_ok, symlinks=symlinks, ignore = ignore_patterns)
                        except Exception as e:
                            self.logger.error(e.__str__())
                else:
                    self.link(filename, dst)

    # 模板化加载manifest.yml
    def _get_manifest_yaml(self, filename):
        if self.manifest_obj is not None:
            return self.manifest_obj
        manifest_conf = None
        template = self.get_template()
        try:
            manifest_conf = self.common.yaml_load_template(filename, template)
        except:
            pass
        if manifest_conf is None:
            self.logger.info('Load {} return None'.format(filename))
            return None
        self.manifest_obj = manifest_conf
        return self.manifest_obj

    # 从manifest中获取属性值
    def get_manufacture_config(self, src_path, key: str):
        """获取manifest.yml当中的配置
        参数:
            key (str): 要获取的配置，采用路径类似格式，比如'manufacture/05023VAY'
        返回值:
            None: 未找到配置
            str: 配置的值
        """
        # manifest_yml 文件路径
        manifest_yml = f"{src_path}"
        manifest_conf = self._get_manifest_yaml(manifest_yml)
        splits = key.split("/")
        processed = ""
        for split in splits:
            if processed == "":
                processed = split
            else:
                processed = processed + "/" + split
            manifest_conf = manifest_conf.get(split)
            if manifest_conf is None:
                self.logger.info('Cannot get key value {} from yaml file ({}), not configured'.format(processed, manifest_yml))
                return None
        return manifest_conf

# 以下为提供脚本单独调试功能示例代码，在application/build目录下运行python3 works/xxxx.py
if __name__ == "__main__":
    cfg = Config("2488hv6", "target_personal")
    wk = Work(cfg)
    wk.run()
