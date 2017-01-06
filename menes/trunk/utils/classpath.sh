#!/usr/bin/env bash
menes="$(dirname "$0")/.."

if [[ $OSTYPE == 'msys' ]]; then
    CPS=';'
else
    CPS=':'
fi

echo -ne "build"

declare jar
for jar in "$menes"/java-lib/*.jar; do
    echo -ne "$CPS$jar"
done
