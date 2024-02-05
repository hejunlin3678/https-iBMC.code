#! /bin/sh
set -e

PIDFILE=/var/run/sshd.pid

export PATH="${PATH:+$PATH:}/usr/sbin:/sbin"

test -x /usr/sbin/sshd || exit 0

[ -z "$SYSCONFDIR" ] && SYSCONFDIR=/etc/ssh
mkdir -p $SYSCONFDIR


case "$1" in
  start)
    echo -n "Starting ssh..."
    start-stop-daemon -S -p $PIDFILE -x /usr/sbin/sshd -- $SSHD_OPTS
    ;;
  stop)
    echo -n "Stopping ssh..."
    start-stop-daemon -K -p $PIDFILE -x /usr/sbin/sshd
    ;;
  restart)
    echo -n "Restarting ssh..."
    start-stop-daemon -K -p $PIDFILE --oknodo -x /usr/sbin/sshd
    sleep 1
    start-stop-daemon -S -p $PIDFILE -x /usr/sbin/sshd -- $SSHD_OPTS
    ;;
  reload)
    echo -n "Reloading ssh..."
    start-stop-daemon -K -p $PIDFILE -s 1 -x /usr/sbin/sshd
    ;;
  *)
    echo "Usage: /opt/pme/script/sshd_controller {start|stop|restart|reload}"
    exit 1
esac

exit 0
