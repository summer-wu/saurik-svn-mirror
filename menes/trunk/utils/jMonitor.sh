#!/bin/sh
menes=`dirname "$0"`/..

MONITOR="$1"
shift

ARGS=""
while echo -ne "$1" | grep "^-" >/dev/null; do
    ARGS="$ARGS $1"
    shift
done

java -classpath `$menes/utils/classpath.sh` -Dbcel.classloader=jMonitor.ClassLoader -Djmonitor.eventlist=$menes/$MONITOR $ARGS org.apache.bcel.util.JavaWrapper "$@"
