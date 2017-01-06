if [[ -z ${MENES_BUILD_SH_D_JAVA_SH+@} ]]; then
MENES_BUILD_SH_D_JAVA_SH=''

function java:compile_() {
    mak:with-depends "$1" \
        mak:execute_ "$menes/utils/javac.sh" -d "${files_root_[$1]}"
}

mak:translate '.java' '.class' '.java' java:compile_

fi
