#!/usr/bin/env python
# coding:utf-8

"""
文件名：rootfs_property_customization.py
功能：打包生成镜像
版权信息：华为技术有限公司，版本所有(C) 2020-2022
"""
import os
import sys
import argparse
import json
import shutil
import subprocess
import re
from datetime import datetime
cwd = os.path.dirname(__file__)
sys.path.append(f"{cwd}/../")
sys.path.append(f"{cwd}/../../")
sys.path.append(f"{cwd}/../../../")
from works.work import Work
from utils.config import Config
from customization.DefaultCustomization import DefaultCustomization


class rootfs_property_customization(DefaultCustomization):
    def __init__(self, work, config):
        # super(rootfs_property_customization, self).__init__(work, config)
        self.config = config
        self.work = work

    ############################## work_build_rootfs_img start ##############################
    # 解析动作，生成列表
    def parse_conf(self):
        com_conf_file = ""
        if self.config.board_version == "V5":
            com_conf_file = f"{self.config.common.code_root}/config/v5_img.json"
        elif self.config.board_version == "V6":
            com_conf_file = f"{self.config.common.code_root}/config/v6_img.json"

        board_conf_file = f"{self.config.common.code_root}/config/board/{self.config.board_name}/img.json"

        with open(rf'{com_conf_file}') as jsonfile:
            content = json.load(jsonfile)

        if os.path.exists(board_conf_file):
            with open(rf'{board_conf_file}') as jsonfile:
                board_content = json.load(jsonfile)

        act_list = ['dos2unix', 'mkdir -p', 'cp -a', 'rm -rf', 'mv', 'mv -f', 'touch', "sed", 'ln -s', 'chown', 'chmod', 'strip']
        for act in act_list:
            if board_content.get(act, False):
                content.get(act).extend(board_content.get(act))

        return content

    # v5,v6,board的json内容配置
    def deal_img_conf(self, act, content):
        actions = content.get(act, False)
        if actions:
            for do in actions:
                do = do.replace('src_tree/application/src_bin', self.config.common.src_bin)
                do = do.replace('src_tree/application/platform_v5', self.config.common.platform_v5_path)
                do = do.replace('src_tree/application/platform_v6', self.config.common.platform_v6_path)
                do = do.replace('src_tree/tools', self.config.common.tools_path)
                do = do.replace('build_tree/buildimg', self.config.buildimg_dir)
                do = do.replace('build_tree/output', self.config.work_out)
                do = do.replace('build_tree', self.config.build_path)
                do = do.replace('src_tree', self.config.common.code_root)
                do = do.replace('temp_tree', self.config.common.temp_path)
                do = do.replace('board_name', self.config.board_name)
                do = do.replace('web_tree', self.config.common.src_data_web)
                do = do.replace(r'{OEM_SRC_TREE}', self.config.common.oem_code_root)

                if act == "strip":
                    self.work.run_command(f"{self.config.strip} {do}", ignore_error=True)
                elif act == "chmod":
                    if do.find('find ') != -1:
                        self.work.run_command(do, ignore_error=True)
                    else:
                        self.work.run_command(f"{act} {do}", ignore_error=True)
                else:
                    self.work.run_command(f"{act} {do}", ignore_error=True)
        return

    # 删除不必要的app
    def delete_unnecessary_app(self, work_path: str):
        necessary_alias_app_tuple = ("fsync", "maint_debug_mod", "maint_debug_mod2", "maint_debug_mod3")
        pdt_dir = f"{self.config.common.code_root}/application/src/resource/board/{self.config.board_name}"
        if self.config.board_name == "1288hv6_2288hv6_5288v6" or self.config.board_name == "DP1210_DP2210_DP4210":
            files = os.listdir(f"{pdt_dir}")
            for f in files:
                if os.path.isdir(f"{pdt_dir}/{f}") and f != "archive_emmc_nand":
                    self.config.logger.info(f"{f}")
                    pdt_dir = f"{self.config.common.code_root}/application/src/resource/board/{self.config.board_name}/{f}"
        rootfs_path = f"{work_path}/rtos_with_driver/rootfs"
        # TCE合入app未放入opt/bmc/apps/legacy目录，需适配
        app_file_path_tuple = (f"{rootfs_path}/opt/bmc/apps/legacy", f"{rootfs_path}/opt/pme/apps")
        app_list_dir = os.path.join(pdt_dir, "cfgfs/app.list")
        # 目录名即app名，部分app有别名，需要排除
        for app_file_path in app_file_path_tuple:
            all_app_list = self.work.run_command(f"ls {app_file_path}", return_result=True).split("\n")
            for app_name in all_app_list:
                if self.work.run_command(f"grep -v '#' {app_list_dir} | grep {app_name} || echo 'failed'", return_result=True) == "failed":
                    if app_name not in necessary_alias_app_tuple:
                        self.work.run_command(f"rm -rf {app_file_path}/{app_name}")

    def hostsec_build(self, work_path: str):
        # 解压HostSec
        hisec_tar_path = f"{self.config.common.temp_path}/platforms/HostSec/HostSec-rtos.arm64.tar.gz"
        hisec_install_path = f"{work_path}/opt/pme/bin/intrusion-detection"
        self.work.run_command(f"mkdir -p {hisec_install_path}")
        self.work.run_command(f"tar -zxf {hisec_tar_path} -C {hisec_install_path}")

        # 添加systemd、start脚本
        self.work.run_command(f"mv {work_path}/opt/pme/conf/security_policy/intrusion_detection_start.sh {hisec_install_path}")
        self.work.run_command(f"cp -a {work_path}/opt/pme/conf/security_policy/policy_bmc.dat {hisec_install_path}/iplc.dat")
        self.work.run_command(f"chmod 750 -R {work_path}/opt/pme/conf/security_policy")
        self.work.run_command(f"chmod 550 -R {hisec_install_path}")
        self.work.run_command(f"chmod 550 -R {hisec_install_path}/intrusion_detection_start.sh")
        self.work.run_command(f"chmod 640 -R {hisec_install_path}/iplc.dat")
        self.work.run_command(f"chmod 640 -R {work_path}/opt/pme/conf/security_policy/policy_bmc.dat")
        self.work.run_command(f"chown -R 0:0 {hisec_install_path}")

    def tsb_build(self, work_path: str):
        tsb_origin_path = f"{self.config.common.internal_release_path}"
        tsb_run_path = f"{work_path}/opt/pme/bin/tsb"
        self.work.run_command(f"mkdir -p {tsb_run_path}")
        self.work.run_command(f"cp {work_path}/opt/pme/conf/security_policy/tsb_start.sh {tsb_run_path}")
        self.work.run_command(f"cp {tsb_origin_path}/tsb_bmc {tsb_run_path}")
        self.work.run_command(f"chmod 550 {tsb_run_path}/tsb_start.sh")
        self.work.run_command(f"chmod 550 {tsb_run_path}/tsb_bmc")

    # 收集各个组件包，整合到一起，形成一个基础包
    def rootfs_base_deploy(self, work_path: str):
        os.chdir(work_path)
        rootfs_path = f"{work_path}/rtos_with_driver/rootfs"
        datafs_path = f"{work_path}/rtos_with_driver/datafs"
        self.config.logger.info("ibmc_log: Source integrate start ...")

        # 解析配置json文件
        content = self.parse_conf()

        # 压缩包位置
        if self.config.board_version == "V5":
            rtos_tar = os.path.join(self.config.common.platform_v5_path, "rtos.tar.gz")
            pme_tar = os.path.join(self.config.common.platform_v5_path, "pme_strip.tar.gz")
            sdk_tar = os.path.join(self.config.common.platform_v5_path, "hi1710sdk.tar.gz")
        elif self.config.board_version == "V6":
            rtos_tar = os.path.join(self.config.common.platform_v6_path, "rtos.tar.gz")
            pme_tar = os.path.join(self.config.common.platform_v6_path, "pme_strip.tar.gz")
            sdk_tar = os.path.join(self.config.common.platform_v6_path, "hi1711sdk.tar.gz")

        # 打包前清理垃圾
        self.work.run_command(f"rm -rf {rootfs_path}")

        # 解压rtos包
        self.work.run_command(f"tar -zxf {rtos_tar}")
        if self.config.board_version == "V6":
            self.work.run_command(f"rm -rf {rootfs_path}/etc/ssh")
            self.work.run_command(f"find {self.config.rootfs_path}/usr/lib/* | grep -v '\\(filehook.*\\|diskslot_custom.*\\)' | xargs -I {{}} mv -f {{}} {self.config.rootfs_path}/usr/lib64/")
        # 解压pme包
        self.work.run_command(f"tar -zxvf {pme_tar} -C {rootfs_path}")
        self.deal_img_conf("patch", content)
        
        # 复制ibmc app到打包目录
        self.work.run_command(f"cp -dfr {self.config.rootfs_path}/* {rootfs_path}")

        # 除去特定单板，其他单板删除security_policy文件
        if not self.config.common.get_support_security_policy(self.config.board_name):
            self.work.run_command(f"rm -rf {rootfs_path}/opt/pme/conf/security_policy")

        # 启动脚本打补丁
        if self.config.board_version == "V6":
            self.work.run_command(f"patch {rootfs_path}/etc/rc.d/rc.sysinit {self.config.common.code_root}/tools/build_tools/rtos-sdk-patch/1711/rc.sysinit.patch", ignore_error=True)
        # 修改systemd配置，将标准输出打印到串口
        self.work.run_command(f'sed -i "s/#DefaultStandardOutput=journal/DefaultStandardOutput=tty/g" {rootfs_path}/etc/systemd/system.conf', ignore_error=True)
        # 带v3组件组件的复制v3目录
        if os.path.exists(self.config.common.v3_component_dir):
            self.config.logger.info("-------->>> copy v3 component to rootfs <<<--------")
            self.work.run_command(f"cp -a {self.config.common.v3_component_dir}/* {rootfs_path}")
        if "v2component" in self.config.target:
            self.work.run_command(f"rm -rf {rootfs_path}/opt/pme/sbin/dfm")
            self.config.logger.info("-------->>> copy v3 component to rootfs <<<--------")
            self.work.run_command(f"cp -a {self.config.common.code_root}/../compile/temp/debug/tmp_root/* {rootfs_path}")

        # 解压SDK，由vx_img.json及deal_conf函数复制到指定位置
        sdk_path = f"{self.config.buildimg_dir}/sdk"
        self.work.run_command(f"rm -rf {sdk_path}")
        self.work.run_command(f"mkdir {sdk_path}")
        self.work.run_command(f"tar -zxf {sdk_tar} -C {sdk_path}")
        # self.work.run_command("cp -af {}/ko {}/opt/pme/lib/modules/".format(sdk_path, rootfs_path))

        # 预处理并复制平台xml
        if self.config.board_version == "V5":
            self.work.run_command(f"rm {sdk_path}/ko/bmc_edma_drv.ko -f")
            plat_file = f"{rootfs_path}/opt/pme/conf/profile/platform_v5.xml"
        else:
            plat_file = f"{rootfs_path}/opt/pme/conf/profile/platform_v6.xml"
        ff_xml_file = f"{self.config.common.code_root}/application/src/resource/template/static/_ff.xml"
        # 修改copyright中年份字段
        new_year = datetime.now().year
        self.work.run_command(f'sed -i "s/\(Copyright.*-\)[0-9][0-9][0-9][0-9]/\\1{new_year}/g" {plat_file}', ignore_error=True)
        self.work.run_command(f'sed -i "s/\(Copyright.*-\)[0-9][0-9][0-9][0-9]/\\1{new_year}/g" {ff_xml_file}', ignore_error=True)

        # xml格式化
        xml_format_script = f"{self.config.common.code_path}/xml_format.py"
        profile_path = f"{rootfs_path}/opt/pme/conf/profile"
        self.work.run_command(f"python3 {xml_format_script} {profile_path} covert_keywords")

        # img_vx.json和单板的img.json配置源包
        act_list = ['dos2unix', 'mkdir -p', 'cp -a', 'rm -rf', 'mv', 'mv -f', 'touch', "sed", 'ln -s', 'chown', 'chmod', 'strip']
        for act in act_list:
            self.deal_img_conf(act, content)

        # 删除冗余文件
        self.work.run_command(f"find {self.config.buildimg_dir} -name '.svn' -type d | xargs rm -rf >/dev/null 2>&1", ignore_error=True)
        self.work.run_command(f"find {self.config.buildimg_dir} -name '.gitkeep' | xargs rm -rf  >/dev/null 2>&1", ignore_error=True)
        self.work.run_command(f"mkdir -p {datafs_path}/var/db")
        self.work.run_command(f"mkdir -p {datafs_path}/var/net-snmp")

        # release版本安全性修改
        if self.config.build_type != "debug":
            # 是否允许root用户登录
            self.work.run_command(f"sed -i 's/MaxStartups 10:30:100/MaxStartups 10:30:12/g' {datafs_path}/etc/ssh/sshd_config", ignore_error=True)
            self.work.run_command(f"sed -i 's/PermitRootLogin yes/PermitRootLogin no/g' {datafs_path}/etc/ssh/sshd_config", ignore_error=True)
            self.work.run_command(f"rm -rf {datafs_path}/var/firstboot2.flag")
            self.work.run_command(f"mv -f {datafs_path}/var/firstboot2_v5.flag {datafs_path}/var/firstboot2.flag")

            # 只读版本将openssh替换成沙箱安全增强版本
            self.work.run_command(f"rm -rf {rootfs_path}/usr/bin/ssh-keygen")
            self.work.run_command(f"rm -rf {rootfs_path}/usr/sbin/sshd")
            open_source_path = "open_source/arm64" if self.config.board_version == "V6" else "open_source"
            self.work.run_command(f"cp -df {self.config.common.code_root}/application/src_bin/{open_source_path}/OpenSSH/ssh-keygen_sandbox_enhance {rootfs_path}/usr/bin/ssh-keygen")
            self.work.run_command(f"cp -df {self.config.common.code_root}/application/src_bin/{open_source_path}/OpenSSH/sshd_sandbox_enhance {rootfs_path}/usr/sbin/sshd")

        if self.config.board_version == "V5":
            os.chdir(f"{rootfs_path}/usr/lib")
            # pme提供的muparser软链接失效导致占用空间增大，在此临时规避，后续pme解决后删除
            self.work.run_command("rm -rf libmuparser.so")
            self.work.run_command("rm -rf libmuparser.so.2")
            self.work.run_command("find . -name 'libmuparser*' -type f -exec ln -s {} libmuparser.so \\;")
            self.work.run_command("find . -name 'libmuparser*' -type f -exec ln -s {} libmuparser.so.2 \\;")
            if self.config.build_type == "release":
                self.work.run_command(f"cp -rf {self.config.common.platform_v5_path}/u-boot.bin {rootfs_path}/usr/upgrade/u-boot.bin")
            else:
                self.work.run_command(f"cp -rf {self.config.common.platform_v5_path}/u-boot_debug.bin {rootfs_path}/usr/upgrade/u-boot.bin")

        self.work.run_command("ls -al {}/opt/pme/lib/modules/ko".format(rootfs_path))
        self.config.common.delete_for_matched(rootfs_path, "rootfs")
        self.rootfs_base_cust_deploy(rootfs_path)
        return

    # 基础包公共配置
    def rootfs_base_conf(self, work_path: str):
        # v5与v6差异配置
        rootfs_path = f"{work_path}/rtos_with_driver/rootfs"
        self.config.logger.info(f"Make {self.config.board_name} common config ...")
        if self.config.board_version == 'V5':
            self.work.run_command(f"cp -f {rootfs_path}/opt/pme/conf/profile/platform_v5.xml {rootfs_path}/opt/pme/conf/profile/platform.xml")
        if self.config.board_version == 'V6':
            self.work.run_command(f"cp {rootfs_path}/opt/pme/conf/profile/platform_v6.xml {rootfs_path}/opt/pme/conf/profile/platform.xml")
            self.work.run_command(f"sed -i 's/getty -L 115200 ttyAMA0 vt102/getty -L 115200 ttyS0 vt102/' {rootfs_path}/etc/inittab", ignore_error=True)
            self.work.run_command(f"sed -i 's/1710/1711/' {rootfs_path}/etc/profile", ignore_error=True)
        self.work.run_command(f"rm -rf {rootfs_path}/opt/pme/conf/profile/platform_v5.xml")
        self.work.run_command(f"rm -rf {rootfs_path}/opt/pme/conf/profile/platform_v6.xml")
        self.work.run_command(f"cp -f {rootfs_path}/etc/rc.d/rc.sysinit {rootfs_path}/etc/rc.d/rc_bak.sysinit")
        if self.config.board_version == "V5":
            self.work.run_command(f"sed -i '/mount -t tmpfs -o nodev tmpfs/a\mount -o remount,rw \/' {rootfs_path}/etc/rc.d/rc.sysinit")
        self.work.run_command(f"sed -i 's/ulimit -S -c.*/ulimit -S -c 51200/' {rootfs_path}/etc/rc.d/rc.local", ignore_error=True)
        self.work.run_command(f"sed -i 's/ulimit -S -c.*/ulimit -S -c 51200/' {rootfs_path}/etc/profile", ignore_error=True)
        self.rootfs_base_cust_conf(rootfs_path)
        return

    # 数据包部署
    def datafs_deploy(self, work_path: str):
        # 切换工作目录与定义工作目录
        os.chdir(work_path)
        rootfs_path = f"{work_path}/rtos_with_driver/rootfs"
        datafs_path = f"{work_path}/rtos_with_driver/datafs"
        datafs_mount_path = f"{self.config.buildimg_dir}/mnt_datafs"
        self.work.run_command(f"mkdir -p {datafs_mount_path}")

        # 制作datafs img镜像并挂载
        img_path = f"{self.config.work_out}/datafs_{self.config.board_name}.img"
        bs_count = 249995 if (self.config.board_version == "V6") else 499990
        self.config.common.create_image_and_mount_datafs(bs_count, img_path, datafs_mount_path)

        # 复制所有文件到挂载目录
        self.work.run_command(f"cp -a {datafs_path}/* {datafs_mount_path}/")
        self.work.run_command(f"chown 0:0 {datafs_mount_path}")
        self.config.common.umount_datafs(datafs_mount_path)
        self.work.run_command(f"rm -rf {datafs_mount_path}")
        self.work.run_command(f"ls -sh {img_path}")

        # 检查datafs文件系统
        img_tmp_path = f"{self.config.common.output_path}/datafs_{self.config.board_name}_tmp.img"
        self.work.run_command(f"cp -f {img_path} {img_tmp_path}")
        self.work.run_command(f"fsck.ext4 -n {img_tmp_path}")
        self.work.run_command(f"rm -rf {img_tmp_path}")
        
        # 打包datafs.tar.gz
        os.chdir(f"{work_path}/rtos_with_driver")
        self.work.run_command(f"tar --format=gnu -czvf {rootfs_path}/usr/upgrade/datafs.tar.gz ./datafs/* 1>/dev/null 2>&1")
        self.work.run_command(f"chmod 440 {rootfs_path}/usr/upgrade/datafs.tar.gz")
        self.work.run_command(f"chown {self.config.common.non_root_ug} {self.config.work_out}/*.img", ignore_error=True)
        self.work.run_command(f"chown {self.config.common.non_root_ug} {self.config.work_out}/*.tar.gz", ignore_error=True)
        return

    # 只读包部署
    def rootfs_ro_deploy(self, work_path: str):
        # 切换工作目录和定义变量
        os.chdir(work_path)
        rootfs_path = f"{work_path}/rtos_with_driver/rootfs"
        bs_count = 51200 if (self.config.board_version == "V5") else 96256
        img_path = f"{self.config.work_out}/rootfs_{self.config.board_name}.img"
        tar_path = f"{self.config.work_out}/{self.config.board_name}_rootfs.tar.gz"
        ro_mount_path = f"{self.config.buildimg_dir}/ro_rootfs"
        self.work.run_command(f"mkdir {ro_mount_path}")
        self.config.common.create_image_and_mount_datafs(bs_count, img_path, ro_mount_path)
        self.work.run_command(f"cp -a {rootfs_path}/* {ro_mount_path}/")
        # 只读包额外处理
        self.work.run_command(f"sed -i 's/ulimit -S -c.*/ulimit -S -c 0/' {ro_mount_path}/etc/rc.d/rc.local")
        self.work.run_command(f"sed -i 's/ulimit -S -c.*/ulimit -S -c 0/' {ro_mount_path}/etc/profile")
        self.work.run_command(f"cp -f {rootfs_path}/etc/rc.d/rc_bak.sysinit {ro_mount_path}/etc/rc.d/rc.sysinit")
        self.work.run_command(f'sed -i "/export PROMPT_COMMAND=/c\export PROMPT_COMMAND=\'{{ return_vaue=\\$? ; msg=\\$(history 1); user=\\$(whoami); history -w; touch \\$HISTFILE 2>/dev/null; }}\'" {ro_mount_path}/rootfs/etc/profile', ignore_error=True)
        self.work.run_command(f"cp -af {ro_mount_path}/opt/bmc/apps/legacy/maint_debug_mod3/maint_debug_mod {ro_mount_path}/opt/bmc/apps/legacy/maint_debug_mod/maint_debug_mod", ignore_error=True)
        if os.path.exists(f"{ro_mount_path}/opt/pme/bin/maint_debug_cli3"):
            self.work.run_command(f"rm -rf {ro_mount_path}/opt/pme/bin/maint_debug_cli")
            self.work.run_command(f"cp -af {ro_mount_path}/opt/pme/bin/maint_debug_cli3 {ro_mount_path}/opt/pme/bin/maint_debug_cli")
        if self.config.board_version == "V5":
            self.work.run_command(f"cp {self.config.common.platform_v5_path}/u-boot.bin {ro_mount_path}/usr/upgrade/u-boot.bin")
        # 支持装备分离要进行额外配置
        if self.config.support_dft_separation:
            # 需要修改的xml位置
            dst_file = f"{ro_mount_path}/opt/pme/conf/profile/platform.xml"
            # 从xml中读取配置的版本号
            major_ver = self.work.run_command(f"awk -F '[<>]' '/PMEMajorVer/{{print $5}}' {dst_file}",
                                              return_result=True)
            minor_ver = self.work.run_command(f"awk -F '[<>]' '/PMEMinorVer/{{print $5}}' {dst_file}",
                                              return_result=True)
            release_major_ver = self.work.run_command(f"awk -F '[<>]' '/ReleaseMajorVer/{{print $5}}' {dst_file}",
                                                      return_result=True)
            release_minor_ver = self.work.run_command(f"awk -F '[<>]' '/ReleaseMinorVer/{{print $5}}' {dst_file}",
                                                      return_result=True)
            dft_minor_ver = str(int(release_minor_ver) + 1)
            dft_version = '.'.join([major_ver, minor_ver.zfill(2), release_major_ver.zfill(2), dft_minor_ver.zfill(2)])
            # 配置版本号
            self.work.run_command(f'sed -i "/ReleaseMinorVer/s#V>.*</V#V>{dft_minor_ver}</V#" {dst_file}')
            self.work.run_command(f'sed -i "/PMEBackupVer/s#V>.*</V#V>{dft_version}</V#" {dst_file}')
            self.work.run_command(f'sed -i "/PMEVer/s#V>.*</V#V>{dft_version}</V#" {dst_file}')
            self.work.run_command(f'sed -i "/BMCAvailableVer/s#V>.*</V#V>{dft_version}</V#" {dst_file}')
            self.ro_cust_conf(ro_mount_path, dft_minor_ver=dft_minor_ver, dft_version=dft_version)
        else:
            self.ro_cust_conf(ro_mount_path)
        # V5单板直接打包(v6是取镜像)
        if self.config.board_version == "V5":
            os.chdir(ro_mount_path)
            self.work.run_command(f"tar --format=gnu -czf {tar_path} .", ignore_error=True)
            os.chdir(f"{self.config.buildimg_dir}/rtos_with_driver")
        # 卸载挂载的设备
        self.work.run_command(f"chown 0:0 {ro_mount_path}")
        self.config.common.umount_datafs(ro_mount_path)
        if self.config.board_version == "V6":
            self.work.run_command(f"chmod a+rw {img_path}", ignore_error=True)
            subprocess.run(
                f"{self.config.common.code_path}/get_img_parma_area.sh {self.config.common.code_root}/application/src/resource/pme_profile/platform_v6.xml {img_path}", shell=True)
            self.work.run_command(f"chmod o-rw {img_path}", ignore_error=True)
            self.work.run_command(
                f"tar --format=gnu -czf {tar_path} -C {self.config.work_out} rootfs_{self.config.board_name}.img", ignore_error=True)
        if self.config.cb_flag:
            cb_img_path = f"{self.config.work_out}/rootfs_CB_FLAG_{self.config.board_name}.img"
            cb_tar = f"{self.config.work_out}/CB_FLAG_{self.config.board_name}_rootfs.tar.gz"
            cb_mnt_datafs = f"{self.config.buildimg_dir}/cb_mnt_datafs"
            cb_build_tmp = f"{self.config.common.temp_path}/cb_cfgs_img"
            self.work.run_command(f"cp -rf {img_path} {cb_img_path}")
            self.work.run_command(f"rm -rf {cb_mnt_datafs}")
            self.work.run_command(f"mkdir {cb_mnt_datafs}")
            try:
                self.config.common.get_img_flag()
                # 指定loop，解决偶现mount失败问题
                self.config.common.get_loop()
                self.work.run_command(
                    f"mount -o loop={self.config.common.loop_dev} -t ext4 {cb_img_path} {cb_mnt_datafs}")
                self.work.run_command(
                    f"sed -i '/IsCustomBrandFW/s#V>.*</V#V>1</V#' {cb_mnt_datafs}/opt/pme/conf/profile/platform.xml")
                if self.config.board_version == "V5":
                    os.chdir(cb_mnt_datafs)
                    self.work.run_command(f"rm -rf {cb_mnt_datafs}/opt/pme/extern/profile")
                    self.work.run_command(f"mkdir {cb_mnt_datafs}/opt/pme/extern/profile")
                    self.work.run_command(f"cp -dfr {cb_build_tmp}/profile/* {cb_mnt_datafs}/opt/pme/extern/profile")
                    self.work.run_command(f"tar --format=gnu -czf {cb_tar} .", ignore_error=True)
                    os.chdir(f"{self.config.buildimg_dir}/rtos_with_driver")
                elif self.config.board_version == "V6":
                    self.work.run_command(f"cp -dfr {cb_build_tmp}/* {cb_mnt_datafs}/opt/pme/extern")
                    self.work.run_command(
                        f"tar --format=gnu -czf {cb_tar} -C {self.config.work_out} rootfs_CB_FLAG_{self.config.board_name}.img", ignore_error=True)
                self.work.run_command(f"chown 0:0 {cb_mnt_datafs}")
                self.config.common.umount_datafs(cb_mnt_datafs)
            except Exception as e:
                self.config.logger.error(f"build cb failed, msg:{e}")
                self.work.run_command(f"rm -rf {self.config.common.code_root}/img.flag")
                os._exit(1)
        self.work.run_command(f"chown {self.config.common.non_root_ug} {self.config.work_out}/*.img", ignore_error=True)
        self.work.run_command(f"chown {self.config.common.non_root_ug} {self.config.work_out}/*.tar.gz", ignore_error=True)
        return

    # 读写包部署
    def rootfs_rw_deploy(self, work_path: str):
        # 切换工作目录和定义变量
        os.chdir(work_path)
        rootfs_path = f"{work_path}/rtos_with_driver/rootfs"
        bs_count = 51200 if (self.config.board_version == "V5") else 96256
        img_path = f"{self.config.work_out}/rootfs_{self.config.board_name}.img"
        tar_path = f"{self.config.work_out}/{self.config.board_name}_rootfs.tar.gz"
        rw_mount_path = f"{self.config.buildimg_dir}/rw_rootfs"
        self.work.run_command(f"mkdir {rw_mount_path}")
        self.config.common.create_image_and_mount_datafs(bs_count, img_path, rw_mount_path)
        self.work.run_command(f"cp -a {rootfs_path}/* {rw_mount_path}/")
        # 支持装备分离要进行额外配置
        if self.config.support_dft_separation:
            # 需要修改的xml位置
            dst_file = f"{rw_mount_path}/opt/pme/conf/profile/platform.xml"
            # 从xml中读取配置的版本号
            major_ver = self.work.run_command(f"awk -F '[<>]' '/PMEMajorVer/{{print $5}}' {dst_file}", return_result=True)
            minor_ver = self.work.run_command(f"awk -F '[<>]' '/PMEMinorVer/{{print $5}}' {dst_file}", return_result=True)
            release_major_ver = self.work.run_command(f"awk -F '[<>]' '/ReleaseMajorVer/{{print $5}}' {dst_file}", return_result=True)
            release_minor_ver = self.work.run_command(f"awk -F '[<>]' '/ReleaseMinorVer/{{print $5}}' {dst_file}", return_result=True)
            dft_minor_ver = str(int(release_minor_ver) + 1)
            dft_version = '.'.join([major_ver, minor_ver.zfill(2), release_major_ver.zfill(2), dft_minor_ver.zfill(2)])
            # 配置版本号
            self.work.run_command(f'sed -i "/ReleaseMinorVer/s#V>.*</V#V>{dft_minor_ver}</V#" {dst_file}')
            self.work.run_command(f'sed -i "/PMEBackupVer/s#V>.*</V#V>{dft_version}</V#" {dst_file}')
            self.work.run_command(f'sed -i "/PMEVer/s#V>.*</V#V>{dft_version}</V#" {dst_file}')
            self.work.run_command(f'sed -i "/BMCAvailableVer/s#V>.*</V#V>{dft_version}</V#" {dst_file}')
            self.rw_cust_conf(rw_mount_path, dft_minor_ver=dft_minor_ver, dft_version=dft_version)
        else:
            self.rw_cust_conf(rw_mount_path)
        # V5单板直接打包(v6是取镜像)
        if self.config.board_version == "V5":
            os.chdir(rw_mount_path)
            self.work.run_command(f"tar --format=gnu -czf {tar_path} .", ignore_error=True)
            os.chdir(f"{self.config.buildimg_dir}/rtos_with_driver")
        # 卸载挂载的设备
        self.work.run_command(f"chown 0:0 {rw_mount_path}")
        self.config.common.umount_datafs(rw_mount_path)
        if self.config.board_version == "V6":
            self.work.run_command(f"chmod a+rw {img_path}", ignore_error=True)
            subprocess.run(f"{self.config.common.code_path}/get_img_parma_area.sh {self.config.common.code_root}/application/src/resource/pme_profile/platform_v6.xml {img_path}", shell=True)
            self.work.run_command(f"chmod o-rw {img_path}", ignore_error=True)
            self.work.run_command(f"tar --format=gnu -czf {tar_path} -C {self.config.work_out} rootfs_{self.config.board_name}.img", ignore_error=True)
        if self.config.cb_flag:
            cb_img_path = f"{self.config.work_out}/rootfs_CB_FLAG_{self.config.board_name}.img"
            cb_tar = f"{self.config.work_out}/CB_FLAG_{self.config.board_name}_rootfs.tar.gz"
            cb_mnt_datafs = f"{self.config.buildimg_dir}/cb_mnt_datafs"
            cb_build_tmp = f"{self.config.common.temp_path}/cb_cfgs_img"
            self.work.run_command(f"cp -dfr {img_path} {cb_img_path}")
            self.work.run_command(f"rm -rf {cb_mnt_datafs}")
            self.work.run_command(f"mkdir {cb_mnt_datafs}")
            try:
                self.config.common.get_img_flag()
                # 指定loop，解决偶现mount失败问题
                self.config.common.get_loop()
                self.work.run_command(f"mount -o loop={self.config.common.loop_dev} -t ext4 {cb_img_path} {cb_mnt_datafs}")
                self.work.run_command(f"sed -i '/IsCustomBrandFW/s#V>.*</V#V>1</V#' {cb_mnt_datafs}/opt/pme/conf/profile/platform.xml")
                if self.config.board_version == "V5":
                    os.chdir(cb_mnt_datafs)
                    self.work.run_command(f"rm -rf {cb_mnt_datafs}/opt/pme/extern/profile")
                    self.work.run_command(f"mkdir {cb_mnt_datafs}/opt/pme/extern/profile")
                    self.work.run_command(f"cp -dfr {cb_build_tmp}/profile/* {cb_mnt_datafs}/opt/pme/extern/profile")
                    self.work.run_command(f"tar --format=gnu -czf {cb_tar} .", ignore_error=True)
                    os.chdir(f"{self.config.buildimg_dir}/rtos_with_driver")
                elif self.config.board_version == "V6":
                    self.work.run_command(f"cp -dfr {cb_build_tmp}/* {cb_mnt_datafs}/opt/pme/extern")
                    self.work.run_command(f"tar --format=gnu -czf {cb_tar} -C {self.config.work_out} rootfs_CB_FLAG_{self.config.board_name}.img", ignore_error=True)
                self.work.run_command(f"chown 0:0 {cb_mnt_datafs}")
                self.config.common.umount_datafs(cb_mnt_datafs)
            except Exception as e:
                self.config.logger.error(f"build cb failed, msg:{e}")
                self.work.run_command(f"rm -rf {self.config.common.code_root}/img.flag")
                os._exit(1)
        self.work.run_command(f"chown {self.config.common.non_root_ug} {self.config.work_out}/*.img", ignore_error=True)
        self.work.run_command(f"chown {self.config.common.non_root_ug} {self.config.work_out}/*.tar.gz", ignore_error=True)
        return

    # 装备分离包部署
    def rootfs_dft_deploy(self, work_path: str):
        # 切换工作目录和定义变量
        os.chdir(work_path)
        rootfs_path = f"{work_path}/rtos_with_driver/rootfs"
        bs_count = 51200 if (self.config.board_version == "V5") else 96256
        img_path = f"{self.config.work_out}/rootfs_{self.config.board_name}.img"
        tar_path = f"{self.config.work_out}/{self.config.board_name}_rootfs.tar.gz"
        dft_mount_path = f"{self.config.buildimg_dir}/dft_rootfs"
        self.work.run_command(f"mkdir {dft_mount_path}")
        self.config.common.create_image_and_mount_datafs(bs_count, img_path, dft_mount_path)
        self.work.run_command(f"cp -a {rootfs_path}/* {dft_mount_path}/")
        # dft包额外配置
        self.work.run_command(f"chown 0:0 {dft_mount_path}")
        self.config.common.umount_datafs(dft_mount_path)
        self.work.run_command(f"chmod a+rw {img_path}", ignore_error=True)
        subprocess.run(f"{self.config.common.code_path}/get_img_parma_area.sh {self.config.common.code_root}/application/src/resource/pme_profile/platform_v6.xml {img_path}", shell=True)
        self.work.run_command(f"chmod o-rw {img_path}", ignore_error=True)
        self.work.run_command(f"tar --format=gnu -czf {tar_path} -C {self.config.work_out} rootfs_{self.config.board_name}.img", ignore_error=True)
        if self.config.cb_flag is True:
            cb_img_path = f"{self.config.work_out}/rootfs_rw_CB_FLAG_{self.config.board_name}_release.img"
            cb_tar = f"{self.config.work_out}/CB_FLAG_{self.config.board_name}_rw_release.tar.gz"
            cb_mnt_datafs = f"{self.config.buildimg_dir}/cb_mnt_datafs"
            cb_build_tmp = f"{self.config.common.temp_path}/cb_cfgs_img"
            self.work.run_command(f"cp -f {img_path} {cb_img_path}")
            self.work.run_command(f"rm -rf {cb_mnt_datafs}")
            self.work.run_command(f"mkdir -p {cb_mnt_datafs}")
            try:
                self.config.common.get_img_flag()
                # 指定loop，解决偶现mount失败问题
                self.config.common.get_loop()
                self.work.run_command(f"mount -o loop={self.config.common.loop_dev} -t ext4 {cb_img_path} {cb_mnt_datafs}")
                self.work.run_command(f"sed -i '/IsCustomBrandFW/s#V>.*</V#V>1</V#' {cb_mnt_datafs}/opt/pme/conf/profile/platform.xml")
                self.work.run_command(f"cp -a {cb_build_tmp}/* {cb_mnt_datafs}/opt/pme/extern")
                self.work.run_command(f"tar --format=gnu -czf {cb_tar} -C {self.config.work_out} rootfs_CB_FLAG_{self.config.board_name}.img", ignore_error=True)
                self.work.run_command(f"chown 0:0 {cb_mnt_datafs}")
                self.config.common.umount_datafs(cb_mnt_datafs)
            except Exception as e:
                self.config.logger.error(f"build cb failed, msg:{e}")
                self.work.run_command(f"rm -rf {self.config.common.code_root}/img.flag")
                os._exit(1)
                pass
        # 增加ro_dft_separation
        img_path = f"{self.config.work_out}/rootfs_ro_{self.config.board_name}.img"
        tar_path = f"{self.config.work_out}/{self.config.board_name}_ro_rootfs.tar.gz"
        self.work.run_command(f"rm -rf {dft_mount_path}")
        self.work.run_command(f"mkdir -p {dft_mount_path}")
        self.config.common.create_image_and_mount_datafs(bs_count, img_path, dft_mount_path)
        self.work.run_command(f"cp -a {rootfs_path}/* {dft_mount_path}")
        # 只读包额外处理
        self.work.run_command(f"sed -i 's/ulimit -S -c.*/ulimit -S -c 0/' {dft_mount_path}/etc/rc.d/rc.local")
        self.work.run_command(f"sed -i 's/ulimit -S -c.*/ulimit -S -c 0/' {dft_mount_path}/etc/profile")
        self.work.run_command(f"cp -f {rootfs_path}/etc/rc.d/rc_bak.sysinit {dft_mount_path}/etc/rc.d/rc.sysinit")
        self.work.run_command(
            f'sed -i "/export PROMPT_COMMAND=/c\export PROMPT_COMMAND=\'{{ return_vaue=\\$? ; msg=\\$(history 1); user=\\$(whoami); history -w; touch \\$HISTFILE 2>/dev/null; }}\'" {dft_mount_path}/etc/profile')
        self.work.run_command(
            f"cp -af {dft_mount_path}/opt/bmc/apps/legacy/maint_debug_mod3/maint_debug_mod {dft_mount_path}/opt/bmc/apps/legacy/maint_debug_mod/maint_debug_mod", ignore_error=True)
        if os.path.exists(f"{dft_mount_path}/opt/pme/bin/maint_debug_cli3"):
            self.work.run_command(f"rm -rf {dft_mount_path}/opt/pme/bin/maint_debug_cli")
            self.work.run_command(
                f"cp -af {dft_mount_path}/opt/pme/bin/maint_debug_cli3 {dft_mount_path}/opt/pme/bin/maint_debug_cli")
        if self.config.board_version == "V5":
            self.work.run_command(
                f"{self.config.common.platform_v5_path}/u-boot.bin {dft_mount_path}/usr/upgrade/u-boot.bin")
        self.work.run_command(f"chown 0:0 {dft_mount_path}")
        self.config.common.umount_datafs(dft_mount_path)
        self.work.run_command(f"chmod a+rw {img_path}", ignore_error=True)
        subprocess.run(f"{self.config.common.code_path}/get_img_parma_area.sh {self.config.common.code_root}/application/src/resource/pme_profile/platform_v6.xml {img_path}", shell=True)
        self.work.run_command(f"chmod o-rw {img_path}", ignore_error=True)
        self.work.run_command(f"tar --format=gnu -czf {tar_path} -C {self.config.work_out} rootfs_ro_{self.config.board_name}.img", ignore_error=True)
        if self.config.cb_flag is True:
            cb_img_path = f"{self.config.work_out}/rootfs_CB_FLAG_{self.config.board_name}_release.img"
            cb_tar = f"{self.config.work_out}/CB_FLAG_{self.config.board_name}_release.tar.gz"
            cb_mnt_datafs = f"{self.config.buildimg_dir}/cb_mnt_datafs"
            cb_build_tmp = f"{self.config.common.temp_path}/cb_cfgs_img"
            self.work.run_command(f"cp -a {img_path} {cb_img_path}")
            self.work.run_command(f"rm -rf {cb_mnt_datafs}")
            self.work.run_command(f"mkdir -p {cb_mnt_datafs}")
            try:
                self.config.common.get_img_flag()
                # 指定loop，解决偶现mount失败问题
                self.config.common.get_loop()
                self.work.run_command(f"mount -o loop={self.config.common.loop_dev} -t ext4 {cb_img_path} {cb_mnt_datafs}")
                self.work.run_command(f"sed -i '/IsCustomBrandFW/s#V>.*</V#V>1</V#' {cb_mnt_datafs}/opt/pme/conf/profile/platform.xml")
                self.work.run_command(f"cp -a {cb_build_tmp} {cb_mnt_datafs}/opt/pme/extern")
                self.work.run_command(f"tar --format=gnu -czf {cb_tar} -C {self.config.work_out} rootfs_CB_FLAG_{self.config.board_name}.img", ignore_error=True)
                self.work.run_command(f"chown 0:0 {cb_mnt_datafs}")
                self.config.common.umount_datafs(cb_mnt_datafs)
            except Exception as e:
                self.config.logger.error(f"build cb failed, msg:{e}")
                self.work.run_command(f"rm -rf {self.config.common.code_root}/img.flag")
                os._exit(1)
        self.work.run_command(f"chown {self.config.common.non_root_ug} {self.config.work_out}/*.img", ignore_error=True)
        self.work.run_command(f"chown {self.config.common.non_root_ug} {self.config.work_out}*.tar.gz", ignore_error=True)
        return

    def rootfs_base_cust_deploy(self, work_path: str):
        # 组合包单板自定义配置
        self.delete_unnecessary_app(self.config.buildimg_dir)
        pass

    def rootfs_base_cust_conf(self, work_path: str):
        # 公共包单板自定义配置
        security_enhance_flag = self.work.run_command(f'grep "^{self.config.board_name}$" {self.config.common.code_path}/security_enhance_3v0_board_list || echo "failed"', ignore_error=True, return_result=True)
        if self.config.build_type == "dft_separation" and security_enhance_flag != "failed":
            self.work.run_command(f"rm -rf {work_path}/opt/pme/conf/ssl/piv.conf")
            self.work.run_command(f"rm -rf {work_path}/opt/pme/conf/ssl/piv_v5.conf")
            self.work.run_command(f"rm -rf {work_path}/opt/pme/conf/ssl/piv_v6.conf")
            self.work.run_command(f"rm -rf {work_path}/opt/pme/web/conf/ssl.key/A-server.pfx")
            self.work.run_command(f"rm -rf {work_path}/opt/pme/web/conf/ssl.key/A-server-ECC.pfx")
            self.config.logger.info("in security_enhance_3v0_board_list and remove ssl certification from support ro version.")
        local_switch_support_boards = ["PangeaV6_Atlantic_Smm", "CST0210V6", "CST0220V6", "CST0221V6", "SMM0001V6", "Atlas800D_G1", "Atlas800D_RM"]
        if self.config.board_name not in local_switch_support_boards:
            self.work.run_command(f"rm -rf {work_path}/usr/lib64/librtl8380_sdk_image64.so")
            self.work.run_command(f"rm -rf {work_path}/usr/lib64/libsf2507e.so")
        tpcm_support_boards = ["BM320"]
        if self.config.board_name not in tpcm_support_boards:
            self.work.run_command(f"rm -rf {work_path}/usr/lib64/libtsb.so")

    # 只读包额外配置及修改
    def ro_cust_conf(self, work_path: str, **version_num):
        pass

    # 读写包额外配置
    def rw_cust_conf(self, work_path: str, **version_num):
        pass

    # 装备包额外配置
    def dft_cust_conf(self, work_path: str):
        pass

    ############################## work_build_rootfs_img end ##############################

    ############################## work_build_cfg start ##############################
    # redfish包配置
    def redfish_conf(self, work_path: str, redfish_dstdir: str):
        # redfish权限配置，防止目录发生变化，存储当前目录在动作完成后返回
        before_enter_dir = os.getcwd()
        os.chdir(work_path)
        self.work.run_command("chown -R 102:102 redfish", super_user=True)
        self.work.run_command(f"chmod -R 640 redfish", super_user=True)
        self.work.run_command("find redfish -type d -exec chmod 750 {} \;", super_user=True)
        self.work.run_command("tar --format=gnu -cjf redfish.tar.bz2 redfish", super_user=True)
        # 打包完成后变更属主为当前用户
        self.work.run_command(f"chown {self.config.common.non_root_ug} redfish.tar.bz2", super_user=True, ignore_error=True)
        if self.config.board_version == "V5":
            self.work.run_command(f"rm -rf {redfish_dstdir}/cfgfs/redfish", super_user=True)
            self.work.run_command(f"mkdir -p {redfish_dstdir}/cfgfs/redfish")
            shutil.move("redfish.tar.bz2", f"{redfish_dstdir}/cfgfs/redfish")
            self.config.common.run_command(f"chmod 755 {redfish_dstdir}/cfgfs/redfish")
            self.config.common.run_command(f"chmod 640 {redfish_dstdir}/cfgfs/redfish/*")

        if self.config.board_version == "V6":
            self.config.common.check_path(f"{redfish_dstdir}/redfish")
            self.work.run_command(f"rm -rf {redfish_dstdir}/redfish", super_user=True)
            self.work.run_command(f"mkdir -p {redfish_dstdir}/redfish")
            shutil.move("redfish.tar.bz2", f"{redfish_dstdir}/redfish")
            self.config.common.run_command(f"chmod 755 {redfish_dstdir}/redfish")
            self.config.common.run_command(f"chmod 640 {redfish_dstdir}/redfish/*")
        os.chdir(before_enter_dir)
        pass

    # profile.img文件权限配置
    def profile_img_conf(self, work_path: str):
        before_enter_dir = os.getcwd()
        os.chdir(work_path)
        self.work.run_command("chmod 640 cfgfs/profile/*.xml", super_user=True, ignore_error=True)
        self.work.run_command("chmod 640 cfgfs/profile/readme.txt", ignore_error=True)
        os.chdir(before_enter_dir)

    # web.img文件权限配置
    def web_img_conf(self, work_path: str):
        before_enter_dir = os.getcwd()
        os.chdir(work_path)
        self.work.run_command("chmod 440 -R cfgfs/web", super_user=True)
        self.work.run_command("find cfgfs/web -type d -exec sudo chmod 750 {} \;", super_user=True)
        self.work.run_command("chmod 755 cfgfs", super_user=True)
        os.chdir(before_enter_dir)

    # profile.img和web.img两镜像本身权限配置
    def profile_web_img_packet(self, work_path: str, board_id):
        before_enter_dir = os.getcwd()
        os.chdir(work_path)
        sha = self.work.run_command("sha256sum profile.img", return_result=True)
        self.work.run_command("chown 0:0 *.img", super_user=True, ignore_error=True)
        self.work.run_command("chmod 440 *.img", super_user=True)
        self.work.run_command(f'sed -i "/profile\.img/c profile.img:{sha}" {board_id}.tar.gz.sha256', super_user=True)
        self.work.run_command(f"tar --format=gnu -czf {board_id}.tar.gz profile.img web.img", super_user=True)
        self.work.run_command(f"chown {self.config.common.non_root_ug} {board_id}.tar.gz", super_user=True, ignore_error=True)
        self.work.run_command("rm -rf *.img", super_user=True)
        os.chdir(before_enter_dir)

    ############################## work_build_cfg end ##############################

    ############################## work_build_app start ##############################
    # app目录下文件及权限配置
    def app_package_conf(self, work_path: str):
        os.chdir(work_path)
        # 当前目录不具有读写权限的，可以使用-f强制删除
        subprocess.run("rm -f ./*.sh", shell=True)
        subprocess.run("rm -f ./*.a", shell=True)
        subprocess.run("chmod +x ./*", shell=True)
        # 获取文件符号链接
        self.config.logger.info("-------------------> Get file soft link ! <-------------------")
        os.chdir(self.config.rootfs_path)
        self.config.logger.info(os.getcwd())
        set_file_link_file = "set_file_link.conf" if self.config.board_version == "V5" else "set_file_link_arm64.conf"
        if self.config.board_version == "V6":
            os.makedirs(f"{self.config.rootfs_path}/lib64/security", exist_ok=True)
        command_list = self.work.run_command(
            f'grep -vP "^\s*#|^\s*$" {self.config.rootfs_path}/{set_file_link_file}',
            return_result=True).split("\n")
        for command in command_list:
            subprocess.run(f"{command.rstrip()} > /dev/null 2>&1", shell=True)
        self.work.run_command(f"rm -rf {self.config.rootfs_path}/set_file_link*.conf")

        # 给以下目录下文件增加可执行权限
        self.config.logger.info("-------------------> Add execute permission ! <-------------------")
        add_execable_file_tuple = (
            "./bin/*",
            "./usr/bin/*",
            "./sbin/*",
            "./usr/sbin/*",
            "./etc/init.d/*",
            "./etc/rc.d/*",
            "./etc/sysconfig/network-scripts/*",
            "./usr/local/web/bin/*",
            "./opt/pme/bin/*",
            "./usr/script/*",
            "./usr/libexec/openssh/*",
            "./usr/upgrade/*"
        )
        for file_dir in add_execable_file_tuple:
            self.work.run_command(f"chmod a+x {file_dir} > /dev/null 2>&1", super_user=True, ignore_error=True)
        if self.config.board_version == "V5":
            self.work.run_command("chmod a+x ./etc/rc.d/rc.stop/* > /dev/null 2>&1", super_user=True, ignore_error=True)

    ############################## work_build_app start ##############################
