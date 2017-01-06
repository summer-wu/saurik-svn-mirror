#include <substrate.h>

#import <Foundation/Foundation.h>

MSClassHook(NSURL)
MSClassHook(TabDocument)

static bool dynamic_;

MSInstanceMessageHook0(BOOL, NSURL, isFileURL) {
    return dynamic_ && [NSThread isMainThread] ? NO : MSOldCall();
}

MSInstanceMessageHook5(void, TabDocument, _decidePolicyForAction,request,frame,newFrameName,decisionListener, id, action, id, request, id, frame, id, name, id, listener) {
    dynamic_ = true;
    @try {
        MSOldCall(action, request, frame, name, listener);
    } @finally {
        dynamic_ = false;
    }
}
