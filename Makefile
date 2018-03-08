
ROOT_PATH=$(shell pwd)
CONF=$(ROOT_PATH)/conf
PLUGIN=$(ROOT_PATH)/plugin
WWWROOT=$(ROOT_PATH)/wwwroot
SQL=$(ROOT_PATH)/sql_connect
cc=gcc
FLAGS=-D_STDOUT_i# -D_DEBUG_
LDFLAGS=-lpthread
bin=httpd
src=httpd.c main.c
obj=$(shell echo $(src) | sed 's/\.c/\.o/g')

.PHONY:all
all:$(bin) cgi

cgi:
	cd $(WWWROOT)/cgi-bin;\
	make;\
	cp mathcgi $(ROOT_PATH);\
	cd -;cd $(SQL);\
	make;\
	cp insert_cgi $(ROOT_PATH);\
	cp select_cgi $(ROOT_PATH)

$(bin):$(obj)
	$(cc) -o $@ $^ $(LDFLAGS)

%.o:%.c
	$(cc) -c $< $(FLAGS)

.PHONY:cl
cl:
	rm -rf *.o $(bin) output;\
	cd $(WWWROOT);\
	make cl;\
	cd -;\
	cd $(SQL);\
	make cl;\
	cd -;\
	rm -f insert_cgi select_cgi rm -f mathcgi


.PHONY:output
output:
	mkdir -p output/wwwroot/cgi-bin;\
	mkdir -p output/conf;\
	cp httpd output;\
	cp mathcgi output/wwwroot/cgi-bin;\
	cp insert_cgi output/wwwroot/cgi-bin;\
	cp select_cgi output/wwwroot/cgi-bin;\
	cp wwwroot/index.html output/wwwroot;\
	cp wwwroot/imag output/wwwroot;\
	cp $(PLUGIN)/ctl_server.sh output









