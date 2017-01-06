#!/bin/sh
tidy \
    --force-output yes \
    --wrap 0 \
    --tab-size 8 \
    --output-xml yes \
    --char-encoding latin1 \
    --numeric-entities yes \
    --add-xml-space yes \
    --error-file /dev/null \
    --show-warnings no \
    --quiet yes \
    --gnu-emacs yes \
    "$@" | xml-format -
