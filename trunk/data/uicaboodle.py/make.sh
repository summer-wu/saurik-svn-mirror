${PKG_TARG}-gcc -o _uicaboodle.dylib "${PKG_DATA}/uicaboodle.m" -I"$(PKG_DEST_ python)"/usr/include/python2.5 -I "$(PKG_WORK_ pyobjc)"/*/pyobjc-core/Modules/objc -lpython2.5 -framework UIKit -dynamiclib -framework Foundation -lobjc -framework CoreFoundation
pkg: mkdir -p /usr/lib/python2.5/lib-dynload
pkg: cp -a _uicaboodle.dylib /usr/lib/python2.5/lib-dynload
