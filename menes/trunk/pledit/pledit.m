#include <CoreFoundation/CFPropertyList.h>
#import <Foundation/Foundation.h>
#include <string.h>
#include <stdint.h>

void SavePropertyList(CFPropertyListRef plist, char *path, CFURLRef url, CFPropertyListFormat format) {
    if (path[0] != '\0')
        url = CFURLCreateFromFileSystemRepresentation(kCFAllocatorDefault, (uint8_t *) path, strlen(path), false);
    CFWriteStreamRef stream = CFWriteStreamCreateWithFile(kCFAllocatorDefault, url);
    CFWriteStreamOpen(stream);
    CFPropertyListWriteToStream(plist, stream, format, NULL);
    CFWriteStreamClose(stream);
}

int main(int argc, char *argv[]) {
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    //CFStringEncoding utf8 = kCFStringEncodingUTF8;

    char *path = argv[1];
    CFURLRef url = CFURLCreateFromFileSystemRepresentation(kCFAllocatorDefault, (uint8_t *) path, strlen(path), false);

    CFPropertyListRef plist; {
        CFReadStreamRef stream = CFReadStreamCreateWithFile(kCFAllocatorDefault, url);
        CFReadStreamOpen(stream);
        plist = CFPropertyListCreateFromStream(kCFAllocatorDefault, stream, 0, kCFPropertyListMutableContainers, NULL, NULL);
        CFReadStreamClose(stream);
    }

    /*for (int argi = 2; argi != argc; ++argi) {
        char *args = argv[i];
        if (args[0] != '-') {
        } else for (char *argp = args + 1; *argp != '\0'; ++argp)
            switch (*argp) {
                case 'P': {
                    CFStringRef key = CFStringCreateWithCString(kCFAllocatorDefault, argv[1], utf8);
                    char *ckey = argv[++i];
                    char *cpath = argv[++i];
                    opath = CFStringCreateWithCString(kCFAllocatorDefault, argv[++i], utf8);

                } break;

                case 'b': {
                    char *path = argv[++i];
                    SavePropertyList(plist, path, url, kCFPropertyListBinaryFormat_v1_0);
                } break;

                case 'x': {
                    char *path = argv[++i];
                    SavePropertyList(plist, argv[i + 1], url, kCFPropertyListXMLFormat_v1_0);
                } break;
            }
    }*/

    if (strcmp(argv[2], "-a") == 0) {
    NSMutableDictionary *root = (NSMutableDictionary *) plist;
    if (root == nil)
        exit(0);
    NSMutableDictionary *ev = [root objectForKey:@"EnvironmentVariables"];
    if (ev == nil) {
        ev = [NSMutableDictionary dictionaryWithCapacity:16];
        [root setObject:ev forKey:@"EnvironmentVariables"];
    }
    NSString *il = [ev objectForKey:@"DYLD_INSERT_LIBRARIES"];
    NSString *st = [NSString stringWithUTF8String:argv[3]];
    if (il == nil) {
        [ev setObject:st forKey:@"DYLD_INSERT_LIBRARIES"];
    } else if ([il length] == 0) {
    [ev setObject:st forKey:@"DYLD_INSERT_LIBRARIES"];
    } else {
    NSArray *cm = [il componentsSeparatedByString:@":"];
    unsigned index = [cm indexOfObject:st];
    if (index != INT_MAX)
        exit(0);
    [ev setObject:[NSString stringWithFormat:@"%@:%@", il, st] forKey:@"DYLD_INSERT_LIBRARIES"];
    }
    } else {
    NSMutableDictionary *root = (NSMutableDictionary *) plist;
    if (root == nil)
        exit(0);
    NSMutableDictionary *ev = [root objectForKey:@"EnvironmentVariables"];
    if (ev == nil)
        exit(0);
    NSString *il = [ev objectForKey:@"DYLD_INSERT_LIBRARIES"];
    if (il == nil)
        exit(0);
    NSArray *cm = [il componentsSeparatedByString:@":"];
    NSString *st = [NSString stringWithUTF8String:argv[3]];
    unsigned index = [cm indexOfObject:st];
    if (index == INT_MAX)
        exit(0);
    NSMutableArray *cmm = [NSMutableArray arrayWithCapacity:16];
    [cmm addObjectsFromArray:cm];
    [cmm removeObject:st];
    if ([cmm count] != 0)
        [ev setObject:[cmm componentsJoinedByString:@":"] forKey:@"DYLD_INSERT_LIBRARIES"];
    else if ([ev count] == 1)
        [root removeObjectForKey:@"EnvironmentVariables"];
    else
        [ev removeObjectForKey:@"DYLD_INSERT_LIBRARIES"];
    }

    SavePropertyList(plist, "", url, kCFPropertyListXMLFormat_v1_0);

    [pool release];
    return 0;
}
