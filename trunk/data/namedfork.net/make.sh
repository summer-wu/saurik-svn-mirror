pkg: mkdir -p /etc/apt/sources.list.d
pkg: cp -a %/namedfork.net.list /etc/apt/sources.list.d
pkg: mkdir -p /usr/share/keyrings
pkg: cp -a %/zydeco.gpg /usr/share/keyrings/namedfork.net-keyring.gpg
pkg: mkdir -p /Applications/Cydia.app/Sources
pkg: cp -a %/namedfork-Icon.png /Applications/Cydia.app/Sources/apt.namedfork.net.png
