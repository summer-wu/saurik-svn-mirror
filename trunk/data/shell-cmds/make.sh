pkg:setup
mkdir -p "${PKG_DEST}/usr/bin"
for bin in killall renice script time which; do
    ${PKG_TARG}-gcc -O2 -mthumb -o "${bin}/${bin}" "${bin}"/*.c -D'__FBSDID(x)=' -save-temps
    cp -a "${bin}/${bin}" "${PKG_DEST}/usr/bin"
done
