#!/bin/sh
LDFLAGS="-nostdlib -lpthread" make 2>&1 | grep "undefined reference" | grep -v "more undefined" | sed -e "s/.*undefined reference to \`\(.*\)'/\1/" | sort -u
