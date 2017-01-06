pkg: mkdir -p /etc/apt/sources.list.d
pkg: cp -a %/pwncenter.com.list /etc/apt/sources.list.d
pkg: mkdir -p /Applications/Cydia.app/Sources
pkg: cp -a %/apt.pwncenter.com.png /Applications/Cydia.app/Sources/apt.pwncenter.com.png
