pkg:setup
pkg:configure --disable-perl-regexp --bindir=/bin
make
pkg:install
