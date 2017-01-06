#!/bin/bash
while [ 1 ]; do
    #./output.sh $(cat userdata.txt) | page.sh 8058957209@mobile.att.net
    ./output.sh $(cat userdata.txt) >output.new

    if [[ $(wc -l output.new | cut -d " " -f 1) -ne 0 ]]; then
        cat output.new | feedify.sh >feed.xml
    fi

    sleep 300
done
