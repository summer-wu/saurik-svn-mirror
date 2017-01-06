#include <substrate.h>

#import <Foundation/NSArray.h>
#import <Foundation/NSDictionary.h>
#import <Foundation/NSNull.h>

#import <CoreGraphics/CGGeometry.h>
#import <SpringBoard/SBButtonBar.h>
#import <SpringBoard/SBIcon.h>

#define libmis "/usr/lib/libmis.dylib"

uintptr_t (*MISCopySignerCertificateData)(NSString *path, NSData **data);
uintptr_t (*MISValidateSignatureAndCopyInfo)(NSString *path, uintptr_t b, NSDictionary **info);
NSString **kMISValidationInfoSignerCertificate;
//NSString **kMISValidationInfoValidatedByProfile;

uintptr_t $MISValidateSignatureAndCopyInfo(NSString *path, uintptr_t b, NSDictionary **info) {
    NSData *data;
    MISCopySignerCertificateData(path, &data);

    *info = [[NSDictionary alloc] initWithObjectsAndKeys:
        data, *kMISValidationInfoSignerCertificate,
        //[NSNumber numberWithBool:YES], *kMISValidationInfoValidatedByProfile,
    nil];

    return 0;
}

extern "C" void TweakInitialize() {
    void *handle(dlopen(libmis, RTLD_LAZY | RTLD_NOLOAD));
    if (handle == NULL)
        return;

    MISCopySignerCertificateData = reinterpret_cast<uintptr_t (*)(NSString *, NSData **)>(dlsym(handle, "MISCopySignerCertificateData"));
    if (MISCopySignerCertificateData == NULL)
        return;

    MISValidateSignatureAndCopyInfo = reinterpret_cast<uintptr_t (*)(NSString *, uintptr_t, NSDictionary **)>(dlsym(handle, "MISValidateSignatureAndCopyInfo"));
    if (MISValidateSignatureAndCopyInfo == NULL)
        return;

    kMISValidationInfoSignerCertificate = reinterpret_cast<NSString **>(dlsym(handle, "kMISValidationInfoSignerCertificate"));
    if (kMISValidationInfoSignerCertificate == NULL)
        return;

    /*kMISValidationInfoValidatedByProfile = reinterpret_cast<NSString **>(dlsym(handle, "kMISValidationInfoValidatedByProfile"));
    if (kMISValidationInfoValidatedByProfile == NULL)
        return;*/

    MSHookFunction(MISValidateSignatureAndCopyInfo, &$MISValidateSignatureAndCopyInfo);
}
