
ROOT_PATH=$(shell pwd)
CONF=$(ROOT_PATH)/conf
PLUGIN=$(ROOT_PATH)/plugin
WWWROOT=$(ROOT_PATH)/wwwroot
cc=gcc
FLAGS=-D_STDOUT_i# -D_DEBUG_
LDFLAGS=-lpthread
bin=httpd
src=httpd.c main.c
obj=$(shell echo $(src) | sed 's/\.c/\.o/g')

$(bin):$(obj)
	$(cc) -o $@ $^ $(LDFLAGS)

#$(obj):$(src)
%.o:%.c
	$(cc) -c $< $(FLAGS)

.PHONY:cl
cl:
	rm *.o $(bin)

debug:
	echo $(obj)
	echo $(src)
