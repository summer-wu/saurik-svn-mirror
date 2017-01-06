EXEPATH := binary
MAINS := $(MENES)/projects/ninetjer/clr-{decompile,verify}.cpp
NAMESPACES += app clr xml
TESTPATH := binary/clr-decompile

TESTARGS := data/tests/test.exe
#TESTARGS := data/mscorlib/mono-1.0.2-1.1/mscorlib.dll
#TESTARGS := data/mscorlib/mono-1.0.2-2.0/mscorlib.dll
#TESTARGS += data/ninetjer/Pocket\ e-Sword.exe
