/* PrivaCy - Global Opt-Out for Anonymous Usage Tracking
 * Copyright (C) 2009-2010  Jay Freeman (saurik)
*/

/*
 *        Redistribution and use in source and binary
 * forms, with or without modification, are permitted
 * provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the
 *    above copyright notice, this list of conditions
 *    and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the
 *    above copyright notice, this list of conditions
 *    and the following disclaimer in the documentation
 *    and/or other materials provided with the
 *    distribution.
 * 3. The name of the author may not be used to endorse
 *    or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <substrate.h>

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

#define _transient

extern "C" NSString *SBSCopyLocalizedApplicationNameForDisplayIdentifier(NSString *);

static void *ReturnZero(id self, SEL sel) {
    //NSLog(@"ReturnZero: [%@ %s]", self, sel_getName(sel));
    return NULL;
}

#if 0
static BOOL ReturnTrue(id self, SEL sel) {
    //NSLog(@"ReturnTrue: [%@ %s]", self, sel_getName(sel));
    return YES;
}
#endif

MSHook(id, MobclixController$initWithApplicationId$applicationLogLevel$recordLocation$notifyUser$, id self, SEL sel, id application, void *level, void *location, BOOL notify) {
    return _MobclixController$initWithApplicationId$applicationLogLevel$recordLocation$notifyUser$(self, sel, application, level, location, NO);
}

static void DisplayEvent(NSString *event) {
    NSLog(@"DisplayEvent(%@)", event);
}

MSHook(void, MMTrackingMgr$trackEvent$, id self, SEL sel, NSString *key) {
    DisplayEvent(key);
    _MMTrackingMgr$trackEvent$(self, sel, key);
}

MSHook(void, MMTrackingMgr$trackEvent$withUserDict$, id self, SEL sel, NSString *key, NSDictionary *dict) {
    DisplayEvent(key);
    _MMTrackingMgr$trackEvent$withUserDict$(self, sel, key, dict);
}

MSHook(void, Mobclix$logEventWithLevel$processName$eventName$description$stop$, id self, SEL sel, unsigned level, NSString *process, NSString *event, NSString *description, BOOL stop) {
    DisplayEvent([NSString stringWithFormat:@"%@: %@", process, event]);
    _Mobclix$logEventWithLevel$processName$eventName$description$stop$(self, sel, level, process, event, description, stop);
}

MSHook(void, FlurryAPI$logEvent$, id self, SEL sel, NSString *event) {
    DisplayEvent(event);
    //_FlurryAPI$logEvent$(self, sel, event);
}

MSInitialize {
    NSAutoreleasePool *pool([[NSAutoreleasePool alloc] init]);

    //NSString *home(NSHomeDirectory());
    NSString *home(@"/var/mobile");

    if (NSDictionary *settings = [NSDictionary dictionaryWithContentsOfFile:[home stringByAppendingString:@"/Library/Preferences/com.saurik.PrivaCy.plist"]]) {
        bool display([[settings objectForKey:@"DisplayEvents"] boolValue]);

        if (Class $FlurryAPI = objc_getClass("FlurryAPI"))
            if (Class $$FlurryAPI = object_getClass($FlurryAPI)) {
                if (display)
                    _FlurryAPI$logEvent$ = MSHookMessage($$FlurryAPI, @selector(logEvent:withParameters:), &$FlurryAPI$logEvent$);

                if ([[settings objectForKey:@"OptOut/Flurry"] boolValue]) {
                    if (Class $FlurrySession = objc_getClass("FlurrySession"))
                        if (Class $$FlurrySession = object_getClass($FlurrySession))
                            MSHookMessage($$FlurrySession, @selector(alloc), &ReturnZero);
                    MSHookMessage($$FlurryAPI, @selector(serverURL), &ReturnZero);
                }
            }

        if (Class $MMTrackingMgr = objc_getClass("MMTrackingMgr")) {
            if (display) {
                _MMTrackingMgr$trackEvent$ = MSHookMessage($MMTrackingMgr, @selector(trackEvent:), &$MMTrackingMgr$trackEvent$);
                _MMTrackingMgr$trackEvent$withUserDict$ = MSHookMessage($MMTrackingMgr, @selector(trackEvent:withUserDict:), &$MMTrackingMgr$trackEvent$withUserDict$);
            }

            if ([[settings objectForKey:@"OptOut/Medialets"] boolValue])
                if (Class $$MMTrackingMgr = object_getClass($MMTrackingMgr))
                    MSHookMessage($$MMTrackingMgr, @selector(sharedInstance), &ReturnZero);
        }

        if (display)
            if (Class $Mobclix = objc_getClass("Mobclix"))
                if (Class $$Mobclix = object_getClass($Mobclix))
                    _Mobclix$logEventWithLevel$processName$eventName$description$stop$ = MSHookMessage($$Mobclix, @selector(logEventWithLevel:processName:eventName:description:stop:), &$Mobclix$logEventWithLevel$processName$eventName$description$stop$);

        if ([[settings objectForKey:@"OptOut/Mobclix"] boolValue])
            if (Class $MobclixController = objc_getClass("MobclixController")) {
                MSHookMessage($MobclixController, @selector(isMobclixEnabled), &ReturnZero);
                MSHookMessage($MobclixController, @selector(isMobclixEnabledKnown), &ReturnZero);
                _MobclixController$initWithApplicationId$applicationLogLevel$recordLocation$notifyUser$ = MSHookMessage($MobclixController, @selector(initWithApplicationId:applicationLogLevel:recordLocation:notifyUser:), &$MobclixController$initWithApplicationId$applicationLogLevel$recordLocation$notifyUser$);
            }

        if ([[settings objectForKey:@"OptOut/PinchMedia"] boolValue])
            if (objc_getClass("PM_Database") != nil)
                if (Class $Beacon = objc_getClass("Beacon"))
                    if (Class $$Beacon = object_getClass($Beacon)) {
                        MSHookMessage($$Beacon, @selector(alloc), &ReturnZero);
                        MSHookMessage($$Beacon, @selector(shared), &ReturnZero);
                    }
    }

    [pool release];
}
