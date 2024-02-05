#!/bin/bash
# Sign the HPM package.
# Copyright © Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.

set -e

function add_len_to_hpm()
{
	file_len=$(stat -c%s $1)
	file_len_format=$(printf %08x $file_len)
	if [ 8 != $(echo $file_len_format | wc -L) ]
	then
		echo "$1 length is invalid, $file_len_format"
        exit 1
	fi
	echo -n $file_len_format >> $2
}

function check_prefix()
{
	PREFIX_PATH=$1
	file_list=$2
	#check prefix sign files
	if [ -f $PREFIX_PATH/cms.crl ];then
		echo "use prefixed cms.crl"
		cp $PREFIX_PATH/cms.crl cms.crl
	fi
	if [ -f $PREFIX_PATH/package.filelist ];then
		echo "use prefixed filelist as $file_list"
		cp $PREFIX_PATH/package.filelist $file_list
	fi
	if [ -f $PREFIX_PATH/package.filelist.cms ];then
		echo "use prefixed filelist.cms as $file_list.cms"
		cp $PREFIX_PATH/package.filelist.cms $file_list.cms
	fi
}

#转售hpm包特殊处理(包括读写和只读)
start_self_sign_for_cb()
{
	for cb_hpm_name in $(ls output/ | grep rootfs.*cb_flag_*.*hpm)
	do
		echo "start_self_sign_for_cb add SHA256-Digest for $cb_hpm_name"
		cb_hpm_prefix=$(echo "$cb_hpm_name"|cut -d'.' -f 1)
		ro_hpm_file=output/${cb_hpm_prefix}.hpm
		ro_hpm_file_list=output/${cb_hpm_prefix}.filelist
        # 获取0502编码
		pre_0502_name=$(echo "${file_list_temp}"|awk -F '_' '{print $NF}' | cut -d '.' -f 1)
		cb_prd_name=$(cat ../src/resource/board/${board}/${pre_0502_name}_cb_flag_archive.ini | grep -n CB_FLAG_TOSUPPORT_LP_NAME | awk -F '=' '{print $2}' | cut -d '-' -f 1|tr 'A-Z' 'a-z')
		sha_hpm_file_name=$(echo "${ro_hpm_file}"|sed "s/cb_flag_${board}_${pre_0502_name}/${cb_prd_name}/g")
		cat <<EOF > $ro_hpm_file_list
Manifest Version: 1.0
Create By: Huawei Technology Inc.
Name: ${sha_hpm_file_name##*/}
SHA256-Digest: `sha256sum $ro_hpm_file | awk '{print $1}'`
EOF
	done
}

j=0
for i in $(find ./ -maxdepth 1 -name "*.hpm")
do
	hpm_list[j]=${i#*./} #exclude ./
	j=$(expr $j + 1)
done

#转售包处理
start_self_sign_for_cb

if [ $1 == 1 ]
	then
	#handle outside hpm file
	for ((i=0;i<${#hpm_list[@]};i++))
	do
		file_list_temp=${hpm_list[$i]##*/}
		file_list=${file_list_temp%.*}.filelist #get file name

        if echo "$file_list_temp" | grep _cb_flag_
		then
			# 获取board名
			board=$(echo "${file_list_temp}" | cut -d '.' -f 1| sed "s/_cb_flag//g"| sed "s/_0502.*//g"|sed "s/rootfs_//g"|sed "s/rw_//g")
			# 获取0502编码
			pre_0502_name=$(echo "$file_list_temp"|awk -F '_' '{print $NF}')
			cb_prd_name=$(cat ../../src/resource/board/${board}/${pre_0502_name}_cb_flag_archive.ini | grep -n CB_FLAG_TOSUPPORT_LP_NAME | awk -F '=' '{print $2}' | cut -d '-' -f 1|tr 'A-Z' 'a-z')
			file_list_temp=$(echo "${file_list_temp}"|sed "s/cb_flag_${board}_${pre_0502_name}/${cb_prd_name}/g")
		fi

		cat <<EOF > $file_list
Manifest Version: 1.0
Create By: Huawei Technology Inc.
Name: $file_list_temp
SHA256-Digest: $(sha256sum ${hpm_list[$i]} | awk '{print $1}')
EOF
	done
fi

if [ $1 == 2 ]
	then
	for ((i=0;i<${#hpm_list[@]};i++))
	do
		echo ${hpm_list[$i]}
		hpm_temp=${hpm_list[$i]}_temp
		file_list_temp=${hpm_list[$i]##*/}
		file_list=${file_list_temp%.*}.filelist #get file name

		# 给语言包添加签名
		# 读取文件名，根据语言包名判断包的语言类型，用以定位预置秘钥，签名文件所在位置
		PACKAGE_NAME=${hpm_list[$i]%.hpm}
		PREFIX_PATH=$PACKAGE_NAME
		echo "package name: $PACKAGE_NAME"
		if [[ $PACKAGE_NAME =~ "fr" ]];then
			PREFIX_PATH={$2}/prefix_language_fr/signature/
		fi
		if [[ $PACKAGE_NAME =~ "ja" ]];then
			PREFIX_PATH={$2}/prefix_language_ja/signature/
		fi
		if [[ $PACKAGE_NAME =~ "ru" ]];then
			PREFIX_PATH={$2}/prefix_language_ru/signature/
		fi

		echo "search for prefix: $PREFIX_PATH"
		check_prefix $PREFIX_PATH $file_list

		#add file num
		echo -n $(printf %08x 3) >> $hpm_temp
		#add file list id and length
		echo -n $(printf %08x 1) >> $hpm_temp
		add_len_to_hpm $file_list $hpm_temp
		
		#add cms file id and length
		echo -n $(printf %08x 2) >> $hpm_temp
		add_len_to_hpm $file_list.cms $hpm_temp
		
		#add crl file id and length
		echo -n $(printf %08x 3) >> $hpm_temp
		add_len_to_hpm cms.crl $hpm_temp
		
		#add cms related files
		cat $file_list >> $hpm_temp
		cat $file_list.cms >> $hpm_temp
		cat cms.crl >> $hpm_temp

		cat ${hpm_list[$i]} >> $hpm_temp
		mv -f $hpm_temp ${hpm_list[$i]}
	done

	rm -rf *.filelist
	rm -rf *.crl
	rm -rf *.cms
fi
