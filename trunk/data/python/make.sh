pkg:extract
cd *
./configure --prefix=/usr
make
cp -a Parser/pgen{,_}
cp -a python{,_}
sleep 10
pkg:patch
autoconf
SO=.dylib CXX=${PKG_TARG}-g++ pkg:configure --enable-shared --with-system-ffi --with-signal-module --disable-toolbox-glue
make clean
rm libpython2.5.a
#make Include/graminit.h Python/graminit.c CC=gcc BASECFLAGS=
#cp -a Parser/pgen pgen-host
#make clean
make python AR=${PKG_TARG}-ar
sleep 2
touch python_
make BUILDPYTHON=python_
pkg:install BUILDPYTHON=python_
pkg: rm -f /usr/bin/python{,2.5}
pkg: cp -fa python /usr/bin/python2.5
pkg: ln -s python2.5 /usr/bin/python
