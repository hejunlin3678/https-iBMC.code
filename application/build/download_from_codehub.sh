#!/bin/bash
# Download open_souce file and chip software from codehub
# Copyright © Huawei Technologies Co., Ltd. 2021-2023. All rights reserved.

set -e

SCRIPT_DIR=$(cd $(dirname $0); pwd)
CODE_ROOT_DIR=$(cd $SCRIPT_DIR/../..; pwd)

OPENSOURCE_DIR=${CODE_ROOT_DIR}/open_source
VENDOR_DIR=${CODE_ROOT_DIR}/vendor
TEMP_DIR=${CODE_ROOT_DIR}/temp

# Reserved interfaces for vendor extension
manifest_file_list=(
    "opensource_manifest.xml"
    "vendor_manifest.xml"
)
for manifest_file in ${manifest_file_list[@]};
do
    URL_TEMP_DIR=${TEMP_DIR}/${manifest_file%.*}

    # Initializing the manifest repositry
    [ -e "${URL_TEMP_DIR}" ] && rm -rf ${URL_TEMP_DIR}
    mkdir -p ${URL_TEMP_DIR}
    cd ${URL_TEMP_DIR}
    git init .
    cp ${SCRIPT_DIR}/dependency_for_hpm/downxml/${manifest_file} ${URL_TEMP_DIR}

    # Replace the XML configuration path
    TEMP_REAL_PATH=$(realpath ${TEMP_DIR})
    
    if [[ "${TEMP_REAL_PATH}" =~ "ssdnfs" ]]; then
        SED_CMD="s#path=\"#&../../${CODE_ROOT_DIR#/*/}/#g"
    else
        SED_CMD="s#path=\"#&../../#g"
    fi

    sed -i ${SED_CMD} ${manifest_file}

    # Add file to the manifest repositry
    git add ${URL_TEMP_DIR}/. && git commit -m "init"

    # Add to .mm directory
    git mm init -u ${URL_TEMP_DIR} -m ${manifest_file}

    # Download file, only the latest node is pulled.
    cd ${URL_TEMP_DIR}
    git mm sync --depth 1 -j8
done

# Do not modify the following statements. The script execution is related to the path.
cd ${OPENSOURCE_DIR}/lldpd/lldpd
./autogen.sh

# remove zlib in StoreLib
rm -rf ${VENDOR_DIR}/StoreLib/versionChangeSet/common_storelib_unf_rel/app_util/common_storelib_unf/src/common/opensource/zlib
rm -rf ${VENDOR_DIR}/StoreLib3108/versionChangeSet/common_storelib_unf_rel/app_util/common_storelib_unf/src/common/opensource/zlib