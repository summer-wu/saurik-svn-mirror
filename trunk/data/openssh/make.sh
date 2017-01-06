pkg:setup
autoconf
pkg:configure --disable-strip --sysconfdir=/etc/ssh --disable-libutil --disable-utmp --disable-wtmp ac_cv_path_AR=arm-apple-darwin9-ar --
pkg:make
pkg:install INSTALL_SSH_RAND_HELPER=yes
pkg: cp -a %/sshd-keygen-wrapper /usr/libexec
pkg: mkdir -p /Library/LaunchDaemons
pkg: cp -a %/com.openssh.sshd.plist /Library/LaunchDaemons
pkg: cp -af %/ssh{d,}_config /etc/ssh
