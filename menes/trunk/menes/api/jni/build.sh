if [ "${CFG_FRAMEWORK_JAVAVM+@}" ]; then
    mak:build *

    hi-ccxx-flags -DAPI_HAS_JNI
    hi-ld-flags -fJavaVM
elif [ "${CFG_JDK_PATH+@}" ]; then
    mak:build *

    hi-ccxx-flags -DAPI_HAS_JNI

    IFS=$'\n'
    hi-ccxx-flags $(
        IFS=$'\n'
        for h in $(find $CFG_JDK_PATH -name 'jni.h' -o -name 'jni_md.h'); do
            dirname "$h"
        done | sed -e 's/^/-I/'
    )

    libjava=$(dirname $(find "$CFG_JDK_PATH" -name 'libjava.*') | head -n 1)
    hi-ld-flags -{L,r}"$libjava"{,/server}
    hi-ld-flags -ljava -ljvm -lverify
fi
