#include "minimal/stdlib.h"

#include "joc/Java.h"
#include "joc/Pointer.h"
#include "joc/Strap.h"

jobject joc_ToException(JNIEnv *env, id value) {
    jobject object = joc_ToObject(env, value);
    if ((*(*env)->IsInstanceOf)(env, object, java_lang_Throwable))
        return object;
    else if ((*(*env)->IsInstanceOf)(env, object, joc_Strap)) {
        jobject native = ((*env)->NewObject)(env, joc_NativeException, joc_NativeException_$init$, object);
        (*(*env)->DeleteLocalRef)(env, object);
        return native;
    } else _assert(false);
}
