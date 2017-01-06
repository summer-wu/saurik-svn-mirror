#include "cxx/platform.hpp"

#include <typeinfo> //OK
#include <setjmp.h> //OK

#include <unwind.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {

struct __cxa_exception {
    std::type_info *type;
    void (*destructor)(void *);

    std::unexpected_handler unexpected;
    std::terminate_handler terminate;

    __cxa_exception *next;

    int handlers;
    int handler_switch;
    const char *action;
    const uint8_t *lsda;
    void *temporary;
    void *adjusted;

    _Unwind_Exception unwind_header;
};

}

extern "C" {

/*struct LsdaHeader {
    _Unwind_Ptr start_;
    _Unwind_Ptr landingpad_;
    _Unwind_Ptr ttype_base;

    const uint8_t *ttype;
    const uint8_t *action_table;

    uint8_t ttype_encoding;
    uint8_t call_site_encoding;

    LsdaHeader(_Unwind_Context *context, const uint8_t *&pointer) {
        start_ = _Unwind_GetRegionStart(context);
        uint8_t encoding(*pointer++);
        if (encoding == DW_EH_PE_omit)
            ReadEncoded_(context, encoding, pointer);
    }
};*/

struct __cxa_eh_globals {
    __cxa_exception *caught;
    unsigned int uncaught;
};

__cxa_eh_globals globals_ = {0, 0};

__cxa_exception *__cxa_get_exception_ptr(void *object) {
    return reinterpret_cast<__cxa_exception *>(object) - 1;
}

void __cxa_free_exception(void *object) {
    printf("__cxa_free_exception()\n");
    __cxa_exception *exception(__cxa_get_exception_ptr(object));
    free(exception);
}

static void __gxx_exception_cleanup(_Unwind_Reason_Code code, _Unwind_Exception *unwind_header) {
    void *object(unwind_header + 1);
    __cxa_exception *exception(__cxa_get_exception_ptr(object));

    if (exception->destructor)
        exception->destructor(object);

    __cxa_free_exception(object);
}

uint64_t __gxx_exception_class(
    uint64_t('G') << 56 |
    uint64_t('N') << 48 |
    uint64_t('U') << 40 |
    uint64_t('C') << 32 |
    uint64_t('C') << 24 |
    uint64_t('+') << 16 |
    uint64_t('+') << 8 |
    uint64_t('\0')
);

/*void free(void *q) {
    printf("-: %p\n", q);
}*/

void *__cxa_allocate_exception(size_t size) {
    printf("__cxa_allocate_exception()\n");
    __cxa_exception *exception(reinterpret_cast<__cxa_exception *>(malloc(size + sizeof(__cxa_exception))));
    memset(exception, 0, sizeof(__cxa_exception));
    printf("+: %p\n", exception);
    return exception + 1;
}

void *__cxa_begin_catch(void *unwind_header) {
    printf("__cxa_begin_catch(%p)\n", unwind_header);

    __cxa_exception *exception(__cxa_get_exception_ptr(reinterpret_cast<_Unwind_Exception *>(unwind_header) + 1));

    if (exception->handlers < 0)
        exception->handlers = -exception->handlers + 1;
    else {
        ++exception->handlers;
        --globals_.uncaught;
    }

    exception->next = globals_.caught;
    globals_.caught = exception;

    return exception->adjusted;
}

void __cxa_end_catch() {
    printf("__cxa_end_catch()\n");

    __cxa_exception *exception(globals_.caught);

    if (--exception->handlers == 0) {
        globals_.caught = exception->next;
        _Unwind_DeleteException(&exception->unwind_header);
    }
}

void unexpected_handler() {
    printf("unexpected?()\n");
}

void terminate_handler() {
    printf("terminate?()\n");
    __cxa_end_catch();
    __asm__ ("mov $0x7, %eax; leave; leave; leave; ret");
}

_Unwind_Reason_Code __stop_function(int, _Unwind_Action action, _Unwind_Exception_Class, _Unwind_Exception *, _Unwind_Context *, void *) {
    printf("__stop_function(%d)\n", action);
    static int i(0);
    if ((action & _UA_END_OF_STACK) != 0) {
        printf("cxa: attempting catastrophic unwinder recovery\n");
        __asm__ ("mov $0x7, %eax; add $0x14, %esp; pop %ebx; pop %ebp; leave; ret");
    }
    return _URC_NO_REASON;
}

void __cxa_throw(void *object, std::type_info *type, void (*destructor) (void *)) {
    printf("__cxa_throw()\n");

    __cxa_exception *exception(__cxa_get_exception_ptr(object));

    exception->unexpected = &unexpected_handler;
    exception->terminate = &terminate_handler;

    exception->type = type;
    exception->destructor = destructor;

    exception->unwind_header.exception_class = __gxx_exception_class;
    exception->unwind_header.exception_cleanup = __gxx_exception_cleanup;

    ++globals_.uncaught;

    _Unwind_Reason_Code code;

    //try {
        code = _Unwind_RaiseException(&exception->unwind_header);
    /*} catch (...) {
        throw;
    }*/

    if (code == _URC_END_OF_STACK) {
        __cxa_begin_catch(&exception->unwind_header);
        __cxa_end_catch();
        code = _Unwind_ForcedUnwind(&exception->unwind_header, &__stop_function, NULL);
        //_Unwind_Resume(&exception->unwind_header);
    }

    printf("uncaught exception: %d\n", code);
    __cxa_begin_catch(&exception->unwind_header);
    std::terminate();
}

void __cxa_rethrow() {
    printf("__cxa_rethrow()\n");

    __cxa_exception *exception(globals_.caught);

    exception->handlers = -exception->handlers;
    _Unwind_Reason_Code code(_Unwind_Resume_or_Rethrow(&exception->unwind_header));

    printf("uncaught exception: %d\n", code);

    __cxa_begin_catch(&exception->unwind_header);
    std::terminate();
}

/*_Unwind_Reason_Code __gxx_personality_v0(int version, _Unwind_Action actions, _Unwind_Exception_Class _class, _Unwind_Exception *unwind_header, _Unwind_Context *context) {
    printf("__gxx_personality_v0()\n");

    // XXX: what if this happens during phase 2?
    if (version != 1)
        return _URC_FATAL_PHASE1_ERROR;

    _assert(_class == __gxx_exception_class);

    __cxa_exception *exception(__cxa_get_exception_ptr(unwind_header + 1));

    int handler_switch;
    _Unwind_Ptr landingpad;
    const uint8_t *lsda;

    if (actions == (_UA_CLEANUP_PHASE | _UA_HANDLER_FRAME)) {
        handler_switch = exception->handler_switch;
        landingpad = reinterpret_cast<_Unwind_Ptr>(exception->temporary);
        lsda = exception->lsda;
    } else {
        lsda = reinterpret_cast<const uint8_t *>(_Unwind_GetLanguageSpecificData(context));

        const uint8_t *pointer(lsda);
        be::LsdaHeader header(context, pointer);
    }

    if (actions == _UA_SEARCH_PHASE) {
        exception->handler_switch = handler_switch;
        exception->temporary = landingpad;
        exception->lsda = lsda;

        return _URC_HANDLER_FOUND;
    } else {
        _Unwind_SetGR(context, __builtin_eh_return_data_regno(0), __builtin_extend_pointer(&exception->unwind_header));
        _Unwind_SetGR(context, __builtin_eh_return_data_regno(1), handler_switch);
        _Unwind_SetIP(context, landingpad);

        return _URC_INSTALL_CONTEXT;
    }
}*/

}
