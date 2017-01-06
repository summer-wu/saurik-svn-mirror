pkg:setup
pkg:autoconf
pkg:configure --enable-static
make
pkg:install
