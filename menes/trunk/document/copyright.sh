#!/bin/bash
grep Copyright $(svn status | grep "^M" | cut -d " " -f 7) | grep -v $(date +%Y)
