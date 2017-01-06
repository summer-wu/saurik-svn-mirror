/* #include Directives {{{ */
#include <Foundation/NSURL.h>
#include <UIKit/UIKit.h>
#import <GraphicsServices/GraphicsServices.h>

extern "C" {
#include <mach-o/nlist.h>
}

#include <objc/objc-class.h>
#include <sys/sysctl.h>

#include <errno.h>
#include <string.h>
/* }}} */
/* Extension Keywords {{{ */
#define _trace() fprintf(stderr, "_trace()@%s:%u[%s]\n", __FILE__, __LINE__, __FUNCTION__)

#define _assert(test) do \
    if (!(test)) { \
        fprintf(stderr, "_assert(%d:%s)@%s:%u[%s]\n", errno, #test, __FILE__, __LINE__, __FUNCTION__); \
        exit(-1); \
    } \
while (false)
/* }}} */
/* Miscellaneous Messages {{{ */
@interface WebView
- (void) setApplicationNameForUserAgent:(NSString *)applicationName;
- (id) frameLoadDelegate;
- (void) setFrameLoadDelegate:(id)delegate;
- (NSString *) mainFrameURL;
@end

@interface NSString (Cydia)
- (NSString *) stringByAddingPercentEscapes;
- (NSString *) stringByReplacingCharacter:(unsigned short)arg0 withCharacter:(unsigned short)arg1;
@end
/* }}} */
/* CoreGraphicsServices Primitives {{{ */
class CGColor {
  private:
    CGColorRef color_;

  public:
    CGColor(CGColorSpaceRef space, float red, float green, float blue, float alpha) {
        float color[] = {red, green, blue, alpha};
        color_ = CGColorCreate(space, color);
    }

    ~CGColor() {
        CGColorRelease(color_);
    }

    operator CGColorRef() {
        return color_;
    }
};

class GSFont {
  private:
    GSFontRef font_;

  public:
    ~GSFont() {
        /* XXX: no GSFontRelease()? */
        CFRelease(font_);
    }
};
/* }}} */

const char *Url_ = NULL;
const char *Machine_ = NULL;
const char *SerialNumber_ = NULL;

@interface Cydia : UIApplication {
    bool loading_;
    bool reloading_;
    UIWindow *window_;
    UIView *underlay_;
    UINavigationBar *navbar_;
    UIScroller *scroller_;
    UIWebView *webview_;
    NSMutableArray *urls_;
    UIProgressIndicator *indicator_;
}

- (void) reloadData;

- (void) navigationBar:(UINavigationBar *)navbar buttonClicked:(int)button;
- (void) navigationBar:(UINavigationBar *)navbar poppedItem:(UINavigationItem *)item;
- (void) navigationBar:(UINavigationBar *)navbar pushedItem:(UINavigationItem *)item;

- (void) alertSheet:(UIAlertSheet *)sheet buttonClicked:(int)button;

- (void) view:(UIView *)sender didSetFrame:(CGRect)frame;
- (void) view:(UIView *)sender didSetFrame:(CGRect)frame oldFrame:(CGRect)old;

- (void) webView:(WebView *)sender didReceiveTitle:(NSString *)title forFrame:(WebFrame *)frame;
- (void) webView:(WebView *)sender didStartProvisionalLoadForFrame:(WebFrame *)frame;
- (void) webView:(WebView *)sender didFinishLoadForFrame:(WebFrame *)frame;

- (void) applicationDidFinishLaunching:(id)unused;
@end

@implementation Cydia

- (NSMethodSignature *) methodSignatureForSelector:(SEL)selector {
    fprintf(stderr, "[%s]S-%s\n", self->isa->name, sel_getName(selector));
    return [super methodSignatureForSelector:selector];
}

- (BOOL) respondsToSelector:(SEL)selector {
    fprintf(stderr, "[%s]R-%s\n", self->isa->name, sel_getName(selector));
    return [super respondsToSelector:selector];
}

- (void) _pushItem {
    UINavigationItem *navitem = [[[UINavigationItem alloc] initWithTitle:nil] autorelease];
    [navitem setBackButtonTitle:@"..."];
    [navbar_ pushNavigationItem:navitem];
}

- (void) _clearItem {
    UINavigationItem *navitem = [navbar_ topItem];
    [navitem setTitle:nil];
    [navitem setBackButtonTitle:@"..."];
}

- (void) reloadData {
    NSURL *url = [[[urls_ lastObject] retain] autorelease];
    [urls_ removeLastObject];

    reloading_ = true;
    [self _clearItem];

    NSMutableURLRequest *request = [NSMutableURLRequest
        requestWithURL:url
        cachePolicy:NSURLRequestReloadIgnoringCacheData
        timeoutInterval:30.0
    ];

    [request addValue:[NSString stringWithCString:Machine_] forHTTPHeaderField:@"X-Machine"];
    [request addValue:[NSString stringWithCString:SerialNumber_] forHTTPHeaderField:@"X-Serial-Number"];

    [webview_ loadRequest:request];
}

- (void) navigationBar:(UINavigationBar *)navbar buttonClicked:(int)button {
    switch (button) {
        case 0:
            [self reloadData];
        break;

        case 1:
            UIAlertSheet *sheet = [[[UIAlertSheet alloc]
                initWithTitle:@"About iPhone Pamphlet"
                buttons:[NSArray arrayWithObjects:@"Close", nil]
                defaultButtonIndex:0
                delegate:self
                context:self
            ] autorelease];

            [sheet setBodyText:
                @"Copyright (C) 2008\n"
                "Jay Freeman (saurik)\n"
                "saurik@saurik.com\n"
                "http://www.saurik.com/\n"
                "\n"
                "The Okori Group\n"
                "http://www.theokorigroup.com/\n"
                "\n"
                "College of Creative Studies,\n"
                "University of California,\n"
                "Santa Barbara\n"
                "http://www.ccs.ucsb.edu/"
            ];

            [sheet popupAlertAnimated:YES];
        break;
    }
}

- (void) _showButtons {
    NSString *left;
    NSString *right;

    if ([[navbar_ navigationItems] count] == 1)
        left = @"About";
    else
        left = nil;

    if (loading_) {
        right = @"";
        [indicator_ startAnimation];
    } else {
        right = @"Reload";
        [indicator_ stopAnimation];
    }

    [navbar_ showButtonsWithLeftTitle:left rightTitle:right];
}

- (void) navigationBar:(UINavigationBar *)navbar poppedItem:(UINavigationItem *)item {
    [urls_ removeLastObject];
    [self reloadData];
    [self _showButtons];
}

- (void) navigationBar:(UINavigationBar *)navbar pushedItem:(UINavigationItem *)item {
    if ([[navbar_ navigationItems] count] != 1)
        [self _showButtons];
}

- (void) alertSheet:(UIAlertSheet *)sheet buttonClicked:(int)button {
    [sheet dismiss];
}

- (void) view:(UIView *)sender didSetFrame:(CGRect)frame {
    [scroller_ setContentSize:frame.size];
}

- (void) view:(UIView *)sender didSetFrame:(CGRect)frame oldFrame:(CGRect)old {
    [self view:sender didSetFrame:frame];
}

- (void) webView:(WebView *)sender didReceiveTitle:(NSString *)title forFrame:(WebFrame *)frame {
    UINavigationItem *navitem = [navbar_ topItem];
    [navitem setTitle:title];
    [navitem setBackButtonTitle:title];
}

- (void) webView:(WebView *)sender didStartProvisionalLoadForFrame:(WebFrame *)frame {
    if (reloading_)
        reloading_ = false;
    else
        [self _pushItem];

    WebView *webview = [webview_ webView];
    NSString *href = [webview mainFrameURL];
    [urls_ addObject:[NSURL URLWithString:href]];

    loading_ = true;
    [self _showButtons];

    CGRect webrect = [scroller_ bounds];
    webrect.size.height = 0;
    [webview_ setFrame:webrect];
}

- (void) webView:(WebView *)sender didFinishLoadForFrame:(WebFrame *)frame {
    loading_ = false;
    [self _showButtons];
}

- (void) applicationDidFinishLaunching:(id)unused {
    loading_ = false;
    reloading_ = false;

    CGRect screenrect = [UIHardware fullScreenApplicationContentRect];
    window_ = [[UIWindow alloc] initWithContentRect:screenrect];

    [window_ orderFront: self];
    [window_ makeKey: self];
    [window_ _setHidden: NO];

    underlay_ = [[UIView alloc] initWithFrame:[window_ bounds]];
    [window_ setContentView:underlay_];

    CGSize navsize = [UINavigationBar defaultSize];
    CGRect navrect = {{0, 0}, navsize};

    navbar_ = [[UINavigationBar alloc] initWithFrame:navrect];
    [underlay_ addSubview:navbar_];

    [navbar_ setBarStyle:1];
    [navbar_ setDelegate:self];

    struct CGRect subbounds = [underlay_ bounds];
    subbounds.origin.y += navsize.height;
    subbounds.size.height -= navsize.height;

    UIImageView *pinstripe = [[[UIImageView alloc] initWithFrame:subbounds] autorelease];
    [pinstripe setImage:[UIImage applicationImageNamed:@"pinstripe.png"]];
    [underlay_ addSubview:pinstripe];

    scroller_ = [[UIScroller alloc] initWithFrame:subbounds];
    [underlay_ addSubview:scroller_];

    [scroller_ setScrollingEnabled:YES];
    [scroller_ setAdjustForContentSizeChange:YES];
    [scroller_ setClipsSubviews:YES];
    [scroller_ setAllowsRubberBanding:YES];
    [scroller_ setScrollDecelerationFactor:0.99];
    [scroller_ setDelegate:self];

    CGRect webrect = [scroller_ bounds];
    webrect.size.height = 0;

    webview_ = [[UIWebView alloc] initWithFrame:webrect];
    [scroller_ addSubview:webview_];

    [webview_ setTilingEnabled:YES];
    [webview_ setTileSize:CGSizeMake(screenrect.size.width, 500)];
    [webview_ setAutoresizes:YES];
    [webview_ setDelegate:self];

    CGSize indsize = [UIProgressIndicator defaultSizeForStyle:0];
    indicator_ = [[UIProgressIndicator alloc] initWithFrame:CGRectMake(281, 13, indsize.width, indsize.height)];
    [indicator_ setStyle:0];
    [underlay_ addSubview:indicator_];

    WebView *webview = [webview_ webView];
    [webview setApplicationNameForUserAgent:@"Pamphlet/0.9.2318"];
    [webview setFrameLoadDelegate:self];

    urls_ = [[NSMutableArray alloc] initWithCapacity:16];
    [urls_ addObject:[NSURL URLWithString:[NSString stringWithCString:Url_]]];

    [self _pushItem];
    [self reloadData];
}

@end

int main(int argc, char *argv[]) {
    struct nlist nl[2];
    memset(nl, 0, sizeof(nl));
    nl[0].n_un.n_name = "_useMDNSResponder";
    nlist("/usr/lib/libc.dylib", nl);
    if (nl[0].n_type != N_UNDF)
        *(int *) nl[0].n_value = 0;

    Url_ = "http://cydia.saurik.com/gdcmeet08/";

    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    size_t size;
    sysctlbyname("hw.machine", NULL, &size, NULL, 0);
    char *machine = new char[size];
    sysctlbyname("hw.machine", machine, &size, NULL, 0);
    Machine_ = machine;

    if (CFMutableDictionaryRef dict = IOServiceMatching("IOPlatformExpertDevice"))
        if (io_service_t service = IOServiceGetMatchingService(kIOMasterPortDefault, dict)) {
            if (CFTypeRef serial = IORegistryEntryCreateCFProperty(service, CFSTR(kIOPlatformSerialNumberKey), kCFAllocatorDefault, 0)) {
                SerialNumber_ = strdup(CFStringGetCStringPtr((CFStringRef) serial, CFStringGetSystemEncoding()));
                CFRelease(serial);
            }

            IOObjectRelease(service);
        }

    UIApplicationMain(argc, argv, [Cydia class]);
    [pool release];
    return 0;
}
