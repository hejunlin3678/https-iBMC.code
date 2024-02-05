#!/bin/bash
# Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
set -e
dir_path="/opt/pme/bin/intrusion-detection"
pid_file="/var/run/intrusion_detection.pid"
export LD_LIBRARY_PATH=/opt/pme/bin/intrusion-detection:${LD_LIBRARY_PATH}

cpu_limit() {
  sleep 1
  AGENT_CPU_LIMIT=5000
  pid=$(ps -ef | grep ${dir_path}/seceye-agent | grep -v grep | awk '{print $2}')

  if [ -z "${pid}" ]; then
    echo "seceye-agent not startup!"
    exit 1
  fi

  if [ ! -d "/sys/fs/cgroup/cpu/seceye-agent-${pid}" ]; then
    mkdir /sys/fs/cgroup/cpu/seceye-agent-${pid} >/dev/null 2>&1
  fi

  if [ ! -f "${pid_file}" ]; then
    touch ${pid_file} >/dev/null 2>&1
  fi

  echo ${pid} >${pid_file} 2>/dev/null
  echo ${AGENT_CPU_LIMIT} >/sys/fs/cgroup/cpu/seceye-agent-${pid}/cpu.cfs_quota_us 2>/dev/null
  echo ${pid} >/sys/fs/cgroup/cpu/seceye-agent-${pid}/cgroup.procs 2>/dev/null
}

stop() {
  # main process
  pid_of_agent=$(ps -ef | grep ${dir_path}/seceye-agent | grep -v grep | awk '{print $2}')
  if [ -n "${pid_of_agent}" ]; then
    kill -15 ${pid_of_agent} >/dev/null 2>&1
    sleep 1
  fi
  set +e
  if [ ! -f "${pid_file}" ]; then
    rmdir /sys/fs/cgroup/cpu/seceye-agent-* >/dev/null 2>&1
  else
    pid=$(cat ${pid_file})
    if [ -d "/sys/fs/cgroup/cpu/seceye-agent-${pid}" ]; then
      rmdir /sys/fs/cgroup/cpu/seceye-agent-${pid} >/dev/null 2>&1
    fi
    rm ${pid_file} -f >/dev/null 2>&1
  fi
  set -e
  echo "intrusion-detection stop"
}

start() {
  cd ${dir_path}
  ${dir_path}/seceye-agent --mgmt-socket-path /var/run/security_policy.socket --slot-id 1 --cpu-id 1 --path ${dir_path} &
  sleep 2

  count=$(ps -ef | grep seceye-agent | grep -v "grep" | wc -l)
  if [ $count == 0 ]; then
    ${dir_path}/seceye-agent --mgmt-socket-path /var/run/security_policy.socket --slot-id 1 --cpu-id 1 --path ${dir_path} &
    echo "intrusion-detection retry..."
    sleep 2
  fi
  cpu_limit

  echo "intrusion-detection start"
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
