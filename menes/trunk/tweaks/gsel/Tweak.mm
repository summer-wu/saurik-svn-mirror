#include <substrate.h>

#import <CoreFoundation/CFRuntime.h>
#import <UIKit/UIKit.h>

/* even MobileSubstrate has undocumented features */
void MSLogHex(const void *vdata, size_t size, const char *mark = 0);

MSHook(BOOL, UIApplication$handleEvent$withNewEvent$, UIApplication *self, SEL sel, GSEventRef gs, id ui) {
    CFRuntimeBase *base(reinterpret_cast<CFRuntimeBase *>(gs));
    GSEventRecord *record(reinterpret_cast<GSEventRecord *>(base + 1));

    MSLogHex(base, sizeof(*base), "type");
    MSLogHex(record, sizeof(*record), "rcrd");

    uint8_t *data(reinterpret_cast<uint8_t *>(record + 1));
    MSLogHex(data, record->size, "data");

    CFPropertyListRef plist(reinterpret_cast<CFPropertyListRef (*)(GSEventRef)>(dlsym(RTLD_DEFAULT, "GSEventCreatePlistRepresentation"))(gs));
    NSLog(@"[list] %@", [(id)plist description]);
    CFRelease(plist);

    NSLog(@"[evnt] %@", ui, GSEventGetTypeID());

    return _UIApplication$handleEvent$withNewEvent$(self, sel, gs, ui);
}

MSHook(void, UIKeyboardImpl$handleHardwareKeyDownFromSimulator$, UIKeyboardImpl *self, SEL sel, GSEventRef event) {
    NSLog(@"handleHardwareKeyDownFromSimulator:%p,%@", event, MSHookIvar<UIKeyboardLayout *>(self, "m_layout"));
    _UIKeyboardImpl$handleHardwareKeyDownFromSimulator$(self, sel, event);
}

extern "C" void TweakInitialize() {
    _UIApplication$handleEvent$withNewEvent$ = MSHookMessage([UIApplication class], @selector(handleEvent:withNewEvent:), &$UIApplication$handleEvent$withNewEvent$);
    _UIKeyboardImpl$handleHardwareKeyDownFromSimulator$ = MSHookMessage([UIKeyboardImpl class], @selector(handleHardwareKeyDownFromSimulator:), &$UIKeyboardImpl$handleHardwareKeyDownFromSimulator$);
}
