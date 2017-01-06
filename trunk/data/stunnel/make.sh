pkg:setup
pkg:configure ac_cv_file___dev_urandom_=yes ac_cv_file___dev_ptmx_=yes ac_cv_file___dev_ptc_=no --with-ssl="$(PKG_DEST_ openssl)/usr"
make
y '' | pkg:install openssl="$(which openssl)"
pkg: cp -af %/stunnel3 /usr/bin
