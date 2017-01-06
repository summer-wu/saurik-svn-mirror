shopt -s extglob

pkg:setup
cd pyobjc-core

$("${PKG_TARG}-gcc" -print-prog-name=cc1obj) -print-objc-runtime-info <(echo) >Modules/objc/objc-runtime-info.h

find Modules/objc/!(test) -name '*.m' -print0 | while read -d $'\0' -r m; do
    o=${m%.m}.o
    mi=${m%.m}.mi
    ${PKG_TARG}-gcc -O2 -I"$(PKG_DEST_ libxml2)"/usr/include/libxml2 -I"$(PKG_DEST_ python)"/usr/include/python2.5 -ObjC -c -o "$o" "$m" -IModules/objc -fno-common || exit 1
done || exit 1

${PKG_TARG}-gcc -O2 -dynamiclib -lpython2.5 -o _objc.dylib Modules/objc/*.o -lobjc -lffi -lxml2 -framework CoreFoundation -framework Foundation

pkg: mkdir -p /usr/lib/python2.5/lib-dynload
pkg: cp -a _objc.dylib /usr/lib/python2.5/lib-dynload
pkg: cp -a Lib/objc /usr/lib/python2.5
