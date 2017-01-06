shopt -s extglob
pkg:setup
pkg:configure
make AR="${PKG_TARG}-ar" CFLAGS='-O2 -mthumb'
pkg:install
rm -f "${PKG_DEST}"/usr/share/aptitude/!(aptitude-defaults|section-descriptions) #function_groups|function_pkgs
