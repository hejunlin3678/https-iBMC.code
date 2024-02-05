chmod +x *
####################################################################
# bash script: 用于装备测试从ESDP系统获取的数据中提取License文件
####################################################################
#把项目编码赋值
project_code=$1
SAVEFILE=$project_code"_save.txt"
DEFAULT_LICENSE_FILE="license.xml"

error_quit()
{
    exit 1
}

ok_quit()
{
    exit 0
}

############################################################################
#########################程序开始运行#####################################
############################################################################

if [ $# -lt 2 ]; then
	echo "$0 <project_code> <sourcefile> <destfile>"
	error_quit
fi

if [ ! -f $2 ]; then
	echo "$1 is not exist"
	echo "$1 is not exist" >>$SAVEFILE
	error_quit
fi

if [ ! -n "$3" ]; then
	cat $2 | awk -F "lkf\":\"" '{print $2}' | awk -F"\",\"msg" '{printf $1}' | sed 's/\\n/\r\n/g' | sed 's/\\t/\t/g' | sed 's/\\"/\"/g' > $DEFAULT_LICENSE_FILE
else
	filepath=$3
	cat $2 | awk -F "lkf\":\"" '{print $2}' | awk -F"\",\"msg" '{printf $1}' | sed 's/\\n/\r\n/g' | sed 's/\\t/\t/g' | sed 's/\\"/\"/g' > $filepath
fi

ok_quit
