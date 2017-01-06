pkg:setup
pkg: mkdir -p /usr/bin

${PKG_TARG}-gcc -o hexdump/hexdump hexdump/{conv,display,hexdump,hexsyntax,odsyntax,parse}.c -D'__FBSDID(x)='
pkg: cp -a hexdump/hexdump /usr/bin

for bin in ctags error rpcgen unifdef; do
    ${PKG_TARG}-gcc -o "${bin}/${bin}" "${bin}"/*.c
    pkg: cp -a "${bin}/${bin}" /usr/bin
done
