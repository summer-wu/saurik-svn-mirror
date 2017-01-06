all: analyze disarm memset percent scour

%: %.c ../minimal/*.h makefile
	gcc -I .. -O3 -g3 -Wunused-value -Wall -o $@ $<
