os=$(uname -s)

if sed --version 2>/dev/null | grep GNU >/dev/null; then
    sed_modern='-r'
else
    sed_modern='-E'
fi

(
    echo '#line 1 "3sh.cpp"'
    sed "$sed_modern" -e '
        s/::/_/g
        s/\\_/::/g
        s/([_A-Za-z])-([_A-Za-z])/\1_\2/g
        s/\\-/-/g
    ' 3sh.cpp
) > 3sh-.cpp

cl.exe -J -Zc:forScope -Zc:wchar_t -Os -Fe3sh.exe lookup2.c 3sh-.cpp &&
ls -la 3sh #&& /usr/bin/time ./3sh #./test.3sh; echo $?
if [[ $? -eq 11 ]]; then
    : reset
fi
