pkg: mkdir -p /etc/apt/sources.list.d
pkg: cp -a %/ste.list /etc/apt/sources.list.d
pkg: mkdir -p /usr/share/keyrings
pkg: cp -a %/jay /usr/share/keyrings/ste-keyring.gpg
pkg: mkdir -p /Applications/Cydia.app/Sources
pkg: cp -a %/repo.smxy.org.png /Applications/Cydia.app/Sources/repo.smxy.org.png
