lo-ccxx-flags -Ui386 -Ulinux -Uunix

lo-ccxx-flags -funroll-loops -funsigned-char
lo-ccxx-flags -fmessage-length=0 -fdollars-in-identifiers
lo-ccxx-flags -ffast-math -fno-math-errno
lo-ccxx-flags -funit-at-a-time -fno-default-inline

lo-ccxx-flags -fpic
lo-ld-flags -fpic

# XXX: I don't know how to use this, and it requires better linkage.hpps
#lo-ccxx-flags -fvisibility-inlines-hidden -fvisibility=hidden

# XXX: this may be _really_ stupid, but frankly it seems more correct
#lo-ccxx-flags -fno-threadsafe-statics

# XXX: this segment faults a _lot_
#lo-ccxx-flags -frepo

lo-ccxx-flags -Wformat=2 -Wno-multichar
lo-ccxx-flags -Wreturn-type -Wimplicit
lo-ccxx-flags -Wstrict-aliasing #-Wextra
lo-ccxx-flags -Wwrite-strings -Wno-parentheses
lo-ccxx-flags -Wcast-align -Wcast-qual
lo-ccxx-flags -Wpointer-arith -Wsign-compare
lo-ccxx-flags -Wundef -Wendif-labels
lo-ccxx-flags -Wmissing-braces -Wunused-label
lo-ccxx-flags -Wswitch
#lo-ccxx-flags -Wunused-value #-Wold-style-cast

# this looked interesting, but has hdrstop problems
#lo-ccxx-flags -Wunknown-pragmas

# XXX: I really want this for C++
lo-c-flags -Wsequence-point

lo-ccxx-flags -fuse-cxa-atexit -ffor-scope
lo-ccxx-flags -fno-gnu-keywords -fno-enforce-eh-specs
lo-ccxx-flags -Wreorder -Woverloaded-virtual

if [[ ${CFG_LO_CFLAG__WL__E+@} ]]; then
    lo-ld-flags -Wl,-E
fi

# XXX: -Wpadded is interesting, but probably useless
# XXX: -Winline doesn't seem to work well
# XXX: -Wfloat-equal is anal about indirect instantiations

if [[ ${CFG_LO_CFLAG__WINIT_SELF+@} ]]; then
    lo-ccxx-flags -Winit-self
fi

if [[ ${CFG_LO_CFLAG__WNO_LONG_DOUBLE+@} ]]; then
    lo-ccxx-flags -Wno-long-double
fi

if [[ ${CFG_LO_CFLAG__PTHREAD+@} ]]; then
    lo-ccxx-flags -pthread
    lo-ld-flags -pthread
fi

if [[ -z ${CFG_SYMBOL___CXA_ATEXIT+@} ]]; then
    mak:build "$menes/menes/crt/cxa-atexit.cpp"
fi

if [[ -z ${CFG_SYMBOL___DSO_HANDLE+@} ]]; then
    mak:build "$menes/menes/crt/dso-handle.cpp"
fi

case "$CFG_CXX_VERSION" in
    (3.3.*) unset 'GCC_USE_PCH';;

    (*) if [[ ${CFG_LO_CFLAG__WINVALID_PCH+@} ]]; then
        GCC_USE_PCH=
    else
        unset 'GCC_USE_PCH'
    fi;;
esac

if [[ ${GCC_USE_PCH+@} ]]; then
    lo-ccxx-flags -Winvalid-pch
fi

#lo-ccxx-flags -march=pentium4 -msse2
#lo-ld-flags -nostdlib -lstdc++ /home/saurik/opt/glibc-2.3.3/lib/crt1.o

CFG_AR='ar'
CFG_AR_CLASS='ar'

CFG_LD="$CFG_CXX"
CFG_LD_CLASS="$CFG_CXX_CLASS"

C_OBJ_EXTENSION='.o'

CXX_DLL_EXTENSION='.so'
CXX_LIB_EXTENSION='.a'
CXX_OBJ_EXTENSION='.o'
CXX_PCH_EXTENSION='.hh.gch'
CXX_EXE_EXTENSION=''
