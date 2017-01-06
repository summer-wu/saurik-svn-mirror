EXEPATH := binary/develop
NAMESPACES += app
LOCALS := $(MENES)/menes/prg/develop.cpp
TESTARGS := 

COMPILEFLAGS += -D GTK -D SCI_LEXER
CXXFLAGS += $(shell pkg-config --cflags gconf-2.0 gnome-vfs-2.0 gtk+-2.0 libgnomeui-2.0 vte)
LDFLAGS += $(shell pkg-config --libs gconf-2.0 gnome-vfs-2.0 gtk+-2.0 libgnomeui-2.0 vte)

CCXXFLAGS += $(shell pkg-config --cflags glib-2.0)
LDFLAGS += $(shell pkg-config --libs glib-2.0)

PACKAGES += http://aleron.dl.sourceforge.net/sourceforge/scintilla/scintilla161.tgz
EXECUTE += make:packages/scintilla/gtk
COMPILEFLAGS += -I packages/scintilla/{gtk,include,src}
LOCALS += packages/scintilla/{gtk,src}

PACKAGES += ftp://ftp.gnome.org/pub/gnome/desktop/2.8/2.8.0/sources/gnome-terminal-2.7.3.tar.gz
EXECUTE += ./configure:packages/gnome-terminal-2.7.3 make:packages/gnome-terminal-2.7.3

COMPILEFLAGS += -I packages/gnome-terminal-2.7.3/src
OBJECTS += packages/gnome-terminal-2.7.3/src/terminal-widget-vte.o

PACKAGES += ftp://ftp.gnome.org/pub/gnome/desktop/2.8/2.8.0/sources/nautilus-2.8.0.tar.gz
NAUTILUS := packages/nautilus-2.8.0
EXECUTE += ./configure:$(NAUTILUS) make:$(NAUTILUS)

COMPILEFLAGS += -I $(NAUTILUS) $(NAUTILUS)/src
OBJECTS += $(NAUTILUS)/src/file-manager/.libs/fm-tree-model.o

LINKFLAGS += -L$(NAUTILUS)/libnautilus-private/.libs -l nautilus-private
