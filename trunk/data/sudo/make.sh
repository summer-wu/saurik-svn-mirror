pkg:setup
pkg:configure sudo_cv_uid_t_len=10 sudo_cv_type_long_is_quad=no --without-pam --with-env-editor
make
pkg:install
