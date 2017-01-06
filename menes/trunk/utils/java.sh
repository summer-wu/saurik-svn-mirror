#!/bin/sh
menes="`dirname "$0"`/.."
java \
    -Xss1M \
    "-Xbootclasspath/p:$menes/java-bcp/xercesImpl.jar" \
    -classpath "`$menes/utils/classpath.sh`:${CLASSPATH}" \
    "$@"
