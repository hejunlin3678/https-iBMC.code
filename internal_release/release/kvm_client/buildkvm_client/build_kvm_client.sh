#!/bin/bash

# 输出 vconsole_client.Jar需要做以下准备：
# 在http://3ms.huawei.com/km/blogs/details/8171767?l=zh-cn获取独立KVM编译.zip包
# 根据指导步骤进行操作执行出包


# 获取当前脚本目录
current_dir=$(pwd)
base_dir=$(dirname ${current_dir})
dir_path=$base_dir/../../../
cd $dir_path
echo "base_dir---------------------"$base_dir
JRE_windows=$dir_path/vendor/bin/kvm_client/jre_windows
JRE_linux=$dir_path/vendor/bin/kvm_client/jre_linux
JRE_mac=$dir_path/vendor/bin/kvm_client/jre_mac
kvm_client_windows_file=$dir_path/bin_cmc/application/src_bin/user_interface/kvm_client/KVM/src/com/kvm
kvm_client_windows_tool=$dir_path/tools/release/kvm_client
kvm_client_mac_tool=$dir_path/tools/release/kvm_client/mac
kvm_client_linux_tool=$dir_path/tools/release/kvm_client/linux
kvm_client_Kvm_file=$dir_path/application/src/user_interface/kvm_client/KVM//src/com/kvm
kvm_client_Kvm_vm_file=$dir_path/temp/bin_cmc/application/src_bin/user_interface/kvm_client/KVM/src/com/huawei/vm

#copy vconsole_client_iMana.jar jsmoothgen-ant.jar
cp $dir_path/bin_cmc/tools/release/kvm_client/iMana_client/vconsole_client_iMana.jar $dir_path/application/src/user_interface/kvm_client/KVM/lib
cp $dir_path/temp/bin_cmc/internal_release/release/kvm_client/jsmoothgen-ant.jar $dir_path/application/src/user_interface/kvm_client/KVM/lib
cp $kvm_client_Kvm_vm_file/console/VMConsoleLib.dll $kvm_client_Kvm_file/../huawei/vm/console
cp $kvm_client_Kvm_vm_file/console/VMConsoleLib_x64.dll $kvm_client_Kvm_file/../huawei/vm/console
cp $kvm_client_windows_tool/iMana_client/KVMMouseDisPlace_iMana_x64.dll $kvm_client_Kvm_file
cp $kvm_client_windows_tool/iMana_client/KVMMouseDisPlace_iMana.dll  $kvm_client_Kvm_file
cp $kvm_client_windows_tool/iMana_client/KVMKeyboard_iMana_x64.so $kvm_client_Kvm_file
cp $kvm_client_windows_tool/iMana_client/KVMKeyboard_iMana.so $kvm_client_Kvm_file
cp $kvm_client_windows_file/KVMKeyboard_x64.so $kvm_client_Kvm_file
cp $kvm_client_windows_file/KVMMouseDisPlace.dll $kvm_client_Kvm_file
cp $kvm_client_windows_file/KVMMouseDisPlace_x64.dll $kvm_client_Kvm_file
cp $kvm_client_windows_file/KVMKeyboard.so $kvm_client_Kvm_file

cd $dir_path/application/src/user_interface/kvm_client/BuildMaterials
buildMaterials_path=$(pwd)
echo "base_dir---------------------"$buildMaterials_path

#copy FuncSwitch_on.java,VmmFuncSwitchOn.java到kvm中
cp -r $buildMaterials_path/FuncSwitch_on.java $buildMaterials_path/../KVM/src/com/kvm/FuncSwitch.java
cp -r $buildMaterials_path/VmmFuncSwitchOn.java $buildMaterials_path/../KVM/src/com/kvm/VmmFuncSwitch.java
cd $buildMaterials_path/../KVM/
target_dir=$(pwd)
# Create kvm_client dir
mkdir target
mkdir target/kvm_client
mkdir target/kvm_client/kvm_client_windows
mkdir target/kvm_client/kvm_client_ubuntu
mkdir target/kvm_client/kvm_client_mac
mkdir target/kvm_client/kvm_client_linux

# Create kvm_client_Mac
cp -r $kvm_client_mac_tool/KVM.sh $target_dir/target/kvm_client/kvm_client_mac
echo "cp kvm_client_Mac source end"
# Create kvm_client_linux
cp -r $kvm_client_linux_tool/KVM.sh $target_dir/target/kvm_client/kvm_client_linux
echo "cp kvm_client_linux source end"
# Create kvm_client_ubuntu
cp -r $kvm_client_linux_tool/KVM.sh $target_dir/target/kvm_client/kvm_client_ubuntu
echo "cp kvm_client_ubuntu source end"
# Create kvm_client_windows
cp -rf $kvm_client_windows_tool/skeletons  $target_dir/target/kvm_client/kvm_client_windows
cp -rf $kvm_client_windows_tool/ipmitool $target_dir/target/kvm_client/kvm_client_windows
cp -r $kvm_client_windows_tool/vconsole.png $target_dir/target/kvm_client/kvm_client_windows
cp -r $kvm_client_windows_tool/Jsmooth_config.jsmooth $target_dir/target/kvm_client/kvm_client_windows
echo "cp kvm_client_windows source end"
cp -r $current_dir/pom.xml $dir_path/application/src/user_interface/kvm_client/KVM
# cd kvm_client
cd $dir_path/application/src/user_interface/kvm_client
kvm_client=$dir_path/application/src/user_interface/kvm_client
# mvn maven package
mvn package
echo "maven package end"
mkdir -p $dir_path/application/build/output/packet/ToInner/KVMClient
cp -rf $kvm_client/KVM/target/*.zip $dir_path/application/build/output/packet/ToInner/KVMClient
