ebl:array menes_xxx_

for xxx in !(abu); do
    if [ -d "$xxx" ]; then
        menes_xxx_.+= "$xxx"
    fi
done

IFS=' '
declare menes_xxx_=" ${menes_xxx_[*]} "

function remove_() {
    menes_xxx_="${menes_xxx_/ $1 / }"
}

function subset_() {
    declare target="$1"
    shift

    mak:push-build
    for xxx in "$@"; do
        XXX="$(echo "$xxx" | tr '[:lower:]' '[:upper:]')"
        hi-ccxx-flags "-D${XXX}_EXPORTED"
    done

    for xxx in "$@"; do
        mak:build $xxx
        remove_ $xxx
    done

    c++:link -k -S "binary/$target" -- "${eisodos[@]}"
    mak:pop-build
}

function oneoff_() {
    c++:link -S "binary/menes-$1" "$1"  -- "${eisodos[@]}"
    remove_ "$1"
}

function oneoffs_() {
    for xxx in "$@"; do
        oneoff_ "$xxx"
    done
}

ebl:array eisodos
c++:link -s "binary/eisodos" abu
mak:use '' -- eisodos.+=

subset_ menes api app cse cxx etl ext ios hop opt
subset_ diapexis com szf
subset_ skiazo g3d
subset_ thuris gui1

oneoffs_ clr dbi enc exe msg net xml

IFS=' '
for xxx in $menes_xxx_; do
    mak:build "$xxx"
done
