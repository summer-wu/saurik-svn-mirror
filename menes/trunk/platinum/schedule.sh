#!/usr/bin/env bash
set -e

./search.sh
./submit.sh C_POS="1305" F_1305_1="N"
./quarter.sh F05

START=$(date --iso-8601=seconds --utc)
echo -ne "" > ./scratch/schedule.dat

. ./scratch/settings.sh
while [ "$SCREENID" != "searchforclasses.login" ]; do
    echo -ne "T ${SESSIONKEY} " >> ./scratch/schedule.dat
    date --iso-8601=seconds --utc >> ./scratch/schedule.dat
    xsltproc schedule.xsl ./scratch/screen.xml >> ./scratch/schedule.dat
    echo >> ./scratch/schedule.dat
    ./help.sh 17
    . ./scratch/settings.sh
done

END=$(date --iso-8601=seconds --utc)
mv ./scratch/schedule.dat schedule/${START}=${END}.dat

./logoff.sh
