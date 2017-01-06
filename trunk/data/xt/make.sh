pkg:setup
pkg:configure --enable-malloc0returnsnull
gcc -c -o util/makestrs-makestrs.o util/makestrs.c
gcc -o util/makestrs util/makestrs-makestrs.o
make
pkg:install
