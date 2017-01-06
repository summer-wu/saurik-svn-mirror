all: c++0x

test: c++0x
	@/usr/bin/time ./c++0x main.cpp

c++0x: c++0x.hs
	@/usr/bin/time ghc -fth -O -o $@ $<
	@ls -la $@
