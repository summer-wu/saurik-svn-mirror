#!/bin/bash
sed -e "s/\/\/.*\$//" $1 > /tmp/il-parse.xml && time binary/bnf-parse data/grammars/msil.cbnf /tmp/il-parse.xml | ./saxon - menes/clr/verify.xsl
