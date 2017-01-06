kernel := /lib/modules/$(shell uname -r)/build
obj-m += s3fs.o
s3fs-objs := base64.o module.o udftime.o

-include secrets.mk

#addr := 127.0.0.1
#host := localhost
#port := 3002

addr := 207.171.181.148
#addr := 207.171.191.141
host := s3.amazonaws.com
port := 80

ifeq ($(shell id -u),0)
sudo := 
else
sudo := sudo
endif

time := $(shell which time)

mnt := /mnt/s3

all: s3fs.ko mks3fs

mks3fs: base64.c base64.h mks3fs.c platform.h s3data.h
	g++ -Wall -Werror $$(curl-config --cflags --libs) -luuid -o mks3fs mks3fs.c base64.c

s3fs.ko: base64.c base64.h ecma_167.h module.c platform.h s3data.h udfdecl.h udftime.c
	make -C "$(kernel)" M="$$(pwd)" modules

clean:
	make -C "$(kernel)" M="$$(pwd)" clean
	rm -f mks3fs *.symvers

mount:
	$(time) $(sudo) mount -t s3fs -o "id=$(id),key=$(key),host=$(addr),port=$(port)" "$(base)" "$(mnt)"

umount:
	$(sudo) umount "$(mnt)" || true

create: mks3fs
	s3clear
	./mks3fs --id="$(id)" --key="$(key)" --host="$(host)" --port="$(port)" "$(base)"

test: remove install mount umount

remount: umount remove install mount

tsts3fs: tsts3fs.c
	gcc -Wall -Werror -o $@ $^

install: s3fs.ko
	$(sudo) insmod ./s3fs.ko

remove:
	lsmod | grep s3fs >/dev/null && $(sudo) rmmod s3fs || true

.PHONY: all clean create install mount remount remove test umount
