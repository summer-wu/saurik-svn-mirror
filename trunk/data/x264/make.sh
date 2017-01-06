pkg:setup
CC=${PKG_TARG}-gcc ./configure --enable-shared --host="${PKG_TARG}"
make
pkg:install
