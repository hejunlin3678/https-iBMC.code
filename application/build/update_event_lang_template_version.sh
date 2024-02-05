#!/bin/bash
# Update the version number of event_lang_template_v2.xml based on platform_5.xml.
# Copyright © Huawei Technologies Co., Ltd. 2021-2023. All rights reserved.

set -e

# 目的：根据输入中的版本号，自动修改输出中的版本号字段
# 参见DTS2020011313982
# 方案：
# 1 前提条件:platform_v5.xml中的版本号是有人手动维护的，所以直接读取作为输入
# 2 新增此脚本，从platform里面读取版本号信息自动修改。
# 3 此脚本集成到一键式当中，出包时调用。
# 输入：INPUT中的CLASS="BMC"的PROPERTY NAME="PMEVer" PROPERTY NAME="BuildNumber"版本号字段；
# 输出：OUTPUT中的 <MAJOR_VERSION> <MINOR_VERSION> <AUX_VERSION>字段

INPUT=application/src/resource/pme_profile/platform_v5.xml
OUTPUT=application/rootfs/opt/pme/conf/language/event_lang_template_v2.xml
CURRENT_DIR=$(dirname $(realpath $0))
CURRENT_V2R2_TRUNK=$(realpath ${CURRENT_DIR}/../..)

echo "update event_lang_template_v2.xml version info in ${CURRENT_V2R2_TRUNK}.."
cd ${CURRENT_V2R2_TRUNK}
echo "work_dir is ${CURRENT_V2R2_TRUNK}"
# 获取版本号
PMEVER=$(grep '<PROPERTY NAME="PMEVer"  TYPE="s">' ${INPUT} | grep -o '[0-9]*\.[0-9]*')
MAJOR_VERSION=$(echo $PMEVER | grep -o '^[0-9]*')
MINOR_VERSION=$(echo $PMEVER | grep -o '[0-9]*$')
AUX_VERSION=$(grep '<PROPERTY NAME="BuildNumber"' ${INPUT} | grep -o '[0-9]*')
# 更新版本号
if [[ "${MAJOR_VERSION}" != "x" ]]
then
    echo "update MAJOR_VERSION: ${MAJOR_VERSION}"
    sed -i "s/<MAJOR_VERSION>[0-9xX]*<\/MAJOR_VERSION>/<MAJOR_VERSION>${MAJOR_VERSION}<\/MAJOR_VERSION>/" ${OUTPUT}
else
    echo 'error when read MAJOR_VERSION'
    echo 'update event_lang_template_v2.xml version info failed!'
    exit 1
fi

if [[ "${MINOR_VERSION}x" != "x" ]]
then
    echo "update MINOR_VERSION: ${MINOR_VERSION}"
    sed -i "s/<MINOR_VERSION>[0-9xX]*<\/MINOR_VERSION>/<MINOR_VERSION>${MINOR_VERSION}<\/MINOR_VERSION>/" ${OUTPUT}
else
    echo 'error when read MINOR_VERSION'
    echo 'update event_lang_template_v2.xml version info failed!'
    exit 1
fi

if [[ "${AUX_VERSION}x" != "x" ]]
then
    echo "update AUX_VERSION: ${AUX_VERSION}"
    sed -i "s/<AUX_VERSION>[0-9xX]*<\/AUX_VERSION>/<AUX_VERSION>${AUX_VERSION}<\/AUX_VERSION>/" ${OUTPUT}
else
    echo 'error when read AUX_VERSION'
    echo 'update event_lang_template_v2.xml version info failed!'
    exit 1
fi
echo "update event_lang_template_v2.xml version info success."
exit 0
