#!/bin/bash
base="$(dirname "$0")"
"${base}"/java.sh com.saurik.swill.Servlet com.saurik.swill.JavaScriptTemplate file:"$1"
