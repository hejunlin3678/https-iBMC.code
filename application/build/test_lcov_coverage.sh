#!/bin/bash
# test lcov for coverage
# Copyright © Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.

G_CODE_BASE="$1"
G_TEST_TREE_DIR="${G_CODE_BASE}/test_tree"
# 增量覆盖率与全量覆盖率输出报告目录
G_ALL_OUTPUT_DIR="${G_TEST_TREE_DIR}/standard_all_coverage"
export LD_LIBRARY_PATH="/usr/local/lib/"
# 需要运行DT的app列表文件（由build_UT.sh创建）
G_DT_APPS_FILE="dt_apps.txt"
# 保存DT测试的app列表
G_APP_LIST=""
# lcov覆盖率数据路径参数字符串
G_COV_DIR_PARAM=""
# 无app需要进行DT测试时标识符
G_APP_NONE="None"

# 从G_DT_APPS_FILE获取测试的app列表
get_app_list()
{
    if [ -f  ${G_DT_APPS_FILE} ]; then
        G_APP_LIST=$(cat ${G_DT_APPS_FILE})
        if [[ ${G_APP_LIST[0]} == ${G_APP_NONE} ]]; then
            echo "App list is none, exit normally"
            exit 0
        fi
        echo "App List: ${G_APP_LIST[@]}"
    else
        echo "${G_DT_APPS_FILE} not exist, exit!"
        exit 1
    fi
}

# 拼接lcov的directory参数，用于统计各个app覆盖率
get_app_cov_param()
{
    if [[ ${G_APP_LIST[0]} == "all" ]]; then
        # 为all时，直接拼接test_tree/src/apps/作为覆盖率统计参数
        G_COV_DIR_PARAM=" --directory "${G_TEST_TREE_DIR}"/src/apps/"
    else
        # 为数组或单个app时，依次拼接各个app路径作为覆盖率统计参数
        for app in ${G_APP_LIST[@]}
        do
            G_COV_DIR_PARAM=${G_COV_DIR_PARAM}" --directory "${G_TEST_TREE_DIR}"/src/apps/"${app}
        done
    fi

    echo "cov_param: $G_COV_DIR_PARAM"
}

make_standard_all_coverage() {
    # 默认标准的gcov/lcov全量报告
    local exclude_file="--exclude=*include* --exclude=*.h \
                        --exclude=*test_code* \
                        --exclude=*platforms/kmc* \
                        --exclude=*application/src/libs*"
    local src_code_dir="${G_CODE_BASE}/application/src"
    local all_report="${G_ALL_OUTPUT_DIR}/all.info"
    local enable_branch="--rc lcov_branch_coverage=1"
    local commit_id
    commit_id=$(git log | head -n1 | awk '{print $2}')
    local branch_name
    branch_name=$(git symbolic-ref --short -q HEAD)
    local curr_path=${G_CODE_BASE}
    # clear
    if [[ -d "${G_ALL_OUTPUT_DIR}" ]]; then
        rm -rf "${G_ALL_OUTPUT_DIR}"
    fi
    mkdir -p "${G_ALL_OUTPUT_DIR}"
    # run
    lcov ${exclude_file} -b ${src_code_dir} \
        ${G_COV_DIR_PARAM} \
        -c -o ${all_report} ${enable_branch}
    if [[ $? == 0 ]]; then
        echo "make_standard_all_coverage done!"
    else
        echo "make_standard_all_coverage failed!"
        return 1
    fi
    genhtml -o ${G_ALL_OUTPUT_DIR} ${enable_branch} --legend \
        --title "branch:${branch_name} commit:${commit_id}" \
        --prefix=${curr_path} \
        ${all_report}
}

get_app_list
get_app_cov_param
make_standard_all_coverage
