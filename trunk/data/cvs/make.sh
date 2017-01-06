pkg:setup
pkg:configure --without-gssapi
make
pkg:install
pkg: mkdir -p /etc/profile.d
pkg: cp -a %/cvs.sh /etc/profile.d
