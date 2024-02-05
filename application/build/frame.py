#!/usr/bin/python
# -*- coding: UTF-8 -*-

'''
功    能：构建框架
版权信息：华为技术有限公司，版本所有(C) 2021
修改记录：2021-10-11 创建
'''
import json
import importlib
import os
import time
import sys
import argparse
import re
import threading
import traceback
import signal
from multiprocessing import Process,Queue
from socket import *
import subprocess

from concurrent.futures import ThreadPoolExecutor
from utils.log import Logger
from utils.config import Config

sys.tracebacklimit = 0

parser = argparse.ArgumentParser(description="build target")
parser.add_argument("-t", "--target", help="target file name", default="target_personal")
parser.add_argument("-b", "--board_name", help="Board name", default="2488hv6")
parser.add_argument("-d", "--debug_frame", help="debug flag for build frame", const="debug", action="store_const")
# 默认从cmc下载，-f表示下载源码并编译
parser.add_argument("-f", "--from_source", help="From source required", action="store_true")
# 区分ro、rw、dft
parser.add_argument("-y", "--build_type", help="Build debug or release or dft_separation", default="debug")
# 构建特定app
parser.add_argument("-p", "--app_name", help="Build special app", default="")
parser.add_argument("-o", "--open_local_debug", help="open_local_debug", action="store_true")
parser.add_argument("-m", "--maca", help="Replace pme with maca.", action="store_true")
parser.add_argument("-c", "--fuzz_cov", help="Fuzz test coverage", action="store_true")
parser.add_argument("-a", "--asan_test", help="Asan test for fuzz", action="store_true")
args = parser.parse_args()

logger = Logger().logger
# 注意端口号需要与V3的配置不同
work_server_port = 62345
# 用于通知主程序 server已经启动
q = Queue(1) 

def read_json(json_path):
    '''
    功能描述：读取json内容
    '''
    if not os.path.exists(json_path):
        logger.error(f"json_path is not exist!")
        os._exit(1)

    with open(json_path, "r") as json_file:
        return json.load(json_file)
    logger.error(f"Open json_path failed!")
    os._exit(1)

class WorkStatusServer(Process):
    """
    全局任务状态管理器
    """
    def __init__(self):
        Process.__init__(self)
        self.task_status_dict = {}
        self.fd = socket(AF_INET, SOCK_STREAM)
        self.fd.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
        address = ("", work_server_port)
        try:
            self.fd.bind(address)
            self.fd.listen(10)
            # 向主进程发送准备好的消息
            q.put(1)
        except Exception as e:
            logger.error(f"Work status error: {e}")
            logger.error(f"Please run command manually: killall python")
            os._exit(7)

    def run(self):
        logger.info(f"start listen socket: {self.fd}")
        start_time = time.time()
        while True:
            try:
                cur_time = time.time()
                if int(cur_time - start_time)/60 > 60:
                    start_time = time.time()
                    logger.error(f"task_status_dict is : {self.task_status_dict}")
                    logger.error("time is over 60 min !")

                cli_fd,cli = self.fd.accept()
                recv = cli_fd.recv(1024)
                recv_json = recv.decode()
                msg = json.loads(recv_json)
                # logger.info(f"recv msg:{msg}")
                act = msg["action"]
                resp = ""
                wn = msg["work_name"]
                if act == "set_if_not_exist":
                    st = msg["work_status"]
                    resp = "False"
                    status = self.task_status_dict.get(wn)
                    if status is None:
                        self.task_status_dict[wn] = st
                        resp = "True"
                elif act == "set":
                    st = msg["work_status"]
                    self.task_status_dict[wn] = st
                    resp = "True"
                elif act == "get":
                    st = self.task_status_dict.get(wn)
                    if st is None:
                        resp = "None"
                    else:
                        resp = st

                cli_fd.send(resp.encode())
                cli_fd.close()
            except Exception as e:
                logger.info(f"recv message failed, msg:{e}")

class WorkStatusClient(Process):
    """
    全局任务状态管理器
    """
    def __init__(self):
        Process.__init__(self)
        self.task_status_dict = {}

    def __comm(self, msg):
        fd = socket(AF_INET, SOCK_STREAM)
        try:
            fd.connect(("", work_server_port))
            fd.send(msg.encode())
            msg = fd.recv(1024)
            fd.close()
            return msg.decode()
        except Exception as e:
            logger.error(f"recv message failed, msg:{e}")
            os._exit(3)

    # 获取任务状态
    def get(self, target_name, work_name):
        return self.__comm("{\"action\":\"get\", \"work_name\":\"%s/%s\"}"%(target_name, work_name))

    # 任务不存在时创建任务状态，否则返回False
    def set_if_not_exist(self, target_name, work_name, status):
        resp = self.__comm("{\"action\":\"set_if_not_exist\", \"work_name\":\"%s/%s\", \"work_status\":\"%s\"}"%(target_name, work_name, status))
        if resp == "True":
            return True
        else:
            return False

    # 设置任务状态
    def set(self, target_name, work_name, status):
        resp = self.__comm("{\"action\":\"set\", \"work_name\":\"%s/%s\", \"work_status\":\"%s\"}"%(target_name, work_name, status))

ws_client = WorkStatusClient()

def wait_finish(target_name, wait_list, prev_work_name):
    if not wait_list:
        return True
    start_time = time.time()
    while True:
        finish = True
        time.sleep(0.1)
        for work_name in wait_list:
            cur_time = time.time()
            status = ws_client.get(target_name, work_name)
            if status == "Done":
                continue
            if status == "Failed":
                ws_client.set(target_name, prev_work_name, "Failed")
                raise Exception(f"Build failed, work {work_name} error")
            finish = False
            if int(cur_time - start_time)/60 > 10:
                start_time = time.time()
                logger.info(f"target_name:{target_name} work_name:{work_name} status:{status}")
            break
        if finish:
            return True

class WorkerScheduler(Process):
    '''
    '''
    def __init__(self, target_name, config, work):
        Process.__init__(self)
        self.config = config
        self.work = work
        self.target_name = target_name

    def run(self):
        '''
        功能描述：执行work
        '''
        work_name = self.work["work_name"]
        logger.info(f"work ({self.target_name}/{work_name}) start...")
        ret = wait_finish(self.target_name, self.work.get("wait"), work_name)
        if not ret:
            logger.error(f"wait_finish failed, target={self.target_name}, work={work_name}")
            os._exit(1)
        work_path = self.work.get("path")
        class_name = self.work.get("class_name")
        target_config = self.work.get("target_config")
        self.config.deal_conf(target_config)
        # 如果未指定类时，不需要执行
        if work_path is not None and class_name is not None:
            work_py_file = importlib.import_module(work_path)
            work_class = getattr(work_py_file, self.work["class_name"])
            work_x = work_class(self.config, work_name)
            # work配置项和target配置项
            work_config = self.work.get("work_config")
            work_x.deal_conf(work_config)
            ret = ws_client.set_if_not_exist(self.target_name, work_name, "Running")
            if not args.debug_frame:
                if ret:
                    # 创建进程并且等待完成或超时
                    logger.info(f"work ({work_name}) running")
                    work_x.run()
                else:
                    # 不需要创建进程，等待任务执行完成即可
                    wait_list = []
                    wait_list.append(work_name)
                    ret = wait_finish(self.target_name, wait_list, work_name)
                    if not ret:
                        logger.error(f"Build work({work_name}) error")
                        os._exit(1)

            logger.info(f"work ({work_name}) installing...")
            if not args.debug_frame:
                work_x.install()

        # 创建子任务
        exec_works(self.target_name, self.work.get("subworks"), work_name, self.config)
        # 创建include_target子任务
        target_include = self.work.get("target_include")
        if target_include:
            create_target_scheduler(work_name, target_include, self.config)
        logger.info(f"work ({work_name}) finish...")
        ws_client.set(self.target_name, self.work["work_name"], "Done")
        os._exit(os.EX_OK)

class Worker():
    """
    任务执行器
    """
    work_name = ""
    target_name = ""

    def exec_work(self, target_name, work, config):
        ws = WorkerScheduler(target_name, config, work)
        ws.start()
        ws.join()

        if ws.exitcode != 0:
            if ws.is_alive():
                return
            ws_client.set(self.target_name, self.work_name, "Failed")
            raise Exception(f"work {self.work_name} exit with error, maybe timeout")

    def run(self, target_name, work, config):
        self.work_name = work["work_name"]
        self.target_name = target_name
        t = threading.Thread(target=self.exec_work, args=(target_name, work, config))
        t.start()

def exec_works(target_name, work_list, prev_work_name, config):
    if not work_list:
        return
    # 创建任务并等待完成
    wait_list = []
    for work in work_list:
        worker = Worker()
        worker.run(target_name, work, config)
        wait_list.append(work["work_name"])
    wait_finish(target_name, wait_list, prev_work_name)

def create_target_scheduler(target_alias, target_name, config = None):
    logger.info(f"create new target({target_name}) scheduler")
    if config is None:
        target_file = f"./target/{target_name}.json"
    else:
        target_file = f"{config.common.code_path}/target/{target_name}.json"
    if not os.path.isfile(target_file):
        raise Exception(f"target file ({target_file}) not exist")
    is_build_for_open = False
    if target_name == "target_build_for_open":
        board_name = "BM320"
        is_build_for_open = True
        logger.info(f"target_build_for_open use board_name BM320")
    else:
        board_name = args.board_name
    # 创建配置
    if config is None:
        config = Config(board_name, target_name)
        config.set_build_type(args.build_type)
        config.set_build_for_open(is_build_for_open)
        config.set_app_name(args.app_name)
        config.from_source = args.from_source
        config.open_local_debug = args.open_local_debug
        config.use_maca = args.maca
        config.fuzz_cov = args.fuzz_cov
        config.asan_test = args.asan_test

    # 打印配置
    config.print_config()
    work_list = read_json(target_file)
    # 打印任务清单
    logger.debug(f"work list:{work_list}")
    # 创建任务调度器
    exec_works(target_alias, work_list, "TOP", config)

def process_killer():
    logger.error(f"Error: 构建目标({args.target})失败，请参考“构建检查”和下载“全量构建日志（右上角-日志-全量构建日志）”检查错误！！！！！！！！")
    pid_list = subprocess.getstatusoutput("ps -elf | grep frame.py | grep -v 'auto' | awk '{print $4}' | tr '\n' ' '")[1].split(" ")
    for pid in pid_list:
        os.kill(int(pid), signal.SIGTERM)

if __name__ == "__main__":
    print(f"debug_frame:{args.debug_frame}, build args:{args}")

    ws_server = WorkStatusServer()
    ws_server.start()
    try:
        # 等待sever起来
        q.get()
        create_target_scheduler(args.target, args.target)
    except Exception as e:
        logger.info(e)
        killer = Process(target=process_killer, args="")
        killer.start()

    ws_server.kill()
    logger.info(f"targets ({args.target}) succcess")