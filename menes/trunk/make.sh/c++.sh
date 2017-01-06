if [[ -z ${MENES_BUILD_SH_D_CXX_SH+@} ]]; then
MENES_BUILD_SH_D_CXX_SH=''

function mak:debug-c++_() {
    echo "$menes/make.sh/run-$CFG_CXX_CLASS.sh" "$CFG_CXX" \
        "${hi_ccxx_flags[@]}" "${hi_cxx_flags[@]}" -- \
        "${lo_ccxx_flags[@]}" "${lo_cxx_flags[@]}"
}

function c++:compile_() {
    declare type="$1"
    shift

    declare name="${files_root_[$1]}${files_name_[$1]}"
    mak:with-depends "$1" \
        mak:execute_ "$menes/make.sh/run-$CFG_CXX_CLASS.sh" "$CFG_CXX" \
            "${hi_ccxx_flags[@]}" "${hi_cxx_flags[@]}" "-$type$name" -- \
            "${lo_ccxx_flags[@]}" "${lo_cxx_flags[@]}"
}

function mak:debug-ld_() {
    echo "$menes/make.sh/run-$CFG_LD_CLASS.sh" "$CFG_LD" \
        "${hi_ld_flags[@]}" "-$type$name" -- "${lo_ld_flags[@]}"
}

function c++:link_() {
    declare type="$1"
    shift

    declare name="${files_root_[$1]}${files_name_[$1]}"

    case $type in
        (C|S)
            mak:with-depends "$1" \
                mak:execute_ "$menes/make.sh/run-$CFG_LD_CLASS.sh" "$CFG_LD" \
                    "${hi_ld_flags[@]}" "-$type$name" -- "${lo_ld_flags[@]}"
        ;;

        (s)
            mak:with-depends "$1" \
                mak:execute_ "$menes/make.sh/run-$CFG_AR_CLASS.sh" "$CFG_AR" \
                    "${hi_ar_flags[@]}" "-$type$name" -- "${lo_ar_flags[@]}"
        ;;
    esac
}

function c++:setup-flags_() {
    declare flag="${1//-/_}"

    eval '
        '"$flag"'s_=()
        '"$flag"'=0

        function '"$1"'s() {
            declare flag
            for flag in "$@"; do
                '"$flag"'s['"$flag"'++]="$flag"
            done
        }
    '
}

function c++:setup-hi-lo-flags_() {
    c++:setup-flags_ "hi-$1"
    c++:setup-flags_ "lo-$1"
}

c++:setup-hi-lo-flags_ ccxx-flag
c++:setup-hi-lo-flags_ cxx-flag
c++:setup-hi-lo-flags_ c-flag
c++:setup-hi-lo-flags_ ld-flag
c++:setup-hi-lo-flags_ ar-flag

if [[ ${MENES_INCLUDE_PATH:+@} ]]; then
    hi-ccxx-flags $(echo ":$MENES_INCLUDE_PATH" | sed -e 's/::*/:/g; s/^:/-I/; s/\([^\\]\):/\1 -I/g; s/\\:/:/g')
fi

if [[ ${MENES_LIBRARY_PATH:+@} ]]; then
    hi-ld-flags $(echo ":$MENES_LIBRARY_PATH" | sed -e 's/::*/:/g; s/^:/-L/; s/\([^\\]\):/\1 -L/g; s/\\:/:/g')
fi


hi-ccxx-flags "-DMAK_BUILD_FOLDER=\"$MAK_BUILD_FOLDER\"" "-I$MAK_BUILD_FOLDER"
source "$menes/make.sh/build-$CFG_CXX_CLASS.sh"
CXX_EXTENSIONS=($CXX_OBJ_EXTENSION $CXX_LIB_EXTENSION $CXX_DLL_EXTENSION)

function c++:tag_() {
    linktag_[$2]="$1"
}

function c++:link() {
    declare type='C'
    declare keep=''

    OPTIND=1
    while getopts CkSs OPTKEY; do
        case "$OPTKEY" in
            (C) declare type='C';;
            (k) declare keep=@;;
            (S) declare type='S';;
            (s) declare type='s';;
        esac
    done
    shift $((OPTIND - 1))

    case $type in
        (C) declare ext="$CXX_EXE_EXTENSION";;
        (S) declare ext="$CXX_DLL_EXTENSION";;
        (s) declare ext="$CXX_LIB_EXTENSION";;
    esac

    declare target="$1$ext"
    shift

    if [[ -z ${keep:+@} ]]; then
        mak:push-build
    fi

    mak:build "$@"
    mak:cd_ '' mak:use "${CXX_EXTENSIONS[@]}" -- mak:register '' "$target" "c++:link_ $type" '' --

    if [[ -z ${keep:+@} ]]; then
        mak:pop-build
    fi
}

mak:translate '.hh' "$CXX_PCH_EXTENSION" '.hpp' 'c++:compile_ H'
mak:translate '.cpp' "$CXX_OBJ_EXTENSION" ".hpp $CXX_PCH_EXTENSION" 'c++:compile_ c'

function c++:gcc-flags() {
    declare command="$1"
    shift

    declare -a flags
    declare flag=0

    while [[ $# -ne 0 ]]; do
        case "$1" in
            (-pthread|-rpath=*) ;;
            (-[DFIiLl]*) flags[flag++]="$1";;
            (-rpath=*) flags[flag++]="-Wl";;
            (*) fatal "unknown gcc-flag: $1";;
        esac

        shift
    done

    IFS=$' \t\n'
    $command "${flags[@]}"
}

function makIignore__hpp() {
    return 0
}

fi
