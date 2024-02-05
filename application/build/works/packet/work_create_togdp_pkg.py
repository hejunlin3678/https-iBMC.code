#!/usr/bin/env python
# coding: utf-8
"""
功 能：work_create_togdp_pkg.py脚本
版权信息：华为技术有限公司，版本所有(C) 2022
"""

import os
import shutil
import subprocess
import sys

cwd=os.path.dirname(__file__)
sys.path.append(f"{cwd}/../")
sys.path.append(f"{cwd}/../../")
sys.path.append(f"{cwd}/../../../")
from works.work import Work
from utils.config import Config
from utils.error_info import EXCEPTION_CODE
from datetime import datetime

class Togdb(Work):
    def __init__(self, config, work_name=""):
        super(Togdb, self).__init__(config)
        self.config = config
        self.archive_conf_dic = self.common.get_archive_config(self.config.board_name)
        self.release_dir = f"{self.config.board_name}_togdb_release"
        self.resource_dir = f"{self.common.code_root}/application/src/resource"
        self.archive_emmc_nand_dir = f"{self.resource_dir}/board/{self.config.board_name}/archive_emmc_nand"
        self.customize_dir = f"{self.release_dir}/dft_tools/config/Common/customize"
        self.sw_code = self.archive_conf_dic['togdp_dir'].split('/')[-1]
        self.version = self.common.get_board_version(self.config.board_name) 
        self.product_arch = self.common.get_product_arch(self.config.board_name) 
        if self.config.board_name == "RM211":
            self.bin_dir_name = "burned_file"
            self.other_dir_name = "other_documents"
        else:
            self.bin_dir_name = "烧片文件"
            self.other_dir_name = "其它文档"
        self.bin_dir = f"{self.release_dir}/{self.bin_dir_name}"
        self.other_dir = f"{self.release_dir}/{self.other_dir_name}"

    def modify_copyright_year(self, file):
        new_year = datetime.now().year
        self.common.py_sed(file, "\d{4}(?=\. All rights reserved)", str(new_year))
        return

    def delete_unification_unnecessary_files(self):
        self.logger.info(f"delete_unification_unnecessary_files  ----------> [begin]")

        # 如果配置TOGDP_DIR_FCUS(归一0502编码)，则进行正向文件删除
        if self.archive_conf_dic.get("togdp_dir_fcus", "") != "":
            self.logger.info("config TOGDP_DIR_BCUS,so delete custom forward pkg")
            self.common.remove_file(f"{self.release_dir}/{self.archive_conf_dic['togdp_fcus_0502']}_load.sh")

        # 如果配置TOGDP_DIR_FCUS(归一0502编码)，则进行逆向文件删除
        if self.archive_conf_dic.get("togdp_bcus_0502", "") != "":
            self.logger.info("config TOGDP_DIR_BCUS,so delete custom forward pkg")
            self.common.remove_file(f"{self.release_dir}/{self.archive_conf_dic['togdp_bcus_0502']}_load.sh")

        # 如果配置TOGDP_DIR_HQE(k与非k归一0502编码)，则进行升级为k文件删除
        if self.archive_conf_dic.get("togdp_dir_hqe", "") != "":
            self.logger.info("cinfig TOGDP_DIR_HQE,so delete custom forward pkg")
            self.common.remove_file(f"{self.release_dir}/{self.archive_conf_dic['togdp_hqe_0502']}_load.sh")

        # 如果配置TOGDP_DIR_HQH(k与非k归一0502编码)，则进行升级为非k文件删除
        if self.archive_conf_dic.get("togdp_dir_hqh", "") != "":
            self.logger.info("cinfig TOGDP_DIR_HQH,so delete custom forward pkg")
            self.common.remove_file(f"{self.release_dir}/{self.archive_conf_dic['togdp_hqh_0502']}_load.sh")
        
        # 如果配置TOGDP_DIR_HMF(归一0502编码)，则进行防回退文件删除
        if self.archive_conf_dic.get("togdp_dir_hmf", "") != "":
            self.logger.info("config TOGDP_DIR_HMF,so delete custom forward pkg")
            self.common.remove_file(f"{self.release_dir}/{self.archive_conf_dic['togdp_hmf_0502']}_load.sh")
        
        # 如果配置TOGDP_DIR_HMF(归一0502编码)，则进行防回退文件删除
        if self.archive_conf_dic.get("togdp_dir_fw_revs", "") != "":
            self.logger.info("config TOGDP_DIR_FW_REVS,so delete custom forward pkg")
            self.common.remove_file(f"{self.release_dir}/{self.archive_conf_dic['togdp_fw_revs_0502']}_load.sh")

        # 如果配置TOGDP_DIR_EFUSE_PARTENR_TCM(归一0502编码)，则进行文件删除
        if self.archive_conf_dic.get("togdp_dir_efuse_partner_tcm", "") != "":
            self.logger.info("config TOGDP_DIR_EFUSE_PARTENR_TCM,so delete custom forward pkg")
            self.common.remove_file(f"{self.release_dir}/{self.archive_conf_dic['togdp_efuse_partner_tcm_0502']}_load.sh")

        self.common.remove_file(f"{self.release_dir}/upgrade_hpm.sh")
        self.common.remove_file(f"{self.release_dir}/wbd_load.sh")

    def delete_revision_unnecessary_files(self):
        self.logger.info(f"delete_revision_unnecessary_files  ----------> [begin]")

        self.common.remove_file(f"{self.release_dir}/revision.ini")
        # 如果配置TOGDP_DIR_FCUS(归一0502编码)，则进行正向文件删除
        if self.archive_conf_dic.get("togdp_dir_tbj", "") != "":
            self.common.remove_file(f"{self.release_dir}/revision_tbj.ini")
            self.common.remove_file(f"{self.release_dir}/05023TBJ_load.sh")

    def delete_efuse_unnecessary_files(self):
        self.logger.info(f"delete_efuse_unnecessary_files  ----------> [begin]")

        # 如果配置TOGDP_DIR_FCUS(归一0502编码)，则进行正向文件删除
        if self.archive_conf_dic.get("togdp_dir_efuse", "") != "":
            self.logger.info("config TOGDP_DIR_EFUSE,so delete load efuse pkg")
            self.common.remove_file(f"{self.release_dir}/{self.archive_conf_dic['togdp_efuse_0502']}_load.sh")

    def create_cb_load_pkg(self, ver_num):
        cb_archives = self.common.py_find_dir(f".*_cb_flag_archive.ini", f"{self.resource_dir}/board/{self.config.board_name}")
        for cb_archive in cb_archives:
            cb_archive_dic = self.common.get_cb_archive_config(cb_archive)
            if self.config.build_type == "dft_separation":
                # 支持单板装备分离下白牌不装备分离(如2288h v6)
                if len(cb_archive_dic['cb_flag_togdp_dir_delivery']) == 0 or len(cb_archive_dic['cb_flag_togdp_name_delivery']) == 0:
                    self.logger.info(f"Board is DFT-Separation but CustomBrand is not DFT-Separation")
                    continue
                target_pkg = f"{cb_archive_dic['cb_flag_togdp_name_delivery']}{ver_num}"
               
            else:
                target_pkg = f"{cb_archive_dic['cb_flag_togdp_name']}{ver_num}"
            
            # 生成ToGDP目录下的zip包
            os.chdir(self.config.build_path)
            self.common.check_path(f"{cb_archive_dic['cb_flag_sw_code']}_tmp")
            self.common.copy_all(f"{self.release_dir}", f"{cb_archive_dic['cb_flag_sw_code']}_tmp/{target_pkg}")
            # 原来的image.hpm改为hw_image.hpm依旧保留
            self.common.run_command(f"mv {cb_archive_dic['cb_flag_sw_code']}_tmp/{target_pkg}/image.hpm {cb_archive_dic['cb_flag_sw_code']}_tmp/{target_pkg}/hw_image.hpm")
            cb_other_dir = f"{cb_archive_dic['cb_flag_sw_code']}_tmp/{target_pkg}/{self.other_dir_name}"
            self.common.remove_file(f"{cb_other_dir}/rootfs_rw_{self.config.board_name}.hpm")
            self.common.remove_file(f"{cb_archive_dic['cb_flag_sw_code']}_tmp/{target_pkg}/clear_wbd_config.hpm")
            self.common.copy(f"{self.resource_dir}/config/Common/custombrand/cb_clear_wbd_config_v5.hpm", f"{cb_archive_dic['cb_flag_sw_code']}_tmp/{target_pkg}/clear_wbd_config.hpm")
            if self.config.build_type == "dft_separation":
                # 修改现网包version.ini版本号
                self.common.extract_ver_num(f"{cb_archive_dic['cb_flag_sw_code']}_tmp/{target_pkg}/{self.sw_code}_version.ini", self.config.board_name)
                self.common.copy(f"{self.common.work_out}/rootfs_rw_cb_flag_{self.config.board_name}_{cb_archive_dic['cb_flag_sw_code']}_release.hpm", f"{cb_archive_dic['cb_flag_sw_code']}_tmp/{target_pkg}/image.hpm")
                self.common.copy(f"{self.common.work_out}/rootfs_rw_cb_flag_{self.config.board_name}_{cb_archive_dic['cb_flag_sw_code']}_release.hpm", f"{cb_other_dir}/rootfs_rw_cb_flag_{self.config.board_name}_{cb_archive_dic['cb_flag_sw_code']}.hpm")
                os.chdir(f"{cb_archive_dic['cb_flag_sw_code']}_tmp")
                # 将华为的0502加载文件全部改为转售的
                self.common.run_command(f"rename {target_pkg}/{self.sw_code}_  {target_pkg}/{cb_archive_dic['cb_flag_sw_delivery_code']}_ {target_pkg}/*")
            else:
                self.common.copy(f"{self.common.work_out}/rootfs_rw_cb_flag_{self.config.board_name}_{cb_archive_dic['cb_flag_sw_code']}.hpm", f"{cb_archive_dic['cb_flag_sw_code']}_tmp/{target_pkg}/image.hpm")
                self.common.copy(f"{self.common.work_out}/rootfs_rw_cb_flag_{self.config.board_name}_{cb_archive_dic['cb_flag_sw_code']}.hpm", f"{cb_other_dir}/rootfs_rw_cb_flag_{self.config.board_name}_{cb_archive_dic['cb_flag_sw_code']}.hpm")
                os.chdir(f"{cb_archive_dic['cb_flag_sw_code']}_tmp")
                # 将华为的0502加载文件全部改为转售的
                self.common.run_command(f"rename {target_pkg}/{self.sw_code}_  {target_pkg}/{cb_archive_dic['cb_flag_sw_code']}_ {target_pkg}/*")
           
            # 用0502XXX_cb_flag_load.sh覆盖原先的load.sh
            self.common.copy(f"{self.resource_dir}/board/{self.config.board_name}/{cb_archive_dic['cb_flag_sw_code']}_cb_flag_load.sh", f"{target_pkg}/load.sh")
            self.common.run_command(f"zip -r {target_pkg}.zip {target_pkg}")
            if self.config.build_type == "dft_separation":
                self.common.check_path(f"{self.common.work_out}/packet/{cb_archive_dic['cb_flag_togdp_dir_delivery']}")
                self.common.run_command(f"mv \"{self.config.build_path}/{cb_archive_dic['cb_flag_sw_code']}_tmp/{target_pkg}.zip\" \"{self.common.work_out}/packet/{cb_archive_dic['cb_flag_togdp_dir_delivery']}/\"")
            else:
                self.common.check_path(f"{self.common.work_out}/packet/{cb_archive_dic['cb_flag_togdp_dir']}")
                self.common.run_command(f"mv \"{self.config.build_path}/{cb_archive_dic['cb_flag_sw_code']}_tmp/{target_pkg}.zip\" \"{self.common.work_out}/packet/{cb_archive_dic['cb_flag_togdp_dir']}/\"")
            self.common.remove_path(f"{self.config.build_path}/{cb_archive_dic['cb_flag_sw_code']}_tmp")
            os.chdir(self.config.build_path)

    def create_script_pkg(self, ver_num):
        os.chdir(self.config.build_path)
        self.logger.info(f"create script pkg begin")

        # 创建临时打包目录 boardname_togdp_script_tmp
        self.common.check_path(f"{self.config.board_name}_togdp_script_tmp")

        # 拼接打包文件名
        target_pkg = f"{self.archive_conf_dic['togdp_name_script']}{ver_num}"
        # copy到包文件到打包文件目录下
        self.common.copy_all(f"{self.release_dir}", f"{self.config.board_name}_togdp_script_tmp/{target_pkg}")

        # 进入boardname_togdp_script_tmp目录
        os.chdir(f"{self.config.board_name}_togdp_script_tmp")

        # 修改打包文件0502脚本名 为新的编码 
        new_code = self.archive_conf_dic['togdp_dir_script'].split("/")[-1];
        self.common.run_command(f"rename {target_pkg}/{self.sw_code}_  {target_pkg}/{new_code}_ {target_pkg}/*")

        os.chdir(f"{target_pkg}")
        # 删除hpm文件
        self.common.run_command(f"rm image.hpm")
        self.common.run_command(f"rm -r {self.other_dir_name}")

        os.chdir(self.config.build_path)
        os.chdir(f"{self.config.board_name}_togdp_script_tmp")
        # 生产压缩包
        subprocess.run(f'zip -rq "{target_pkg}".zip {target_pkg}', shell=True)

        # 创建0502编码目录
        self.common.check_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_script']}")
    
        # 如果存在旧的0502文件则需要先删除
        if os.path.exists(f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_script']}/{target_pkg}.zip"):
            os.remove(f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_script']}/{target_pkg}.zip")
        # 输出ZIP文件到0502目录下
        shutil.move(f"{self.config.build_path}/{self.config.board_name}_togdp_script_tmp/{target_pkg}.zip",
                    f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir_script']}")

        #删除临时目录
        self.common.remove_path(f"{self.config.build_path}/{self.config.board_name}_togdp_script_tmp")

        # 判断是否生产文件成功
        if not os.path.exists(f'{self.common.work_out}/packet/{self.archive_conf_dic.get("togdp_dir_script", "")}/{target_pkg}.zip'):
            err_msg = f"{EXCEPTION_CODE[634]}{EXCEPTION_STAGE['P']}{EXCEPTION_RESPONSE[1]}"
            self.logger.error(f"{err_msg}create packet for {self.config.board_name} : {target_pkg}.zip  failed")
        else:
            self.logger.info(f"create packet for {self.config.board_name} : {target_pkg}.zip  success")

        os.chdir(self.config.build_path)

    def run(self):
        if not self.config.support_dft_separation and self.config.build_type == "dft_separation":
            return
        self.logger.info(f"create compressed packet for {self.config.board_name} of  Togdb GDP PKG ------------> [begin]")
        # 创建临时目录 
        os.chdir(self.config.build_path)
        self.common.remove_path(self.release_dir)
        self.common.check_path(self.release_dir)
        self.common.remove_path(self.bin_dir)
        self.common.check_path(self.bin_dir)
        self.common.remove_path(self.other_dir)
        self.common.check_path(self.other_dir)

        self.common.get_dft_tools(self.release_dir, self.config.board_name)
        self.modify_copyright_year(f"{self.customize_dir}/function/sys_check.sh")
        self.modify_copyright_year(f"{self.customize_dir}/function/sys_config.sh")
        # copy archive 下的文件
        file_list = self.common.py_find_dir(".*version\.ini", self.archive_emmc_nand_dir)
        for copy_file in file_list:
            shutil.copy(copy_file, self.release_dir)
        
        if self.config.board_name != "SMM":
            self.common.copy(f"{self.customize_dir}/config.sh", f"{self.release_dir}/config.sh")
            self.common.copy(f"{self.customize_dir}/white_branding.sh", f"{self.release_dir}/white_branding.sh")
            self.common.copy(f"{self.customize_dir}/bmc_upload_cmes.sh", f"{self.release_dir}/bmc_upload_cmes.sh")
            self.common.copy(f"{self.customize_dir}/{self.product_arch}/conver", f"{self.release_dir}/conver")
            self.common.copy(f"{self.customize_dir}/verify.sh", f"{self.release_dir}/verify.sh")
            self.common.copy(f"{self.customize_dir}/defaultserverport.ini", f"{self.release_dir}/defaultserverport.ini")
            self.common.copy(f"{self.customize_dir}/common_factory_restore.sh", f"{self.release_dir}/common_factory_restore.sh")
            self.common.copy(f"{self.resource_dir}/config/Common/equipment_test/equipment_test_alarm_wlist.ini",
                f"{self.release_dir}/equipment_test_alarm_wlist.ini")
            self.common.copy(f"{self.resource_dir}/config/Common/equipment_test/equipment_TSS_test_alarm_wlist.ini",
                f"{self.release_dir}/equipment_TSS_test_alarm_wlist.ini")
            self.common.copy(f"{self.resource_dir}/config/Common/equipment_test/dft_enable.sh",
                f"{self.release_dir}/dft_enable.sh")

        if self.config.board_name != "xr320":
            self.common.copy(f"{self.customize_dir}/dftimage_v5.hpm", f"{self.release_dir}/dftimage.hpm")
            self.common.copy(f"{self.customize_dir}/dftdisableimage_v5.hpm", f"{self.release_dir}/dftdisableimage.hpm")

            # 安全红线3.0机型使用单独的去白牌包：此白牌包中不带有iBMC的默认证书
            if len(self.common.file_find_text(self.config.board_name, f"{self.common.code_path}/security_enhance_3v0_board_list")):
                self.common.copy(f"{self.customize_dir}/clear_wbd_config_3v0.hpm", f"{self.release_dir}/clear_wbd_config.hpm")
                self.logger.info("in security_enhance_3v0_board_list and use clear_wbd_config_3v0 hpm.")
            else:
                self.common.copy(f"{self.customize_dir}/clear_wbd_config_v5.hpm", f"{self.release_dir}/clear_wbd_config.hpm")
        else:
            self.common.copy(f"{self.customize_dir}/dftimage.hpm", f"{self.release_dir}/dftimage.hpm")
            self.common.copy(f"{self.customize_dir}/dftdisableimage.hpm", f"{self.release_dir}/dftdisableimage.hpm")
            self.common.copy(f"{self.customize_dir}/clear_wbd_config.hpm", f"{self.release_dir}/clear_wbd_config.hpm")

        self.common.copy_all(f"{self.customize_dir}/function", f"{self.release_dir}/function")

        if self.config.support_dft_separation :
            self.common.run_command(f"sed -i 's/EQUIPMENT_SEPARATION_SUPPORT_STATE=.*/EQUIPMENT_SEPARATION_SUPPORT_STATE=1/' {self.release_dir}/function/common.sh")
            self.common.copy(f"{self.customize_dir}/wbd-clearlog-image.hpm",
                        f"{self.release_dir}/wbd-clearlog-image.hpm")

        self.common.copy(f"{self.customize_dir}/driver.tar.gz", f"{self.release_dir}/driver.tar.gz")
        # 拷贝健康状态解析工具
        self.common.copy(
            f"{self.release_dir}/dft_tools/GetBmcHealthevents/{self.product_arch}/GetBmcHealthevents",
            f"{self.release_dir}/GetBmcHealthevents")
        # 拷贝高温拷机文件
        self.common.copy_all(
            f"{self.release_dir}/dft_tools/bake/RackServer/bake_config_file",
            f"{self.release_dir}/bake_config_file")
        self.common.copy(
            f"{self.release_dir}/dft_tools/bake/RackServer/bake.sh",
            f"{self.release_dir}/bake.sh")
        self.common.check_path(f"{self.release_dir}/bake_control_file")
        self.common.copy(
            f"{self.release_dir}/dft_tools/bake/RackServer/bake_control_file/bake_control",
            f"{self.release_dir}/bake_control_file/bake_control")

        # 检查单板是否有专有的定制化脚本及配置，如有，将其目录下的文件拷贝过来，覆盖同名文件和目录
        if os.path.isdir(f"{self.resource_dir}/board/{self.config.board_name}/archive/proprietary"):
            self.common.copy_all(
                f"{self.resource_dir}/board/{self.config.board_name}/archive/proprietary",
                self.release_dir)
            self.common.copy_all(f"{self.release_dir}/dft_tools/config/{self.config.board_name}",
                            self.release_dir)

        if self.config.board_name == "SMM":
            self.common.copy_all(f"{self.release_dir}/dft_tools/config/{self.config.board_name}/ft_st_tool", self.release_dir)
            self.common.copy_all(f"{self.resource_dir}/board/{self.config.board_name}/conf/datafs/opt/pme/conf/smm_aging_test_conf.ini",
                            self.release_dir)
            os.remove(f"{self.release_dir}/clear_wbd_config.hpm")
            os.remove(f"{self.release_dir}/bake.sh")
            os.remove(f"{self.release_dir}/driver.tar.gz")
            self.common.remove_path(f"{self.release_dir}/bake_config_file")
            self.common.remove_path(f"{self.release_dir}/bake_control_file")
            self.common.remove_path(f"{self.release_dir}/GetBmcHealthevents")
            self.common.remove_path(f"{self.release_dir}/function")
            self.common.remove_path(self.bin_dir)

        else:
            self.common.remove_path(f"{self.release_dir}/function/.svn")
            self.run_command(f"chmod +x {self.release_dir}/function/*.sh")
            os.chdir(self.release_dir)
            # 解决Bep 比对差异 加入--format=gnu 参数
            self.run_command("tar --format=gnu -czf function.tar.gz function")
            os.chdir(self.config.build_path)
            self.common.remove_path(f"{self.release_dir}/function")
            self.common.copy(
                f"{self.release_dir}/dft_tools/hpmfwupg/{self.product_arch}/hpmfwupg_v5",
                f"{self.release_dir}/hpmfwupg")
            # 备注要整改
            ipmcimage_dir = f"{self.common.code_root}/tools/toolchain/hpmpack/ipmcimage"
            if os.path.exists(f"{ipmcimage_dir}/{self.product_arch}/crypto_tool_ext4"):
                self.common.copy(f"{ipmcimage_dir}/{self.product_arch}/crypto_tool_ext4",
                            f"{self.release_dir}/crypto_tool")
            else:
                self.common.copy(f"{ipmcimage_dir}/{self.product_arch}/crypto_tool",
                            f"{self.release_dir}/crypto_tool")

        self.common.copy_all(self.archive_emmc_nand_dir, self.release_dir)
        self.common.copy_all(f"{self.release_dir}/dft_tools/config/{self.config.board_name}/archive_emmc_nand",
            self.release_dir)

        need_ro_list = ["1288hv6_2288hv6_5288v6", "2288hv5", "2288HV6-16DIMM", "2288xv5_1711", "2488hv6", "BM320", 
                        "Atlas200I_SoC_A1", "Atlas500_3000", "Atlas800_9010", "Atlas800D_G1", "Atlas880", 
                        "Atlas900_PoD_A2", "S902X20", "S920S03", "S920X05", "TaiShan2280Ev2", "TaiShan2280Mv2", 
                        "TaiShan2280v2", "TaiShan2280v2_1711", "TaiShan2280v2Pro", "TaiShan2480v2", "TaiShan2480v2Pro", "TaiShan5290v2"]
        if self.config.board_name in need_ro_list:
            self.common.copy(f"{self.common.temp_path}/release/output/rootfs_{self.config.board_name}.hpm",
                    f"{self.release_dir}/image.hpm")
        else:
            self.common.copy(f"{self.common.work_out}/rootfs_rw_{self.config.board_name}.hpm",
                    f"{self.release_dir}/image.hpm")
        if self.config.board_name != "SMM":
            self.common.copy(f"{self.config.work_out}/Hi1711_boot_4096.bin",
                    f"{self.other_dir}/Hi1711_boot_4096.bin")

        self.common.copy(f"{self.config.work_out}/u-boot.bin", f"{self.other_dir}/u-boot.bin")
        self.common.copy(f"{self.common.work_out}/u-boot_debug.bin", f"{self.other_dir}/u-boot_debug.bin")
        self.common.copy(f"{self.common.work_out}/rootfs_rw_{self.config.board_name}.hpm",
                    f"{self.other_dir}/rootfs_rw_{self.config.board_name}.hpm")

        # 删除RM210单板多余ipmi_control脚本，该脚本用于装备测试阶段控制ipmi功能，已有单独0502包
        self.common.remove_file(f"{self.release_dir}/ipmi_enable.sh")
        self.common.remove_file(f"{self.release_dir}/ipmi_disable.sh")

        tce_tuple = ("CM221", "CM221S", "CM221")
        if self.config.board_name in tce_tuple:
            try:
                files = os.listdir(f"{self.release_dir}/dft_tools/config/{self.config.board_name}")
                for file in files:
                    if ".so" in file:
                        self.common.copy(f"{self.release_dir}/dft_tools/config/{self.config.board_name}/{file}",
                                        f"{self.release_dir}/{file}")
            except:
                self.logger.info(f"{self.config.board_name} no have this *.so")
            self.common.copy_all(f"{self.release_dir}/dft_tools/config/{self.config.board_name}/efuse-crypt-image.hpm", f"{self.release_dir}/efuse_image.hpm")
            os.remove(f"{self.other_dir}/Hi1711_boot_4096.bin")

        found_list = self.common.py_find_dir("\.svn", self.release_dir, True)
        found_list += self.common.py_find_dir("\.gitkeep", self.release_dir, True)
        if len(found_list):
            for found_file in found_list:
                if os.path.isdir(found_file):
                    shutil.rmtree(found_file)
                else:
                    os.remove(found_file)

        # 删除归一多余文件
        self.delete_unification_unnecessary_files()
        # 删除防回退多余文件
        self.delete_revision_unnecessary_files()
        # 删除升级efuse多余文件
        self.delete_efuse_unnecessary_files()  

        os.chdir(f"{self.config.build_path}/{self.release_dir}")
        # 更新version.ini文件；
        ar_ver_file = f"{self.sw_code}_version.ini"
        if not os.path.exists(ar_ver_file):
            self.logger.error("can not find version profile")
            self.common.remove_path(f"{self.config.build_path}/{self.release_dir}")
            return
        # 提取xml中的版本号
        if self.config.support_dft_separation:
            ver_num = self.common.extract_ver_num(ar_ver_file, self.config.board_name, dft_flag=True)
        else:
            ver_num = self.common.extract_ver_num(ar_ver_file, self.config.board_name)

        add_execute_file_list = self.common.py_find_dir(".*\.sh", ".")
        if self.config.board_name != "SMM":
            add_execute_file_list += ["conver", "hpmfwupg"]
        for add_execute_file in add_execute_file_list:
            self.run_command(f"chmod +x {add_execute_file}")

        del_execute_file_list = self.common.py_find_dir(".*\.ini", ".")
        del_execute_file_list += self.common.py_find_dir(".*\.cfg", ".")
        for del_execute_file in del_execute_file_list:
            self.run_command(f"chmod -x {del_execute_file}")

        if not os.path.exists("archive.ini"):
            self.logger.error(f"cannot find archive config file - archive.ini")
            os._exit(1)

        tce_tuple = ("CM221", "CM221S", "CM221", "SMM")
        if self.config.board_name in tce_tuple:
            if self.archive_conf_dic.get("togdp_code_efuse", "") != "":
                self.common.remove_path(f"{self.config.build_path}/{self.release_dir}/{self.archive_conf_dic['togdp_code_efuse']}")


        if self.config.board_name == "2288hv5":
            if os.path.exists(f'{self.archive_conf_dic.get("togdp_eco_ft_0502", "")}_check.sh'):
                self.logger.info("clear eco_ft check.sh")
                os.remove(f"{self.archive_conf_dic['togdp_eco_ft_0502']}_check.sh")
        os.remove("archive.ini")

        os.chdir(self.config.build_path)
      
        self.common.remove_path(f"{self.release_dir}/dft_tools")
        if self.config.cb_flag:
            self.create_cb_load_pkg(ver_num)
        # 没有配置 TOGDP_PATH_SCRIPT ，不需要打包。
        if self.archive_conf_dic.get("togdp_dir_script", "") != "":
            self.create_script_pkg(ver_num)

        # 针对RM210还有一个发货包，工具和ST装备包很多相同，因此在这里打包
        irm_board_list = ("RM210", "RM210_SECURITY_3V0", "RM211")
        if self.config.support_dft_separation and (self.common.no_need_support_pkg(self.config.board_name) == False) and self.config.build_type == "dft_separation":
            self.logger.info(f"Begin packet for DELIVERY_PKG {self.archive_conf_dic['togdp_path_delivery']} {self.archive_conf_dic['togdp_name_delivery']}")
            self.common.copy(f"{self.config.build_path}/target_app/src/exec_tools/clearlog/clearlog", 
            f"{self.config.build_path}/{self.release_dir}/clearlog")
            os.chdir(self.config.build_path)
            self.logger.info("create packet for DELIVERY_PKG")
            shutil.move(f"{self.release_dir}/{self.archive_conf_dic['delivery_0502_name']}_version.ini", "temp_version.ini")
            ver_num = self.common.extract_ver_num("temp_version.ini", self.config.board_name)
            delivery_pkg = f"{self.archive_conf_dic['togdp_name_delivery']}{ver_num}"
            os.rename(self.release_dir, delivery_pkg)
            self.run_command(f"rm -rf {delivery_pkg}/{self.archive_conf_dic['togdp_0502_name']}*.ini")
            
            shutil.move(f"temp_version.ini", f"{delivery_pkg}/{self.archive_conf_dic['delivery_0502_name']}_version.ini")

            remove_file_list = self.common.py_find_dir(f"{self.archive_conf_dic['togdp_0502_name']}.*\.sh", delivery_pkg)
            for remove_file in remove_file_list:
                if os.path.isdir(remove_file):
                    shutil.rmtree(remove_file)
                else:
                    self.common.remove_file(remove_file)
            self.common.copy(f"{self.common.work_out}/rootfs_rw_{self.config.board_name}_release.hpm", f"{delivery_pkg}/{self.other_dir_name}/rootfs_rw_{self.config.board_name}_release.hpm")
            self.common.copy(f"{self.common.work_out}/rootfs_{self.config.board_name}_release.hpm", f"{delivery_pkg}/image.hpm")
            if self.version == "V6":
                os.chdir(delivery_pkg)
                self.common.create_sign_filelist(self.config.board_name, "sep")
                os.chdir(self.config.build_path)

            self.run_command(f'zip -1 -rq "{delivery_pkg}".zip {delivery_pkg}')
            self.common.check_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_path_delivery']}")
            if os.path.exists(f'{self.common.work_out}/packet/{self.archive_conf_dic.get("togdp_path_delivery", "")}/{delivery_pkg}.zip'):
                os.remove(f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_path_delivery']}/{delivery_pkg}.zip")
            self.logger.info(f"move file to '{self.common.work_out}/packet/{self.archive_conf_dic['togdp_path_delivery']}'")
            shutil.move(f"{delivery_pkg}.zip", f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_path_delivery']}")
            self.common.remove_path(delivery_pkg)

            self.logger.info(f"create packet for {self.config.board_name} : {delivery_pkg}.zip  success")
            return

        elif self.config.board_name in irm_board_list:
            self.common.copy(f"{self.config.build_path}/target_app/src/exec_tools/clearlog/clearlog", 
            f"{self.config.build_path}/{self.release_dir}/clearlog")
        # 删除不需要的文件
        if self.config.support_dft_separation:
            self.common.run_command(f"rm -rf {self.release_dir}/{self.archive_conf_dic['delivery_0502_name']}*.sh")
            self.common.run_command(f"rm -rf {self.release_dir}/{self.archive_conf_dic['delivery_0502_name']}*.ini")

        target_pkg = f"{self.archive_conf_dic['togdp_name']}{ver_num}"
        os.chdir(self.config.build_path)
        os.rename(self.release_dir, target_pkg)
        if self.config.board_name in irm_board_list:
            self.logger.info("RM210/RM211 rw hpm instead r hpm===================")
            self.common.copy(f"{self.common.work_out}/rootfs_rw_{self.config.board_name}.hpm",
                    f"{target_pkg}/image.hpm")
        if self.version == "V6":
            os.chdir(target_pkg)
            self.common.create_sign_filelist(self.config.board_name)
            os.chdir(self.config.build_path)

        self.run_command(f'zip -1 -rq "{target_pkg}".zip {target_pkg}')
        self.common.check_path(f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir']}")
        if os.path.exists(f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir']}/{target_pkg}.zip"):
            os.remove(f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir']}/{target_pkg}.zip")
        self.logger.info(f"move file to '{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir']}'")
        shutil.move(f"{target_pkg}.zip", f"{self.common.work_out}/packet/{self.archive_conf_dic['togdp_dir']}")
        self.common.remove_path(target_pkg)
        self.logger.info(f"create compressed packet for {self.config.board_name} of Togdb GDP PKG ------------> [end]")



if __name__ == "__main__":
    cfg = Config("1288hv6_2288hv6_5288v6", "target_personal")
    cfg.set_build_type("debug")
    wk = Togdb(cfg)
    wk.run()