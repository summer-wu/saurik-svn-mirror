pkg:setup
make CC=arm-apple-darwin9-gcc LD=arm-apple-darwin9-gcc
pkg: mkdir -p /usr/bin
pkg: cp -a pngcrush /usr/bin
