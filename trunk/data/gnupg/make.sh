pkg:setup
pkg:configure --disable-nls
make
pkg:install
pkg: rm -f /usr/share/gnupg/{FAQ,faq.html}
