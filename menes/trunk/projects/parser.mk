EXEPATH := binary
MAINS := $(MENES)/projects/parser/bnf-parse.cpp
NAMESPACES += app bnf xml
#TESTARGS := data/grammars/java-g2.xml test.java
#TESTARGS := data/grammars/bnf2complex.gll data/tests/seth-subset.bnf
#TESTARGS := data/grammars/msil.gll test.il /home/saurik/cs263-w05/files/boxtest.il
#TESTARGS := data/grammars/msil.gll /tmp/il-parse.xml
TESTARGS := data/grammars/java.gll data/tests/test.java
