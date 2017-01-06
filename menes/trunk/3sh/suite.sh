typeset=typeset
binary='0 1'

{
    (typeset a=$binary; echo -n $a); echo
    ('typeset' a=$binary; echo -n $a); echo
    ($typeset a=$binary; echo -n $a); echo
} 2>/dev/null
