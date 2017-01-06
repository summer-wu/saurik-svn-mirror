export CODESIGN_ALLOCATE=$(which arm-apple-darwin9-codesign_allocate)
for x in extrainst_ prerm; do
    /apl/tel/exec.sh - arm-apple-darwin9-g++ -o "$x" "$x.mm" -framework Foundation -framework CoreFoundation -lobjc
    /apl/tel/util/ldid -S "$x"
done
rm -rf syslogd
mkdir -p syslogd/{DEBIAN,etc}
cp -a syslog.conf syslogd/etc
cp -a conffiles control extrainst_ prerm syslogd/DEBIAN
dpkg-deb -b syslogd syslogd_$(grep ^Version: control | cut -d ' ' -f 2)_iphoneos-arm.deb
