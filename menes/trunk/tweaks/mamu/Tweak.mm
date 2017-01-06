#include <substrate.h>

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

#import <MessageUI/MailComposeController.h>
#import <MessageUI/ComposeBodyField.h>
#import <MessageUI/MFMailComposeViewController.h>

MSHook(id, MailCompositionContext$initNewMessageWithURL$, MailCompositionContext *self, SEL sel, NSURL *url) {
    if ((self = _MailCompositionContext$initNewMessageWithURL$(self, sel, url)) != nil) do {
        if (url == nil)
            break;
        NSString *scheme([url scheme]);
        if (scheme == nil || ![[scheme lowercaseString] isEqualToString:@"mailto"])
            break;
        NSString *href([url absoluteString]);
        NSRange question([href rangeOfString:@"?"]);
        if (question.location == NSNotFound)
            break;
        NSString *query([href substringFromIndex:(question.location + 1)]);
        if ([query length] == 0)
            break;
        NSMutableArray *attachments([[[NSMutableArray alloc] init] autorelease]);
        for (NSString *assign in [query componentsSeparatedByString:@"&"]) {
            NSRange equal([assign rangeOfString:@"="]);
            if (equal.location == NSNotFound)
                continue;
            NSString *name([assign substringToIndex:equal.location]);
            NSString *value([assign substringFromIndex:(equal.location + 1)]);
            value = [value stringByReplacingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
            if ([name isEqualToString:@"attachment"])
                [attachments addObject:value];
        }
        for (NSString *attachment in attachments)
            if (NSData *data = [NSData dataWithContentsOfFile:attachment])
                [self addAttachmentData:data mimeType:nil fileName:[attachment lastPathComponent]];
    } while (false); return self;
}

MSHook(void, MailComposeController$_setupForURL$, MailComposeController *self, SEL sel, NSURL *url) {
    if (url == nil)
        return;
    NSString *scheme([url scheme]);
    if (scheme == nil || ![[scheme lowercaseString] isEqualToString:@"mailto"])
        return;
    NSString *href([url absoluteString]);
    NSRange question([href rangeOfString:@"?"]);
    if (question.location == NSNotFound)
        return;
    NSString *query([href substringFromIndex:(question.location + 1)]);
    if ([query length] == 0)
        return;
    BOOL contents(NO);
    NSMutableArray *attachments([[[NSMutableArray alloc] init] autorelease]);
    for (NSString *assign in [query componentsSeparatedByString:@"&"]) {
        NSRange equal([assign rangeOfString:@"="]);
        if (equal.location == NSNotFound)
            continue;
        NSString *name([assign substringToIndex:equal.location]);
        NSString *value([assign substringFromIndex:(equal.location + 1)]);
        value = [value stringByReplacingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
        if ([name isEqualToString:@"attachment"])
            [attachments addObject:value];
        else if ([name isEqualToString:@"attachdircontent"])
            contents = [value boolValue];
    }
    if (contents) {
        ComposeBodyField *&_bodyField(MSHookIvar<ComposeBodyField *>(self, "_bodyField"));
        [_bodyField setLayoutInterval:10];
    }
    for (NSString *attachment in attachments)
        [self addInlineAttachmentAtPath:attachment includeDirectoryContents:contents];
}

NSArray *NSFileManager$rawDirectoryContentsAtPath$(NSFileManager *self, SEL sel, NSString *path) {
    return [self directoryContentsAtPath:path];
}

extern "C" void TweakInitialize() {
    MSHookMessage(objc_getClass("NSFileManager"), @selector(rawDirectoryContentsAtPath:), &NSFileManager$rawDirectoryContentsAtPath$);
    Class $MailComposeController(objc_getClass("MailComposeController"));
    _MailComposeController$_setupForURL$ = MSHookMessage($MailComposeController, @selector(_setupForMessageWithURL:), &$MailComposeController$_setupForURL$);
    if (_MailComposeController$_setupForURL$ == NULL) {
        _MailComposeController$_setupForURL$ = MSHookMessage($MailComposeController, @selector(setupForURL:), &$MailComposeController$_setupForURL$);
        _MailCompositionContext$initNewMessageWithURL$ = MSHookMessage(objc_getClass("MailCompositionContext"), @selector(initNewMessageWithURL:), &$MailCompositionContext$initNewMessageWithURL$);
    }
}
