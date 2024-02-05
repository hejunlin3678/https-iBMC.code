chmod +x *
#自动获取项目编码  每个版本只需要改脚本的文件名
project_code=`echo $0 | awk -F "_" '{printf $1}'`
project_code=`echo $project_code | awk -F "/" '{printf $2}'`

#查看日志文件是否存在
if [ ! -f $SAVEFILE ] ; then
	rm -rf $SAVEFILE
fi 

./common_load.sh image_repairs.hpm $project_code

if [ $? -ne 0 ]; then
    exit 1
fi

exit 0


