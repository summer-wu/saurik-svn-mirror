#ifndef JOC_WRAPPER_H
#define JOC_WRAPPER_H

#include <objc/objc-runtime.h>
#include <jni.h>

#include <Foundation/NSProxy.h>

@interface JOCWrap : NSProxy {
    JNIEnv *env_;
    jobject wrapper_;
    jobject object_;
}

- (void) dealloc;
- (jobject) _jocStrap:(JNIEnv *)env;
- (JOCWrap *) _initWithEnv:(JNIEnv *)env wrapper:(jobject)wrapper object:(jobject)object;
//- forward:(SEL)sel :(marg_list)args;
- (NSMethodSignature *) methodSignatureForSelector:(SEL)sel;
- (void) forwardInvocation:(NSInvocation *)invocation;
- (BOOL) respondsToSelector:(SEL)sel;
@end

id joc_Wrap(JNIEnv *env, jobject value);

#endif/*JOC_WRAPPER_H*/
