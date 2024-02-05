【软件安装】
CodingPartner 下载路径：http://nettools-scm/systools/CodingPartner/CodingPartnerSetup.exe 工具主页： http://nettools/Product/ProductIndex?id=2
python        下载路径：\\hwrnd-fs\szx01\CRDU_RTOOLS_F\RD_Tools_ForTest\Python V2.7.3 (for windows)(2.7.3)

【在LINT脚本中添加一个LINT模块】
1. 新增localbuild\lint\xxx.lnt
    定义模块的lint规则：  在"lint"目录下添加"模块名.lnt"文件（例如cim模块的文件名就是cim.ini），此文件包含了此模块特有的lint选项。包括：头文件搜索路径，预定义宏等。

2. 编辑localbuild\config.ini
    配置模块的源文件列表：在"lint\config.ini"中添加一个section(可参考已有的cim模块)。

【lint命令行使用说明】
lint一个模块:     localbuild.py lint 模块名
lint一个文件：    localbuild.py lint 模块名 文件名   //文件名不包含路径

注意事项:
1. 命令行的lint依赖于CodingPartner的 C:\Program Files\CodingPartner\tools\lint\lint-nt.exe。使用前先确定CodingPartner已经安装在默认路径。

【圈复杂度命令行使用说明】
检查一个模块:     localbuild.py ccn 模块名
检查一个文件：    localbuild.py ccn 模块名 文件名   //文件名不包含路径


【SI中配置lint】
1. 打开配置界面 CodingPartner -> 工程属性。
2. 点击左边的lint -> 规则
3. 输入配置
   (1) 配置项：自定义方案入口LINT文件(×.lnt)
         输入：“d:\工程根路径\localbuild\lint\模块名.lnt”

   (1) 配置项：新增环境变量
         变量: BMC_BASE
		 值:   d:\工程根路径
		 
	注意事项：“d:\工程根路径” 和 “模块名.lnt” 仅为示例，实际配置时需按具体情况替换。
	   
	   
	   