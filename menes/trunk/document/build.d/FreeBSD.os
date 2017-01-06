LOCALS += $(MENES)/menes-api/{apr,devrandom,execinfo,libuuid,mhash,proc}
LINKLIB += apr-0 aprutil-0 db-4.2 execinfo expat iconv mhash uuid
CC := g++34
CXXFLAGS += -pthread -I/usr/local/include/apache2
LDFLAGS += -pthread -L/usr/local/lib/apache2
