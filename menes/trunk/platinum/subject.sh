#!/usr/bin/env bash
set -e

./search.sh
./help.sh 491

START=$(date --iso-8601=seconds --utc)
echo -ne "" > ./scratch/subject.dat

. ./scratch/settings.sh
while !(grep ZOOL ./scratch/subject.dat > /dev/null); do
    echo -ne "T ${SESSIONKEY} " >> ./scratch/subject.dat
    date --iso-8601=seconds --utc >> ./scratch/subject.dat
    xsltproc subject.xsl ./scratch/screen.xml >> ./scratch/subject.dat
    echo >> ./scratch/subject.dat
    ./submit.sh C_POS="1704"
    . ./scratch/settings.sh
done

END=$(date --iso-8601=seconds --utc)
mv ./scratch/subject.dat subject/${START}=${END}.dat

./logoff.sh
