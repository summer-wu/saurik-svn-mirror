iPhone := 192.168.1.100

CC := gcc

all: atpkg atsrc

test: atpkg

clean:
	rm -f atpkg atsrc

%: %.m makefile
	$(CC) -std=c99 -Wall -Werror -o $@ $< -framework Foundation -framework CoreFoundation -lobjc -F$${PKG_ROOT}/Applications/Installer.app -framework AppTapp -framework UIKit
	chmod +s $@

.PHONY: all clean test
