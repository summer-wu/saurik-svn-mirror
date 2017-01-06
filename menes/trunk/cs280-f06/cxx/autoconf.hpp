#ifdef _WIN32

#define CFG_SIZEOF_CHAR 1
static const int cfgSIZEOF_CHAR = 1;
#define CFG_SIZEOF_SHORT_INT 2
static const int cfgSIZEOF_SHORT_INT = 2;
#define CFG_SIZEOF_INT 4
static const int cfgSIZEOF_INT = 4;
#define CFG_SIZEOF_LONG_INT 4
static const int cfgSIZEOF_LONG_INT = 4;
#define CFG_SIZEOF_LONG_LONG_INT 8
static const int cfgSIZEOF_LONG_LONG_INT = 8;
#define CFG_SIZEOF_FLOAT 4
static const int cfgSIZEOF_FLOAT = 4;
#define CFG_SIZEOF_DOUBLE 8
static const int cfgSIZEOF_DOUBLE = 8;
#define CFG_SIZEOF_LONG_DOUBLE 8
static const int cfgSIZEOF_LONG_DOUBLE = 8;
#define CFG_SIZEOF_VOID_X 4
static const int cfgSIZEOF_VOID_X = 4;
#define CFG_SIZEOF_VOID_XCXDCD 4
static const int cfgSIZEOF_VOID_XCXDCD = 4;
#define CFG_SIZEOF_VOID_XCTIIXDCD 16
static const int cfgSIZEOF_VOID_XCTIIXDCD = 16;
#define CFG_SIZEOF_SIZE_T 4
static const int cfgSIZEOF_SIZE_T = 4;
#define CFG_DEFINE__WIN32

#define API_HAS_WIN32
#define API_HAS_WINSOCK

#else
//#error XXX: implement menes/autoconf.hpp to approximate the old menes/api #ifdefs
#endif
