pkg:setup

"${PKG_TARG}-gcc" -O2 -mthumb -o startupfiletool startupfiletool.c
"${PKG_TARG}-gcc" -O2 -mthumb -o sw_vers sw_vers.c -framework CoreFoundation

pkg: mkdir -p /usr/{,s}bin
pkg: cp -a sw_vers /usr/bin
pkg: cp -a startupfiletool /usr/sbin
