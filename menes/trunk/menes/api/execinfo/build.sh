if [ "${CFG_INCLUDE_EXECINFO_H+@}" ]; then
    mak:build *
    hi-ccxx-flags -DAPI_HAS_EXECINFO

    if [ "${CFG_LINK_EXECINFO+@}" ]; then
        hi-ld-flags -lexecinfo

		if [ "$CFG_KERNEL_NAME" = 'FreeBSD' ] && [ "$CFG_ARCHITECTURE" = 'amd64' ]; then
			lo-ccxx-flags -fno-omit-frame-pointer
		fi
    fi
fi
