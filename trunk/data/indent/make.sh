pkg:setup
pkg:configure
cd man
make CC=gcc
cd ..
make
pkg:install
