SHELL := /bin/bash -O extglob

all: main

test: main
	./main probes/pisa.exr

DevIL-1.6.8.mk: DevIL-1.6.8-rc2.tar.gz
	tar -zxf $^
	cd DevIL-1.6.8 && ./configure && make
	touch DevIL-1.6.8.mk

-include DevIL-1.6.8.mk

ipaths := . $(shell ls -d DevIL-*/include ~/opt/openexr-1.4.0/include/OpenEXR 2>/dev/null)
lpaths := $(shell ls -d DevIL-*/src-*/src/.libs 2>/dev/null)

flags := -pthread -g3
comma := ,

#flags += -g3 -DNFINLINE -O0
flags += -O3 -funroll-loops -DNASSERT

cflags := $(flags) -Wall -Werror
cflags += $(patsubst %,-I%,$(ipaths))

lflags := $(flags) -lIlmImf
lflags += $(patsubst %,-L%,$(lpaths))
lflags += $(patsubst %,-Wl$(comma)-rpath -Wl$(comma)%,$(lpaths))

main: main.o teapot.o torus.o
	g++ $(lflags) -lILUT -lCg -lCgGL -lglut -o $@ $^ 2>&1

clean:
	rm -rf *.o main DevIL-!(*.tar.gz)

%.o: %.c makefile *.hpp */*.*pp
	gcc $(cflags) -c -o $@ $<

%.o: %.cpp makefile *.hpp */*.*pp
	set -o pipefail; g++ $(cflags) -c -o $@ $< 2>&1 | head -n 30

.PHONY: all test clean
