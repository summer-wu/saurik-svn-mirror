pkg:setup
pkg:configure --with-pcap=bpf
make
mkdir -p "${PKG_DEST}/usr/lib"
pkg: mkdir -p /usr/bin
make install install-shared DESTDIR="${PKG_DEST}"
pkg: rm -f /usr/lib/libpcap.a
pkg: ln -s libpcap.A.dylib /usr/lib/libpcap.0.dylib
