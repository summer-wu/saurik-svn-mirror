#!/bin/bash
arm-apple-darwin-gcc -o _uicaboodle.dylib uicaboodle.m -I/home/saurik/iphone/sysroot/usr/include/python2.5 -I/dat/src/pyobjc/pyobjc/pyobjc-core/Modules/objc -lpython2.5 -framework UIKit -dynamiclib -framework Foundation
