pkg:setup
pkg:configure
make XCBPROTO_XCBINCLUDEDIR=$(PKG_DEST_ xcb-proto)/usr/share/xcb
pkg:install
