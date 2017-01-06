pkg: mkdir -p /etc/apt/sources.list.d
pkg: cp -a %/hackndev.org.list /etc/apt/sources.list.d
pkg: mkdir -p /usr/share/keyrings
pkg: cp -a %/farcaller.gpg /usr/share/keyrings/hackndev.org-keyring.gpg
