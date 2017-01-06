#!/usr/bin/env bash
menes="$(dirname "$0")"

source "$menes/make.sh/init.sh"
source "$menes/make.sh/c++.sh"
source "$menes/make.sh/menes.sh"

#hi-ccxx-flags -Omost -DNDEBUG -DNPROFILE
#hi-ccxx-flags -Omost -DNDEBUG
hi-ccxx-flags -Omost -g
# XXX: hi-ccxx-flags -Osome -g -Wall -Wuninitialized
#hi-cxx-flags -Osome -g
#hi-ccxx-flags -Osome -g -DNFINLINE
#hi-ccxx-flags -Onone -g -DNFINLINE
#hi-ccxx-flags -Onone -g -DNFINLINE -DNPROFILE
#hi-ccxx-flags -Onone

#hi-ld-flags -Omost
hi-ld-flags -Omost -g
#hi-ld-flags -Osome
#hi-ld-flags -Osome -g
#hi-ld-flags -Onone -g
#hi-ld-flags -Onone

#lo-ccxx-flags -H -ftime-report
hi-ld-flags -r"$(realpath $menes)/binary"

for tool in "$menes"/tools/*.{c,cpp}; do
    declare name="$(basename "$tool" .$(extension "$tool"))"
    c++:link "binary/$name" "$tool" -- "${libmenes[@]}"
done

mak:push-build
mak:build "$menes/jmonitor"
mak:pop-build

mak:push-build
IFS=' '
c++:gcc-flags hi-ccxx-flags $(pkg-config --cflags gconf-2.0 gnome-vfs-2.0 gtk+-2.0 libgnomeui-2.0)
c++:gcc-flags hi-ld-flags $(pkg-config --libs gconf-2.0 gnome-vfs-2.0 gtk+-2.0 libgnomeui-2.0)
c++:link "binary/ramsey" "ramsey/ramsey.cpp" -- "${libmenes[@]}"
mak:pop-build

source "$menes/make.sh/fini.sh"
