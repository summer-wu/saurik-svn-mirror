v0=s0; echo -n 0=$v0:; function f1 { echo -n 3=$v0:; typeset v0=s2; echo -n 4=$v0:; unset v0; echo -n 5=$v0:; unset v0; echo -n 6=$v0:; v0=s3; echo -n 7=$v0:; }; function f0 { echo -n 1=$v0:; typeset v0; v0=s1; echo -n 2=$v0:; f1; echo -n 8=$v0:; }; f0; echo 9=$v0
v0=s0; function f0 { typeset v0; echo v0=${v0-ok}; }; f0
function fds { echo -n '/dev/fd:'; ls -la /dev/fd/ | grep -e '->' | sed -re 's/.*[^0-9]([0-9]+) -> / \1=/' | sort -g | tr '\n' ',' | sed -e '$ s/,$/\n/'; }
{ { fds; } 3>/dev/zero; fds; } 3>/dev/null; fds
case free in (tree) echo 0:no;; (free) echo 0:ok;; esac
case free in (tr*) echo 1:no;; (fr*) echo 1:ok;; esac
case free in (tr*(e)) echo 2:no;; (fr*(e)) echo 2:ok;; esac
v0=okoo; echo -n 0=${v0%%*(o)}:; v0=kkok; echo -n 1=${v0##*(k)}:; v0=okk; echo -n 2=${v0%+(k)}:; v0=ook; echo 3=${v0#+(o)}
