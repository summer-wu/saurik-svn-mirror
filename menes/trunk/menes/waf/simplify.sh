#!/usr/bin/env bash
sed -e "
    s/<$/\&lt;/
    s/<\([^A-Za-z/?]\)/\&lt;\1/g
    s/\&\&$/\&amp;\&amp;/
    s/\&$/\&amp;/
    s/ \& / \&amp; /g
    s/ \&\& / \&amp;\&amp; /g
" "$@"
