#include "minimal/stdlib.h"

#include "joc/Java.h"
#include "joc/String.h"

@implementation JOCString

- (void) dealloc {
    (*(*env_)->DeleteGlobalRef)(env_, object_);
    [super dealloc];
}

- (jobject) _jocStrap:(JNIEnv *)env {
    return (*(*env)->NewLocalRef)(env, object_);
}

- (JOCString *) _initWithEnv:(JNIEnv *)env object:(jobject)object {
    if ((self = [super init]) != nil) {
        env_ = env;
        object_ = (*(*env)->NewGlobalRef)(env, object);
        return self;
    } else return nil;
}

- (unsigned int) length {
    return (*(*env_)->CallIntMethod)(env_, object_, java_lang_CharSequence_length);
}

- (unichar) characterAtIndex:(unsigned)index {
    return (*(*env_)->CallCharMethod)(env_, object_, java_lang_CharSequence_charAt, index);
}

@end

jstring joc_ToString(JNIEnv *env, id value) {
    if (value == NULL)
        return NULL;
    const char *string = [value UTF8String];
    _assert(string != NULL);
    return (*(*env)->NewStringUTF)(env, string);
}

id joc_FromString(JNIEnv *env, jobject value) {
    return [[[JOCString alloc] _initWithEnv:env object:value] autorelease];
}
