#!/bin/bash

ROOT_PATH=$(pwd)
CONF=$ROOT_PATH/conf/server.conf
BIN=$ROOT_PATH/myhttpd
proc=$(basename $0)
name=$(basename $BIN)
server_pid=''

#./server_ctl start/stop/restart/usage
function usage()
{
    printf "usage: %s [start(-s)|stop(-t)|reboot(-rs)]\n" "$proc"
}

function is_exist()
{

    server_pid=$(pidof $name)
    #if [ $? -eq 0 -a $server_pid -gt 0 ];then 
    if [ $? -eq 0 ];then 
        return 0
    else
        return 1
    fi
}

function start()
{
    if is_exist;then
        echo "server is exist,pid is : $server_pid"
    else
        ip=$(awk -F: '/^IP/{print $NF}' $CONF)
        port=$(awk -F: '/^PORT/{print $NF}' $CONF)
        echo "ip:"$ip
        echo "port:"$port
        ./$name $ip $port
        echo "start ... done"
    fi
}

function stop()
{
    if is_exist; then
        echo "pid:"$server_pid
        $(kill -9 $server_pid)
        echo "stop ... done"
    else
        echo "server not exist, not need to stop!"
    fi
}

function status()
{
    server_pid=$(pidof $name)
    if is_exist;then 
        echo "server is runing! pid: " $server_pid
    else
        echo "server is stop"
    fi
}

if [ $# -ne 1 ];then
    usage
    exit 1
fi

case $1 in
    start | -s )
        start
    ;;
    stop | -t )
        stop
    ;;
    reboot | -rs )
        stop
        start
    ;;
    status | -stat )
        status
    ;;
    * )
        usage
    ;;
esac
