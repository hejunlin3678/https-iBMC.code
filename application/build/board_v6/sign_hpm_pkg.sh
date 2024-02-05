#!/bin/bash
# Digital signature verification in the equipment loading process
# Copyright © Huawei Technologies Co., Ltd. 2021-2023. All rights reserved.

set -e
G_CURRENT_DIR=$(pwd)
if [ $(grep -c "v6" $G_CURRENT_DIR) -eq '0' ]; then
    echo "The current path is $G_CURRENT_DIR"
    G_WORK_DIR=${G_CURRENT_DIR%/board_v6}
else
    G_WORK_DIR=$(pwd)
fi
TEMP_DIR=${G_WORK_DIR}/../../temp

OUTPUT_DIR=${TEMP_DIR}/output
PACKET_DIR=${TEMP_DIR}/output/packet

create_signed_pkg(){
    BOARD=$1
    DELIVERY_SIGN_ENABLE_FLAG=$2
    SIGN_DIR=${OUTPUT_DIR}/${BOARD}_sign_files_temp

    if [ -d ${SIGN_DIR} ]
    then
        rm -rf ${SIGN_DIR}
    fi

    mkdir -p ${SIGN_DIR}

    cd ${G_WORK_DIR}

    cp -f ../src/resource/board/${BOARD}/archive_emmc_nand/archive.ini ${SIGN_DIR}/

    # 如果使能了装备加载工序数字签名校验， 但是没有 delivery 配置时，直接退出
    if [ "${DELIVERY_SIGN_ENABLE_FLAG}" == "enable" ] && [ -z "$(cat ${SIGN_DIR}/archive.ini | grep -n TOGDP_PATH_DELIVERY=)" ]; then
        rm -rf ${SIGN_DIR}
        # 退出状态为 0， 兼容没有配置的板子
        exit 0
    fi
    cd ${SIGN_DIR}
    chmod +x *.ini
    if [ ! -f archive.ini ]
    then
        echo "Error : cannot find archive config file - archive.ini"
        exit
    fi

    if [ -z "${DELIVERY_SIGN_ENABLE_FLAG}" ]; then
        # 没有使能装备加载工序数字签名校验
        TARGET_DIR=$(cat archive.ini | grep -n TOGDP_PATH= | awk -F '=' '{print $2}')
        TARGET_NAME=$(cat archive.ini | grep -n TOGDP_NAME= | awk -F '=' '{print $2}')
        TARGET_VERSION=$(cat archive.ini | grep -n TOGDP_VERSION= | awk -F '=' '{print $2}')
    elif [ "${DELIVERY_SIGN_ENABLE_FLAG}" == "IPMI_ENABLE" ]; then
        TARGET_DIR=$(cat archive.ini | grep -n TOGDP_PATH_IPMI_ENABLE= | awk -F '=' '{print $2}')
        TARGET_NAME=$(cat archive.ini | grep -n TOGDP_NAME_IPMI_ENABLE= | awk -F '=' '{print $2}')
        TARGET_VERSION=""
    elif [ "${DELIVERY_SIGN_ENABLE_FLAG}" == "IPMI_DISABLE" ]; then
        TARGET_DIR=$(cat archive.ini | grep -n TOGDP_PATH_IPMI_DISABLE= | awk -F '=' '{print $2}')
        TARGET_NAME=$(cat archive.ini | grep -n TOGDP_NAME_IPMI_DISABLE= | awk -F '=' '{print $2}')
        TARGET_VERSION=""
    else
        # 使能装备加载工序数字签名校验
        TARGET_DIR=$(cat archive.ini | grep -n TOGDP_PATH_DELIVERY= | awk -F '=' '{print $2}')
        TARGET_NAME=$(cat archive.ini | grep -n TOGDP_NAME_DELIVERY= | awk -F '=' '{print $2}')
        TARGET_VERSION=""
    fi

    cd ${G_WORK_DIR}
    MAJOR_VER=$(cat ../src/resource/pme_profile/platform_v6.xml | grep PMEMajorVer | cut -f5 | awk -F '>' '{print $2}' | awk -F '<' '{print $1}')
    MINOR_VER=$(cat ../src/resource/pme_profile/platform_v6.xml | grep PMEMinorVer | cut -f5 | awk -F '>' '{print $2}' | awk -F '<' '{print $1}' | awk '{printf("%02d", $0)}')
    RELEASE_MAJOR_VER=$(cat ../src/resource/pme_profile/platform_v6.xml | grep ReleaseMajorVer | cut -f4 | awk -F '>' '{print $2}' | awk -F '<' '{print $1}' | awk '{printf("%02d", $0)}')
    RELEASE_MINOR_VER=$(cat ../src/resource/pme_profile/platform_v6.xml | grep ReleaseMinorVer | cut -f4 | awk -F '>' '{print $2}' | awk -F '<' '{print $1}' | awk '{printf("%02d", $0)}')

    # 使能装备加载工序数字签名校验且有 delivery 配置()
    if [ -z "${DELIVERY_SIGN_ENABLE_FLAG}" ] && [ -n "$(cat ${SIGN_DIR}/archive.ini | grep -n TOGDP_PATH_DELIVERY=)" ]; then
        # 由于拿到的版本号前带有0值，使用数学运算，让其变更为10进制数
        RELEASE_MINOR_VER=$(expr ${RELEASE_MINOR_VER} + 1)
        RELEASE_MINOR_VER=$(printf "%02d" ${RELEASE_MINOR_VER})
    fi
    rm -f ${SIGN_DIR}/archive.ini

    if [[ $MINOR_VER == 0x* ]]
    then
        MINOR_VER=$(echo $MINOR_VER | awk -F 'x' '{print $2}')
    fi

    TARGET_PKG=${TARGET_NAME}${MAJOR_VER}"."${MINOR_VER}"."${RELEASE_MAJOR_VER}"."${RELEASE_MINOR_VER}
    # 如果TARGET_VERSION指定了发布版本号
    if [ "$TARGET_VERSION" != "" ] ; then
        # 使用指定的发布版本号填充0502****_version.ini的archive_version字段
        # 使用指定的发布版本号命名zip
        TARGET_PKG=${TARGET_NAME}${TARGET_VERSION}
        echo "sign_hpm_kg.sh : TARGET_PKG = "${TARGET_PKG}""
    fi
    # PACKET_DIR=/usr1/jenkins/workspace/compile/application/build/output/packet
    #            /usr1/jenkins/workspace/compile/application/build/output/packet/RM210/ToGDP/05022YQR/IT3RMUB02040.zip
    mv "${PACKET_DIR}/${TARGET_DIR}/${TARGET_PKG}.zip" ${SIGN_DIR}/

    cd ${SIGN_DIR}

    unzip ${TARGET_PKG}.zip
    # 拷贝签名文件
    unsupport_sign_board_list=(PangeaV6_Atlantic PangeaV6_Atlantic_Smm PangeaV6_Pacific PangeaV6_Arctic)
    need_sign_flag=1
    for((i=0;i<${#unsupport_sign_board_list[@]};i++))
    do
        if [ "${BOARD}" == "${unsupport_sign_board_list[${i}]}" ] ; then
            need_sign_flag=0
            break
        fi
    done
    if [ "${DELIVERY_SIGN_ENABLE_FLAG}" == "enable" ]; then
        cp ${OUTPUT_DIR}/${BOARD}_sep_vercfg.xml ${TARGET_PKG}/${BOARD}_vercfg.xml
        cp ${OUTPUT_DIR}/${BOARD}_sep_vercfg.xml.cms ${TARGET_PKG}/${BOARD}_vercfg.xml.cms
        cp ${OUTPUT_DIR}/${BOARD}_sep_vercfg.xml.crl ${TARGET_PKG}/${BOARD}_vercfg.xml.crl
    elif [ "${DELIVERY_SIGN_ENABLE_FLAG}" == "IPMI_ENABLE" ]; then
        cp ${OUTPUT_DIR}/${BOARD}_IPMI_ENABLE_vercfg.xml ${TARGET_PKG}/${BOARD}_vercfg.xml
        cp ${OUTPUT_DIR}/${BOARD}_IPMI_ENABLE_vercfg.xml.cms ${TARGET_PKG}/${BOARD}_vercfg.xml.cms
        cp ${OUTPUT_DIR}/${BOARD}_IPMI_ENABLE_vercfg.xml.crl ${TARGET_PKG}/${BOARD}_vercfg.xml.crl
    elif [ "${DELIVERY_SIGN_ENABLE_FLAG}" == "IPMI_DISABLE" ]; then
        cp ${OUTPUT_DIR}/${BOARD}_IPMI_DISABLE_vercfg.xml ${TARGET_PKG}/${BOARD}_vercfg.xml
        cp ${OUTPUT_DIR}/${BOARD}_IPMI_DISABLE_vercfg.xml.cms ${TARGET_PKG}/${BOARD}_vercfg.xml.cms
        cp ${OUTPUT_DIR}/${BOARD}_IPMI_DISABLE_vercfg.xml.crl ${TARGET_PKG}/${BOARD}_vercfg.xml.crl
    else
        if [ ${need_sign_flag} == 1 ] ; then
            cp ${OUTPUT_DIR}/${BOARD}_vercfg.xml ${TARGET_PKG}/
            cp ${OUTPUT_DIR}/${BOARD}_vercfg.xml.cms ${TARGET_PKG}/
            cp ${OUTPUT_DIR}/${BOARD}_vercfg.xml.crl ${TARGET_PKG}/
        fi
    fi

    if [ -f ${TARGET_PKG}.zip ]
    then 
        rm ${TARGET_PKG}.zip
    fi

    zip -r ${TARGET_PKG}.zip ${TARGET_PKG}

    mv ${TARGET_PKG}.zip "${PACKET_DIR}/${TARGET_DIR}/"

    if [ ! -f "${PACKET_DIR}/${TARGET_DIR}/${TARGET_PKG}.zip" ]
    then
        echo "create signed hpm packet failed"
    fi

    cd ${OUTPUT_DIR}
    rm -rf ${SIGN_DIR}
}

BOARDS=($@)

for board in ${BOARDS[@]};do
    create_signed_pkg $board
    create_signed_pkg $board enable

    if [ "$board" == "RM210_SECURITY_3V0" -o "$board" == "RM211" ]; then
        create_signed_pkg $board IPMI_ENABLE
        create_signed_pkg $board IPMI_DISABLE
    fi
done
