#!/bin/sh
### BEGIN INIT INFO
# Provides:          katalyzer
# Required-Start:    $network $local_fs
# Required-Stop:
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: <Enter a short description of the sortware>
# Description:       <Enter a long description of the software>
#                    <...>
#                    <...>
### END INIT INFO

# Author:	2008	Tomas Kovacik 
#		2012	Andrej Kozemcak <akozemcak@gmail.com>

# PATH should only include /usr/* if it runs after the mountnfs.sh script
PATH=/sbin:/usr/sbin:/bin:/usr/bin
DESC=katalyzer             # Introduce a short description here
NAME=katalyzer             # Introduce the short server's name here
DAEMON=/usr/bin/katalyzer # Introduce the server's location here
DAEMON_ARGS="-c /etc/katalyzer/config.conf"             # Arguments to run the daemon with
SCRIPTNAME=/etc/init.d/$NAME

# Exit if the package is not installed
[ -x $DAEMON ] || exit 0

# Read configuration variable file if it is present
[ -r /etc/katalyzer/config.conf ] && . /etc/katalyzer/config.conf

#
# Function that starts the daemon/service
#
do_start()
{
	# Return
	#   0 if daemon has been started
	#   1 if daemon was already running
	#   2 if daemon could not be started
	#start-stop-daemon -b --start --quiet --pidfile $PIDFILE --exec $DAEMON --test > /dev/null \
	#	|| return 1
	start-stop-daemon -b --start --exec $DAEMON -- \
		$DAEMON_ARGS \
		|| return 2
	# Add code here, if necessary, that waits for the process to be ready
	# to handle requests from services started subsequently which depend
	# on this one.  As a last resort, sleep for some time.
}

#
# Function that stops the daemon/service
#
do_stop()
{
	# Return
	#   0 if daemon has been stopped
	#   1 if daemon was already stopped
	#   2 if daemon could not be stopped
	#   other if a failure occurred
	start-stop-daemon --stop --exec $DAEMON
	RETVAL="$?"
	return "$RETVAL"
}

case "$1" in
  start)
    [ "$VERBOSE" != no ] && echo "Starting $DESC "
    do_start
    case "$?" in
		0|1) [ "$VERBOSE" != no ] && echo "Start" ;;
		2) [ "$VERBOSE" != no ] && echo "Failed start" ;;
	esac
  ;;
  stop)
	[ "$VERBOSE" != no ] && echo "Stopping $DESC" "$NAME"
	do_stop
	case "$?" in
		0|1) [ "$VERBOSE" != no ] && echo "Stop" ;;
		2) [ "$VERBOSE" != no ] && echo "Failed stop" ;;
	esac
	;;
  status)
       status_of_proc "$DAEMON" "$NAME" && exit 0 || exit $?
       ;;
  restart|force-reload)
	#
	# If the "reload" option is implemented then remove the
	# 'force-reload' alias
	#
	echo "Restarting $DESC"
	do_stop
	case "$?" in
	  0|1)
		do_start
		case "$?" in
			0) echo "Restart" ;;
			*) echo "Failed to start" ;; # Failed to start
		esac
		;;
	  *)
	  	# Failed to stop
		echo "Failed to stop"
		;;
	esac
	;;
  *)
	#echo "Usage: $SCRIPTNAME {start|stop|restart|reload|force-reload}" >&2
	echo "Usage: $SCRIPTNAME {start|stop|status|restart|force-reload}" >&2
	exit 3
	;;
esac

:
