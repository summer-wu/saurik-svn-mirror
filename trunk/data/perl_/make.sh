pkg:setup
env $(env | grep '^[^=]*:[^=]*=' | cut -d '=' -f 1 | while read -r line; do echo "--unset=${line}"; done) \
#sh ./Configure -des -Dusecrosscompile -Dtargethost=network.saurik.com -Dcc="${PKG_TARG}-gcc" -Dprefix=/usr
cp -a "${PKG_DATA}"/config.sh .
sh ./Configure -der -Dcc="${PKG_TARG}-gcc" -Dprefix=/usr
make
pkg:install
