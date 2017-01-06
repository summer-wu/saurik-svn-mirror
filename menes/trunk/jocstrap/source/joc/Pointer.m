#include "minimal/stdlib.h"

#include "joc/Java.h"
#include "joc/Pointer.h"
#include "joc/Strap.h"

#include <memory.h>

JNIEXPORT jobject JNICALL Java_joc_Pointer_malloc(JNIEnv *env, jclass _this, jlong size) {
    return joc_ToPointer(env, malloc(size));
}

JNIEXPORT void JNICALL Java_joc_Pointer_free(JNIEnv *env, jclass _this, jobject value) {
    free(joc_FromPointer(env, value));
}

JNIEXPORT void JNICALL Java_joc_Pointer_memmove(JNIEnv *env, jclass _this, jobject dst, jobject src, long len) {
    memmove(joc_FromPointer(env, dst), joc_FromPointer(env, src), len);
}

JNIEXPORT void JNICALL Java_joc_Pointer_memcpy(JNIEnv *env, jclass _this, jobject dst, jobject src, long len) {
    memcpy(joc_FromPointer(env, dst), joc_FromPointer(env, src), len);
}

JNIEXPORT jobject JNICALL Java_joc_Pointer_box___3Ljoc_Pointer_2(JNIEnv *env, jclass _class, jobjectArray value) {
    jsize size = (*(*env)->GetArrayLength)(env, value);
    void **data = malloc(sizeof(void *) * size);
    for (jsize index = 0; index != size; ++index) {
        jobject object = (*(*env)->GetObjectArrayElement)(env, value, index);
        data[index] = joc_FromPointer(env, object);
        (*(*env)->DeleteLocalRef)(env, object);
    }

    return joc_ToPointer(env, data);
}

#define Value(T, Type, type, TYPE, tYPE) \
    JNIEXPORT jobject JNICALL Java_joc_Pointer_box___3 ## T(JNIEnv *env, jclass _class, j ## type ## Array value) { \
        j ## type *values = (*(*env)->Get ## tYPE ## ArrayElements)(env, value, NULL); \
        jsize size = (*(*env)->GetArrayLength)(env, value); \
        TYPE *data = malloc((size + 1) * sizeof(TYPE)); \
        for (jsize index = 0; index != size; ++index) \
            data[index] = values[index]; \
        data[size] = 0; \
        (*(*env)->Release ## tYPE ## ArrayElements)(env, value, values, JNI_ABORT); \
        return joc_ToPointer(env, data); \
    }

#include "joc/Value.h"
#undef Value

jobject joc_ToPointer(JNIEnv *env, void *value) {
    return value == NULL ? NULL : ((*env)->NewObject)(env, joc_Pointer, joc_Pointer_$init$, (jlong) (uintptr_t) value);
}

void *joc_FromPointer(JNIEnv *env, jobject value) {
    if (value == NULL)
        return NULL;
    _assert((*(*env)->IsInstanceOf)(env, value, joc_Pointer));
    return (void *) (uintptr_t) (*(*env)->GetLongField)(env, value, joc_Pointer_address);
}
