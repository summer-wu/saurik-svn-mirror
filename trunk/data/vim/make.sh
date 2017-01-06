pkg:setup

for ((x = 1; x != 23; ++x)); do
    patch -p0 <"${PKG_DATA}/patches/7.2.$(printf '%.3u\n' "$x")"
done

cd src
autoconf
cd ..
pkg:configure --enable-gui=no --without-x vim_cv_toupper_broken=no --with-tlib=ncurses vim_cv_terminfo=yes vim_cv_tty_group=4 vim_cv_tty_mode=0620 vim_cv_getcwd_broken=no vim_cv_stat_ignores_slash=no ac_cv_sizeof_int=4 vim_cv_memmove_handles_overlap=yes
make
pkg:install
pkg: mkdir -p /etc/profile.d
pkg: cp -a %/vim.sh /etc/profile.d
