pkg:extract
cd *

for ((x = 1; x != 41; ++x)); do
    patch -p0 <"${PKG_DATA}/bash40-$(printf '%.3u\n' "$x")"
done

pkg:patch

pkg:configure --disable-nls --with-installed-readline --disable-largefile bash_cv_dev_fd=absent bash_cv_sys_named_pipes=present bash_cv_job_control_missing=present bash_cv_func_sigsetjmp=present bash_cv_func_ctype_nonascii=no bash_cv_must_reinstall_sighandlers=no bash_cv_func_strcoll_broken=yes ac_cv_c_stack_direction=-1 ac_cv_func_mmap_fixed_mapped=yes gt_cv_int_divbyzero_sigfpe=no ac_cv_func_setvbuf_reversed=no ac_cv_func_strcoll_works=yes ac_cv_func_working_mktime=yes ac_cv_type_getgroups=gid_t bash_cv_dup2_broken=no ac_cv_prog_cc_g=no ac_cv_rl_version=6.0
#pgrp_pipe...

pkg:make
pkg:install
pkg:bin bash
ln -s bash "${PKG_DEST}/bin/sh"
