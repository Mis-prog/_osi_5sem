target1=server
target2=client

.PHONY: se cl clean all build_lib

all: se cl

build_lib: src/src_lib/calculation.c
	mkdir -p bin
	gcc -c src/src_lib/calculation.c -o bin/calculation.o
	ar rcs bin/libcalculation.a bin/calculation.o

se: src/server.c
	gcc -c src/server.c -o bin/$(target1).o
	gcc bin/$(target1).o -o bin/$(target1) -L bin -lcalculation -luuid

cl: src/client.c
	gcc src/client.c -o bin/$(target2) -luuid

clean:
	rm -rf bin