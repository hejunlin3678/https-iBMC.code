chmod +x *
#�Զ���ȡ��Ŀ����  ÿ���汾ֻ��Ҫ�Ľű����ļ���
project_code=`echo $0 | awk -F "_" '{printf $1}'`
project_code=`echo $project_code | awk -F "/" '{printf $2}'`

./load.sh  $project_code
if [ $? -ne 0 ]; then
	exit 1
fi

exit 0