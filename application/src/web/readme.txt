2019/3/20 by z00356691 zhongqiu:
本文件为临时存在，主要说明iBMC新Web合入前的注意事项：
1) 当前webapp_irm目录为iRM Web的文件，如果iBMC新web合入，请联系黄元江 hwx612483进行iBMC新Web三合一，保证所有产品均打包以及功能正常;
2) iBMC新Web三合一完成后，需要对buildimg.sh以及buildimg_ex4.sh的改动如下：
    a) 删除buildimg.sh中的以下代码段：
        #iBMC新Web合入后，此处处理无意义，需要删除
        rm ${TEMP_DIR}/rtos_with_driver/appfs/web/conf/httpd-with-ssl-irm.conf
 
    b) 删除buildimg_ext4.sh中的以下代码段：
    #iBMC新Web未合入，需要临时使用屏蔽掉web文件权限校验的配置文件，iBMC新Web合入后，此处需要删除
	if [ "$BOARD_NAME" == "RM110" ];then
		cp -a ${TEMP_DIR}/rtos_with_driver/rootfs/opt/pme/web/conf/httpd-with-ssl-irm.conf ${TEMP_DIR}/rtos_with_driver/rootfs/opt/pme/web/conf/httpd-with-ssl.conf
	fi
    
	#iBMC新Web未合入，需要临时使用屏蔽掉web文件权限校验的配置文件，iBMC新Web合入后，此处处理无意义，需要删除
	rm ${TEMP_DIR}/rtos_with_driver/rootfs/opt/pme/web/conf/httpd-with-ssl-irm.conf
    
    c) 删除buildimg_ext4.sh中的以下判断语句，保留if语句内的内容：
    #iBMC新Web合入前临时判断RM110产品，iBMC新Web合入后，需要去掉RM110的产品判断以及else分支处理
    if [ "$BOARD_NAME" == "RM110" ];then
    
    else
        cp -a ${TEMP_DIR}/rtos_with_driver/appfs/web/webapp_v1/* ${TEMP_DIR}/rtos_with_driver/appfs/web/htdocs
    fi

2023/2/27 by g00804215 guanbin:
Atlas800D_Rm管理板目前不支持Web访问，仅提供redfish访问接口。所以需要关闭web功能, 新增了关闭webrest接口，保留redfish功能的nginx配置文件。
1、新增nginx_noweb.conf文件，根据nginx.conf修改，删除Webrest端口
2、新增nginx_https_server_ext_noweb.conf文件，根据nginx_https_server_ext.conf修改，删除webrest，删除Uirest
3、在V2R2_trunk\config\board\Atlas800D_RM\img.json文件中，在构建时，对nginx.conf、nginx_https_server_ext.conf两个文件进行替换