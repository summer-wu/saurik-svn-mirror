all: Pamphlet

Pamphlet: *.mm makefile
	arm-apple-darwin-g++ -fobjc-call-cxx-cdtors -g3 -O2 -Wall -Werror -o $@ $< -framework UIKit -framework IOKit -lobjc -fobjc-exceptions -framework CoreFoundation
