#!/bin/bash
# test lcov
# Copyright © Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.

cd "$(dirname "$(realpath $0)")" || exit
G_CURRENT_DIR=$(pwd)
G_RELATIVE_DIR="../../"
G_CODE_BASE=$(realpath ${G_CURRENT_DIR}/${G_RELATIVE_DIR}) # 代码仓路径
G_TEST_TREE_DIR="${G_CODE_BASE}/test_tree"
KVM_PATH="${G_CODE_BASE}/tools/others/dev_tools/llt/java_test"
KVM_COVERAGE_INDEX="$KVM_PATH/coverage/index.html"
# 增量覆盖率与全量覆盖率输出报告目录
G_ALL_OUTPUT_DIR="${G_TEST_TREE_DIR}/standard_all_coverage"
export LD_LIBRARY_PATH="/usr/local/lib/"
# 需要运行DT的app列表文件（由build_UT.sh创建）
G_DT_APPS_FILE="dt_apps.txt"
# 无app需要进行DT测试时标识符
G_APP_NONE="None"
# 打包后的覆盖率报告名称(如要改动，需联系CIE同步修改jenkins仓库的yaml文件)
G_COV_REPORT="cov_report.tar.gz"
# git差异比较
G_SINCE="HEAD~"
G_UNTIL="HEAD"
# 脚本名称
G_SCRIPT_NAME="$0"
# 无需DT场景生成桩ut_incremental_coverage_report.html文件
G_STUB_INCREMENT_HTML=$(cat << EOF
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html lang="en"><head><meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
  <title>coverage report</title>
  <link rel="stylesheet" type="text/css" href="./ut_incremental_coverage_report_files/gcov.css">
</head>
<body>
  <table width="100%" border="0" cellspacing="0" cellpadding="0">
    <tbody><tr><td class="title">iBMC - incremental code coverage report</td></tr>
    <tr><td class="ruler"><img src="./ut_incremental_coverage_report_files/glass.png" width="3" height="6" alt=""></td></tr>
    <tr>
      <td width="100%">
        <table cellpadding="1" border="0" width="100%">
          <tbody><tr>
            <td></td>
            <td width="33%" class="headerCovTableHead">UT covered</td>
            <td width="33%" class="headerCovTableHead">Total</td>
            <td width="33%" class="headerCovTableHead">Coverage</td>
          </tr>
          <tr>
            <td class="headerItem">Incremental Lines:</td>
            <td class="incrementalCoveredLines">0</td>
            <td class="incrementalTotalLines">0</td>
            <td class="incrementalLineCoverageRate">100.0 %</td>
          </tr>
          <tr><td><img src="./ut_incremental_coverage_report_files/glass.png" width="3" height="3" alt=""></td></tr>
        </tbody></table>
      </td>
    </tr>
    <tr><td class="ruler"><img src="./ut_incremental_coverage_report_files/glass.png" width="3" height="3" alt=""></td></tr>
  </tbody></table>
</body></html>
EOF
)

# 生成桩ut_incremental_coverage_report.html
gen_stub_incremental_report()
{
    echo "generate stub ut_incremental_coverage_report.html"
    local incremental_file="${G_TEST_TREE_DIR}/standard_all_coverage/ut_incremental_coverage_report.html"
    echo ${G_STUB_INCREMENT_HTML} > ${incremental_file}
}

# function
kvm_test() {
    # 判断是否为java模块
    # 确保ant已安装并运行测试
    cd ${KVM_PATH} || exit
    if [[ -z $ANT_HOME ]]; then
        echo "ANT_HOME DOES NOT EXIST"
        exit 1
    fi
    ant coverage

    echo ""
    echo "See ${KVM_COVERAGE_INDEX} for details"
    exit 0
}

clear() {
    rm ${G_CODE_BASE}/ut_incremental_check.py
    rm ${G_CODE_BASE}/ut_incremental_coverage_report.template
}

init_env() {
    clear
    cp ${G_CODE_BASE}/tools/others/dev_tools/llt/tool/ut_* "${G_CODE_BASE}"
}

debug_python() {
    cp ${G_CODE_BASE}/ut_incremental_check.py ${G_CODE_BASE}/tools/others/dev_tools/llt/tool/
    cp ${G_CODE_BASE}/ut_incremental_coverage_report.template ${G_CODE_BASE}/tools/others/dev_tools/llt/tool/
}

make_increasement_report(){
    # 增量覆盖率报告
    # 依赖于全量报告生成后
    local base_commit="$1"
    local current_commit="$2"
    local module_name="$3"
    # param
    if [[ "${base_commit}" == "" ]]; then
        base_commit="trunk/master" # 默认或调试时比较的基础结点
    fi
    if [[ "${current_commit}" == "" ]]; then
        current_commit=$(git log | head -n1 | awk '{print $2}') # 当前结点
    fi
    # run
    cd ${G_CODE_BASE} || exit
    ${G_CODE_BASE}/ut_incremental_check.py "--since_until=${base_commit}..${current_commit}" "--module=${module_name}" "--lcov_dir=${G_ALL_OUTPUT_DIR}" "--thresh=0.8"
    cd - || exit
}

cp_test_report() {
    if [ ! -d ${G_TEST_TREE_DIR}/output/coverage/ ]; then
        echo "No coverage report to be copied, exit"
        exit 0
    fi
    cd ${G_TEST_TREE_DIR}/output/coverage/
    reports=$(ls . | grep '_o.xml')
    for report in $reports; do
        cp $report ${G_TEST_TREE_DIR}/standard_all_coverage/test_detail.xml
    done
}

merge_html_report() {
    # 简单合并增量与全量报告
    if [[ -f ${G_ALL_OUTPUT_DIR}/ut_incremental_coverage_report.html ]]; then
        cat ${G_ALL_OUTPUT_DIR}/ut_incremental_coverage_report.html >>${G_ALL_OUTPUT_DIR}/index.html
        cat ${G_ALL_OUTPUT_DIR}/ut_incremental_coverage_report.html >>${G_ALL_OUTPUT_DIR}/index-sort-f.html
        cat ${G_ALL_OUTPUT_DIR}/ut_incremental_coverage_report.html >>${G_ALL_OUTPUT_DIR}/index-sort-b.html
        cat ${G_ALL_OUTPUT_DIR}/ut_incremental_coverage_report.html >>${G_ALL_OUTPUT_DIR}/index-sort-l.html
    fi
}

pack_coverage_report()
{
    if [[ ! -d ${G_ALL_OUTPUT_DIR} ]]; then
        echo "${G_ALL_OUTPUT_DIR} not exist, coverage report not generate, ${G_SCRIPT_NAME} exit abnormally"
        exit 1
    fi
    cd ${G_ALL_OUTPUT_DIR}
    # 1.输出到test_tree路径下(供构建任务获取); 2.排除all.info文件(libing解析xml和html，该文件已不再需要)
    tar czvf ${G_TEST_TREE_DIR}/${G_COV_REPORT} --exclude="all.info" *
    echo "Coverage report packaged as ${G_TEST_TREE_DIR}/${G_COV_REPORT}"
}

# main
g_mode=$1

if [[ "${g_mode}" == "--cache" ]]; then
    debug_python
    make_increasement_report $2 $3
    exit 0
else
    init_env
fi

if [ -f ${G_DT_APPS_FILE} ]; then
    APP=$(cat ${G_DT_APPS_FILE})
    if [[ $APP == ${G_APP_NONE} ]]; then
        gen_stub_incremental_report
        echo "No need to generate coverage report, ${G_SCRIPT_NAME} exit normally"
        exit 0
    fi
fi

# 利用HEAD和HEAD~之间差异计算增量覆盖率
base_commit=${G_SINCE}
current_commit=${G_UNTIL}
module_name=""

bash ${G_CURRENT_DIR}/test_lcov_coverage.sh "${G_CODE_BASE}"
make_increasement_report ${base_commit} ${current_commit} ${module_name}
merge_html_report
cp_test_report
pack_coverage_report
exit 0
