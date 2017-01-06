#include "minimal/stdlib.h"

#include <jni.h>

#include "joc/Java.h"
#include "joc/Memory.h"

jclass java_lang_CharSequence = NULL;
jmethodID java_lang_CharSequence_charAt = NULL;
jmethodID java_lang_CharSequence_length = NULL;

jclass java_lang_Class = NULL;
jmethodID java_lang_Class_getDeclaredMethod = NULL;
jmethodID java_lang_Class_getDeclaredMethods = NULL;
jmethodID java_lang_Class_getName = NULL;
jmethodID java_lang_Class_getSuperclass = NULL;

jclass java_lang_Object = NULL;
jmethodID java_lang_Object_equals = NULL;
jmethodID java_lang_Object_toString = NULL;

jclass java_lang_Number = NULL;
jclass java_lang_String = NULL;

jclass java_lang_ref_Reference = NULL;
jmethodID java_lang_ref_Reference_get = NULL;

jclass java_lang_ref_WeakReference = NULL;
jmethodID java_lang_ref_WeakReference_$init$ = NULL;

jclass java_lang_reflect_Method = NULL;
jmethodID java_lang_reflect_Method_getAnnotation = NULL;
jmethodID java_lang_reflect_Method_getModifiers = NULL;
jmethodID java_lang_reflect_Method_getName = NULL;

jclass java_lang_reflect_Modifier = NULL;
jmethodID java_lang_reflect_Modifier_isStatic = NULL;

jclass java_lang_System = NULL;
jclass java_lang_Throwable = NULL;

jclass joc_Identifier = NULL;
jmethodID joc_Identifier_getIdentifier = NULL;

jclass joc_Message = NULL;
jmethodID joc_Message_name = NULL;
jmethodID joc_Message_types = NULL;

jclass joc_NativeException = NULL;
jmethodID joc_NativeException_$init$ = NULL;

jclass joc_Pointer = NULL;
jfieldID joc_Pointer_address = NULL;
jmethodID joc_Pointer_$init$ = NULL;

jclass joc_Runtime = NULL;
jmethodID joc_Runtime_getSignature = NULL;
jmethodID joc_Runtime_getWrapper = NULL;

jclass joc_Selector = NULL;
jfieldID joc_Selector_address = NULL;
jmethodID joc_Selector_$init$ = NULL;

jclass joc_Strap = NULL;
jfieldID joc_Strap_address = NULL;
jfieldID joc_Strap_incorrect = NULL;

jclass joc_Struct = NULL;
jmethodID joc_Struct_getFields = NULL;
jmethodID joc_Struct_setFields = NULL;

jclass joc_Wrapper = NULL;
jmethodID joc_Wrapper_forwardInvocation = NULL;
jmethodID joc_Wrapper_methodSignatureForSelector = NULL;
jmethodID joc_Wrapper_respondsToSelector = NULL;

#define Value(T, Type, type, TYPE, tYPE) \
    jclass java_lang_ ## Type = NULL; \
    jmethodID java_lang_ ## Type ## _$init$ = NULL;

#include "joc/Value.h"
#undef Value

#define Value(T, Type, type, TYPE, tYPE) \
    jmethodID java_lang_Number_ ## type ## Value = NULL;

#include "joc/Value_.h"
#undef Value

#define GetClass(cname, jname) do { \
    cname = (*(*env)->FindClass)(env, jname); \
    _assert(cname != NULL); \
    cname = (*(*env)->NewGlobalRef)(env, cname); \
    _assert(cname != NULL); \
} while (false)

#define GetMethod(static, class, method, type) do { \
    class ## _ ## method = (* \
        (static ? (*env)->GetStaticMethodID : (*env)->GetMethodID) \
    )(env, class, #method, type); \
    _assert(class ## _ ## method != NULL); \
} while (false)

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    (*(*vm)->GetEnv)(vm, (void **) (void *) &env, JNI_VERSION_1_2);

    GetClass(java_lang_CharSequence, "java/lang/CharSequence");
    GetMethod(false, java_lang_CharSequence, length, "()I");
    GetMethod(false, java_lang_CharSequence, charAt, "(I)C");

    GetClass(java_lang_Class, "java/lang/Class");
    GetMethod(false, java_lang_Class, getDeclaredMethod, "(Ljava/lang/String;[Ljava/lang/Class;)Ljava/lang/reflect/Method;");
    GetMethod(false, java_lang_Class, getDeclaredMethods, "()[Ljava/lang/reflect/Method;");
    GetMethod(false, java_lang_Class, getName, "()Ljava/lang/String;");
    GetMethod(false, java_lang_Class, getSuperclass, "()Ljava/lang/Class;");

    GetClass(java_lang_Object, "java/lang/Object");
    GetMethod(false, java_lang_Object, equals, "(Ljava/lang/Object;)Z");
    GetMethod(false, java_lang_Object, toString, "()Ljava/lang/String;");

    GetClass(java_lang_Number, "java/lang/Number");
    GetClass(java_lang_String, "java/lang/String");

    GetClass(java_lang_reflect_Method, "java/lang/reflect/Method");
    GetMethod(false, java_lang_reflect_Method, getAnnotation, "(Ljava/lang/Class;)Ljava/lang/annotation/Annotation;");
    GetMethod(false, java_lang_reflect_Method, getModifiers, "()I");
    GetMethod(false, java_lang_reflect_Method, getName, "()Ljava/lang/String;");

    GetClass(java_lang_ref_Reference, "java/lang/ref/Reference");
    GetMethod(false, java_lang_ref_Reference, get, "()Ljava/lang/Object;");

    GetClass(java_lang_ref_WeakReference, "java/lang/ref/WeakReference");
    java_lang_ref_WeakReference_$init$ = ((*env)->GetMethodID)(env, java_lang_ref_WeakReference, "<init>", "(Ljava/lang/Object;)V");

    GetClass(java_lang_reflect_Modifier, "java/lang/reflect/Modifier");
    GetMethod(true, java_lang_reflect_Modifier, isStatic, "(I)Z");

    GetClass(java_lang_System, "java/lang/System");
    GetClass(java_lang_Throwable, "java/lang/Throwable");

    GetClass(joc_Identifier, "joc/Identifier");
    GetMethod(false, joc_Identifier, getIdentifier, "()J");

    GetClass(joc_Message, "joc/Message");
    GetMethod(false, joc_Message, name, "()Ljava/lang/String;");
    GetMethod(false, joc_Message, types, "()Ljava/lang/String;");

    GetClass(joc_NativeException, "joc/NativeException");
    joc_NativeException_$init$ = ((*env)->GetMethodID)(env, joc_NativeException, "<init>", "(Ljoc/Strap;)V");

    GetClass(joc_Pointer, "joc/Pointer");
    joc_Pointer_address = (*(*env)->GetFieldID)(env, joc_Pointer, "address_", "J");
    joc_Pointer_$init$ = ((*env)->GetMethodID)(env, joc_Pointer, "<init>", "(J)V");

    GetClass(joc_Runtime, "joc/Runtime");
    GetMethod(true, joc_Runtime, getSignature, "(Ljava/lang/reflect/Method;)Ljava/lang/String;");
    GetMethod(true, joc_Runtime, getWrapper, "(Ljava/lang/Object;)Ljoc/Wrapper;");

    GetClass(joc_Selector, "joc/Selector");
    joc_Selector_address = (*(*env)->GetFieldID)(env, joc_Selector, "address_", "J");
    joc_Selector_$init$ = ((*env)->GetMethodID)(env, joc_Selector, "<init>", "(J)V");

    GetClass(joc_Strap, "joc/Strap");
    joc_Strap_address = (*(*env)->GetFieldID)(env, joc_Strap, "address_", "J");
    joc_Strap_incorrect = (*(*env)->GetFieldID)(env, joc_Strap, "incorrect_", "Z");

    GetClass(joc_Struct, "joc/Struct");
    GetMethod(false, joc_Struct, getFields, "()[Ljava/lang/Object;");
    GetMethod(false, joc_Struct, setFields, "([Ljava/lang/Object;)V");

    GetClass(joc_Wrapper, "joc/Wrapper");
    GetMethod(false, joc_Wrapper, forwardInvocation, "(Ljava/lang/Object;Ljava/lang/String;[Ljava/lang/Object;)Ljava/lang/Object;");
    GetMethod(false, joc_Wrapper, methodSignatureForSelector, "(Ljava/lang/Object;Ljava/lang/String;)Ljava/lang/String;");
    GetMethod(false, joc_Wrapper, respondsToSelector, "(Ljava/lang/Object;Ljava/lang/String;)Z");

#define Value(T, Type, type, TYPE, tYPE) \
    GetClass(java_lang_ ## Type, "java/lang/" #Type); \
    java_lang_ ## Type ## _$init$ = (*(*env)->GetMethodID)(env, java_lang_ ## Type, "<init>", "(" #T ")V"); \
    _assert(java_lang_ ## Type ## _$init$ != NULL); \
    java_lang_Number_ ## type ## Value = (*(*env)->GetMethodID)(env, java_lang_Number, #type "Value", "()" #T); \
    _assert(java_lang_Number_ ## type ## Value != NULL); \

#include "joc/Value_.h"
#undef Value

    mem_Initialize();
    (*(*env)->FindClass)(env, "obc/NSException");

    return JNI_VERSION_1_2;
}

#undef GetClass
