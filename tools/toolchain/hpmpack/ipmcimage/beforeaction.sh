#!/bin/bash
# 升级前执行脚本,暂时为空,修改了此文件需要修改CfgFileList.conf中相应的文件大小值,
# 打包时会自动脚本packethpm.sh会自动更新大小

if [ ! -f "/data/opt/pme/conf/customize.pfx" ]
then
    if [ -f "/opt/pme/web/conf/ssl.key/A-server.pfx" ]
    then
        rm -f /data/opt/pme/conf/ssl/B-server.pfx
        rm -f /data/backup/cert/B-server.pfx.bak
        rm -f /data/backup/cert/B-server.pfx.bak.sha256
        cp -pf /opt/pme/web/conf/ssl.key/A-server.pfx /data/opt/pme/conf/ssl/B-server.pfx
        chmod 400 /data/opt/pme/conf/ssl/B-server.pfx
    fi
    
    if [ -f "/opt/pme/web/conf/ssl.key/A-server-ECC.pfx" ]
    then
        rm -f /data/opt/pme/conf/ssl/B-server-ECC.pfx
        rm -f /data/backup/cert/B-server-ECC.pfx.bak
        rm -f /data/backup/cert/B-server-ECC.pfx.bak.sha256
        cp -pf /opt/pme/web/conf/ssl.key/A-server-ECC.pfx /data/opt/pme/conf/ssl/B-server-ECC.pfx
        chmod 400 /data/opt/pme/conf/ssl/B-server-ECC.pfx
    fi

    if [ -f "/opt/pme/conf/ssl/piv.conf" ]
    then
        rm -f /data/opt/pme/conf/ssl/piv.conf
        rm -f /data/backup/cert/piv.conf.bak
        rm -f /data/backup/cert/piv.conf.bak.sha256
        cp -pf /opt/pme/conf/ssl/piv.conf /data/opt/pme/conf/ssl/piv.conf
        chmod 400 /data/opt/pme/conf/ssl/piv.conf
    fi
fi