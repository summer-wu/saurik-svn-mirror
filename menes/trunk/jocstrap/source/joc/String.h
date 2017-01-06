#ifndef JOC_STRING_H
#define JOC_STRING_H

#include <jni.h>

#include <Foundation/NSString.h>

@interface JOCString : NSString {
    JNIEnv *env_;
    jstring object_;
}

- (void) dealloc;
- (jobject) _jocStrap:(JNIEnv *)env;
- (JOCString *) _initWithEnv:(JNIEnv *)env object:(jobject)object;
- (unsigned int) length;
- (unichar) characterAtIndex:(unsigned)index;
@end

jstring joc_ToString(JNIEnv *env, id value);
id joc_FromString(JNIEnv *env, jobject value);

#endif/*JOC_STRING_H*/
