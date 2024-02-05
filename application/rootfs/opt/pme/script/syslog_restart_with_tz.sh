#!/bin/sh
# AR20230707880810 新增IPMI命令实现开启/关闭夏令时操作, 在调用该脚本时，不再传入 time_zone 参数
echo "destination d_pme_dfm_debug { file(\"/var/log/pme/dfm_debug_log\"); };" > /data/etc/extern.conf
echo "destination d_pme_security { file(\"/var/log/pme/security_log\" perm(0640) group(103)); };" >> /data/etc/extern.conf
echo "destination d_kern { file(\"/var/log/pme/linux_kernel_log\"); };" >> /data/etc/extern.conf

echo "filter f_driver   { facility(kern) and level(notice) and match(\"edma\" value(\"MESSAGE\")); };" >> /data/etc/extern.conf
echo "destination d_driver { file(\"/var/log/pme/agentless_driver_log\" perm(0640) ); };" >> /data/etc/extern.conf
echo "log { source(s_kern); filter(f_driver); destination(d_driver); };" >> /data/etc/extern.conf

chmod 640 /var/log/pme/agentless_driver_log >/dev/null 2>&1
chmod 640 /var/log/pme/security_log >/dev/null 2>&1

/bin/kill -HUP `cat /var/run/syslogd.pid 2> /dev/null`