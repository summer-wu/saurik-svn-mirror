shopt -s extglob
shopt -s nullglob

# XXX: fix set -u in a personal copy of bash and use it for debugging
#set -u

export PATH="$menes/utils:$PATH"
source "$menes"/utils/ebl-*.sh
