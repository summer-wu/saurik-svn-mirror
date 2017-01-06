if [ \( "${CFG_TEST__L__PROC_CURPROC_FILE+@}" -o "${CFG_TEST__L__PROC_SELF_EXE+@}" \) ]; then
    mak:build *
    hi-ccxx-flags -DAPI_HAS_PROCFS
fi
