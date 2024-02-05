#!/bin/sh

G_BAK_FLAG_FILE="/var/third_party_file_bak.flag"
G_BAK_FLAG=0
G_BAK_LOG_FILE="/data/var/log/third_party_file_bak.log"
G_BAK_PATH="/data/opt/third_party"

#初始化备份标记
init_bak_flag()
{
    if [ ! -f "$G_BAK_FLAG_FILE" ]; then
        echo 0 > "$G_BAK_FLAG_FILE"
        chown root:root $G_BAK_FLAG_FILE > /dev/null 2>&1
        chmod 600 $G_BAK_FLAG_FILE > /dev/null 2>&1
		
        G_BAK_FLAG=0
    else
        G_BAK_FLAG=$(cat "$G_BAK_FLAG_FILE")
    fi
}

update_bak_flag()
{
    echo 1 > "$G_BAK_FLAG_FILE"
    G_BAK_FLAG=1
}

#备份第三方依赖库
copy_third_party_lib()
{
    #核心文件存在时才进行备份操作
    #拷贝storelib相关文件
    if [  -f "/usr/lib/libstorelib.so" ]; then
        #storelib较小，直接删除上个版本的备份文件然后备份
        rm -rf $G_BAK_PATH/lib/storelib
        mkdir -p $G_BAK_PATH/lib/storelib
        chmod 600 $G_BAK_PATH/lib/storelib > /dev/null 2>&1
        
        cp -af /usr/lib/libstorelib.so* $G_BAK_PATH/lib/storelib > /dev/null 2>&1
        cp -af /usr/lib/libstorelibir3.so* $G_BAK_PATH/lib/storelib > /dev/null 2>&1
        cp -af /usr/lib/libstorelibit.so* $G_BAK_PATH/lib/storelib > /dev/null 2>&1
        cp -af /opt/pme/lib/libsml_lsi.so* $G_BAK_PATH/lib/storelib > /dev/null 2>&1
        
        chmod 750 $G_BAK_PATH/lib/storelib/libsml_lsi.so*
        chmod 750 $G_BAK_PATH/lib/storelib/libstorelib.so*
        chmod 750 $G_BAK_PATH/lib/storelib/libstorelibir3.so*
        chmod 750 $G_BAK_PATH/lib/storelib/libstorelibit.so*
    fi

    #拷贝storelib相关文件
    if [  -f "/usr/lib64/libstorelib.so" ]; then
        #storelib较小，直接删除上个版本的备份文件然后备份
        rm -rf $G_BAK_PATH/lib/storelib
        mkdir -p $G_BAK_PATH/lib/storelib
        chmod 600 $G_BAK_PATH/lib/storelib > /dev/null 2>&1

        cp -af /usr/lib64/libstorelib.so* $G_BAK_PATH/lib/storelib > /dev/null 2>&1
        cp -af /usr/lib64/libstorelibir3.so* $G_BAK_PATH/lib/storelib > /dev/null 2>&1
        cp -af /usr/lib64/libstorelibit.so* $G_BAK_PATH/lib/storelib > /dev/null 2>&1
        cp -af /opt/pme/lib/libsml_lsi.so* $G_BAK_PATH/lib/storelib > /dev/null 2>&1
        
        chmod 750 $G_BAK_PATH/lib/storelib/libsml_lsi.so*
        chmod 750 $G_BAK_PATH/lib/storelib/libstorelib.so*
        chmod 750 $G_BAK_PATH/lib/storelib/libstorelibir3.so*
        chmod 750 $G_BAK_PATH/lib/storelib/libstorelibit.so*
    fi
    
    #拷贝unilsw相关文件
    if [  -f "/usr/lib/libunilsw_smm.so" ]; then
        #lsw库较大，不存在或比较不一致才拷贝
        if [ ! -d $G_BAK_PATH/lib/lsw ]; then
            mkdir -p $G_BAK_PATH/lib/lsw
            chmod 600 $G_BAK_PATH/lib/lsw > /dev/null 2>&1
        fi
        
        if [ ! -f $G_BAK_PATH/lib/lsw/libunilsw_smm.so ] || [ "`diff /usr/lib/libunilsw_smm.so $G_BAK_PATH/lib/lsw/libunilsw_smm.so`" != "" ]; then
            cp -af /usr/lib/libunilsw_smm.so $G_BAK_PATH/lib/lsw > /dev/null 2>&1
        fi
        
    fi
}

#备份第三方依赖ko
copy_third_party_driver()
{
    #拷贝unilsw依赖的相关ko
    if [  -f "/lib/modules/lsw/linux-kernel-bde.ko" ]; then
        #删除上个版本的备份文件
        rm -rf $G_BAK_PATH/module/lsw
        mkdir -p $G_BAK_PATH/module/lsw
        chmod 600 $G_BAK_PATH/module/lsw > /dev/null 2>&1
        
        cp -af /lib/modules/lsw/linux-kernel-bde.ko $G_BAK_PATH/module/lsw > /dev/null 2>&1
        cp -af /lib/modules/lsw/linux-user-bde.ko $G_BAK_PATH/module/lsw > /dev/null 2>&1
    fi
}

#备份第三方文件入口
back_up_third_party_files()
{
    init_bak_flag
    
    if [ "$G_BAK_FLAG" -ne 1 ]; then
        copy_third_party_lib
        copy_third_party_driver
        
        if [ ! -f $G_BAK_LOG_FILE ]; then
            touch $G_BAK_LOG_FILE
            chown root:root $G_BAK_LOG_FILE > /dev/null 2>&1
            chmod 600 $G_BAK_LOG_FILE > /dev/null 2>&1
        fi
		
        out_t=$(date +'%Y-%m-%d %H:%M:%S')
        echo "$out_t Back up third_party files successfully" >> $G_BAK_LOG_FILE
        
        update_bak_flag
    fi
}

back_up_third_party_files
