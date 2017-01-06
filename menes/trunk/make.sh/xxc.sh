if [[ -z ${MENES_BUILD_SH_D_XXC_SH+@} ]]; then
MENES_BUILD_SH_D_XXC_SH=''

source "$menes/make.sh/java.sh"

function xxc:compile_() {
    declare name="${files_name_[$1]}"
    declare path="${files_root_[$1]}$name"

    mak:with-depends "$1" \
        mak:execute_ "$menes/xxc/xxc/process.sh" >"$path"
}

function xxc:be:antlr_() {
    assert test $# -eq 2

    mak:execute_ "$menes/utils/antlr.sh" -o "${1%/*}" "$2"
    declare result=$?

    if [[ $result -eq 0 && $2 -nt $1 ]]; then
        touch -r "$2" "$1"
    fi

    return $result
}

function xxc:antlr_() {
    declare path="${files_root_[$1]}${files_name_[$1]}"
    mak:with-depends "$1" \
        xxc:be:antlr_ "$path"
}

function makIbuild__g() {
    declare name="${files_name_[$1]}"
    declare target="${name%.g}Parser.java"
    mak:register "$MAK_BUILD_FOLDER" "$target" xxc:antlr_ '' -- "$1"
}

mak:push-build
mak:build "$menes/xxc"

ebl:array menes_xxc
mak:use '' -- menes_xxc.+=

mak:translate '.xxh' '.hpp' '' 'xxc:compile_' "${menes_xxc[@]}"
mak:translate '.xxc' '.cpp' '.xxh' 'xxc:compile_' "${menes_xxc[@]}"

mak:pop-build

fi
