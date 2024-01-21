target1=server
target2=client

.PHONY: se cl clean all build_lib

all: se cl

build_lib: src_lib/calculation.c
	mkdir -p ../bin
	gcc -c src_lib/calculation.c -o ../bin/calculation.o
	ar rcs ../bin/libcalculation.a ../bin/calculation.o

se: server.c
	gcc -c server.c -o ../bin/$(target1).o
	gcc ../bin/$(target1).o -o ../bin/$(target1) -L../bin -lcalculation -luuid

cl: client.c
	gcc client.c -o ../bin/$(target2) -luuid

clean:
	rm -rf ../bin