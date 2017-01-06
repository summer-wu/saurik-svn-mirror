#include "minimal/stdlib.h"

#include <objc/objc-runtime.h>

#include <ffi.h>

#include "joc/Cast.h"
#include "joc/Java.h"
#include "joc/Pointer.h"
#include "joc/Selector.h"
#include "joc/Strap.h"

struct java_deleteref {
    JNIEnv *env;
    jstring object;
};

apr_status_t java_deleteref(void *arg) {
    struct java_deleteref *deleteref = (struct java_deleteref *) arg;
    (*(*deleteref->env)->DeleteGlobalRef)(deleteref->env, deleteref->object);
    return APR_SUCCESS;
}

struct java_releasestr {
    JNIEnv *env;
    jstring object;
    const char *string;
};

apr_status_t java_releasestr(void *arg) {
    struct java_releasestr *releasestr = (struct java_releasestr *) arg;
    (*(*releasestr->env)->ReleaseStringUTFChars)(releasestr->env, releasestr->object, releasestr->string);
    (*(*releasestr->env)->DeleteGlobalRef)(releasestr->env, releasestr->object);
    return APR_SUCCESS;
}

/* passed references freed by caller
 * returned (global) references freed by pool */
void java_cast_from(
    apr_pool_t *pool,
    struct sig_type *sig_type,
    ffi_type *ffi_type,
    uint8_t *value,
    JNIEnv *env,
    jobject object
) {
    _assert(sig_type != NULL);
    _assert(ffi_type != NULL);
    _assert(value != NULL);
    _assert(env != NULL);

    switch (sig_type->primitive) {
        case boolean_P:
            _assert((*(*env)->IsInstanceOf)(env, object, java_lang_Number));
            * (BOOL *) value = (*(*env)->CallByteMethod)(env, object, java_lang_Number_byteValue);
        break;

        case uchar_P:
        case char_P:
            _assert((*(*env)->IsInstanceOf)(env, object, java_lang_Number));
            * (char *) value = (*(*env)->CallByteMethod)(env, object, java_lang_Number_byteValue);
        break;

        case ushort_P:
        case short_P:
            _assert((*(*env)->IsInstanceOf)(env, object, java_lang_Number));
            * (short *) value = (*(*env)->CallShortMethod)(env, object, java_lang_Number_shortValue);
        break;

        case ulong_P:
        case long_P:
        case uint_P:
        case int_P:
            _assert((*(*env)->IsInstanceOf)(env, object, java_lang_Number));
            * (int *) value = (*(*env)->CallIntMethod)(env, object, java_lang_Number_intValue);
        break;

        case ulonglong_P:
        case longlong_P:
            _assert((*(*env)->IsInstanceOf)(env, object, java_lang_Number));
            * (long *) value = (*(*env)->CallLongMethod)(env, object, java_lang_Number_longValue);
        break;

        case float_P:
            _assert((*(*env)->IsInstanceOf)(env, object, java_lang_Number));
            * (float *) value = (*(*env)->CallFloatMethod)(env, object, java_lang_Number_floatValue);
        break;

        case double_P:
            _assert((*(*env)->IsInstanceOf)(env, object, java_lang_Number));
            * (double *) value = (*(*env)->CallDoubleMethod)(env, object, java_lang_Number_doubleValue);
        break;

        case object_P:
            * (id *) value = joc_FromObject(env, object);
        break;

        case selector_P:
            * (SEL *) value = joc_FromSelector(env, object);
        break;

        case pointer_P:
            * (void **) value = joc_FromPointer(env, object);
        break;

        case string_P:
            if (object == NULL)
                * (const char **) value = NULL;
            else {
                const char *string = (*(*env)->GetStringUTFChars)(env, object, NULL);
                * (const char **) value = string;

                /* XXX: replace with autorelease'd NSData */
                struct java_releasestr *releasestr = apr_palloc(pool, sizeof(struct java_releasestr));
                releasestr->env = env;
                releasestr->object = (*(*env)->NewGlobalRef)(env, object);
                releasestr->string = string;
                apr_pool_cleanup_register(pool, releasestr, &java_releasestr, NULL);
            }
        break;

        case struct_P: {
            size_t offset = 0;
            jobjectArray fields = (*(*env)->CallObjectMethod)(env, object, joc_Struct_getFields);
            _assert(fields != NULL);

            for (jsize index = 0; index != sig_type->data.signature.count; ++index) {
                jobject field = (*(*env)->GetObjectArrayElement)(env, fields, index);
                java_cast_from(pool, sig_type->data.signature.elements[index].type, ffi_type->elements[index], value + offset, env, field);
                /* XXX: alignment? */
                offset += ffi_type->elements[index]->size;
                (*(*env)->DeleteLocalRef)(env, field);
            }

            (*(*env)->DeleteLocalRef)(env, fields);
        } break;

        case void_P:
        break;

        default:
            _assert(false);
    }
}

/* returned (local) reference freed by caller */
jobject java_cast_to(
    apr_pool_t *pool,
    struct sig_type *sig_type,
    ffi_type *ffi_type,
    uint8_t *value,
    JNIEnv *env
) {
    switch (sig_type->primitive) {
#define joc_Wrap(Wrap, wrap) do { \
    return ((*env)->NewObject)(env, java_lang_ ## Wrap, java_lang_ ## Wrap ## _$init$, * (wrap *) value); \
} while (false)

        case boolean_P:
            joc_Wrap(Boolean, bool);
        break;

        case uchar_P:
        case char_P:
            joc_Wrap(Byte, jbyte);
        break;

        case ushort_P:
        case short_P:
            joc_Wrap(Short, jshort);
        break;

        case ulong_P:
        case long_P:
        case uint_P:
        case int_P:
            joc_Wrap(Integer, jint);
        break;

        case ulonglong_P:
        case longlong_P:
            joc_Wrap(Long, jlong);
        break;

        case float_P:
            joc_Wrap(Float, jfloat);
        break;

        case double_P:
            joc_Wrap(Double, jdouble);
        break;

        case object_P:
            return joc_ToObject(env, * (id *) value);
        break;

        case selector_P:
            return joc_ToSelector(env, * (SEL *) value);
        break;

        case pointer_P:
            return joc_ToPointer(env, * (void **) value);
        break;

        case string_P:
            return * (char **) value == NULL ? NULL : (*(*env)->NewStringUTF)(env, * (char **) value);
        break;

        case struct_P: {
            char *name;
            asprintf(&name, "obc/%s", sig_type->name);
            jclass _class = (*(*env)->FindClass)(env, name);
            _assert(_class != NULL);
            free(name);

            jmethodID init = (*(*env)->GetMethodID)(env, _class, "<init>",  "()V");
            _assert(init != NULL);

            jobjectArray fields = (*(*env)->NewObjectArray)(env, sig_type->data.signature.count, java_lang_Object, NULL);

            size_t offset = 0;

            for (jsize index = 0; index != sig_type->data.signature.count; ++index) {
                jobject field = java_cast_to(pool, sig_type->data.signature.elements[index].type, ffi_type->elements[index], value + offset, env);
                (*(*env)->SetObjectArrayElement)(env, fields, index, field);
                (*(*env)->DeleteLocalRef)(env, field);
                /* XXX: alignment */
                offset += ffi_type->elements[index]->size;
            }

            jobject object = (*(*env)->NewObject)(env, _class, init);
            (*(*env)->CallVoidMethod)(env, object, joc_Struct_setFields, fields);
            (*(*env)->DeleteLocalRef)(env, fields);
            return object;
        } break;

        case void_P:
            return NULL;
        break;

        default:
            printf("%c\n", sig_type->primitive);
            _assert(false);
    }
}

/* passed (local) references freed internally */
void java_copy_from(
    apr_pool_t *pool,
    struct sig_type *sig_type,
    ffi_type *ffi_type,
    uint8_t *value,
    JNIEnv *env,
    uint8_t *result
) {
    _assert(sig_type != NULL);
    _assert(ffi_type != NULL);
    _assert(value != NULL);
    _assert(env != NULL);
    _assert(result != NULL);

    switch (sig_type->primitive) {
        case boolean_P:

        case uchar_P:
        case char_P:

        case ushort_P:
        case short_P:

        case ulong_P:
        case long_P:
        case uint_P:
        case int_P:

        case ulonglong_P:
        case longlong_P:
            memcpy(value, result, ffi_type->size);
        break;

        case float_P:
        case double_P:
            memcpy(value, result, ffi_type->size);
        break;

        case object_P:
        case selector_P:
        case pointer_P:
        case string_P:
        case struct_P: {
            jobject object = * (jobject *) result;
            java_cast_from(pool, sig_type, ffi_type, value, env, object);
            (*(*env)->DeleteLocalRef)(env, object);
        } break;

        case void_P:
        break;

        default:
            _assert(false);
    }
}

/* returned (global) references freed by pool */
void java_copy_to(
    apr_pool_t *pool,
    struct sig_type *sig_type,
    ffi_type *ffi_type,
    uint8_t *value,
    JNIEnv *env,
    uint8_t *result
) {
    switch (sig_type->primitive) {
        case boolean_P:

        case uchar_P:
        case char_P:

        case ushort_P:
        case short_P:

        case ulong_P:
        case long_P:
        case uint_P:
        case int_P:

        case ulonglong_P:
        case longlong_P:
            memcpy(result, value, ffi_type->size);
        break;

        case float_P: {
            double dvalue = * (float *) value;
            memcpy(result, &dvalue, sizeof(jdouble));
        } break;

        case double_P:
            memcpy(result, value, sizeof(jdouble));
        break;

        case object_P:
        case selector_P:
        case pointer_P:
        case string_P:
        case struct_P: {
            jobject object = java_cast_to(pool, sig_type, ffi_type, value, env);
            struct java_deleteref *deleteref = apr_palloc(pool, sizeof(struct java_deleteref));
            deleteref->env = env;
            deleteref->object = (*(*env)->NewGlobalRef)(env, object);
            (*(*env)->DeleteLocalRef)(env, object);
            apr_pool_cleanup_register(pool, deleteref, &java_deleteref, NULL);
            * (jobject *) result = deleteref->object;
        } break;

        case void_P:
        break;

        default:
            printf("%c\n", sig_type->primitive);
            _assert(false);
    }
}
