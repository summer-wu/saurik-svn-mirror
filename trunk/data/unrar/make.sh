pkg:setup
make -f makefile.unix CXX="${PKG_TARG}-g++" all
pkg: mkdir -p /usr/bin
pkg: cp -a unrar /usr/bin
