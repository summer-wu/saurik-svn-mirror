#include <substrate.h>

#include <kjs/object.h>
#include <kjs/function.h>

#include <Foundation/Foundation.h>

FILE *log_;

@interface NSString (SafariLog)
+ (NSString *) stringWithBytesNoCopy:(const char *)bytes length:(int)length encoding:(NSStringEncoding)encoding;
+ (NSString *) stringWithBytes:(const char *)bytes length:(int)length encoding:(NSStringEncoding)encoding;
+ (NSString *) stringWithUTF8BytesNoCopy:(const char *)bytes length:(int)length;
+ (NSString *) stringWithUTF8Bytes:(const char *)bytes length:(int)length;
@end

@implementation NSString (SafariLog)

+ (NSString *) stringWithBytesNoCopy:(const char *)bytes length:(int)length encoding:(NSStringEncoding)encoding {
    return [[[NSString alloc] initWithBytesNoCopy:const_cast<char *>(bytes) length:length encoding:encoding freeWhenDone:NO] autorelease];
}

+ (NSString *) stringWithBytes:(const char *)bytes length:(int)length encoding:(NSStringEncoding)encoding {
    return [[[NSString alloc] initWithBytes:const_cast<char *>(bytes) length:length encoding:encoding] autorelease];
}

+ (NSString *) stringWithUTF8BytesNoCopy:(const char *)bytes length:(int)length {
    return [NSString stringWithBytesNoCopy:bytes length:length encoding:NSUTF8StringEncoding];
}

+ (NSString *) stringWithUTF8Bytes:(const char *)bytes length:(int)length {
    return [NSString stringWithBytes:bytes length:length encoding:NSUTF8StringEncoding];
}

@end

MSHook(void, UIWebDocumentView$webView$didReceiveMessage$, id self, SEL sel, id webview, id info) {
    NSString *message([info objectForKey:@"message"]);
    NSString *lineNumber([info objectForKey:@"lineNumber"]);
    NSString *sourceID([info objectForKey:@"sourceID"]);

    NSString *level([info objectForKey:@"MessageLevel"]);
    if ([level hasSuffix:@"MessageLevel"])
        level = [level substringToIndex:([level length] - 12)];

    if (log_ != NULL) {
        fprintf(log_, "%s\n", [[NSString stringWithFormat:@"Console <%@ #%@> (%@): \"%@\"", sourceID, lineNumber, level, message] UTF8String]);
        fflush(log_);
    }

    _UIWebDocumentView$webView$didReceiveMessage$(self, sel, webview, info);
}

KJS::Identifier (*__ZN3KJS11FunctionImp16getParameterNameEi)(KJS::FunctionImp *_this, int index);

NSString *NS(const KJS::Identifier &idname) {
    return [NSString stringWithBytes:reinterpret_cast<const char *>(idname.data()) length:(idname.size() * 2) encoding:NSUTF16LittleEndianStringEncoding];
}

NSString *NS(KJS::ExecState *exec, KJS::JSValue *jsvalue) {
    const KJS::UString &usvalue(jsvalue->toString(exec));
    return [NSString stringWithBytes:reinterpret_cast<const char *>(usvalue.data()) length:(usvalue.size() * 2) encoding:NSUTF16LittleEndianStringEncoding];
}

MSHook(KJS::JSValue *, KJS$FunctionImp$call, KJS::FunctionImp *_this, KJS::ExecState *exec, KJS::JSObject *thisObj, const KJS::List &args) {
    NSString *call([NSString stringWithFormat:@"%@(", NS(_this->functionName())]);

    for (size_t i(0), e(args.size()); i != e; ++i) {
        NSString *nsname(NS(__ZN3KJS11FunctionImp16getParameterNameEi(_this, i)));
        NSString *nsvalue(NS(exec, args.at(i)));
        call = [call stringByAppendingString:[NSString stringWithFormat:@"%@%@:\"%@\"", (i == 0 ? @"" : @", "), nsname, nsvalue]];
    }

    if (log_ != NULL) {
        fprintf(log_, "%s\n", [[NSString stringWithFormat:@"Call: %@)", call] UTF8String]);
        fflush(log_);
    }

    return _KJS$FunctionImp$call(_this, exec, thisObj, args);
}

extern "C" void TweakInitialize() {
    log_ = fopen("/tmp/safari.log", "w");
    NSLog(@"%p", log_);

    struct nlist nl[3];
    memset(nl, 0, sizeof(nl));
    nl[0].n_un.n_name = (char *) "__ZN3KJS11FunctionImp14callAsFunctionEPNS_9ExecStateEPNS_8JSObjectERKNS_4ListE";
    nl[1].n_un.n_name = (char *) "__ZN3KJS11FunctionImp16getParameterNameEi";
    nlist(JavaScriptCore_f, nl);
    KJS::JSValue *(*__ZN3KJS11FunctionImp14callAsFunctionEPNS_9ExecStateEPNS_8JSObjectERKNS_4ListE)(KJS::FunctionImp *, KJS::ExecState *, KJS::JSObject *, const KJS::List &) = reinterpret_cast<KJS::JSValue *(*)(KJS::FunctionImp *, KJS::ExecState *, KJS::JSObject *, const KJS::List &)>(nl[0].n_value);
    __ZN3KJS11FunctionImp16getParameterNameEi = reinterpret_cast<KJS::Identifier (*)(KJS::FunctionImp *, int)>(nl[1].n_value);

    MSHookFunction(__ZN3KJS11FunctionImp14callAsFunctionEPNS_9ExecStateEPNS_8JSObjectERKNS_4ListE, &$KJS$FunctionImp$call, &_KJS$FunctionImp$call);

    _UIWebDocumentView$webView$didReceiveMessage$ = MSHookMessage(objc_getClass("UIWebDocumentView"), @selector(webView:didReceiveMessage:), &$UIWebDocumentView$webView$didReceiveMessage$);
}
