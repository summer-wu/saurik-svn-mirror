pkg:setup
pkg:configure --disable-largefile --with-perl=no --with-fontconfig=no --with-x=no
make
pkg:install
