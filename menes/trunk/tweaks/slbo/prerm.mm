#import <Foundation/Foundation.h>
#include <cstring>

int main(int argc, const char *argv[]) {
    if (argc < 2 || strcmp(argv[1], "remove") != 0)
        return 0;
    NSAutoreleasePool *pool([[NSAutoreleasePool alloc] init]);

    NSString *path(@"/Library/LaunchDaemons/com.apple.syslogd.plist");
    if (![[NSFileManager defaultManager] fileExistsAtPath:path])
        path = [@"/System" stringByAppendingString:path];

    NSMutableDictionary *plist([[NSMutableDictionary alloc] initWithContentsOfFile:path]);
    if (plist == nil)
        return 1;

    NSMutableArray *args([plist objectForKey:@"ProgramArguments"]);
    if (args == nil)
        return 1;

    bool changed(false);

    for (;;) {
        NSUInteger bsd([args indexOfObject:@"-bsd_out"]);
        if (bsd == NSNotFound)
            break;
        if (bsd == [args count] - 1)
            return 1;
        [args removeObjectsInRange:NSMakeRange(bsd, 2)];
        changed = true;
    }

    if (changed) {
        if (![plist writeToFile:path atomically:YES])
            return 1;
        system([[NSString stringWithFormat:@"/bin/launchctl unload %@", path] UTF8String]);
        system([[NSString stringWithFormat:@"/bin/launchctl load %@", path] UTF8String]);
    }

    [pool release];
    return 0;
}
