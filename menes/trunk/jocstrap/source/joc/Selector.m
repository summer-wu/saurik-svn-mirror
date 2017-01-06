#include "minimal/stdlib.h"

#include "joc/Java.h"
#include "joc/Selector.h"
#include "joc/Strap.h"

JNIEXPORT jstring JNICALL Java_joc_Selector_toString(JNIEnv *env, jobject _this) {
    return (*(*env)->NewStringUTF)(env, sel_getName(joc_FromSelector(env, _this)));
}

jobject joc_ToSelector(JNIEnv *env, SEL value) {
    return value == NULL ? NULL : ((*env)->NewObject)(env, joc_Selector, joc_Selector_$init$, (jlong) (uintptr_t) value);
}

jlong Java_joc_Selector_registerName(JNIEnv *env, jclass _this, jstring name) {
    const char *cname = (*(*env)->GetStringUTFChars)(env, name, NULL);
    SEL sel = sel_registerName(cname);
    (*(*env)->ReleaseStringUTFChars)(env, name, cname);
    return (jlong) (uintptr_t) sel;
}

SEL joc_FromSelector(JNIEnv *env, jobject value) {
    if (value == NULL)
        return NULL;
    _assert((*(*env)->IsInstanceOf)(env, value, joc_Selector));
    return (void *) (uintptr_t) (*(*env)->GetLongField)(env, value, joc_Selector_address);
}
