#!/bin/sh
menes="`dirname "$0"`/.."
"$menes/utils/java.sh" antlr.Tool "$@"
