EXEPATH := binary
MAINS := $(MENES)/projects/apache-module/mod_menes.so.cpp
NAMESPACES += app com dbi enc net/http xml waf
COMPILEFLAGS += -I$(shell apxs -q INCLUDEDIR) -D{AP_HAVE_DESIGNATED_INITIALIZER,APR_NOT_DONE_YET=0}
LDFLAGS += -shared

.PHONY: all install

all:

install: $(EXEPATH)/$(BINARY)
	@-apxs -ian menes $(EXEPATH)/$(BINARY)
