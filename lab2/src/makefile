target1=server
target2=client

.PHONY: se cl clean all

all: se cl

se: server.c ../src_lib/calculation.a
	gcc -c server.c -o $(target1).o
	gcc $(target1).o -o $(target1) -L ../src_lib -l:calculation.a

cl: client.c
	gcc client.c -o $(target2)

clean:
	rm -f $(target1) $(target2) $(target1).o
