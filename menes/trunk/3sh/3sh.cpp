# Bash Header {{{
OS=$(uname -s)

case ${OS} in
    (Darwin|FreeBSD)
        MNT_=MNT_
    ;;

    (Linux)
        MNT_=MS_
    ;;
esac

function fix {
    echo "# $(($1 + 1)) \"3sh.cpp\""
}

function include {
    echo "# 1 \"$1\""
    . process.sh "$1" | bash
}
# }}}
@fix@; cat << EOF
// Copyright & Warrany Disclaimer {{{
/* 3sh - 3rd Generation Bourne Shell
 * Copyright (C) 2005  Jay Freeman (saurik)
*/

/*
 *        Redistribution and use in source and binary
 * forms, with or without modification, are permitted
 * provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the
 *    above copyright notice, _this list of conditions
 *    and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the
 *    above copyright notice, _this list of conditions
 *    and the following disclaimer in the documentation
 *    and/or other materials provided with the
 *    distribution.
 * 3. The name of the author may not be used to endorse
 *    or promote products derived from _this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
// }}}

// XXX: scrutinize: http://code.dogmap.org/lintsh/

#define CFG_OS_${OS}

#include "language.hpp"
#include "platform.hpp"
#include "mathematics.hpp"

#include "linux/types.h"
#include "linux/dirent.h"

#define LQ_ "`"
#define RQ_ "'"

//#define LQ_ "\xe2\x80\x98"
//#define RQ_ "\xe2\x80\x99"

#define BufferSize_ 4096

_finline int sys:GetProcessId() {
    #ifdef SYS_getpid
        return syscall(SYS_getpid);
    #else
        return getpid();
    #endif
}

_finline pid_t sys:WaitProcessId(pid_t pid, int *status, int options) {
    return syscall(SYS_waitpid, pid, status, options);
}

_finline int sys:Open(const char *pathname, int flags, mode_t mode) {
    return syscall(SYS_open, pathname, flags, mode);
}

_finline int sys:Write(int fd, const char *data, size_t size) {
    return syscall(SYS_write, fd, data, size);
}

_finline int sys:Read(int fd, char *data, size_t size) {
    return syscall(SYS_read, fd, data, size);
}

_finline char *FindLast(char *begin, char value, size_t size) {
    for (char *i(begin + size); i != begin; )
        if (*--i == value)
            return i;
    return NULL;
}

void vfwritef(int fd, const char *format, va_list arg);

void fwritef(int fd, const char *format, ...) {
    va_list arg;
    va_start(arg, format);
    vfwritef(fd, format, arg);
    va_end(arg);
}

void writef(const char *format, ...) {
    va_list arg;
    va_start(arg, format);
    vfwritef(1, format, arg);
    va_end(arg);
}

void writef2(const char *format, ...) {
    va_list arg;
    va_start(arg, format);
    vfwritef(2, format, arg);
    va_end(arg);
}

void assert_(bool test, const char *code, const char *file, int line) {
    if (!test) {
        writef("%s(%d): _assert(%s)\n", file, line, code);
        _breakpoint();
    }
}

_finline bool IsReservedPointer(void *value) {
    return reinterpret_cast<uintptr_t>(value) < 1024;
}

#define _assert(test) assert_(test, #test, __FILE__, __LINE__)
#define _trace() writef2("<%d> %s(%d)\n", sys:GetProcessId(), __FILE__, __LINE__)

const char *error-names_[] = {
    NULL, "PERM", "NOENT", "SRCH", "INTR", "IO", "NXIO", "2BIG", "NOEXEC",
    "BADF", "CHILD", "AGAIN", "NOMEM", "ACCES", "FAULT", "NOTBLK", "BUSY",
    "EXIST", "XDEV", "NODEV", "NOTDIR", "ISDIR", "INVAL", "NFILE", "MFILE",
    "NOTTY", "TXTBSY", "FBIG", "NOSPC", "SPIPE", "ROFS", "MLINK", "PIPE",
    "DOM", "RANGE", "DEADLK", "NAMETOOLONG", "NOLCK", "NOSYS", "NOTEMPTY",
    "LOOP", "WOULDBLOCK"
};

const char *error-descriptions_[] = {
    "that was _too_ successful",
    "permission denied",
    "there is no such entry",
    "the search found nothing",
    "i was interrupted",
    "i/o has failed, it has",
    "no can do... i/o?",
    "something is too big",
    "that is not executable",
    "this file is very very bad",
    "that is not your child",
    "ask again later",
    "out of memory",
    "accessibility denied",
    "system is faulty",
    "this is not a block device",
    "they said they are busy",
    "that file exists",
    "file system short circuited",
    "there is no such device",
    "this is not a directory",
    "this is a directory",
    "that was invalid",
    "global file shortage",
    "local file storage",
    "this is not a teletype",
    "text is... busy?",
    "this file is very big",
    "there is no space",
    "you can't seek a pipe",
    "that file system is read-only",
    "way too many links",
    "the pipe is now broken",
    "input outside of designated domain",
    "output outside of designated range",
    "resource deadlock narrowly averted",
    "this name is too long",
    "there are no more locks",
    "syscall wasn't implemented",
    "this is not empty",
    "encountered a causality loop",
    "this would take forever"
};

void Posix:PrintError_(const char *file, int line, int code, const char *function) {
    writef2("<%d> %s(%d): %s() errno = %d (E%s: %s)\n", sys:GetProcessId(), file, line, function, code,
        code < sizeof(error-names_) / sizeof(error-names_[0]) ?
            error-names_[code] :
            "?",
        code < sizeof(error-descriptions_) / sizeof(error-descriptions_[0]) ?
            error-descriptions_[code] :
            "???"
    );
}

#define Posix:PrintError(code, function) \
    Posix:PrintError_(__FILE__, __LINE__, code, function)

void Posix:Fatal(const char *file, int line) {
    static bool fatal = false;

    if (!fatal) {
        fatal = true;
        Posix:PrintError_(file, line, errno, NULL);
    }

    _trace();
    _exit(1);
}

#ifdef _WIN32
void Win32:Fatal(const char *file, int line) {
    _trace();
    _exit(1);
}

_finline HANDLE Win32:CheckHandle(HANDLE handle, const char *file, int line) {
    if (handle == INVALID_HANDLE_VALUE)
        Win32:Fatal(file, line);
    return handle;
}

template <typename Type_>
_finline Type_ Win32:CheckZero(Type_ handle, const char *file, int line) {
    if (value == 0)
        Win32:Fatal(file, line);
    return value;
}

#define Eh(handle) Win32:CheckHandle(handle, __FILE__, __LINE__)
#define Ez(value) Win32:CheckZero(value, __FILE__, __LINE__)
#endif

#define _syscall(call) \
    do if ((int) (call) == -1) \
        if (errno != EINTR) \
            Posix:Fatal(__FILE__, __LINE__); \
        else continue; \
    else break; \
    while (true)

bool Kill_(int pid, int signal) {
    if (::kill(pid, signal) != -1)
        return true;
    {
        Posix:PrintError(errno, "kill");
        return false;
    }
}

void Signal_(int signum, void (*handler)(int)) {
#if defined(SYS_signal)
    syscall(SYS_signal, signum, handler);
#elif defined(_WIN32)
    signal(signum, handler);
#else
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = handler;
    _syscall(sigaction(signum, &action, NULL));
#endif
}

struct Map {
    char *data_;
    size_t size_;

    bool Initialize(const char *path, int open_flags = O_RDONLY, int mmap_prot = PROT_READ, int mmap_flags = MAP_SHARED) {
        int file(sys:Open(path, open_flags, 0));
        if (file == -1) {
            Posix:PrintError(errno, "open");
            return false;
        }

        struct stat stat;
        _syscall(fstat(file, &stat));

        if (stat.st_size == 0) {
            _syscall(close(file));
            return true;
        }

        size_ = stat.st_size;

        _syscall(data_ = ((char *) mmap(NULL, size_, PROT_READ, MAP_SHARED, file, 0)));
        _syscall(close(file));
        return true;
    }

    ~Map() {
        _syscall(munmap(data_, size_));
    }
};

void *map_(size_t size) {
    //writef("map_(%d)\n", size);
    void *map;
    _syscall(map = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0));
    return map;
}

#include "memory.hpp"

_finline char ToUpper(char value) {
    return value >= 'a' && value <= 'z' ?
        value - 'a' + 'A' : value;
}

class Set {
  private:
    uint32_t set_[4];

  public:
    _finline Set() {
        set_[0] = 0;
        set_[1] = 0;
        set_[2] = 0;
        set_[3] = 0;
    }

    _finline Set(uint32_t set0, uint32_t set1, uint32_t set2, uint32_t set3) {
        set_[0] = set0;
        set_[1] = set1;
        set_[2] = set2;
        set_[3] = set3;
    }

    _finline void Merge(uint32_t set0, uint32_t set1, uint32_t set2, uint32_t set3) {
        set_[0] |= set0;
        set_[1] |= set1;
        set_[2] |= set2;
        set_[3] |= set3;
    }

    _finline void Merge(const Set &rhs) {
        set_[0] |= rhs.set_[0];
        set_[1] |= rhs.set_[1];
        set_[2] |= rhs.set_[2];
        set_[3] |= rhs.set_[3];
    }

    _finline bool operator ()(char value) const {
        return (set_[value / 32] >> (value % 32)) & 0x1;
    }
};

template <typename Type_>
class _R {
  private:
    Type_ *value_;

  public:
    _finline _R() :
        value_(NULL)
    {
    }

    _finline _R(Type_ *value) :
        value_(value)
    {
    }

    _finline ~_R() {
        Drop();
    }

    _finline void Drop() {
        drop(value_);
    }

    _finline void Null() {
        value_ = NULL;
    }

    _finline void Clear() {
        Drop();
        Null();
    }

    _finline _R<Type_> &operator =(Type_ *value) {
        value_ = value;
        return *this;
    }

    _finline _R<Type_> &operator =(intptr_t value) {
        value_ = reinterpret_cast<Type_ *>(value);
        return *this;
    }

    _finline Type_ *&GetValue() {
        return value_;
    }

    _finline operator Type_ *&() {
        return value_;
    }

    _finline operator intptr_t() {
        return reinterpret_cast<intptr_t>(value_);
    }

    _finline bool operator !() {
        return !value_;
    }

    _finline Type_ *operator ->() {
        return value_;
    }

    _finline Type_ *Release() {
        Type_ *value(value_);
        value_ = NULL;
        return value;
    }
};

unsigned CountDigits(int value) {
    unsigned count(1);
    while (value /= 10)
        ++count;
    return count;
}

bool ParseDigit(char digit, int radix, int *value) {
    if (digit >= '0' && digit <= '9')
        *value = digit - '0';
    else if (digit >= 'A' && digit < 'A' - 10 + radix)
        *value = 10 + digit - 'A';
    else if (digit >= 'a' && digit < 'a' - 10 + radix)
        *value = 10 + digit - 'a';
    else return false;
    return true;
}

void Integer:PrintError(const char *begin, size_t length) {
    writef2(LQ_"%S"RQ_" is non\-integral\n", begin, length);
}

bool ParseInteger(const char *begin, size_t length, unsigned radix, int *value) {
    *value = 0;

    if (length == 0)
        return false;

    bool negative;
    if (*begin != '-')
        negative = false;
    else {
        ++begin;
        --length;
        negative = true;
    }

    while (length != 0) {
        *value *= radix;
        int digit;
        if (!ParseDigit(*begin++, radix, &digit))
            return false;
        *value += digit;
        --length;
    }

    if (negative)
        *value = -*value;

    return true;
}

bool ParseDecimal(const char *begin, size_t length, int *value) {
    if (ParseInteger(begin, length, 10, value))
        return true;
    else {
        Integer:PrintError(begin, length);
        return false;
    }
}

bool writeall(int fd, const char *begin, size_t length) {
    while (length != 0) {
        int writ(sys:Write(fd, begin, length));
        if (writ == -1) {
            Posix:PrintError(errno, "write");
            return false;
        }

        begin += writ;
        length -= writ;
    }

    return true;
}

// XXX: buffer the hell out of this
void vfwritef(int fd, const char *format, va_list arg) {
    _forever {
        const char *end(strchr(format, '%'));

        if (!end) {
            writeall(fd, format, strlen(format));
            break;
        }

        if (end[1] == '%')
            writeall(fd, format, end + 1 - format);
        else {
            writeall(fd, format, end - format);

            switch (end[1]) {
                case 'b': {
                    int value(va_arg(arg, int));
                    if (value)
                        writeall(fd, "true", 4);
                    else
                        writeall(fd, "false", 5);
                } break;

                case 'c': {
                    char value(va_arg(arg, int));
                    writeall(fd, &value, 1);
                } break;

                case 'd': {
                    int value(va_arg(arg, int));
                    if (value == 0)
                        writeall(fd, "0", 1);
                    else {
                        char buffer[11], *end(buffer + sizeof(buffer));

                        bool negative(value < 0);
                        if (negative)
                            value = -value;

                        while (value != 0) {
                            *--end = '0' + value % 10;
                            value /= 10;
                        }

                        if (negative)
                            *--end = '-';

                        writeall(fd, end, buffer + sizeof(buffer) - end);
                    }
                } break;

                case 'r': {
                    int value(va_arg(arg, int));
                    unsigned count(va_arg(arg, unsigned));

                    _array(char, line, count);
                    for (unsigned i = 0; i != count; ++i)
                        line[i] = value;

                    writeall(fd, line, count);
                } break;

                case 's': {
                    const char *string(va_arg(arg, const char *));
                    if (!string)
                        writeall(fd, "<null>", 6);
                    else
                        writeall(fd, string, strlen(string));
                } break;

                case 'S': {
                    const char *string(va_arg(arg, const char *));
                    size_t length(va_arg(arg, size_t));
                    writeall(fd, string, length);
                } break;

                case 'x': {
                    unsigned value(va_arg(arg, unsigned));
                    if (value == 0)
                        writeall(fd, "0", 1);
                    else {
                        char buffer[10], *end(buffer + sizeof(buffer));

                        while (value != 0) {
                            int digit(value % 16);
                            *--end = (digit < 10 ?
                                '0' + digit :
                                'a' + digit - 10);
                            value /= 16;
                        }

                        writeall(fd, end, buffer + sizeof(buffer) - end);
                    }
                } break;
            }
        }

        format = end + 2;
    }
}

struct Buffer {
    int fd_;
    bool unbuffered_;

    char data_[BufferSize_];
    size_t count_;

    bool WriteFully_(const char *data, size_t length) {
        return writeall(fd_, data, length);
    }

    _finline Buffer(int fd, bool unbuffered) :
        fd_(fd),
        unbuffered_(unbuffered),
        count_(0)
    {
    }

    _finline ~Buffer() {
        Flush();
    }

    bool WriteFully(char *data, size_t length) {
        if (unbuffered_)
            WriteFully_(data, length);
        else if (count_ + length <= BufferSize_) {
            memcpy(data_ + count_, data, length);
            count_ += length;
        } else {
            /* XXX: why the FUCK does this cost so much?!?
            struct iovec vector[2];
            vector[0].iov_base = data_;
            vector[0].iov_len = count_;
            vector[1].iov_base = data;
            vector[1].iov_len = length;

          write:
            int writ = ::writev(fd_, vector, 2);
            if (writ == -1) {
                Posix:PrintError(errno, "writev");
                return false;
            } else if (writ <= vector[0].iov_len) {
                vector[0].iov_len -= writ;
                goto write;
            } else {
                writ -= vector[0].iov_len;
                return WriteFully_(data + writ, length - writ);
            }*/

            size_t rest(BufferSize_ - count_);
            memcpy(data_ + count_, data, rest);

            data += rest;
            length -= rest;

            if (!WriteFully_(data_, BufferSize_))
                return false;

            while (length >= BufferSize_) {
                if (!WriteFully_(data, BufferSize_))
                    return false;
                data += BufferSize_;
                length -= BufferSize_;
            }

            memcpy(data_, data, length);
            count_ = length;
        }

        return true;
    }

    _finline bool Flush() {
        if (count_) {
            if (!WriteFully_(data_, count_))
                return false;
            count_ = 0;
        }

        return true;
    }
};

struct Object {
    virtual _finline ~Object() {
    }
};

template <typename Type_>
struct Ring {
    size_t count_;
    size_t size_;
    Type_ **data_;
    size_t offset_;

    _finline Ring(size_t size) :
        count_(0),
        size_(size),
        data_((Type_ **) allocate(size * sizeof(void *))),
        offset_(_not(size_t))
    {
    }

    _finline ~Ring() {
        Type_ **data = data_ + offset_;

        for (size_t i(0); i != count_; ++i) {
            drop(*data);
            if (data == data_)
                data = data_ + size_;
            --data;
        }

        drop(data_);
    }

    Type_ *&operator [](size_t index) {
        return data_[(size_ + offset_ - index) % size_];
    }

    _finline Type_ *&First() {
        return data_[offset_];
    }

    void InsertFirst(Type_ *value) {
        if (count_ != size_)
            ++count_;
        offset_ = (offset_ + 1) % size_;
        First() = value;
    }
};

struct Vector_ {
    size_t count_;
    size_t size_;
    void **data_;

    _finline Vector_() :
        count_(0),
        size_(0),
        data_(NULL)
    {
    }

    _finline ~Vector_() {
        drop(data_);
    }

    void Reallocate_() {
        void **data = (void **) allocate(size_ * sizeof(void *));
        memcpy(data, data_, count_ * sizeof(void *));
        drop(data_);
        data_ = data;
    }

    void DropAll_(void (*finalizer)(void *)) {
        for (size_t i(0); i != count_; ++i)
            drop_(data_[i], finalizer);
    }

    void InsertLast_(void *value) {
        if (count_ == size_) {
            size_ = (size_ + 4) * 2;
            Reallocate_();
        }

        data_[count_++] = value;
    }

    _finline void InsertLast(void *value) {
        _assert(value != NULL);
        InsertLast_(value);
    }

    void Reserve(size_t size) {
        if (size_ < size) {
            size_ = size + 4;
            Reallocate_();
        }
    }

    _finline bool IsEmpty() const {
        return !count_;
    }

    _finline void RemoveLast() {
        _assert(count_);
        --count_;
    }

    // XXX: maybe replace with a faster function that swaps entries
    void Remove(size_t index) {
        memmove(data_ + index, data_ + index + 1, (--count_ - index) * sizeof(void *));
    }
};

template <typename Type_, bool External_ = false>
struct Vector :
    public Vector_
{
    _finline void DropAll_() {
        if (!External_)
            Vector_::DropAll_(Deconstruct_<Type_>::Get());
    }

    void Clear() {
        DropAll_();
        count_ = 0;
    }

    ~Vector() {
        DropAll_();
    }

    _finline void InsertLast_(Type_ *value) {
        return Vector_::InsertLast_(value);
    }

    _finline void InsertLast(Type_ *value) {
        return Vector_::InsertLast(value);
    }

    _finline Type_ *&operator [](size_t index) const {
        return ((Type_ **) Vector_::data_)[index];
    }

    _finline Type_ *&Last() {
        return operator [](Vector_::count_ - 1);
    }
};

#define Vector:ForEach(type, index, vector) \
    for (size_t i(0); i != (vector)->count_; ++i) \
        for (type *index = (*(vector))[i]; index != NULL; index = NULL)

template <typename Type_>
void Vector:MoveAllLast(Vector<Type_> *_this, Vector<Type_> *values) {
    _this->Reserve(_this->count_ + values->count_);
    memcpy(_this->data_ + _this->count_, values->data_, values->count_ * sizeof(Type_ *));
    _this->count_ += values->count_;
    values->count_ = 0;
}

template <typename Type_>
void Vector:RemoveFirst(Vector<Type_> *_this, size_t count) {
    for (size_t i(0); i != count; ++i)
        drop(_this->data_[i]);
    memmove(_this->data_, _this->data_ + count, (_this->count_ -= count) * sizeof(void *));
}

_finline void Swap_(void *lhs, void *rhs, size_t size) {
    _array(char, data, size);
    memcpy(data, lhs, size);
    memcpy(lhs, rhs, size);
    memcpy(rhs, data, size);
}

#define Swap(lhs, rhs) Swap_(&lhs, &rhs, sizeof(lhs))

#include "string.hpp"

void UnparseDecimal(String *string, int value) {
    if (!value)
        string->InsertLast('0');
    else {
        char buffer[11], *end = buffer + sizeof(buffer);

        bool negative = value < 0;
        if (negative)
            value = -value;

        while (value != 0) {
            *--end = '0' + value % 10;
            value /= 10;
        }

        if (negative)
            *--end = '-';

        string->CopyRangeLast(end, buffer + sizeof(buffer) - end);
    }
}

bool System:GetCurrentDirectory(String *pwd) {
    size_t size(256);

  attempt:
    _array(char, buffer, size);
#ifdef SYS_getcwd
    int result(::syscall(SYS_getcwd, buffer, size));
    if (result != -1)
        size = result - 1;
    else {
#else
    char *path(::getcwd(buffer, size));
    if (path != NULL)
        size = strlen(path);
    else {
#endif
        int code(errno);
        if (code == ERANGE) {
            size *= 2;
            goto attempt;
        }

        Posix:PrintError(code, "getcwd");
        return false;
    }

    pwd->AssignRange(buffer, size);
    return true;
}

#include "hashtable.hpp"

class Options {
  private:
    Vector<String> &arguments_;
    size_t &shift_;

    Set extra_;
    size_t offset_;

    char *data_;
    size_t count_;

  public:
    Options(Vector<String> &arguments, size_t &shift, uint32_t set0, uint32_t set1, uint32_t set2, uint32_t set3) :
        arguments_(arguments),
        shift_(shift),
        extra_(set0, set1, set2, set3),
        offset_(0)
    {
    }

    _finline char *GetArgumentData() {
        return data_;
    }

    _finline size_t GetArgumentCount() {
        return count_;
    }

    int Parse() {
        if (shift_ == arguments_.count_)
            return 0;

        String *argument(arguments_[shift_]);
        if (offset_++) option: {
            int value(argument->data_[offset_]);
            if (offset_ + 1 == argument->count_) {
                ++shift_;
                offset_ = 0;

                if (argument->count_ == 2 && value == '-')
                    return 0;
            }

            if (extra_(value))
                if (shift_ == arguments_.count_) {
                    writef2(LQ_"-%c"RQ_" missing argument\n", value);
                    return '?';
                } else {
                    size_t offset(offset_ ? offset_ + 1: 0);
                    data_ = arguments_[shift_]->data_ + offset;
                    count_ = arguments_[shift_]->count_ - offset;

                    ++shift_;
                    offset_ = 0;
                }

            return value;
        } else if (
            !argument->count_ ||
            argument->data_[0] != '-' ||
            argument->count_ == 1
        )
            return 0;
        else goto option;
    }
};

#ifndef CFG_NO_INTERACTIVE
struct Input {
    String buffer_;
    size_t offset_;

    Input() :
        offset_(0)
    {
        buffer_.Reserve(BufferSize_);
    }
};

_finline bool Input:Read_(Input *_this) {
    _this->offset_ = 0;

    // XXX: Shell:Trap(); ?!?? :(
    _syscall(_this->buffer_.count_ = sys:Read(0, _this->buffer_.data_, _this->buffer_.size_));
    return true;
}

int Input:Get_(Input *_this) {
    if (_this->offset_ == _this->buffer_.count_)
        if (!Input:Read_(_this) || !_this->buffer_.count_)
            return -1;
    return _this->buffer_.data_[_this->offset_++];
}

int Input:Get(Input *_this, char *utfd, size_t *utfs) {
    size_t total = 1;

    int first(Input:Get_(_this));
    if (first == -1)
        return -1;

    *utfd++ = first;

    if ((first & 0x80) != 0)
        while ((first & (0x80 >> ++total)) != 0);

    *utfs = total;

    uint32_t value = first & (0xff >> total);
    while (--total != 0) {
        value = (value << 6) | ((*utfd++ = first = Input:Get_(_this)) & ~0xc0);
        if (first == -1) {
            _trace();
            return -1;
        }
    }

    return value;
}

struct Prompt:Buffer {
    Prompt:Buffer *next_;
    size_t count_;
    size_t size_;
    int data_[];

    _finline Prompt:Buffer(Prompt:Buffer *next, size_t count, size_t size) :
        next_(next),
        count_(count),
        size_(size)
    {
    }
};

struct Prompt {
    int fin_;
    int fout_;
    bool terminal_;
    unsigned cursor_;

    Ring<Prompt:Buffer> history_;
    size_t offset_;
    size_t furthest_;

    _finline Prompt(int fin, int fout) :
        fin_(fin),
        fout_(fout),
        terminal_(::isatty(fin) && ::isatty(fout)),
        history_(1000),
        offset_(_not(size_t))
    {
    }
};

void Prompt:Allocate(Prompt *_this) {
    size_t size(16);
    Prompt:Buffer *buffer(new (allocate(size * sizeof(int) + sizeof(Prompt:Buffer))) Prompt:Buffer(NULL, 0, size));

    _this->history_.InsertFirst(buffer);
    _this->offset_ = 0;
}

_finline Prompt:Buffer *&Prompt:GetBuffer(Prompt *_this) {
    return _this->history_[_this->offset_];
}

_finline size_t *Prompt:Count(Prompt *_this) {
    return &Prompt:GetBuffer(_this)->count_;
}

_finline void Prompt:Output_(Prompt *_this, const char *begin, size_t length) {
    if (_this->terminal_)
        writeall(_this->fout_, begin, length);
}

_finline void Prompt:Output(Prompt *_this, const char *begin, size_t length) {
    Prompt:Output_(_this, begin, length);
    _this->cursor_ += length;
}

void Prompt:OutputRange(Prompt *_this, size_t begin, size_t end) {
    Prompt:Buffer *buffer(Prompt:GetBuffer(_this));

    for (size_t i(begin); i != end; ++i) {
        int value(buffer->data_[i]);

        if (value < 0x80) {
            char c(value);
            Prompt:Output_(_this, &c, 1);
        } else {
            unsigned levels = ilog2(value) / 6;

            char buffer[4];
            char *data(buffer);

            *data++ = 0xff << 8 - levels - 1 | value >> levels * 6;
            for (unsigned level(0); level != levels; ++level)
                *data++ = value >> (levels - level - 1) * 6 & 0x3f | 0x80;

            Prompt:Output_(_this, buffer, data - buffer);
        }
    }

    _this->cursor_ += end - begin;
}

void Prompt:Touch(Prompt *_this) {
    if (_this->offset_ == 0)
        return;

    Prompt:Buffer *buffer(Prompt:GetBuffer(_this));
    if (!buffer->next_) {
        size_t size(buffer->size_ * sizeof(int) + sizeof(Prompt:Buffer));
        Prompt:Buffer *next((Prompt:Buffer *) allocate(size));
        memcpy(next, buffer, size);
        buffer->next_ = next;
    }
}

void Prompt:Repeat_(Prompt *_this, const char *begin, size_t length, unsigned count) {
    for (unsigned i(0); i != count; ++i)
        Prompt:Output_(_this, begin, length);
}

void Prompt:Erase(Prompt *_this, unsigned count) {
    Prompt:Repeat_(_this, " ", 1, count);
    _this->cursor_ += count;
}

void Prompt:MoveLeft(Prompt *_this, unsigned count) {
    Prompt:Repeat_(_this, "\x1b[D", 3, count);
    _this->cursor_ -= count;
}

void Prompt:MoveRight(Prompt *_this, unsigned count) {
    Prompt:Repeat_(_this, "\x1b[C", 3, count);
    _this->cursor_ += count;
}

void Prompt:Clear(Prompt *_this) {
    Prompt:Buffer *buffer = Prompt:GetBuffer(_this);
    Prompt:MoveLeft(_this, _this->cursor_);
    Prompt:Erase(_this, buffer->count_);
    Prompt:MoveLeft(_this, buffer->count_);
}

typedef enum {
    Modifier:None = 0x0,

    Modifier:Alt = 0x1,
    Modifier:Control = 0x2,
    Modifier:Shift = 0x4,

    Modifier:AltControl = 0x3,
    Modifier:AltShift = 0x5,
    Modifier:ControlShift = 0x6
} Prompt:Modifier;

Prompt:Modifier Prompt:Modify_(int code) {
    switch (code) {
        case 0: return Modifier:None;
        case 2: return Modifier:Shift;
        case 3: return Modifier:Alt;
        case 4: return Modifier:AltShift;
        case 5: return Modifier:Control;
        case 6: return Modifier:ControlShift;
        case 7: return Modifier:AltControl;

        default:
            _assert(false);
            return Modifier:None;
    }
}

_finline void Prompt:Press-Tab(Prompt *_this) {
    // XXX: implement
}

_finline void Prompt:Press-Backspace(Prompt *_this, Prompt:Modifier modifier) {
    size_t cursor(_this->cursor_);
    if (cursor != 0) {
        Prompt:Buffer *buffer(Prompt:GetBuffer(_this));

        Prompt:Touch(_this);
        Prompt:MoveLeft(_this, 1);
        Prompt:OutputRange(_this, cursor, buffer->count_);
        Prompt:Erase(_this, 1);
        Prompt:MoveLeft(_this, buffer->count_ - cursor + 1);

        memmove(buffer->data_ + cursor - 1, buffer->data_ + cursor, (buffer->count_ - cursor) * sizeof(int));

        --buffer->count_;
    }
}

_finline void Prompt:Press-Delete(Prompt *_this, Prompt:Modifier modifier) {
    Prompt:Buffer *buffer = Prompt:GetBuffer(_this);

    size_t cursor = _this->cursor_;
    if (cursor != buffer->count_) {
        Prompt:Touch(_this);

        memmove(buffer->data_ + cursor, buffer->data_ + cursor + 1, (buffer->count_ - cursor - 1) * sizeof(int));
        --buffer->count_;

        Prompt:OutputRange(_this, cursor, buffer->count_);
        Prompt:Erase(_this, 1);
        Prompt:MoveLeft(_this, buffer->count_ - cursor + 1);
    }
}

_finline void Prompt:Press-Home(Prompt *_this, Prompt:Modifier modifier) {
    Prompt:MoveLeft(_this, _this->cursor_);
}

_finline void Prompt:Press-End(Prompt *_this, Prompt:Modifier modifier) {
    Prompt:MoveRight(_this, Prompt:GetBuffer(_this)->count_ - _this->cursor_);
}

_finline void Prompt:Press-PageUp(Prompt *_this, Prompt:Modifier modifier) {
    if (_this->offset_ != _this->history_.count_ - 1) {
    }
}

_finline void Prompt:Press-PageDown(Prompt *_this, Prompt:Modifier modifier) {
    if (_this->offset_ != 0) {
    }
}

_finline void Prompt:Press-Up(Prompt *_this, Prompt:Modifier modifier) {
    if (_this->offset_ != _this->history_.count_ - 1) {
        Prompt:Clear(_this);
        if (_this->offset_++ == _this->furthest_)
            _this->furthest_ = _this->offset_;

        Prompt:Buffer *buffer = Prompt:GetBuffer(_this);
        Prompt:OutputRange(_this, 0, buffer->count_);
    }
}

_finline void Prompt:Press-Down(Prompt *_this, Prompt:Modifier modifier) {
    if (_this->offset_ != 0) {
        Prompt:Clear(_this);
        --_this->offset_;

        Prompt:Buffer *buffer = Prompt:GetBuffer(_this);
        Prompt:OutputRange(_this, 0, buffer->count_);
    }
}

_finline void Prompt:Press-Left(Prompt *_this, Prompt:Modifier modifier) {
    if (_this->cursor_ != 0)
        Prompt:MoveLeft(_this, 1);
}

_finline void Prompt:Press-Right(Prompt *_this, Prompt:Modifier modifier) {
    if (_this->cursor_ != *Prompt:Count(_this))
        Prompt:MoveRight(_this, 1);
}

_finline void Prompt:Press-Key(Prompt *_this, int key) {
    Prompt:Touch(_this);

    Prompt:Buffer *&current = Prompt:GetBuffer(_this);
    if (current->count_ == current->size_) {
        Prompt:Buffer *buffer = current;
        size_t size = buffer->size_ * 2;
        Prompt:Buffer *data = new (allocate(size * sizeof(int) + sizeof(Prompt:Buffer))) Prompt:Buffer(buffer->next_, buffer->count_, size);
        memcpy(data->data_, buffer->data_, buffer->size_ * sizeof(int));
        drop(buffer);
        current = data;
    }

    Prompt:Buffer *buffer = current;
    size_t cursor = _this->cursor_;

    memmove(buffer->data_ + cursor + 1, buffer->data_ + cursor, (buffer->count_ - cursor) * sizeof(int));
    ++buffer->count_;
    buffer->data_[cursor] = key;

    Prompt:OutputRange(_this, cursor, buffer->count_);
    Prompt:MoveLeft(_this, buffer->count_ - cursor - 1);
}

#ifndef _WIN32
bool Prompt:Prompt(Prompt *_this, String &text, size_t width, String &command) {
    struct termios termold;

    if (_this->terminal_) {
        struct termios termios;
        _syscall(tcgetattr(0, &termios));
        termold = termios;

        termios.c_lflag &= ~(ECHO | ICANON);
        _syscall(tcsetattr(0, TCSANOW, &termios));
    }

    Input input;

    _this->cursor_ = 0;

    Prompt:Allocate(_this);
    _this->furthest_ = 0;

    bool complete(false);

  old-prompt:
    writef("%S", text.data_, text.count_);

    _forever {
        char utfd[4];
        size_t utfs;

        int next = Input:Get(&input, utfd, &utfs);
        //writef("0x%x %d '%c'\n", next, next, next);

        switch (next) {
            case -1:
                Prompt:Output_(_this, "\n", 1);
                goto done;
            break;

            case 0x04: {
                if (*Prompt:Count(_this) == 0) {
                    Prompt:Output_(_this, "\n", 1);
                    goto done;
                }
            } break;

            case 0x08:
            case 0x7f:
                Prompt:Press-Backspace(_this, Modifier:None);
            break;

            case '\t':
                Prompt:Press-Tab(_this);
            break;

            case '\n': {
                Prompt:Output_(_this, "\n", 1);

                Prompt:Buffer *buffer = Prompt:GetBuffer(_this);

                if (buffer->count_ == 0)
                    goto old-prompt;

                command.Clear();

                for (size_t i(0); i != buffer->count_; ++i) {
                    int value = buffer->data_[i];

                    if (value < 0x80)
                        command.InsertLast(value);
                    else {
                        unsigned levels = ilog2(value) / 6;
                        command.Increase(levels + 1);

                        char *data = command.data_ + command.count_;
                        command.count_ += levels + 1;

                        *data++ = 0xff << 8 - levels - 1 | value >> levels * 6;
                        for (unsigned level = 0; level != levels; ++level)
                            *data++ = value >> (levels - level - 1) * 6 & 0x3f | 0x80;
                    }
                }

                command.Increase(1);
                command.data_[command.count_++] = '\n';

                if (_this->offset_ != 0) {
                    Prompt:Touch(_this);
                    Prompt:Buffer *&current = _this->history_[_this->offset_];
                    Prompt:Buffer *&first = _this->history_.First();

                    Prompt:Buffer *buffer = current;
                    current = buffer->next_;
                    buffer->next_ = first;
                    first = buffer;
                }

                for (size_t i(0); i != _this->furthest_ + 1; ++i) {
                    Prompt:Buffer *buffer = _this->history_[i];
                    drop(buffer->next_);
                    buffer->next_ = NULL;
                }

                //writef("%d\n", command.count_);

                complete = true;
                goto done;
            } break;

            case 0x1b: {
                next = Input:Get(&input, utfd, &utfs);
                //writef("%c\n", next);

                switch (next) {
                    case 'O': {
                        next = Input:Get_(&input);
                        switch (next) {
                            case 'F': Prompt:Press-End(_this, Modifier:None); break;
                            case 'H': Prompt:Press-Home(_this, Modifier:None); break;
                        }
                    } break;

                    case '[': {
                        int argv[3] = {0, 0, 0};
                        size_t argc = 1;

                        _forever {
                            next = Input:Get_(&input);
                            if (next == ';')
                                _assert(++argc != sizeof(argv) / sizeof(argv[0]));
                            else if (next >= '0' && next <= '9')
                                argv[argc - 1] = argv[argc - 1] * 10 + next - '0';
                            else break;
                        }

                        //writef("%d{%d;%d;%d} %c\n", argc, argv[0], argv[1], argv[2], next);

                        switch (next) {
                            case 0x1b: {
                                goto output;
                            } break;

                            case '~': {
                                switch (argv[0]) {
                                    case 1: Prompt:Press-Home(_this, Prompt:Modify_(argv[1])); break;
                                    case 3: Prompt:Press-Delete(_this, Prompt:Modify_(argv[1])); break;
                                    case 4: Prompt:Press-End(_this, Prompt:Modify_(argv[1])); break;
                                    case 5: Prompt:Press-PageUp(_this, Prompt:Modify_(argv[1])); break;
                                    case 6: Prompt:Press-PageDown(_this, Prompt:Modify_(argv[1])); break;
                                }
                            } break;

                            case 'A': Prompt:Press-Up(_this, Prompt:Modify_(argv[0])); break;
                            case 'B': Prompt:Press-Down(_this, Prompt:Modify_(argv[0])); break;
                            case 'C': Prompt:Press-Right(_this, Prompt:Modify_(argv[0])); break;
                            case 'D': Prompt:Press-Left(_this, Prompt:Modify_(argv[0])); break;
                            case 'F': Prompt:Press-End(_this, Prompt:Modify_(argv[0])); break;
                            case 'H': Prompt:Press-Home(_this, Prompt:Modify_(argv[0])); break;
                        }
                    } break;
                }
            } break;

            default:
            output:
                Prompt:Press-Key(_this, next);
            break;
        }
    }

  done:
    if (_this->terminal_)
        _syscall(tcsetattr(0, TCSANOW, &termold));

    return complete;
}
#endif
#endif

struct Shell;
extern Shell shell_;

typedef enum {
    Fragment:Execute,
    Fragment:Glob,
    Fragment:Literal,
    Fragment:Parameter,
    Fragment:Parametric,
    Fragment:Process
} Fragment:Type;

struct Fragment {
    Fragment:Type type_;
    bool quoted_;

    _finline Fragment(Fragment:Type type, bool quoted) :
        type_(type),
        quoted_(quoted)
    {
    }

    virtual ~Fragment() {
    }
};

struct Word {
    Vector<Fragment> fragments_;
};

struct Assign {
    _R<Word> name_;
    _R<Word> subscript_;
    _R<Word> value_;
};

typedef enum {
    Redirect:LeftAngle,
    Redirect:LeftAngleAmpersand,
    Redirect:LeftAngleLeftAngle,
    Redirect:LeftAngleRightAngle,

    Redirect:LeftAngleLeftAngleMinus,
    Redirect:LeftAngleLeftAngleLeftAngle,

    Redirect:RightAngle,
    Redirect:RightAngleAmpersand,
    Redirect:RightAnglePipe,
    Redirect:RightAngleRightAngle
} Redirect:Type;

struct Redirect {
    Redirect:Type type_;
    bool numeric_;
    int descriptor_;
    Word *word_;

    Redirect() :
        word_(NULL)
    {
    }

    ~Redirect() {
        drop(word_);
    }
};

typedef enum {
    Command:Case,
    Command:Compound,
    Command:Conditional,
    Command:ArithmeticFor,
    Command:IterativeFor,
    Command:Function,
    Command:If,
    Command:Not,
    Command:Simple,
    Command:Subshell,
    Command:While
} Command:Type;

struct Command :
    public Object
{
    Command:Type type_;
    bool inverse_;
    Vector<Redirect> redirects_;
    Command *pipe_;

    Command(Command:Type type) :
        type_(type),
        pipe_(NULL)
    {
    }

    virtual ~Command() {
        drop(pipe_);
    }
};

struct Logical {
    Vector<Command> commands_;
    bool asynchronous_;
};

struct Execute :
    public Fragment
{
    Vector<Logical> list_;

    _finline Execute(bool quoted) :
        Fragment(Fragment:Execute, quoted)
    {
    }
};

struct Glob :
    public Fragment
{
    char type_;
    Vector<Word> patterns_;

    _finline Glob(char type) :
        Fragment(Fragment:Glob, false),
        type_(type)
    {
    }
};

struct Literal :
    public Fragment
{
    String value_;

    Literal(bool quoted) :
        Fragment(Fragment:Literal, quoted)
    {
    }
};

struct Parameter :
    public Fragment
{
    String name_;

    Parameter(bool quoted) :
        Fragment(Fragment:Parameter, quoted)
    {
    }
};

enum Parametric:Type {
    Parametric:None,

    Parametric:ColonEqual,
    Parametric:ColonMinus,
    Parametric:ColonPlus,
    Parametric:ColonQuestion,

    Parametric:Equal,
    Parametric:Minus,
    Parametric:Plus,
    Parametric:Question,

    Parametric:SubscriptAt,
    Parametric:SubscriptStar,

    Parametric:Percent,
    Parametric:PercentPercent,
    Parametric:Pound,
    Parametric:PoundPound
};

struct Parametric :
    public Fragment
{
    Word name_;
    Word subscript_;

    bool indirect_;
    Parametric:Type type_;
    Word arg0_;

    Parametric(bool quoted) :
        Fragment(Fragment:Parametric, quoted),
        type_(Parametric:None)
    {
    }
};

struct Process :
    public Fragment
{
    bool input_;
    Vector<Logical> list_;

    Process(bool input) :
        Fragment(Fragment:Process, false),
        input_(input)
    {
    }
};

struct Test {
    Word pattern_;
    Vector<Logical> list_;
};

struct Case :
    public Command
{
    _R<Word> value_;
    Vector<Test> tests_;

    Case() :
        Command(Command:Case)
    {
    }
};

struct Compound :
    public Command
{
    Vector<Logical> list_;

    Compound() :
        Command(Command:Compound)
    {
    }
};

struct If:Elif {
    Vector<Logical> test_;
    Vector<Logical> then_;
};

struct If :
    public Command
{
    Vector<Logical> test_;
    Vector<Logical> then_;
    Vector<If:Elif> elifs_;
    Vector<Logical> else_;

    If() :
        Command(Command:If)
    {
    }
};

struct Condition {
    virtual bool Evaluate(bool &error) {
        _assert(false);
        return false;
    }
};

struct Conditional :
    public Command
{
    _R<Condition> condition_;

    Conditional(Condition *condition) :
        Command(Command:Conditional),
        condition_(condition)
    {
    }
};

struct Arithmetic {
    virtual int Evaluate() {
        _assert(false);
        return 0;
    }
};

struct ArithmeticFor :
    public Command
{
    _R<Arithmetic> start_;
    _R<Arithmetic> test_;
    _R<Arithmetic> next_;

    Vector<Logical> body_;

    ArithmeticFor() :
        Command(Command:ArithmeticFor)
    {
    }
};

struct IterativeFor :
    public Command
{
    _R<Word> name_;

    Vector<Word> arguments_;
    Vector<Logical> body_;

    IterativeFor() :
        Command(Command:IterativeFor)
    {
    }
};

struct Not :
    public Command
{
    _R<Command> command_;

    Not(Command *command) :
        Command(Command:Not),
        command_(command)
    {
    }
};

struct Function :
    public Command
{
    _R<Word> name_;
    _R<Command> command_;

    _finline Function() :
        Command(Command:Function)
    {
    }
};

struct Simple :
    public Command
{
    Vector<Assign> assigns_;
    Vector<Word> words_;

    Simple() :
        Command(Command:Simple)
    {
    }
};

struct Subshell :
    public Command
{
    Vector<Logical> list_;

    Subshell() :
        Command(Command:Subshell)
    {
    }
};

struct While :
    public Command
{
    bool inverse_;
    Vector<Logical> test_;
    Vector<Logical> body_;

    While(bool inverse) :
        Command(Command:While),
        inverse_(inverse)
    {
    }
};

struct Element :
    public Object
{
    String value_;

    _finline ~Element() {
    }
};

struct Variable :
    public Object
{
    _R<Element> value_;
    HashTable elements_;
    _R<Variable> last_;

    String export_;
    bool exported_;
    size_t environ_;

    _finline Variable() :
        elements_(6),
        exported_(false),
        environ_(_not(size_t))
    {
    }
};

_finline Variable **Shell:UpdateVariable(const String *name, bool create);

struct Scope {
    Vector<String> arguments_;
    Vector<String> locals_;

    size_t shift_;
    _R<Scope> last_;

    _finline Scope() :
        shift_(0)
    {
    }

    ~Scope() {
        Vector:ForEach(String, local, &locals_) {
            Variable **variable = Shell:UpdateVariable(local, false);
            _assert(variable);
            Variable *previous(*variable);
            _assert(previous);
            *variable = previous->last_.Release();
            drop(previous);
        }
    }
};

struct Mapping {
    Vector<void, true> map_;
    Mapping *next_;

    Mapping(Mapping *&next) :
        next_(next)
    {
        next = this;
    }
};

void Mapping:Backup(Vector<void, true> *mapping, int fd) {
    int result;
    if ((result = dup(fd)) == -1) {
        int code(errno);
        if (errno != EBADF)
            // XXX: more serious?
            Posix:PrintError(code, "dup");
    }

    mapping->InsertLast_((void *) result);
    mapping->InsertLast_((void *) fd);
}

enum Job:Status {
    Job:Running = 0,
    Job:Stopped = 1
};

struct Job {
    Job:Status status_;

    int group_;
    int last_;

    Vector<void, true> pids_;
    Vector<void, true> fds_;
    Vector<void, true> extra_;

    Job() :
        status_(Job:Running),
        group_(0),
        last_(0)
    {
    }
};

enum Break {
    Break:None = 0,
    Break:Cancel = 1,
    Break:Function = 2,
    Break:Loop = 3
};

struct Shell {
    HashTable variables_;
    HashTable functions_;

    Vector<void, true> environ_;
    Vector<void, true> exported_;

    _R<Job> job_;
    Vector<Job> jobs_;
    bool complete_;

    _R<String> arg0_;

    Vector<void, true> mapping_;
    _R<Scope> scope_;

    int status_;
    int pid_;

    int signal_;

    Break break_;
    int loop_;

    unsigned command_;

    _R<Variable> accelerated_[29];
    _R< Vector<Logical> > traps_[64];

    bool terminal_;

    bool set-bash_;
    bool set-ksh_;
    bool set-ignoreeof_;
    bool set-monitor_;
    bool set-noclobber_;
    bool set-posix_;

    bool shopt-accelerated_;
    bool shopt-failglob_;
    bool shopt-fallback_;
    bool shopt-noecho_;
    bool shopt-nullglob_;
    bool shopt-strict_;

    Shell() :
        variables_(10),
        functions_(5),
        job_(new Job()),
        complete_(false),
        arg0_(new String()),
        scope_(new Scope()),
        status_(0),
        pid_(sys:GetProcessId()),
        signal_(_not(int)),
        break_(Break:None),
        command_(0),

        terminal_(false),

        set-bash_(false),
        set-ksh_(false),
        set-monitor_(false),
        set-noclobber_(false),
        set-posix_(false),

        shopt-accelerated_(true),
        shopt-failglob_(false),
        shopt-fallback_(false),
        shopt-noecho_(false),
        shopt-nullglob_(false),
        shopt-strict_(false)
    {
        memset(accelerated_, 0, sizeof(accelerated_));
        environ_.InsertLast_(NULL);
    }
} shell_;

void Shell:Error_(const char *file, int line, const char *message) {
    writef2("<%d> %s(%d): %s\n", sys:GetProcessId(), file, line, message);
}

#define Shell:Error(message) \
    Shell:Error_(__FILE__, __LINE__, message)

enum {
    Accelerated:IFS = 0,
    Accelerated:PS1 = 1,
    Accelerated:PS2 = 2,
    Accelerated:PS3 = 3,
    Accelerated:PS4 = 4,
    Accelerated:PWD = 5,

    Accelerated:EUID = 6,
    Accelerated:HOME = 7,
    Accelerated:PATH = 8,
    Accelerated:PPID = 9,

    Accelerated:LINES = 10,
    Accelerated:REPLY = 11,
    Accelerated:SHELL = 12,
    Accelerated:SHLVL = 13,
    Accelerated:TMOUT = 14,

    Accelerated:CDPATH = 15,
    Accelerated:LINENO = 16,
    Accelerated:OLDPWD = 17,
    Accelerated:OPTARG = 18,
    Accelerated:OPTERR = 19,
    Accelerated:OPTIND = 20,
    Accelerated:RANDOM = 21,

    Accelerated:COLUMNS = 22,
    Accelerated:FIGNORE = 23,

    Accelerated:DIRSTACK = 24,
    Accelerated:HOSTNAME = 25,

    Accelerated:GLOBIGNORE = 26,
    Accelerated:PIPESTATUS = 27,
    Accelerated:TIMEFORMAT = 28

};

size_t Shell:FindAccelerated(const char *begin, size_t length) {
    switch (length) {
        case 3:
            switch (begin[2]) {
                case 'S':
                    if (!memcmp(begin, "IF", 2))
                        return Accelerated:IFS;
                break;

                case '1':
                    if (!memcmp(begin, "PS", 2))
                        return Accelerated:PS1;
                break;

                case '2':
                    if (!memcmp(begin, "PS", 2))
                        return Accelerated:PS2;
                break;

                case '3':
                    if (!memcmp(begin, "PS", 2))
                        return Accelerated:PS3;
                break;

                case '4':
                    if (!memcmp(begin, "PS", 2))
                        return Accelerated:PS4;
                break;

                case 'D':
                    if (!memcmp(begin, "PW", 2))
                        return Accelerated:PWD;
                break;
            }
        break;

        case 4:
            switch (begin[1]) {
                case 'U':
                    if (!memcmp(begin, "EUID", 4))
                        return Accelerated:EUID;
                break;

                case 'O':
                    if (!memcmp(begin, "HOME", 4))
                        return Accelerated:HOME;
                break;

                case 'A':
                    if (!memcmp(begin, "PATH", 4))
                        return Accelerated:PATH;
                break;

                case 'P':
                    if (!memcmp(begin, "PPID", 4))
                        return Accelerated:PPID;
                break;
            }
        break;

        case 5:
            switch (begin[2]) {
                case 'N':
                    if (!memcmp(begin, "LINES", 5))
                        return Accelerated:LINES;
                break;

                case 'P':
                    if (!memcmp(begin, "REPLY", 5))
                        return Accelerated:REPLY;
                break;

                case 'E':
                    if (!memcmp(begin, "SHELL", 5))
                        return Accelerated:SHELL;
                break;

                case 'L':
                    if (!memcmp(begin, "SHLVL", 5))
                        return Accelerated:SHLVL;
                break;

                case 'O':
                    if (!memcmp(begin, "TMOUT", 5))
                        return Accelerated:TMOUT;
                break;
            }
        break;

        case 6:
            switch (begin[5]) {
                case 'H':
                    if (!memcmp(begin + 1, "CDPAT", 5))
                        return Accelerated:CDPATH;
                break;

                case 'L':
                    if (!memcmp(begin + 1, "INENO", 5))
                        return Accelerated:LINENO;
                break;

                case 'G':
                    if (!memcmp(begin + 1, "OPTAR", 5))
                        return Accelerated:OPTARG;
                break;

                case 'R':
                    if (!memcmp(begin + 1, "OPTER", 5))
                        return Accelerated:OPTERR;
                break;

                case 'D':
                    switch (begin[4]) {
                        case 'W':
                            if (!memcmp(begin + 1, "OLDP", 4))
                                return Accelerated:OLDPWD;
                        break;

                        case 'N':
                            if (!memcmp(begin + 1, "OPTI", 4))
                                return Accelerated:OPTIND;
                        break;
                    }
                break;

                case 'M':
                    if (!memcmp(begin + 1, "RANDO", 5))
                        return Accelerated:RANDOM;
                break;
            }
        break;

        case 7:
            switch (begin[0]) {
                case 'C':
                    if (!memcmp(begin + 1, "OLUMNS", 6))
                        return Accelerated:COLUMNS;
                break;

                case 'F':
                    if (!memcmp(begin + 1, "IGNORE", 6))
                        return Accelerated:FIGNORE;
                break;
            }
        break;

        case 8:
            switch (begin[0]) {
                case 'D':
                    if (!memcmp(begin, "IRSTACK", 7))
                        return Accelerated:DIRSTACK;
                break;

                case 'H':
                    if (!memcmp(begin, "OSTNAME", 7))
                        return Accelerated:HOSTNAME;
                break;
            }
        break;

        case 10:
            switch (begin[0]) {
                case 'G':
                    if (!memcmp(begin + 1, "LOBIGNORE", 9))
                        return Accelerated:GLOBIGNORE;
                break;

                case 'P':
                    if (!memcmp(begin + 1, "IPESTATUS", 9))
                        return Accelerated:PIPESTATUS;
                break;

                case 'T':
                    if (!memcmp(begin + 1, "IMEFORMAT", 9))
                        return Accelerated:TIMEFORMAT;
                break;
            }
        break;
    }

    return _not(size_t);
}

Variable **Shell:UpdateVariable(const char *name-begin, size_t name-length, bool create) {
    size_t accelerated = Shell:FindAccelerated(name-begin, name-length);
    return accelerated == _not(size_t) ?
        reinterpret_cast<Variable **>(shell_.variables_.Set(name-begin, name-length, create)) :
        &shell_.accelerated_[accelerated].GetValue();
}

_finline Variable **Shell:UpdateVariable(const String *name, bool create) {
    return Shell:UpdateVariable(name->data_, name->count_, create);
}

Variable *Shell:GetVariable(const char *name-begin, size_t name-length) {
    Variable **variable(Shell:UpdateVariable(name-begin, name-length, false));
    return variable ? *variable : NULL;
}

Variable *Shell:SetVariable(const String *name) {
    Variable *&variable = *Shell:UpdateVariable(name, true);
    if (!variable)
        variable = new Variable();
    return variable;
}

void Shell:Reexport_(size_t name-size, Variable *variable) {
    if (variable->environ_ != _not(size_t)) {
        variable->export_.count_ = name-size + 1;
        variable->export_.CopyStringLast(&variable->value_->value_);
        shell_.environ_.data_[variable->environ_] = variable->export_.data_;
    }
}

void Shell:Export(String *name, Variable *variable, char *begin = NULL, size_t length = 0) {
    _assert(variable);

    if (variable->environ_ != _not(size_t))
        Shell:Reexport_(name->count_, variable);
    else {
        variable->exported_ = true;

        if (variable->value_) {
            variable->environ_ = shell_.exported_.count_;
            shell_.environ_.InsertLast_(NULL);
            shell_.exported_.InsertLast(variable);

            if (begin) {
                variable->export_.data_ = begin;
                variable->export_.count_ = length;
                shell_.environ_.data_[variable->environ_] = begin;
            } else {
                variable->export_.Reserve(name->count_ + variable->value_->value_.count_ + 1);
                variable->export_.CopyRangeLast(name->data_, name->count_);
                variable->export_.InsertLast('=');
                Shell:Reexport_(name->count_, variable);
            }
        }
    }
}

Element *&Variable:UpdateElement(Variable *variable, const char *begin, size_t length) {
    return
        !begin || length == 1 && begin[0] == '0' ?
            variable->value_.GetValue() :
            *reinterpret_cast<Element **>(variable->elements_.Set(begin, length, true));
}

void Shell:UnsetVariable(const char *name-begin, size_t name-length, const char *subscript-begin, size_t subscript-length) {
    if (Variable *variable = Shell:GetVariable(name-begin, name-length)) {
        //XXX: this needs to handle exported variables!!
        Element *&element = Variable:UpdateElement(variable, subscript-begin, subscript-length);
        drop(element);
        element = NULL;
    }
}

void Shell:SetElement(Variable *variable, String *name, String *subscript, String *value, bool _export) {
    if (!variable)
        variable = Shell:SetVariable(name);

    const char *subscript-begin;
    size_t subscript-length;

    if (subscript) {
        subscript-begin = subscript->data_;
        subscript-length = subscript->count_;
    } else {
        subscript-begin = NULL;
        subscript-length = 0;
    }

    Element *&element(Variable:UpdateElement(variable, subscript-begin, subscript-length));
    if (!element)
        element = new Element();

    element->value_.AssignString(value);
    if (_export || !subscript && variable->exported_)
        Shell:Export(name, variable);
}

String *Shell:GetAcceleratedZero(size_t accelerated) {
    Variable *variable(shell_.accelerated_[accelerated]);
    return variable && variable->value_ ? &variable->value_->value_ : NULL;
}

Variable *Shell:SetAccelerated(size_t accelerated) {
    Variable *&variable = shell_.accelerated_[accelerated];
    if (!variable)
        variable = new Variable();
    return variable;
}

String *Variable:SetZero(Variable *variable) {
    if (!variable->value_)
        variable->value_ = new Element();
    return &variable->value_->value_;
}

void Shell:ShareAccelerated_(size_t accelerated, char *begin, size_t length) {
    Variable *&variable = shell_.accelerated_[accelerated];

    if (variable == NULL) {
        variable = new Variable();
        variable->value_ = new Element();
        variable->value_->value_.Share_(begin, length);
    }
}

bool Shell:Initialize(char *envp[]) {
    for (char **env = envp; *env != NULL; ++env) {
        size_t length = strlen(*env);
        char *equal = (char *) memchr(*env, '=', length);
        _assert(equal);

        String name;
        name.count_ = equal - *env;
        name.data_ = *env;

        Variable *variable(Shell:SetVariable(&name));
        variable->value_ = new Element();
        variable->value_->value_.Share(equal + 1);
        Shell:Export(&name, variable, *env, length);
    }

    {
        Variable *variable(Shell:SetAccelerated(Accelerated:EUID));
        UnparseDecimal(Variable:SetZero(variable), ::geteuid());
        Shell:Reexport_(4, variable);
    }

    {
        Variable *variable(Shell:SetAccelerated(Accelerated:HOSTNAME));
        if (!variable->value_) {
            // XXX: bad buffer
            char buffer[256];
            _syscall(::gethostname(buffer, sizeof(buffer)));
            Variable:SetZero(variable)->AssignRange(buffer, strlen(buffer));
        }
    }

    Shell:ShareAccelerated_(Accelerated:IFS, " \t\n", 3);

#ifndef CFG_NO_INTERACTIVE
    // XXX: process.sh didn't parse this right
    Shell:ShareAccelerated_(Accelerated:PS1, "\\""$ ", 3);
    Shell:ShareAccelerated_(Accelerated:PS2, "> ", 2);
#endif

    {
        Variable *variable(Shell:SetAccelerated(Accelerated:SHELL));
        Variable:SetZero(variable)->Share_("3sh", 3);
        Shell:Reexport_(5, variable);
    }

    {
        /* XXX: this should do something awesome with PWD for non-physical cwd */
        Variable *variable(Shell:SetAccelerated(Accelerated:PWD));
        String *pwd(Variable:SetZero(variable));
        if (!System:GetCurrentDirectory(pwd))
            return false;
    }

    {
        Variable *variable(Shell:SetAccelerated(Accelerated:SHLVL));

        if (!variable->value_)
            Variable:SetZero(variable)->Share_("1", 1);
        else {
            int level;

            String *value(&variable->value_->value_);
            if (!ParseInteger(value->data_, value->count_, 10, &level))
                value->Share_("1", 1);
            else {
                value->Clear();
                UnparseDecimal(value, level + 1);
            }
        }

        Shell:Reexport_(5, variable);
    }

    return true;
}

int Shell:Fork(bool split) {
    //writef("<%d>Shell::Fork_(%b)\n", ::getpid(), split);

    int pid;
#ifdef CFG_USE_SYS_FORK
    _syscall(pid = syscall(SYS_fork));
#else
    _syscall(pid = ::fork());
#endif

    if (!(split ^ (bool) pid)) {
        Vector<void, true> &fds(shell_.job_->fds_);
        for (size_t i(0); i != fds.count_; ++i)
            _syscall(close(reinterpret_cast<intptr_t>(fds[i])));
        fds.count_ = 0;
    }

    if (pid) {
        //writef("<%d>Shell::Fork_() = %d\n", ::getpid(), pid);
        return pid;
    } else {
        {
            int *data((int *) shell_.mapping_.data_);
            for (size_t i(0); i != shell_.mapping_.count_; i += 2)
                if (data[i] != -1)
                    _syscall(close(data[i]));
            shell_.mapping_.count_ = 0;
        }

        if (shell_.set-monitor_) {
            if (::setpgid(0, shell_.job_->group_) == -1) {
                Posix:PrintError(errno, "setpgid");
                _trace();
                _exit(1);
            }

            shell_.set-monitor_ = false;
        }

        // XXX: clear the job, don't delete it
        shell_.job_.Drop();
        shell_.job_ = new Job();

        // XXX: unset traps!!

        return 0;
    }
}

bool Shell:Split(bool complete) {
    //writef("<%d>Shell::Split(%b)\n", ::getpid(), complete);
    if (complete)
        return false;

    Job *job(shell_.job_);

    if (int pid = Shell:Fork(true)) {
        if (job->group_) {
            job->pids_.InsertLast(reinterpret_cast<void *>(job->last_));
            job->last_ = pid;
        } else {
            job->group_ = pid;
            job->last_ = pid;
        }

        return true;
    } else {
        shell_.complete_ = true;
        return false;
    }
}

_finline bool Shell:Split() {
    return Shell:Split(shell_.complete_);
}

void Shell:Reap(Job *job) {
    for (size_t i(0); i != job->extra_.count_; ++i)
        _syscall(sys:WaitProcessId(reinterpret_cast<intptr_t>(job->extra_.data_[i]), NULL, 0));
    job->extra_.count_ = 0;
}

void Execute-List(const Vector<Logical> *list);

void Shell:Interrupt(int signal) {
    shell_.break_ = Break:Cancel;
}

void Shell:Trap() {
    if (shell_.signal_ != _not(int)) {
        int signal(shell_.signal_);
        shell_.signal_ = _not(int);

        Job *job(shell_.job_);
        shell_.job_ = new Job();
        Execute-List(shell_.traps_[signal]);
        shell_.job_.Drop();
        shell_.job_ = job;
    }
}

void Shell:Activate() {
    if (shell_.terminal_ && shell_.set-monitor_) {
        Signal_(SIGTTIN, SIG_IGN);
        Signal_(SIGTTOU, SIG_IGN);

        if (::tcsetpgrp(0, ::getpgrp())) {
            Posix:PrintError(errno, "tcsetpgrp");
            shell_.terminal_ = false;
        }
    }
}


void Shell:Async() {
    shell_.jobs_.InsertLast(shell_.job_);
    shell_.job_ = new Job();
}

void Shell:Continue() {
    Job *job(shell_.job_);

    if (job->status_ == Job:Stopped) {
        if (!Kill_(-job->group_, SIGCONT))
            return;
        job->status_ = Job:Running;
    }
}

bool Shell:Wait(int pid) {
  wait:
    int status;
    int result(sys:WaitProcessId(pid, &status, WUNTRACED));
    if (result == -1) {
        int code(errno);
        if (code == EINTR) {
            Shell:Trap();
            goto wait;
        }

        Posix:PrintError(code, "waitpid");
        _trace();
        _exit(1);
    }

    if (WIFSIGNALED(status))
        shell_.status_ = WTERMSIG(status) + 128;
    else if (WIFEXITED(status))
        shell_.status_ = WEXITSTATUS(status);
    else if (WIFSTOPPED(status))
        return false;
    else _assert(false);

    return true;
}

// XXX: examine this more closely
// XXX: if the last process ends fast and you background and...
void Shell:Sync(bool foreground = true) {
    Job *job(shell_.job_);

    if (
        foreground &&
        job->group_ &&
        shell_.terminal_ &&
        shell_.set-monitor_
    ) {
        Signal_(SIGTTIN, SIG_IGN);
        Signal_(SIGTTOU, SIG_IGN);

        if (::tcsetpgrp(0, job->group_) == -1)
            Posix:PrintError(errno, "tcsetpgrp");
    }

    Shell:Continue();

    Vector<void, true> &pids(job->pids_);
    while (pids.count_)
        if (Shell:Wait(reinterpret_cast<intptr_t>(pids.Last())))
            --pids.count_;
        else stopped: {
            job->status_ = Job:Stopped;
            Shell:Activate();
            Shell:Async();
            return;
        }

    if (job->last_) {
        if (!Shell:Wait(job->last_))
            goto stopped;
        job->last_ = 0;
    }

    for (size_t i(0); i != job->extra_.count_; ++i)
        _syscall(waitpid(reinterpret_cast<intptr_t>(job->extra_.data_[i]), NULL, 0));
    job->extra_.count_ = 0;

    if (foreground && job->group_)
        Shell:Activate();
    job->group_ = 0;
}

void Shell:Switch(int pid) {
    for (size_t i(0); i != shell_.jobs_.count_; ++i) {
        Job *&job(shell_.jobs_[i]);
        if (job->group_ == pid) {
            shell_.job_.Drop();
            shell_.job_ = job;
            job = shell_.jobs_.Last();
            --shell_.jobs_.count_;
            return;
        }
    }
}

void Shell:Handle(int signal) {
    Signal_(signal, &Shell:Handle);
    shell_.signal_ = signal;
}

String *Word:IsUnquotedLiteral(Word *_this) {
    if (_this->fragments_.count_ == 1) {
        Fragment *fragment = _this->fragments_[0];
        if (fragment->type_ == Fragment:Literal && !fragment->quoted_)
            return &static_cast<Literal *>(fragment)->value_;
    }

    return NULL;
}

typedef enum {
    Terminator:None = 0,
    Terminator:Error = 1,

    Terminator:Alias = 2,
    Terminator:NewLine = 3,
    Terminator:Script = 4,
    Terminator:Semicolon = 5,

    Terminator:Ampersand = 6,
    Terminator:AmpersandAmpersand = 7,
    Terminator:Equal = 8,
    Terminator:EqualEqual = 9,
    Terminator:EqualTilde = 10,
    Terminator:LeftAngle = 11,
    Terminator:LeftCurly = 12,
    Terminator:LeftRound = 13,
    Terminator:LeftSquareLeftSquare = 14,
    Terminator:Not = 15,
    Terminator:NotEqual = 16,
    Terminator:Pipe = 17,
    Terminator:PipePipe = 18,
    Terminator:RightAngle = 19,
    Terminator:RightCurly = 20,
    Terminator:RightRound = 21,
    Terminator:RightRoundRightRound = 22,
    Terminator:RightSquareRightSquare = 23,
    Terminator:SemicolonSemicolon = 24,

    Terminator:Case = 25,
    Terminator:Do = 26,
    Terminator:Done = 27,
    Terminator:Elif = 28,
    Terminator:Else = 29,
    Terminator:Esac = 30,
    Terminator:Fi = 31,
    Terminator:For = 32,
    Terminator:Function = 33,
    Terminator:If = 34,
    Terminator:In = 35,
    Terminator:Select = 36,
    Terminator:Then = 37,
    Terminator:Until = 38,
    Terminator:While = 39
} Parser:Terminator;

const char *Terminators_[] = {
    "<nothing>", "<parse error>", "<end\-of\-alias>",
    LQ_"\\n"RQ_, "<end\-of\-script>", LQ_";"RQ_, LQ_"&"RQ_,
    LQ_"&&"RQ_, LQ_"="RQ_, LQ_"=="RQ_, LQ_"=~"RQ_, LQ_"<"RQ_,
    LQ_"{"RQ_, LQ_"("RQ_, LQ_"[["RQ_, LQ_"!"RQ_, LQ_"!="RQ_,
    LQ_"|"RQ_, LQ_"||"RQ_, LQ_">"RQ_, LQ_"}"RQ_, LQ_")"RQ_,
    LQ_"))"RQ_, LQ_"]]"RQ_, LQ_";;"RQ_, LQ_"case"RQ_,
    LQ_"do"RQ_, LQ_"done"RQ_, LQ_"elif"RQ_, LQ_"else"RQ_,
    LQ_"esac"RQ_, LQ_"fi"RQ_, LQ_"for"RQ_, LQ_"function"RQ_,
    LQ_"if"RQ_, LQ_"in"RQ_, LQ_"select"RQ_, LQ_"then"RQ_,
    LQ_"until"RQ_, LQ_"while"RQ_
};

_finline bool IsCommandTerminator(intptr_t token) {
    return token == Terminator:Semicolon || token == Terminator:NewLine;
}

typedef enum {
    Reserve:File,
    Reserve:Range,
    Reserve:Terminal
} Reserve:Type;

struct Reserve {
    Reserve:Type type_;
    const char *name_;
    unsigned command_;

    Reserve(Reserve:Type type, const char *name) :
        type_(type),
        name_(name),
        command_(0)
    {
    }
};

struct File :
    public Reserve
{
    int fd_;
    String buffer_;

    File(const char *name, int fd) :
        Reserve(Reserve:File, name),
        fd_(fd)
    {
    }

    File(int fd) :
        Reserve(Reserve:File, NULL),
        fd_(fd)
    {
    }
};

struct Range :
    public Reserve
{
    char *begin_;
    char *end_;

    Range(const char *name, char *begin, char *end) :
        Reserve(Reserve:Range, name),
        begin_(begin),
        end_(end)
    {
    }
};

#ifndef CFG_NO_INTERACTIVE
struct Terminal :
    public Reserve
{
    Prompt prompt_;

    Terminal(const char *name, int fin, int fout) :
        Reserve(Reserve:Terminal, name),
        prompt_(fin, fout)
    {
    }
};
#endif

void Reserve:Request(Reserve *_this);

struct Parser {
    Reserve *reserve_;
    bool primary_;

    String data_;
    const char *current_;

    unsigned line_;
    _R<Word> token_;

    Parser(Reserve *reserve) :
        reserve_(reserve),
        primary_(false),
        current_(NULL),
        line_(0)
    {
    }

    _finline operator bool() {
        return current_ != data_.data_ + data_.count_;
    }
};

void Parser:PrintLine(Parser *_this, bool mark) {
    const char *begin = _this->data_.data_;
    const char *end = _this->data_.data_ + _this->data_.count_;
    if (begin != end && end[-1] == '\n')
        --end;

    size_t size = CountDigits(_this->line_);
    const char *name(_this->reserve_->name_);
    size_t length(strlen(name));

    writef2("%S: line %d: "LQ_"%S"RQ_"\n", name, length, _this->line_, begin, end - begin);

    if (mark)
        writef2("%r%r/\n", ' ', length + 9 + size, '-', _this->current_ - _this->data_.data_);
}

bool Parser:Request(Parser *_this, const char *file, int line) {
    //writef("%s(%d): Parser:Request()\n", file, line);

    ++_this->line_;

    Reserve *reserve = _this->reserve_;
    switch (reserve->type_) {
        // XXX: maybe ring buffer _this sucker?
        case Reserve:File: {
            File *file = (File *) reserve;
            memmove(file->buffer_.data_, file->buffer_.data_ + _this->data_.count_, file->buffer_.count_ -= _this->data_.count_);

            const char *newline = (const char *) memchr(file->buffer_.data_, '\n', file->buffer_.count_);

            check: if (newline) {
              newline:
                _this->data_.data_ = file->buffer_.data_;
                _this->data_.count_ = newline + 1 - _this->data_.data_;
            } else if (file->fd_ != -1) {
                size_t count = file->buffer_.count_;
                file->buffer_.Increase(BufferSize_);

                int length;
                _syscall(length = sys:Read(file->fd_, file->buffer_.data_, BufferSize_));

                if (!length) {
                    file->fd_ = -1;
                    if (!file->buffer_.count_)
                        goto fail;
                    newline = file->buffer_.data_ + file->buffer_.count_ - 1;
                    goto newline;
                }

                file->buffer_.count_ += length;
                newline = (const char *) memchr(file->buffer_.data_ + count, '\n', length);
                goto check;
            } else goto fail;
        } break;

        case Reserve:Range: {
            Range *range = (Range *) reserve;
            _this->data_.data_ = range->begin_;

            char *end = (char *) memchr(range->begin_, '\n', range->end_ - range->begin_);
            end = end ? end + 1 : range->end_;

            range->begin_ = end;
            _this->data_.count_ = end - _this->data_.data_;
        } break;

#ifndef CFG_NO_INTERACTIVE
        case Reserve:Terminal: {
            Terminal *terminal = static_cast<Terminal *>(reserve);

            String text;

            Variable *PS(shell_.accelerated_[
                _this->primary_ ?
                    Accelerated:PS1 :
                    Accelerated:PS2
            ]);

            if (!PS || !PS->value_)
                text.Share_("3sh$ ", 5);
            else {
                const char *begin(PS->value_->value_.data_);
                const char *end(begin + PS->value_->value_.count_);

                while (begin != end) {
                    const char *slash(reinterpret_cast<const char *>(memchr(begin, '\\', end - begin)));
                    if (!slash) {
                        text.CopyRangeLast(begin, end - begin);
                        break;
                    } else {
                        text.CopyRangeLast(begin, slash - begin);
                        begin = slash + 1;

                        if (begin != end)
                            if (*begin >= '0' && *begin <= '7')
                                if (end - begin < 3)
                                    break;
                                else {
                                    int value;
                                    if (!ParseInteger(begin, 3, 8, &value))
                                        break;
                                    else {
                                        text.InsertLast(value);
                                        begin += 3;
                                    }
                                }
                            else switch (*begin++) {
                                case 'a': text.InsertLast('\x07'); break;

                                case 'd': break;
                                case 'D': break;
                                case 'e': break;

                                case 'h':
                                    if (String *HOSTNAME = Shell:GetAcceleratedZero(Accelerated:HOSTNAME)) {
                                        if (const char *dot = reinterpret_cast<const char *>(memchr(HOSTNAME->data_, '.', HOSTNAME->count_)))
                                            text.CopyRangeLast(HOSTNAME->data_, dot - HOSTNAME->data_);
                                        else
                                            text.CopyStringLast(HOSTNAME);
                                    }
                                break;

                                case 'H':
                                    if (String *HOSTNAME = Shell:GetAcceleratedZero(Accelerated:HOSTNAME))
                                        text.CopyStringLast(HOSTNAME);
                                break;

                                case 'j': UnparseDecimal(&text, shell_.jobs_.count_); break;

                                case 'l': {
                                    /* // XXX: I'm really starting to hate GNU
                                               oh, that's right... I already _did_
                                    char buffer[BufferSize_] = {'\0'};
                                    // XXX: better buffer management?
                                    if (!::ttyname_r(0, buffer, sizeof(buffer))) {
                                        size_t size(strlen(buffer));
                                        const char *slash(reinterpret_cast<const char *>(FindLast(buffer, '/', size)));
                                        slash = slash ? slash + 1 : buffer;
                                        text.CopyRangeLast(slash, buffer + size - slash);
                                    }*/
                                    text.CopyRangeLast("-1", 2);
                                } break;

                                case 'n': text.InsertLast('\n'); break;
                                case 'r': text.InsertLast('\r'); break;

                                case 's': {
                                    String &arg0(*shell_.arg0_);
                                    const char *slash(reinterpret_cast<const char *>(FindLast(arg0.data_, '/', arg0.count_)));
                                    slash = slash ? slash + 1 : arg0.data_;
                                    text.CopyRangeLast(slash, arg0.data_ + arg0.count_ - slash);
                                } break;

                                case 't': break;
                                case 'T': break;
                                case '@': break;
                                case 'A': break;

                                case 'u': {
                                    // XXX: let it be known that here, saurik the Avatar was defeated in battle against the GNU C Standard Library
                                    text.CopyRangeLast("anubis", 6);
                                } break;

                                case 'v': text.CopyRangeLast("0.90", 4); break;
                                case 'V': text.CopyRangeLast("0.90.0", 6); break;

                                case 'w':
                                    if (String *PWD = Shell:GetAcceleratedZero(Accelerated:PWD)) {
                                        size_t count(PWD->count_);
                                        if (count > 1 && PWD->data_[count - 1] == '/')
                                            --count;
                                        if (!count)
                                            break;

                                        if (String *HOME = Shell:GetAcceleratedZero(Accelerated:HOME)) {
                                            size_t size(HOME->count_);
                                            if (size > 1 && HOME->data_[size - 1] == '/')
                                                --size;

                                            if (
                                                size && (count == size || count > size && PWD->data_[size] == '/') &&
                                                !memcmp(PWD->data_, HOME->data_, size)
                                            ) {
                                                char temp(PWD->data_[size - 1]);
                                                PWD->data_[size - 1] = '~';
                                                text.CopyRangeLast(PWD->data_ + size - 1, count - (size - 1));
                                                PWD->data_[size - 1] = temp;
                                            } else goto homeless;
                                        } else homeless:
                                            text.CopyStringLast(PWD);
                                    }
                                break;

                                case 'W':
                                    if (String *PWD = Shell:GetAcceleratedZero(Accelerated:PWD)) {
                                        size_t count(PWD->count_);

                                        if (!count)
                                            break;
                                        else if (PWD->data_[count - 1] == '/')
                                            if (count != 1)
                                                --count;
                                            else {
                                                text.CopyRangeLast("/", 1);
                                                break;
                                            }

                                        const char *slash(reinterpret_cast<const char *>(FindLast(PWD->data_, '/', count)));
                                        slash = slash ? slash + 1 : PWD->data_;
                                        text.CopyRangeLast(slash, PWD->data_ + count - slash);
                                    }
                                break;

                                case '!': break;

                                case '#': UnparseDecimal(&text, reserve->command_); break;
                                case '$': text.InsertLast(::geteuid() ? '$' : '#'); break;
                                case '\\': text.InsertLast('\\'); break;

                                case '[': break;
                                case ']': break;

                                default:
                                break;
                            }
                    }
                }
            }

            // XXX: calculate exact width
            if (!Prompt:Prompt(&terminal->prompt_, text, text.count_, _this->data_))
                goto fail;
        } break;
#endif

        default:
            _assert(false);
        break;
    }

    _this->current_ = _this->data_.data_;
    //Parser:PrintLine(_this, false);
    return true;

  fail:
    _this->current_ = _this->data_.data_;
    _this->data_.count_ = 0;
    return false;
}

#define Parser:Request(parser) Parser:Request(parser, __FILE__, __LINE__)

void Parser:OutputTerminator_(Parser *_this) {
    if (IsReservedPointer(_this->token_))
        writef2("%s", Terminators_[_this->token_.operator intptr_t()]);
    else
        writef2("<!%d:%b>", _this->token_->fragments_.count_, Word:IsUnquotedLiteral(_this->token_));
}

void Parser:Error_(Parser *_this, const char *file, int line, bool terminator, const char *format, ...) {
    va_list arg;
    va_start(arg, format);

    Parser:PrintLine(_this, true);

    writef2("%s(%d): ", file, line);
    writef2("parse error: ", file, line);

    vfwritef(2, format, arg);
    va_end(arg);

    if (terminator) {
        Parser:OutputTerminator_(_this);
        writef2("\n");
    }

    _this->token_ = Terminator:Error;
}

#define Parser:Error1(parser, arg0) Parser:Error_(parser, __FILE__, __LINE__, false, arg0)
#define Parser:Error2(parser, arg0, arg1) Parser:Error_(parser, __FILE__, __LINE__, false, arg0, arg1)
#define Parser:Error3(parser, arg0, arg1, arg2) Parser:Error_(parser, __FILE__, __LINE__, false, arg0, arg1, arg2)
#define Parser:Error4(parser, arg0, arg1, arg2, arg3) Parser:Error_(parser, __FILE__, __LINE__, false, arg0, arg1, arg2, arg3)
#define Parser:Error5(parser, arg0, arg1, arg2, arg3, arg4) Parser:Error_(parser, __FILE__, __LINE__, false, arg0, arg1, arg2, arg3, arg4)

void Parser:Expected(Parser *_this, Parser:Terminator terminator, const char *file, int line) {
    if (_this->token_ != Terminator:Error)
        Parser:Error_(_this, file, line, true, "expected %s, found ", Terminators_[terminator]);
}

void Parser:Unexpected(Parser *_this, const char *file, int line) {
    if (_this->token_ != Terminator:Error)
        Parser:Error_(_this, file, line, true, "unexpected ", file, line);
}

#define Parser:Expected_(parser, terminator) \
    Parser:Error3(parser, "expected %S\n", LQ_ terminator RQ_, sizeof(LQ_) - 1 + sizeof(terminator) - 1 + sizeof(RQ_) - 1)
#define Parser:Expected(parser, terminator) \
    Parser:Expected(parser, terminator, __FILE__, __LINE__)
#define Parser:Unexpected(parser) \
    Parser:Unexpected(parser, __FILE__, __LINE__)

#define Parser:Expect(parser, terminator) \
    if (parser->token_ != terminator) { \
        Parser:Expected(parser, terminator); \
        return NULL; \
    }

bool Parser:Assert(Parser *_this) {
    if (*_this)
        return true;
    else {
        _this->token_ = Terminator:Script;
        Parser:Unexpected(_this);
        return false;
    }
}

void Execute-Parser(Parser *parser);

void Execute-Reserve(Reserve *reserve) {
    Parser parser(reserve);
    Execute-Parser(&parser);
}

void Execute-File(const char *name, int fd) {
    File file(name, fd);
    Execute-Reserve(&file);
}

void Execute-Range(const char *name, char *begin, size_t size) {
    Range range(name, begin, begin + size);
    Execute-Reserve(&range);
}

bool Parse-Range(const char *name, char *begin, size_t size, Vector<Logical> &list);

void Shell:Evaluate(String *code) {
    Execute-Range("<eval>", code->data_, code->count_);
}

_finline bool IsStatus(int status, bool success) {
    return success ? status == 0 : status != 0;
}

const char *Signal-Names_[] = {
    "0", "HUP", "INT", "QUIT", "BUS", "FPE", "KILL", "USR1",
    "SEGV", "USR2", "PIPE", "ALRM", "TERM", "CHLD", "CONT",
    "STOP", "TSTP", "TTIN", "TTOU", "URG", "XCPU", "XFSZ",
    "VTARLM", "PROF", "WINCH", "IO", "PWR", "SYS"
};

int Get-Signal(const char *begin, size_t length) {
    if (length >= 3 && !memcmp(begin, "SIG", 3)) {
        begin += 3;
        length -= 3;
    }

    switch (length) {
        case 1:
            if (begin[0] == '0')
                return 0;
        break;

        case 2:
            if (!memcmp(begin, "IO", 2))
                return SIGIO;
        break;

        case 3:
            switch (begin[0]) {
                case 'B':
                    if (!memcmp(begin + 1, "US", 2))
                        return SIGBUS;
                break;

                case 'F':
                    if (!memcmp(begin + 1, "PE", 2))
                        return SIGFPE;
                break;

                case 'H':
                    if (!memcmp(begin + 1, "UP", 2))
                        return SIGHUP;
                break;

                case 'I':
                    if (!memcmp(begin + 1, "NT", 2))
                        return SIGINT;
                break;

                case 'U':
                    if (!memcmp(begin + 1, "RG", 2))
                        return SIGURG;
                break;


#ifdef SIGPWR
                case 'P':
                    if (!memcmp(begin + 1, "WR", 2))
                        return SIGPWR;
                break;
#endif

                case 'S':
                    if (!memcmp(begin + 1, "YS", 2))
                        return SIGSYS;
                break;
            }
        break;

        case 4:
            switch (begin[3]) {
                case 'M':
                    switch (begin[0]) {
                        case 'A':
                            if (!memcmp(begin + 1, "LR", 2))
                                return SIGALRM;
                        break;

                        case 'T':
                            if (!memcmp(begin + 1, "ER", 2))
                                return SIGTERM;
                        break;
                    }
                break;

                case 'D':
                    if (!memcmp(begin, "CHL", 3))
                        return SIGCHLD;
                break;

                case 'T':
                    switch (begin[0]) {
                        case 'C':
                            if (!memcmp(begin + 1, "ON", 2))
                                return SIGCONT;
                        break;

                        case 'Q':
                            if (!memcmp(begin + 1, "UI", 2))
                                return SIGQUIT;
                        break;
                    }
                break;

                case 'L':
                    if (!memcmp(begin, "KIL", 3))
                        return SIGKILL;
                break;

                case 'E':
                    if (!memcmp(begin, "PIP", 3))
                        return SIGPIPE;
                break;

                case 'F':
                    if (!memcmp(begin, "PRO", 3))
                        return SIGPROF;
                break;

                case 'V':
                    if (!memcmp(begin, "SEG", 3))
                        return SIGSEGV;
                break;

                case 'P':
                    switch (begin[0]) {
                        case 'S':
                            if (!memcmp(begin + 1, "TO", 2))
                                return SIGSTOP;
                        break;

                        case 'T':
                            if (!memcmp(begin + 1, "ST", 2))
                                return SIGTSTP;
                        break;
                    }
                break;

                case 'N':
                    if (!memcmp(begin, "TTI", 3))
                        return SIGTTIN;
                break;

                case 'U':
                    switch (begin[0]) {
                        case 'T':
                            if (!memcmp(begin + 1, "TO", 2))
                                return SIGTTOU;
                        break;

                        case 'X':
                            if (!memcmp(begin + 1, "CP", 2))
                                return SIGXCPU;
                        break;
                    }
                break;

                case '1':
                    if (!memcmp(begin, "USR", 3))
                        return SIGUSR1;
                break;

                case '2':
                    if (!memcmp(begin, "USR", 3))
                        return SIGUSR2;
                break;

                case 'Z':
                    if (!memcmp(begin, "XFS", 3))
                        return SIGXFSZ;
                break;
            }
        break;

        case 5:
            if (!memcmp(begin, "WINCH", 5))
                return SIGWINCH;
        break;

        case 6:
            if (!memcmp(begin, "VTARLM", 6))
                return SIGVTALRM;
        break;
    }

    return _not(int);
}

bool Parse-Signal(const char *begin, size_t length, int &signal) {
    if (!ParseInteger(begin, length, 10, &signal)) {
        if ((signal = Get-Signal(begin, length)) != _not(int))
            return true;

        writef(LQ_"%S"RQ_" is not a signal\n", begin, length);
        return false;
    }

    return true;
}

typedef int (*BuiltIn)();

int Special-DeclareExport(bool _default, bool local, bool _export) {
    Vector<String> &arguments(shell_.scope_->arguments_);
    size_t shift(shell_.scope_->shift_);

    Scope *last(shell_.scope_->last_);

    for (size_t i(shift); i != arguments.count_; ++i) {
        String *assign(arguments[i]);
        char *equal(reinterpret_cast<char *>(::memchr(assign->data_, '=', assign->count_)));

        _R<String> name(new String());
        name->CopyRangeLast(assign->data_, equal ? equal - assign->data_ : assign->count_);

        Variable **variable(Shell:UpdateVariable(name, true));

        if (local) {
            Variable *local(new Variable());
            local->last_ = *variable;
            *variable = local;

            last->locals_.InsertLast(name.Release());
        } else if (!*variable)
            *variable = new Variable();

        String value;
        if (equal) {
            value.data_ = equal + 1;
            value.count_ = assign->data_ + assign->count_ - value.data_;
            Shell:SetElement(*variable, name, NULL, &value, _export);
        } else if (_default)
            Shell:SetElement(*variable, name, NULL, &value, _export);
        else if (_export)
            Shell:Export(name, *variable);
    }

    return 0;
}

bool Execute-Path(const char *path);

// Special Commands {{{
// break {{{
int special-break() {
    Vector<String> &arguments(shell_.scope_->arguments_);
    size_t shift(shell_.scope_->shift_);

    Options options(arguments, shift, $(./codes $''));
    while (int option = options.Parse())
        switch (option) {
            default: goto usage; break;
        }

    if (false) usage: {
        writef2("usage: break <count>?\n");
        return 1;
    }

    if (arguments.count_ - shift == 0)
        shell_.loop_ = -1;
    else if (arguments.count_ - shift == 1) {
        String *count(arguments[shift]);

        int count-int;
        if (!ParseDecimal(count->data_, count->count_, &count-int))
            return 1;
        if (count-int < 0) {
            Shell:Error("negative count");
            return 1;
        }

        shell_.loop_ = -count-int;
    } else goto usage;

    shell_.break_ = Break:Loop;
    return 0;
}
// }}}
// colon {{{
int special-colon() {
    return 0;
}
// }}}
// continue {{{
int special-continue() {
    Vector<String> &arguments(shell_.scope_->arguments_);
    size_t shift(shell_.scope_->shift_);

    Options options(arguments, shift, $(./codes $''));
    while (int option = options.Parse())
        switch (option) {
            default: goto usage; break;
        }

    if (false) usage: {
        writef2("usage: continue <count>?\n");
        return 1;
    }

    if (arguments.count_ - shift == 0)
        shell_.loop_ = 1;
    else if (arguments.count_ - shift == 1) {
        String *count(arguments[shift]);

        int count-int;
        if (!ParseDecimal(count->data_, count->count_, &count-int))
            return 1;
        if (count-int < 0) {
            Shell:Error("negative count");
            return 1;
        }

        shell_.loop_ = count-int;
    } else goto usage;

    shell_.break_ = Break:Loop;
    return 0;
}
// }}}
// declare {{{
int special-declare() {
    Vector<String> &arguments(shell_.scope_->arguments_);
    size_t shift(shell_.scope_->shift_);

    bool array(false);
    bool _export(false);
    bool global(false);

    Options options(arguments, shift, $(./codes $''));
    while (int option = options.Parse())
        switch (option) {
            case 'a': array = true; break;
            case 'g': global = true; break;
            case 'x': _export = true; break;
            default: goto usage; break;
        }

    if (false) usage: {
        writef2("usage: declare (-[ag])? (<name>(=<value>)?)*\n");
        return 1;
    }

    return Special-DeclareExport(!array, !global, _export);
}
// }}}
// eval {{{
int special-eval() {
    Vector<String> &arguments(shell_.scope_->arguments_);
    size_t shift(shell_.scope_->shift_);

    if (arguments.count_ - shift == 0)
        return 0;
    else {
        String code;

        size_t count(arguments.count_ - shift);
        if (count == 1)
            code.AssignString(arguments[shift]);
        else {
            size_t total(count - 1);
            for (size_t i(shift); i != arguments.count_; ++i)
                total += arguments[i]->count_;

            code.Reserve(total);

            for (size_t i(shift); i != arguments.count_; ++i) {
                if (i != shift)
                    code.InsertLast(' ');
                code.CopyStringLast(arguments[i]);
            }
        }

        Shell:Evaluate(&code);
        return shell_.status_;
    }
}
// }}}
// exec {{{
int special-exec() {
    _assert(false);
    return 0;
}
// }}}
// exit {{{
int special-exit() {
    Vector<String> &arguments(shell_.scope_->arguments_);
    size_t shift(shell_.scope_->shift_);

    Options options(arguments, shift, $(./codes $''));
    while (int option = options.Parse())
        switch (option) {
            default: goto usage; break;
        }

    if (false) usage: {
        writef2("usage: exit <status>?\n");
        return 1;
    }

    if (arguments.count_ - shift == 0) {
        _trace();
        _exit(shell_.status_);
    } else if (arguments.count_ - shift == 1) {
        String *status(arguments[shift]);

        int status-int;
        if (!ParseDecimal(status->data_, status->count_, &status-int))
            return 1;

        _trace();
        _exit(status-int);
    } else goto usage;
}
// }}}
// export {{{
int special-export() {
    Vector<String> &arguments(shell_.scope_->arguments_);
    size_t shift(shell_.scope_->shift_);

    Options options(arguments, shift, $(./codes $''));
    while (int option = options.Parse())
        switch (option) {
            default: goto usage; break;
        }

    if (false) usage: {
        writef2("usage: export (<name>(=<value>)?)*\n");
        return 1;
    }

    return Special-DeclareExport(false, false, true);
}
// }}}
// readonly {{{
int special-readonly() {
    _assert(false);
    return 0;
}
// }}}
// return {{{
int special-return() {
    Vector<String> &arguments(shell_.scope_->arguments_);
    size_t shift(shell_.scope_->shift_);

    Options options(arguments, shift, $(./codes $''));
    while (int option = options.Parse())
        switch (option) {
            default: goto usage; break;
        }

    if (false) usage: {
        writef2("usage: return <status>?\n");
        return 1;
    }

    if (arguments.count_ - shift == 1) {
        String *status(arguments[shift]);

        int status-int;
        if (!ParseDecimal(status->data_, status->count_, &status-int))
            return 1;

        shell_.status_ = status-int;
    } else if (arguments.count_ - shift != 0)
        goto usage;

    shell_.break_ = Break:Function;
    return shell_.status_;
}
// }}}
// set {{{
// XXX: I hate life
int special-set() {
    Vector<String> &arguments(shell_.scope_->arguments_);
    size_t shift(shell_.scope_->shift_);

    Options options(arguments, shift, $(./codes $''));
    while (int option = options.Parse())
        switch (option) {
            case 'm': shell_.set-monitor_ = true; break;
            case 'c': shell_.set-noclobber_ = true; break;
            default: goto usage; break;
        }

    if (false) usage: {
        writef2("usage: set (-[Cm])*\n");
        return 1;
    }

    return 0;
}
// }}}
// shift {{{
int special-shift() {
    Vector<String> &arguments(shell_.scope_->arguments_);
    size_t shift(shell_.scope_->shift_);

    Options options(arguments, shift, $(./codes $''));
    while (int option = options.Parse())
        switch (option) {
            default: goto usage; break;
        }

    if (false) usage: {
        writef2("usage: shift <count>?\n");
        return 1;
    }

    int count-int;
    if (arguments.count_ - shift == 0)
        count-int = 1;
    else if (arguments.count_ - shift == 1) {
        String *count(arguments[shift]);
        if (!ParseDecimal(count->data_, count->count_, &count-int))
            return 1;
    } else goto usage;

    Scope *last(shell_.scope_->last_);

    count-int += last->shift_;
    if (count-int > last->arguments_.count_)
        count-int = last->arguments_.count_;

    for (size_t i(last->shift_); i != count-int; ++i) {
        drop(last->arguments_[i]);
        last->arguments_[i] = NULL;
    }

    last->shift_ = count-int;
    return 0;
}
// }}}
// source {{{
int special-source() {
    Vector<String> &arguments(shell_.scope_->arguments_);
    size_t shift(shell_.scope_->shift_);

    Options options(arguments, shift, $(./codes $''));
    while (int option = options.Parse())
        switch (option) {
            default: goto usage; break;
        }

    if (false) usage: {
        writef2("usage: source <shift> (<arg>)*\n");
        return 1;
    }

    Scope *scope(shell_.scope_);

    if (arguments.count_ - shift == 0)
        goto usage;
    else if (arguments.count_ - shift == 1)
        shell_.scope_ = scope->last_;

    String *script(arguments[shift++]);
    _assert(script->size_);

    int status(Execute-Path(script->data_) ? shell_.status_ : 1);

    shell_.scope_ = scope;
    return status;
}
// }}}
// times {{{
int special-times() {
    _assert(false);
    return 0;
}
// }}}
// trap {{{
int special-trap() {
    Vector<String> &arguments(shell_.scope_->arguments_);
    size_t shift(shell_.scope_->shift_);

    // XXX: this doesn't support all the options
    // XXX: when it does it should share -l code with kill
    Options options(arguments, shift, $(./codes $''));
    while (int option = options.Parse())
        switch (option) {
            default: goto usage; break;
        }

    if (false) usage: {
        writef2("usage: trap ((-p)?|-l|<action> <signal>*)\n");
        return 1;
    }

    if (arguments.count_ - shift == 0) {
        _assert(false);
        return 0;
    } else {
        String *action;
        _R< Vector<Logical> > list;

        if (arguments.count_ - shift == 1)
            action = NULL;
        else {
            action = arguments[shift++];
            if (action->count_ == 1 && action->data_[0] == '-')
                action = NULL;
            else {
                list = new Vector<Logical>();
                if (!Parse-Range("<trap>", action->data_, action->count_, *list))
                    return 1;
            }
        }

        int status(0);

        for (size_t i(shift); i != arguments.count_; ++i) {
            String *signal(arguments[i]);

            int signal-int;
            if (!Parse-Signal(signal->data_, signal->count_, signal-int))
                status = 1;

            _R< Vector<Logical> > &trap(shell_.traps_[signal-int]);
            trap.Drop();

            if (!action) {
                trap.Null();
                Signal_(signal-int, SIG_DFL);
            } else if (!list->count_) {
                trap.Null();
                Signal_(signal-int, SIG_IGN);
            } else {
                trap = list;
                take(trap);
                Signal_(signal-int, &Shell:Handle);
            }
        }

        return status;
    }
}
// }}}
// typeset {{{
_finline int special-typeset() {
    return special-declare();
}
// }}}
// unset {{{
int special-unset() {
    Vector<String> &arguments(shell_.scope_->arguments_);
    size_t shift(shell_.scope_->shift_);

    Options options(arguments, shift, $(./codes $''));
    while (int option = options.Parse())
        switch (option) {
            default: goto usage; break;
        }

    if (false) usage: {
        writef2("usage: unset <name>*\n");
        return 1;
    }

    for (size_t i(shift); i != arguments.count_; ++i) {
        String *assign(arguments[i]);
        if (!assign->count_ || assign->data_[assign->count_ - 1] != ')') scalar:
            Shell:UnsetVariable(assign->data_, assign->count_, NULL, 0);
        else if (const char *parenthesis = reinterpret_cast<const char *>(memchr(assign->data_, '(', assign->count_ - 1)))
            Shell:UnsetVariable(assign->data_, parenthesis - assign->data_, parenthesis + 1, assign->data_ + assign->count_ - parenthesis - 2);
        else goto scalar;
    }

    return 0;
}
// }}}
// }}}

BuiltIn Get-Special(String *name) {
    const char *token = name->data_;

    switch (name->count_) {
        case 1:
            switch (token[0]) {
                case ':':
                    return &special-colon;
                break;

                case '.':
                    return &special-source;
                break;
            }
        break;

        case 3:
            if (!memcmp(token, "set", 3))
                return &special-set;
        break;

        case 4:
            switch (token[3]) {
                case 'l':
                    if (!memcmp(token, "eva", 3))
                        return &special-eval;
                break;

                case 'c':
                    if (!memcmp(token, "exe", 3))
                        return &special-exec;
                break;

                case 't':
                    if (!memcmp(token, "exi", 3))
                        return &special-exit;
                break;

                case 'p':
                    if (!memcmp(token, "tra", 3))
                        return &special-trap;
                break;
            }
        break;

        case 5:
            switch (token[0]) {
                case 'b':
                    if (!memcmp(token + 1, "reak", 4))
                        return &special-break;
                break;

                case 's':
                    if (!memcmp(token + 1, "hift", 4))
                        return &special-shift;
                break;

                case 't':
                    if (!memcmp(token + 1, "imes", 4))
                        return &special-times;
                break;

                case 'u':
                    if (!memcmp(token + 1, "nset", 4))
                        return &special-unset;
                break;
            }
        break;

        case 6:
            switch (token[0]) {
                case 'e':
                    if (!memcmp(token + 1, "xport", 5))
                        return &special-export;
                break;

                case 'r':
                    if (!memcmp(token + 1, "eturn", 5))
                        return &special-return;
                break;

                case 's':
                    if (!memcmp(token + 1, "ource", 5))
                        return &special-source;
                break;
            }
        break;

        case 7:
            switch (token[0]) {
                case 'd':
                    if (!memcmp(token + 1, "eclare", 6))
                        return &special-declare;
                break;

                case 't':
                    if (!memcmp(token + 1, "ypeset", 6))
                        return &special-typeset;
                break;
            }
        break;

        case 8:
            switch (token[0]) {
                case 'c':
                    if (!memcmp(token + 1, "ontinue", 7))
                        return &special-continue;
                break;

                case 'r':
                    if (!memcmp(token + 1, "eadonly", 7))
                        return &special-readonly;
                break;
            }
        break;
    }

    return NULL;
}

int Utility-BgFg(bool asynchronous) {
    if (!shell_.set-monitor_) {
        writef2("job control has been disabled\n");
        return 1;
    }

    Vector<String> &arguments(shell_.scope_->arguments_);
    size_t shift(shell_.scope_->shift_);

    if (false) usage: {
        writef2("usage: %cg <job>?\n", asynchronous ? 'b' : 'f');
        return 1;
    }

    if (!shell_.jobs_.count_) {
        writef2("no jobs\n");
        return 1;
    } else if (arguments.count_ - shift == 0) {
        Job *job = shell_.jobs_.Last();
        shell_.job_.Drop();
        shell_.job_ = job;
        --shell_.jobs_.count_;
    } else if (arguments.count_ - shift == 1) {
        String *pid(arguments[shift]);

        int pid-int;
        if (!ParseDecimal(pid->data_, pid->count_, &pid-int))
            return 1;

        Shell:Switch(pid-int);
    } else goto usage;

    if (!asynchronous)
        Shell:Sync();
    else {
        Shell:Continue();
        Shell:Async();
    }

    return shell_.status_;
}

// Utility Commands {{{
// alias {{{
int utility-alias() {
    _assert(false);
    return 0;
}
// }}}
// bg {{{
_finline int utility-bg() {
    return Utility-BgFg(true);
}
// }}}
// cd {{{
int utility-cd() {
    Vector<String> &arguments(shell_.scope_->arguments_);
    size_t shift(shell_.scope_->shift_);

    bool physical(false);

    Options options(arguments, shift, $(./codes $''));
    while (int option = options.Parse())
        switch (option) {
            case 'L': physical = false; break;
            case 'P': physical = true; break;
            default: goto usage; break;
        }

    if (false) usage: {
        writef2("usage: cd (-[LP])* <path>?\n");
        return 1;
    }

    String temp;
    String *dir;

    bool indirect(false);

    if (arguments.count_ - shift == 0) {
        if (!(dir = Shell:GetAcceleratedZero(Accelerated:HOME))) {
            writef2(LQ_"%S"RQ_" is as yet unset\n", "HOME", 4);
            return 1;
        }
    } else if (arguments.count_ - shift != 1)
        goto usage;
    else if (arguments[shift]->Equals("-"))
        if (dir = Shell:GetAcceleratedZero(Accelerated:OLDPWD))
            indirect = true;
        else {
            writef2(LQ_"%S"RQ_" is as yet unset\n", "OLDPWD", 6);
            return 1;
        }
    else if (arguments[shift]->data_[0] == '/') literal: {
        dir = arguments[shift];
    // XXX: support CDPATH
    } else goto literal;

    if (!physical) {
        _assert(false);
        dir = &temp;
    }

    if (::chdir(dir->data_) == -1) {
        Posix:PrintError(errno, "chdir");
        return 1;
    } else if (indirect)
        writef("%S\n", dir->data_, dir->count_);

    String *pwd(Variable:SetZero(Shell:SetAccelerated(Accelerated:PWD)));

    if (!physical)
        pwd->AssignString(dir);
    else if (!System:GetCurrentDirectory(pwd))
        return 1;

    return 0;
}
// }}}
// command {{{
int utility-command() {
    _assert(false);
    return 0;
}
// }}}
// false {{{
_finline int utility-false() {
    return 1;
}
// }}}
// fc {{{
int utility-fc() {
    _assert(false);
    return 0;
}
// }}}
// fg {{{
_finline int utility-fg() {
    return Utility-BgFg(false);
}
// }}}
// getopts {{{
int utility-getopts() {
    Vector<String> &arguments(shell_.scope_->arguments_);
    size_t shift(shell_.scope_->shift_);

    if (false) usage: {
        writef2("usage: getopts <options> <name>\n");
        return 1;
    }

    if (arguments.count_ - shift != 2)
        goto usage;

    _assert(false);
    return 0;
}
// }}}
// jobs {{{
// XXX: this isn't compliant with anything
int utility-jobs() {
    Vector<String> &arguments(shell_.scope_->arguments_);
    size_t shift(shell_.scope_->shift_);

    if (false) usage: {
        writef2("usage: jobs\n");
        return 1;
    }

    if (arguments.count_ - shift != 0)
        goto usage;

    String value;

    for (size_t i(0); i != shell_.jobs_.count_; ++i) {
        value.count_ = 0;
        UnparseDecimal(&value, shell_.jobs_[i]->group_);
        writeall(1, value.data_, value.count_);
        writeall(1, i == shell_.jobs_.count_ - 1 ? "\n" : " ", 1);
    }

    return 0;
}
// }}}
// kill {{{
int utility-kill() {
    Vector<String> &arguments(shell_.scope_->arguments_);
    size_t shift(shell_.scope_->shift_);

    bool list(false);
    int signal(_not(int));

    if (shift != arguments.count_) {
        String *first(arguments[shift]);
        if (first->count_ > 1 && first->data_[0] == '-') {
            if (first->data_[1] >= '0' && first->data_[1] <= '9') {
                if (!ParseDecimal(first->data_ + 1, first->count_ - 1, &signal))
                    return 1;
            } else if ((signal = Get-Signal(first->data_ + 1, first->count_ - 1)) == _not(int))
                goto sigterm;

            ++shift;
            goto parse;
        }
    }

  sigterm:
    signal = SIGTERM;

  parse:
    Options options(arguments, shift, $(./codes $'s'));
    while (int option = options.Parse())
        switch (option) {
            case 's':
                if (!Parse-Signal(options.GetArgumentData(), options.GetArgumentCount(), signal))
                    return 1;
            break;

            case 'l': list = true; break;
            default: goto usage; break;
        }

    if (false) usage: {
        writef2("usage: kill ((-<signal>|-s <signal>)? <pid>*|-l <status>?)\n");
        return 1;
    }

    if (list) {
        if (arguments.count_ - shift == 0)
            _assert(false);
        else if (arguments.count_ - shift != 1)
            goto usage;
        else {
            String *status(arguments[shift]);
            int status-int;

            if (!ParseDecimal(status->data_, status->count_, &status-int))
                return 1;

            if (status-int > 128 && status-int <= 128 + _arraylen(Signal-Names_))
                status -= 128;
            if (status-int <= 0 || status-int > _arraylen(Signal-Names_)) {
                writef2(LQ_"%S"RQ_" is not a signal status\n", status->data_, status->count_);
                return 1;
            }

            writef("%s\n", Signal-Names_[status-int]);
        }
    } else for (size_t i(shift); i != arguments.count_; ++i) {
        String *pid(arguments[i]);

        int pid-int;
        if (!ParseDecimal(pid->data_, pid->count_, &pid-int))
            return 1;
        if (!Kill_(pid-int, signal))
            return 1;
    }

    return 0;
}
// }}}
// newgrp {{{
int utility-newgrp() {
    _assert(false);
    return 0;
}
// }}}
// pwd {{{
int utility-pwd() {
    Vector<String> &arguments(shell_.scope_->arguments_);
    size_t shift(shell_.scope_->shift_);

    bool physical(false);

    Options options(arguments, shift, $(./codes $''));
    while (int option = options.Parse())
        switch (option) {
            case 'L': physical = false; break;
            case 'P': physical = true; break;
            default: goto usage; break;
        }

    if (false) usage: {
        writef2("usage: pwd (-[LP])*\n");
        return 1;
    }

    if (physical) {
        String pwd;
        if (!System:GetCurrentDirectory(&pwd))
            return 1;
        pwd.data_[pwd.count_] = '\n';
        writef("%S", pwd.data_, pwd.count_ + 1);
        return 0;
    } else {
        _assert(false);
        return 1;
    }
}
// }}}
// read {{{
// XXX: this is just weak
int utility-read() {
    Vector<String> &arguments(shell_.scope_->arguments_);
    size_t shift(shell_.scope_->shift_);

    bool raw(false);

    Options options(arguments, shift, $(./codes $''));
    while (int option = options.Parse())
        switch (option) {
            case 'r': raw = true; break;
            default: goto usage; break;
        }

    if (false) usage: {
        writef2("usage: read -r <name>\n");
        return 1;
    }

    if (!raw || arguments.count_ - shift != 1)
        goto usage;

    String value;

  read:
    char c;
    int result(sys:Read(0, &c, 1));
    if (result == -1) {
        Posix:PrintError(errno, "read");
        return 1;
    } else if (result == 0)
        if (!value.count_)
            return 1;
        else goto data;
    else if (c == '\n') data:
        Shell:SetElement(NULL, arguments[shift], NULL, &value, false);
    else {
        value.InsertLast(c);
        goto read;
    }

    return 0;
}
// }}}
// true {{{
_finline int utility-true() {
    return 0;
}
// }}}
// umask {{{
int utility-umask() {
    _assert(false);
    return 0;
}
// }}}
// unalias {{{
int utility-unalias() {
    _assert(false);
    return 0;
}
// }}}
// wait {{{
int utility-wait() {
    Vector<String> &arguments(shell_.scope_->arguments_);
    size_t shift(shell_.scope_->shift_);

    if (false) usage: {
        writef2("usage: wait <job>\n");
        return 1;
    }

    if (arguments.count_ - shift != 1)
        goto usage;

    String *pid(arguments[shift]);

    int pid-int;
    if (!ParseDecimal(pid->data_, pid->count_, &pid-int))
        return 1;

    Shell:Switch(pid-int);
    Shell:Sync(false);
    return shell_.status_;
}
// }}}
// }}}

BuiltIn Get-Utility(String *name) {
    const char *token = name->data_;

    switch (name->count_) {
        case 2:
            switch (token[0]) {
                case 'b':
                    if (token[1] == 'g')
                        return &utility-bg;
                break;

                case 'c':
                    if (token[1] == 'd')
                        return &utility-cd;
                break;

                case 'f':
                    switch (token[1]) {
                        case 'c':
                            return &utility-fc;
                        case 'g':
                            return &utility-fg;
                    }
                break;
            }
        break;

        case 3:
            if (!memcmp(token, "pwd", 3))
                return &utility-pwd;
        break;

        case 4:
            switch (token[0]) {
                case 'j':
                    if (!memcmp(token + 1, "obs", 3))
                        return &utility-jobs;
                break;

                case 'k':
                    if (!memcmp(token + 1, "ill", 3))
                        return &utility-kill;
                break;

                case 'r':
                    if (!memcmp(token + 1, "ead", 3))
                        return &utility-read;
                break;

                case 't':
                    if (!memcmp(token + 1, "rue", 3))
                        return &utility-true;
                break;

                case 'w':
                    if (!memcmp(token + 1, "ait", 3))
                        return &utility-wait;
                break;
            }
        break;

        case 5:
            switch (token[0]) {
                case 'a':
                    if (!memcmp(token + 1, "lias", 4))
                        return &utility-alias;
                break;

                case 'f':
                    if (!memcmp(token + 1, "alse", 4))
                        return &utility-false;
                break;

                case 'u':
                    if (!memcmp(token + 1, "mask", 4))
                        return &utility-umask;
                break;
            }
        break;

        case 6:
            if (!memcmp(token, "newgrp", 6))
                return &utility-newgrp;
        break;

        case 7:
            switch (token[0]) {
                case 'c':
                    if (!memcmp(token + 1, "ommand", 6))
                        return &utility-command;
                break;

                case 'g':
                    if (!memcmp(token + 1, "etopts", 6))
                        return &utility-getopts;
                break;

                case 'u':
                    if (!memcmp(token + 1, "nalias", 6))
                        return &utility-unalias;
                break;
            }
        break;
    }

    return NULL;
}

// XXX: needs to optionally check EROFS for test vs access
bool Access_(const char *path, int mode, bool *error) {
    if (::access(path, mode) != -1)
        return true;
    else {
        int code(errno);
        if (
            code != EACCES &&
            code != ENOENT &&
            code != ENOTDIR
        ) {
            Posix:PrintError(code, "access");
            if (error)
                *error = true;
        }

        return false;
    }
}

bool ParseMode(const char *begin, size_t length, mode_t &mode) {
    writef2(LQ_"%S"RQ_" is not a valid mode\n", begin, length);
    return false;
}

// Fallback Commands {{{
// access {{{
int fallback-access() {
    Vector<String> &arguments(shell_.scope_->arguments_);
    size_t shift(shell_.scope_->shift_);

    int mode(0);

    Options options(arguments, shift, $(./codes $''));
    while (int option = options.Parse())
        switch (option) {
            case 'r': mode |= R_OK; break;
            case 'w': mode |= W_OK; break;
            case 'x': mode |= X_OK; break;
            case 'f': mode |= F_OK; break;
            default: goto usage; break;
        }

    if (false) usage: {
        writef2("usage: access (-[rwxf])* <path>\n");
        return 1;
    }

    if (arguments.count_ - shift != 1)
        goto usage;

    return Access_(arguments[shift]->data_, mode, NULL);
}
// }}}
// cat {{{
int fallback-cat() {
    Vector<String> &arguments(shell_.scope_->arguments_);
    size_t shift(shell_.scope_->shift_);

    bool number-nonblank(false);
    bool show-ends(false);
    bool number(false);
    bool squeeze-blank(false);
    bool show-tabs(false);
    bool unbuffered(false);
    bool show-nonprinting(false);

    Options options(arguments, shift, $(./codes $''));
    while (int option = options.Parse())
        switch (option) {
            case 'A':
                show-nonprinting = true;
                show-ends = true;
                show-tabs = true;
            break;

            case 'e':
                show-nonprinting = true;
                show-ends = true;
            break;

            case 't':
                show-nonprinting = true;
                show-tabs = true;
            break;

            case 'b':
                number-nonblank = true;
                number = true;
            break;

            case 'E': show-ends = true; break;
            case 'n': number = true; break;
            case 's': squeeze-blank = true; break;
            case 'T': show-tabs = true; break;
            case 'u': unbuffered = true; break;
            case 'v': show-nonprinting = true; break;
            default: goto usage; break;
        }

    if (false) usage: {
        writef2("usage: cat (-[AbeEnstTuv])* <path>*\n");
        return 1;
    }

    bool modified(
        number-nonblank ||
        show-ends ||
        number ||
        squeeze-blank ||
        show-tabs ||
        show-nonprinting
    );

    int status(0);

    size_t i;
    int fd;
    bool stdin;

    Buffer buffer(1, unbuffered);

    if (arguments.count_ - shift == 0) {
        i = arguments.count_ - 1;
        fd = 0;
        stdin = true;
        goto cat;
    }

    for (i = shift; i != arguments.count_; ++i) {
        if (stdin = arguments[i]->Equals("-"))
            fd = 0;
        else {
            fd = sys:Open(arguments[i]->data_, O_RDONLY, 0);
            if (fd == -1) {
                Posix:PrintError(errno, "open");
                status = 1;
                continue;
            }
        }

      cat:
        unsigned line(0);
        bool newline(number);
        bool lined(false);

      read:
        char data[BufferSize_ + 3];
        int size(sys:Read(fd, data + 1, BufferSize_));

        if (size == -1) {
            Posix:PrintError(errno, "read");
            goto soft-error;
        } else if (size == 0)
            goto done;
        else if (!modified) {
            if (!writeall(1, data + 1, size))
                goto hard-error;
        } else {
            size_t begin(1);
            for (size_t end(1); end != size + 1; ++end) {
                if (newline) {
                    newline = false;

                    if (!number-nonblank || data[end] != '\n') {
                        ++line;
                        writef("%r%d  ", ' ', 6 - CountDigits(line), line);
                    }
                }

                if (data[end] < 32) {
                    if (data[end] == '\n') {
                        if (!lined) {
                            char next;

                            size_t last;
                            if (!show-ends)
                                last = end + 1;
                            else {
                                last = end + 2;
                                data[end] = '$';
                                next = data[end + 1];
                                data[end + 1] = '\n';
                            }

                            if (!buffer.WriteFully(data + begin, last - begin))
                                goto hard-error;

                            if (number)
                                newline = true;
                            if (show-ends) {
                                data[end] = '\n';
                                data[end + 1] = next;
                            }

                            begin = end + 1;
                        }
                    } else if (data[end] == '\t') {
                        if (show-tabs) {
                            if (!buffer.WriteFully(data + begin, end - begin))
                                goto hard-error;
                            data[end - 1] = '^';
                            data[end] = 'I';
                            begin = end - 1;
                        }
                    } else if (show-nonprinting) {
                        if (!buffer.WriteFully(data + begin, end - begin))
                            goto hard-error;
                        data[end - 1] = '^';
                        data[end] += '@';
                        begin = end - 1;
                    }
                } else if (data[end] == 127) {
                    if (show-nonprinting) {
                        if (!buffer.WriteFully(data + begin, end - begin))
                            goto hard-error;
                        data[end - 1] = '^';
                        data[end] = '?';
                        begin = end - 1;
                    }
                } else if (data[end] >= 127) {
                    if (data[end] == 127) {
                        if (show-nonprinting) {
                            if (!buffer.WriteFully(data + begin, end - begin))
                                goto hard-error;
                            data[end - 1] = '^';
                            data[end] = '?';
                            begin = end - 1;
                        }
                    } else if (show-nonprinting) {
                        char next0(data[end]);
                        data[end] = 'M';

                        char next1(data[end + 1]);
                        data[end + 1] = '-';

                        if (!buffer.WriteFully(data + begin, end + 2 - begin))
                            goto hard-error;
                        data[end + 1] = next1;

                        if (next0 == 128 + '\n') {
                            data[end - 1] = '^';
                            data[end] = 'J';
                            begin = end - 1;
                        } else {
                            data[end] = next0 - 128;
                            begin = end--;
                        }
                    }
                }
            }

            if (!buffer.WriteFully(data + begin, size + 1 - begin))
                goto hard-error;
        }

        if (size != BufferSize_ && !buffer.Flush())
            goto hard-error;
        goto read;

      hard-error:
        if (!stdin)
            ::close(fd);
        return 1;

      soft-error:
        status = 1;

      done:
        if (!stdin && ::close(fd) == -1) {
            Posix:PrintError(errno, "close");
            status = 1;
        }
    }

    return status;
}
// }}}
// echo {{{
int fallback-echo() {
    if (shell_.shopt-noecho_)
        return 0;

    Vector<String> *arguments = &shell_.scope_->arguments_;

    bool newline = true;
    bool extended = false;

    size_t i;
    for (i = 0; i != arguments->count_; ++i) {
        String *arg = (*arguments)[i];

        if (
            !arg->count_ ||
            arg->data_[0] != '-' ||
            strspn(arg->data_ + 1, "neE") != arg->count_ - 1
        )
            break;
        else for (size_t i = 1; i != arg->count_; ++i)
            switch (arg->data_[i]) {
                case 'e': extended = true; break;
                case 'E': extended = false; break;
                case 'n': newline = false; break;
            }
    }

    if (i != arguments->count_)
        for (size_t j = i; j != arguments->count_; ++j) {
            String *arg = (*arguments)[j];
            char temp, *data = arg->data_ ? arg->data_ : &temp;

            if (j != arguments->count_ - 1)
                data[arg->count_] = ' ';
            else if (newline)
                data[arg->count_] = '\n';
            else {
                writef("%S", data, arg->count_);
                break;
            }

            writef("%S", data, arg->count_ + 1);
            data[arg->count_] = '\0';
        }
    else if (newline)
        writef("\n");

    return 0;
}
// }}}
// find {{{
int fallback-find() {
    _assert(false);
    return 0;
}
// }}}
// findlodev {{{
#ifdef CFG_OS_Linux
int fallback-findlodev() {
    Vector<String> &arguments(shell_.scope_->arguments_);
    size_t shift(shell_.scope_->shift_);

    if (false) usage: {
        writef2("usage: findlodev\n");
        return 1;
    }

    if (arguments.count_ - shift != 0)
        goto usage;

    String device;
    device.Reserve(13);
    device.CopyRangeLast("/dev/loop", 9);

    bool error(false);
    if (Access_("/dev/.devfsd", X_OK, &error))
        device.InsertLast('/');
    else if (error)
        return 1;

    size_t last(device.count_);

    for (unsigned i(0); i != 256; ++i) {
        UnparseDecimal(&device, i);

        int dfd(sys:Open(device.data_, O_RDONLY, 0));
        if (dfd == -1) {
            int code(errno);
            if (code == ENOENT || code == ENXIO)
                return 0;
            else {
                Posix:PrintError(code, "open");
                return 1;
            }
        }

        struct loop_info loop;
        int result(::ioctl(dfd, LOOP_GET_STATUS, &loop));
        ::close(dfd);

        if (result == -1) {
            int code(errno);
            if (code == ENXIO) {
                writef("%S\n", device.data_, device.count_);
                return 0;
            } else {
                Posix:PrintError(code, "ioctl");
                return 1;
            }
        }

        device.count_ = last;
    }

    return 0;
}
#endif
// }}}
// insmod {{{
#ifdef CFG_OS_Linux
int fallback-insmod() {
    Vector<String> &arguments(shell_.scope_->arguments_);
    size_t shift(shell_.scope_->shift_);

    if (false) usage: {
        // XXX: options!
        writef2("usage: insmod <module>" /* " <options>*" */ "\n");
        return 1;
    }

    if (arguments.count_ - shift != 1)
        goto usage;

    Map map;
    if (!map.Initialize(arguments[shift]->data_))
        return 1;

    if (::syscall(SYS_init_module, map.data_, map.size_, "") == -1) {
        Posix:PrintError(errno, "init_module");
        return 1;
    }

    return 0;
}
#endif
// }}}
// lsmod {{{
#ifdef CFG_OS_Linux
int fallback-lsmod() {
    _assert(false);
    return 0;
}
#endif
// }}}
// losetup {{{
#ifdef CFG_OS_Linux
int fallback-losetup() {
    Vector<String> &arguments(shell_.scope_->arguments_);
    size_t shift(shell_.scope_->shift_);

    bool detach(false);
    size_t offset(0);

    /*int pfd(-1);*/

    Options options(arguments, shift, $(./codes $'eEop'));
    while (int option = options.Parse())
        switch (option) {
            case 'o': {
                int offset-int;
                if (!ParseDecimal(options.GetArgumentData(), options.GetArgumentCount(), &offset-int))
                    return 1;
                offset = offset-int;
            } break;

            /*case 'p':
                if (!ParseDecimal(options.GetArgumentData(), options.GetArgumentCount(), &pfd))
                    return 1;
            break;*/

            case 'd': detach = true; break;
            default: goto usage; break;
        }

    if (false) usage: {
        writef2("usage: losetup (("/*-e <encryption name>|-E <encryption number>|-p <fd>|*/"-o <offset>)* <device> <path>|-d <device>)\n");
        return 1;
    }

    if (detach) {
        if (arguments.count_ - shift != 1)
            goto usage;

        int dfd(sys:Open(arguments[shift]->data_, O_RDWR, 0));
        if (dfd == -1) {
            Posix:PrintError(errno, "open");
            return 1;
        }

        int result(ioctl(dfd, LOOP_CLR_FD, 0));
        // XXX: better error messages
        if (result == -1 || result) {
            Posix:PrintError(errno, "ioctl");
            ::close(dfd);
            return 1;
        }

        ::close(dfd);
        return 0;
    } else {
        if (arguments.count_ - shift != 2)
            goto usage;

        int dfd(sys:Open(arguments[shift]->data_, O_RDWR, 0));
        if (dfd == -1) {
            Posix:PrintError(errno, "open");
            return 1;
        }

        int ffd(sys:Open(arguments[shift]->data_, O_RDWR, 0));
        if (dfd == -1) {
            Posix:PrintError(errno, "open");
            ::close(dfd);
            return 1;
        }

        int result(ioctl(dfd, LOOP_SET_FD, ffd));
        // XXX: better error messages
        if (result == -1 || result) {
            Posix:PrintError(errno, "ioctl");
            ::close(ffd);
            ::close(dfd);
            return 1;
        }

        ::close(ffd);
        ::close(dfd);
        return 0;
    }
}
#endif
// }}}
// ls {{{
int fallback-ls_(const char *path) {
    int fd = sys:Open(path, O_RDONLY, 0);
    if (fd == -1) {
        Posix:PrintError(errno, "open");
        return 1;
    }

    uint8_t data[1024];
    int size = ::syscall(SYS_getdents, fd, data, sizeof(data));
    if (size < 0)
        return 1;

    struct dirent *entry = (struct dirent *) data;
    while (size > 0) {
        writef("");
        writef("%s\e[0m\n", entry->d_name);

        size -= entry->d_reclen;
        entry = (struct dirent *) ((uint8_t *) entry + entry->d_reclen);
    }

    if (::close(fd) == -1) {
        Posix:PrintError(errno, "close");
        return 1;
    }

    return 0;
}

int fallback-ls() {
    Vector<String> &arguments(shell_.scope_->arguments_);
    size_t shift(shell_.scope_->shift_);

    if (false) usage: {
        writef2("usage: ls\n");
        return 1;
    }

    int status(0);

    Options options(arguments, shift, $(./codes $''));
    while (int option = options.Parse())
        switch (option) {
            default: goto usage; break;
        }

    if (shift == arguments.count_)
        status = fallback-ls_(".");
    else {
        size_t i;
        for (i = shift; i != arguments.count_; ++i) {
            int value = fallback-ls_(arguments[i]->data_);
            if (status == 0 && value != 0)
                status = value;
        }
    }

    return status;
}
// }}}
// makedevs {{{
int fallback-makedevs() {
    _assert(false);
    return 0;
}
// }}}
// mkdevices {{{
int fallback-mkdevices() {
    Vector<String> &arguments(shell_.scope_->arguments_);
    size_t shift(shell_.scope_->shift_);

    if (false) usage: {
        writef2("usage: mkdevices <path>\n");
        return 1;
    }

    if (arguments.count_ - shift != 1)
        goto usage;

    String path;
    path.AssignString(arguments[shift]);
    path.InsertLast('/');

    int pfd(sys:Open("/proc/partitions", O_RDONLY, 0));

    if (pfd == -1) {
        Posix:PrintError(errno, "open");
        return 1;
    }

    {
        File pfile(pfd);
        _assert(false);
    }

    if (::close(pfd) == -1) {
        Posix:PrintError(errno, "close");
        return 1;
    }

    return 0;
}
// }}}
// mkdir {{{
int fallback-mkdir() {
    Vector<String> &arguments(shell_.scope_->arguments_);
    size_t shift(shell_.scope_->shift_);

    bool parents(false);
    mode_t mode(0777);

    Options options(arguments, shift, $(./codes $'m'));
    while (int option = options.Parse())
        switch (option) {
            case 'm':
                if (!ParseMode(options.GetArgumentData(), options.GetArgumentCount(), mode))
                    return 1;
            break;

            case 'p': parents = true; break;
            default: goto usage; break;
        }

    if (false) usage: {
        writef2("usage: mkdir (-m <mode>|-p)* (<path>)*\n");
        return 1;
    }

    int status(0);
    for (size_t i(shift); i != arguments.count_; ++i) {
        const char *path(arguments[i]->data_);

        if (::mkdir(path, mode) == -1) {
            int code(errno);
            if (!parents || code != EEXIST) {
                Posix:PrintError(errno, "mkdir");
                status = 1;
            }
        }
    }

    return status;
}
// }}}
// mkdmnod {{{
#ifdef CFG_OS_Linux
int fallback-mkdmnod() {
    Vector<String> &arguments(shell_.scope_->arguments_);
    size_t shift(shell_.scope_->shift_);

    if (false) usage: {
        writef2("usage: mkdmnod\n");
        return 1;
    }

    if (arguments.count_ - shift != 0)
        goto usage;

    // XXX: apparently I have to look this up
    dev_t device(10 << 8 | 63);

    struct stat control;
    if (::stat("/dev/mapper/control", &control) == -1) {
        int code(errno);
        if (code != ENOENT && code != ENOTDIR) {
            Posix:PrintError(errno, "stat");
            return 1;
        }
    } else if (control.st_dev == device)
        return 0;
    else if (::unlink("/dev/mapper/control") == -1) {
        Posix:PrintError(errno, "unlink");
        return 1;
    }

    // XXX: this needs to create /dev/mapper first, possibly only if stat returned ENOTDIR
    if (::mknod("/dev/mapper/control", 0600, device) == -1) {
        Posix:PrintError(errno, "mknod");
        return 1;
    }

    return 0;
}
#endif
// }}}
// mknod {{{
int fallback-mknod() {
    Vector<String> &arguments(shell_.scope_->arguments_);
    size_t shift(shell_.scope_->shift_);

    mode_t mode(0666);

    Options options(arguments, shift, $(./codes $'m'));
    while (int option = options.Parse())
        switch (option) {
            case 'm':
                if (!ParseMode(options.GetArgumentData(), options.GetArgumentCount(), mode))
                    return 1;
            break;

            default: goto usage; break;
        }

    if (false) usage: {
        writef2("usage: mknod (-m <mode>)* <path> (p|[bc] <major> <minor>)\n");
        return 1;
    }

    dev_t device;

    if (arguments.count_ - shift == 2) {
        String *type(arguments[shift + 1]);
        if (type->count_ != 1 || type->data_[0] != 'p')
            goto usage;

        mode |= S_IFIFO;
        device = 0;
    } else if (arguments.count_ - shift == 4) {
        String *type(arguments[shift + 1]);
        if (type->count_ != 1)
            goto usage;
        else if (type->data_[0] == 'b')
            mode |= S_IFBLK;
        else if (type->data_[1] == 'c')
            mode |= S_IFCHR;
        else goto usage;

        int major;
        if (!ParseDecimal(arguments[shift + 2]->data_, arguments[shift + 2]->count_, &major))
            return 1;

        if (major & ~0xff) {
            writef2("%S out of range\n", "major mode", 10);
            return 1;
        }

        int minor;
        if (!ParseDecimal(arguments[shift + 3]->data_, arguments[shift + 3]->count_, &minor))
            return 1;

        if (minor & ~0xff) {
            writef2("%S out of range\n", "minor mode", 10);
            return 1;
        }

        device = major << 8 | minor;
    } else goto usage;

    if (::mknod(arguments[shift]->data_, mode, device) != -1)
        return 0;
    else {
        Posix:PrintError(errno, "mknod");
        return 1;
    }
}
// }}}
// mkrootdev {{{
int fallback-mkrootdev() {
    _assert(false);
    return 0;
}
// }}}
// mount {{{
/* XXX: this function is weird */
bool Parse-MountFlag(const char *token, size_t count, unsigned long &flags) {
    switch (count) {
        case 2:
            if (token[0] == 'r')
                switch (token[1]) {
                    case 'o':
                        flags |= ${MNT_}RDONLY;
                        return true;
                    break;

                    case 'w':
                        flags &= ~${MNT_}RDONLY;
                        return true;
                    break;
                }
        break;

        case 3:
            if (!memcmp(token, "dev", 3)) {
                flags &= ~${MNT_}NODEV;
                return true;
            }
        break;

        case 4:
            switch (token[1]) {
                case 'x':
                    if (!memcmp(token, "exec", 4)) {
                        flags &= ~${MNT_}NOEXEC;
                        return true;
                    }
                break;

                case 'u':
                    if (!memcmp(token, "suid", 4)) {
                        flags &= ~${MNT_}NOSUID;
                        return true;
                    }
                break;

                case 'y':
                    if (!memcmp(token, "sync", 4)) {
                        flags |= ${MNT_}SYNCHRONOUS;
                        return true;
                    }
                break;
            }
        break;

        case 5:
            switch (token[1]) {
                case 's':
                    if (!memcmp(token, "async", 5)) {
                        flags &= ~${MNT_}SYNCHRONOUS;
                        return true;
                    }
                break;

#ifdef ${MNT_}NOATIME
                case 't':
                    if (!memcmp(token, "atime", 5)) {
                        flags &= ~${MNT_}NOATIME;
                        return true;
                    }
                break;
#endif

                case 'o':
                    if (!memcmp(token, "nodev", 5)) {
                        flags |= ${MNT_}NODEV;
                        return true;
                    }
                break;
            }
        break;

        case 6:
            switch (token[5]) {
                case 'c':
                    if (!memcmp(token, "noexe", 5)) {
                        flags |= ${MNT_}NOEXEC;
                        return true;
                    }
                break;

                case 'd':
                    if (!memcmp(token, "nosui", 5)) {
                        flags |= ${MNT_}NOSUID;
                        return true;
                    }
                break;
            }
        break;

        case 7:
            switch (token[0]) {
#ifdef ${MNT_}NOATIME
                case 'n':
                    if (!memcmp(token + 1, "oatime", 6)) {
                        flags |= ${MNT_}NOATIME;
                        return true;
                    }
                break;
#endif

#ifdef ${MNT_}REMOUNT
                case 'r':
                    if (!memcmp(token + 1, "emount", 6)) {
                        flags |= ${MNT_}REMOUNT;
                        return true;
                    }
                break;
#endif

#ifdef ${MNT_}DIRSYNC
                case 'd':
                    if (!memcmp(token + 1, "irsync", 6)) {
                        flags |= ${MNT_}DIRSYNC;
                        return true;
                    }
                break;
#endif
            }
        break;

        case 8:
            if (!memcmp(token, "defaults", 8)) {
                flags = 0;
                return true;
            }
        break;
    }

    return false;
}

// XXX: add more mount options
// XXX: nash handles a device of LABEL=*
// XXX: this needs to be made more FreeBSD friendly
int fallback-mount() {
    Vector<String> &arguments(shell_.scope_->arguments_);
    size_t shift(shell_.scope_->shift_);

    const char *type(NULL);
    unsigned long flags(0);

    Options options(arguments, shift, $(./codes $'to'));
    while (int option = options.Parse())
        switch (option) {
            case 'o': {
                const char *data(options.GetArgumentData());
                size_t count(options.GetArgumentCount());

                _forever {
                    const char *comma(reinterpret_cast<const char *>(memchr(data, ',', count)));
                    size_t size(comma ? comma - data : count);

                    if (!Parse-MountFlag(data, size, flags)) {
                        writef2(LQ_"%S"RQ_" not an option\n", data, size);
                        return 1;
                    }

                    if (!comma)
                        break;

                    count -= comma + 1 - data;
                    data = comma + 1;
                }
            } break;

            case 't': type = options.GetArgumentData(); break;

            default: goto usage; break;
        }

    if (false) usage: {
        writef2("usage: mount (-t <type>|-o <option>(,<option>)*)* <device> <path>\n");
        return 1;
    }

    if (arguments.count_ - shift != 2)
        goto usage;

#if defined(CFG_OS_Darwin)
    #define MOUNT "mount"
    if (::mount(type, arguments[shift + 1]->data_, flags, NULL) == -1) {
#elif defined(CFG_OS_FreeBSD)
    #define MOUNT "nmount"
    /*"fstype"
    "fspath"*/
    if (::nmount(NULL, 0, flags) == -1) {
#elif defined(CFG_OS_Linux)
    #define MOUNT "mount"
    if (::mount(arguments[shift]->data_, arguments[shift + 1]->data_, type, flags, NULL) == -1) {
#else
    #error mount
#endif
        Posix:PrintError(errno, MOUNT);
        #undef MOUNT
        return 1;
    }

    return 0;
}
// }}}
// pivot_root {{{
#ifdef CFG_OS_Linux
int fallback-pivot_root() {
    Vector<String> &arguments(shell_.scope_->arguments_);
    size_t shift(shell_.scope_->shift_);

    if (false) usage: {
        writef2("usage: pivot_root <new\-path> <old\-path>\n");
        return 1;
    }

    if (arguments.count_ - shift != 2)
        goto usage;

    if (::syscall(SYS_pivot_root, arguments[shift]->data_, arguments[shift + 1]->data_) == -1) {
        Posix:PrintError(errno, "pivot_root");
        return 1;
    }

    return 0;
}
#endif
// }}}
// raidautorun {{{
#ifdef CFG_OS_Linux
int fallback-raidautorun() {
    Vector<String> &arguments(shell_.scope_->arguments_);
    size_t shift(shell_.scope_->shift_);

    if (false) usage: {
        writef2("usage: raidautorun <device>\n");
        return 1;
    }

    if (arguments.count_ - shift != 1)
        goto usage;

    int dfd(sys:Open(arguments[shift]->data_, O_RDWR, 0));
    if (dfd == -1) {
        Posix:PrintError(errno, "open");
        return 1;
    }

    int result(ioctl(dfd, RAID_AUTORUN, 0));
    // XXX: better error messages
    if (result == -1 || result) {
        Posix:PrintError(errno, "ioctl");
        ::close(dfd);
        return 1;
    }

    ::close(dfd);
    return 0;
}
#endif
// }}}
// readlink {{{
int fallback-readlink() {
    Vector<String> &arguments(shell_.scope_->arguments_);
    size_t shift(shell_.scope_->shift_);

    bool canonicalize(false);
    bool no-newline(false);
    bool quiet(false);
    bool silent(false);
    bool verbose(false);

    Options options(arguments, shift, $(./codes $''));
    while (int option = options.Parse())
        switch (option) {
            case 'f': canonicalize = true; break;
            case 'n': no-newline = true; break;
            case 'q': quiet = true; break;
            case 's': silent = true; break;
            case 'v': verbose = true; break;
            default: goto usage; break;
        }

    if (false) usage: {
        writef2("usage: readlink (-[fnqsv])* <path>\n");
        return 1;
    }

    if (arguments.count_ - shift != 1)
        goto usage;

    _assert(false);
    return 0;
}
// }}}
// resume {{{
int fallback-resume() {
    _assert(false);
    return 0;
}
// }}}
// rmmod {{{
#ifdef CFG_OS_Linux
int fallback-rmmod() {
    _assert(false);
    return 0;
}
#endif
// }}}
// setquiet {{{
int fallback-setquiet() {
    shell_.shopt-noecho_ = true;
    return 0;
}
// }}}
// showlabels {{{
int fallback-showlabels() {
    _assert(false);
    return 0;
}
// }}}
// sleep {{{
int fallback-sleep() {
    Vector<String> &arguments(shell_.scope_->arguments_);
    size_t shift(shell_.scope_->shift_);

    if (false) usage: {
        writef2("usage: sleep <seconds>\n");
        return 1;
    }

    if (arguments.count_ - shift != 1)
        goto usage;

    String *seconds(arguments[shift]);

    int seconds-int;
    if (!ParseDecimal(seconds->data_, seconds->count_, &seconds-int))
        return 1;

    if (::usleep(seconds-int * 1000000) == -1) {
        Posix:PrintError(errno, "usleep");
        return 1;
    } else return 0;
}
// }}}
// switchroot {{{
int fallback-switchroot() {
    _assert(false);
    return 0;
}
// }}}
// umount {{{
// XXX: apparently /etc/mtab is stupidly important (I really hate bad standards)
int fallback-umount() {
    Vector<String> &arguments(shell_.scope_->arguments_);
    size_t shift(shell_.scope_->shift_);

    int flags(0);

    // XXX: this is missing numerous options
    Options options(arguments, shift, $(./codes $''));
    while (int option = options.Parse())
        switch (option) {
#ifdef CFG_OS_Linux
            case 'l': flags |= MNT_DETACH; break;
#endif

            case 'f': flags |= MNT_FORCE; break;
            default: goto usage; break;
        }

    if (false) usage: {
        writef2("usage: umount (-[f"
#ifdef CFG_OS_Linux
            "l"
#endif
        "])* <path>\n");
        return 1;
    }

    if (arguments.count_ - shift != 1)
        goto usage;

#if defined(CFG_OS_FreeBSD) || defined(CFG_OS_Darwin)
    #define UNMOUNT "unmount"
    if (::unmount(arguments[shift]->data_, flags) != -1)
#elif defined(CFG_OS_Linux)
    #define UNMOUNT "unmount2"
    if (::umount2(arguments[shift]->data_, flags) != -1)
#else
    #error umount
#endif
        return 0;
    else {
        Posix:PrintError(errno, UNMOUNT);
        #undef UNMOUNT
        return 1;
    }

    return 0;
}
// }}}
// }}}

BuiltIn Get-Fallback(String *name) {
    const char *token = name->data_;

    switch (name->count_) {
        case 2:
            if (!memcmp(token, "ls", 2))
                return &fallback-ls;
        break;

        case 3:
            if (!memcmp(token, "cat", 3))
                return &fallback-cat;
        break;

        case 4:
            switch (token[0]) {
                case 'e':
                    if (!memcmp(token + 1, "cho", 3))
                        return &fallback-echo;
                break;

                case 'f':
                    if (!memcmp(token + 1, "ind", 3))
                        return &fallback-find;
                break;
            }
        break;

        case 5:
            switch (token[2]) {
                case 'd':
                    if (!memcmp(token, "mkdir", 5))
                        return &fallback-mkdir;
                break;

                case 'n':
                    if (!memcmp(token, "mknod", 5))
                        return &fallback-mknod;
                break;

                case 'u':
                    if (!memcmp(token, "mount", 5))
                        return &fallback-mount;
                break;

#ifdef CFG_OS_Linux
                case 'm':
                    switch (token[0]) {
                        case 'l':
                            if (!memcmp(token + 1, "smod", 4))
                                return &fallback-lsmod;
                        break;

                        case 'r':
                            if (!memcmp(token + 1, "mmod", 4))
                                return &fallback-rmmod;
                        break;
                    }
                break;
#endif

                case 'e':
                    if (!memcmp(token, "sleep", 5))
                        return &fallback-sleep;
                break;
            }
        break;

        case 6:
            switch (token[0]) {
                case 'a':
                    if (!memcmp(token + 1, "ccess", 5))
                        return &fallback-access;
                break;

#ifdef CFG_OS_Linux
                case 'i':
                    if (!memcmp(token + 1, "nsmod", 5))
                        return &fallback-insmod;
                break;
#endif

                case 'r':
                    if (!memcmp(token + 1, "esume", 5))
                        return &fallback-resume;
                break;

                case 'u':
                    if (!memcmp(token + 1, "mount", 5))
                        return &fallback-umount;
                break;
            }
        break;

#ifdef CFG_OS_Linux
        case 7:
            switch (token[0]) {
                case 'l':
                    if (!memcmp(token + 1, "osetup", 6))
                        return &fallback-losetup;
                break;

                case 'm':
                    if (!memcmp(token + 1, "kdmnod", 6))
                        return &fallback-mkdmnod;
                break;
            }
        break;
#endif

        case 8:
            switch (token[0]) {
#ifdef CFG_OS_Linux
                case 'm':
                    if (!memcmp(token + 1, "akedevs", 7))
                        return &fallback-makedevs;
                break;
#endif

                case 'r':
                    if (!memcmp(token + 1, "eadlink", 7))
                        return &fallback-readlink;
                break;

                case 's':
                    if (!memcmp(token + 1, "etquiet", 7))
                        return &fallback-umount;
                break;
            }
        break;

        case 9:
            switch (token[2]) {
#ifdef CFG_OS_Linux
                case 'n':
                    if (!memcmp(token, "findlodev", 9))
                        return &fallback-findlodev;
                break;
#endif

                case 'd':
                    if (!memcmp(token, "mkdevices", 9))
                        return &fallback-mkdevices;
                break;

                case 'r':
                    if (!memcmp(token, "mkrootdev", 9))
                        return &fallback-mkrootdev;
                break;
            }
        break;

        case 10:
            switch (token[1]) {
#ifdef CFG_OS_Linux
                case 'i':
                    if (!memcmp(token, "pivot_root", 10))
                        return &fallback-pivot_root;
                break;
#endif

                case 'h':
                    if (!memcmp(token, "showlabels", 10))
                        return &fallback-showlabels;
                break;

#ifdef CFG_OS_Linux
                case 'w':
                    if (!memcmp(token, "switchroot", 10))
                        return &fallback-switchroot;
                break;
#endif
            }
        break;

#ifdef CFG_OS_Linux
        case 11:
            if (!memcmp(token, "raidautorun", 11))
                return &fallback-raidautorun;
        break;
#endif
    }

    return NULL;
}

BuiltIn Get-Accelerated(String *name) {
    const char *token = name->data_;

    switch(name->count_) {
        case 4:
            if (!memcmp(token, "echo", 4))
                return &fallback-echo;
        break;
    }

    return NULL;
}

const char *Shell:Get-IFS() {
    String *IFS = Shell:GetAcceleratedZero(Accelerated:IFS);
    return IFS ? IFS->count_ ? IFS->data_ : NULL : " \t\n";
}

enum Atom:Type {
    Atom:Exact,
    Atom:Expression,
    Atom:Piece,
    Atom:Wildcard
};

struct Atom {
    Atom:Type type_;
    _R<Atom> next_;

    _finline Atom(Atom:Type type) :
        type_(type)
    {
    }
};

struct Exact :
    public Atom
{
    String value_;

    _finline Exact() :
        Atom(Atom:Exact)
    {
    }
};

struct Piece :
    public Atom
{
    _R<Atom> atom_;
    size_t lower_;
    size_t upper_;

    _finline Piece() :
        Atom(Atom:Piece)
    {
    }
};

struct Expression :
    public Atom
{
    Vector<Atom> atoms_;

    _finline Expression() :
        Atom(Atom:Expression)
    {
    }
};

struct Wildcard :
    public Atom
{
    _finline Wildcard() :
        Atom(Atom:Wildcard)
    {
    }
};

struct Molecule {
    String value_;
    Vector<Molecule> nexts_;
};

_finline void Fragment:Insert_(Atom *&atom, _R<Atom> *first, Vector<Atom> *atoms, Atom *content) {
    if (first && !*first)
        *first = atom;
    if (atoms)
        atoms->InsertLast(content);
    atom = content;
}

_finline void Fragment:Create_(Atom *&atom, _R<Atom> *first, Vector<Atom> *atoms) {
    // XXX: this logic is super twisty, and might be speed-up-able
    if (!atom || atoms) {
        atom = new Exact();
        if (first && !*first)
            *first = atom;
        if (atoms)
            atoms->InsertLast(atom);
    }
 }

_finline void Fragment:Append_(Atom *&atom, _R<Atom> *first, Vector<Atom> *atoms) {
    if (!atom)
        Fragment:Create_(atom, first, atoms);
}

void Atom:InsertLast(Atom *&atom, _R<Atom> *first, Vector<Atom> *atoms, Atom *content) {
    if (atom)
        atom = atom->next_ = content;
    else
        Fragment:Insert_(atom, first, atoms, content);
}

void Atom:InsertLast(Atom *&_this, const char *begin, size_t length, bool quoted) {
    if (_this->type_ == Atom:Exact)
        static_cast<Exact *>(_this)->value_.CopyRangeLast(begin, length);
    else {
        _R<Exact> exact(new Exact());
        exact->value_.AssignRange(begin, length);
        Atom:InsertLast(_this, NULL, NULL, exact.Release());
    }

    // XXX: this needs to parse extraneous glob thingees
}

void Fragment:AppendRange_(Atom *&atom, _R<Atom> *first, Vector<Atom> *atoms, const char *begin, size_t length, bool quoted) {
    if (!atoms || quoted) quoted: {
        Fragment:Append_(atom, first, atoms);
        Atom:InsertLast(atom, begin, length, true);
    } else if (length)
        if (const char *ifs = Shell:Get-IFS()) {
            size_t start(0);
            do {
                while (start != length && strchr(ifs, begin[start]))
                    ++start;
                if (start == length) {
                    atom = NULL;
                    return;
                }

                if (start)
                    Fragment:Create_(atom, first, atoms);
                else
                    Fragment:Append_(atom, first, atoms);

                size_t end = start + 1;
                while (end != length && !strchr(ifs, begin[end]))
                    ++end;

                Atom:InsertLast(atom, begin + start, end - start, false);
                start = end;
            } while (start != length);
        } else goto quoted;
}

_finline void Fragment:AppendString_(Atom *&atom, _R<Atom> *first, Vector<Atom> *atoms, const String *value, bool quoted) {
    // XXX: this should be intelligently optimized to share strings
    if (value)
        Fragment:AppendRange_(atom, first, atoms, value->data_, value->count_, quoted);
    else if (quoted)
        Fragment:Append_(atom, first, atoms);
}

void Execute-Pipe(const Command *command);

String *Shell:GetParameterValue(const String *name, const String *subscript) {
    Scope *scope(shell_.scope_);

    int position;
    if (ParseInteger(name->data_, name->count_, 10, &position)) {
        _assert(scope);
        position += scope->shift_;
        return position > scope->arguments_.count_ ? NULL : scope->arguments_[position - 1];
    } else {
        Variable *variable(Shell:GetVariable(name->data_, name->count_));
        if (!variable)
            return NULL;
        else {
            Element *element(
                !subscript || !subscript->count_ ?
                    variable->value_.GetValue() :
                    static_cast<Element *>(variable->elements_.Get(subscript->data_, subscript->count_))
            );

            return element ? &element->value_ : NULL;
        }
    }
}

bool Fragment:Expand(const Fragment *_this, Atom *&atom, _R<Atom> *first, Vector<Atom> *atoms);

bool Word:ToString(const Word *_this, String &value) {
    Atom *temp(NULL);
    _R<Atom> atom;

    Vector:ForEach(Fragment, fragment, &_this->fragments_)
        if (!Fragment:Expand(fragment, temp, &atom, NULL))
            return false;

    if (!atom)
        return true;
    else if (atom->type_ != Atom:Exact) {
        writef2("this isn't a name\n");
        return false;
    } else {
        value.AssignString(&static_cast<Exact *>(atom.GetValue())->value_);
        return true;
    }
}

void Fragment:AppendArguments_(Atom *&atom, _R<Atom> *first, Vector<Atom> *atoms, bool separated, bool quoted) {
    Scope *scope(shell_.scope_);
    Vector<String> &arguments(scope->arguments_);

    const char *ifs(Shell:Get-IFS());

    for (size_t i(scope->shift_); i != arguments.count_; ++i) {
        String *argument(arguments[i]);
        if (i != scope->shift_)
            if (quoted) {
                if (separated) split:
                    atom = NULL;
                else if (ifs)
                    Fragment:AppendRange_(atom, first, atoms, ifs, 1, true);
            } else goto split;

        Fragment:AppendString_(atom, first, atoms, argument, quoted);
    }
}

void Fragment:AppendMap_(Atom *&atom, _R<Atom> *first, Vector<Atom> *atoms, String *name, bool indirect, bool separated, bool quoted) {
    const char *ifs(Shell:Get-IFS());

    Variable *variable(Shell:GetVariable(name->data_, name->count_));
    if (variable && variable->elements_.data_) {
        bool notfirst(false);

        for (size_t i(0); i != 1 << variable->elements_.size_; ++i) {
            HashTable::Slot &slot(variable->elements_.data_[i]);
            if (slot.name_ && slot.name_ != _not(intptr_t)) {
                if (!notfirst)
                    notfirst = true;
                else if (quoted) {
                    if (separated) split:
                        atom = NULL;
                    else if (ifs)
                        Fragment:AppendRange_(atom, first, atoms, ifs, 1, true);
                } else goto split;

                Fragment:AppendString_(
                    atom,
                    first,
                    atoms,
                    indirect ?
                        slot.name_.GetValue() :
                        &static_cast<Element *>(slot.value_.GetValue())->value_,
                    quoted
                );
            }
        }
    }
}

Atom *Word:ToAtom(const Word *_this) {
    Atom *temp(NULL);
    _R<Atom> atom;

    Vector:ForEach(Fragment, fragment, &_this->fragments_)
        if (!Fragment:Expand(fragment, temp, &atom, NULL)) {
            _trace();
            return NULL;
        }

    return atom ?
        atom.Release() :
        new Exact();
}

bool Fragment:Expand(const Fragment *_this, Atom *&atom, _R<Atom> *first, Vector<Atom> *atoms) {
    switch (_this->type_) {
        case Fragment:Execute: {
            const Execute *execute = (const Execute *) _this;
            bool quoted = execute->quoted_;

            int fds[2];
            _syscall(pipe(fds));

            int pid;
            // XXX: this should be using a subjob
            if (pid = Shell:Fork(false)) {
                _syscall(close(fds[1]));

                size_t newlines(0);

                _forever {
                    char buffer[BufferSize_];

                    int size;
                    _syscall(size = sys:Read(fds[0], buffer, sizeof(buffer)));
                    if (size == 0)
                        break;

                    const char *begin = buffer;

                    if (newlines) {
                        size_t broken = _not(size_t);
                        for (size_t i(0); i != size; ++i)
                            if (buffer[i] != '\n') {
                                broken = i;
                                break;
                            }

                        if (broken == _not(size_t)) {
                            newlines += size;
                            begin = NULL;
                        } else {
                            // XXX: _this is (unamazingly) slow
                            for (size_t i(0); i != newlines; ++i)
                                Fragment:AppendRange_(atom, first, atoms, "\n", 1, quoted);

                            newlines = 0;
                            begin += broken;
                            size -= broken;
                        }
                    } else {
                        size_t end = size;
                        while (end != 0 && buffer[end - 1] == '\n')
                            --end;

                        newlines = size - end;
                        size -= newlines;
                    }

                    if (begin)
                        Fragment:AppendRange_(atom, first, atoms, begin, size, quoted);
                }

                _syscall(waitpid(pid, &shell_.status_, 0));
            } else {
                _syscall(dup2(fds[1], 1));
                _syscall(close(fds[0]));
                _syscall(close(fds[1]));

                Execute-List(&execute->list_);
                _trace();
                _exit(shell_.status_);
            }
        } break;

        case Fragment:Glob: {
            const Glob *glob = (const Glob *) _this;

            _R<Atom> content;
            if (glob->patterns_.count_ == 1) {
                content = Word:ToAtom(glob->patterns_[0]);
                if (!content)
                    return false;
            } else if (!glob->patterns_.count_)
                // XXX: maybe make this a shared singleton
                content = new Wildcard();
            else {
                Expression *expression(new Expression());
                content = expression;

                Vector:ForEach(Word, word, &glob->patterns_)
                    if (Atom *atom = Word:ToAtom(word))
                        expression->atoms_.InsertLast(atom);
                    else return false;
            }

            if (glob->type_ != '@') {
                _R<Piece> piece(new Piece());
                switch (glob->type_) {
                    case '?':
                        piece->lower_ = 0;
                        piece->upper_ = 1;
                    break;

                    case '*':
                        piece->lower_ = 0;
                        piece->upper_ = _not(unsigned);
                    break;

                    case '+':
                        piece->lower_ = 1;
                        piece->upper_ = _not(unsigned);
                    break;

                    case '!':
                        piece->lower_ = _not(unsigned);
                        piece->upper_ = _not(unsigned);
                    break;
                }

                piece->atom_ = content;
                content = piece.Release();
            }

            Atom:InsertLast(atom, first, atoms, content.Release());
        } break;

        case Fragment:Literal: {
            const Literal *literal = (const Literal *) _this;
            Fragment:AppendString_(atom, first, atoms, &literal->value_, true);
        } break;

        case Fragment:Parameter: {
            const Parameter *parameter = (const Parameter *) _this;
            Scope *scope(shell_.scope_);

            String temp, *value;
            if (parameter->name_.count_ != 1) variable:
                if (String *string = Shell:GetParameterValue(&parameter->name_, NULL)) {
                    temp.AssignString(string);
                    value = &temp;
                } else
                    value = NULL;
            else switch (parameter->name_.data_[0]) {
                case '@':
                    Fragment:AppendArguments_(atom, first, atoms, true, parameter->quoted_);
                    goto done;
                break;

                case '*':
                    Fragment:AppendArguments_(atom, first, atoms, false, parameter->quoted_);
                    goto done;
                break;

                case '#':
                    UnparseDecimal(&temp, scope->arguments_.count_ - scope->shift_);
                    value = &temp;
                break;

                case '?':
                    UnparseDecimal(&temp, shell_.status_);
                    value = &temp;
                break;

                case '-':
                    _assert(false);
                break;

                case '$':
                    UnparseDecimal(&temp, shell_.pid_);
                    value = &temp;
                break;

                case '!':
                    if (!shell_.jobs_.count_)
                        value = NULL;
                    else {
                        UnparseDecimal(&temp, shell_.jobs_.Last()->group_);
                        value = &temp;
                    }
                break;

                case '0':
                    temp.AssignString(shell_.arg0_);
                    value = &temp;
                break;

                case '_':
                    _assert(false);
                break;

                default:
                    goto variable;
                break;
            }

            Fragment:AppendString_(atom, first, atoms, value, parameter->quoted_);
        } done: break;

        case Fragment:Parametric: {
            const Parametric *parametric = (const Parametric *) _this;

            String name, subscript;

            if (
                !Word:ToString(&parametric->name_, name) ||
                !Word:ToString(&parametric->subscript_, subscript)
            )
                return false;

            String temp;

            switch (parametric->type_) {
                case Parametric:SubscriptAt:
                    Fragment:AppendMap_(atom, first, atoms, &name, parametric->indirect_, true, parametric->quoted_);
                break;

                case Parametric:SubscriptStar:
                    Fragment:AppendMap_(atom, first, atoms, &name, parametric->indirect_, false, parametric->quoted_);
                break;

                default: {
                    String *data(Shell:GetParameterValue(&name, &subscript));

                    switch (parametric->type_) {
                        case Parametric:None:
                        break;

                        case Parametric:ColonMinus:
                            if (!data || !data->count_) {
                                if (!Word:ToString(&parametric->arg0_, temp))
                                    return false;
                                data = &temp;
                            }
                        break;

                        case Parametric:ColonPlus:
                            if (data && data->count_) {
                                if (!Word:ToString(&parametric->arg0_, temp))
                                    return false;
                                data = &temp;
                            }
                        break;

                        case Parametric:Minus:
                            if (!data) {
                                if (!Word:ToString(&parametric->arg0_, temp))
                                    return false;
                                data = &temp;
                            }
                        break;

                        case Parametric:Plus:
                            if (data) {
                                if (!Word:ToString(&parametric->arg0_, temp))
                                    return false;
                                data = &temp;
                            }
                        break;

                        default:
                            writef("0x%x\n", parametric->type_);
                            _assert(false);
                        break;
                    }

                    Fragment:AppendString_(atom, first, atoms, data, parametric->quoted_);
                } break;
            }
        } break;

        case Fragment:Process: {
            const Process *process = (const Process *) _this;

            int fds[2];
            _syscall(pipe(fds));

            int pid;
            // XXX: why not just add this task to the job?
            if (pid = Shell:Fork(false)) {
                int fd = fds[process->input_ ? 0 : 1];
                _syscall(close(fds[process->input_ ? 1 : 0]));

                shell_.job_->fds_.InsertLast_((void *) fd);
                shell_.job_->extra_.InsertLast_((void *) pid);

                String value;

                value.CopyRangeLast("/dev/fd/", 8);
                UnparseDecimal(&value, fd);

                Fragment:AppendString_(atom, first, atoms, &value, true);
            } else {
                if (process->input_)
                    _syscall(dup2(fds[1], 1));
                else
                    _syscall(dup2(fds[0], 0));

                _syscall(close(fds[0]));
                _syscall(close(fds[1]));

                Execute-List(&process->list_);
                _trace();
                _exit(shell_.status_);
            }
        } break;

        default:
            _assert(false);
        break;
    }

    return true;
}

bool Atom:Match(Vector<Atom> &stack, String *value, size_t begin, size_t end) {
    _assert(value);

    Atom *atom(stack.Last());

    if (atom == NULL)
        return begin == end;
    else switch (atom->type_) {
        case Atom:Exact: {
            _trace();
            Exact *exact(static_cast<Exact *>(atom));
            writef("%d '%S'\n", exact->value_.count_, exact->value_.data_, exact->value_.count_);

            if (
                begin + exact->value_.count_ > end ||
                memcmp(value->data_ + begin, exact->value_.data_, exact->value_.count_)
            ) {
                _trace();
                return false;
            }

            begin += exact->value_.count_;
        } break;

        case Atom:Expression: {
            Expression *expression(static_cast<Expression *>(atom));
            stack.Last() = atom->next_;
            stack.InsertLast_(NULL);

            bool matched(false);

            Vector:ForEach(Atom, branch, &expression->atoms_) {
                stack.Last() = branch;
                if (Atom:Match(stack, value, begin, end)) {
                    matched = true;
                    goto done;
                }
            }

          done:
            stack.RemoveLast();
            stack.Last() = atom;

            return matched;
        } break;

        case Atom:Piece: {
            _trace();
            Piece *piece(static_cast<Piece *>(atom));
            /*for (size_t i(0); i != piece->lower_; ++i)
                if (!Atom:Match(piece->atom_, value, index))
                    return false;
            for (size_t i(piece->lower_); i != piece->upper_; ++i)
                if (!Atom:Match(piece->atom_, value, index))
                    return true;*/
            return true;
        } break;

        case Atom:Wildcard:
            if (begin == end)
                return false;
            ++begin;
        break;

        default:
            _assert(false);
            return false;
        break;
    }

    stack.Last() = atom->next_;
    bool matched(Atom:Match(stack, value, begin, end));
    stack.Last() = atom;
    return matched;
}

Molecule *Atom:Compile(const Atom *_this) {
    
}

bool Word:Match(const Word *_this, String *value, bool &error) {
    _R<Atom> atom(Word:ToAtom(_this));
    if (!atom) {
        error = true;
        return false;
    }

    _R<Molecule> molecule(Atom:Compile(atom));

    Vector<Atom> stack;
    stack.InsertLast(atom);
    return Atom:Match(stack, value, 0, value->count_);
}

struct ConditionOne :
    public Condition
{
    char type_;
    _R<Word> arg0_;

    _finline ConditionOne(char type, Word *arg0) :
        type_(type),
        arg0_(arg0)
    {
    }

    virtual bool Evaluate(bool &error) {
        String arg0;
        if (!Word:ToString(arg0_, arg0)) {
            error = true;
            return false;
        }

        switch (type_) {
            case 't': {
                int fd;
                if (!ParseDecimal(arg0.data_, arg0.count_, &fd)) {
                    error = true;
                    return false;
                }

                return ::isatty(fd);
            } break;

            case 'o':
                _assert(false);
            break;

            case 'z': return !arg0.count_; break;
            case 'n': return arg0.count_; break;

            case 'r': return Access_(arg0.data_, R_OK, &error); break;
            case 'w': return Access_(arg0.data_, W_OK, &error); break;
            case 'x': return Access_(arg0.data_, X_OK, &error); break;

            default: {
                struct stat stat;
                if (::lstat(arg0.data_, &stat) == -1) {
                    int code(errno);
                    if (code != ENOENT && code != ENOTDIR) {
                        Posix:PrintError(code, "lstat");
                        error = true;
                    }

                    return false;
                }

                switch (type_) {
                    case 'a':
                    case 'e':
                        return true;
                    break;

                    case 'h':
                    case 'L':
                        writef("%d\n", stat.st_mode);
                        return S_ISLNK(stat.st_mode);
                    break;

                    case 'b': return S_ISBLK(stat.st_mode); break;
                    case 'c': return S_ISCHR(stat.st_mode); break;
                    case 'd': return S_ISDIR(stat.st_mode); break;
                    case 'f': return S_ISREG(stat.st_mode); break;
                    case 'g': return stat.st_mode & S_ISGID; break;
                    case 'k': return stat.st_mode & S_ISVTX; break;
                    case 'p': return S_ISFIFO(stat.st_mode); break;
                    case 's': return stat.st_size; break;
                    case 'u': return stat.st_mode & S_ISUID; break;
                    case 'O': return stat.st_uid == ::geteuid(); break;
                    case 'G': return stat.st_gid == ::getegid(); break;
                    case 'S': return S_ISSOCK(stat.st_mode); break;
                    case 'N': return stat.st_atime < stat.st_mtime; break;
                }
            } break;
        }

        _assert(false);
        return false;
    }
};

enum ConditionTwo:Type {
    ConditionTwo:Unknown,

    ConditionTwo:-nt,
    ConditionTwo:-ot,
    ConditionTwo:-ef,

    ConditionTwo:Equal,
    ConditionTwo:EqualTilde,
    ConditionTwo:EqualEqual,
    ConditionTwo:NotEqual,
    ConditionTwo:LeftAngle,
    ConditionTwo:RightAngle,

    ConditionTwo:-eq,
    ConditionTwo:-ne,
    ConditionTwo:-lt,
    ConditionTwo:-le,
    ConditionTwo:-gt,
    ConditionTwo:-ge
};

struct ConditionTwo :
    public Condition
{
    ConditionTwo:Type type_;

    _R<Word> arg0_;
    _R<Word> arg1_;

    ConditionTwo(ConditionTwo:Type type, Word *arg0, Word *arg1) :
        type_(type),
        arg0_(arg0),
        arg1_(arg1)
    {
    }

    virtual bool Evaluate(bool &error) {
        switch (type_) {
            case ConditionTwo:Equal:
            case ConditionTwo:EqualEqual:
            case ConditionTwo:NotEqual: {
                String arg0;
                if (!Word:ToString(arg0_, arg0)) {
                    error = true;
                    return false;
                }

                bool matched(Word:Match(arg1_, &arg0, error));
                if (error)
                    return false;

                return type_ == ConditionTwo:NotEqual ? !matched : matched;
            } break;

            case ConditionTwo:EqualTilde:
                _assert(false);
            break;

            default: {
                String arg0, arg1;
                if (
                    !Word:ToString(arg0_, arg0) ||
                    !Word:ToString(arg1_, arg1)
                ) {
                    error = true;
                    return false;
                }

                switch (type_) {
                    case ConditionTwo:-nt:
                    case ConditionTwo:-ot:
                    case ConditionTwo:-ef: {
                        bool file0(true);
                        struct stat stat0;
                        if (::lstat(arg0.data_, &stat0) == -1) {
                            int code(errno);
                            if (code != ENOENT && code != ENOTDIR) {
                                Posix:PrintError(code, "lstat");
                                error = true;
                                return false;
                            }

                            file0 = false;
                        }

                        bool file1(true);
                        struct stat stat1;
                        if (::lstat(arg1.data_, &stat1) == -1) {
                            int code(errno);
                            if (code != ENOENT && code != ENOTDIR) {
                                Posix:PrintError(code, "lstat");
                                error = true;
                                return false;
                            }

                            file1 = false;
                        }

                        switch (type_) {
                            case ConditionTwo:-nt:
                                return file0 && file1 ? stat1.st_mtime < stat0.st_mtime : true;
                            break;

                            case ConditionTwo:-ot:
                                return file1 && file0 ? stat0.st_mtime < stat1.st_mtime : true;
                            break;

                            case ConditionTwo:-ef:
                                return
                                    stat0.st_dev == stat1.st_dev &&
                                    stat0.st_ino == stat1.st_ino;
                            break;
                        }
                    } break;

                    case ConditionTwo:LeftAngle:
                    case ConditionTwo:RightAngle:
                    break;

                    case ConditionTwo:-eq:
                    case ConditionTwo:-ne:
                    case ConditionTwo:-lt:
                    case ConditionTwo:-le:
                    case ConditionTwo:-gt:
                    case ConditionTwo:-ge: {
                        int value0;
                        if (!ParseDecimal(arg0.data_, arg0.count_, &value0)) {
                            error = true;
                            return false;
                        }

                        int value1;
                        if (!ParseDecimal(arg1.data_, arg1.count_, &value1)) {
                            error = true;
                            return false;
                        }

                        switch (type_) {
                            case ConditionTwo:-eq: return value0 == value1; break;
                            case ConditionTwo:-ne: return value0 != value1; break;
                            case ConditionTwo:-lt: return value0 < value1; break;
                            case ConditionTwo:-le: return value0 <= value1; break;
                            case ConditionTwo:-gt: return value0 > value1; break;
                            case ConditionTwo:-ge: return value0 >= value1; break;
                        }
                    } break;
                }
            } break;
        }

        _assert(false);
        return false;
    }
};

struct ConditionNot :
    public Condition
{
    _R<Condition> condition_;

    ConditionNot(Condition *condition) :
        condition_(condition)
    {
    }

    virtual bool Evaluate(bool &error) {
        return !condition_->Evaluate(error);
    }
};

struct ConditionIf :
    public Condition
{
    bool and_;

    _R<Condition> lhs_;
    _R<Condition> rhs_;

    ConditionIf(bool _and, Condition *lhs, Condition *rhs) :
        and_(_and),
        lhs_(lhs),
        rhs_(rhs)
    {
    }

    virtual bool Evaluate(bool &error) {
        return and_ ^ lhs_->Evaluate(error) ? !and_ : !error && rhs_->Evaluate(error);
    }
};

bool Execute-Assigns(const Vector<Assign> *assigns, bool _export) {
    Vector:ForEach(Assign, assign, assigns) {
        String name, value, subscript;

        if (
            !Word:ToString(assign->name_, name) ||
            assign->subscript_ && !Word:ToString(assign->subscript_, subscript) ||
            assign->value_ && !Word:ToString(assign->value_, value)
        )
            return false;

        Shell:SetElement(NULL, &name, assign->subscript_ ? &subscript : NULL, &value, _export);
    }

    return true;
}

void RedirectFile(int source, const char *target, int flags) {
    int fd;
    _syscall(fd = sys:Open(target, flags, 0666));
    if (fd != source) {
        _syscall(dup2(fd, source));
        _syscall(close(fd));
    }
}

void Expand-Words(Vector<Word> &words, Vector<String> &arguments) {
    Vector<Atom> atoms;

    Vector:ForEach(Word, word, &words) {
        Atom *atom(NULL);
        Vector:ForEach(Fragment, fragment, &word->fragments_)
            Fragment:Expand(fragment, atom, NULL, &atoms);
    }

    Vector:ForEach(Atom, atom, &atoms) {
        if (atom->type_ == Atom:Exact) {
            String *value(new String());
            // XXX: maybe String:AssignString()?
            Swap(*value, static_cast<Exact *>(atom)->value_);
            arguments.InsertLast(value);
        } else _assert(false);
    }
}

// XXX: should be using pool allocation
void Execute-Command(const Command *command) {
    if (shell_.break_)
        return;

    bool complete(shell_.complete_);
    shell_.complete_ = false;

    size_t last-map(shell_.mapping_.count_);

    Vector:ForEach(Redirect, redirect, &command->redirects_) {
        String target;
        if (!Word:ToString(redirect->word_, target)) {
            shell_.status_ = 1;
            goto _break;
        }

        if (!target.count_) {
            Shell:Error("missing redirection target");
            shell_.status_ = 1;
            goto _break;
        }

        bool minus = target.data_[target.count_ - 1] == '-';

        int descriptor;
        bool numeric;

        if (minus && target.count_ == 1)
            numeric = false;
        else if (ParseInteger(target.data_, minus ? target.count_ - 1 : target.count_, 10, &descriptor))
            numeric = true;
        else {
            numeric = false;
            minus = false;
        }

        switch (redirect->type_) {
            case Redirect:LeftAngle:
            case Redirect:LeftAngleRightAngle: {
                int source(redirect->numeric_ ? redirect->descriptor_ : 0);
                if (!complete)
                    Mapping:Backup(&shell_.mapping_, source);

                RedirectFile(source, target.data_, redirect->type_ == Redirect:LeftAngle ? O_RDONLY : O_RDWR);
            } break;

            case Redirect:RightAngle:
            case Redirect:RightAnglePipe:
            case Redirect:RightAngleRightAngle: {
                int source(redirect->numeric_ ? redirect->descriptor_ : 1);
                if (!complete)
                    Mapping:Backup(&shell_.mapping_, source);

                int flags = O_WRONLY | O_CREAT;
                if (shell_.set-noclobber_ && redirect->type_ != Redirect:RightAnglePipe)
                    flags |= O_EXCL;
                if (redirect->type_ == Redirect:RightAngleRightAngle);
                    flags |= O_APPEND;

                RedirectFile(source, target.data_, flags);
            } break;

            case Redirect:LeftAngleAmpersand:
            case Redirect:RightAngleAmpersand: {
                bool input = redirect->type_ == Redirect:LeftAngleAmpersand;
                int fd = redirect->numeric_ ? redirect->descriptor_ : input ? 0 : 1;

                if (!minus && numeric) {
                    if (!complete)
                        Mapping:Backup(&shell_.mapping_, fd);
                    _syscall(dup2(descriptor, fd));
                } else if (input && minus) {
                    if (!complete)
                        Mapping:Backup(&shell_.mapping_, fd);
                    _syscall(close(fd));
                } else if (numeric) {
                    if (!complete)
                        Mapping:Backup(&shell_.mapping_, fd);
                    _syscall(dup2(descriptor, fd));
                    _syscall(close(descriptor));
                } else if (!input) {
                    if (!complete) {
                        Mapping:Backup(&shell_.mapping_, 1);
                        Mapping:Backup(&shell_.mapping_, 2);
                    }

                    RedirectFile(1, target.data_, O_WRONLY | O_CREAT | O_EXCL);
                    _syscall(dup2(1, 2));
                } else _assert(false);
            } break;

            case Redirect:LeftAngleLeftAngleLeftAngle: {
                //int fd = redirect->numeric_ ? redirect->descriptor_ : 0;
                _assert(false);
            } break;
        }
    }

    switch (command->type_) {
        case Command:Case: {
            Case *_case = (Case *) command;

            String value;
            if (!Word:ToString(_case->value_, value)) {
                shell_.status_ = 1;
                break;
            }

            // XXX: implement break
            Vector:ForEach(Test, test, &_case->tests_) {
                bool error(false);
                if (Word:Match(&test->pattern_, &value, error)) {
                    Execute-List(&test->list_);
                    goto _break;
                } else if (error) {
                    shell_.status_ = 1;
                    break;
                }
            }
        } break;

        case Command:Compound: {
            Compound *compound = (Compound *) command;
            Execute-List(&compound->list_);
        } break;

        case Command:Conditional: {
            Conditional *conditional = (Conditional *) command;

            bool error(false);
            shell_.status_ = conditional->condition_->Evaluate(error) ? 0 : 1;
        } break;

        case Command:IterativeFor: {
            IterativeFor *_for = (IterativeFor *) command;

            Vector<String> strings;

            Vector<String> *arguments;
            size_t begin;

            String name;
            if (!Word:ToString(_for->name_, name)) {
                shell_.status_ = 1;
                break;
            }

            Variable *variable(Shell:SetVariable(&name));
            String *value(Variable:SetZero(variable));

            if (!_for->arguments_.count_) {
                arguments = &shell_.scope_->arguments_;
                begin = shell_.scope_->shift_;
            } else {
                arguments = &strings;
                begin = 0;
                Expand-Words(_for->arguments_, strings);
            }

            for (size_t i(begin); i != arguments->count_; ++i) {
                value->AssignString((*arguments)[i]);
                Execute-List(&_for->body_);

                if (shell_.break_ == Break:Loop) {
                    bool _break(shell_.loop_ < 0);
                    if (int loop = (_break ? -shell_.loop_ : shell_.loop_) - 1)
                        shell_.loop_ = _break ? -loop : loop;
                    else {
                        shell_.break_ = Break:None;
                        if (_break)
                            break;
                    }
                } else if (shell_.break_)
                    break;
            }
        } break;

        case Command:Function: {
            Function *function = (Function *) command;

            String name;
            if (!Word:ToString(function->name_, name)) {
                shell_.status_ = 1;
                break;
            }

            take(function->command_);
            *shell_.functions_.Set(&name, true) = function->command_;
        } break;

        case Command:If: {
            If *_if = (If *) command;

            Execute-List(&_if->test_);
            if (IsStatus(shell_.status_, true))
                Execute-List(&_if->then_);
            else {
                Vector:ForEach(If:Elif, elif, &_if->elifs_) {
                    Execute-List(&elif->test_);
                    if (IsStatus(shell_.status_, true)) {
                        Execute-List(&elif->then_);
                        goto fi;
                    }
                }

                Execute-List(&_if->else_);
            }
        } fi: break;

        case Command:Not: {
            Not *_not = (Not *) command;

            Execute-Pipe(_not->command_);
            shell_.status_ = !shell_.status_;
        } break;

        case Command:Simple: {
            Scope scope;

            Simple *simple = (Simple *) command;
            Expand-Words(simple->words_, scope.arguments_);

            scope.last_ = shell_.scope_;
            shell_.scope_ = &scope;

            if (!scope.arguments_.count_ && !Execute-Assigns(&simple->assigns_, false))
                shell_.status_ = 1;
            else if (!scope.arguments_.count_ && !simple->redirects_.count_)
                shell_.status_ = 0;
            else {
                _R<String> arg0;
                if (scope.arguments_.count_) {
                    arg0 = scope.arguments_[0];
                    take(arg0);
                }

                Vector:RemoveFirst(&scope.arguments_, 1);

                BuiltIn builtin = NULL;
                Command *function = NULL;

                if (
                    arg0 && (
                        (builtin = Get-Accelerated(arg0)) ||
                        (builtin = Get-Special(arg0)) ||
                        shell_.shopt-accelerated_ && (
                            (builtin = Get-Fallback(arg0)) ||
                            (builtin = Get-Utility(arg0))
                        )
                    )
                ) builtin:
                    shell_.status_ = builtin();
                else if (arg0 && (function = static_cast<Command *>(shell_.functions_.Get(arg0->data_, arg0->count_)))) {
                    Execute-Command(function);
                    if (shell_.break_ == Break:Function)
                        shell_.break_ = Break:None;
                } else if (arg0 && !shell_.shopt-accelerated_ && (builtin = Get-Utility(arg0)))
                    goto builtin;
                else if (!Shell:Split(complete))
                    if (!arg0) {
                        _trace();
                        _exit(0);
                    } else if (!Execute-Assigns(&simple->assigns_, true)) {
                        _trace();
                        _exit(1);
                    } else {
                        _array(char *, argv, scope.arguments_.count_ + 2);
                        argv[scope.arguments_.count_ + 1] = NULL;

                        argv[0] = arg0->data_;

                        for (size_t i(0); i != scope.arguments_.count_; ++i) {
                            if (!(argv[i + 1] = scope.arguments_[i]->data_))
                                argv[i + 1] = "";
                            //writef2("argv[%d] = \"%s\"\n", i, argv[i]);
                        }

                        //writef("! %S\n", arg0->data_, arg0->count_);

                        environ = (char **) shell_.environ_.data_;
                        //writef("0x%x: %s\n", environ, environ[1]);
                        if (memchr(arg0->data_, '/', arg0->count_))
                            execv(argv[0], argv);
                        else {
                            String file;

                            if (String *path = Shell:GetAcceleratedZero(Accelerated:PATH)) {
                                const char *begin = path->data_;
                                size_t length = path->count_;

                                _forever {
                                    const char *colon = (const char *) memchr(begin, CFG_PATHENV_SEPARATOR, length);
                                    if (colon == NULL)
                                        break;

                                    size_t size = colon - begin;
                                    file.Reserve(size + arg0->count_ + 1);
                                    file.AssignRange(begin, size);
                                    file.InsertLast(CFG_PATHSYS_SEPARATOR);
                                    file.CopyStringLast(arg0);

                                    // XXX: maybe some error handling?
                                    execv(file.data_, argv);

                                    begin = colon + 1;
                                    length -= size + 1;
                                }
                            } else {
                                execv(argv[0], argv);
                            }

                            if (shell_.shopt-fallback_ && (builtin = Get-Fallback(arg0)))
                                _exit(builtin());
                        }

                        writef2("3sh: %S: command not found\n", arg0->data_, arg0->count_);
                        _exit(127);
                    }
            }

            shell_.scope_ = scope.last_;
            scope.last_.Null();
        } break;

        case Command:Subshell: {
            Subshell *subshell = (Subshell *) command;

            if (!Shell:Split(complete)) {
                Execute-List(&subshell->list_);
                _exit(shell_.status_);
            }
        } break;

        case Command:While: {
            While *_while = (While *) command;

            _forever {
                Execute-List(&_while->test_);
                if (IsStatus(shell_.status_, _while->inverse_))
                    break;
                Execute-List(&_while->body_);

                if (shell_.break_ == Break:Loop) {
                    bool _break(shell_.loop_ < 0);
                    if (int loop = (_break ? -shell_.loop_ : shell_.loop_) - 1)
                        shell_.loop_ = _break ? -loop : loop;
                    else {
                        shell_.break_ = Break:None;
                        if (_break)
                            break;
                    }
                } else if (shell_.break_)
                    break;
            }
        } break;

        default:
            _assert(false);
        break;
    }

  _break:
    if (complete) {
        _trace();
        _exit(shell_.status_);
    }

    {
        int *map(reinterpret_cast<int *>(shell_.mapping_.data_));
        while (shell_.mapping_.count_ != last-map)
            if (map[shell_.mapping_.count_ -= 2] == -1)
                _syscall(close(map[shell_.mapping_.count_ + 1]));
            else {
                _syscall(dup2(map[shell_.mapping_.count_], map[shell_.mapping_.count_ + 1]));
                _syscall(close(map[shell_.mapping_.count_]));
            }
    }

    Shell:Trap();
}

void Execute-Pipe(const Command *command) {
    if (!command->pipe_)
        Execute-Command(command);
    else {
        size_t pipes(0);
        for (const Command *i = command; i != NULL; i = i->pipe_)
            ++pipes;

        shell_.job_->pids_.Reserve(pipes - 1);

        int last;
        int fds[2];

        for (size_t i(0); i != pipes; ++i) {
            bool notlast = i != pipes - 1;
            if (notlast)
                _syscall(pipe(fds));

            if (!Shell:Split()) {
                if (i != 0) {
                    _syscall(dup2(last, 0));
                    _syscall(close(last));
                }

                if (notlast) {
                    _syscall(dup2(fds[1], 1));
                    _syscall(close(fds[0]));
                    _syscall(close(fds[1]));
                }

                Execute-Command(command);
            } else if (notlast) {
                if (i != 0)
                    _syscall(close(last));
                last = fds[0];
                _syscall(close(fds[1]));
            }

            command = command->pipe_;
        }

        _syscall(close(last));
    }
}

void Execute-Logical(const Logical *logical) {
    if (logical->commands_.count_ == 1) {
        Execute-Pipe(logical->commands_[0]);

        if (logical->asynchronous_)
            Shell:Async();
        else
            Shell:Sync();
    } else if (logical->commands_.count_) {
        if (!logical->asynchronous_ || !Shell:Split()) {
            shell_.complete_ = false;

            Vector:ForEach(Command, command, &logical->commands_) {
                Execute-Pipe(command);
                Shell:Sync();
                if (shell_.break_ || !IsStatus(shell_.status_, command->inverse_))
                    goto done;
            }

          done:
            if (logical->asynchronous_)
                _exit(shell_.status_);
        }

        if (logical->asynchronous_)
            Shell:Async();
    }
}

void Execute-List(const Vector<Logical> *list) {
    if (list->count_ > 1)
        shell_.complete_ = false;

    //writef("Execute-List()\n");
    Vector:ForEach(Logical, logical, list) {
        Execute-Logical(logical);
        if (shell_.break_)
            return;
    }
}

#define Log-Parse_ \
    fwritef( \
        2, \
        "%s(%d): data[%d,%d] = '%c' : ", \
        __FILE__, __LINE__, \
        parser->line_, \
        parser->current_ - parser->data_.data_, \
        !*parser ? \
            '$' : \
            *parser->current_ < 32 ? \
                '%' : \
                *parser->current_ \
    ), Parser:OutputTerminator_(parser), writef2

#define Log-Parse(name)
/*#define Log-Parse(name) \
    writef("Parse-" name "(): "); \
    Log-Parse_("\n");*/

#define GlobStart_ "?*"
#define ExtGlobStart_ "+@!"
#define FragmentStart_ "'\"$\\\`"

Set AssignCheck_;
Set FieldSeparators_;
Set GlobSeparators_;
Set SpecialParams_;
Set UnaryConditional_;

Set ConditionalSpecial_;
Set ParameterStart_;
Set ParameterEnd_;

Set SegmentSpecial_;

_finline void SetupSets_() {
    AssignCheck_.Merge($(./codes $'=('));
    FieldSeparators_.Merge($(./codes $' \r\n'));
    GlobSeparators_.Merge($(./codes $'|)'));
    SpecialParams_.Merge($(./codes $'*@#?-$!_/'));
    UnaryConditional_.Merge($(./codes $'abcdefghkprstuwxOGLSNozn'));

    ConditionalSpecial_.Merge(FieldSeparators_);
    ConditionalSpecial_.Merge($(./codes $';()'));

    ParameterStart_.Merge($(./codes $':*@(}-=?+#%/'));
    ParameterEnd_.Merge($(./codes $'}'));

    SegmentSpecial_.Merge($(./codes $')'));
}

bool IsNameCharacter(char value) {
    return value == '_' ||
        value >= 'a' && value <= 'z' ||
        value >= 'A' && value <= 'Z' ||
        value >= '0' && value <= '9';
}

Command *Parse-Command(Parser *parser, bool leading);
void Parse-List(Parser *parser, Vector<Logical> *list, bool primary);
void Parse-Fragments(Parser *parser, Word *word, const Set &special = Set());

Fragment *Parse-Substitute(Parser *parser, bool quoted) {
    if (!*parser)
        return NULL;

    char start = *parser->current_;

    if (start == '{') {
        ++parser->current_;

        _R<Parametric> parametric(new Parametric(quoted));

        if (!Parser:Assert(parser))
            return NULL;

        if (*parser->current_ != '!')
            parametric->indirect_ = false;
        else {
            parametric->indirect_ = true;
            ++parser->current_;
        }

        Parse-Fragments(parser, &parametric->name_, ParameterStart_);
        if (parser->token_) {
            Parser:Unexpected(parser);
            return NULL;
        }

        bool subscript(false);

      parse:
        switch (*parser->current_++) {
            case '(':
                if (subscript) {
                    Parser:Error1(parser, "unexpected "LQ_"("RQ_"\n");
                    return NULL;
                }

                if (!Parser:Assert(parser))
                    return NULL;

                subscript = true;

                Parse-Fragments(parser, &parametric->subscript_, SegmentSpecial_);
                if (parser->token_ || *parser->current_++ != ')') {
                    Parser:Unexpected(parser);
                    return NULL;
                }

                if (String *literal = Word:IsUnquotedLiteral(&parametric->subscript_)) {
                    if (literal->count_ != 1)
                        goto parse;
                    else if (literal->data_[0] == '@')
                        parametric->type_ = Parametric:SubscriptAt;
                    else if (literal->data_[0] == '*')
                        parametric->type_ = Parametric:SubscriptStar;
                    else goto parse;

                    parametric->subscript_.fragments_.Clear();
                }

                goto parse;
            break;

            case ':':
                if (!Parser:Assert(parser))
                    return NULL;

                switch (*parser->current_++) {
                    case '=': parametric->type_ = Parametric:ColonEqual; break;
                    case '-': parametric->type_ = Parametric:ColonMinus; break;
                    case '+': parametric->type_ = Parametric:ColonPlus; break;
                    case '?': parametric->type_ = Parametric:ColonQuestion; break;

                    default:
                        Parser:Error1(parser, "invalid "LQ_":"RQ_" sequence\n");
                        return NULL;
                    break;
                }

                goto arg0;
            break;

            case '=':
                parametric->type_ = Parametric:Equal;
                goto arg0;
            break;

            case '-':
                parametric->type_ = Parametric:Minus;
                goto arg0;
            break;

            case '+':
                parametric->type_ = Parametric:Plus;
                goto arg0;
            break;

            case '?':
                parametric->type_ = Parametric:Question;
                goto arg0;
            break;

            case '}':
                return parametric.Release();
            break;

            case '%':
                if (!Parser:Assert(parser))
                    return NULL;
                else if (*parser->current_ != '%')
                    parametric->type_ = Parametric:Percent;
                else {
                    ++parser->current_;
                    parametric->type_ = Parametric:PercentPercent;
                }

                goto arg0;
            break;

            case '#':
                if (!Parser:Assert(parser))
                    return NULL;
                else if (*parser->current_ != '#')
                    parametric->type_ = Parametric:Pound;
                else {
                    ++parser->current_;
                    parametric->type_ = Parametric:PoundPound;
                }

                goto arg0;
            break;

            arg0:
                Parse-Fragments(parser, &parametric->arg0_, ParameterEnd_);
                if (parser->token_ || *parser->current_++ != '}') {
                    Parser:Unexpected(parser);
                    return NULL;
                }

                return parametric.Release();
            break;

            default:
                _assert(false);
                return NULL;
            break;
        }

        return parametric.Release();
    } else if (start == '(') {
        ++parser->current_;

        _R<Execute> execute(new Execute(quoted));

        Parse-List(parser, &execute->list_, false);
        if (parser->token_ != Terminator:RightRound) {
            Parser:Expected(parser, Terminator:RightRound);
            return NULL;
        }

        parser->token_ = Terminator:None;
        return execute.Release();
    } else if (IsNameCharacter(start)) {
        const char *loc = parser->current_;

        do
            ++loc;
        while (
            loc != parser->data_.data_ + parser->data_.count_ &&
            IsNameCharacter(*loc)
        );

        Parameter *parameter = new Parameter(quoted);
        parameter->name_.CopyRangeLast(parser->current_, loc - parser->current_);

        parser->current_ = loc;
        return parameter;
    } else if (SpecialParams_(start)) {
        ++parser->current_;

        Parameter *parameter = new Parameter(quoted);
        parameter->name_.InsertLast(start);
        return parameter;
    } else return NULL;
}

Literal *Word:GetLiteral(Word *_this, bool quoted) {
    Fragment *last = NULL;

    if (_this->fragments_.count_) {
        last = _this->fragments_.Last();
        if (last->type_ != Fragment:Literal || quoted ^ !last->quoted_)
            last = NULL;
    }

    if (last)
        return static_cast<Literal *>(last);
    else {
        Literal *literal = new Literal(false);
        _this->fragments_.InsertLast(literal);
        return literal;
    }
}

bool Parse-Quoted(Parser *parser, Word *word, bool quoted) {
    const char *loc = parser->current_;

    const char *terminators = quoted ? FragmentStart_ + 1 : FragmentStart_ + 2;

    bool not-empty(false);
    _forever {
        if (loc == parser->data_.data_ + parser->data_.count_) {
            parser->current_ = loc;
            Parser:Error1(parser, "unmatched "LQ_"\""RQ_"\n");
            return false;
        }

        Literal *literal(NULL);

        switch (*loc++) {
            case '"':
                if (!quoted)
                    goto unquoted;

                // XXX: this seems like a kludge
                if (!not-empty)
                    Word:GetLiteral(word, true);
                goto matched;
            break;

            case '$': {
                parser->current_ = loc;
                Fragment *substitute = Parse-Substitute(parser, true);

                if (substitute) {
                    literal = NULL;
                    loc = parser->current_;
                    word->fragments_.InsertLast(substitute);
                } else goto unquoted;
            } break;

            case '\\': {
                if (
                    loc != parser->data_.data_ + parser->data_.count_ &&
                    strchr(terminators, *loc)
                ) {
                    // XXX: _this is really slow, but I hardly care anymore
                    if (!literal)
                        literal = Word:GetLiteral(word, true);
                    literal->value_.InsertLast(*loc++);
                } else if (*loc == '\n') {
                    Parser:Request(parser);
                    loc = parser->current_;
                } else goto unquoted;
            } break;

            default:
            unquoted: {
                const char *begin = --loc;
                while (++loc != parser->data_.data_ + parser->data_.count_)
                    if (strchr(terminators, *loc))
                        break;

                if (!literal)
                    literal = Word:GetLiteral(word, true);
                literal->value_.CopyRangeLast(begin, loc - begin);

                if (loc == parser->data_.data_ + parser->data_.count_) {
                    Parser:Request(parser);
                    loc = parser->current_;
                }
            }
        }

        not-empty = true;
    }

  matched:
    parser->current_ = loc;
    return true;
}

void Parse-Fragments(Parser *parser, Word *word, const Set &special) {
    Log-Parse("Fragments")

    if (*parser)
        parser->token_ = Terminator:None;
    else {
        parser->token_ = Terminator:Script;
        return;
    }

    const char *loc = parser->current_;

    _forever if (loc == parser->data_.data_ + parser->data_.count_) {
        goto done;
    } else if (
        (*loc == '<' || *loc == '>') &&
        loc + 1 != parser->data_.data_ + parser->data_.count_ &&
        loc[1] == '('
    ) {
        Process *process = new Process(*loc == '<');
        word->fragments_.InsertLast(process);

        parser->current_ = loc + 2;
        Parse-List(parser, &process->list_, false);
        loc = parser->current_;

        if (parser->token_ != Terminator:RightRound) {
            Parser:Expected(parser, Terminator:RightRound);
            goto done;
        }

        parser->token_ = Terminator:None;
    } else if (special(*loc))
        goto done;
    else switch (*loc++) {
        case '?':
        case '*':
        case '+':
        case '@':
        case '!': {
            if (loc != parser->data_.data_ + parser->data_.count_ && *loc == '(') {
                Glob *glob = new Glob(loc[-1]);
                word->fragments_.InsertLast(glob);

                parser->current_ = loc;

                do {
                    ++parser->current_;
                    Word *pattern = new Word();
                    glob->patterns_.InsertLast(pattern);

                    Parse-Fragments(parser, pattern, GlobSeparators_);
                    if (parser->token_) {
                        Parser:Unexpected(parser);
                        goto done;
                    }
                } while (*parser->current_ == '|');

                if (parser->token_ || *parser->current_++ != ')') {
                    Parser:Expected(parser, Terminator:RightRound);
                    goto done;
                }

                loc = parser->current_;
            } else if (loc[-1] == '?' || loc[-1] == '*')
                word->fragments_.InsertLast(new Glob(loc[-1]));
            else goto literal;
        } break;

        case '\'': {
            parser->current_ = loc;
            Literal *literal = Word:GetLiteral(word, true);

            _forever {
                if (loc == parser->data_.data_ + parser->data_.count_) {
                    Parser:Error1(parser, "unmatched "LQ_"'"RQ_"\n");
                    goto done;
                }

                const char *end = (const char *) memchr(loc, '\'', parser->data_.data_ + parser->data_.count_ - loc);
                if (end) {
                    literal->value_.CopyRangeLast(loc, end - loc);
                    loc = end + 1;
                    break;
                }

                literal->value_.CopyRangeLast(loc, parser->data_.data_ + parser->data_.count_ - loc);

                Parser:Request(parser);
                loc = parser->current_;
            }
        } break;

        case '"':
            parser->current_ = loc;
            if (!Parse-Quoted(parser, word, true))
                goto done;
            loc = parser->current_;
        break;

        case '$': {
            if (loc == parser->data_.data_ + parser->data_.count_)
                goto literal;
            else if (*loc++ != '\'') {
                parser->current_ = loc - 1;
                Fragment *substitute = Parse-Substitute(parser, false);

                if (substitute) {
                    loc = parser->current_;
                    word->fragments_.InsertLast(substitute);
                } else goto done;
            } else {
                Literal *literal = Word:GetLiteral(word, true);

                _forever {
                    const char *quote = loc;

                    if (quote == parser->data_.data_ + parser->data_.count_) {
                        Parser:Error1(parser, "unmatched "LQ_"\'"RQ_"\n");
                        goto done;
                    }

                    _forever {
                        if (!(quote = (const char *) memchr(quote, '\'', parser->data_.data_ + parser->data_.count_ - quote)))
                            break;

                        int slashes;
                        for (slashes = 0; quote[-1 - slashes] == '\\'; ++slashes);
                        if (slashes % 2 == 0)
                            break;
                        ++quote;
                    }

                    const char *end = quote ? quote : parser->data_.data_ + parser->data_.count_;

                    literal->value_.Reserve(literal->value_.count_ + end - loc);
                    char *data = literal->value_.data_ + literal->value_.count_;

                    while (loc != end)
                        if (*loc++ != '\\')
                            *data++ = loc[-1];
                        else if (*loc >= '0' && *loc <= '7') {
                            int value(0);

                            if (!shell_.shopt-strict_) {
                                int count(0);

                                do
                                    value = value * 8 + *loc++ - '0';
                                while (
                                    loc != end && ++count != 3 &&
                                    *loc >= '0' && *loc <= '7'
                                );
                            } else if (end - loc < 3) {
                                parser->current_ = loc + 1;
                                Parser:Error1(parser, "incomplete octal escape\n");
                                goto done;
                            } else if (ParseInteger(loc, 3, 8, &value))
                                loc += 3;
                            else {
                                parser->current_ = loc + 1;
                                Parser:Error1(parser, "invalid octal number\n");
                                goto done;
                            }

                            *data++ = value;
                        } else switch (*loc++) {
                            case 'a': *data++ = '\a'; break;
                            case 'b': *data++ = '\b'; break;
                            case 'e': *data++ = '\x1b'; break;
                            case 'f': *data++ = '\f'; break;
                            case 'n': *data++ = '\n'; break;
                            case 'r': *data++ = '\r'; break;
                            case 't': *data++ = '\t'; break;
                            case 'v': *data++ = '\v'; break;
                            case '\\': *data++ = '\\'; break;
                            case '\'': *data++ = '\''; break;
                            case 'c': *data++ = ToUpper(*loc++) & 0x1f; break;

                            case 'x': {
                                if (loc == end)
                                    if (!shell_.shopt-strict_)
                                        goto direct;
                                    else {
                                        parser->current_ = loc;
                                        Parser:Error1(parser, "incomplete hexadecimal escape\n");
                                        goto done;
                                    }

                                int value;
                                if (!ParseDigit(*loc, 16, &value))
                                    if (!shell_.shopt-strict_)
                                        goto direct;
                                    else {
                                        parser->current_ = loc + 1;
                                        Parser:Error2(parser, LQ_"%c"RQ_" is not a hexadecimal digit\n", *loc);
                                        goto done;
                                    }

                                int digit;
                                if (++loc != end && ParseDigit(*loc, 16, &digit)) {
                                    value = value * 16 + digit;
                                    ++loc;
                                } else if (shell_.shopt-strict_) {
                                    parser->current_ = loc;
                                    Parser:Error1(parser, "single character hexadecimal escape\n");
                                    goto done;
                                }

                                *data++ = value;
                            } break;

                            case '\n':
                            break;

                            default:
                                if (shell_.shopt-strict_) {
                                    parser->current_ = loc;
                                    Parser:Error2(parser, LQ_"%c"RQ_" is not a valid "LQ_"\\"RQ_" escape\n", loc[-1]);
                                    goto done;
                                }

                              direct:
                                *data++ = '\\';
                                *data++ = loc[-1];
                            break;
                        }

                    literal->value_.count_ = data - literal->value_.data_;

                    if (!quote) {
                        Parser:Request(parser);
                        loc = parser->current_;
                        continue;
                    }

                    break;
                }

                ++loc;
            }
        } break;

        case '`': {
            _assert(false);
        } break;

        case '\\': {
            Literal *literal = NULL;

            _forever {
                if (loc == parser->data_.data_ + parser->data_.count_) {
                    parser->current_ = loc;
                    Parser:Error1(parser, "eof unescapable\n");
                    goto done;
                }

                if (*loc != '\n') {
                    if (!literal)
                        literal = Word:GetLiteral(word, true);
                    literal->value_.InsertLast(*loc);
                }

                if (++loc == parser->data_.data_ + parser->data_.count_) {
                    Parser:Request(parser);
                    loc = parser->current_;
                    break;
                }

                if (*loc != '\\')
                    break;
                ++loc;
            }
        } break;

        default:
        literal: {
            const char *begin = loc - 1;

            while (
                loc != parser->data_.data_ + parser->data_.count_ &&
                !strchr(GlobStart_ ExtGlobStart_ FragmentStart_, *loc) &&
                !special(*loc)
            )
                ++loc;

            Literal *literal = Word:GetLiteral(word, false);
            literal->value_.CopyRangeLast(begin, loc - begin);

            if (loc[-1] == '\n') {
                Parser:Request(parser);
                loc = parser->current_;
            }
        } break;
    }

  done:
    parser->current_ = loc;
}

void Skip(Parser *parser) {
    if (!*parser)
        Parser:Request(parser);

    const char *loc = parser->current_;
    while (
        loc != parser->data_.data_ + parser->data_.count_ &&
        *loc != '\n' &&
        FieldSeparators_(*loc)
    )
        ++loc;

    parser->current_ = loc;

    if (!*parser)
        parser->token_ = Terminator:Script;
}

bool Parse-CommandWord(Parser *parser, bool reserved, const Set &extra-special = Set()) {
    Log-Parse("Word")
    Skip(parser);

    {
        const char *loc = parser->current_;

        switch (*loc++) {
            case '\n':
                parser->token_ = Terminator:NewLine;
            break;

            case '|':
                if (loc == parser->data_.data_ + parser->data_.count_ || *loc != '|')
                    parser->token_ = Terminator:Pipe;
                else {
                    parser->token_ = Terminator:PipePipe;
                    ++loc;
                }
            break;

            case '&':
                if (loc == parser->data_.data_ + parser->data_.count_ || *loc != '&')
                    parser->token_ = Terminator:Ampersand;
                else {
                    parser->token_ = Terminator:AmpersandAmpersand;
                    ++loc;
                }
            break;

            case ';': {
                if (loc == parser->data_.data_ + parser->data_.count_ || *loc != ';')
                    parser->token_ = Terminator:Semicolon;
                else {
                    parser->token_ = Terminator:SemicolonSemicolon;
                    ++loc;
                }
            } break;

            case '(':
                parser->token_ = Terminator:LeftRound;
            break;

            case ')':
                parser->token_ = Terminator:RightRound;
            break;

            case '#':
                _forever {
                    if (loc == parser->data_.data_ + parser->data_.count_)
                        parser->token_ = Terminator:Script;
                    else if (*loc++ == '\n')
                        parser->token_ = Terminator:NewLine;
                    else continue;

                    break;
                }

                loc = parser->current_;
            break;

            default:
                goto parse;
            break;
        }

        parser->current_ = loc;
        return false;
    }

  parse:
    Word temporary;

    Set special(extra-special);
    special.Merge(FieldSeparators_);
    special.Merge($(./codes $'|&;<>()#'));

    Parse-Fragments(parser, &temporary, special);
    if (*parser && parser->token_) {
        Parser:Unexpected(parser);
        return false;
    }

    bool terminated(*parser && extra-special(*parser->current_));

    if (reserved && !terminated)
        if (String *literal = Word:IsUnquotedLiteral(&temporary)) {
            const char *token = literal->data_;

            switch (literal->count_) {
                case 1:
                    switch (token[0]) {
                        case '!': { // R:!
                            parser->token_ = Terminator:Not;
                            return false;
                        } break;

                        case '{': { // R:{
                            parser->token_ = Terminator:LeftCurly;
                            return false;
                        } break;

                        case '}': { // R:}
                            parser->token_ = Terminator:RightCurly;
                            return false;
                        } break;
                    }
                break;

                case 2:
                    switch (token[1]) {
                        case '[':
                            if (token[0] == '[') { // R:[[
                                parser->token_ = Terminator:LeftSquareLeftSquare;
                                return false;
                            }
                        break;

                        case ']':
                            if (token[0] == ']') { // R:]]
                                parser->token_ = Terminator:RightSquareRightSquare;
                                return false;
                            }
                        break;

                        case 'o':
                            if (token[0] == 'd') { // R:do
                                parser->token_ = Terminator:Do;
                                return false;
                            }
                        break;

                        case 'i':
                            if (token[0] == 'f') { // R:fi
                                parser->token_ = Terminator:Fi;
                                return false;
                            }
                        break;

                        case 'f':
                            if (token[0] == 'i') { // R:if
                                parser->token_ = Terminator:If;
                                return false;
                            }
                        break;

                        case 'n':
                            if (token[0] == 'i') { // R:in
                                parser->token_ = Terminator:In;
                                return false;
                            }
                        break;
                    }
                break;

                case 3:
                    if (!memcmp(token, "for", 3)) { // R:for
                        parser->token_ = Terminator:For;
                        return false;
                    }
                break;

                case 4:
                    switch (token[1]) {
                        case 'a':
                            if (!memcmp(token, "case", 4)) { // R:case
                                parser->token_ = Terminator:Case;
                                return false;
                            }
                        break;

                        case 'o':
                            if (!memcmp(token, "done", 4)) { // R:done
                                parser->token_ = Terminator:Done;
                                return false;
                            }
                        break;

                        case 'l':
                            if (token[0] == 'e')
                                switch (token[2]) {
                                    case 'i':
                                        if (token[3] == 'f') { // R:elif
                                            parser->token_ = Terminator:Elif;
                                            return false;
                                        }
                                    break;

                                    case 's':
                                        if (token[3] == 'e') { // R:else
                                            parser->token_ = Terminator:Else;
                                            return false;
                                        }
                                    break;
                                }
                        break;

                        case 's':
                            if (!memcmp(token, "esac", 4)) { // R:esac
                                parser->token_ = Terminator:Esac;
                                return false;
                            }
                        break;

                        case 'h':
                            if (!memcmp(token, "then", 4)) { // R:then
                                parser->token_ = Terminator:Then;
                                return false;
                            }
                        break;
                    }
                break;

                case 5:
                    switch (token[0]) {
                        case 'u':
                            if (!memcmp(token + 1, "ntil", 4)) { // R:until
                                parser->token_ = Terminator:Until;
                                return false;
                            }
                        break;

                        case 'w':
                            if (!memcmp(token + 1, "hile", 4)) { // R:while
                                parser->token_ = Terminator:While;
                                return false;
                            }
                        break;
                    }
                break;

                case 6:
                    if (!memcmp(token, "select", 6)) { // R:select
                        parser->token_ = Terminator:Select;
                        return false;
                    }
                break;

                case 8:
                    if (!memcmp(token, "function", 8)) { // R:function
                        parser->token_ = Terminator:Function;
                        return false;
                    }
                break;
            }
        }

    if (temporary.fragments_.count_ != 0) {
        parser->token_ = new Word();
        Swap(*parser->token_, temporary);
    }

    return terminated;
}

void Sync(Parser *parser) {
  sync:
    Skip(parser);

    if (*parser) {
        if (*parser->current_ == '\n') {
            Parser:Request(parser);
            if (!*parser) {
                parser->token_ = Terminator:Script;
                return;
            }
        } else if (*parser->current_ == '#')
            do
                ++parser->current_;
            while (
                *parser &&
                *parser->current_ != '\n'
            );
        else return;

        goto sync;
    }
}

Command *Parse-Pipe(Parser *parser, Command *command) {
    if (parser->token_ == Terminator:Pipe) {
        Sync(parser);
        command->pipe_ = Parse-Command(parser, false);
    }

    return command;
}

bool Parse-Redirect(Parser *parser, Command *command) {
    if (!*parser)
        return false;

    bool input = *parser->current_ == '<';
    bool ampersand = *parser->current_ == '&';
    if (!input && !ampersand && *parser->current_ != '>')
        return false;

    const Fragment *fragment;
    if (!parser->token_) invalid:
        fragment = NULL;
    else if (IsReservedPointer(parser->token_))
        return false;
    else if (parser->token_->fragments_.count_ != 1)
        goto invalid;
    else {
        fragment = parser->token_->fragments_[0];
        if (fragment->type_ != Fragment:Literal || fragment->quoted_)
            return false;
    }

    int descriptor;
    if (fragment) {
        const Literal *literal = (const Literal *) fragment;
        if (!ParseInteger(literal->value_.data_, literal->value_.count_, 10, &descriptor))
            return false;
    }

    Redirect:Type type;
    switch (*++parser->current_) {
        case '&':
            if (ampersand) {
                --parser->current_;
                return false;
            }

            ++parser->current_;
            type = input ?
                Redirect:LeftAngleAmpersand :
                Redirect:RightAngleAmpersand;
        break;

        case '>':
            ++parser->current_;
            type = ampersand ?
                Redirect:RightAngleAmpersand :
                input ?
                    Redirect:LeftAngleRightAngle :
                    Redirect:RightAngleRightAngle;
        break;

        // XXX: here documents are really entirely separate
        case '<':
            if (ampersand) {
                --parser->current_;
                return false;
            } else if (input)
                if (*++parser->current_ != '<')
                    type = Redirect:LeftAngleLeftAngle;
                else if (*++parser->current_ != '-')
                    type = Redirect:LeftAngleLeftAngleLeftAngle;
                else {
                    ++parser->current_;
                    type = Redirect:LeftAngleLeftAngleMinus;
                }
        break;

        case '|':
            if (ampersand) {
                --parser->current_;
                return false;
            } else if (!input) {
                ++parser->current_;
                type = Redirect:RightAnglePipe;
            }
        break;

        default:
            if (ampersand) {
                --parser->current_;
                return false;
            }

            type = input ?
                Redirect:LeftAngle :
                Redirect:RightAngle;
        break;
    }

    Parse-CommandWord(parser, false);
    if (IsReservedPointer(parser->token_)) {
        Parser:Unexpected(parser);
        return false;
    } else {
        Redirect *redirect = new Redirect();
        redirect->type_ = type;
        redirect->numeric_ = fragment;
        redirect->descriptor_ = descriptor;
        redirect->word_ = parser->token_;

        command->redirects_.InsertLast(redirect);
        return true;
    }
}

Command *Parse-Redirects(Parser *parser, _R<Command> command) {
    Log-Parse("Redirect")

  start:
    Parse-CommandWord(parser, false);
    if (Parse-Redirect(parser, command))
        goto start;
    else if (!IsReservedPointer(parser->token_)) {
        Parser:Error1(parser, "word not redirection\n");
        return NULL;
    }

    return Parse-Pipe(parser, command.Release());
}

Command *Parse-Compound(Parser *parser) {
    _R<Compound> compound(new Compound());

    Parse-List(parser, &compound->list_, false);
    if (parser->token_ != Terminator:RightCurly) {
        Parser:Expected(parser, Terminator:RightCurly);
        return NULL;
    }

    return Parse-Redirects(parser, compound.Release());
}

void Parse-ConditionalWord(Parser *parser) {
    Sync(parser);

    if (!*parser) {
        Parser:Unexpected(parser);
        return;
    }

    switch (*parser->current_) {
        case ';':
            return;
        break;

        case '&':
            if (parser->current_ + 1 == parser->data_.data_ + parser->data_.count_)
                Parser:Error1(parser, "invalid conditional token\n");
            else if (parser->current_[1] == '&') {
                parser->current_ += 2;
                parser->token_ = Terminator:AmpersandAmpersand;
            } else goto fragment;
        break;

        case '|':
            if (parser->current_ + 1 == parser->data_.data_ + parser->data_.count_)
                Parser:Error1(parser, "invalid conditional token\n");
            else if (parser->current_[1] == '|') {
                parser->current_ += 2;
                parser->token_ = Terminator:PipePipe;
            } else goto fragment;
        break;

        case '=':
            if (parser->current_ + 1 == parser->data_.data_ + parser->data_.count_)
                goto other-equal;
            else switch (parser->current_[1]) {
                case '=':
                    parser->current_ += 2;
                    parser->token_ = Terminator:EqualEqual;
                break;

                case '~':
                    parser->current_ += 2;
                    parser->token_ = Terminator:EqualTilde;
                break;

                default:
                other-equal: {
                    ++parser->current_;
                    parser->token_ = Terminator:Equal;
                } break;
            }
        break;

        case ']':
            if (parser->current_ + 1 == parser->data_.data_ + parser->data_.count_)
                Parser:Error1(parser, "invalid conditional token\n");
            else if (parser->current_[1] == ']') {
                parser->current_ += 2;
                parser->token_ = Terminator:RightSquareRightSquare;
            } else goto fragment;
        break;

        case '(':
            ++parser->current_;
            parser->token_ = Terminator:LeftRound;
        break;

        case ')':
            ++parser->current_;
            parser->token_ = Terminator:RightRound;
        break;

        case '!':
            if (parser->current_ + 1 == parser->data_.data_ + parser->data_.count_)
                goto other-not;
            else if (parser->current_[1] == '=') {
                parser->current_ += 2;
                parser->token_ = Terminator:NotEqual;
            } else other-not: {
                ++parser->current_;
                parser->token_ = Terminator:Not;
            }
        break;

        default:
        fragment: {
            _R<Word> word(new Word());
            Parse-Fragments(parser, word, ConditionalSpecial_);
            parser->token_ = word.Release();
        } break;
    }
}

Condition *Parse-ConditionalOr(Parser *parser);

Condition *Parse-ConditionalAtomic(Parser *parser) {
    Parse-ConditionalWord(parser);
    _R<Word> word(parser->token_.Release());

    _R<Condition> condition;

    if (!IsReservedPointer(word))
        if (String *literal = Word:IsUnquotedLiteral(word)) {
            if (
                literal->count_ < 2 ||
                literal->count_ > 3 ||
                literal->data_[0] != '-'
            )
                goto quoted1;
            else if (literal->count_ == 3) {
                Parser:Error5(parser, "%S operator in %S position\n", "binary", 6, "unary", 5);
                return NULL;
            } else if (!UnaryConditional_(literal->data_[1])) {
                Parser:Error3(parser, "unknown %S operator\n", "unary", 5);
                return NULL;
            } else {
                Parse-ConditionalWord(parser);
                if (!IsReservedPointer(parser->token_))
                    condition = new ConditionOne(literal->data_[1], parser->token_.Release());
                else {
                    Parser:Unexpected(parser);
                    return NULL;
                }
            }
        } else quoted1: {
            Parse-ConditionalWord(parser);
            ConditionTwo:Type type(ConditionTwo:Unknown);

            if (!IsReservedPointer(parser->token_)) {
                if (String *literal = Word:IsUnquotedLiteral(parser->token_)) {
                    if (
                        literal->count_ < 2 ||
                        literal->count_ > 3 ||
                        literal->data_[0] != '-'
                    )
                        goto quoted2;
                    else if (literal->count_ == 2) {
                        Parser:Error5(parser, "%S operator in %S position\n", "unary", 5, "binary", 6);
                        return NULL;
                    } else switch (literal->data_[1]) {
                        case 'e':
                            switch (literal->data_[2]) {
                                case 'q': type = ConditionTwo:-eq; break;
                                case 'f': type = ConditionTwo:-ef; break;
                            }
                        break;

                        case 'g':
                            switch (literal->data_[2]) {
                                case 'e': type = ConditionTwo:-ge; break;
                                case 't': type = ConditionTwo:-gt; break;
                            }
                        break;

                        case 'l':
                            switch (literal->data_[2]) {
                                case 'e': type = ConditionTwo:-le; break;
                                case 't': type = ConditionTwo:-lt; break;
                            }
                        break;

                        case 'n':
                            switch (literal->data_[2]) {
                                case 'e': type = ConditionTwo:-ne; break;
                                case 't': type = ConditionTwo:-nt; break;
                            }
                        break;

                        case 'o':
                            if (literal->data_[2] == 't')
                                type = ConditionTwo:-ne;
                        break;
                    }
                } else quoted2: {
                    Parser:Error3(parser, "expected %S\n", "binary operator", 15);
                    return NULL;
                }
            } else switch (parser->token_) {
                case Terminator:RightSquareRightSquare:
                case Terminator:RightRound:
                case Terminator:AmpersandAmpersand:
                case Terminator:PipePipe:
                    return new ConditionOne('n', word.Release());
                break;

                case Terminator:Equal:
                    type = ConditionTwo:Equal;
                break;

                case Terminator:EqualTilde:
                    type = ConditionTwo:EqualTilde;
                break;

                case Terminator:EqualEqual:
                    type = ConditionTwo:EqualEqual;
                break;

                case Terminator:NotEqual:
                    type = ConditionTwo:NotEqual;
                break;

                case Terminator:LeftAngle:
                    type = ConditionTwo:LeftAngle;
                break;

                case Terminator:RightAngle:
                    type = ConditionTwo:RightAngle;
                break;

                default:
                    Parser:Unexpected(parser);
                    return NULL;
                break;
            }

            if (type == ConditionTwo:Unknown) {
                Parser:Error3(parser, "unknown %S operator\n", "binary", 6);
                return NULL;
            }

            parser->token_.Clear();

            _R<Word> arg0(word.Release());

            Parse-ConditionalWord(parser);
            if (!IsReservedPointer(parser->token_))
                condition = new ConditionTwo(type, arg0.Release(), parser->token_.Release());
            else {
                Parser:Unexpected(parser);
                return NULL;
            }
        }
    else switch (word) {
        case Terminator:None:
            Parser:Error3(parser, "expected %S\n", "conditional operator", 20);
            return NULL;
        break;

        case Terminator:LeftRound:
            condition = Parse-ConditionalOr(parser);

            if (!IsReservedPointer(parser->token_) || parser->token_ != Terminator:RightRound) {
                Parser:Expected_(parser, ")");
                return NULL;
            }

            parser->token_.Release();
        break;

        case Terminator:Not:
            condition = new ConditionNot(Parse-ConditionalAtomic(parser));
        break;

        default:
            Parser:Unexpected(parser);
            return NULL;
        break;
    }

    if (!parser->token_)
        Parse-ConditionalWord(parser);
    return condition.Release();
}

Condition *Parse-ConditionalAnd(Parser *parser) {
    _R<Condition> lhs(Parse-ConditionalAtomic(parser));

    if (!lhs)
        return NULL;

    if (parser->token_ == Terminator:AmpersandAmpersand) {
        _R<Condition> rhs(Parse-ConditionalAnd(parser));
        return rhs ? new ConditionIf(true, lhs.Release(), rhs.Release()) : NULL;
    }

    return lhs.Release();
}

Condition *Parse-ConditionalOr(Parser *parser) {
    _R<Condition> lhs(Parse-ConditionalAnd(parser));

    if (!lhs)
        return NULL;

    if (parser->token_ == Terminator:PipePipe) {
        _R<Condition> rhs(Parse-ConditionalOr(parser));
        return rhs ? new ConditionIf(false, lhs.Release(), rhs.Release()) : NULL;
    }

    return lhs.Release();
}

Arithmetic *Parse-Arithmetic(Parser *parser) {
    _assert(false);
    return NULL;
}

bool Parse-AssignValue(Parser *parser, Assign *assign) {
    Parse-CommandWord(parser, false);
    if (!IsReservedPointer(parser->token_))
        assign->value_ = parser->token_.Release();
    else if (
        !IsCommandTerminator(parser->token_) &&
        parser->token_ != Terminator:Script
    ) {
        Parser:Unexpected(parser);
        return false;
    }

    return true;
}

Command *Parse-Command(Parser *parser, bool leading) {
    Log-Parse("Command")

    if (!*parser) {
        parser->token_ = Terminator:Script;
        return NULL;
    }

    bool special(Parse-CommandWord(parser, true, AssignCheck_));

    if (IsReservedPointer(parser->token_))
        switch (parser->token_) {
            case Terminator:Not: { // R:!
                if (!leading) {
                    Parser:Error1(parser, LQ_"!"RQ_" only valid on pipelines\n");
                    return NULL;
                }

                if (Command *command = Parse-Command(parser, false))
                    return new Not(command);
                else {
                    Parser:Error1(parser, "usage of "LQ_"!"RQ_" implies a pipeline\n");
                    return NULL;
                }
            } break;

            case Terminator:LeftCurly: { // R:{
                return Parse-Compound(parser);
            } break;

            case Terminator:LeftRound: {
                _R<Subshell> subshell(new Subshell());

                Parse-List(parser, &subshell->list_, false);
                if (parser->token_ != Terminator:RightRound) {
                    Parser:Expected(parser, Terminator:RightRound);
                    return NULL;
                }

                return Parse-Redirects(parser, subshell.Release());
            } break;

            case Terminator:LeftSquareLeftSquare: { // R:[[
                _R<Condition> condition(Parse-ConditionalOr(parser));
                if (!condition)
                    return NULL;

                if (parser->token_ != Terminator:RightSquareRightSquare) {
                    Parser:Expected(parser, Terminator:RightSquareRightSquare);
                    return NULL;
                }

                return Parse-Redirects(parser, new Conditional(condition.Release()));
            } break;

            case Terminator:If: { // R:if
                _R<If> _if(new If());

                Parse-List(parser, &_if->test_, false);
                if (parser->token_ != Terminator:Then) {
                    Parser:Expected(parser, Terminator:Then);
                    return NULL;
                }

                if (!_if->test_.count_) {
                    Parser:Error3(parser, "missing "LQ_"%S"RQ_" test\n", "if", 2);
                    return NULL;
                }

                Parse-List(parser, &_if->then_, false);
                while (parser->token_ == Terminator:Elif) {
                    If:Elif *elif = new If:Elif();
                    _if->elifs_.InsertLast(elif);

                    Parse-List(parser, &elif->test_, false);
                    if (parser->token_ != Terminator:Then) {
                        Parser:Expected(parser, Terminator:Then);
                        return NULL;
                    }

                    if (!elif->test_.count_) {
                        Parser:Error3(parser, "missing "LQ_"%S"RQ_" test\n", "elif", 4);
                        return NULL;
                    }

                    Parse-List(parser, &elif->then_, false);
                }

                if (parser->token_ == Terminator:Else)
                    Parse-List(parser, &_if->else_, false);

                if (parser->token_ != Terminator:Fi) {
                    Parser:Expected(parser, Terminator:Fi);
                    return NULL;
                }

                return Parse-Redirects(parser, _if.Release());
            } break;

            case Terminator:For: { // R:for
                Parse-CommandWord(parser, false);
                if (parser->token_ == Terminator:LeftRound) {
                    if (*parser && *parser->current_ == '(')
                        ++parser->current_;
                    else {
                        Parser:Expected_(parser, "((");
                        return NULL;
                    }

                    _R<ArithmeticFor> _for(new ArithmeticFor());

                    if (!(_for->start_ = Parse-Arithmetic(parser)))
                        return NULL;
                    else if (
                        parser->token_ == Terminator:None &&
                        *parser && *parser->current_ == ';'
                    )
                        ++parser->current_;
                    else {
                        Parser:Expected_(parser, ";");
                        return NULL;
                    }

                    if (!(_for->test_ = Parse-Arithmetic(parser)))
                        return NULL;
                    else if (
                        parser->token_ == Terminator:None &&
                        *parser && *parser->current_ == ';'
                    )
                        ++parser->current_;
                    else {
                        Parser:Expected_(parser, ";");
                        return NULL;
                    }

                    if (!(_for->next_ = Parse-Arithmetic(parser)))
                        return NULL;
                    else if (
                        parser->token_ == Terminator:RightRound &&
                        *parser && *parser->current_ == ')'
                    )
                        ++parser->current_;
                    else {
                        Parser:Expected_(parser, "))");
                        return NULL;
                    }

                    Parse-CommandWord(parser, true);
                    if (IsCommandTerminator(parser->token_)) {
                        Sync(parser);
                        Parse-CommandWord(parser, true);
                    }

                    Parser:Expect(parser, Terminator:Do);

                    Parse-List(parser, &_for->body_, false);
                    Parser:Expect(parser, Terminator:Done);

                    return Parse-Redirects(parser, _for.Release());
                } else if (IsReservedPointer(parser->token_)) {
                    Parser:Error3(parser, "missing "LQ_"%S"RQ_" name\n", "for", 3);
                    return NULL;
                } else {
                    _R<IterativeFor> _for(new IterativeFor());
                    _for->name_ = parser->token_.Release();

                    Parse-CommandWord(parser, true);
                    if (IsCommandTerminator(parser->token_)) {
                        Sync(parser);
                        Parse-CommandWord(parser, true);
                    }

                    if (parser->token_ == Terminator:In) {
                        _forever {
                            Parse-CommandWord(parser, false);
                            if (IsReservedPointer(parser->token_))
                                if (IsCommandTerminator(parser->token_))
                                    break;
                                else {
                                    Parser:Unexpected(parser);
                                    return NULL;
                                }

                            _for->arguments_.InsertLast(parser->token_);
                        }

                        Sync(parser);
                        Parse-CommandWord(parser, true);
                    }

                    Parser:Expect(parser, Terminator:Do);

                    Parse-List(parser, &_for->body_, false);
                    Parser:Expect(parser, Terminator:Done);

                    return Parse-Redirects(parser, _for.Release());
                }
            } break;

            case Terminator:Case: { // R:case
                _R<Case> _case(new Case());

                Parse-CommandWord(parser, false);
                if (IsReservedPointer(parser->token_)) {
                    Parser:Error1(parser, "missing "LQ_"case"RQ_" word\n");
                    return NULL;
                }

                _case->value_ = parser->token_.Release();

                Sync(parser);
                Parse-CommandWord(parser, true);
                Parser:Expect(parser, Terminator:In);

                _forever {
                    Sync(parser);
                    if (!*parser || *parser->current_ != '(')
                        break;

                    _R<Test> test(new Test());

                    ++parser->current_;
                    Parse-Fragments(parser, &test->pattern_, SegmentSpecial_);
                    if (!*parser || *parser->current_ != ')') {
                        Parser:Expected_(parser, ")");
                        return NULL;
                    }

                    ++parser->current_;
                    Parse-List(parser, &test->list_, false);
                    Parser:Expect(parser, Terminator:SemicolonSemicolon);

                    _case->tests_.InsertLast(test.Release());
                }

                Parse-CommandWord(parser, true);
                Parser:Expect(parser, Terminator:Esac);

                return Parse-Redirects(parser, _case.Release());
            } break;

            case Terminator:Until: { // R:until
                _R<While> _while(new While(true));

                Parse-List(parser, &_while->test_, false);
                if (parser->token_ != Terminator:Do) {
                    Parser:Expected(parser, Terminator:Do);
                    return NULL;
                }

                if (!_while->test_.count_) {
                    Parser:Error3(parser, "missing "LQ_"%S"RQ_" test\n", "until", 5);
                    return NULL;
                }

                Parse-List(parser, &_while->body_, false);
                if (parser->token_ != Terminator:Done) {
                    Parser:Expected(parser, Terminator:Done);
                    return NULL;
                }

                return Parse-Redirects(parser, _while.Release());
            } break;

            case Terminator:While: { // R:while
                _R<While> _while(new While(false));

                Parse-List(parser, &_while->test_, false);
                if (parser->token_ != Terminator:Do) {
                    Parser:Expected(parser, Terminator:Do);
                    return NULL;
                }

                if (!_while->test_.count_) {
                    Parser:Error3(parser, "missing "LQ_"%S"RQ_" test\n", "while", 5);
                    return NULL;
                }

                Parse-List(parser, &_while->body_, false);
                if (parser->token_ != Terminator:Done) {
                    Parser:Expected(parser, Terminator:Done);
                    return NULL;
                }

                return Parse-Redirects(parser, _while.Release());
            } break;

            case Terminator:Select: { // R:select
                _assert(false);
            } break;

            case Terminator:Function: { // R:function
                _R<Function> function(new Function());

                Parse-CommandWord(parser, false);
                if (IsReservedPointer(parser->token_)) {
                    Parser:Error3(parser, "missing "LQ_"%S"RQ_" name\n", "function", 8);
                    return NULL;
                }

                function->name_ = parser->token_.Release();

                Parse-CommandWord(parser, true);
                if (parser->token_ == Terminator:LeftRound) {
                    Parse-CommandWord(parser, false);
                    if (!IsReservedPointer(parser->token_)) {
                        Parser:Error1(parser, LQ_"function"RQ_" cannot have arguments\n");
                        return NULL;
                    } else if (parser->token_ != Terminator:RightRound) {
                        Parser:Expected(parser, Terminator:RightRound);
                        return NULL;
                    }

                    Parse-CommandWord(parser, true);
                }

                if (parser->token_ == Terminator:NewLine) {
                    Sync(parser);
                    Parse-CommandWord(parser, true);
                }

                if (!IsReservedPointer(parser->token_)) {
                    Parser:Error1(parser, LQ_"function"RQ_" body not compound\n");
                    return NULL;
                } else if (parser->token_ != Terminator:LeftCurly) {
                    Parser:Expected(parser, Terminator:LeftCurly);
                    return NULL;
                }

                function->command_ = Parse-Compound(parser);
                return function.Release();
            } break;
        }

    _R<Simple> simple;

    _forever {
        if (!special) {
            // XXX: I need to check for functions here as well
            if (!Parse-Redirect(parser, simple)) {
                if (IsReservedPointer(parser->token_))
                    goto done;

                if (!simple)
                    simple = new Simple();
                simple->words_.InsertLast(parser->token_);
                break;
            }
        } else if (!parser->token_) {
            Parser:Error1(parser, "invalid variable name\n");
            return NULL;
        } else if (*parser->current_ == '=') {
            ++parser->current_;
            _R<Assign> assign(new Assign());
            assign->name_ = parser->token_.Release();

            if (!Parse-AssignValue(parser, assign))
                return NULL;

            if (!simple)
                simple = new Simple();
            simple->assigns_.InsertLast(assign.Release());
        } else if (*parser->current_ == '(') {
            ++parser->current_;
            _R<Assign> assign(new Assign());
            assign->name_ = parser->token_.Release();
            assign->subscript_ = new Word();

            Parse-Fragments(parser, assign->subscript_, SegmentSpecial_);
            if (parser->token_ || *parser->current_++ != ')') {
                Parser:Expected(parser, Terminator:RightRound);
                return NULL;
            }

            if (!*parser || *parser->current_++ != '=') {
                Parser:Expected(parser, Terminator:Equal);
                return NULL;
            }

            if (!Parse-AssignValue(parser, assign))
                return NULL;

            if (!simple)
                simple = new Simple();
            simple->assigns_.InsertLast(assign.Release());
        }

        special = Parse-CommandWord(parser, false, AssignCheck_);
    }

    _forever {
        do Parse-CommandWord(parser, false);
        while (Parse-Redirect(parser, simple));

        if (IsReservedPointer(parser->token_))
            break;

        if (!simple)
            simple = new Simple();
        simple->words_.InsertLast(parser->token_);
    }

  done:
    return Parse-Pipe(parser, simple.Release());
}

Logical *Parse-Logical(Parser *parser, bool primary) {
    Log-Parse("Logical")

    _R<Logical> logical;

    _forever {
        if (!primary || logical)
            Sync(parser);

        _R<Command> command(Parse-Command(parser, true));
        if (!command)
            return NULL;

        if (!logical)
            logical = new Logical();

        logical->commands_.InsertLast(command);
        take(command);

        if (parser->token_ == Terminator:AmpersandAmpersand)
            command->inverse_ = true;
        else if (parser->token_ == Terminator:PipePipe)
            command->inverse_ = false;
        else return logical.Release();
    }
}

void Parse-List(Parser *parser, Vector<Logical> *list, bool primary) {
    Log-Parse("List")

    parser->primary_ = primary;
    Sync(parser);
    parser->primary_ = false;

    do parse: if (Logical *logical = Parse-Logical(parser, primary)) {
        list->InsertLast(logical);
        if (parser->token_ != Terminator:Ampersand)
            logical->asynchronous_ = false;
        else {
            logical->asynchronous_ = true;
            goto parse;
        }
    } while (
        parser->token_ == Terminator:Semicolon ||
        !primary && parser->token_ == Terminator:NewLine
    );
}

void Execute-Parser(Parser *parser) {
    Vector<Logical> list;

    do {
        ++parser->reserve_->command_;
        Parse-List(parser, &list, true);
        if (*parser && parser->token_ != Terminator:NewLine)
            break;
        Execute-List(&list);
        // XXX: move this
        if (shell_.break_ == Break:Cancel)
            shell_.break_ = Break:None;
        list.Clear();
    } while (parser->token_ == Terminator:NewLine);

    if (parser->token_ != Terminator:Script)
        Parser:Unexpected(parser);
}

bool Parse-Range(const char *name, char *begin, size_t size, Vector<Logical> &list) {
    Range range(name, begin, begin + size);
    Parser parser(&range);
    Parse-List(&parser, &list, false);

    if (parser.token_ == Terminator:Script)
        return true;
    else {
        Parser:Unexpected(&parser);
        return false;
    }
}

#ifndef CFG_NO_INTERACTIVE
void Execute-Terminal(const char *name, int fin, int fout) {
    Terminal terminal(name, fin, fout);
    Execute-Reserve(&terminal);
}
#endif

bool Execute-Path(const char *path) {
#ifdef _WIN32
    HANDLE file(Eh(CreateFileA(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)));
    HANDLE map(Ez(CreateFileMapping(file, NULL, PAGE_READONLY, 0, 0, NULL)));
    void *view(Ez(MapViewOfFile(map, FILE_MAP_READ, 0, 0, 0)));

    BY_HANDLE_FILE_INFORMATION info;
    Ez(GetFileInformationByHandle(file, &info));

    size_t size(info.nFileSizeLow);

    Execute-Range(path, reinterpret_cast<const char *>(view), size);

    Ez(UnmapViewOfFile(view));
    Ez(CloseHandle(view));
    Ez(CloseHandle(file));
#else
    Map map;

    if (!map.Initialize(path))
        return false;

    if (map.size_ == 0)
        return true;

    Execute-Range(path, map.data_, map.size_);
    return true;
#endif
}

void Share-argv(Vector<String> &arguments, int argc, char *argv[]) {
    arguments.Reserve(argc);

    for (int i(0); i != argc; ++i) {
        String *argument(new String());
        argument->Share(argv[i]);
        arguments.InsertLast(argument);
    }
}

// C Entrypoint {{{
int main(int argc, char *argv[], char *envp[]) {
    Signal_(SIGINT, &Shell:Interrupt);
    //Signal_(SIGSTOP, SIG_IGN);

#if 0
    HashTable table;
    HashTable:_Construct(&table, 10);

    String *command = new String();
    command->AssignRange("command", 7);

    uint32_t hash = Hash("command", 7);
    HashTable:Set_(&table, hash, "command", 7, command);

    for (size_t i(0); i != 1200000000; ++i)
        //Get-Utility(command);                        // 1200000000
        //HashTable:Get_(&table, hash, "command", 7); // 12000000
        //HashTable:Get(&table, "command", 7);        // 7500000

    drop(command);
#else
    if (!Shell:Initialize(envp))
        return 1;

    size_t arg0s(strlen(argv[0]));
    shell_.arg0_->Share_(argv[0], arg0s);

    String name;
    char *slash(reinterpret_cast<char *>(FindLast(argv[0], '/', arg0s)));
    name.data_ = slash ? slash + 1 : argv[0];
    name.count_ = argv[0] + arg0s - name.data_;

    if (BuiltIn builtin = Get-Fallback(&name)) {
        Share-argv(shell_.scope_->arguments_, argc - 1, argv + 1);
        return builtin();
    } else if (name.Equals("bash"))
        shell_.set-bash_ = true;
    else if (name.Equals("ksh"))
        shell_.set-ksh_ = true;
    else if (name.Equals("sh"))
        shell_.set-posix_ = true;
    else if (
        !name.Equals("3sh") &&
        !name.Equals("-3sh") &&
        !name.Equals("3sh\-g")
    ) {
        writef2(LQ_"%S"RQ_" is not a known tool\n", name.data_, name.count_);
        return 1;
    }

    Share-argv(shell_.scope_->arguments_, argc - 1, argv + 1);

    Vector<String> &arguments(shell_.scope_->arguments_);
    size_t &shift(shell_.scope_->shift_);

    String command;
    bool interactive(false);

    Options options(arguments, shift, $(./codes $'c'));
    while (int option = options.Parse())
        switch (option) {
            case 'c':
                command.data_ = options.GetArgumentData();
                command.count_ = options.GetArgumentCount();
            break;

            case 'i': interactive = true; break;
            default: goto usage; break;
        }

    if (false) usage: {
        writef2("usage: %S ((-c <command>|-i|<script>) <arg>*)?\n", name.data_, name.count_);
        return 1;
    }

    if (command.data_)
        Shell:Evaluate(&command);
    else {
        shell_.terminal_ = isatty(0) && isatty(1);

        if (
            !interactive &&
            arguments.count_ - shift == 0 &&
            shell_.terminal_
        )
            interactive = true;

        if (interactive) {
            // XXX: remove interactive and protect with set-monitor_ for clarity?
            shell_.set-monitor_ = true;

            Shell:Activate();
            Execute-Terminal(argv[0], 0, 1);
        } else if (arguments.count_ - shift == 0)
            Execute-File(argv[0], 0);
        else
            Execute-Path(arguments[shift++]->data_);
    }
#endif

    int status(shell_.status_);
    return status;
}
// }}}
// Linux Entrypoint {{{
#ifdef CFG_NOSTARTFILES
#ifdef CFG_ENVIRON
char **environ;
#endif

static void *_dso_handle;
void *__dso_handle(&_dso_handle);

#ifdef CFG_PROGNAME
const char *__progname;
#else
extern const char *__progname;
#endif

extern "C" void _start(char *args) {
    new (&shell_) Shell();
    SetupSets_();

    __progname = args;

    int argc((int) (&args)[-1]);
    int status(main(argc, &args, &args + argc + 1));

    shell_.~Shell();
    _exit(status);
}
#endif
// }}}
EOF
