if [[ -z ${MENES_BUILD_SH_D_MENES_SH+@} ]]; then
MENES_BUILD_SH_D_MENES_SH=''

source "$menes/make.sh/xxc.sh"
source "$menes/make.sh/c++.sh"

hi-ccxx-flags "-I$menes/config" "-I$menes/menes" "-DMENES=\"$menes\""

if [[ -e $menes/config/cfg/menes.h ]]; then
    hi-c-flags -DCFG_HAS_CONFIG
fi

if [[ -e $menes/config/cfg/menes.hpp ]]; then
    hi-cxx-flags -DCFG_HAS_CONFIG
fi

mak:push-build
#profile '%menes%' \
mak:build "$menes/menes"

ebl:array libmenes
mak:use "${CXX_EXTENSIONS[@]}" -- libmenes.+=
mak:pop-build

fi
