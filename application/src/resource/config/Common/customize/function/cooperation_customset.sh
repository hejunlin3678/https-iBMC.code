#!/bin/sh
if [ $# == 2 ] ; then
        source ./common.sh 
        init_common $1 $2
        cp ../conver .
        cp ../defaultserverport.ini .   
fi

DEFAULT_OPTIONS_FILE=cooperation_customset.ini
SAVE_CUSTOMIZE_FILE=save.ini

#匹配机型对应的配置项
match_machine_default_options()
{
    local i
    #先获取每行的配置项个数
    num=`cat ./function/$DEFAULT_OPTIONS_FILE | grep "$BP_ProductUniqueID" | grep -o "\^" | wc -l`
    num=$[$num + 1]
    for((i=1;i<num;i++))
    do
        buf_options[$i]=`cat ./function/$DEFAULT_OPTIONS_FILE | grep "$BP_ProductUniqueID" | cut -d '^' -f $i`
        buf_options[$i]=`remove_space "${buf_options[$i]}"`
        echo "${buf_options[$i]}" >> $SAVE_CUSTOMIZE_FILE
    done
}

check_save_file()
{
    string=`cat $SAVE_CUSTOMIZE_FILE | grep "$BP_ProductUniqueID"`
    if [ "$string" != "$BP_ProductUniqueID" ] ; then
        echo_and_save_fail "no machine type model with PUID $BP_ProductUniqueID is matched"
        error_quit
    else
        echo_and_save_success "machine type model with PUID $BP_ProductUniqueID is matched"
    fi
}

main()
{
    echo_and_save_success "the machine is customize machine, IsCustomBrand is 1"
    #1、获取白牌机ProductUniqueID
    get_BP_ProductUniqueID
    #2、先删除文件确保配置项唯一性
    rm -rf $SAVE_CUSTOMIZE_FILE
    #3、匹配机型对应的配置项
    match_machine_default_options
    #4、校验是否匹配到了正确的机型
    check_save_file
    echo_and_save_success "The default configuration item is loaded successfully"
}


main