if [[ -z $os ]]; then
    os=$(uname -s)
fi

if sed --version 2>/dev/null | grep GNU >/dev/null; then
    sed_r='sed -r'
else
    sed_r='sed -E'
fi

export sed_r

declare -a flags
declare flag=0

time=`which time 2>/dev/null`

function nostartfiles() {
    flags[flag++]=-nostartfiles
    flags[flag++]=-DCFG_NOSTARTFILES
}

function static() {
    flags[flag++]=-static
}

function sys_fork() {
    flags[flag++]=-DCFG_USE_SYS_FORK
}

function environ() {
    flags[flag++]=-DCFG_ENVIRON
}

function progname() {
    flags[flag++]=-DCFG_PROGNAME
}

case $os in
    Darwin)
        flags[flag++]=-force_cpusubtype_ALL
    ;;

    FreeBSD)
        sys_fork
        static
        nostartfiles
        environ
		progname
    ;;

    Linux)
        flags[flag++]=-DCFG_ON_LINUX
        sys_fork
        static
        nostartfiles
    ;;

    Linux-ARM)
        flags[flag++]=-DCFG_ON_LINUX
        sys_fork
        static
    ;;
esac

#flags[flag++]=-Wl,-v; flags[flag++]=-Wl,--verbose; flags[flag++]=-v
flags[flag++]=-fno-inline-functions;flags[flag++]=-DCFG_NO_FINLINE
#flags[flag++]=-DCFG_NO_INTERACTIVE

#reset

for tool in -3sh bash cat echo insmod ksh losetup lsmod mkdir mknod readlink sh sleep umount; do
    ln -sf ../3sh bin/$tool
done

if [[ -z $gcc ]]; then
    gcc=gcc
fi

if [[ -z $gxx ]]; then
    gxx=g++
fi

if [[ codes.c -nt codes ]]; then
    "$gcc" -o codes codes.c
fi

killall -9 3sh
{ [[ 3sh -nt 3sh.cpp && 3sh -nt go.sh ]] || { $time "$gxx" \
    -fno-exceptions -fno-rtti \
    -Wreturn-type \
    -Wunused \
    -funsigned-char \
    -O0 -g3 -I. \
    "${flags[@]}" \
    -o 3sh -I. ${exflags} \
    -x c lookup2.c \
    -x c++ <(. process.sh 3sh.cpp | tee 3sh.sh | bash | tee 3sh-.cpp) -lc &&
ls -la 3sh && (! which objdump >/dev/null 2>&1 || objdump -d --demangle 3sh >3sh.s) &&
cp -f 3sh 3sh-g && strip 3sh && ls -la 3sh; }; } && exec $time ./3sh "$@"
#if [[ $? -eq 11 ]]; then
#    : reset
#fi
