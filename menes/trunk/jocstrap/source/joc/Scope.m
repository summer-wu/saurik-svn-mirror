#include "minimal/stdlib.h"

#include <jni.h>

#include <Foundation/NSDate.h>
#include <Foundation/NSAutoreleasePool.h>

#include "joc/NativeException.h"

jlong Java_joc_Scope_push(JNIEnv *env, jclass _this) {
    @try {
        return (jlong) (uintptr_t) [[NSAutoreleasePool alloc] init];
    } @catch (id error) {
        _trace();
        (*(*env)->Throw)(env, joc_ToException(env, error));
    }

    return 0;
}

void Java_joc_Scope_pop(JNIEnv *env, jclass _this, jlong pool) {
    @try {
        [(NSAutoreleasePool *) (uintptr_t) pool release];
    } @catch (id error) {
        _trace();
        (*(*env)->Throw)(env, joc_ToException(env, error));
    }
}
