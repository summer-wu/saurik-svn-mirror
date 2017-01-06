#!/bin/sh
menes="`dirname $0`/.."
java -classpath "$menes/java-lib/ant-launcher.jar" org.apache.tools.ant.launch.Launcher "$@"
