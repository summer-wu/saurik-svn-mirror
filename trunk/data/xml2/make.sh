pkg:setup
make CC=${PKG_TARG}-gcc CFLAGS='-O2 -I/home/saurik/iphone/sysroot/usr/include/libxml2 -I/home/saurik/iphone/sysroot/usr/include/libxml2/libxml' LDLIBS=-lxml2
pkg:usrbin 2html 2xml csv2 html2 xml2
