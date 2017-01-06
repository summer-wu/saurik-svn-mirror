LOCALS += $(MENES)/menes-api/{devrandom,execinfo,libuuid,linux,mhash,proc}
CC := g++
CXXFLAGS += -pthread
LDFLAGS += -pthread
LINKLIB += dl mhash uuid
