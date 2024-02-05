#!/bin/bash
# Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
set -e
dir_path="/opt/pme/bin/tsb"
pid_file="/var/run/tsb.pid"

cpu_limit() {
  sleep 1
  AGENT_CPU_LIMIT=10000
  pid=$(ps -ef | grep ${dir_path}/tsb_bmc | grep -v grep | awk '{print $2}')

  if [ -z "${pid}" ]; then
    echo "tsb_bmc not startup!"
    exit 1
  fi

  if [ ! -d "/sys/fs/cgroup/cpu/tsb_bmc-${pid}" ]; then
    mkdir /sys/fs/cgroup/cpu/tsb_bmc-${pid} >/dev/null 2>&1
  fi

  if [ ! -f "${pid_file}" ]; then
    touch ${pid_file} >/dev/null 2>&1
  fi

  echo ${pid} >${pid_file} 2>/dev/null
  echo ${AGENT_CPU_LIMIT} >/sys/fs/cgroup/cpu/tsb_bmc-${pid}/cpu.cfs_quota_us 2>/dev/null
  echo ${pid} >/sys/fs/cgroup/cpu/tsb_bmc-${pid}/cgroup.procs 2>/dev/null
}

stop() {
  # main process
  pid_of_agent=$(ps -ef | grep ${dir_path}/tsb_bmc | grep -v grep | awk '{print $2}')
  if [ -n "${pid_of_agent}" ]; then
    kill -15 ${pid_of_agent} >/dev/null 2>&1
    sleep 1
  fi
  if [ ! -f "${pid_file}" ]; then
    # 初始时cgroup下无该文件夹可能导致命令执行失败，此处跳过失败
    set +e
    rmdir /sys/fs/cgroup/cpu/tsb_bmc-* >/dev/null 2>&1
    set -e
  else
    pid=$(cat ${pid_file})
    if [ -d "/sys/fs/cgroup/cpu/tsb_bmc-${pid}" ]; then
      rmdir /sys/fs/cgroup/cpu/tsb_bmc-${pid} >/dev/null 2>&1
    fi
    rm ${pid_file} -f >/dev/null 2>&1
  fi
  echo "tsb stop"
}

start() {
  cd ${dir_path}
  ${dir_path}/tsb_bmc --slot-id 1 --cpu-id 1 --path ${dir_path} &
  sleep 2

  count=$(ps -ef | grep tsb_bmc | grep -v "grep" | wc -l)
  if [ $count == 0 ]; then
    ${dir_path}/tsb_bmc --slot-id 1 --cpu-id 1 --path ${dir_path} &
    echo "tsb retry..."
    sleep 2
  fi
  cpu_limit

  echo "tsb start"
}

case "$1" in
start)
  start
  ;;
restart)
  stop
  start
  ;;
stop)
  stop
  ;;
*)
  echo "Usage: $0 {start|stop|restart}"
  ;;
esac

exit 0
