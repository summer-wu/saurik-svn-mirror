pkg: mkdir -p /etc/apt/sources.list.d
pkg: cp -a %/hvkls.dyndns.org.list /etc/apt/sources.list.d
pkg: mkdir -p /usr/share/keyrings
pkg: cp -a %/hvkls_public_gpg.asc /usr/share/keyrings/hvkls.dyndns.org-keyring.gpg
pkg: mkdir -p /Applications/Cydia.app/Sources
pkg: cp -a %/sun.png /Applications/Cydia.app/Sources/hvkls.dyndns.org.png
