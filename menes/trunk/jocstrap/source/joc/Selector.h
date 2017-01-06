#ifndef JOC_SELECTOR_H
#define JOC_SELECTOR_H

#include <jni.h>
#include <objc/objc-runtime.h>

jobject joc_ToSelector(JNIEnv *env, SEL value);
SEL joc_FromSelector(JNIEnv *env, jobject value);

#endif/*JOC_SELECTOR_H*/
