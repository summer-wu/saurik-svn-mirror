lo-ccxx-flags -W3 -J -MD -GR -EHsc -Zc:forScope -Zc:wchar_t

hi-ld-flags -lmsvcrt
lo-ld-flags -nodefaultlib -subsystem:console

CFG_AR='link.exe'
CFG_AR_CLASS='link.exe'

CFG_LD='link.exe'
CFG_LD_CLASS='link.exe'

CXX_DLL_EXTENSION='.dll'
CXX_LIB_EXTENSION='.lib'
CXX_OBJ_EXTENSION='.obj'
CXX_PCH_EXTENSION='.pch'
CXX_EXE_EXTENSION='.exe'
