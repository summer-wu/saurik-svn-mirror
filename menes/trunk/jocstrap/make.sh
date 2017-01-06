#!/bin/bash
PATH=/apl/n42/pre/bin:$PATH /apl/tel/exec.sh jocstrap make -k "$@"
#CODESIGN_ALLOCATE=$(which arm-apple-darwin9-codesign_allocate) /apl/tel/util/ldid -S libjocstrap.jnilib libobjc-sig.dylib
