#ifndef JOC_NATIVEEXCEPTION_H
#define JOC_NATIVEEXCEPTION_H

#include <jni.h>

jobject joc_ToException(JNIEnv *env, id value);

#endif/*JOC_NATIVEEXCEPTION_H*/
