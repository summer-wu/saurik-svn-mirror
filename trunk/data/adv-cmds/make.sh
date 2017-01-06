pkg:setup
"${PKG_TARG}-gcc" -O2 -mthumb -o tabs tabs.tproj/*.c -lncurses
"${PKG_TARG}-g++" -O2 -mthumb -o locale/locale locale/*.cc
for tproj in finger fingerd last lsvfs md ps; do
    echo "${tproj}"
    "${PKG_TARG}-gcc" -O2 -mthumb -o "${tproj}" "${tproj}.tproj"/*.c -D'__FBSDID(x)='
done
pkg: mkdir -p /bin /usr/bin /usr/libexec
pkg: cp -a ps /usr/bin
pkg: cp -a finger last locale/locale lsvfs md tabs /usr/bin
pkg: cp -a fingerd /usr/libexec
ldid -S"${PKG_DATA}/ps.xml" "${PKG_DEST}/usr/bin/ps"
