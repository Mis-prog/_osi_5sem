target1=server
target2=client

.PHONY:se cl clear all
all: se cl
se: server.c
	gcc server.c -o $(target1)
cl: client.c
	gcc client.c -o $(target2)
clear:
	rm serve
	rm client