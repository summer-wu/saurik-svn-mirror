#include <substrate.h>

#import <Foundation/NSArray.h>
#import <Foundation/NSAutoreleasePool.h>
#import <Foundation/NSNull.h>

#import <CoreGraphics/CGGeometry.h>

#import <SpringBoard/SBButtonBar.h>
#import <SpringBoard/SBIcon.h>
#import <SpringBoard/SBIconModel.h>

@interface ISIconSupport : NSObject {
}

+ (ISIconSupport *) sharedInstance;
- (void) addExtension:(NSString *)name;

@end

@interface SBDockIconListView : NSObject {
}
- (NSArray *) icons;
@end

MSClassHook(SBButtonBar)
MSClassHook(SBDockIconListView)
MSClassHook(SBIcon)
MSClassHook(SBIconModel)

MSMetaClassHook(SBDockIconListView)
MSMetaClassHook(SBIconModel)

__attribute__((__constructor__))
static void MSFixClass() {
    if ($SBIcon == nil)
        $SBIcon = objc_getClass("SBIconView");
}

MSInstanceMessageHook1(float, SBButtonBar, leftMarginForIconRowArray, NSArray *, row) {
    return [row indexOfObject:[NSNull null]] == NSNotFound ? 4 : MSOldCall(row);
}

MSInstanceMessageHook0(int, SBButtonBar, maxIconColumns) {
    return 5;
}

MSClassMessageHook1(int, SBDockIconListView, iconColumnsForInterfaceOrientation, int, orientation) {
    return 5;
}

static unsigned $SBButtonBar$count(SBButtonBar *self) {
    MSIvarHook(NSArray *, _iconMatrix);
    NSArray *row([_iconMatrix objectAtIndex:0]);
    unsigned count(0);
    for (id icon in row)
        if (icon != [NSNull null])
            ++count;
    return count;
}

MSInstanceMessageHook0(float, SBButtonBar, topIconPadding) {
    return 313370 + $SBButtonBar$count(self);
}

MSInstanceMessageHook1(void, SBIcon, setOrigin, CGPoint, origin) {
    unsigned count(origin.y);
    if (count < 313370)
        goto set;
    count -= 313370;

    if (count > 40)
        origin.x = 256;
    else if (count > 10 || count < 0)
        goto set;
    else if (count == 5)
        origin.x = (origin.x - 4) / 76 * 63 + 4;

    origin.y = 11;
  set:
    MSOldCall(origin);
}

MSInstanceMessageHook2(CGPoint, SBButtonBar, originForIconAtX,Y, int, x, int, y) {
    CGPoint origin;//(MSOldCall(x, y, ));
    //NSLog(@"f:%u,%u=(%f,%f)", x, y, origin.x, origin.y);
    origin.y = 11;

    unsigned count($SBButtonBar$count(self)), gap;
    unsigned space(320 - 60 * count);

    if (count >= 4)
        gap = space / (count + 1);
    else // I hate people who love Apple.
        gap = 16;

    origin.x = (space - gap * (count - 1)) / 2 + (60 + gap) * x;

    return origin;
}

// XXX: merge with previous function if possible, but not until after iPhone 4
MSInstanceMessageHook2(CGPoint, SBDockIconListView, originForIconAtX,Y, int, x, int, y) {
    CGPoint origin;//(MSOldCall(x, y, ));
    //NSLog(@"f:%u,%u=(%f,%f)", x, y, origin.x, origin.y);
    origin.y = 11;

    unsigned count([[self icons] count]), gap;
    unsigned space(320 - 60 * count);

    if (count >= 4)
        gap = space / (count + 1);
    else // I hate people who love Apple.
        gap = 16;

    origin.x = (space - gap * (count - 1)) / 2 + (60 + gap) * x;

    return origin;
}

/*MSClassMessageHook0(int, SBIconModel, maxIconListCount) {
    return 16;
}*/

MSInitialize {
    NSAutoreleasePool *pool([[NSAutoreleasePool alloc] init]);

    if (dlopen("/Library/MobileSubstrate/DynamicLibraries/IconSupport.dylib", RTLD_GLOBAL | RTLD_LAZY) != NULL)
        if (Class $ISIconSupport = objc_getClass("ISIconSupport"))
            [[$ISIconSupport sharedInstance] addExtension:@"com.saurik.iphone.fid"];

    [pool release];
}
