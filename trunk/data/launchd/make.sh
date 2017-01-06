pkg:setup
cd launchd
pkg:configure --bindir=/bin --sbindir=/sbin --sysconfdir=/etc
export SDKROOT=${PKG_ROOT}
make
pkg:install
