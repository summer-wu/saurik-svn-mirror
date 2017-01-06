#!/usr/bin/env bash
menes="$(dirname "$0")/../.."
source $menes/utils/bashrc.sh

insulate="$(escape.sh "$1")"

cat << EOF

#endif//$insulate
EOF
