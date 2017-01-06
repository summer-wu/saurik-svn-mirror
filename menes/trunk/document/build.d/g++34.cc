CXXFLAGS += -funroll-loops -fmessage-length=0
CXXFLAGS += -ffor-scope -fno-dollars-in-identifiers
CXXFLAGS += -fno-gnu-keywords -funsigned-char
CXXFLAGS += -fno-enforce-eh-specs -ffast-math
CXXFLAGS += -Wold-style-cast -Wstrict-aliasing
CXXFLAGS += -Wreorder -Woverloaded-virtual
CXXFLAGS += -Wwrite-strings -Winvalid-pch
CXXFLAGS += -Wcast-align -Wcast-qual
CXXFLAGS += -Wpointer-arith -Wsign-compare
CXXFLAGS += -Wno-multichar -Wreturn-type
CXXFLAGS += -Wundef -Wendif-labels
#CXXFLAGS += -Wsequence-point -Winline
CXXFLAGS += -Wmissing-braces -Wunused-label
CXXFLAGS += -Winit-self #-Wuninitialized
CXXFLAGS += -Wunknown-pragmas -Wswitch
#CXXFLAGS += -Wfloat-equal -Wpadded

#CXXFLAGS += -nostdlib
#LDFLAGS += -nostdlib

# TheMatrix
#CXXFLAGS += -march=pentium4 -msse2
# Dreadnought
#CXXFLAGS += -march=pentium3 -msse
# Ironclad
#CXXFLAGS += -march=pentium2 -mmmx
