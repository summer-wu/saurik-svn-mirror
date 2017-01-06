if [ "${CFG_INCLUDE_CARBON_CARBON_H+@}" ]; then
    mak:build *

    hi-ccxx-flags -DAPI_HAS_CARBON
    hi-ld-flags -fCoreFoundation -fCoreServices
fi
