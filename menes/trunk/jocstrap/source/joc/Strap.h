#ifndef JOC_STRAP_H
#define JOC_STRAP_H

#include <jni.h>

#import <Foundation/NSString.h>

@interface NSObject (JOCStrap)

+ (jobject) _jocStrap:(JNIEnv *)env;
- (jobject) _jocStrap:(JNIEnv *)env;

@end

/*@interface NSString (JOCStrap)
- (jobject) _jocStrap:(JNIEnv *)env;
@end*/

char *joc_ToName(const char *cname);
char *joc_FromName(const char *cname);

jobject joc_ToObject(JNIEnv *env, id value);
id joc_FromObject(JNIEnv *env, jobject value);

jclass joc_ToClass(JNIEnv *env, Class value);
Class joc_FromClass(JNIEnv *env, jclass value);

#endif/*JOC_STRAP_H*/
