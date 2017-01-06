#include "minimal/stdlib.h"

#include <objc/objc-runtime.h>

#include <string.h>

#include "joc/Java.h"
#include "joc/Strap.h"
#include "joc/String.h"
#include "joc/Wrapper.h"

/* XXX: remove me */
OBJC_EXPORT const char *class_getName(Class cls);

jobject _jocStrap(JNIEnv *env, const char *type, id value) {
    jboolean incorrect = false;

    jclass _class = (*(*env)->FindClass)(env, type);
    if (_class == NULL) {
        printf("cannot find class: %s\n", type);
        _assert(false);
    }

    //printf("_jocStrap:retain(%s)\n", type);
    value = [value retain];

    /* XXX: this leaks an Objective-C object */
    jmethodID method = (*(*env)->GetMethodID)(env, _class, "<init>", "()V");
    jobject object = (*(*env)->NewObject)(env, _class, method);

    if (object == NULL) {
        fprintf(stderr, "_jocStrap(%s)\n", type);
        _assert(false);
    }

    /*id leaked = (id) (uintptr_t) (*(*env)->GetLongField)(env, object, joc_Strap_address);
    printf("leaking %p (%u)...\n", leaked, [leaked retainCount]);
    [leaked release];
    printf("leaked\n");*/

    (*(*env)->SetLongField)(env, object, joc_Strap_address, (jlong) (uintptr_t) value);
    (*(*env)->SetBooleanField)(env, object, joc_Strap_incorrect, incorrect);

    (*(*env)->DeleteLocalRef)(env, _class);
    return object;
}

@implementation NSObject (JOCStrap)

+ (jobject) _jocStrap:(JNIEnv *)env {
    _assert(false);
    return NULL;
}

- (jobject) _jocStrap:(JNIEnv *)env {
    const char *cname = class_getName([self class]);
    char jname[strlen(cname) + 5];
    sprintf(jname, "obc/%s", cname);
    return _jocStrap(env, jname, self);
}

@end

/*@implementation NSString (JOCStrap)

- (jobject) _jocStrap:(JNIEnv *)env {
    return joc_ToString(env, self);
}

@end*/

jobject joc_ToObject(JNIEnv *env, id value) {
    return value == NULL ? NULL : [value _jocStrap:env];
}

id joc_FromObject(JNIEnv *env, jobject value) {
    if (value == NULL)
        return NULL;
    else if ((*(*env)->IsInstanceOf)(env, value, java_lang_Class))
        return joc_FromClass(env, value);
    else if ((*(*env)->IsInstanceOf)(env, value, joc_Identifier))
        return [[(id) (uintptr_t) (*(*env)->CallLongMethod)(env, value, joc_Identifier_getIdentifier) retain] autorelease];
    else if ((*(*env)->IsInstanceOf)(env, value, java_lang_String))
        return joc_FromString(env, value);
    else
        return joc_Wrap(env, value);
}
