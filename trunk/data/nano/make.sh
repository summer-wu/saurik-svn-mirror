pkg:setup
pkg:configure --enable-all --sysconfdir=/etc
make
pkg:install
pkg: mkdir -p /etc/profile.d
pkg: cp -a %/nano.sh /etc/profile.d
pkg: cp -a ./doc/nanorc.sample /etc/nanorc
