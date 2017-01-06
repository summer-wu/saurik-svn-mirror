#!/usr/bin/env bash
set -e

xsltproc -o ./scratch/settings.sh settings.xsl ./scratch/screen.xml
. ./scratch/settings.sh
echo "$SESSIONKEY [$SCREENID]: $TITLETAG"
