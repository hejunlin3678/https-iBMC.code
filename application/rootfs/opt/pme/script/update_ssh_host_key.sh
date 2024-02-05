#!/bin/bash

# This script update ssh host key file.

# global var define
G_KEY_PATH="/etc/ssh"
G_SSH_KEYGEN_COUNT=0
G_SSH_KEYGEN_MAX=3

G_RSA_KEY_SIZE=0
G_ED25519_KEY_SIZE=0

G_RSA_KEY_VALID=1
G_ED25519_KEY_VALID=1
G_RSA_KEY_CREATE=1
G_ED25519_KEY_CREATE=1

G_UPDATE_DONE_FILE="/dev/shm/ssh_host_update_done"

G_PASSWD_FILE="/data/etc/passwd"
G_MAX_RETRY_COUNT=60
G_WAIT_TIME=3

make_new_key()
{
    # delete old key
    rm -f ${G_KEY_PATH}/ssh_host*

    # rsa key
    ssh-keygen -t rsa -f ${G_KEY_PATH}/ssh_host_rsa_key -N ""
    G_RSA_KEY_CREATE=$?

    # ed25519 key
    ssh-keygen -t ed25519 -f ${G_KEY_PATH}/ssh_host_ed25519_key -N ""
    G_ED25519_KEY_CREATE=$?
}

validate_priviate_key()
{
    ssh-keygen -yf ${G_KEY_PATH}/ssh_host_rsa_key > /dev/null  2>&1
    G_RSA_KEY_VALID=$?

    ssh-keygen -yf ${G_KEY_PATH}/ssh_host_ed25519_key > /dev/null  2>&1
    G_ED25519_KEY_VALID=$?
}

update_key()
{
    while [ $G_SSH_KEYGEN_COUNT -lt $G_SSH_KEYGEN_MAX ]
    do
        # make new key
        for i in $(seq 1 $G_MAX_RETRY_COUNT)
        do
            if [ ! -f ${G_PASSWD_FILE} ]; then
                sleep ${G_WAIT_TIME}
            else
                break
            fi
        done
        make_new_key
        if [ $G_RSA_KEY_CREATE -ne 0 ] || [ $G_ED25519_KEY_CREATE -ne 0 ]; then
            echo `date "+%Y-%m-%d %H:%M:%S"`" create key file faild: rsa_key status = "$G_RSA_KEY_CREATE", ed25519_key status = "$G_ED25519_KEY_CREATE"." >> /var/log/pme/app_debug_log_all
        fi
        validate_priviate_key
		
        if [ $G_RSA_KEY_VALID -eq 0 ] && [ $G_ED25519_KEY_VALID -eq 0 ];then
           break
        fi
		
        G_SSH_KEYGEN_COUNT=`expr $G_SSH_KEYGEN_COUNT + 1`
    done

    echo "Generate SSH key has retried" $G_SSH_KEYGEN_COUNT
}

config_security()
{
    chmod 700 /data/etc/ssh
    chmod 400 /etc/ssh/*key
    chmod 400 /etc/ssh/*.pub
}

delete_unused_key()
{
    rm -f ${G_KEY_PATH}/ssh_host_dsa_key > /dev/null
    rm -f ${G_KEY_PATH}/ssh_host_dsa_key.pub > /dev/null
    rm -f ${G_KEY_PATH}/ssh_host_ecdsa_key > /dev/null
    rm -f ${G_KEY_PATH}/ssh_host_ecdsa_key.pub > /dev/null
    rm -rf ${G_KEY_PATH}/backup > /dev/null	
}

#this is main
main()
{
    rm $G_UPDATE_DONE_FILE
    delete_unused_key
    validate_priviate_key

    if [ $G_RSA_KEY_VALID -ne 0 ] || [ $G_ED25519_KEY_VALID -ne 0 ] ;then
        update_key;
        config_security;
    fi

    if [ -f /opt/pme/script/enable_local_ssh.sh ]; then
        chmod 710 /data/etc/ssh
    fi
    
    touch $G_UPDATE_DONE_FILE
    chmod 600 $G_UPDATE_DONE_FILE
}

main

exit 0

