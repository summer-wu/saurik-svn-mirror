pkg:setup
cd dist
./s_config
cd ../build_unix
PKG_CONF=../dist/configure pkg:configure --with-mutex=Darwin/_spin_lock_try
make
pkg:install
