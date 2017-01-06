#ifndef JOC_JAVA_H
#define JOC_JAVA_H

#include <jni.h>

extern jclass java_lang_CharSequence;
extern jmethodID java_lang_CharSequence_charAt;
extern jmethodID java_lang_CharSequence_length;

extern jclass java_lang_Class;
extern jmethodID java_lang_Class_getDeclaredMethod;
extern jmethodID java_lang_Class_getDeclaredMethods;
extern jmethodID java_lang_Class_getName;
extern jmethodID java_lang_Class_getSuperclass;

extern jclass java_lang_Object;
extern jmethodID java_lang_Object_equals;
extern jmethodID java_lang_Object_toString;

extern jclass java_lang_Number;
extern jclass java_lang_String;

extern jclass java_lang_ref_Reference;
extern jmethodID java_lang_ref_Reference_get;

extern jclass java_lang_ref_WeakReference;
extern jmethodID java_lang_ref_WeakReference_$init$;

extern jclass java_lang_reflect_Method;
extern jmethodID java_lang_reflect_Method_getAnnotation;
extern jmethodID java_lang_reflect_Method_getModifiers;
extern jmethodID java_lang_reflect_Method_getName;

extern jclass java_lang_reflect_Modifier;
extern jmethodID java_lang_reflect_Modifier_isStatic;

extern jclass java_lang_System;
extern jclass java_lang_Throwable;

extern jclass joc_Identifier;
extern jmethodID joc_Identifier_getIdentifier;

extern jclass joc_Message;
extern jmethodID joc_Message_name;
extern jmethodID joc_Message_types;

extern jclass joc_NativeException;
extern jmethodID joc_NativeException_$init$;

extern jclass joc_Pointer;
extern jfieldID joc_Pointer_address;
extern jmethodID joc_Pointer_$init$;

extern jclass joc_Runtime;
extern jmethodID joc_Runtime_getSignature;
extern jmethodID joc_Runtime_getWrapper;

extern jclass joc_Selector;
extern jfieldID joc_Selector_address;
extern jmethodID joc_Selector_$init$;

extern jclass joc_Strap;
extern jfieldID joc_Strap_address;
extern jfieldID joc_Strap_incorrect;

extern jclass joc_Struct;
extern jmethodID joc_Struct_getFields;
extern jmethodID joc_Struct_setFields;

extern jclass joc_Wrapper;
extern jmethodID joc_Wrapper_forwardInvocation;
extern jmethodID joc_Wrapper_methodSignatureForSelector;
extern jmethodID joc_Wrapper_respondsToSelector;

#define Value(T, Type, type, TYPE, tYPE) \
    extern jclass java_lang_ ## Type; \
    extern jmethodID java_lang_ ## Type ## _$init$; \
    extern jmethodID java_lang_Number_ ## type ## Value;

#include "joc/Value.h"
#undef Value

#endif/*JOC_JAVA_H*/
