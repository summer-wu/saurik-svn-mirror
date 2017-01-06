shopt -s extglob
pkg:setup

for tproj in !(natd|ping|rarpd|spray).tproj; do
    tproj=$(basename "${tproj}" .tproj)
    echo "${tproj}"
    "${PKG_TARG}-gcc" -O2 -mthumb -o "${tproj}" "${tproj}.tproj"/!(ns).c -DPRIVATE -Dether_ntohost=_old_ether_ntohost
done

echo natd
"${PKG_TARG}-gcc" -Ialias -O2 -mthumb -o natd natd.tproj/*.c alias/*.c -DPRIVATE

pkg: mkdir -p {,/usr}/{,s}bin /usr/libexec

# XXX: what /is/ ip6conf? where is ip6conf/ig/?
pkg: cp -a ip6conf /usr/bin

pkg: cp -a kdumpd /usr/libexec
pkg: cp -a ifconfig ip6fw ipfw netstat ping6 route routed rtsol slattach /sbin
pkg: cp -a arp natd ndp rtadvd traceroute traceroute6 trpt /usr/sbin

ln -s /sbin/ping6 "${PKG_DEST}"/bin
