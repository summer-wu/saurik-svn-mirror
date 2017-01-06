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
#include <stdbool.h>

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

#import <AppTapp/ATPackageManager.h>
#import <AppTapp/NSArray-AppTappSources.h>
#import <AppTapp/NSDictionary-AppTappSource.h>

void usage(const char *argv0) {
    printf("usage: %s -<mode> [sources...]\n", argv0);
    printf("  -l - list all or specified\n");
    printf("  -p - dump all or specified\n");
    printf("  -u - update all or specified\n");
    printf("  -a - add specified\n");
    printf("  -r - remove specified\n");
}

void run(char mode, ATPackageManager *manager, NSDictionary *source) {
    switch (mode) {
        case 'l':
            printf("%s - %s\n",
                [[source sourceLocation] UTF8String],
                [[source sourceName] UTF8String]
            );
        break;

        case 'u':
            [manager refreshSource:source];
        break;

        case 'p':
            printf("%s\n", [[source description] UTF8String]);
        break;
    }
}

int main(int argc, char *argv[]) {
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    if (argc == 1 || argv[1][0] != '-' || strchr("alrux", argv[1][1]) == NULL || argv[1][2] != '\0') {
        printf("AppTapp Tools by Jay Freeman (saurik) http://www.saurik.com/\n\n");
        usage(argv[0]);
        exit(0);
    }

    char mode = argv[1][1];

    setreuid(501, 0);
    setregid(501, 0);

    ATPackageManager *manager = [ATPackageManager sharedPackageManager];
    NSArray *sources = [manager packageSources];

    if (argc == 2)
        if (mode == 'u')
            [manager refreshAllSources];
        else for (int i = 0, e = [sources count]; i != e; ++i)
            run(mode, manager, [sources objectAtIndex:i]);
    else {
        for (int argi = 2; argi != argc; ++argi) {
            NSString *location = [NSString stringWithUTF8String:argv[argi]];

            switch (mode) {
                case 'a':
                    [manager addSourceWithLocation:location];
                break;

                case 'r':
                    [manager removeSourceWithLocation:location];
                break;

                default:
                    run(mode, manager, [sources sourceWithLocation:location]);
                break;
            }
        }

        if (mode == 'u')
            [manager saveState];
    }

    [pool release];
    return 0;
}
