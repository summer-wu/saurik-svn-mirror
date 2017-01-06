#include <substrate.h>

#import <CoreFoundation/CoreFoundation.h>
#import <UIKit/UIKit.h>
#import <SpringBoard/SBPowerDownView.h>

#define _trace() do { \
    fprintf(stderr, "_trace()@%s:%u[%s]\n", \
        __FILE__, __LINE__, __FUNCTION__\
    ); \
} while (false)

static UITextView *text_;
static NSInteger size_;
static FILE *tail_;

static Class $SBPowerDownView;

static void ScrollToBottom() {
    if (![text_ superview] || [text_ isDragging] || [text_ isTracking])
        return;

    CGRect rect;
    rect.origin.x = 0.0f;
    rect.origin.y = [text_ contentSize].height - 1.0f;
    rect.size.height = 1.0f;
    rect.size.width = 1.0f;
    [text_ scrollRectToVisible:rect animated:YES];
}

static void SBPowerDownView$cy$addSyslogOutput$(SBPowerDownView *self, SEL sel, NSString *output) {
    if (text_ == nil)
        return;

    [text_ setSelectedRange:NSMakeRange(size_, 0)];
    [text_ insertText:output];

    size_ += [output length];
    ScrollToBottom();
}

static void SBPowerDownView$cy$readSyslogInBackground(SBPowerDownView *self, SEL sel) {
    tail_ = fopen("/var/log/syslog", "rb");
    if (tail_ == NULL)
        return;
    fseek(tail_, -4096 * 4, SEEK_END);

    uint8_t data[4096 * 4];
    size_t hold(0);

    while (!ferror(tail_)) {
        size_t size(fread(data + hold, 1, sizeof(data) - hold, tail_));
        if (size == 0) {
            fseek(tail_, 0, SEEK_CUR);
            usleep(500000);
            continue;
        }

        size += hold;
        hold = size;

        goto utf8;
      size:
        if (size == 0) {
            // missing initial byte
            hold = 0;
            continue;
        }
      utf8:
        if ((data[size - 1] & 0xc0) == 0x80) {
            --size;
            goto size;
        }

        hold -= size;

        uint8_t last(data[size - 1]);
        if ((last & 0x80) != 0x00) {
            size_t need;
            if ((last & 0xe0) == 0xc0)
                need = 1;
            else if ((last & 0xf0) == 0xe0)
                need = 2;
            else if ((last & 0xf8) == 0xf0)
                need = 3;
            else {
                // invalid UTF-8 byte
                hold = 0;
                continue;
            }

            if (hold >= need) {
                hold -= need;
                size += need;
            } /*else if (size + hold == sizeof(data)) {
                // character longer than buffer
                hold = 0;
                continue;
            }*/ else {
                ++hold;
                --size;
            }
        }

        if (size != 0) {
            NSString *text([[NSString alloc] initWithBytes:data length:size encoding:NSUTF8StringEncoding]);

            [self
                performSelectorOnMainThread:@selector(cy$addSyslogOutput:)
                withObject:text
                waitUntilDone:NO
            ];

            [text release];
        }

        memmove(data, data + size, hold);
    }
}

MSHook(id, SBPowerDownView$initWithFrame$, SBPowerDownView *self, SEL sel, CGRect frame) {
    if ((self = _SBPowerDownView$initWithFrame$(self, sel, frame)) != nil) {
        text_ = [[UITextView alloc] initWithFrame:CGRectMake(0,116,320,268)];
        [text_ setTextColor:[UIColor whiteColor]];
        [text_ setBackgroundColor:[UIColor clearColor]];
        [text_ setFont:[UIFont systemFontOfSize:[UIFont smallSystemFontSize]]];
        [text_ setBottomBufferHeight:0.0f];
        [text_ setAlpha:0.0f];

        [self performSelectorInBackground:@selector(cy$readSyslogInBackground) withObject:nil];
    } return self;
}

MSHook(void, SBPowerDownView$animateIn, SBPowerDownView *self, SEL sel) {
    [self addSubview:text_];

    _SBPowerDownView$animateIn(self, sel);

    [UIView beginAnimations:@"CyLoggerAnimateIn"];

    [UIView setAnimationDuration:0.5];
    [UIView setAnimationCurve:1];

    [text_ setAlpha:1.0f];

    [UIView endAnimations];
}

MSHook(void, SBPowerDownView$animateOut, SBPowerDownView *self, SEL sel) {
    _SBPowerDownView$animateOut(self, sel);

    [UIView beginAnimations:@"CyLoggerAnimateOut"];

    [UIView setAnimationDuration:0.5];
    [UIView setAnimationCurve:2];

    [text_ setAlpha:0.0f];

    [UIView endAnimations];
}

MSHook(void, SBPowerDownView$finishedAnimatingOut, SBPowerDownView *self, SEL sel) {
    _SBPowerDownView$finishedAnimatingOut(self, sel);
    [text_ removeFromSuperview];
}

extern "C" void TweakInitialize() {
    $SBPowerDownView = objc_getClass("SBPowerDownView");

    class_addMethod($SBPowerDownView, @selector(cy$addSyslogOutput:), reinterpret_cast<IMP>(&SBPowerDownView$cy$addSyslogOutput$), "v@:@");
    class_addMethod($SBPowerDownView, @selector(cy$readSyslogInBackground), reinterpret_cast<IMP>(&SBPowerDownView$cy$readSyslogInBackground), "v@:");

    MSHookMessage($SBPowerDownView, @selector(initWithFrame:), MSHake(SBPowerDownView$initWithFrame$));
    MSHookMessage($SBPowerDownView, @selector(animateIn), MSHake(SBPowerDownView$animateIn));
    MSHookMessage($SBPowerDownView, @selector(animateOut), MSHake(SBPowerDownView$animateOut));
    MSHookMessage($SBPowerDownView, @selector(finishedAnimatingOut), MSHake(SBPowerDownView$finishedAnimatingOut));
}
