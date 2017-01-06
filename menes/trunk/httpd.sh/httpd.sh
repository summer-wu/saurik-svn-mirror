#!/usr/bin/env bash
ROOT='/dat/saurik/src/menes-trunk'
uname -n | grep "^douglaspc" > /dev/null && ROOT='/cygdrive/d/Saurik.com/menes'
uname -n | grep "^daemon" > /dev/null && ROOT='/usr/home/douglas/Saurik.com/menes'

if [ "`uname`" =  'FreeBSD' ]; then
    shopt -sq expand_aliases
    alias readlink='greadlink'
    alias ls='gls'
    alias date='gdate'
fi

SERV="$ROOT/httpd.sh"
LOG="$SERV/httpd.sh.log"
LINE=`head -n 1 | sed -e "s/^\([^ ]*\) \([^? ]*\).*/\1 \2/"`
CMD=`echo "$LINE" | cut -d " " -f 1`
FILE=`echo "$LINE" | cut -d " " -f 2`
FILE=`readlink -f "$ROOT/$FILE"`
FILE=`ls -dp "$FILE" 2> /dev/null`
TEMP=''

echo -n " `date -u "+%F %R:%S"`" >> $LOG
echo -n " $LINE" >> $LOG

if [ "$CMD" = 'GET' -o "$CMD" = 'HEAD' ]; then
    if (echo "$FILE" | grep "^$ROOT/" > /dev/null) then
        STATUS='200 OK'

        if (echo "$FILE" | grep "/$" > /dev/null) then
            TEMP="$SERV/.httpd.dir"
            echo "<html><head><title>`basename "$FILE"`</title></head><body><pre>" > $TEMP

            pushd $FILE > /dev/null
            for ITEM in .. *; do
                if [ -d "$ITEM" ]; then
                    LINK="$ITEM/"
                    SUFFIX='/'
                    CLASS='directory'
                elif [ -h "$ITEM" ]; then
                    LINK="$ITEM"
                    SUFFIX='@'
                    CLASS='symlink'
                else
                    LINK="$ITEM"
                    SUFFIX=''
                    CLASS='file'
                fi

                echo "<a href=\"$LINK\" class=\"$CLASS\">$ITEM$SUFFIX</a>" >> $TEMP
            done
            popd > /dev/null

            echo "</pre><ADDRESS>Mini-Bash Server/0.9 at ??? Port ???</ADDRESS></body></html>" >> $TEMP
            FILE="$TEMP"
        fi
    else
        STATUS='404 Not Found'
        FILE="$SERV/error.html"
    fi
else
    STATUS='400 Bad Request'
    FILE="$SERV/error.html"
fi

echo " $STATUS" >> $LOG

DATE=`date -u "+%a, %d %b %Y %H:%M:%S GMT"`

echo "HTTP/1.1 $STATUS"
echo "Date: $DATE"
echo "Server: Mini-Bash/0.9"
echo "Last-Modified: `date -u -d "\`ls --full-time "$FILE" | sed -e "s/  */ /g" | cut -d " " -f 6-8 | sed -e "s/\([0-9][0-9][0-9][0-9]\)-\([0-9][0-9]\)-\([0-9][0-9]\) \([0-9][0-9]\):\([0-9][0-9]\):\([0-9][0-9]\)\.\([0-9][0-9]*\) \(.[0-9][0-9][0-9][0-9]\)/\1-\2-\3 \4:\5:\6 \8/g"\`" "+%a, %d %b %Y %H:%M:%S GMT"`"

if [ -n "$FILE" ]; then
    echo "Content-Length: `ls -l "$FILE" | sed -e "s/  */ /g" | cut -d " " -f 5`"
    # XXX: this should not be some weird file type for .cs or .java or, well, anything
    echo "Content-Type: `file -i "$FILE" | cut -d " " -f 2-`"
fi

echo ""

if [ "$CMD" = 'GET' ]; then
    if [ -n "$FILE" ]; then
        cat "$FILE"
    fi
fi
