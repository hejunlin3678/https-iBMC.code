#!/usr/bin/env python
# coding=utf-8

"""
文件名：work_build_app.py
功能：编译app，依赖work_build_app_prepare.py准备环境
版权信息：华为技术有限公司，版本所有(C) 2019-2020
"""

import os
import time
import shutil
import sys

import argparse
import subprocess

cwd=os.path.dirname(__file__)
sys.path.append(f"{cwd}/../")
sys.path.append(f"{cwd}/../../")
from works.work import Work
from utils.config import Config
from utils.common import Common
from utils.error_info import EXCEPTION_STAGE
from utils.error_info import EXCEPTION_RESPONSE
from utils.error_info import EXCEPTION_CODE

class WorkBuildApp(Work):
    '''
    '''
    timeout = 600
    verbose = False
    no_pme_build = ""

    def set_verbose(self, verbose):
        if verbose:
            self.verbose = True
        else:
            self.verbose = False
        self.logger.info(f"set verbose:{self.verbose}")

    def set_global(self):
        self.cmake_cmd = ""

        # 复制头文件到构建目录
        subprocess.run(f"cp -af {self.common.code_root}/tools/others/other_tools/open_source \
            {self.common.code_root}/application/src/include/", shell=True)
        # 设置和打印编译器相关环境变量
        self.config.set_env_global()
        self.common.path_env_revise()
        self.target_path = f"{self.config.build_path}/target_app"
        self.common.remove_path(self.target_path)
        self.common.check_path(self.target_path)
        from_source_cmd = "-DFROM_SOURCE=false"
        fuzz_cov_cmd = ""
        if self.config.fuzz_cov:
            fuzz_cov_cmd = "-DFUZZ_COV=ON"
        asan_test_cmd = ""
        if self.config.asan_test:
            asan_test_cmd = "-DASAN_TEST=ON"

        support_pidspeed_regulation = self.common.get_PIDspeed_regulation_flag(self.config.board_name)
        if support_pidspeed_regulation:
            pidspeed_regulation = "-DPIDSPEED_REGULATION=1"
        else:
            pidspeed_regulation = "-DPIDSPEED_REGULATION=0"

        cmake_cmd_args = f"-DDFT_ENABLED=1 -DCMAKE_INSTALL_PREFIX={self.config.rootfs_path} {from_source_cmd} {self.common.pme_dir} {pidspeed_regulation} {fuzz_cov_cmd} {asan_test_cmd}"
        if self.config.board_version == "V5":
            cmake_file = f"{self.config.build_path}/build.cmake"
        elif self.config.board_version == "V6":
            cmake_file = f"{self.common.code_path}/arch/arm64/aarch64-linux-gnu-ultimate.cmake"
        ret = subprocess.run("ccache -V > /dev/null", shell=True)
        if ret.returncode == 0:
            self.logger.info("enable ccache")
            subprocess.run(f"sed -i 's/CMAKE_C_COMPILER[ ][ ]*a/CMAKE_C_COMPILER ccache a/g' {cmake_file}", shell=True)
            subprocess.run(f"sed -i 's/CMAKE_CXX_COMPILER[ ][ ]*a/CMAKE_CXX_COMPILER ccache a/g' {cmake_file}", shell=True)
        self.cmake_cmd = f"-DCMAKE_TOOLCHAIN_FILE={cmake_file} {cmake_cmd_args}"

        if not os.path.exists(f"{self.common.temp_path}/cmake_env_dir"):
            os.makedirs(f"{self.common.temp_path}/cmake_env_dir", exist_ok=True)

        self.cmake_env_dir = f"{self.common.temp_path}/cmake_env_dir"
        return

    def get_cs4343_inc(self):
        self.common.check_path(f"{self.common.code_root}/application/src/include/pme_app/cs4343")
        self.common.copy_all(f"{self.common.code_root}/vendor/CS4343_PHY_API/inc",
                        f"{self.common.code_root}/application/src/include/pme_app/cs4343")
        self.common.copy_all(f"{self.common.code_root}/vendor/CS4343_PHY_API/src",
                        f"{self.common.code_root}/application/src/libs/cs4343/src")

    def generate_cmake_file(self, src_cmake):
        os.chdir(f"{self.common.code_path}/arch/arm")
        if os.path.isfile(f"{self.config.build_path}/build.cmake"):
            os.remove(f"{self.config.build_path}/build.cmake")
        if self.common.compability_enable == 1:
            subprocess.run(f"cat {src_cmake} compatibility_enable > {self.config.build_path}/build.cmake",
                        shell=True)
        else:
            shutil.copyfile(src_cmake, f"{self.config.build_path}/build.cmake")
        subprocess.run(f"chmod +x {self.config.build_path}/build.cmake", shell=True)
        return

    def rm_unused_file_in_release_package(self):
        # 删除${G_RELEASE_INSTALL_PREFIX}目录下冗余的文件
        file_list=("usr/lib/libnetsnmp.a","usr/lib/libnetsnmptrapd.a")
        for i in file_list:
            if not os.path.exists(os.path.join(self.config.rootfs_path, i)):
                continue
            os.remove(os.path.join(self.config.rootfs_path, i))
        return

    def make_partner(self):
        self.logger.info(f"make_partner self.config.partner_compile_mode = {self.config.partner_compile_mode} begin ...")
        self.common.check_path(f"{self.common.temp_path}/{self.config.partner_compile_mode}")
        self.common.copy_all(f"{self.common.temp_path}/release", f"{self.common.temp_path}/{self.config.partner_compile_mode}/release")
        self.common.copy_all(f"{self.common.temp_path}/debug", f"{self.common.temp_path}/{self.config.partner_compile_mode}/debug") 
        self.run_command(f"rm -rf {self.common.temp_path}/{self.config.partner_compile_mode}/release/target_app/*")
        self.run_command(f"rm -rf {self.common.temp_path}/{self.config.partner_compile_mode}/debug/target_app/*")
        os.chdir(f"{self.common.temp_path}/{self.config.partner_compile_mode}/release/target_app")
        self.config.set_build_type("release")
        self.make_171x_package()
        subprocess.run("make -j {} {}".format(self.config.cpu_count, self.config.app_name), shell=True, check=True)
        self.common.copy(f"src/apps/{self.config.app_name}/{self.config.app_name}", 
            f"{self.common.temp_path}/{self.config.partner_compile_mode}/release/tmp_root/opt/bmc/apps/legacy/{self.config.app_name}/{self.config.app_name}")
        self.strip_bin_files(f"{self.common.temp_path}/{self.config.partner_compile_mode}/release/tmp_root")
        
        os.chdir(f"{self.common.temp_path}/{self.config.partner_compile_mode}/debug/target_app")
        self.config.set_build_type("debug")
        self.make_171x_package()
        subprocess.run("make -j {} {} ".format(self.config.cpu_count, self.config.app_name), shell=True, check=True)
        self.common.copy(f"src/apps/{self.config.app_name}/{self.config.app_name}", 
            f"{self.common.temp_path}/{self.config.partner_compile_mode}/debug/tmp_root/opt/bmc/apps/legacy/{self.config.app_name}/{self.config.app_name}")
        self.strip_bin_files(f"{self.common.temp_path}/{self.config.partner_compile_mode}/debug/tmp_root")
        self.logger.info(f"make_partner end !")

        return 

    def make_171x_package(self, sdk_select=""):
        cmake_args=""
        # 确保 cmake 库路径正确
        os.environ["CMAKE_LIBRARY_PATH"] = f"{self.common.code_path}/../open_source/lib"
        self.common.path_env_revise()

        if self.config.enable_arm_gcov:
            self.cmake_cmd += " -DGCOV=ON"

        if sdk_select == "standard" and self.config.board_version == "V5":
            self.logger.info("*************************** use V3 SDK #################################")
            os.chdir(f"{self.common.pme_dir}/platform")
            shutil.rmtree(f"{self.common.pme_dir}/sdk", ignore_errors=True)
            os.makedirs(f"{self.common.pme_dir}/sdk", exist_ok=True)
            self.common.untar_to_dir("PME_SDK.tar.gz", f"{self.common.pme_dir}/sdk")
            os.chdir(self.target_path)
            cmake_args = f"-DCMAKE_TOOLCHAIN_FILE={self.common.code_path}/arch/arm/arm-linux-gnueabi.cmake -DCMAKE_INSTALL_PREFIX={self.config.rootfs_path} {self.common.pme_dir}"
        elif sdk_select == "standard" and self.config.board_version == "V6":
            cmake_args = f"-DCMAKE_TOOLCHAIN_FILE={self.common.code_path}/arch/arm64/aarch64-linux-gnu.cmake  -DCMAKE_INSTALL_PREFIX={self.config.rootfs_path} {self.common.pme_dir}"
        else:
            self.logger.info(
                f"Current CMAKE_LIBRARY_PATH: {os.getenv('CMAKE_LIBRARY_PATH')}")
            if self.config.board_version == "V5":
                self.generate_cmake_file("arm-linux-gnueabi-ultimate.cmake")
                os.chdir(self.target_path)
            if self.common.security_enhanced_compatible_board_v1 == 1:
                self.cmake_cmd = f"{self.cmake_cmd} -DSECURITY_ENHANCED_COMPATIBLE_BOARD_V1=1"
            if self.common.security_enhanced_compatible_board_v2 == 1:
                self.cmake_cmd = f"{self.cmake_cmd} -DSECURITY_ENHANCED_COMPATIBLE_BOARD_V2=1"
            if self.common.security_enhanced_compatible_board_v3 == 1:
                self.cmake_cmd = f"{self.cmake_cmd} -DSECURITY_ENHANCED_COMPATIBLE_BOARD_V3=1"
            if self.common.security_enhanced_compatible_board_v4 == 0:
                self.cmake_cmd = f"{self.cmake_cmd} -DSECURITY_ENHANCED_COMPATIBLE_BOARD_V4=1"
            if self.common.security_enhanced_compatible_board_v5 == 1:
                self.cmake_cmd = f"{self.cmake_cmd} -DSECURITY_ENHANCED_COMPATIBLE_BOARD_V5=1"
            if self.common.board_with_ebmc_list == 1:
                self.cmake_cmd = f"{self.cmake_cmd} -DBOARD_WITH_EBMC=1"

            self.cmake_cmd = f"{self.cmake_cmd} -DPARTNER_COMPILE_VERSION={self.config.partner_compile_mode}"
            cmake_args = self.cmake_cmd
        if self.config.build_type == "release":
            cmake_args += f" -DCMAKE_BUILD_TYPE=Release"
        elif self.config.build_type == "debug":
            cmake_args += f" -DCMAKE_BUILD_TYPE=Debug"
        elif self.config.build_type == "dft_separation":
            cmake_args += f" -DCMAKE_BUILD_TYPE=Debug -DDFT_ENABLED=0"
        cmake_args += self.no_pme_build

        self.logger.info(f"cmake {cmake_args}")
        subprocess.run(f"cmake {cmake_args}", shell=True)
        return

    def make_itest_package(self):
        """
        复制 itest 代码到各 app 目录下
        """
        subprocess.run(f"sh {self.common.code_path}/itest_copy.sh 1", shell=True)
        os.environ["CMAKE_LIBRARY_PATH"] = f"{self.common.code_path}/../open_source/lib"
        self.common.path_env_revise()
        if self.config.board_version == "V5":
            subprocess.run(
                f"cmake -DCMAKE_TOOLCHAIN_FILE={self.common.code_path}/arch/arm/arm-linux-gnueabi-itest.cmake  -DCMAKE_INSTALL_PREFIX={self.config.rootfs_path} {self.common.pme_dir} {self.no_pme_build}",
                shell=True)
        elif self.config.board_version == "V6":
            subprocess.run(
                f"cmake -DCMAKE_TOOLCHAIN_FILE={self.common.code_path}/arch/arm64/aarch64-linux-gnu-itest.cmake  -DCMAKE_INSTALL_PREFIX={self.config.rootfs_path} {self.common.pme_dir} {self.no_pme_build}",
                shell=True)
        return


    # 为了便于调试， 直接生成一个 tar 包， 用以放到别的 x86 机器上跑
    def make_x86_package(self):
        # 确保 CMAKE 库路径正确
        os.environ["CMAKE_LIBRARY_PATH"] = "/usr/local/lib"
        subprocess.run(
            f"cmake -DCMAKE_TOOLCHAIN_FILE={self.common.code_path}/arch/x86/x86-linux.cmake -DCMAKE_INSTALL_PREFIX={self.config.rootfs_path} {self.common.pme_dir} {self.no_pme_build}",
            shell=True)
        return


    def make_gcov_package(self):
        os.environ["CMAKE_LIBRARY_PATH"] = f"{self.common.code_path}/../open_source/lib"
        if self.config.board_version == "V5":
            subprocess.run(
                f"cmake -DCMAKE_TOOLCHAIN_FILE={self.common.code_path}/arch/arm/arm-linux-gnueabi-gcov.cmake  -DCMAKE_INSTALL_PREFIX={self.config.rootfs_path} {self.common.pme_dir} {self.no_pme_build}",
                shell=True)
        elif self.config.board_version == "V6":
            subprocess.run(
                f"cmake -DCMAKE_TOOLCHAIN_FILE={self.common.code_path}/arch/arm64/aarch64-linux-gnu-gcov.cmake  -DCMAKE_INSTALL_PREFIX={self.config.rootfs_path} {self.common.pme_dir} {self.no_pme_build}",
                shell=True)
        return


    def make_itestgcov_package(self):
        subprocess.run(f"sh {self.common.code_path}/itest_copy.sh 1", shell=True)
        os.environ["CMAKE_LIBRARY_PATH"] = f"{self.common.code_path}/../open_source/lib"
        self.common.path_env_revise()
        if self.config.board_version == "V5":
            subprocess.run(
                f"cmake -DCMAKE_TOOLCHAIN_FILE={self.common.code_path}/arch/arm/arm-linux-gnueabi-itestgcov.cmake  -DCMAKE_INSTALL_PREFIX={self.config.rootfs_path} {self.common.pme_dir} {self.no_pme_build}",
                shell=True)
        elif self.config.board_version == "V6":
            subprocess.run(
                f"cmake -DCMAKE_TOOLCHAIN_FILE={self.common.code_path}/arch/arm64/aarch64-linux-gnu-itestgcov.cmake  -DCMAKE_INSTALL_PREFIX={self.config.rootfs_path} {self.common.pme_dir} {self.no_pme_build}",
                shell=True)
        return

    def strip_bin_files(self, strip_dir):
        usr_lib_dir = ""
        if self.config.board_version == "V5":
            usr_lib_dir = f"{strip_dir}/usr/lib/*"
        elif self.config.board_version == "V6":
            usr_lib_dir = f"{strip_dir}/usr/lib/lib*"
        lib_dir_tuple = (
            f"{strip_dir}/lib/lib*",
            usr_lib_dir,
            f"{strip_dir}/bin/*",
            f"{strip_dir}/usr/bin/*",
            f"{strip_dir}/sbin/*",
            f"{strip_dir}/usr/sbin/*",
            f"{strip_dir}/usr/local/web/modules/*",
            f"{strip_dir}/usr/local/web/bin/*",
            f"{strip_dir}/opt/bmc/apps/*",
            f"{strip_dir}/opt/pme/sbin/*",
            f"{strip_dir}/opt/pme/bin/*",
            f"{strip_dir}/opt/pme/lib/*",
            f"{strip_dir}/opt/bmc/lib/*"
        )
        for lib_dir in lib_dir_tuple:
            subprocess.run(f"{self.config.strip} -R .comment {lib_dir} > /dev/null 2>&1", shell=True)
        subprocess.run(f"find {strip_dir}/opt/bmc/apps/ -type f | xargs -I {{}} file {{}}   | grep -v text | awk -F : '{{print $1}}'| xargs -I {{}} {self.config.strip} -R .comment {{}} > /dev/null 2>&1", shell=True)

    def build(self, sdk_select):
        os.chdir(self.target_path)

        if self.config.board_version == "V5":
            arc_value_tuple = ("arm", "itest", "gcov", "itestgcov")
        elif self.config.board_version == "V6":
            arc_value_tuple = ("arm64", "itest", "gcov", "itestgcov")
        if self.config.arch == arc_value_tuple[0]:
            self.make_171x_package(sdk_select)
        elif self.config.arch == arc_value_tuple[1]:
            self.make_itest_package()
        elif self.config.arch == "":
            self.make_x86_package()
        elif self.config.arch == arc_value_tuple[2]:
            self.make_gcov_package()
        elif self.config.arch == arc_value_tuple[3]:
            self.make_itestgcov_package()
        else:
            err_msg = f"{EXCEPTION_CODE[701]}{EXCEPTION_STAGE['C']}{EXCEPTION_RESPONSE[4]}"
            self.logger.error(f"{err_msg}package arch is wrong")
            os._exit(1)

        # 编译app
        if not self.verbose:
            ret = subprocess.run("make -j {} {}".format(self.config.cpu_count, self.config.app_name), shell=True)
        else:
            ret = subprocess.run("make VERBOSE=1 -j {} {}".format(self.config.cpu_count, self.config.app_name), shell=True)
        if ret.returncode != 0:
            self.logger.error("make failed")
            sys.exit(-1)
        # 打包到work_out目录
        work_out = os.path.realpath(f"{self.config.work_out}/target_app")
        self.logger.info("package elf files to {}".format(work_out))
        shutil.rmtree(work_out, ignore_errors=True)
        subprocess.run("mkdir -p {}".format(work_out), shell=True)
        subprocess.run("find ./ -type f | xargs file | grep -E 'interpreter|dynamically' | awk -F':' '{print $1}' | xargs -i{} cp {} " + work_out + " -af", shell=True)
        # 如果是指定构建app，则退出构建
        if self.config.app_name != "":
            return
        # 对release的包进行strip
        if self.config.build_type == "release":
            self.run_command("make install/strip -j {}".format(self.config.cpu_count))
            self.rm_unused_file_in_release_package()
        return

    def clean_ver_mgm_file(self):
        f = open(f"{self.config.rootfs_path}/opt/pme/app-revision.txt", "w")
        f.close()
        # 清理 git 文件
        gitkeep_file_list = self.common.py_find_dir("\.gitkeep", f"{self.config.rootfs_path}", True)
        for gitkeep_file in gitkeep_file_list:
            os.remove(gitkeep_file)

        # 给所有脚本增加执行权限
        sh_file_list = self.common.py_find_dir(".*\.sh", f"{self.config.rootfs_path}", True)
        for sh_file in sh_file_list:
            subprocess.run(f"chmod u+x {sh_file}", shell=True)

        # 删除所有 CMake 配置文件 CMakeLists.txt
        cmake_file_list = self.common.py_find_dir("CMakeLists\.txt", f"{self.config.rootfs_path}", True)
        for cmake_file in cmake_file_list:
            os.remove(cmake_file)
        return


    def package(self):
        if (self.config.app_name != ""):
            os._exit(0)

        os.chdir(self.target_path)
        self.logger.info("-------------------> Start install in package ! <-------------------")
        if self.config.build_type != "release":
            self.run_command("make install -j {}".format(self.config.cpu_count))
        self.logger.info("-------------------> Install in package finish ! <-------------------")

        self.clean_ver_mgm_file()

        if self.config.board_version == "V5":
            arch_tuple = ("arm", "itest", "gcov", "itestgcov")
            usr_lib_work_dir = f"{self.config.rootfs_path}/usr/lib"
        elif self.config.board_version == "V6":
            arch_tuple = ("arm64", "itest", "gcov", "itestgcov")
            usr_lib_work_dir = f"{self.config.rootfs_path}/usr/lib64"

        if self.config.arch in arch_tuple:
            self.customization.app_package_conf(usr_lib_work_dir)

        self.logger.info("-------------------> Start tar tmp_root ! <-------------------")

        # 删除.comment段
        self.strip_bin_files(self.config.rootfs_path)
        os.chdir(f"{self.config.rootfs_path}")
        # 使用超级用户打包原因是某些目录不存在执行权限，无法打包，使用超级用户打包完成后属主改回
        app_tar_name = f"{self.config.work_out}/app_{self.config.build_type}.tar.gz"
        self.run_command(f"tar --format=gnu --exclude={app_tar_name} -czf {app_tar_name} ./", super_user=True)
        self.run_command(f"chown {self.config.common.non_root_ug} {self.config.work_out}/*.tar.gz", ignore_error=True, super_user=True)
        shutil.copy(f"{self.config.work_out}/app_{self.config.build_type}.tar.gz", self.common.output_path)
        self.logger.info("-------------------> Package finish ! <-------------------")
        return

    def make_package(self, build_mod=""):
        self.logger.info(f"Build Mode : {build_mod}")
        if self.config.board_version == "V5":
            arc_value_tuple = ("arm", "itest", "gcov", "itestgcov")
        elif self.config.board_version == "V6":
            arc_value_tuple = ("arm64", "itest", "gcov", "itestgcov")

        if self.config.step == "" or self.config.step == "build":
            self.build(build_mod)
            if self.config.fuzz_cov:
                self.get_cov_file()
            if self.config.app_name != "":
                return

        # 将 apps 文件夹拷贝到 /tmp/gcno_tmp/ 目录下，用于计算覆盖率
        if self.config.arch == "gcov" or self.config.arch == "itestgcov":
            self.common.tar_to_dir(f"{self.target_path}/src",
                                f"{self.config.work_out}/gcno")
            os.chdir(f"{self.common.code_path}/..")
            build_mod_dir = os.getcwd()
            subprocess.run(f"tar -zcf \"{self.config.work_out}/ibmc_src.tar.gz\" \
                                --exclude=\"{build_mod_dir}/src/user_interface/kvm_client\" \"{build_mod_dir}/src\"",
                        shell=True)
            os.chdir(self.common.code_path)

        if self.config.step == "" or self.config.step == "package":
            self.logger.info("-------------------> Start build package ! <-------------------")
            self.package()
            self.logger.info("-------------------> Build package end! <-------------------")
        # 生成后删除 itest 相关文件
        os.chdir(self.common.code_path)
        if self.config.arch == "itest" or self.config.arch == "itestgcov":
            subprocess.run(f"sh {self.common.code_path}/itest_copy.sh 2", shell=True)
        if self.config.enable_arm_gcov:
            # 收集编译目录下的gcno文件, 并打包
            subprocess.run(f"cd {self.target_path}/; find ./ -name '*.gcno' -type f -print0 | tar -zcf {self.target_path}/gcno.tar.gz --null --files-from -; cp -af {self.target_path}/gcno.tar.gz {self.config.work_out}/gcno.tar.gz", shell=True)
        return
    

    def get_cov_file(self):
        # 收集编译app的源码，并打包
        os.chdir(f"{self.common.code_path}/../src/apps/{self.config.app_name}")
        build_mod_dir = os.getcwd()
        subprocess.run(f"tar -zcf {self.config.work_out}/ibmc_src.tar.gz {build_mod_dir}/src", shell=True)
        os.chdir(self.common.code_path)
        # 收集编译目录下的gcno文件, 并打包
        subprocess.run(f"cd {self.target_path}/src/apps/{self.config.app_name}/CMakeFiles/{self.config.app_name}.dir/; find ./ -name '*.gcno' -type f -print0 | tar -zcf {self.config.work_out}/gcno.tar.gz --null --files-from -", shell=True)


    def run(self):
        if self.config.use_maca:
            # 不要删掉字符串中的空格！加在别的字符串结尾，需要空格隔开
            self.no_pme_build = " -DCMAKE_NO_PME"

        if not self.config.support_dft_separation and self.config.build_type == "dft_separation":
            return
        self.set_global()
        if self.config.partner_compile_mode:
            self.make_partner()
            return
        self.get_cs4343_inc()
        self.logger.info(f"build.py: self.config.enable_arm_gcov = {self.config.enable_arm_gcov}")

        # build 参数默认编译极限模式 ( 定义宏 ULTIMATE_ENABLED )
        if self.config.step == "build":
            self.logger.info("now build for server with build")
            if self.config.board_version == "V6":
                self.config.step = ""
            self.make_package()
        # 编译极限模式 ( 定义宏 ULTIMATE_ENABLED )
        elif self.config.step == "ultimate":
            self.config.step = ""
            self.logger.info("now build for server with ultimate")
            self.make_package()
        # 编译标准模式 ( 定义宏 ULTIMATE_ENABLED )
        elif self.config.step == "standard":
            self.config.step = ""
            self.logger.info("now build for server with standard")
            self.make_package("standard")
        # 两者都编译
        elif self.config.step == "":
            self.config.step = ""
            if self.config.board_version == "V5":
                self.logger.info("now build for server with ultimate and standard")
                self.make_package("standard")
            elif self.config.board_version == "V6":
                self.logger.info("now build for server with ultimate")
            self.make_package()
        elif self.config.step == "package":
            self.logger.info("now package")
            self.make_package()
        else:
            err_msg = f"{EXCEPTION_CODE[701]}{EXCEPTION_STAGE['C']}{EXCEPTION_RESPONSE[4]}"
            self.logger.error(f"{err_msg}main: arch is wrong")
            os._exit(1)

# 提供脚本单独调试功能，在application/build目录下运行python3 works/xxxx.py
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="build target")
    parser.add_argument("-b", "--board_name", help="Board list must in '' and separated by space, default value is 2488hv6", default="2488hv6")
    parser.add_argument("-t", "--build_type", help="build type, default value is debug, can be: debug, release, dft_separation", default="debug")
    parser.add_argument("-p", "--app_name", help="app_name", default="")
    parser.add_argument("-v", "--verbose", help="make VERBOSE=1", action="store_true")
    parser.add_argument("-c", "--fuzz_cov", help="Fuzz test coverage", action="store_true")
    parser.add_argument("-a", "--asan_test", help="Asan test for fuzz", action="store_true")
    args = parser.parse_args()
    cfg = Config(args.board_name)
    cfg.set_debug(True)
    cfg.set_build_type(args.build_type)
    cfg.set_app_name(args.app_name)
    cfg.fuzz_cov = args.fuzz_cov
    cfg.asan_test = args.asan_test
    wk = WorkBuildApp(cfg)
    wk.set_verbose(args.verbose)
    wk.run()
