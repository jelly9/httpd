#!/bin/bash

ROOT_PATH=$(pwd)
CONF=$ROOT_PATH/conf


#./server_ctl start/stop/restart/usage

proc=$(basename $0)
function usage()
{
    printf "%s [start(-s)|stop(-t)|reboot(-rs)\n" "$proc"
}

function start()
{

}

function stop()
{

}

if [ $# -ne 1 ];then
    usage
    exit 1
fi
fi

case $1 in
    start | -s )
        start`:w
        ;;
    stop | -t )
        stop
        ;;
    reboot | -rs )
        stop
        start
        ;;
    * )
        ;;
esac
