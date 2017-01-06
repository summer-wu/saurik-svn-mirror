shopt -s extglob
pkg:setup

cd getconf.tproj
for gperf in *.gperf; do
    LC_ALL=C awk -f fake-gperf.awk <"${gperf}" >"$(basename "${gperf}" .gperf).c"
done
cd ..

${PKG_TARG}-gcc -o passwd passwd.tproj/!(od_passwd).c -I. -DTARGET_OS_EMBEDDED
# XXX: ${PKG_TARG}-gcc -o chpass chpass.tproj/*.c -I. -Ipwd_mkdb.tproj -Ivipw.tproj
${PKG_TARG}-gcc -o dmesg dmesg.tproj/*.c -I.
${PKG_TARG}-gcc -o sysctl sysctl.tproj/sysctl.c -I.
${PKG_TARG}-gcc -o arch arch.tproj/*.m -I. -framework CoreFoundation -framework Foundation -lobjc

cd dynamic_pager.tproj
mig -server backing_store_triggers_server.h -user /dev/null -header /dev/null backing_store_triggers.defs
mig -server /dev/null -user backing_store_alerts.h -header /dev/null backing_store_alerts.defs
mig -server default_pager_alerts_server.h -user /dev/null -header /dev/null default_pager_alerts.defs
cd ..

cp -va "${PKG_DATA}"/kextmanager* .
# XXX: kvm_mkdb shutdown
for tproj in ac accton dynamic_pager getconf getty hostinfo iostat login mkfile pwd_mkdb reboot sync update vifs vipw zdump zic; do
    cflags=

    case ${tproj} in
        (dynamic_pager) cflags="${cflags} -Idynamic_pager.tproj";;
        (kvm_mkdb) cflags="${cflags} -DBSD_KERNEL_PRIVATE";;
        (login) cflags="${cflags} -lpam -DUSE_PAM";;
        (pwd_mkdb) cflags="${cflags} -D_PW_NAME_LEN=MAXLOGNAME -D_PW_YPTOKEN=\"__YP!\"";;
        (shutdown) cflags="${cflags} -lbsm";;
    esac

    echo "${tproj}"
    ${PKG_TARG}-gcc -o "${tproj}" "${tproj}.tproj"/*.c -I. -D'__FBSDID(x)=' -DTARGET_OS_EMBEDDED -framework CoreFoundation -framework IOKit kextmanagerUser.c ${cflags}
done

chmod u+s passwd login

pkg: mkdir -p /bin /sbin /usr/bin /usr/sbin

pkg: cp -a nologin.tproj/nologin.sh /sbin/nologin
pkg: cp -a pagesize.tproj/pagesize.sh /usr/bin/pagesize
pkg: chmod 755 /sbin/nologin /usr/bin/pagesize

pkg: cp -a sync /bin
pkg: cp -a dmesg dynamic_pager reboot /sbin
pkg: ln -s reboot /sbin/halt
pkg: cp -a arch getconf getty hostinfo login passwd /usr/bin
pkg: ln -s chpass /usr/bin/chfn
pkg: ln -s chpass /usr/bin/chsh
pkg: cp -a ac accton iostat mkfile pwd_mkdb sysctl update vifs vipw zdump zic /usr/sbin
