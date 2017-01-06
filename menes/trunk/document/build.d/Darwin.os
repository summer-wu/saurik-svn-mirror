LOCALS += $(MENES)/menes-api/{apr,mach-o}
LINKLIB := apr-0 aprutil-0
CC := g++
CXXFLAGS += -DNFINLINE -I. -I/usr/local/apr/include/apr-0
LDFLAGS += -L/usr/local/apr/lib
