OS := $(shell uname -s)
PC := $(shell uname -n)

CXXFLAGS += $(INCLUDES:%=-Iobject/%) $(INCLUDES:%=-I%)
CXXFLAGS += -Ui386 -Ulinux -nostdlib

-include $(MENES)/docs/build.d/$(OS).os
-include $(MENES)/docs/build.d/$(PC).pc
include $(MENES)/docs/build.d/$(CC).cc
-include $(MENES)/docs/build.d/$(CC)-$(OS).cc-os
-include $(MENES)/docs/build.d/$(PC)-$(CC).pc-cc

# XXX: technically I don't think I need the -not -path "*/.svn/*"

SOURCES := $(shell find $(LOCALS) -name "*.cpp" -not -path "*/.svn/*" $(foreach REGEX,$(EXCLUDE),-not -regex ".*$(REGEX).*") | sort)
OBJECTS := $(SOURCES:%.cpp=object/%.o)

HEADERS := $(shell find $(LOCALS) -name "*.hh" -not -path "*/.svn/*" $(foreach REGEX,$(EXCLUDE),-not -regex ".*$(REGEX).*") | sort)
PRECOMP := $(HEADERS:%=object/%.gch)

LDFLAGS += $(LINKPATH:%=-Wl,-rpath,%) $(LINKPATH:%=-L%) $(LINKLIB:%=-l%)

TIME := $(shell which time)
TIMESH := $(MENES)/docs/build.d/time.sh

$(EXEPATH): $(PRECOMP) $(OBJECTS)
	@mkdir -p binary
	@$(TIMESH) "[$@]" $(CC) -o $@ $(LDFLAGS) $(OBJECTS)

test: execute

execute: $(EXEPATH)
	@$(TIME) $(EXEPATH) $(TESTARGS)

profile: $(EXEPATH)
	@$(TIME) $(EXEPATH) --profile-report --memory-report $(TESTARGS)

debug: $(EXEPATH)
	@echo run "$(TESTARGS)" > cmdargs.tmp
	@gdb $(EXEPATH) -x cmdargs.tmp

clean:
	rm -rf object/* $(EXEPATH)

object/%.o: %.cpp $(PRECOMP)
	@mkdir -p $(@D)
	@$(TIMESH) $< $(CC) -MP -MD $(CXXFLAGS) -c -o $@ $< `cat $<.$(CC) 2>/dev/null`

object/%.hh.gch: %.hh
	@mkdir -p $(@D)
	@$(TIMESH) $< $(CC) -MP -MD $(CXXFLAGS) -c -o $@ $<

.PHONY: clean test execute

-include $(OBJECTS:.o=.d) $(PRECOMP:.gch=.d)
