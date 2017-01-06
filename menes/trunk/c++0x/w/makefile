all: c++0x

test: c++0x
	@/usr/bin/time ./c++0x main.cpp

c++0x: c++0x.hs makefile
	@/usr/bin/time ghc -static -fth -O -o $@ $<
	@ls -la $@
