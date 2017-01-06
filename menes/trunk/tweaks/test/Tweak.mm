#include <substrate.h>

#import <Foundation/Foundation.h>

bool (*_Z24GetFileNameForThisActionmPcRb)(unsigned long a0, char *a1, bool &a2);

MSHook(bool, _Z24GetFileNameForThisActionmPcRb, unsigned long a0, char *a1, bool &a2) {
    NSLog(@"TS:Debug:GetFileNameForThisAction(%u, %p, %u)", a0, a1, a2);
    bool value = __Z24GetFileNameForThisActionmPcRb(a0, a1, a2);
    return value;
}

#define AudioToolbox "/System/Library/Frameworks/AudioToolbox.framework/AudioToolbox"

template <typename Type_>
static void nlset(Type_ &function, struct nlist *nl, size_t index) {
    struct nlist &name(nl[index]);
    uintptr_t value(name.n_value);
    if ((name.n_desc & N_ARM_THUMB_DEF) != 0)
        value |= 0x00000001;
    function = reinterpret_cast<Type_>(value);
}

extern "C" void AudioServicesPlaySystemSound(int sound);

MSHook(void, AudioServicesPlaySystemSound, int sound) {
    NSLog(@"!!!!!! hooking AudioServicesPlaySystemSound %d", sound);
    _AudioServicesPlaySystemSound(sound);
    NSLog(@"!!!!!! hooking OVER WITH %d", sound);
}

extern bool MSDebug;

MSInitialize {
    if (dlopen(AudioToolbox, RTLD_LAZY | RTLD_NOLOAD) != NULL) {
        struct nlist nl[2];
        memset(nl, 0, sizeof(nl));
        nl[0].n_un.n_name = (char *) "__Z24GetFileNameForThisActionmPcRb";
        nlist(AudioToolbox, nl);
        nlset(_Z24GetFileNameForThisActionmPcRb, nl, 0);
        MSHookFunction(_Z24GetFileNameForThisActionmPcRb, &$_Z24GetFileNameForThisActionmPcRb, &__Z24GetFileNameForThisActionmPcRb);
        MSDebug = true;
        MSHookFunction(AudioServicesPlaySystemSound, MSHake(AudioServicesPlaySystemSound));
        MSDebug = false;
    }
}
