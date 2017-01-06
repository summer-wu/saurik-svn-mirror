#!/usr/bin/env bash
menes="$(dirname "$0")/../.."
source "$menes/utils/bashrc.sh"

insulate="$(escape.sh "$1")"

cat << EOF
#ifndef $insulate
#define $insulate

#include "cxx/platform.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

EOF
