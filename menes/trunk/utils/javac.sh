#!/bin/sh
menes="`dirname "$0"`/.."
# XXX: jmonitor/ should be removed from this
javac \
    -Xlint:unchecked \
    "-Xbootclasspath/p:$menes/java-bcp/xercesImpl.jar" \
    -classpath "$menes/jmonitor:`$menes/utils/classpath.sh`" \
    "$@"
