#!/bin/bash
# test func map gen tool
# Copyright © Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.

G_CURRENT_DIR=$(pwd)
G_RELATIVE_DIR="../../"
G_CODE_BASE=$(realpath ${G_CURRENT_DIR}/${G_RELATIVE_DIR}) # 代码仓路径
G_TEST_TREE_DIR="${G_CODE_BASE}/test_tree"
G_TEST_TREE_BAKUP_DIR="${G_CODE_BASE}/test_tree_backup"
G_COVERAGE_FOLDER="${G_TEST_TREE_DIR}/standard_all_coverage"
G_UNITTEST_FOLDER="${G_TEST_TREE_DIR}/unit_test"
G_COVERAGE_BACKUP="${G_TEST_TREE_BAKUP_DIR}/standard_all_coverage"
G_UNITTEST_BACKUP="${G_TEST_TREE_BAKUP_DIR}/unit_test"
app=$1

# backup test_tree files
backup_test_file() {
    echo "create test_tree_backup"
    if [ -d ${G_TEST_TREE_BAKUP_DIR} ]; then
        rm -r ${G_TEST_TREE_BAKUP_DIR}
        mkdir ${G_TEST_TREE_BAKUP_DIR}
    else
        mkdir ${G_TEST_TREE_BAKUP_DIR}
    fi

    echo "start to backup test_tree files"
    if [ -d ${G_COVERAGE_FOLDER} ]; then
        mv ${G_COVERAGE_FOLDER} ${G_TEST_TREE_BAKUP_DIR}/
    fi
    if [ -d ${G_UNITTEST_FOLDER} ]; then
        mv ${G_UNITTEST_FOLDER} ${G_TEST_TREE_BAKUP_DIR}/
    fi
}

# restore test_tree/standard_all_coverage
restore_test_file() {
    echo "start to restore test_tree files"
    if [ -d ${G_COVERAGE_BACKUP} ]; then
        cp -rf ${G_COVERAGE_BACKUP} ${G_TEST_TREE_DIR}/
    fi
    if [ -d ${G_UNITTEST_BACKUP} ]; then
        cp -rf ${G_UNITTEST_BACKUP} ${G_TEST_TREE_DIR}/
    fi
}

# generate test func map file
test_func_map() {
    echo "start to insert stub and build app"
    /bin/bash ./build_UT.sh $app --enable-coverage-stub

    echo "start to test app and generate test func map file"
    /bin/bash ./test_UT.sh
}

backup_test_file
test_func_map
restore_test_file
exit 0