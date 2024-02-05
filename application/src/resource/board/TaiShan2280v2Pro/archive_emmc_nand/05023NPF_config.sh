chmod +x *
#自动获取项目编码  每个版本只需要改脚本的文件名
project_code=`echo $0 | awk -F "_" '{printf $1}'`
project_code=`echo $project_code | awk -F "/" '{printf $2}'`
./config.sh  $1  $2  $project_code