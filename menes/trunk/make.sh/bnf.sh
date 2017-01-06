if [[ -z ${MENES_BUILD_SH_D_BNF_SH+@} ]]; then
MENES_BUILD_SH_D_BNF_SH=''

source "$menes/make.sh/java.sh"

function xsl:translate_() {
    declare name="${files_name_[$1]}"
    declare path="${files_root_[$1]}$name"

    echo $name $path
    exit 0

    mak:with-depends "$1" \
        mak:execute_ "$menes/menes-xxc/xxc/process.sh" >"$path"
}

function xsl:translate() {
    declare fext="$1"
    declare text="$2"
    shift

    for ((i = 0; i != $#; ++i)); do
        if [[ $i -eq 0 ]]; then
            
        fi
    done
}

xsl:translate '.bnf' '.gll' "$menes"/menes/bnf/{collapse,simplify,combine,tokenize,combine,degenerate,leftcorner,combine,degenerate}.xsl

fi
