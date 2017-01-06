# Utility Sourcing {{{
source "$menes/utils/bashrc.sh"
source "$menes/utils/assert.sh"
source "$menes/utils/color.sh"
source "$menes/utils/extension.sh"
source "$menes/utils/profile.sh"
source "$menes/utils/spawn.sh"
# }}}

# XXX: this should have a trailing /
pwd_="$PWD"
MAK_BUILD_FOLDER='build/'

# Console Logging {{{
function mak:notify_() {
    if [[ ${1:+@} && $1 != '---' && $1 != 000 ]]; then
        declare color="$C_DKRED"
    else
        declare color=''
    fi

    echo "[$color${1:----}$C_CLEAR] --:--.---: %make%: $2"
}

function mak:echoexec_() {
    echo "$@"
    "$@"
}
# }}}

function mak:clean() {
    mak:echoexec_ rm -rf -- "$pwd_/$MAK_BUILD_FOLDER"*
}

function mak:execute_() {
    if [[ ${OPT_PRINTONLY+@} ]]; then
        "$menes/utils/shescape.sh" "$@"
    else
        spawn "$@"
    fi
}

# Option Parsing {{{
OPT_TASKS=1

OPTIND=1
while getopts 'cd:j:k?nr' OPTKEY; do
    case "$OPTKEY" in
        (c) profile "%clean%" mak:clean;;
        (d) OPT_DEBUG="$OPTARG";;
        (j) OPT_TASKS="$OPTARG";;
        (k) OPT_KEEPGOING='';;
        (n) OPT_PRINTONLY='';;
        (r) OPT_RECONFIGURE='';;

        (\?)
            echo "Usage: $0 [options] [target]*"
            echo "Options:"
            echo "  -r               reconfigure environment before build"
            echo "  -c               perform mostly clean rebuild"
            echo "  -d<s:string>     output debugging information for stage <s>"
            #echo "                   <s>=debug to print list of stages"
            echo "  -j<n:integer>    perform parallel build using <n> jobs"
            echo "  -k               keep going after build failure"
            echo "  -n               print commands instead of execution"
            exit 0
        ;;
    esac
done
shift $((OPTIND - 1))
# }}}

function makIignore__cfg() {
    return 0
}

function makIignore__tag() {
    return 0
}

function makIignore__sh() {
    return 0
}

# File Storage {{{
declare -a files_root_
declare -a files_name_
declare -a files_command_
declare -a files_depends_
declare -a files_requires_
declare -a files_provides_
declare -a files_process_
declare -a files_queue_
declare -a files_prereqs_
file_=0

declare -a classes_ext_
declare -a classes_files_
class_=0
# }}}

declare -a builds_
bend_=0

level_=0
declare -a bbegin_
bbegin_[level_]=0

declare -a queue_
qbegin_=0
qend_=0

function mak:push-build() {
    bbegin_[++level_]=$bend_
}

function mak:pop-build() {
    unset 'bbegin_[level_--]'
}

function mak:cd_() {
    declare dir="$1"
    shift

    declare rooted=''

    if [[ $dir == '-r' ]]; then
        declare rooted=@
        declare dir="$1"
        shift
    fi

    declare root="$root_"
    declare sub="$sub_"

    if [[ $dir ]]; then
        if [[ -z ${rooted:+@} ]]; then
            sub_="$sub/$dir"
        elif [[ ${dir:0:1} == '/' ]]; then
            root_='/'
            sub_="${dir:1}"
        else
            root_="$root$sub/$dir/"
            sub_=''
        fi

        pushd "$dir" >/dev/null
    else
        root_=''
        sub_=''
        pushd "$pwd_" >/dev/null
    fi

    if [[ -z $sub_ ]]; then
        sub_='.'
    fi

    "$@"
    popd >/dev/null

    root_="$root"
    sub_="$sub"
}

function mak:assign_() {
    if [[ $# -eq 2 ]]; then
        declare file="${2#${2%-*([0-9])}}"
        declare file="${file:1}"

        if [[ $file -eq $file_ ]]; then
            files_name_[file_++]="$1"
            return_="$file"
            return
        elif [[ $file -lt $file_ && ${files_name_[file]} == $1 ]]; then
            return_="$file"
            return
        fi

        rm -vf -- "$2"
    else
        assert test $# -eq 1
    fi

    declare file="$file_"
    files_name_[file_++]="$1"
    mkdir -p -- "$pwd_/$MAK_BUILD_FOLDER${1%/*}"
    touch -- "$pwd_/$MAK_BUILD_FOLDER$1.id-$file"
    return_="$file"
}

function mak:assign() {
    #mak:assign_ "$1" "$pwd_/$MAK_BUILD_FOLDER$1".id-*([0-9])

    declare file="$file_"
    files_root_[file_]="${1##*(./)}"
    files_name_[file_++]="$2"
    return_="$file"
}

function mak:rebuild() {
    declare file="$1"

    declare name="${files_name_[file]}"
    declare base="${name##*/}"
    declare ext="${base##*.}"

    if [[ ${#base} -eq ${#ext} ]]; then
        declare ext=''
    fi
    
    builds_[bend_++]="$file"

    #declare root="$root_"
    #root_="$root_"

    if declare -F "makIbuild__$ext" >/dev/null; then
        mak:cd_ '' "makIbuild__$ext" "$file"
    fi

    #root_="$root"
}

function mak:build.() {
    mak:build "$@" *
}

root_=''
sub_='.'
function mak:build() {
    declare -a options
    declare option=-1

    declare force=''

    while [[ $# -ne 0 ]]; do
        if [[ ${1:0:1} == '-' ]]; then
            options[++option]="$1"

            case "${1:1:1}" in
                (f) declare force='*';;

                (-) while [[ $# -ne 1 ]]; do
                    shift
                    builds_[bend_++]="$1"
                done;;

                (*) fatal "unknown mak:build option \`$1'";;
            esac
        elif [[ ${1:0:1} == '/' ]]; then
            mak:cd_ -r / mak:build "${options[@]}" "${1:1}"
        elif [[ -d $1 ]]; then
            declare tag=''

            if [[ -e $1/build.tag ]]; then
                declare tag='-r'
            fi

            if [[ -e $1/build.sh ]]; then
                mak:cd_ $tag "$1" source "./build.sh"
            else
                mak:cd_ $tag "$1" mak:build. "${options[@]}"
            fi
        else
            declare base="${1##*/}"
            declare ext="${base##*.}"

            if [[ ${#base} -eq ${#ext} ]]; then
                declare ext=''
            fi

            declare ignore=''

            if [[ -z ${force:+@} ]]; then
                if declare -F "makIignore__$ext" >/dev/null && "makIignore__$ext"; then
                    declare ignore=@
                elif [[ -e $name.ign ]] && source "$name.ign"; then
                    declare ignore=@
                fi
            fi

            if [[ -z ${ignore:+@} ]]; then
                declare name="$sub_/$1"
                declare name="${name##*(./)}"

                #echo :3 "$root_" "$name"
                mak:assign "$root_" "$name"
                mak:rebuild "$return_"
            fi
        fi

        shift
    done
}

function mak:register() {
    #echo mak:register "$@"

    mak:assign "$1" "$2"
    declare file="$return_"
    declare command="$3"
    declare classes="$4"
    shift 4

    files_command_[file]="$command"

    declare class
    IFS=' '
    for class in $classes; do
        unset found

        declare i
        for i in "${!classes_ext_[@]}"; do
            if [[ ${classes_ext_[i]} = $class ]]; then
                classes_files_[i]="${classes_files_[i]}$file "
                declare found=''
            fi
        done

        if [[ -z ${found+@} ]]; then
            classes_ext_[class_]="$class"
            classes_files_[class_++]=" $file "
        fi
    done

    declare -a prereqs
    declare prereq=0

    while [[ $1 != '--' ]]; do
        prereqs[prereq++]="$1"
        shift
    done
    shift

    declare i
    for i in "$@"; do
        prereqs[prereq++]="${files_root_[i]}${files_name_[i]}"
    done

    IFS=$'\n'
    files_prereqs_[file]="${prereqs[*]}"

    IFS=' '
    files_depends_[file]="${files_depends_[file]- }$* "

    if mak:requires_ "$file" "$@"; then
        files_queue_[file]="$qend_"
        queue_[qend_++]="$file"
    fi

    mak:rebuild "$file"
    return_="$file"
}

function mak:requires_() {
    #echo mak:requires_ "$@"

    file="$1"
    shift

    declare result=0

    declare require
    for require in "$@"; do
        if [[ ${files_requires_[file]} == !(* $require *) ]]; then
            if [[ ${files_command_[require]+@} ]]; then
                files_requires_[file]="${files_requires_[file]- }$require "
                files_provides_[require]="${files_provides_[require]- }$file "
                declare result=1
            fi
        fi
    done

    if [[ $result -ne 0 && ${files_queue_[file]+@} ]]; then
        mak:dequeue_ "$file"
    fi

    return $result
}

function older() {
    declare file="$1"
    shift

    while [[ $# -ne 0 ]]; do
        if [[ $1 -nt $file ]]; then
            return 0
        fi

        shift
    done

    return 1
}

function mak:verify_() {
    "$@"
    declare result=$?

    if [[ $result -ne 0 ]]; then
        rekill_
        exit $result
    fi
}

# Interrupt Handling {{{
exit_=0
interrupt_=0
function mak:interrupt_() {
    if [[ $interrupt_ -eq 0 || -z ${1+@} ]]; then
        case "$interrupt_" in
            (0)
                mak:notify_ 'INT' 'cancel'
                exit_=1
                signaled_=2
            ;;

            (1)
                mak:notify_ 'INT' 'terminate'

                declare job
                IFS=$' \t\n'
                for job in $(jobs -pr); do
                    kill "$job" 2>/dev/null
                done
            ;;
        esac

        interrupt_=$((interrupt_ + 1))
    fi
}
# }}}

function mak:translate() {
    declare fext="$1"
    declare text="$2"
    declare classes="$3"
    declare command="$4"
    shift 4

    IFS=' '
    eval function "makIbuild_${fext/./_}()" '{
        declare root="$MAK_BUILD_FOLDER"
        declare name="${files_name_[$1]%.*}'"$text"'"
        declare path="$root$name"

        #echo :1 ${files_name_[$1]} $root_ $sub_ $name

        declare -a prereqs
        declare prereq=0

        if [[ -e $path && -e $path.dep ]]; then {
            declare line
            while read -r line; do
                prereqs[prereq++]="${line%$'"'"'\r'"'"'}"
            done
        } <"$path.dep"; fi

        mak:register "$root" "$name" "'"$command"'" "'"$classes"'" "${prereqs[@]}" -- "$1"
        mak:requires_ "$return_" '"$*"'
    }'
}

function mak:filter() {
    declare -a exts
    declare ext=0

    while [[ $1 != '--' ]]; do
        exts[ext++]="$1"
        shift
    done
    shift

    declare -a files
    declare file=0

    declare i
    for i in "${!files_name_[@]}"; do
        declare ext
        for ext in "${exts[@]}"; do
            if [[ ${files_name_[i]} == *$ext ]]; then
                files[file++]="$i"
            fi
        done
    done

    "$@" "${files[@]}"
}

function mak:use() {
    declare -a exts
    declare ext=0

    while [[ $1 != '--' ]]; do
        exts[ext++]="$1"
        shift
    done
    shift

    declare -a objects
    declare object=0

    declare build
    for ((build = ${bbegin_[level_]}; build != bend_; ++build)); do
        if [[ ${builds_[build]+@} ]]; then
            declare bfile="${builds_[build]}"
            declare name="${files_name_[bfile]}"

            declare ext
            for ext in "${exts[@]}"; do
                if [[ $name == *$ext ]]; then
                    objects[object++]="$bfile"
                    unset 'builds_[build]'
                fi
            done
        fi
    done

    "$@" "${objects[@]}"
}

# Debug Overrides {{{
function mak:debug-depend_() {
    declare i
    for i in "${!files_name_[@]}"; do
        echo "$i: ${files_root_[i]}:/${files_name_[i]} r(${files_requires_[i]- }) d:(${files_depends_[i]- }) p:(${files_provides_[i]})"
    done
}

function mak:debug-queue_() {
    echo ${queue_[@]}
}

function mak:debug-ood_() {
    declare i
    for i in "${!files_command_[@]}"; do
        echo "$i: ${files_root_[i]}:/${files_name_[i]}"
    done
}
# }}}

function mak:complete_() {
    declare file="$1"
    unset 'files_command_[file]'

    declare provide
    IFS=' '
    for provide in ${files_provides_[file]}; do
        declare requires=${files_requires_[provide]/ $file / }
        if [[ $requires == ' ' ]]; then
            unset 'files_requires_[provide]'
            queue_[qend_++]=$provide
        else
            files_requires_[provide]="$requires"
        fi
    done
}

function mak:dequeue_() {
    declare file="$1"
    declare queue="${files_queue_[file]}"
    unset 'files_queue_[file]'
    unset 'queue_[queue]'
}

function mak:with-depends() {
    declare -a depends
    declare depend=0

    declare file
    IFS=' '
    for file in ${files_depends_[$1]}; do
        depends[depend++]="${files_root_[file]}${files_name_[file]}"
    done

    shift

    "$@" "${depends[@]}"
}

function mak:resolve_() {
    while [[ $# -ne 0 ]]; do
        echo "${files_root_[$1]}${files_name_[$1]}"
        shift
    done
}

function mak:schedule() {
    declare -a processes

    trap 'mak:interrupt_' SIGINT

    while [[ ${#processes[@]} -ne 0 || $interrupt_ -eq 0 && $qbegin_ -ne $qend_ ]]; do
        if [[ $interrupt_ -eq 0 ]]; then
            declare job
            for ((job = ${#processes[@]}; job != OPT_TASKS && qbegin_ != qend_; ++job)); do
                while [[ $qbegin_ -ne $qend_ && -z ${queue_[qbegin_]+@} ]]; do
                    qbegin_="$((qbegin_ + 1))"
                done

                if [[ $qbegin_ -ne $qend_ ]]; then
                    declare file=${queue_[qbegin_++]}
                    declare name="${files_name_[file]}"
                    declare path="${files_root_[file]}$name"

                    declare uptodate=''

                    if [[ -e $path ]]; then
                        declare uptodate=@

                        if [[ -e $path.cmd ]]; then {
                            IFS=$' \t\n'
                            declare depends="$(mak:resolve_ ${files_depends_[file]})"

                            declare depend
                            IFS=$'\n'
                            for depend in $depends; do
                                declare line
                                read -r line

                                if [[ $line != $depend ]]; then
                                    declare uptodate=''
                                fi
                            done

                            if read -r line; then
                                declare uptodate=''
                                rm -vf -- "$path"
                            fi
                        } <"$path.cmd"; fi

                        if [[ ${uptodate:+@} && ${files_prereqs_[file]+@} ]]; then
                            declare prereq
                            IFS=$'\n'
                            for prereq in ${files_prereqs_[file]}; do
                                #echo :3 "$prereq" -nt "$path"
                                if [[ $prereq -nt $path ]]; then
                                    declare uptodate=''
                                    break
                                fi
                            done
                        fi
                    fi

                    if [[ ${uptodate:+@} ]]; then
                        mak:complete_ "$file"
                        job=$((job - 1))
                    else
                        #echo :6 "$path"
                        declare relative="./$path"
                        mkdir -p -- "${relative%/*}"

                        IFS=$' \t\n'
                        mak:verify_ profile "$name" ${files_command_[file]} $file &
                        declare pid=$!

                        files_process_[file]=$pid
                        processes[pid]=$file
                    fi
                fi
            done
        fi

        sleep 0.05
        if [[ $? -eq 130 ]]; then
            mak:interrupt_
        fi

        IFS=$' \t\n'
        declare -a jobs=( $(jobs -pr) )

        declare pid
        for pid in "${!processes[@]}"; do
            unset 'found'

            if [[ ${#jobs[@]} -ne 0 ]]; then
                declare job
                for job in "${jobs[@]}"; do
                    if [[ $pid -eq $job ]]; then
                        declare found=
                    fi
                done
            fi

            if [[ -z ${found+@} ]]; then
                declare file=${processes[pid]}
                declare path="${files_root_[file]}${files_name_[file]}"

                wait "$pid" 2>/dev/null
                if [[ $? -eq 0 ]]; then
                    if [[ -z ${OPT_PRINTONLY+@} ]]; then
                        assert test -e "$path"

                        declare depend
                        IFS=$' \t\n'
                        for depend in ${files_depends_[file]}; do
                            echo "${files_root_[depend]}${files_name_[depend]}"
                        done >"$path.cmd"
                    fi

                    mak:complete_ "$file"
                else
                    rm -vf -- "$path"

                    if [[ -z ${OPT_KEEPGOING+@} && $interrupt_ -eq 0 ]]; then
                        interrupt_=1
                    fi

                    if [[ $exit_ -eq 0 ]]; then
                        mak:notify_ '  1' 'build failure'
                        exit_=1
                    fi
                fi

                declare file=${processes[pid]}
                unset 'processes[pid]'
                unset 'files_process_[file]'
            fi
        done
    done

    trap SIGINT

    return "$exit_"
}

# Load Configuration {{{
IFS=$'\n'
if [[ ${OPT_RECONFIGURE+@} ]] ||
    older "$menes/config/cfg/menes.sh" \
        "$menes/configure" \
        "$menes/config-sh.d"/* \
        $(find "$menes/menes" -name build.cfg)
then
    "$menes/configure"
fi

source "$menes/config/cfg/menes.sh"

for d in "$menes"/make.sh/*.d; do
    for sh in "$d"/*.sh; do
        source "$sh"
    done
done

function cfg:get() {
    declare -p "CFG_$(escape.sh "$1")" | cut -d '=' -f 2 | sed -e 's/^"//; s/"$//'
}
# }}}
