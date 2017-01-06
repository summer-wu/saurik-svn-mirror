SHELL := /bin/bash -O extglob

cflags := -Wall -Werror
ldflags := -lffi

hcflags += $(cflags) $(shell apr-1-config --includes)
hldflags += $(ldflags) $(shell apr-1-config --link-ld)

frameworks += CoreFoundation
frameworks += Foundation

cobjflags :=
cobjflags += -fobjc-exceptions

ifeq ($(PKG_ARCH),darwin-arm)
    target := $${PKG_TARG}-
    tcflags += $(cflags) -I$${PKG_ROOT}/usr/include/apr-1
    tldflags += $(ldflags) -lapr-1
    frameworks += LayerKit
    frameworks += WebCore
    frameworks += WebKit
    frameworks += UIKit
    frameworks += Celestial
    frameworks += Message
    frameworks += BluetoothManager
    frameworks += MessageUI
    frameworks += AddressBookUI
    frameworks += CoreTelephony
    frameworks += MusicLibrary
    main += HelloJava
else
ifeq ($(PKG_ARCH),iphoneos-arm)
    target := $${PKG_TARG}-
    tcflags += $(cflags) -I$${PKG_ROOT}/usr/include/apr-1
    tldflags += $(ldflags) -lapr-1
    frameworks += MediaToolbox
    frameworks += QuartzCore
    frameworks += WebCore
    frameworks += WebKit
    frameworks += UIKit
    frameworks += Celestial
    frameworks += Message
    frameworks += MessageUI
    frameworks += AddressBookUI
    frameworks += CoreTelephony
    frameworks += MusicLibrary
    frameworks += CoreLocation
    frameworks += AVFoundation
    main += HelloJava
else
    target :=
    tcflags := $(hcflags)
    tldflags := $(hldflags)
    #frameworks += AppKit
    #frameworks += WebKit
    main += HelloDesktop
    cobjflags += -fno-nil-receivers
endif
endif

javah := /System/Library/Frameworks/JavaVM.framework/Versions/1.5.0/Headers
javac := javac -Xlint -J-Xmx1024M -source 1.5 -target 1.5 -cp jocstrap.jar:$$(PKG_DEST_ javasqlite)/usr/share/java/sqlite.jar:.

object := $(patsubst %.m,%.o,$(wildcard source/joc/*.m))
signature := $(patsubst %.c,%.o,$(wildcard source/sig/*.c))
headers := ../minimal/*.h source/joc/*.h source/sig/*.h

all: jocstrap.jar libjocstrap.jnilib $(main).class

test: all
	java -cp jocstrap.jar:. $(main) Argument1 Argument2

test-jam: all
	jamvm -cp jocstrap.jar:. $(main) Argument1 Argument2

backup:
	rlwrap sqlite3 backup.db

sql:
	rlwrap sqlite3 joc.db

tools: joc-import joc-export libjocstrap.jnilib

clean:
	rm -rf joc.db* build/* source/{{hdr,obc,osx}/*,*/*.o} jocstrap.jar libjocstrap.jnilib $(main).class *.sum joc-{im,ex}port

signature.sum::
	cat source/sig/*.[ch] | ./md5sum.sh signature

joc-import.cpp.sum::
	cat joc-import.cpp | ./md5sum.sh joc-import.cpp

joc-import.sum:: joc-import.cpp.sum
	$(MAKE) joc-import
	cat joc-import | ./md5sum.sh joc-import joc-import.cpp

import.sum::
	{ cat joc-repair.sql; echo $(frameworks); } | ./md5sum.sh import

joc-clear.sum::
	cat joc-clear.sql | ./md5sum.sh joc-clear

joc-export.cpp.sum::
	cat joc-export.cpp | ./md5sum.sh joc-export.cpp

joc-export.sum:: joc-export.cpp.sum signature.sum
	$(MAKE) joc-export
	cat joc-export | ./md5sum.sh joc-export joc-export.cpp signature

joc.db.sum: joc-import.sum import.sum joc-clear.sum
	rm -f joc.db joc.db-journal
	./joc-execute joc-clear.sql
	for fw in $(frameworks); do \
	    path=$$(echo $${PKG_ROOT2}/System/Library/?(Private)Frameworks/"$${fw}.framework/$${fw}"); \
	    echo "$${path}"; \
	    ./joc-import "$${fw}" "$${PKG_ROOT2}" "$${path#$${PKG_ROOT2}}"; \
	done
	cp -f joc.db backup.db
	./joc-execute joc-repair.sql
	echo .dump | sqlite3 joc.db | ./md5sum.sh joc.db joc-import import joc-clear

straps.sum: joc.db.sum joc-export.sum
	rm -rf source/{hdr,obc,osx}/*
	mkdir -p source/hdr/struct_
	./joc-export
	find source/{hdr,obc,osx} -type f -exec cat {} \; | ./md5sum.sh straps joc.db joc-export

jocstrap.jar: source/joc/*.java straps.sum
	rm -rf build/*
	$(javac) -d build -cp source source/*/*.java
	jar -cf jocstrap.jar -C build joc -C build obc -C build osx
	ls -la jocstrap.jar

libobjc-sig.dylib: $(signature)
	$(target)gcc $(tldflags) -lobjc -dynamiclib -ObjC -o $@ -install_name /usr/lib/$@ -framework CoreFoundation -framework Foundation $^

libjocstrap.jnilib: $(object) libobjc-sig.dylib
	$(target)gcc $(tldflags) -lobjc -ldl -dynamiclib -ObjC -o $@ -install_name /usr/lib/$@ -framework CoreFoundation -framework Foundation $^

build/joc/RuntimeInfo.h:
	$(shell $(target)gcc -print-prog-name=cc1obj) -print-objc-runtime-info <(echo) >$@

%.class: %.java jocstrap.jar
	$(javac) $<

%.o: %.c $(headers)
	$(target)gcc -std=c99 -O3 $(tcflags) -c -I$(javah) -Isource -I.. -o $@ $<

%.o: %.m $(headers) build/joc/RuntimeInfo.h
	$(target)gcc -std=c99 -O3 $(tcflags) -c -ObjC $(cobjflags) -I$(javah) -Isource -Ibuild -I.. -o $@ $<

%: %.cpp $(headers) source/sig/parse.c
	g++ -O0 -g3 $(hcflags) $(hldflags) -I. -Isource -I.. -I$(shell apr-1-config --includedir) -lsqlite3 -o $@ $< source/sig/parse.c

.PHONY: all clean sql test test-jam tools
