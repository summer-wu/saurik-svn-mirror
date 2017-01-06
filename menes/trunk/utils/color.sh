if [[ -z ${MENES_UTILS_COLOR_SH+@} ]]; then
MENES_UTILS_COLOR_SH=''

# http://www.dee.ufcg.edu.br/~rrbrandt/tools/ansi.html

C_DKBLACK=$'\e[0;30m'
C_LTBLACK=$'\e[1;30m'
C_DKRED=$'\e[0;31m'
C_LTRED=$'\e[1;31m'
C_DKGREEN=$'\e[0;32m'
C_LTGREEN=$'\e[1;32m'
C_DKBROWN=$'\e[0;33m'
C_LTBROWN=$'\e[1;33m'
C_DKBLUE=$'\e[0;34m'
C_LTBLUE=$'\e[1;34m'
C_DKPURPLE=$'\e[0;35m'
C_LTPURPLE=$'\e[1;35m'
C_DKCYAN=$'\e[0;36m'
C_LTCYAN=$'\e[1;36m'
C_DKGREY=$'\e[0;37m'
C_LTGREY=$'\e[1;37m'

C_CLEAR=$'\e[0m'

fi
