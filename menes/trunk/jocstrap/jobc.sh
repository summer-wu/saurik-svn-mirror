#!/bin/bash

# Witness "The Golden Hammer". I love my anti-pattens.
# I remember when I used to use XSL/T to do this shit.

{
    cat <<EOF
function signature() {
    while [[ \$# != 0 ]]; do
        block=\$1
        shift

        while [[ \${#block} != 0 ]]; do
            type=

            while [[ 1 ]]; do
                key=\${block:0:1}
                block=\${block:1}

                case "\${key}" in
                    ( [) type="\${type}array ";;
                    ( d) type="\${type}double";;
                    ( @) type="\${type}class NSObject";;
                    ( :) type="\${type}class NSSelector";;
                    ( f) type="\${type}float";;
                    ( c) type="\${type}int8";;
                    ( s) type="\${type}int16";;
                    ( i) type="\${type}int32";;
                    ( r) type="\${type}pointer ";;
                    ( ^) type="\${type}reference ";;
                    (\*) type="\${type}string";;
                    ( {) type="\${type}struct \${block}";;
                    (\#) type="\${type}typename";;
                    ( C) type="\${type}uint8";;
                    ( S) type="\${type}uint16";;
                    ( I) type="\${type}uint32";;
                    ( v) type="\${type}void";;
                    ( *) type="\${type}?";;
                esac

                if [[ \${key} != [[r^] ]]; then
                    break
                fi
            done

            types[types_++]=\${type}

            if [[ \${key} = { ]]; then
                break
            fi
        done
    done
}

function java_array() { echo "\$(java_"\$@")[]"; }
function java_class() { echo "\$1"; }
function java_double() { echo double; }
function java_float() { echo float; }
function java_int8() { echo byte; }
function java_int16() { echo short; }
function java_int32() { echo int; }
function java_pointer() { echo "\$(java_"\$@")"; }
function java_reference() { echo "\$(java_"\$@")"; }
function java_string() { echo String; }
function java_struct() { echo "\$1"; }
function java_typename() { echo Class; }
function java_uint8() { echo byte; }
function java_uint16() { echo short; }
function java_uint32() { echo int; }
function java_ushort() { echo short; }
function java_void() { echo void; }
function java_?() { echo '?'; }

function camelize() {
    echo -n "\$1"
    shift

    while [[ \$# != 0 ]]; do
        echo -n "\${1:0:1}" | tr abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ
        echo -n "\${1:1}"
        shift
    done

    echo
}

function method() {
    names=(\${1//:/ })
    shift
    camel=\$(camelize \${names[*]})
    types=()
    types_=0
    signature "\$@"
    echo -n "    public native \$(java_\${types[0]}) \${camel}("
    for ((type = 3; type != types_; ++type)); do
        if [[ \${type} != 3 ]]; then
            echo -n ', '
        fi
        echo -n "\$(java_\${types[type]}) a\$((type - 3))"
    done
    echo -n ");"
    echo -n " // \$@"
    echo
}

function array() {
    echo "[\$2 "
}

function struct() {
    echo "{\$1 "
}
EOF

    { echo; otool -ov $1 | tr '\t' ' ' | grep '^ *\(method_\(name\|types\)\|super_class\|name\) \|<'; } | sed -e '
        s/ *method_name 0x[0-9a-f]* \(.*\)/\1/
        s/ *method_types 0x[0-9a-f]* \(.*\)/\1%/
        /.*%$/ {
            s/\(.\)[0-9][0-9]*/\1/g
            s/{/$(struct /g
            s/}/)/g
            s/=/ /g
            s/#/\\#/g
            s/\[\([0-9]*\)/$(array \1 /g
            s/]/)/g
        }
    ' | tr '\n%' ' \n' | sed -e '
        s/^/method/
    '
} | bash
