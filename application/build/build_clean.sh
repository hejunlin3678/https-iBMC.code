#!/bin/bash
# Copyright © Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
# 清除构建产生的中间件，包括文件改动、目录改动、文件格式和权限等

set -e
# 1.变量

G_RELATIVE_DIR="../../" # 代码仓目录相对当前脚本的路径,以符号'.'(dot)开头
G_CURRENT_DIR=$(dirname $(realpath $0)) # 当前文件路径
G_CODE_BASE=$(realpath ${G_CURRENT_DIR}/${G_RELATIVE_DIR}) # 代码仓路径
# DEBUG选项
DEBUG=0 # 默认为0；设置为1 则查看删除的中间件;
DELETED_ITEM=./build/deleted_item.txt

# 2.执行

echo "build_clean begin..."
cd "${G_CODE_BASE}"
source ./application/build/delete_item_config.sh
# 删除构建中间文件
for i in $(find ./ -name Makefile -type f);do
    make clean -f $i >/dev/null 2>&1
done;

for i in ${DELETE_BUILDING_FILE_LIST[@]};do
    rm -rvf $i > $DELETED_ITEM
done;
# 对于该目录下的一些挂载上的文件，发现会处于挂载状态无法删除，需要特殊处理
if [ -d application/build/temp ]; then
    cd application/build/temp
    find . | xargs sudo umount
    find . | xargs sudo chmod 777
    find . | xargs rm -rf
    cd ..
    rm -f application/build/temp
fi
# 为保证构建前后一致性，源码编译目录、安装目录需要删除
for i in ${DELETE_OPENSOURCE_FILE_LIST[@]};do
    if [ -d $i ];then
        rm -rf $i > $DELETED_ITEM
    fi
done;
# 删除过程的日志文件处理；
echo "build_clean done!"
if [ $DEBUG == 1 ] ; then
    echo " see deltails in ${G_CODE_BASE}/build/"
else
    rm $DELETED_ITEM
fi
exit 0
