#include "minimal/stdlib.h"
#include "minimal/hexdump.h"

#include <stdio.h>
#include <string.h>

#include <objc/objc-runtime.h>

#include <ffi.h>
#include <jni.h>

#include "sig/ffi_type.h"
#include "sig/parse.h"
#include "sig/types.h"

#include "joc/Cast.h"
#include "joc/Java.h"
#include "joc/Memory.h"
#include "joc/Strap.h"
#include "joc/Wrapper.h"

#include <Foundation/NSInvocation.h>
#include <Foundation/NSMethodSignature.h>
#include <Foundation/NSString.h>

@interface NSProxy (JocStrap)
- (BOOL) isKindOfClass:(id)class;
@end

@implementation NSProxy (JocStrap)

- (BOOL) isKindOfClass:(id)class {
    NSLog(@"[%@ isKindOfClass:%@]", self, class);
    return NO;
}

@end

@interface NSMethodSignature (NSForwardInvocation)
+ (id) signatureWithObjCTypes:(char *)types;
- (id) _typeString;
@end

@interface NSProxy (NSForwardInvocation)
- forward:(SEL)sel :(marg_list)args;
@end

@implementation JOCWrap

- (void) dealloc {
    (*(*env_)->DeleteGlobalRef)(env_, wrapper_);
    (*(*env_)->DeleteGlobalRef)(env_, object_);
    [super dealloc];
}

- (jobject) _jocStrap:(JNIEnv *)env {
    return (*(*env)->NewLocalRef)(env, object_);
}

- (JOCWrap *) _initWithEnv:(JNIEnv *)env wrapper:(jobject)wrapper object:(jobject)object {
    env_ = env;
    wrapper_ = (*(*env)->NewGlobalRef)(env, wrapper);
    object_ = (*(*env)->NewGlobalRef)(env, object);
    return self;
}

/*- forward:(SEL)sel :(marg_list)args {
    printf("%%forward:@selector(%s):%p\n", sel_getName(sel), args);
    return nil;
    return [super forward:sel :args];
}*/

- (NSMethodSignature *) methodSignatureForSelector:(SEL)sel {
    //printf("%%methodSignatureForSelector:@selector(%s)\n", sel_getName(sel));
    jstring selector = (*(*env_)->NewStringUTF)(env_, sel_getName(sel));
    jstring java_types = (*(*env_)->CallObjectMethod)(env_, wrapper_, joc_Wrapper_methodSignatureForSelector, object_, selector);
    (*(*env_)->DeleteLocalRef)(env_, selector);

    if (java_types == NULL)
        return nil;

    const char *objc_types = (*(*env_)->GetStringUTFChars)(env_, java_types, NULL);
    NSMethodSignature *signature = [NSMethodSignature signatureWithObjCTypes:(char *) objc_types];

    (*(*env_)->ReleaseStringUTFChars)(env_, java_types, objc_types);
    (*(*env_)->DeleteLocalRef)(env_, java_types);

    return signature;
}

- (void) forwardInvocation:(NSInvocation *)invocation {
    NSMethodSignature *apl_signature = [invocation methodSignature];

    apr_pool_t *pool;
    apr_pool_create(&pool, apr_pool_);

    const char *types;

#ifdef OBJC_NEW_PROPERTIES
    types = [[apl_signature _typeString] UTF8String];
#else
    object_getInstanceVariable(apl_signature, "_types", (void **) &types);
#endif

    //printf("%%forwardInvocation:%p:@selector(%s)\n", invocation, types);

    SEL objc_selector = [invocation selector];
    jstring java_selector = (*(*env_)->NewStringUTF)(env_, sel_getName(objc_selector));

    struct sig_signature signature;
    sig_parse(pool, &signature, types);

    jobjectArray arguments = (*(*env_)->NewObjectArray)(env_, signature.count - 3, java_lang_Object, NULL);

    ffi_type **objc_types = apr_palloc(pool, (signature.count - 1) * sizeof(ffi_type));
    ffi_cif *objc_cif = apr_palloc(pool, sizeof(ffi_cif));
    sig_ffi_cif(pool, &sig_objc_ffi_type, &signature, objc_cif, objc_types, 0, 0);

    for (size_t index = 3; index != signature.count; ++index) {
        struct sig_type *sig_type = signature.elements[index].type;
        ffi_type *ffi_type = objc_cif->arg_types[index - 1];
        /* XXX: alignment! */
        uint8_t value[ffi_type->size];
        [invocation getArgument:value atIndex:index - 1];
        jobject object = java_cast_to(pool, sig_type, ffi_type, value, env_);
        (*(*env_)->SetObjectArrayElement)(env_, arguments, index - 3, object);
        (*(*env_)->DeleteLocalRef)(env_, object);
    }

    jobject java_value = (*(*env_)->CallObjectMethod)(env_, wrapper_, joc_Wrapper_forwardInvocation, object_, java_selector, arguments);

    (*(*env_)->DeleteLocalRef)(env_, arguments);
    (*(*env_)->DeleteLocalRef)(env_, java_selector);

    jthrowable jerror = (*(*env_)->ExceptionOccurred)(env_);
    if (jerror == NULL && signature.elements[0].type->primitive != void_P) {
        /* XXX: alignment! */
        uint8_t objc_value[objc_cif->rtype->size];
        java_cast_from(pool, signature.elements[0].type, objc_cif->rtype, objc_value, env_, java_value);
        [invocation setReturnValue:objc_value];
        //hexdump("ret", objc_value, objc_cif->rtype->size);
    }

    (*(*env_)->DeleteLocalRef)(env_, java_value);

    apr_pool_destroy(pool);

    if (jerror != NULL)
        @throw joc_FromObject(env_, jerror);
}

- (BOOL) respondsToSelector:(SEL)sel {
    //printf("%%respondsToSelector:@selector(%s)\n", sel_getName(sel));
    jstring selector = (*(*env_)->NewStringUTF)(env_, sel_getName(sel));
    jboolean responds = (*(*env_)->CallBooleanMethod)(env_, wrapper_, joc_Wrapper_respondsToSelector, object_, selector);
    (*(*env_)->DeleteLocalRef)(env_, selector);
    return responds;
}

@end

id joc_Wrap(JNIEnv *env, jobject value) {
    jobject wrapper = (*(*env)->CallStaticObjectMethod)(env, joc_Runtime, joc_Runtime_getWrapper, value);
    if (wrapper == NULL) {
        jobject _class = (*(*env)->GetObjectClass)(env, value);
        jstring java_class_name = (*(*env)->CallObjectMethod)(env, _class, java_lang_Class_getName);
        const char *c_class_name = (*(*env)->GetStringUTFChars)(env, java_class_name, NULL);
        fprintf(stderr, "unable to locate wrapper for %s\n", c_class_name);
        (*(*env)->ReleaseStringUTFChars)(env, java_class_name, c_class_name);
        (*(*env)->DeleteLocalRef)(env, java_class_name);
        (*(*env)->DeleteLocalRef)(env, _class);
        _assert(false);
    }

    id wrap = [[JOCWrap alloc] _initWithEnv:env wrapper:wrapper object:value];
    (*(*env)->DeleteLocalRef)(env, wrapper);
    return [wrap autorelease];
}
