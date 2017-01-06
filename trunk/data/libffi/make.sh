pkg:setup
mkdir ../build
cd ../build
PKG_CONF=../gcc/libffi/configure pkg:configure --with-sysroot="${PKG_ROOT}"
make
pkg:install
mv "${PKG_DEST}"/usr/lib/gcc/*/include "${PKG_DEST}"/usr
pkg: rm -rfv /usr/lib/gcc
