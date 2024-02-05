set $1
for i in "$@"; do
    if [ `stat -c %s $i` -gt 104857600 ]; then
        echo `date "+%Y-%m-%d %H:%M:%S"` "ERROR : The $i file is too large and needs to be deleted." > $i;
        echo `date "+%Y-%m-%d %H:%M:%S"` "ERROR : The $i file is too large and needs to be deleted." >> /var/log/pme/maintenance_log;
    fi 2> /dev/null || true
done
