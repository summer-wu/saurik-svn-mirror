/* AppTapp Tools - command line tools for AppTapp
 * Copyright (C) 2008  Jay Freeman (saurik)
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

#include <objc/objc.h>
#include <objc/runtime.h>

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

#import <AppTapp/ATPackageManager.h>
#import <AppTapp/NSArray-AppTappPackages.h>
#import <AppTapp/NSDictionary-AppTappPackage.h>

#include <stdbool.h>

void usage(const char *argv0) {
    printf("usage: %s <commands...>\n", argv0);
    printf("  -L - \"consider\" local packages\n");
    printf("  -R - \"consider\" remote packages\n");
    printf("  -P - \"consider\" named plist\n");
    printf("  -a - \"operate\" on all \"considered\"\n");
    printf("  id - \"operate\" on named \"considered\" package\n");
    printf("  -c - clear/cancel \"operating\" packages\n");
    printf("  -i - install \"operating\" packages\n");
    printf("  -u - upgrade \"operating\" packages\n");
    printf("  -r - remove/uninstall \"operating\" packages\n");
    printf("  -d - display list of \"operating\" packages\n");
    printf("  -p - dump plist for \"operating\" packages\n");
    printf("\n");
    printf("example: %s -L com.saurik.Cydia.Update -rP ./com.saurik.Cydia.Update.plist -ai\n", argv0);
    printf("  remove com.saurik.Cydia.Update and reinstall from specified plist file\n");
}

@interface CYAppTapp : UIApplication {
    NSString *status_;
    NSDictionary *package_;
}

- (void) packageManager:(ATPackageManager *)manager startedQueue:(NSArray *)queue;
- (void) packageManager:(ATPackageManager *)manager progressChanged:(NSNumber *)progress;
- (void) packageManager:(ATPackageManager *)manager statusChanged:(NSString *)status;
- (void) packageManager:(ATPackageManager *)manager finishedQueueWithResult:(NSString *)result;
- (void) packageManager:(ATPackageManager *)manager didAddSource:(id)source;
- (void) packageManager:(ATPackageManager *)manager didRemoveSource:(id)source;
- (void) packageManager:(ATPackageManager *)manager issuedNotice:(id)notice;
- (void) packageManager:(ATPackageManager *)manager issuedError:(id)error;

- (void) applicationDidFinishLaunching:(id)unused;

@end

@implementation CYAppTapp

/*- (NSMethodSignature *) methodSignatureForSelector:(SEL)selector {
    fprintf(stderr, "[%s]S-%s\n", class_getName(self->isa), sel_getName(selector));
    return [super methodSignatureForSelector:selector];
}

- (BOOL) respondsToSelector:(SEL)selector {
    fprintf(stderr, "[%s]R-%s\n", class_getName(self->isa), sel_getName(selector));
    return [super respondsToSelector:selector];
}*/

- (void) packageManager:(ATPackageManager *)manager startedQueue:(NSArray *)queue {
}

- (void) packageManager:(ATPackageManager *)manager progressChanged:(NSNumber *)progress {
    NSLog(@"Progress: %@%%", progress);
}

- (void) packageManager:(ATPackageManager *)manager statusChanged:(NSString *)status {
    if (status_ != nil)
        [status_ release];
    status_ = [status retain];
}

- (void) packageManager:(ATPackageManager *)manager finishedQueueWithResult:(NSString *)result {
    NSLog(@"Result: %@", result);
    [self terminateWithSuccess];
}

- (void) packageManager:(ATPackageManager *)manager didAddSource:(id)source {
    printf("%s: %s\n", __FUNCTION__, class_getName([source class]));
}

- (void) packageManager:(ATPackageManager *)manager didRemoveSource:(id)source {
    printf("%s: %s\n", __FUNCTION__, class_getName([source class]));
}

- (void) packageManager:(ATPackageManager *)manager issuedNotice:(NSString *)notice {
}

- (void) packageManager:(ATPackageManager *)manager issuedError:(id)error {
    printf("%s: %s\n", __FUNCTION__, class_getName([error class]));
}

- (id) packageManagerCanContinue:(ATPackageManager *)manager {
    return [NSNumber numberWithBool:YES];
}

- (void) applicationDidFinishLaunching:(id)unused {
    ATPackageManager *manager = [ATPackageManager sharedPackageManager];
    [manager setDelegate:[[CYAppTapp alloc] init]];
    [manager processQueue];
}

@end

int main(int argc, char *argv[]) {
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    if (argc == 1) {
        printf("AppTapp Tools by Jay Freeman (saurik) http://www.saurik.com/\n\n");
        usage(argv[0]);
        exit(0);
    }

    setreuid(501, 0);
    setregid(501, 0);

    ATPackageManager *manager = [ATPackageManager sharedPackageManager];

    NSMutableArray *considered = nil;
    NSMutableArray *operating = [[[NSMutableArray alloc] initWithCapacity:128] autorelease];

    bool queue = false;

    for (int argi = 1; argi != argc; ++argi) {
        char *arg = argv[argi];

        if (arg[0] == '\0');
        else if (arg[0] == '-') {
            for (int i = 1; arg[i] != '\0'; ++i)
                switch (arg[i]) {
                    case 'L':
                        considered = [manager localPackages];
                    break;

                    case 'R':
                        considered = [manager remotePackages];
                    break;

                    case 'P': {
                        char *plist = argv[++argi];
                        if (plist == NULL || plist[0] == '\0' || plist[0] == '-') {
                            fprintf(stderr, "-P requires a plist argument\n");
                            exit(1);
                        }

                        considered = [NSArray arrayWithContentsOfFile:[NSString stringWithUTF8String:plist]];

                        if (considered == nil) {
                            fprintf(stderr, "cannot load plist %s.\n", plist);
                            exit(1);
                        }
                    } break;

                    case 'a':
                        [operating addObjectsFromArray:considered];
                    break;

                    case 'c':
                    case_c:
                        [operating removeAllObjects];
                    break;

                    case 'i':
                        for (int p = 0, e = [operating count]; p != e; ++p) {
                            NSDictionary *package = [operating objectAtIndex:p];
                            [manager queuePackage:package forOperation:@"Install"];
                            queue = true;
                        }
                    goto case_c;

                    case 'r':
                        for (int p = 0, e = [operating count]; p != e; ++p) {
                            NSDictionary *package = [operating objectAtIndex:p];
                            [manager queuePackage:package forOperation:@"Uninstall"];
                            queue = true;
                        }
                    goto case_c;

                    case 'u':
                        for (int p = 0, e = [operating count]; p != e; ++p) {
                            NSDictionary *package = [operating objectAtIndex:p];
                            [manager queuePackage:package forOperation:@"Update"];
                            queue = true;
                        }
                    goto case_c;

                    case 'p':
                        for (int p = 0, e = [operating count]; p != e; ++p) {
                            NSDictionary *package = [operating objectAtIndex:p];
                            printf("%s", [[package description] UTF8String]);
                            if (p != e - 1)
                                printf(",");
                            printf("\n");
                        }
                    goto case_c;

                    case 'd':
                        for (int p = 0, e = [operating count]; p != e; ++p) {
                            NSDictionary *package = [operating objectAtIndex:p];
                            printf("%s - %s\n",
                                [[package packageBundleIdentifier] UTF8String],
                                [[package packageName] UTF8String]
                            );
                        }
                    goto case_c;

                    /*case 'x':
                        for (int p = 0, e = [operating count]; p != e; ++p) {
                            NSDictionary *package = [operating objectAtIndex:p];
                        }
                    goto case_c;*/

                    default:
                        fprintf(stderr, "unknown flag -%c\n", arg[i]);
                        exit(1);
                    break;
                }
        } else {
            NSString *bid = [NSString stringWithUTF8String:arg];
            NSDictionary *package = [considered packageWithBundleIdentifier:bid];

            if (package != nil)
                [operating addObject:package];
            else {
                fprintf(stderr, "package with id %s not found.\n", [bid UTF8String]);
                exit(1);
            }
        }
    }

    int value;

    if (!queue)
        value = 0;
    else {
        char *uiav[] = {"ati", NULL};
        value = UIApplicationMain(1, uiav, [CYAppTapp class]);
    }

    [pool release];
    return value;
}
