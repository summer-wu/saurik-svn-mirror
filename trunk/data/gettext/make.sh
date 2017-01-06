pkg:setup
pkg:configure --disable-java --without-libintl-prefix
make
pkg:install
pkg: rm -rf /usr/share
