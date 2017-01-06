#ifndef JOC_POINTER_H
#define JOC_POINTER_H

#include <jni.h>

jobject joc_ToPointer(JNIEnv *env, void *value);
void *joc_FromPointer(JNIEnv *env, jobject value);

#endif/*JOC_POINTER_H*/
