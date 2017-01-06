#!/bin/sh
# XXX: grep -r should be using find or something to avoid .svn files
grep "#include \"menes/" $(find menes/$1 -name '*.cpp' -o -name '*.hpp') | sed -e 's/^[^"]*"menes\/\(...\)\/.*$/\1/g' | sort -u | grep -v "^$1$"
