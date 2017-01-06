#include <substrate.h>

#import <UIKit/UIKit.h>

MSHook(int, UIApplicationMain, int argc, char *argv[], NSString *principalClassName, NSString *delegateClassName) {
    sleep(10);
    return _UIApplicationMain(argc, argv, principalClassName, delegateClassName);
}

extern "C" void TweakInitialize() {
    MSHookFunction(&UIApplicationMain, &$UIApplicationMain, &_UIApplicationMain);
}
