#include "minimal/stdlib.h"
#include "minimal/string.h"
#include "minimal/hexdump.h"

#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/mman.h>

#include <objc/objc-runtime.h>
#include <objc/objc-class.h>

#include <Foundation/NSInvocation.h>
#include <Foundation/NSMethodSignature.h>

#include <dlfcn.h>
#include <ffi.h>

#include "joc/Cast.h"
#include "joc/Java.h"
#include "joc/Memory.h"
#include "joc/NativeException.h"
#include "joc/Pointer.h"
#include "joc/RuntimeInfo.h"
#include "joc/Selector.h"
#include "joc/Strap.h"
#include "joc/String.h"

#include "sig/ffi_type.h"
#include "sig/parse.h"

#if 0
#define lprintf(...) fprintf(stderr, __VA_ARGS__)
#else
#define lprintf(...)
#endif

#if !defined(__cplusplus)  &&  !__OBJC2__
#define super_class_ class
#else
#define super_class_ super_class
#endif

#define HAVE_WEAKREFS

jobject joc_Globalize(JNIEnv *env, jobject local) {
    jobject global = (*(*env)->NewGlobalRef)(env, local);
    (*(*env)->DeleteLocalRef)(env, local);
    return global;
}

jobject joc_Weaken(JNIEnv *env, jobject object) {
#ifdef HAVE_WEAKREFS
    return (*(*env)->NewWeakGlobalRef)(env, object);
#else
    object = (*(*env)->NewObject)(env, java_lang_ref_WeakReference, java_lang_ref_WeakReference_$init$, object);
    _assert(object != NULL);
    return joc_Globalize(env, object);
#endif
}

jobject joc_Embolden(JNIEnv *env, jobject object) {
#ifdef HAVE_WEAKREFS
    return (*(*env)->NewLocalRef)(env, object);
#else
    return (*(*env)->CallObjectMethod)(env, object, java_lang_ref_Reference_get);
#endif
}

jobject joc_Strengthen(JNIEnv *env, jobject object) {
#ifdef HAVE_WEAKREFS
    return (*(*env)->NewGlobalRef)(env, object);
#else
    object = (*(*env)->CallObjectMethod)(env, object, java_lang_ref_Reference_get);
    _assert(object != NULL);
    return joc_Globalize(env, object);
#endif
}

void joc_Dissipate(JNIEnv *env, jobject object) {
#ifdef HAVE_WEAKREFS
    (*(*env)->DeleteWeakGlobalRef)(env, object);
#else
    (*(*env)->DeleteGlobalRef)(env, object);
#endif
}

@protocol JOCStrap
+ (id) allocWithObject:(jobject)object;
@end

struct meta_strap {
    JNIEnv *joc_env;
    jclass joc_class;
    jmethodID joc_method;
};

struct instance_strap {
    jobject joc_object;
};

struct JocVariable {
    char *name_;
    char *type_;
    size_t size_;
    size_t alignment_;
};

/*static struct JocVariable MetaVariables[] = {
    {"joc_env", "^v", sizeof(JNIEnv *), sizeof(JNIEnv *)},
    {"joc_class", "^v", sizeof(jclass), sizeof(jclass)},
    {"joc_method", "^v", sizeof(jclass), sizeof(jclass)}
};*/

static struct JocVariable InstanceVariables[] = {
    {"joc_object", "^v", sizeof(jobject), sizeof(jobject)}
};

/* TAG: gc */
id joc_Strap_alloc(id self, SEL sel) {
    struct meta_strap *meta_strap = (struct meta_strap *) object_getIndexedIvars(self);
    _assert(meta_strap->joc_env != NULL);
    _assert(meta_strap->joc_class != NULL);
    _assert(meta_strap->joc_method != NULL);

    JNIEnv *joc_env = meta_strap->joc_env;

    lprintf("alloc(): joc_env:%p joc_class:%p joc_method:%p\n", joc_env, meta_strap->joc_class, meta_strap->joc_method);

    jobject object = (*(*joc_env)->NewObject)(joc_env, meta_strap->joc_class, meta_strap->joc_method);
    /* XXX: exception might get thrown */
    _assert(object != NULL);

    id cobject = [joc_FromObject(joc_env, object) retain];
    (*(*joc_env)->DeleteLocalRef)(joc_env, object);
    return cobject;
}

bool joc_IsJava(Class _class) {
    int version = class_getVersion(_class);
    lprintf("joc_IsJava(%p:%s) getVersion():0x%.8x\n", _class, class_getName(_class), version);
    return version == 0xcafebabe;
}

Class joc_GetBase(Class _class) {
    while (joc_IsJava(_class))
        _class = class_getSuperclass(_class);
    return _class;
}

JNIEnv *joc_GetEnv(Class _class) {
    struct meta_strap *meta_strap = (struct meta_strap *) object_getIndexedIvars(_class);
    _assert(meta_strap->joc_env != NULL);
    return meta_strap->joc_env;
}

/* TAG: gc */
id joc_Strap_allocWithObject(id self, SEL sel, jobject object) {
    JNIEnv *joc_env = joc_GetEnv(self);

    struct objc_super super = {self, joc_GetBase(self)->isa};
    id cobject = objc_msgSendSuper(&super, @selector(alloc));
    _assert(cobject != NULL);
    _assert(cobject->isa == self);

    object = joc_Weaken(joc_env, object);
    object_setInstanceVariable(cobject, "joc_object", (void *) object);

    lprintf("allocWithObject(%p:%s)=%u\n", cobject, class_getName(cobject->isa), [cobject retainCount]);
    return cobject;
}

struct joc_userdata {
    struct sig_signature *signature;
    JNIEnv *env;
    jmethodID method;
    ffi_cif *cif;
    void (*fn)();
};

/* TAG: gc */
void joc_closure(ffi_cif *cif, void *result, void **arguments, void *arg) {
    struct joc_userdata *userdata = (struct joc_userdata *) arg;

    lprintf("joc_closure(%p)\n", userdata);

    JNIEnv *env = userdata->env;

    id self = * (id *) arguments[0];

    jobject _this;
    object_getInstanceVariable(self, "joc_object", (void **) &_this);
    _assert(_this != NULL);

#ifndef HAVE_WEAKREFS
    if ((*(*env)->IsInstanceOf)(env, _this, java_lang_ref_Reference))
        _this = (*(*env)->CallObjectMethod)(env, _this, java_lang_ref_Reference_get);
    else
#endif
        _this = (*(*env)->NewLocalRef)(env, _this);

    _assert(_this != NULL);

    /*SEL sel = * (SEL *) arguments[1];
    fprintf(stderr, "self:%p old_this:%p @selector(%s)\n", self, _this, sel_getName(sel));*/

    void *values[userdata->cif->nargs];
    values[0] = &env;
    values[1] = &_this;
    values[2] = &userdata->method;

    apr_pool_t *pool;
    apr_pool_create(&pool, apr_pool_);

    size_t size = 0;

    for (size_t index = 3; index != userdata->cif->nargs; ++index)
        size += userdata->cif->arg_types[index]->size;

    /* XXX: alignment */
    uint8_t data[size];

    size = 0;
    for (size_t index = 3; index != userdata->cif->nargs; ++index) {
        ffi_type *ffi_type = userdata->cif->arg_types[index];
        values[index] = data + size;
        java_copy_to(pool, userdata->signature->elements[index].type, ffi_type, arguments[index - 1], env, values[index]);
        size += ffi_type->size;
    }

    /* XXX: alignment */
    uint8_t value[userdata->cif->rtype->size];
    ffi_call(userdata->cif, userdata->fn, value, values);

    jthrowable error = ((*env)->ExceptionOccurred)(env);
    if (error == NULL)
        java_copy_from(pool, userdata->signature->elements[0].type, userdata->cif->rtype, result, env, value);

    (*(*env)->DeleteLocalRef)(env, _this);
    apr_pool_destroy(pool);

    if (error != NULL)
        @throw joc_FromObject(env, error);
}

#define UseClosures true

void ejprint(JNIEnv *env, jstring jvalue) {
    const char *cvalue = (*(*env)->GetStringUTFChars)(env, jvalue, NULL);
    fprintf(stderr, "%s\n", cvalue);
    (*(*env)->ReleaseStringUTFChars)(env, jvalue, cvalue);
}

void joc_ToName_(char *jname) {
    size_t size = strlen(jname);
    for (size_t i = 0; i != size; ++i)
        if (jname[i] == ':')
            jname[i] = '$';
}

jobject joc_ToMethod(JNIEnv *env, jobject _this, SEL sel) {
    jclass jclass = (*(*env)->GetObjectClass)(env, _this);
    _assert(jclass != NULL);

    jstring oname;
    const char *sname = sel_getName(sel);
    oname = (*(*env)->NewStringUTF)(env, sname);
    jstring jname;
    char *cname = strdup(sname);
    joc_ToName_(cname);
    jname = (*(*env)->NewStringUTF)(env, cname);
    free(cname);

    jobjectArray java_methods = (*(*env)->CallObjectMethod)(env, jclass, java_lang_Class_getDeclaredMethods, jname, NULL);
    jsize method_count = (*(*env)->GetArrayLength)(env, java_methods);
    (*(*env)->DeleteLocalRef)(env, jclass);

    jobject method = NULL;

    for (jsize method_index = 0; method_index != method_count; ++method_index) {
        jobject java_method = (*(*env)->GetObjectArrayElement)(env, java_methods, method_index);
        jobject message = (*(*env)->CallObjectMethod)(env, java_method, java_lang_reflect_Method_getAnnotation, joc_Message);

        if (message == NULL)
            goto free_method;

        jstring message_jname = (*(*env)->CallObjectMethod)(env, message, joc_Message_name);
        (*(*env)->DeleteLocalRef)(env, message);

        jstring name;

        if ((*(*env)->GetStringLength)(env, message_jname) != 0)
            name = oname;
        else {
            name = jname;
            (*(*env)->DeleteLocalRef)(env, message_jname);
            message_jname = (*(*env)->CallObjectMethod)(env, java_method, java_lang_reflect_Method_getName);
        }

        if ((*(*env)->CallBooleanMethod)(env, name, java_lang_Object_equals, message_jname))
            method = java_method;
        (*(*env)->DeleteLocalRef)(env, message_jname);
        if (method != NULL)
            break;
      free_method:
        (*(*env)->DeleteLocalRef)(env, java_method);
    }

    (*(*env)->DeleteLocalRef)(env, java_methods);
    (*(*env)->DeleteLocalRef)(env, jname);
    (*(*env)->DeleteLocalRef)(env, oname);

    return method;
}

BOOL joc_Strap_respondsToSelector$(id self, SEL sel, SEL target) {
    lprintf("respondsToSelector$(%p, %s)\n", self, sel_getName(target));

    JNIEnv *env = joc_GetEnv(self->isa);

    jobject _this;
    object_getInstanceVariable(self, "joc_object", (void **) &_this);
    _assert(_this != NULL);

    jobject java_method = joc_ToMethod(env, _this, target);

    if (java_method == NULL) {
        struct objc_super super = {self, joc_GetBase(self->isa)};
        return (BOOL) (uintptr_t) objc_msgSendSuper(&super, sel, target);
    } else {
        (*(*env)->DeleteLocalRef)(env, java_method);
        return YES;
    }
}

NSMethodSignature *joc_Strap_methodSignatureForSelector$(id self, SEL sel, SEL target) {
    lprintf("methodSignatureForSelector$(%p, %s)\n", self, sel_getName(target));

    JNIEnv *env = joc_GetEnv(self->isa);

    jobject _this;
    object_getInstanceVariable(self, "joc_object", (void **) &_this);
    _assert(_this != NULL);

    jobject java_method = joc_ToMethod(env, _this, target);

    if (java_method == NULL) {
        struct objc_super super = {self, joc_GetBase(self->isa)};
        return objc_msgSendSuper(&super, sel, target);
    } else {
        jobject message = (*(*env)->CallObjectMethod)(env, java_method, java_lang_reflect_Method_getAnnotation, joc_Message);
        (*(*env)->DeleteLocalRef)(env, java_method);

        jstring jtypes = (*(*env)->CallObjectMethod)(env, message, joc_Message_types);
        (*(*env)->DeleteLocalRef)(env, message);

        if ((*(*env)->GetStringLength)(env, jtypes) == 0) {
            (*(*env)->DeleteLocalRef)(env, jtypes);
            jtypes = (*(*env)->CallStaticObjectMethod)(env, joc_Runtime, joc_Runtime_getSignature, java_method);
        }

        const char *ctypes = (*(*env)->GetStringUTFChars)(env, jtypes, NULL);
        _assert(ctypes != NULL);

        NSMethodSignature *signature = [NSMethodSignature signatureWithObjCTypes:ctypes];

        (*(*env)->ReleaseStringUTFChars)(env, jtypes, ctypes);
        (*(*env)->DeleteLocalRef)(env, jtypes);

        return signature;
    }
}

void (*java_instance_call(struct sig_type *type, JNIEnv *env)) {
    switch (type->primitive) {
        case typename_P: return FFI_FN((*env)->CallObjectMethod);
        case union_P: return FFI_FN((*env)->CallObjectMethod);
        case string_P: return FFI_FN((*env)->CallObjectMethod);
        case selector_P: return FFI_FN((*env)->CallObjectMethod);
        case object_P: return FFI_FN((*env)->CallObjectMethod);
        case boolean_P: return FFI_FN((*env)->CallBooleanMethod);
        case uchar_P: return FFI_FN((*env)->CallByteMethod);
        case uint_P: return FFI_FN((*env)->CallIntMethod);
        case ulong_P: return FFI_FN((*env)->CallIntMethod);
        case ulonglong_P: return FFI_FN((*env)->CallLongMethod);
        case ushort_P: return FFI_FN((*env)->CallShortMethod);
        case array_P: return FFI_FN((*env)->CallObjectMethod);
        case pointer_P: return FFI_FN((*env)->CallObjectMethod);
        case bit_P: _assert(false); break;
        case char_P: return FFI_FN((*env)->CallByteMethod);
        case double_P: return FFI_FN((*env)->CallDoubleMethod);
        case float_P: return FFI_FN((*env)->CallFloatMethod);
        case int_P: return FFI_FN((*env)->CallIntMethod);
        case long_P: return FFI_FN((*env)->CallIntMethod);
        case longlong_P: return FFI_FN((*env)->CallLongMethod);
        case short_P: return FFI_FN((*env)->CallShortMethod);
        case void_P: return FFI_FN((*env)->CallVoidMethod);
        case struct_P: return FFI_FN((*env)->CallObjectMethod);
        default: _assert(false); break;
    }
}

void (*java_static_call(struct sig_type *type, JNIEnv *env)) {
    switch (type->primitive) {
        case typename_P: return FFI_FN((*env)->CallStaticObjectMethod);
        case union_P: return FFI_FN((*env)->CallStaticObjectMethod);
        case string_P: return FFI_FN((*env)->CallStaticObjectMethod);
        case selector_P: return FFI_FN((*env)->CallStaticObjectMethod);
        case object_P: return FFI_FN((*env)->CallStaticObjectMethod);
        case boolean_P: return FFI_FN((*env)->CallStaticBooleanMethod);
        case uchar_P: return FFI_FN((*env)->CallStaticByteMethod);
        case uint_P: return FFI_FN((*env)->CallStaticIntMethod);
        case ulong_P: return FFI_FN((*env)->CallStaticIntMethod);
        case ulonglong_P: return FFI_FN((*env)->CallStaticLongMethod);
        case ushort_P: return FFI_FN((*env)->CallStaticShortMethod);
        case array_P: return FFI_FN((*env)->CallStaticObjectMethod);
        case pointer_P: return FFI_FN((*env)->CallStaticObjectMethod);
        case bit_P: _assert(false); break;
        case char_P: return FFI_FN((*env)->CallStaticByteMethod);
        case double_P: return FFI_FN((*env)->CallStaticDoubleMethod);
        case float_P: return FFI_FN((*env)->CallStaticFloatMethod);
        case int_P: return FFI_FN((*env)->CallStaticIntMethod);
        case long_P: return FFI_FN((*env)->CallStaticIntMethod);
        case longlong_P: return FFI_FN((*env)->CallStaticLongMethod);
        case short_P: return FFI_FN((*env)->CallStaticShortMethod);
        case void_P: return FFI_FN((*env)->CallStaticVoidMethod);
        case struct_P: return FFI_FN((*env)->CallStaticObjectMethod);
        default: _assert(false); break;
    }
}

void joc_Strap_forwardInvocation$(id self, SEL sel, NSInvocation *invocation) {
    lprintf("forwardInvocation$(%p)\n", self);

    JNIEnv *env = joc_GetEnv(self->isa);

    jobject _this;
    object_getInstanceVariable(self, "joc_object", (void **) &_this);
    _assert(_this != NULL);

    jobject java_method = joc_ToMethod(env, _this, [invocation selector]);

    if (java_method == NULL) {
        struct objc_super super = {self, joc_GetBase(self->isa)};
        objc_msgSendSuper(&super, sel, invocation);
    } else {
        jobject message = (*(*env)->CallObjectMethod)(env, java_method, java_lang_reflect_Method_getAnnotation, joc_Message);
        (*(*env)->DeleteLocalRef)(env, java_method);

        jstring jtypes = (*(*env)->CallObjectMethod)(env, message, joc_Message_types);
        (*(*env)->DeleteLocalRef)(env, message);

        if ((*(*env)->GetStringLength)(env, jtypes) == 0) {
            (*(*env)->DeleteLocalRef)(env, jtypes);
            jtypes = (*(*env)->CallStaticObjectMethod)(env, joc_Runtime, joc_Runtime_getSignature, java_method);
        }

        const char *message_ctypes = (*(*env)->GetStringUTFChars)(env, jtypes, NULL);
        _assert(message_ctypes != NULL);

        apr_pool_t *pool;
        apr_pool_create(&pool, apr_pool_);

        jint method_modifiers = (*(*env)->CallIntMethod)(env, java_method, java_lang_reflect_Method_getModifiers);
        jboolean method_static = (*(*env)->CallStaticBooleanMethod)(env, java_lang_reflect_Modifier, java_lang_reflect_Modifier_isStatic, method_modifiers);

        struct sig_signature *signature = apr_palloc(pool, sizeof(struct sig_signature));
        sig_parse(pool, signature, message_ctypes);

        (*(*env)->ReleaseStringUTFChars)(env, jtypes, message_ctypes);
        (*(*env)->DeleteLocalRef)(env, jtypes);

        ffi_type **java_types = apr_palloc(pool, (signature->count - 1 + 3) * sizeof(ffi_type));
        java_types[0] = &ffi_type_pointer;
        java_types[1] = &ffi_type_pointer;
        java_types[2] = &ffi_type_pointer;

        ffi_cif *java_cif = apr_palloc(pool, sizeof(ffi_cif));
        sig_ffi_cif(pool, &sig_java_ffi_type, signature, java_cif, java_types, 2, 3);

        ffi_type **objc_types = apr_palloc(pool, (signature->count - 1) * sizeof(ffi_type));
        ffi_cif *objc_cif = apr_palloc(pool, sizeof(ffi_cif));
        sig_ffi_cif(pool, &sig_objc_ffi_type, signature, objc_cif, objc_types, 0, 0);

        struct joc_userdata userdata;
        userdata.signature = signature;
        userdata.env = env;
        userdata.method = (*(*env)->FromReflectedMethod)(env, java_method);
        userdata.cif = java_cif;
        userdata.fn = (*(method_static ? &java_static_call : &java_instance_call))(signature->elements[0].type, env);

        void **arguments = apr_palloc(pool, sizeof(void *) * signature->count);

        for (size_t index = 0; index != signature->count - 1; ++index) {
            arguments[index] = apr_palloc(pool, objc_cif->arg_types[index]->size);
            [invocation getArgument:arguments[index] atIndex:index];
        }

        void *result = apr_palloc(pool, objc_cif->rtype->size);
        joc_closure(NULL, result, arguments, &userdata);
        [invocation setReturnValue:result];

        apr_pool_destroy(pool);
    }
}

jobject joc_static(id self, SEL sel, JNIEnv *env) {
    struct meta_strap *meta_strap = (struct meta_strap *) object_getIndexedIvars(self);
    _assert(meta_strap->joc_class != NULL);
    return (*(*env)->NewLocalRef)(env, meta_strap->joc_class);
}

jobject joc_instance(id self, SEL sel, JNIEnv *env) {
    jobject _this;
    object_getInstanceVariable(self, "joc_object", (void **) &_this);
    return joc_Embolden(env, _this);
}

/* TAG: gc */
void joc_Strap_dealloc(id self, SEL sel) {
    lprintf("dealloc(%p:%s)\n", self, class_getName(self->isa));

    JNIEnv *env = joc_GetEnv(self->isa);

    jobject _this;
    object_getInstanceVariable(self, "joc_object", (void **) &_this);
    _assert(_this != NULL);

    joc_Dissipate(env, _this);

    struct objc_super super = {self, joc_GetBase(self->isa)};
    objc_msgSendSuper(&super, sel);
}

/* TAG: gc */
void joc_Strap_release(id self, SEL sel) {
    unsigned count = [self retainCount];
    lprintf("release(%p:%s)=%u>%u\n", self, class_getName(self->isa), count, count - 1);

    if (count == 2) {
        JNIEnv *env = joc_GetEnv(self->isa);

        jobject _this;
        object_getInstanceVariable(self, "joc_object", (void **) &_this);
        _assert(_this != NULL);

        jobject object = joc_Weaken(env, _this);
        object_setInstanceVariable(self, "joc_object", (void *) object);
        (*(*env)->DeleteGlobalRef)(env, _this);
    }

    struct objc_super super = {self, joc_GetBase(self->isa)};
    objc_msgSendSuper(&super, sel);
}

/* TAG: gc */
id joc_Strap_retain(id self, SEL sel) {
    unsigned count = [self retainCount];
    lprintf("retain(%p:%s)=%u>%u\n", self, class_getName(self->isa), count, count + 1);

    if (count == 1) {
        JNIEnv *env = joc_GetEnv(self->isa);

        jobject _this;
        object_getInstanceVariable(self, "joc_object", (void **) &_this);
        _assert(_this != NULL);

        jobject object = joc_Strengthen(env, _this);
        object_setInstanceVariable(self, "joc_object", (void *) object);
        joc_Dissipate(env, _this);
    }

    struct objc_super super = {self, joc_GetBase(self->isa)};
    return objc_msgSendSuper(&super, sel);
}

struct JocMethod {
    char *name_;
    char *type_;
    IMP code_;
};

struct JocMethod MetaMethods[] = {
    //{"methodSignatureForSelector:", "@12@0:4:8", (IMP) &joc_Strap_methodSignatureForSelector$},
    {"alloc", "@8@0:4", (IMP) &joc_Strap_alloc},
    {"allocWithObject:", "@12@0:4^v8", (IMP) &joc_Strap_allocWithObject},
    {"_jocStrap:", "@12@0:4^v8", (IMP) &joc_static}
};

struct JocMethod InstanceMethods[] = {
    {"_jocStrap:", "@12@0:4^v8", (IMP) &joc_instance},
    {"dealloc", "v8@0:4", (IMP) &joc_Strap_dealloc},
    {"release", "v8@0:4", (IMP) &joc_Strap_release},
    {"retain", "@8@0:4", (IMP) &joc_Strap_retain}
};

struct JocMethod DynamicMethods[] = {
    {"respondsToSelector:", "c12@0:4:8", (IMP) &joc_Strap_respondsToSelector$},
    {"methodSignatureForSelector:", "@12@0:4:8", (IMP) &joc_Strap_methodSignatureForSelector$},
    {"forwardInvocation:", "v12@0:4@8", (IMP) &joc_Strap_forwardInvocation$},
};

void AddVariables(Class _class, struct JocVariable *variables, size_t count) {
    size_t i;
    for (i = 0; i != count; ++i) {
        struct JocVariable *variable = &variables[i];
        _assert(class_addIvar(_class, variable->name_, variable->size_, variable->alignment_, variable->type_));
    }
}

#define AddVariables_(_class, variables) \
    AddVariables(_class, variables, sizeof(variables) / sizeof(*variables))

jclass joc_ToClass(JNIEnv *env, Class value) {
    _assert(false);
    return NULL;
}

void AddMethods(Class _class, struct JocMethod *methods, size_t count) {
    size_t i;
    for (i = 0; i != count; ++i) {
        struct JocMethod *method = &methods[i];
        if (!class_addMethod(_class, sel_registerName(method->name_), method->code_, method->type_)) {
            fprintf(stderr, "failed to add method %s\n", method->name_);
            _assert(false);
        }
    }
}

#define AddMethods_(_class, methods) \
    AddMethods(_class, methods, sizeof(methods) / sizeof(*methods))

Class joc_FromClass(JNIEnv *env, jclass value) {
    if (value == NULL)
        return NULL;

    bool is_objc_class;
    char *objc_class_name; {
        jstring java_class_name = (*(*env)->CallObjectMethod)(env, value, java_lang_Class_getName);
        const char *c_class_name = (*(*env)->GetStringUTFChars)(env, java_class_name, NULL);
        lprintf("joc_FromClass(%s)\n", c_class_name);
        is_objc_class = strncmp(c_class_name, "obc.", 4) == 0;
        objc_class_name = joc_FromName(c_class_name);
        (*(*env)->ReleaseStringUTFChars)(env, java_class_name, c_class_name);
        (*(*env)->DeleteLocalRef)(env, java_class_name);
    }

    Class objc_class = objc_getClass(objc_class_name);
    if (objc_class != NULL) {
        free(objc_class_name);
        return [[objc_class retain] autorelease];
    }

    if (is_objc_class) {
        fprintf(stderr, "objc_getClass(\"%s\")\n", objc_class_name);
        _assert(false);
    }

    Class instance_super; {
        jclass java_super = (*(*env)->CallObjectMethod)(env, value, java_lang_Class_getSuperclass);
        instance_super = joc_FromClass(env, java_super);
        _assert(instance_super != NULL);
        (*(*env)->DeleteLocalRef)(env, java_super);
    }

    Class instance_class = objc_allocateClassPair(instance_super, objc_class_name, sizeof(struct meta_strap));
    Class meta_class = object_getClass(instance_class);
    struct meta_strap *meta_strap = (struct meta_strap *) object_getIndexedIvars(instance_class);

    meta_strap->joc_env = env;

    meta_strap->joc_class = (*(*env)->NewGlobalRef)(env, value);
    _assert(meta_strap->joc_class != NULL);

    meta_strap->joc_method = (*(*env)->GetMethodID)(env, value, "<init>",  "()V");
    if (meta_strap->joc_method == NULL)
        (*(*env)->ExceptionClear)(env);

    lprintf("meta_strap(%p):%p ->joc_env:%p ->joc_class:%p ->joc_method:%p\n", instance_class, meta_strap, meta_strap->joc_env, meta_strap->joc_class, meta_strap->joc_method);
    //fprintf(stderr, "instance_class:%p ->info:%.8lx meta_class:%p\n", instance_class, instance_class->info, meta_class);

    if (!joc_IsJava(instance_super)) {
        if (!UseClosures)
            AddMethods_(instance_class, DynamicMethods);
        AddMethods_(meta_class, MetaMethods);
        AddMethods_(instance_class, InstanceMethods);
    }

    if (UseClosures) {
        jobjectArray java_methods = (*(*env)->CallObjectMethod)(env, value, java_lang_Class_getDeclaredMethods);
        jsize method_count = (*(*env)->GetArrayLength)(env, java_methods);

        for (jsize method_index = 0; method_index != method_count; ++method_index) {
            jobject java_method = (*(*env)->GetObjectArrayElement)(env, java_methods, method_index);
            jobject message = (*(*env)->CallObjectMethod)(env, java_method, java_lang_reflect_Method_getAnnotation, joc_Message);

            if (message == NULL)
                goto free_method;

            jstring message_jname = (*(*env)->CallObjectMethod)(env, message, joc_Message_name);
            jstring message_jtypes = (*(*env)->CallObjectMethod)(env, message, joc_Message_types);

            bool converted;
            if ((*(*env)->GetStringLength)(env, message_jname) != 0)
                converted = false;
            else {
                converted = true;
                (*(*env)->DeleteLocalRef)(env, message_jname);
                message_jname = (*(*env)->CallObjectMethod)(env, java_method, java_lang_reflect_Method_getName);
            }

            const char *message_cname = (*(*env)->GetStringUTFChars)(env, message_jname, NULL);

            const char *name;
            char *converting;

            if (!converted) {
                converting = NULL;
                name = message_cname;
            } else {
                converting = strdup(message_cname);
                for (size_t size = strlen(converting), i = 0; i != size; ++i)
                    if (converting[i] == '$')
                        converting[i] = ':';
                name = converting;
            }

            if ((*(*env)->GetStringLength)(env, message_jtypes) == 0) {
                (*(*env)->DeleteLocalRef)(env, message_jtypes);
                message_jtypes = (*(*env)->CallStaticObjectMethod)(env, joc_Runtime, joc_Runtime_getSignature, java_method);
            }

            const char *message_ctypes = (*(*env)->GetStringUTFChars)(env, message_jtypes, NULL);

            jint method_modifiers = (*(*env)->CallIntMethod)(env, java_method, java_lang_reflect_Method_getModifiers);
            jboolean method_static = (*(*env)->CallStaticBooleanMethod)(env, java_lang_reflect_Modifier, java_lang_reflect_Modifier_isStatic, method_modifiers);

            struct sig_signature *signature = apr_palloc(apr_pool_, sizeof(struct sig_signature));
            sig_parse(apr_pool_, signature, message_ctypes);

            ffi_type **java_types = apr_palloc(apr_pool_, (signature->count - 1 + 3) * sizeof(ffi_type));
            java_types[0] = &ffi_type_pointer;
            java_types[1] = &ffi_type_pointer;
            java_types[2] = &ffi_type_pointer;

            ffi_cif *java_cif = apr_palloc(apr_pool_, sizeof(ffi_cif));
            sig_ffi_cif(apr_pool_, &sig_java_ffi_type, signature, java_cif, java_types, 2, 3);

            ffi_type **objc_types = apr_palloc(apr_pool_, (signature->count - 1) * sizeof(ffi_type));
            ffi_cif *objc_cif = apr_palloc(apr_pool_, sizeof(ffi_cif));
            sig_ffi_cif(apr_pool_, &sig_objc_ffi_type, signature, objc_cif, objc_types, 0, 0);

            struct joc_userdata *userdata = apr_palloc(apr_pool_, sizeof(struct joc_userdata));
            userdata->signature = signature;
            userdata->env = env;
            userdata->method = (*(*env)->FromReflectedMethod)(env, java_method);
            userdata->cif = java_cif;
            userdata->fn = (*(method_static ? &java_static_call : &java_instance_call))(signature->elements[0].type, env);

            /* XXX: these memory pages should be pooled for the sake of even reasonable efficiency */

            ffi_closure *closure;
            _syscall(closure = (ffi_closure *) mmap(
                NULL, sizeof(ffi_closure),
                PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE,
                -1, 0
            ));

            ffi_status status = ffi_prep_closure(closure, objc_cif, &joc_closure, userdata);
            _assert(status == FFI_OK);

            _syscall(mprotect(closure, sizeof(*closure), PROT_READ | PROT_EXEC));

            if (!class_addMethod(
                method_static ? meta_class : instance_class,
                sel_registerName(name),
                (IMP) closure,
                strdup(message_ctypes)
            )) {
                fprintf(stderr, "class_addMethod([%s %c%s])", objc_class_name, method_static ? '+' : '-', name);
                _assert(false);
            }

            free(converting);

            (*(*env)->ReleaseStringUTFChars)(env, message_jtypes, message_ctypes);
            (*(*env)->DeleteLocalRef)(env, message_jtypes);

            (*(*env)->ReleaseStringUTFChars)(env, message_jname, message_cname);
            (*(*env)->DeleteLocalRef)(env, message_jname);

            (*(*env)->DeleteLocalRef)(env, message);
          free_method:
            (*(*env)->DeleteLocalRef)(env, java_method);
        }

        (*(*env)->DeleteLocalRef)(env, java_methods);
    }

    //AddVariables_(meta_class, MetaVariables);

    if (!joc_IsJava(instance_super))
        AddVariables_(instance_class, InstanceVariables);

    class_setVersion(instance_class, 0xcafebabe);
    objc_registerClassPair(instance_class);
    return [[instance_class retain] autorelease];
}

char *joc_ToName(const char *cname) {
    char *jname;
    if (strchr(cname, '.') == NULL)
        asprintf(&jname, "obc.%s", cname);
    else
        jname = strdup(cname);

    joc_ToName_(jname);
    return jname;
}

char *joc_FromName(const char *jname) {
    char *cname;
    if (strncmp(jname, "obc.", 4) == 0)
        cname = strdup(jname + 4);
    else
        cname = strdup(jname);
    return cname;
}

jobject Java_joc_Runtime_toString(JNIEnv *env, jclass _this, jobject object) {
    return joc_ToString(env, joc_FromObject(env, object));
}

void Java_joc_Runtime_allocate(JNIEnv *env, jclass _this, jobject java_object) {
    jclass java_class = (*(*env)->GetObjectClass)(env, java_object);
    Class objc_class = joc_FromClass(env, java_class);
    (*(*env)->DeleteLocalRef)(env, java_class);

    id objc_object;
    jboolean incorrect;

    lprintf("Runtime.allocate(%p):%s\n", objc_class, class_getName(objc_class));

    if (joc_IsJava(objc_class)) {
        objc_object = [objc_class allocWithObject: java_object];
        incorrect = false;
    } else {
        objc_object = [objc_class alloc];
        incorrect = objc_object->isa != objc_class;
    }

    (*(*env)->SetLongField)(env, java_object, joc_Strap_address, (jlong) (uintptr_t) objc_object);
    (*(*env)->SetBooleanField)(env, java_object, joc_Strap_incorrect, incorrect);
}

/*#if defined(__OBJC2__) && defined(__arm__)
#define objc_msgSendSuper objc_msgSendSuper2
OBJC_EXPORT id objc_msgSendSuper(struct objc_super *super, SEL op, ...);
#define objc_msgSendSuper_stret objc_msgSendSuper2_stret
OBJC_EXPORT void objc_msgSendSuper_stret(struct objc_super *super, SEL op, ...);
#endif*/

void (*objc_fn(ffi_type *ffi_type))() {
    return ffi_type->type == FFI_TYPE_STRUCT && (
        ffi_type->size > OBJC_MAX_STRUCT_BY_VALUE ||
        struct_forward_array[ffi_type->size] != 0
    ) ? FFI_FN(&objc_msgSendSuper_stret) : FFI_FN(&objc_msgSendSuper);
}

jlong Java_joc_Runtime_dlopen(JNIEnv *env, jclass _this, jstring path) {
    const char *cpath = path == NULL ? NULL : (*(*env)->GetStringUTFChars)(env, path, NULL);
    void *handle = dlopen(cpath, RTLD_LAZY | RTLD_GLOBAL);
    lprintf("dlopen(%s) = %p\n", cpath, handle);
    if (cpath != NULL)
        (*env)->ReleaseStringUTFChars(env, path, cpath);
    return (jlong) (uintptr_t) handle;
}

jlong Java_joc_Runtime_dlsym(JNIEnv *env, jclass _this, jlong handle, jstring symbol) {
    const char *csymbol = (*(*env)->GetStringUTFChars)(env, symbol, NULL);
    void *address = dlsym((void *) (uintptr_t) handle, csymbol);
    lprintf("dlsym(0x%.8llx:%s) = %p\n", handle, csymbol, address);
    (*env)->ReleaseStringUTFChars(env, symbol, csymbol);
    return (jlong) (uintptr_t) address;
}

void Java_joc_Runtime_dlclose(JNIEnv *env, jclass _this, jlong handle) {
    lprintf("dlclose(0x%.8llx)\n", handle);
    dlclose((void *) (uintptr_t) handle);
}

struct sig_type *sig_guess_type(apr_pool_t *pool, JNIEnv *env, jclass jtype) {
    struct sig_type *type = (struct sig_type *) apr_palloc(pool, sizeof(struct sig_type));
    _assert(type != NULL);
    memset(type, 0, sizeof(struct sig_type));

    if (jtype == NULL)
        type->primitive = void_P;
    else if ((*(*env)->IsSameObject)(env, jtype, joc_Pointer))
        type->primitive = pointer_P;
    else if ((*(*env)->IsSameObject)(env, jtype, java_lang_Byte))
        type->primitive = char_P;
    else if ((*(*env)->IsSameObject)(env, jtype, java_lang_Short))
        type->primitive = short_P;
    else if ((*(*env)->IsSameObject)(env, jtype, java_lang_Integer))
        type->primitive = int_P;
    else if ((*(*env)->IsSameObject)(env, jtype, java_lang_Long))
        type->primitive = long_P;
    else if ((*(*env)->IsSameObject)(env, jtype, java_lang_String))
        type->primitive = string_P;
    else if ((*(*env)->IsSameObject)(env, jtype, java_lang_Object))
        type->primitive = object_P;
    else {
        fprintf(stderr, "unknown type for ffi_call guess\n");
        _assert(false);
    }

    return type;
}

void sig_guess_signature(apr_pool_t *pool, JNIEnv *env, struct sig_signature *signature, jclass jtype, jobjectArray arguments) {
    jsize count = (*(*env)->GetArrayLength)(env, arguments);

    signature->count = count + 1;
    signature->elements = apr_palloc(pool, (count + 1) * sizeof(struct sig_element));

    signature->elements[0].type = sig_guess_type(pool, env, jtype);

    jsize index;
    for (index = 0; index != count; ++index) {
        struct sig_element *element = &signature->elements[index + 1];
        jobject argument = (*(*env)->GetObjectArrayElement)(env, arguments, index);
        jclass _class = (*(*env)->GetObjectClass)(env, argument);
        (*(*env)->DeleteLocalRef)(env, argument);
        element->type = sig_guess_type(pool, env, _class);
        (*(*env)->DeleteLocalRef)(env, _class);
    }
}

jobject Java_joc_Runtime_ffiCall(JNIEnv *env, jclass _this, jlong function, jclass jtype, jobjectArray arguments) {
    lprintf("ffiCall(0x%.8llx...)\n", function);

    apr_pool_t *pool;
    apr_pool_create(&pool, apr_pool_);

    struct sig_signature signature;
    sig_guess_signature(pool, env, &signature, jtype, arguments);
    unsigned count = signature.count - 1;

    ffi_cif ffi_cif;
    ffi_type *types[count];
    sig_ffi_cif(pool, &sig_objc_ffi_type, &signature, &ffi_cif, types, 0, 0);

    size_t offset = 0;
    size_t index;

    for (index = 0; index != signature.count - 1; ++index) {
        size_t alignment = ffi_cif.arg_types[index]->alignment;
        offset = (offset + alignment - 1) / alignment * alignment;
        signature.elements[index + 1].offset = offset;
        offset += ffi_cif.arg_types[index]->size;
    }

    signature.elements[0].offset = offset;

    /* XXX: alignment */
    uint8_t data[signature.elements[0].offset];
    void *values[count];

    for (unsigned index = 0; index != count; ++index) {
        struct sig_element *element = &signature.elements[index + 1];
        values[index] = data + element->offset;

        jobject object = (*(*env)->GetObjectArrayElement)(env, arguments, index);
        java_cast_from(pool, element->type, ffi_cif.arg_types[index], values[index], env, object);
        (*(*env)->DeleteLocalRef)(env, object);
    }

    void (*fn)() = (void (*)()) (uintptr_t) function;

    /* XXX: alignment */
    uint8_t value[ffi_cif.rtype->size];

    jthrowable jerror = NULL;

    @try {
        ffi_call(&ffi_cif, fn, value, values);
    } @catch (id error) {
        jerror = joc_ToException(env, error);
    }

    jobject result;
    if (jerror == NULL)
        result = java_cast_to(pool, signature.elements[0].type, ffi_cif.rtype, value, env);
    else {
        result = NULL;
        (*(*env)->Throw)(env, jerror);
    }

    apr_pool_destroy(pool);
    return result;
}

jobject Java_joc_Runtime_msgSend(JNIEnv *env, jclass _this, jobject object, jclass _class, jstring name, jobjectArray arguments) {
    id cobject = joc_FromObject(env, object);

    Class cclass = joc_FromClass(env, _class);
    if (cclass == NULL)
        cclass = [cobject class];

    SEL selector; {
        const char *cname = (*(*env)->GetStringUTFChars)(env, name, NULL);
        selector = sel_getUid(cname);
        (*env)->ReleaseStringUTFChars(env, name, cname);
    }

    Method method;
    if (!class_isMetaClass(cobject->isa))
        method = class_getInstanceMethod(cclass, selector);
    else {
        method = class_getClassMethod(cobject, selector);
        cclass = cclass->isa;
    }

    _assert(cobject != NULL);
    struct objc_super super = {cobject, cclass};

    lprintf("cobject:%p (%s) selector:%p (%s) method:%p cclass:%p (%s) cobject->isa:%p (%s)\n", cobject, class_getName(cobject->isa), selector, sel_getName(selector), method, cclass, class_getName(cclass), cobject->isa, class_getName(class_getSuperclass(cobject->isa)));

    apr_pool_t *pool;
    apr_pool_create(&pool, apr_pool_);

    struct sig_signature signature;
    sig_parse(pool, &signature, method_getTypeEncoding(method));
    unsigned count = signature.count - 1;

    _assert(count >= 2);
    _assert(signature.elements[1].type->primitive == object_P);
    _assert(signature.elements[2].type->primitive == selector_P);

    ffi_cif ffi_cif;
    ffi_type *types[count];
    sig_ffi_cif(pool, &sig_objc_ffi_type, &signature, &ffi_cif, types, 0, 0);

    /* XXX: alignment */
    uint8_t data[signature.elements[0].offset];
    void *values[count];

    for (unsigned index = 0; index != count; ++index) {
        struct sig_element *element = &signature.elements[index + 1];
        values[index] = data + element->offset;

        if (index == 0)
            * (struct objc_super **) values[index] = &super;
        else if (index == 1)
            * (SEL *) values[index] = selector;
        else {
            jobject object = (*(*env)->GetObjectArrayElement)(env, arguments, index - 2);
            java_cast_from(pool, element->type, ffi_cif.arg_types[index], values[index], env, object);
            (*(*env)->DeleteLocalRef)(env, object);
        }
    }

    void (*fn)() = objc_fn(ffi_cif.rtype);

    /* XXX: alignment */
    uint8_t value[ffi_cif.rtype->size];

    jthrowable jerror = NULL;

    @try {
        ffi_call(&ffi_cif, fn, value, values);
    } @catch (id error) {
        jerror = joc_ToException(env, error);
    }

    //hexdump("sendMsg:value", value, sizeof(value));

    jobject result;
    if (jerror == NULL)
        result = java_cast_to(pool, signature.elements[0].type, ffi_cif.rtype, value, env);
    else {
        result = NULL;
        (*(*env)->Throw)(env, jerror);
    }

    apr_pool_destroy(pool);
    return result;
}
