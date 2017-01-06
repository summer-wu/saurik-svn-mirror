EXEPATH := binary
MAINS := $(MENES)/projects/net-tools/waf-server.cpp
NAMESPACES += app com dbi enc mak net net/{http,mime} vfs waf xml
TESTPATH := binary/waf-server
TESTARGS := --port=9090 --base=websites/blog
