# httpd
测试环境：
Ubuntu 16.04 x86-64，安装有mysql-server和mysql-client

执行下面三步：
1> export LD_LIBRARY_PATH=/home/.../httpd/sql_connect/lib/lib  //导入环境变量，指定动态库的搜索路径
2>  ./build
3>  cd output
4>  ./httpd [local_ip] [local_port]

此时服务器端已经跑起来了，在浏览器上输入你的 ip地址:端口号(例：192.168.43.130:8080)，就可以访问到主页信息了
