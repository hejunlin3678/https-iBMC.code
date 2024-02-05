#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
"""
功能：提供统一错误码协议规范和格式，交互标准化，用于快速定位
错误码规则：

错误阶段：用于应用标识，由一位字母表示， C=compile, P=package,D=deploy,U=unit test ,S=system integration test
错误归属：用于责任标识，由一位数字表示， 1=业务代码逻辑问题，2=工程类脚本与配置类问题

错误类型：用于错误分类，
错误编码：
版权信息：Copyright Huawei Technologies Co., Ltd. 2010-2022. All rights reserved.
"""
EXCEPTION_STAGE = {
    'C': 'Acton: compile\n',
    'P': 'Acton: package\n',
    'D': 'Acton: deploy\n',
    'U': 'Acton: unit test\n',
    'S': 'Acton: system integration test\n'
}
EXCEPTION_RESPONSE = {
    1: "责任归属：业务代码逻辑问题\n",
    2: "责任归属：工程类脚本与配置类问题\n",
    3: "责任归属：仓库未响应问题\n",
    4: "责任归属：当前操作人员\n"
}
EXCEPTION_CODE = {
    # 4xx 系统环境类, 子类别由第二位数字控制
    400: 'ERROR_Code=400,环境校验：操作系统版本不一致，找不到YAML指定的系统环境！\n',
    401: 'ERROR_Code=401,环境校验：环境硬件配置不达标，无法满足YAML指定的最低配置！\n',
    402: 'ERROR_Code=402,环境校验：安装的工具版本与YAML指定的版本不一致！\n',
    403: 'ERROR_Code=403,环境校验：工具未安装！\n',

    410: 'ERROR_Code=410,环境运行：下载Docker Image超时！\n',
    411: 'ERROR_Code=411,环境运行：运行Docker Container 失败！\n',
    412: 'ERROR_Code=412,环境运行：挂载数据卷失败！\n',
    413: 'ERROR_Code=413,环境运行：文件系统IO冲突！\n',
    414: 'ERROR_Code=414,环境运行：系统crash！\n',

    # 5xx 依赖类, 子类别由第二位数字控制
    500: 'ERROR_Code=500,cmc二进制依赖下载：找不到依赖定义的配置文件！\n',
    501: 'ERROR_Code=501,cmc二进制依赖下载：找不到依赖包下载源！\n',
    502: 'ERROR_Code=502,cmc二进制依赖下载：依赖配置文件格式解析错误！\n',
    503: 'ERROR_Code=503,cmc二进制依赖下载：配置文件设置错误！\n',
    504: 'ERROR_Code=504,下载失败 未定义异常！\n',

    510: 'ERROR_Code=510,maven依赖下载：找不到依赖定义的配置文件！\n',
    511: 'ERROR_Code=511,maven依赖下载：找不到依赖包下载源！\n',
    512: 'ERROR_Code=512,maven依赖下载：依赖配置文件格式解析错误！\n',

    520: 'ERROR_Code=520,go依赖下载：找不到依赖定义的配置文件！\n',
    521: 'ERROR_Code=521,go依赖下载：找不到依赖包下载源！\n',
    522: 'ERROR_Code=522,go依赖下载：依赖配置文件格式解析错误！\n',

    530: 'ERROR_Code=530,NPM依赖下载：找不到依赖定义的配置文件！\n',
    531: 'ERROR_Code=531,NPM依赖下载：找不到依赖包下载源！\n',
    532: 'ERROR_Code=532,NPM依赖下载：依赖配置文件格式解析错误！\n',

    540: 'ERROR_Code=540,Pypi依赖下载：找不到依赖定义的配置文件！\n',
    541: 'ERROR_Code=541,Pypi依赖下载：找不到依赖包下载源！\n',
    542: 'ERROR_Code=542,Pypi依赖下载：依赖配置文件格式解析错误！\n',

    550: 'ERROR_Code=550,源码下载：无效的远程仓库！\n',
    551: 'ERROR_Code=551,源码下载：远程仓库time out！\n',
    552: 'ERROR_Code=552,源码下载：git pull 远程与本地缓存冲突！\n',

    560: 'ERROR_Code=560,依赖解析：依赖解析命令执行失败！\n',
    561: 'ERROR_Code=561,依赖解析：依赖分析报告输出失败！\n',

    # 6xx 操作类，子类别由第二位数字控制
    600: 'ERROR_Code=600,配置文件：文件不存在！\n',
    601: 'ERROR_Code=601,配置文件：配置文件解析失败，请检查格式！\n',
    602: 'ERROR_Code=602,配置文件：参数解析失败！\n',
	603: 'ERROR_Code=603,配置文件：配置内容超出升级包极限大小！\n',

    610: 'ERROR_Code=610,命令执行类：命令执行失败，请检查该条命令是否正确，或代码是否错误！\n',

    620: 'ERROR_Code=620,加解密：公私钥证书找不到！\n',
    621: 'ERROR_Code=621,加解密：加解密工具执行失败！\n',

    630: 'ERROR_Code=630,文件操作：重命名、移动文件失败，请检查路径是否正确、重复性与权限！\n',
    631: 'ERROR_Code=631,文件操作：文件权限设置chmod失败，请检查操作权限！\n',
    632: 'ERROR_Code=632,文件操作：文件拷贝失败，请检查路径是否正确与权限！\n',
    633: 'ERROR_Code=633,文件操作：文件超链接设置失败，请检查路径是否正确与权限！\n',
    634: 'ERROR_Code=634,文件操作：解压包失败！\n',
    635: 'ERROR_Code=634,文件操作：压缩失败！\n',
	
    # 7xx 参数类，子类别有第二位数字控制
    701: 'ERROR_Code=701,传参操作：参数错误！\n'
}
