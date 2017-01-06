#ifndef PLATFORM_HPP
#define PLATFORM_HPP

#if defined(_WIN32)
    #define WINVER 0x0500
    #define _WIN32_IE 0x0600
    #define _WIN32_WINNT 0x0500

    typedef int int32_t;
    typedef unsigned int uint32_t;

    #include <cstdlib>

    #include <direct.h>
    #include <io.h>
    #include <process.h>

    #define WIN32_LEAN_AND_MEAN
    #define VC_EXTRALEAN
    #include <windows.h>
#else
    #if defined(CFG_OS_Linux)
        #if defined(__alpha) || (defined(__sparc__) && defined(__arch64__))
            #define dev_t unsigned int
        #else
            #define dev_t unsigned short
        #endif
        #include <linux/loop.h>
        #undef dev_t

        #define MNT_DETACH 2

        #include <linux/major.h>
        #include <linux/raid/md_u.h>
    #endif

    #include <sys/param.h>
    #include <sys/types.h>

    #include <sys/ioctl.h>
    #include <sys/mman.h>
    #include <sys/mount.h>
    #include <sys/syscall.h>
    #include <sys/uio.h>
    #include <sys/wait.h>

    #include <pwd.h>
    #include <stdint.h>
    #include <termios.h>
    #include <unistd.h>
#endif

#include <cstdarg>
#include <cstring>

#include <new>

#include <errno.h>
#include <fcntl.h>
#include <signal.h>

#include <sys/stat.h>

extern char **environ;

#define CFG_PATHENV_SEPARATOR ':'
#define CFG_PATHSYS_SEPARATOR '/'

#endif//PLATFORM_HPP
