#!/usr/bin/env bash
while read -r line; do
    echo "$line" | sed -e "s/^\(..\/..\)\/.... \([-+][0-9.]*\) \"\(.*\)\"\$/\$ \2 \1 \3/" | mail $1 -r "FinanceQuest <fquest@saurik.com>"
done
