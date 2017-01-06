#include <objc/objc-class.h>
#import <UIKit/UIKit.h>

#include "minimal/stdlib.h"

#include "joc/NativeException.h"
#include "joc/Pointer.h"
#include "joc/Strap.h"

void Java_com_saurik_uicaboodle_Main_UIApplicationMain(JNIEnv *env, jclass _this, jint argc, jobject argv, jobject _class) {
    char **cargv = joc_FromPointer(env, argv);
    Class cclass = joc_FromObject(env, _class);

    @try {
#ifdef __OBJC2__
        NSString *name = [NSString stringWithUTF8String:class_getName(cclass)];
        UIApplicationMain(argc, cargv, name, name);
#else
        UIApplicationMain(argc, cargv, cclass);
#endif
    } @catch (id error) {
        (*(*env)->Throw)(env, joc_ToException(env, error));
    }
}
