#!/bin/bash

name=$1
shift

sum=$(md5sum)

if [[ -e "${name}.sum" ]] && echo "${sum}" | diff "${name}.sum" - >/dev/null; then
    for sum in "$@"; do
        if [[ "${sum}.sum" -nt "${name}.sum" ]]; then
            touch -r "${name}.sum" "${sum}.sum"
        fi
    done
else
    echo "${sum}" >"${name}.sum"
fi
