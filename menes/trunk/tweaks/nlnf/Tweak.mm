#import <CydiaSubstrate/CydiaSubstrate.h>
#import <CoreFoundation/CoreFoundation.h>

extern "C" CFStringRef CPPhoneNumberCopyNetworkCountryCode();

MSHook(CFStringRef, UIDefaultCountryCode) {
    return CPPhoneNumberCopyNetworkCountryCode() ?: _UIDefaultCountryCode();
}

MSHook(CFStringRef, ABDefaultCountryCode) {
    return CPPhoneNumberCopyNetworkCountryCode() ?: _ABDefaultCountryCode();
}

MSInitialize {
    MSImageRef UIKit(MSGetImageByName("/System/Library/Frameworks/UIKit.framework/UIKit"));
    MSHookFunction(UIKit, "_UIDefaultCountryCode", MSHake(UIDefaultCountryCode));
    MSHookFunction(NULL, "_ABDefaultCountryCode", MSHake(ABDefaultCountryCode));
}
