#!/bin/bash
# This scripts extracts packet information and set BEP envrionment
# Copyright © Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.

WORKSPACE=$(cd $(dirname $0); pwd)
BASE_PATH=$(realpath ${WORKSPACE}/../../../)
BIN_PATH=/usr1/release
RELEASE_NAME="board"
TOSUPPORT_PATH=$BIN_PATH
SOURCE_PATH=/usr1/upload/Source


# 根据产品名获取发布包，提取签名文件，打包用加密秘钥，打包时间
function set_prefix()
{
    BOARD_NAME=$1
    export EXTRACT_TOOL_HOME=${WORKSPACE}/../../../../extract_tool

    # 下载拆包工具
    if [ ! -d $EXTRACT_TOOL_HOME ]; then
        # 配置下载目标
        echo "downloas extract tool first"
        download_dest=${WORKSPACE}/../../../../

        # 解压工具
        cd ${WORKSPACE}
        dependency_file=dependency_extract_tool.xml
        dependency_path=${WORKSPACE}/${dependency_file}
        artget pull -d ${dependency_path} -ap ${download_dest}
        cd ${download_dest}
        unzip extract_tool.zip
    else
        echo "use local extract tool"
        # 重新解压拆包工具，清理上次拆包遗留
        if [ -n $EXTRACT_TOOL_HOME ]; then
            rm -rf $EXTRACT_TOOL_HOME
        fi
        cd ${WORKSPACE}/../../../../
        unzip extract_tool.zip
    fi

    # 使用拆包工具抽取打包信息
    cd $EXTRACT_TOOL_HOME
    
    if [ -n $EXTRACT_TOOL_HOME/prefix* ]; then
        rm -rf $EXTRACT_TOOL_HOME/prefix*
    fi
    cd $EXTRACT_TOOL_HOME
    echo sh extract.sh "$BIN_PATH/$TOSUPPORT_PATH"
    sh -x extract.sh "$BIN_PATH/$TOSUPPORT_PATH"
    echo "finish extracting"

}

# 裁剪发布二进制，只保留对应板子的目录
function cut_reduntary()
{
    board=$1
    export TOSUPPORT_PATH=$(cat ${BASE_PATH}/application/src/resource/board/${board}/archive_emmc_nand/archive.ini | grep -n TOSUPPORT_PATH | awk -F '=' '{print $2}')
    cd $BIN_PATH
    for item in $(ls $BIN_PATH)
    do
        if [ "$TOSUPPORT_PATH" == "$item" ]; then
            echo "got required directory : $TOSUPPORT_PATH"
        else
            if [ -n "$item" ]; then
                rm -rf "$item"
            fi
        fi
    done
    cd -
}

# 将打包信息加入代码目录重新打包
function repack_src()
{
    cd $SOURCE_PATH
    if [ -f src.zip ]; then
        echo "repack src"
        unzip -o -u src.zip -d source_code/
        rm src.zip
        cp -rf $EXTRACT_TOOL_HOME/prefix_* source_code
        cd source_code
        zip -r /usr1/upload/Source/src.zip ./
    else
        echo "no packed source found!"
    fi
}

cut_reduntary $1
set_prefix $1
repack_src

exit 0
