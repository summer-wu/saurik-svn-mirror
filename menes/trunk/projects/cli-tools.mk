EXEPATH := binary
MAINS := $(MENES)/projects/cli-tools/{codify,md5-password,net-sync,xml-format,vfs}.cpp
NAMESPACES += app com dbi enc vfs xml
TESTPATH := binary/codify
#TESTARGS := md5-hash hex-encode
TESTARGS := password des-encrypt
#TESTPATH := binary/md5-password
#TESTPATH := binary/xml-format
#TESTARGS := data/grammars/java.bnf
#TESTARGS := --decode-concise-xml data/tests/concise.xml
#TESTPATH := binary/vfs
#TESTARGS := export 98f41a3e-dfd7-11d9-82dc-000423883b9f
#TESTARGS := read ee5891ce-e240-11d9-94e2-000423883b9f
#TESTPATH := binary/net-sync
#TESTARGS := hello.txt saurik@cruiser.saurik.com:whack