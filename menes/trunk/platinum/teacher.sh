#!/usr/bin/env bash
set -e

./search.sh
./help.sh 1053

START=$(date --iso-8601=seconds --utc)
echo -ne "" > ./scratch/teacher.dat

. ./scratch/settings.sh
while !(grep ZYLA ./scratch/teacher.dat > /dev/null); do
    echo -ne "T ${SESSIONKEY} " >> ./scratch/teacher.dat
    date --iso-8601=seconds --utc >> ./scratch/teacher.dat
    xsltproc teacher.xsl ./scratch/screen.xml >> ./scratch/teacher.dat
    echo >> ./scratch/teacher.dat
    ./submit.sh C_POS="1704"
    . ./scratch/settings.sh
done

END=$(date --iso-8601=seconds --utc)
mv ./scratch/teacher.dat teacher/${START}=${END}.dat

./logoff.sh
