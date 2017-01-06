/* Cydia Substrate - Powerful Code Insertion Platform
 * Copyright (C) 2012  Jay Freeman (saurik)
*/

/* GNU Lesser General Public License, Version 3 {{{ */
/*
 * Substrate is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * Substrate is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Substrate.  If not, see <http://www.gnu.org/licenses/>.
**/
/* }}} */

%apt Package: com.saurik.iphone.aspectivec
%apt Author: Jay Freeman (saurik) <saurik@saurik.com>

%apt Name: Aspective-C
%apt "Description: I can't believe I logged the whole thing"

%apt Depends: mobilesubstrate (>= 0.9.3367+38)

%bundle com.apple.mobileipod

#include <CydiaSubstrate/CydiaSubstrate.h>
#include <Foundation/Foundation.h>

#include <cstdarg>
#include <cstdio>

#include <set>
#include <vector>

#include <sys/time.h>
#include <pthread.h>

MSClassHook(__NSArrayI)
MSClassHook(__NSArrayM)
MSClassHook(__NSCFArray)

MSClassHook(NSNumber)
MSClassHook(NSString)

static pthread_key_t key_;
static std::set<SEL> banned_;

struct Data { 
    FILE *file_;
    std::vector<uintptr_t> stack_;
    bool lock_;

    Data() :
        file_(NULL),
        lock_(false)
    {
    }
};

struct Lock {
    Data *data_;

    Lock(Data *data) :
        data_(data)
    {
        data_->lock_ = true;
    }

    ~Lock() {
        data_->lock_ = false;
    }
};

void DestroyData(void *pointer) {
    Data *data(reinterpret_cast<Data *>(pointer));
    fclose(data->file_);
    delete data;
}

static void log(FILE *file, const char *value) {
    // XXX: quotation escape
    fprintf(file, "\"%s\"", value);
}

static void log(FILE *file, id value) {
    if (value == nil) {
        fprintf(file, "nil");
        return;
    }

    Class kind(object_getClass(value));
    if (class_isMetaClass(kind)) {
        fprintf(file, "[%s class]", class_getName(value));
        return;
    }

    if (
        kind == $__NSArrayI ||
        kind == $__NSArrayM ||
        kind == $__NSCFArray ||
    false) {
        fprintf(file, "@[");

        for (size_t index(0), count([value count]); index != count; ++index) {
            if (index != 0)
                fprintf(file, ", ");

            if (index == 16) {
                fprintf(file, "...%zu", count);
                break;
            }

            log(file, [value objectAtIndex:index]);
            fflush(file);
        }

        fprintf(file, "]");
        return;
    }
    
    if ([value isKindOfClass:$NSNumber]) {
        double number([value doubleValue]);
        fprintf(file, "@%g", number);
    } else if ([value isKindOfClass:$NSString]) {
        fprintf(file, "@");
        log(file, [value UTF8String]);
    } else {
        fprintf(file, "<%s@0x%08lx>", class_getName(kind), reinterpret_cast<uintptr_t>(value));
    }
}

static bool log(FILE *file, const char *type, va_list &args) {
    type: switch (*type) {
        case '#':
        case '@': {
            id value(va_arg(args, id));
            log(file, value);
        } break;

        case ':': {
            SEL value(va_arg(args, SEL));
            //fprintf(file, "{%p}", value); fflush(file);
            if (value == NULL)
                fprintf(file, "NULL");
            else
                fprintf(file, "@selector(%s)", sel_getName(value));
        } break;

        case '*': {
            const char *value(va_arg(args, const char *));
            log(file, value);
        } break;

        case '^': {
            void *value(va_arg(args, void *));
            if (value == NULL)
                fprintf(file, "NULL");
            else
                fprintf(file, "0x%08lx", reinterpret_cast<uintptr_t>(value));
        } break;

        case 'B': {
            bool value(va_arg(args, int));
            fprintf(file, "%s", value ? "true" : "false");
        } break;

        case 'c': {
            signed char value(va_arg(args, int));
            fprintf(file, "%d", value);
        } break;

        case 'C': {
            unsigned char value(va_arg(args, unsigned int));
            fprintf(file, "%d", value);
        } break;

        case 's': {
            short value(va_arg(args, int));
            fprintf(file, "%d", value);
        } break;

        case 'S': {
            unsigned short value(va_arg(args, unsigned int));
            fprintf(file, "%u", value);
        } break;

        case 'i': {
            int value(va_arg(args, int));
            if (value == INT_MAX)
                fprintf(file, "INT_MAX");
            else
                fprintf(file, "%d", value);
        } break;

        case 'I': {
            unsigned int value(va_arg(args, unsigned int));
            fprintf(file, "%u", value);
        } break;

        case 'l': {
            long value(va_arg(args, long));
            fprintf(file, "%ld", value);
        } break;

        case 'L': {
            unsigned long value(va_arg(args, unsigned long));
            fprintf(file, "%lu", value);
        } break;

        case 'q': {
            long long value(va_arg(args, long long));
            fprintf(file, "%lld", value);
        } break;

        case 'Q': {
            unsigned long long value(va_arg(args, unsigned long long));
            fprintf(file, "%llu", value);
        } break;

        case 'f': {
            union {
                uint32_t i;
                float f;
            } value = {va_arg(args, uint32_t)};
            fprintf(file, "%g", value.f);
        } break;

        case 'd': {
            double value(va_arg(args, double));
            fprintf(file, "%g", value);
        } break;

        case 'N':
        case 'n':
        case 'O':
        case 'o':
        case 'R':
        case 'r':
        case 'V':
            ++type;
            goto type;

        default:
            return false;
    }

    return true;
}

static Method (*_class_getMethodNoSuper)(Class, SEL);
static Class (*realizeClass)(Class);

extern "C" char ***_NSGetArgv(void);

static void $objc_msgSend$(id self, Class _class, SEL _cmd, va_list args) {
    Data *data(reinterpret_cast<Data *>(pthread_getspecific(key_)));
    if (data == NULL) {
        data = new Data();
        pthread_setspecific(key_, data);
    }

    if (data->lock_)
        return;
    Lock lock(data);

    FILE *&file(data->file_);

    if (file == NULL) {
        const char *argv0(**_NSGetArgv());
        if (argv0 == NULL)
            argv0 = "(null)";
        else if (const char *slash = strrchr(argv0, '/'))
            argv0 = slash + 1;

        pid_t pid(getpid());
        pthread_t thread(pthread_self());

        char path[1024];
        sprintf(path, "/tmp/aspectivec_%s_%d_%p.log", argv0, pid, thread);
        fprintf(stderr, "%s", path);
        file = fopen(path, "a");
    }

    if (file == NULL)
        return;

    uintptr_t pointer(reinterpret_cast<uintptr_t>(&data));
    std::vector<uintptr_t> &stack(data->stack_);

    size_t index;
    for (index = 0; index != stack.size(); ++index)
        if (stack[index] <= pointer)
            break;

    if (banned_.find(_cmd) != banned_.end())
        return;

    bool meta;
    const char *name;
    NSMethodSignature *signature;

    if (self == nil) {
        meta = false;
        name = "nil";
        signature = nil;
        return;
    } else {
        Class direct(object_getClass(self));
        if (realizeClass != NULL)
            direct = (*realizeClass)(direct);
        meta = class_isMetaClass(direct);

        if (_class == nil)
            _class = direct;

        Method method;

        if (_class_getMethodNoSuper == NULL)
            method = class_getInstanceMethod(_class, _cmd);
        else while (_class != nil) {
            method = (*_class_getMethodNoSuper)(_class, _cmd);
            if (method != NULL)
                break;
            _class = class_getSuperclass(_class);
        }

        if (method == NULL)
            signature = nil;
        else @try {
            signature = [NSMethodSignature signatureWithObjCTypes:method_getTypeEncoding(method)];
        } @catch (NSException *) {
        }

        if (_class == nil)
            name = "?";
        else
            name = class_getName(_class);

        if (const char *alpha = name) {
            while (*alpha == '_')
                ++alpha;

            if (
                strncmp(alpha, "CA", 2) == 0 ||
                strncmp(alpha, "CF", 2) == 0 ||
                strncmp(alpha, "NS", 2) == 0 ||
                strncmp(alpha, "UI", 2) == 0 ||
            false)
                return;
        }
    }

    {
        timeval time;
        gettimeofday(&time, NULL);

        fprintf(file, "[%lu.%.6u] 0x%.08lx %*s%c[%s",
            time.tv_sec, time.tv_usec,
            reinterpret_cast<uintptr_t>(self),
            int(index * 2), "",
            meta ? '+' : '-', name
        );
    }

    const char *rest(sel_getName(_cmd));

    if (signature == nil)
        fprintf(file, " %s", rest);
    else {
        size_t index(2);

        for (const char *colon; *rest != '\0'; rest = colon) {
            colon = strchr(rest, ':');
            if (colon == NULL) {
                fprintf(file, " %s", rest);
                break;
            }

            size_t length(++colon - rest);
            char part[length + 1];
            memcpy(part, rest, length);
            part[length] = '\0';
            fprintf(file, " %s", part);

            const char *type([signature getArgumentTypeAtIndex:index++]);
            //fprintf(file, "[%s]", type ?: "?"); fflush(file);

            if (!log(file, type, args)) {
                fprintf(file, "<%s>", type);
                if (*colon != '\0') {
                    fprintf(file, " %s", colon);
                    colon = "";
                }
            }

            fflush(file);
        }
    }

    fprintf(file, "]\n");
    fflush(file);

    stack.resize(index);
    stack.push_back(pointer);
}

#define call(b, value) \
    __asm volatile ("push {r0}\n"); \
    __asm volatile ("mov r12, %0\n" :: "r"(value)); \
    __asm volatile ("pop {r0}\n" #b " r12\n");

#define save() \
    __asm volatile ("push {r0, r1, r2, r3}\n");

#define load() \
    __asm volatile ("pop {r0, r1, r2, r3}\n");

#define link(b, value) \
    __asm volatile ("push {lr}\n"); \
    __asm volatile ("sub sp, #4\n"); \
    call(b, value) \
    __asm volatile ("add sp, #4\n"); \
    __asm volatile ("pop {lr}\n");

static id (*_objc_msgSend)(id, SEL, ...);

__attribute__((__naked__))
static void $objc_msgSend() {
    save()
        __asm volatile ("mov r2, r1\n");
        __asm volatile ("mov r1, #0\n");
        __asm volatile ("add r3, sp, #8\n");

        link(blx, &$objc_msgSend$)
    load()

    call(bx, _objc_msgSend)
}

static void (*_objc_msgSend_stret)(id, SEL, ...);

__attribute__((__naked__))
static void $objc_msgSend_stret() {
    save()
        __asm volatile ("mov r0, r1\n");
        __asm volatile ("mov r1, #0\n");
        __asm volatile ("add r3, sp, #12\n");

        link(blx, &$objc_msgSend$)
    load()

    call(bx, _objc_msgSend_stret)
}

static id (*_objc_msgSendSuper)(struct objc_super *, SEL, ...);

__attribute__((__naked__))
static void $objc_msgSendSuper() {
    save()
        __asm volatile ("mov r2, r1\n");
        __asm volatile ("ldr r1, [r0, #4]\n");
        __asm volatile ("ldr r0, [r0, #0]\n");
        __asm volatile ("add r3, sp, #8\n");

        link(blx, &$objc_msgSend$)
    load()

    call(bx, _objc_msgSendSuper)
}

static void (*_objc_msgSendSuper_stret)(struct objc_super *, SEL, ...);

__attribute__((__naked__))
static void $objc_msgSendSuper_stret() {
    save()
        __asm volatile ("ldr r0, [r1, #0]\n");
        __asm volatile ("ldr r1, [r1, #4]\n");
        __asm volatile ("add r3, sp, #12\n");

        link(blx, &$objc_msgSend$)
    load()

    call(bx, _objc_msgSendSuper_stret)
}

static id (*_objc_msgSendSuper2)(struct objc_super *, SEL, ...);

__attribute__((__naked__))
static void $objc_msgSendSuper2() {
    save()
        __asm volatile ("ldr r0, [r0, #4]\n");
        link(blx, class_getSuperclass)
        __asm volatile ("mov r12, r0");
    load()

    save()
        __asm volatile ("mov r2, r1\n");
        __asm volatile ("ldr r0, [r0, #0]\n");
        __asm volatile ("mov r1, r12\n");
        __asm volatile ("add r3, sp, #8\n");

        link(blx, &$objc_msgSend$)
    load()

    call(bx, _objc_msgSendSuper2)
}

static void (*_objc_msgSendSuper2_stret)(struct objc_super *, SEL, ...);

__attribute__((__naked__))
static void $objc_msgSendSuper2_stret() {
    save()
        __asm volatile ("ldr r0, [r1, #4]\n");
        link(blx, class_getSuperclass)
        __asm volatile ("mov r12, r0");
    load()

    save()
        __asm volatile ("ldr r0, [r1, #0]\n");
        __asm volatile ("mov r1, r12\n");
        __asm volatile ("add r3, sp, #12\n");

        link(blx, &$objc_msgSend$)
    load()

    call(bx, _objc_msgSendSuper2_stret)
}

MSInitialize {
    pthread_key_create(&key_, &DestroyData);

    banned_.insert(@selector(alloc));
    banned_.insert(@selector(autorelease));
    banned_.insert(@selector(class));
    banned_.insert(@selector(copy));
    banned_.insert(@selector(copyWithZone:));
    banned_.insert(@selector(dealloc));
    banned_.insert(@selector(delegate));
    banned_.insert(@selector(isKindOfClass:));
    banned_.insert(@selector(lock));
    banned_.insert(@selector(retain));
    banned_.insert(@selector(release));
    banned_.insert(@selector(unlock));

    MSImageRef libobjc(MSGetImageByName("/usr/lib/libobjc.A.dylib"));
    MSHookSymbol(_class_getMethodNoSuper, "__class_getMethodNoSuper", libobjc);

    MSHookSymbol(realizeClass, "__ZL12realizeClassP7class_t", libobjc);
    if (realizeClass == NULL)
        MSHookSymbol(realizeClass, "_realizeClass", libobjc);

    MSHookFunction(&objc_msgSend, (id (*)(id, SEL, ...)) MSHake(objc_msgSend));
    MSHookFunction(&objc_msgSend_stret, (void (*)(id, SEL, ...)) MSHake(objc_msgSend_stret));

    MSHookFunction(&objc_msgSendSuper, (id (*)(struct objc_super *, SEL, ...)) MSHake(objc_msgSendSuper));
    MSHookFunction(&objc_msgSendSuper_stret, (void (*)(struct objc_super *, SEL, ...)) MSHake(objc_msgSendSuper_stret));

    MSHookFunction(libobjc, "objc_msgSendSuper2", (id (*)(struct objc_super *, SEL, ...)) MSHake(objc_msgSendSuper2));
    MSHookFunction(libobjc, "objc_msgSendSuper2_stret", (void (*)(struct objc_super *, SEL, ...)) MSHake(objc_msgSendSuper2_stret));
}
