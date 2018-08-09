### httpd

> 测试环境：
> Ubuntu 16.04 x86-64

- export LD_LIBRARY_PATH=/home/.../httpd/sql_connect/lib/lib  //导入环境变量，指定动态库的搜索路径
- `./build`
- `cd output`
- `./httpd [local_ip] [local_port]`
- 浏览器访问： `ip:port`

