chmod +x *

#�Զ���ȡ��Ŀ����  ÿ���汾ֻ��Ҫ�Ľű����ļ���
project_code=`echo $0 | awk -F "_" '{printf $1}'`
project_code=`echo $project_code | awk -F "/" '{printf $2}'`
./verify.sh  $1  $2  $project_code