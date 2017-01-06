pkg:setup
autoconf
pkg:configure --with-readline-inc=-I"$(PKG_DEST_ readline)"/usr/include/readline --with-readline-lib=-lreadline
make
pkg:install
