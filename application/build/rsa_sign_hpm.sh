#!/bin/bash
# Add certificate information to the end of the hpm package.
# Copyright © Huawei Technologies Co., Ltd. 2021-2023. All rights reserved.

set -e

j=0
for i in $(find ./ -maxdepth 2 -name "*.hpm")
do
	hpm_list[j]=${i#*./} #exclude ./
	j=$(expr $j + 1)
done

for ((i=0;i<${#hpm_list[@]};i++))
do
	echo ${hpm_list[$i]}
	rsa_file=${hpm_list[$i]}.rsa #get file name
	echo $rsa_file
	
	#add rsa related files
	cat $rsa_file >> ${hpm_list[$i]}
	
	[ -e "${rsa_file}" ] && rm -rf ${rsa_file}
done

rm -rf *.rsa
