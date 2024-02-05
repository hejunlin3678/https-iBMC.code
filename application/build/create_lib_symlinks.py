#!/usr/bin/env python
# coding:utf-8

"""
文件名：create_lib_symlinks.py
功能：设置开源库环境
附加信息：如果需要附加新的库，则需要增加新的符号链接
版权信息：华为技术有限公司，版本所有(C) 2019-2020
"""


import os
import argparse
from utils.common import Common

# 获取命令行参数(调用脚本时的位置参数)参数
parser = argparse.ArgumentParser(description="Excute create_lib_symlinks.py")
parser.add_argument("-d", "--dir_path", help="Fill in directory path", default=".")
args = parser.parse_args()


def set_global():
    global g_common
    global g_excute_path
    g_common = Common()
    g_excute_path = args.dir_path


def make_symbol_link_part1():
    # 到目标路径执行
    os.chdir(g_excute_path)

    # curl
    g_common.rm_symbol_link("libcurl.so.4", "libcurl.so")
    os.symlink("libcurl.so.4.7.0", "libcurl.so.4")
    os.symlink("libcurl.so.4", "libcurl.so")

    # jpeg
    g_common.rm_symbol_link("libjpeg.so.9", "libjpeg.so")
    os.symlink("libjpeg.so.9.5.0", "libjpeg.so.9")
    os.symlink("libjpeg.so.9", "libjpeg.so")

    # libedit
    g_common.rm_symbol_link("libedit.so.0", "libedit.so")
    os.symlink("libedit.so.0.0.70", "libedit.so.0")
    os.symlink("libedit.so.0", "libedit.so")

    # JSON-C
    g_common.rm_symbol_link("libjson-c.so.5", "libjson.so")
    os.symlink("libjson-c.so.5.2.0", "libjson-c.so.5")
    os.symlink("libjson-c.so.5", "libjson.so")

    # OpenLDAP
    g_common.rm_symbol_link("liblber.so.2", "liblber.so")
    os.symlink("liblber.so.2.0.200", "liblber.so.2")
    os.symlink("liblber.so.2", "liblber.so")

    g_common.rm_symbol_link("libldap.so.2", "libldap.so")
    os.symlink("libldap.so.2.0.200", "libldap.so.2")
    os.symlink("libldap.so.2", "libldap.so")

    # snmp
    g_common.rm_symbol_link("libnetsnmpagent.so.40", "libnetsnmpagent.so")
    os.symlink("libnetsnmpagent.so.40.1.0", "libnetsnmpagent.so.40")
    os.symlink("libnetsnmpagent.so.40", "libnetsnmpagent.so")

    g_common.rm_symbol_link("libnetsnmphelpers.so.40", "libnetsnmphelpers.so")
    os.symlink("libnetsnmphelpers.so.40.1.0", "libnetsnmphelpers.so.40")
    os.symlink("libnetsnmphelpers.so.40", "libnetsnmphelpers.so")

    g_common.rm_symbol_link("libnetsnmpmibs.so.40", "libnetsnmpmibs.so")
    os.symlink("libnetsnmpmibs.so.40.1.0", "libnetsnmpmibs.so.40")
    os.symlink("libnetsnmpmibs.so.40", "libnetsnmpmibs.so")

    g_common.rm_symbol_link("libnetsnmp.so.40", "libnetsnmp.so")
    os.symlink("libnetsnmp.so.40.1.0", "libnetsnmp.so.40")
    os.symlink("libnetsnmp.so.40", "libnetsnmp.so")

    g_common.rm_symbol_link("libnetsnmptrapd.so.40", "libnetsnmptrapd.so")
    os.symlink("libnetsnmptrapd.so.40.1.0", "libnetsnmptrapd.so.40")
    os.symlink("libnetsnmptrapd.so.40", "libnetsnmptrapd.so")
    return


def make_symbol_link_part2():
    # SASL
    g_common.rm_symbol_link("libntlm.so.3", "libntlm.so")
    os.symlink("libntlm.so.3.0.0", "libntlm.so.3")
    os.symlink("libntlm.so.3", "libntlm.so")

    g_common.rm_symbol_link("libsasl2.so.3", "libsasl2.so")
    os.symlink("libsasl2.so.3.0.0", "libsasl2.so.3")
    os.symlink("libsasl2.so.3", "libsasl2.so")

    # pcre
    os.unlink("libpcrecpp.so.0")
    os.unlink("libpcrecpp.so")
    os.symlink("libpcrecpp.so.0.0.2", "libpcrecpp.so.0")
    os.symlink("libpcrecpp.so.0", "libpcrecpp.so")

    os.unlink("libpcre.so.1")
    os.unlink("libpcre.so")
    os.symlink("libpcre.so.1.2.13", "libpcre.so.1")
    os.symlink("libpcre.so.1", "libpcre.so")

    os.unlink("libpcreposix.so.0")
    os.unlink("libpcreposix.so")
    os.symlink("libpcreposix.so.0.0.7", "libpcreposix.so.0")
    os.symlink("libpcreposix.so.0", "libpcreposix.so")

    # SQLite
    g_common.rm_symbol_link("libsqlite3.so.0", "libsqlite3.so")
    os.symlink("libsqlite3.so.0.8.6", "libsqlite3.so.0")
    os.symlink("libsqlite3.so.0", "libsqlite3.so")

    # ssh2
    g_common.rm_symbol_link("libssh2.so.1", "libssh2.so")
    os.symlink("libssh2.so.1.0.1", "libssh2.so.1")
    os.symlink("libssh2.so.1", "libssh2.so")

    # StoreLib
    g_common.rm_symbol_link("libstorelib.so")
    os.symlink("libstorelib.so.07.1602.0100.0000", "libstorelib.so")

    # StoreLib3108
    g_common.rm_symbol_link("libstorelib.so.07.06")
    os.symlink("libstorelib.so.07.0604.0100.1000", "libstorelib.so.07.06")

    # StoreLibIR3
    g_common.rm_symbol_link("libstorelibir3.so")
    os.symlink("libstorelibir3.so.16.02-0", "libstorelibir3.so")
    return


def make_symbol_link_part3():
    # StoreLibIT
    g_common.rm_symbol_link("libstorelibit.so")
    os.symlink("libstorelibit.so.07.0700.0200.0600", "libstorelibit.so")

    # StorageCore
    g_common.rm_symbol_link("libstoragecore.so.5")
    os.symlink("libstoragecore.so.5.25.11", "libstoragecore.so.5")
    g_common.rm_symbol_link("libdiagnosticcore.so.5")
    os.symlink("libdiagnosticcore.so.5.25.11", "libdiagnosticcore.so.5")

    # krb5
    g_common.rm_symbol_link("libkrb5.so.3", "libkrb5.so")
    os.symlink("libkrb5.so.3.3", "libkrb5.so.3")
    os.symlink("libkrb5.so.3", "libkrb5.so")

    g_common.rm_symbol_link("libk5crypto.so.3", "libk5crypto.so")
    os.symlink("libk5crypto.so.3.1", "libk5crypto.so.3")
    os.symlink("libk5crypto.so.3", "libk5crypto.so")

    g_common.rm_symbol_link("libkrb5support.so.0", "libkrb5support.so")
    os.symlink("libkrb5support.so.0.1", "libkrb5support.so.0")
    os.symlink("libkrb5support.so.0", "libkrb5support.so")

    g_common.rm_symbol_link("libcom_err.so.3", "libcom_err.so")
    os.symlink("libcom_err.so.3.0", "libcom_err.so.3")
    os.symlink("libcom_err.so.3", "libcom_err.so")

    g_common.rm_symbol_link("libgssapi_krb5.so.2", "libgssapi_krb5.so")
    os.symlink("libgssapi_krb5.so.2.2", "libgssapi_krb5.so.2")
    os.symlink("libgssapi_krb5.so.2", "libgssapi_krb5.so")

    # lldpd
    g_common.rm_symbol_link("liblldpctl.so.4")
    os.symlink("liblldpctl.so.4.9.1", "liblldpctl.so.4")

    # luajit
    g_common.rm_symbol_link("libluajit.so")
    g_common.rm_symbol_link("libluajit-5.1.so")
    g_common.rm_symbol_link("libluajit-5.1.so.2")
    os.symlink("libluajit-5.1.so.2.1.0", "libluajit-5.1.so.2")
    os.symlink("libluajit-5.1.so.2", "libluajit-5.1.so")
    os.symlink("libluajit-5.1.so", "libluajit.so")
    return


if __name__ == "__main__":
    set_global()
    make_symbol_link_part1()
    make_symbol_link_part2()
    make_symbol_link_part3()
