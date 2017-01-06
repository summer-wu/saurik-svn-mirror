#include <substrate.h>
#include <UIKit/UIKit.h>

#include <PhotoLibrary/PLCameraController.h>

/* XXX: maybe pass NO instead of YES? */
void PLCameraController$capturePhoto(PLCameraController *self, SEL sel) {
    [self capturePhoto:YES];
}

void UIApplication$removeApplicationBadge(UIApplication *self, SEL sel) {
    [self setApplicationIconBadgeNumber:0];
}

/* XXX: this doesn't support removeOnPop */
void UINavigationBar$setAccessoryView$animate$removeOnPop$(UINavigationBar *self, SEL sel, UIView *view, BOOL animate, BOOL removeOnPop) {
    [self setAccessoryView:view animate:animate];
}

static void MethodRenamed(Class _class, SEL oldname, SEL newname) {
    if (_class != nil)
        if (class_getInstanceMethod(_class, oldname) == NULL)
            if (Method method = class_getInstanceMethod(_class, newname))
                class_addMethod(_class, oldname, method_getImplementation(method), method_getTypeEncoding(method));
}

static void MethodReplaced(Class _class, SEL sel, IMP imp, const char *types) {
    if (_class != nil)
        if (class_getInstanceMethod(_class, sel) == NULL)
            class_addMethod(_class, sel, imp, types);
}

Class $PLCameraController;
Class $UIApplication;
Class $UINavigationBar;

extern "C" void TweakInitialize() {
    $PLCameraController = objc_getClass("PLCameraController");
    $UIApplication = objc_getClass("UIApplication");
    $UINavigationBar = objc_getClass("UINavigationBar");

    MethodReplaced($PLCameraController, @selector(capturePhoto), reinterpret_cast<IMP>(&PLCameraController$capturePhoto), "v8@0:4");

    MethodRenamed($UIApplication, @selector(addStatusBarImageNamed:removeOnAbnormalExit:), @selector(addStatusBarImageNamed:removeOnExit:));
    MethodReplaced($UIApplication, @selector(removeApplicationBadge), reinterpret_cast<IMP>(&UIApplication$removeApplicationBadge), "v8@0:4");
    MethodRenamed($UIApplication, @selector(setApplicationBadge:), @selector(setApplicationBadgeString:));

    MethodReplaced($UINavigationBar, @selector(setAccessoryView:animate:removeOnPop:), reinterpret_cast<IMP>(&UINavigationBar$setAccessoryView$animate$removeOnPop$), "v20@0:4@8c12c16");
}
