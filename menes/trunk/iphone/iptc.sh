#!/bin/bash

# iPhone Toolchain Compiler - Automatic Generation Script
# Copyright (C) 2009  Jay Freeman (saurik)

#        Redistribution and use in source and binary
# forms, with or without modification, are permitted
# provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the
#    above copyright notice, this list of conditions
#    and the following disclaimer.
# 2. Redistributions in binary form must reproduce the
#    above copyright notice, this list of conditions
#    and the following disclaimer in the documentation
#    and/or other materials provided with the
#    distribution.
# 3. The name of the author may not be used to endorse
#    or promote products derived from this software
#    without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS''
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
# BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
# TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

set -e

shopt -s extglob
shopt -s nullglob

base=$(pwd)

miss=()

if ! which git &>/dev/null; then
    miss[${#miss[@]}]=git-core
fi

if ! which xar &>/dev/null; then
    miss[${#miss[@]}]=xar
fi

if ! which xml2 &>/dev/null; then
    miss[${#miss[@]}]=xml2
fi

if [[ ${#miss[@]} -ne 0 ]]; then
    echo -n 'please install:'
    for pkg in "${miss[@]}"; do
        echo -n " ${pkg}"
    done
    exit 1
fi 1>&2

if [[ $# -ne 2 ]]; then
    echo "usage: ./iptc.sh <adc-username> <adc-password>"
    exit 1
fi

adcu=$1
adcp=$2

mkdir -p bin src

cd src

if [[ ! -e odcctools-9.2-ld ]]; then
    svn co http://iphone-dev.googlecode.com/svn/branches/odcctools-9.2-ld
fi

if [[ ! -e llvm-gcc-4.2 ]]; then
    git clone git://git.saurik.com/llvm-gcc-4.2
fi

if [[ ! -e xpwn ]]; then
    git clone git://github.com/planetbeing/xpwn
fi

if [[ ! -e img3decrypt ]]; then
    svn co http://img3decrypt.googlecode.com/svn/trunk img3decrypt
fi

cd ..

agent="Mozilla/5.0 (iPhone; U; CPU iPhone OS 2_2 like Mac OS X; en-us) AppleWebKit/525.18.1 (KHTML, like Gecko) Version/3.1.1 Mobile/5G77 Safari/525.20"

alias wget='wget --user-agent="'"${agent}"'"'
cookies=(--cookies=on --keep-session-cookies)

key=

function login() {
    if [[ $1 == ${key} ]]; then
        echo "logged into ${key}"
        return
    else
        key=$1
        echo "logging into ${key}"
    fi

    action=$(wget -qO- 'https://daw.apple.com/cgi-bin/WebObjects/DSAuthWeb.woa/wa/login?appIdKey='"${key}" | html2 | grep '^[^=]*/@action=' | cut -d '=' -f 2) && wget -O/dev/null "${cookies[@]}" --save-cookies="${base}"/cookies.txt "https://daw.apple.com${action}" --post-data='theAccountName='"${adcu}"'&theAccountPW='"${adcp}"
}

mkdir -p apl
cd apl

for fil in apsl/cctools-667.8.0 apsl/CF-476.14 apsl/configd-210 apsl/DirectoryService-514.23 apsl/DiskArbitration-183 apsl/IOCDStorageFamily-39 apsl/IODVDStorageFamily-26 apsl/IOGraphics-193.2 apsl/IOHIDFamily-258.3 apsl/IOKitUser-388 apsl/IOKitUser-388.2.1 apsl/IOStorageFamily-88 other/JavaScriptCore-466.1 apsl/launchd-258.1 apsl/Libc-498 apsl/libsecurity_authorization-32564 apsl/libsecurity_cdsa_client-32432 apsl/libsecurity_cdsa_utilities-33506 apsl/libsecurity_cms-32521 apsl/libsecurity_codesigning-33803 apsl/libsecurity_cssm-32993 apsl/libsecurityd-33470 apsl/libsecurity_keychain-34101 apsl/libsecurity_mds-32820 apsl/libsecurity_ssl-32463 apsl/libsecurity_utilities-32820 other/WebCore-351.9 apsl/xnu-1228.3.13 apsl/xnu-1228.7.58; do
    dir=${fil#*/}
    tgz=${dir}.tar.gz

    if [[ ! -e "${tgz}" ]]; then
        login D236F0C410E985A7BB866A960326865E7F924EB042FA9A161F6A628F0291F620
        wget -c --cookies=on --load-cookies=../cookies.txt "http://www.opensource.apple.com/darwinsource/tarballs/${fil}.tar.gz"
    fi

    if [[ ! -e ${dir} ]]; then
        tar -zxvf "${tgz}"
    fi
done

cd ..

mach=iPhone1,1

if false; then
    args=($(
        wget -qO- http://phobos.apple.com/version | gunzip -9c | sed -e '
            /http:\/\/.*\/iPhone\/.*\/'"${mach}"'_2.*_Restore\.ipsw/ {
                s/^.\W*<string>//
                s/<\/string>\r$//
                s/^.*\/'"${mach}"'_\([^_]*\)_\([^_]*\).*$/\1 \2 \0/
                p
            }; d
        ' | sort -runk 1,1 | while IFS= read -r line; do
            args=(${line})
            ipfv=${args[0]}
            ipbv=${args[1]}
            ipcu=${args[2]}
            ipfb=src/xpwn/ipsw-patch/FirmwareBundles/${mach}_${ipfv}_${ipbv}.bundle
            if [[ -e ${ipfb} ]]; then
                echo "${line}"
                break
            fi
        done
    ))

    ipfv=${args[0]}
    ipbv=${args[1]}
    ipcu=${args[2]}
else
    ipfv=2.2
    ipbv=5G77
    ipcu=http://appldnld.apple.com.edgesuite.net/content.info.apple.com/iPhone/061-5779.20081120.Pt5yH/iPhone1,1_2.2_5G77_Restore.ipsw
fi

ipfb=src/xpwn/ipsw-patch/FirmwareBundles/${mach}_${ipfv}_${ipbv}.bundle
info=${ipfb}/Info.plist

ipsw=${mach}-${ipfv}.ipsw
if [[ ! -e "${ipsw}" ]]; then
    wget -cO "${ipsw}" "${ipcu}"
fi

sdkd=iPhoneSDK-${ipfv}.dmg
if [[ ! -e ${sdkd} ]]; then
    login D635F5C417E087A3B9864DAC5D25920C4E9442C9339FA9277951628F0291F620

    wget -O/dev/null "${cookies[@]}" --load-cookies=cookies.txt --save-cookies=cookies.txt --referer='http://developer.apple.com/iphone/index.action' 'http://developer.apple.com/iphone/download.action?path=/iphone/iphone_sdk_for_iphone_os_2.2__9m2621__final/iphone_sdk_for_iphone_os_2.2_9m2621_final.dmg'
    wget -O "${sdkd}" "${cookies[@]}" --load-cookies=cookies.txt --referer='http://developer.apple.com/iphone/index.action' 'http://developer.apple.com/iphone/download.action?path=/iphone/iphone_sdk_for_iphone_os_2.2__9m2621__final/iphone_sdk_for_iphone_os_2.2_9m2621_final.dmg'
fi

if [[ ! -e bin/dmg ]]; then
    gcc -DHAVE_CRYPT -o bin/dmg src/xpwn/{common/*,dmg/!(win32test),hfs/!(hfs)}.c -Isrc/xpwn/includes -lz -lssl
fi

if [[ ! -e bin/img3decrypt ]]; then
    gcc -o bin/img3decrypt src/img3decrypt/imagetool.c -Isrc/xpwn/includes src/xpwn/{common/*,ipsw-patch/{img3,libxpwn}}.c -lssl
fi

if [[ ! -e bin/hfs ]]; then
    gcc -o bin/hfs src/xpwn/{common,hfs}/*.c -Isrc/xpwn/includes
fi

sdki=iPhoneSDK-${ipfv}.img
if [[ ! -e ${sdki} ]]; then
    bin/dmg extract "${sdkd}" "${sdki}"
fi

sdkp=iPhoneSDK-${ipfv}.pkg
if [[ ! -e ${sdkp} ]]; then
    bin/hfs "${sdki}" extract Packages/iPhoneSDKHeadersAndLibs.pkg "${sdkp}"
fi

sdkh=iPhoneSDK-${ipfv}
if [[ ! -e ${sdkh} ]]; then
    xar -xf "${sdkp}" Payload
    mkdir "${sdkh}"
    cd "${sdkh}"
    zcat ../Payload | cpio -id
    cd ..
    rm -f Payload
fi

xcsp=MacSDK-10.5.pkg
if [[ ! -e ${xcsp} ]]; then
    bin/hfs "${sdki}" extract Packages/MacOSX10.5.pkg "${xcsp}"
fi

xcsh=MacSDK-10.5
if [[ ! -e ${xcsh} ]]; then
    xar -xf "${xcsp}" Payload
    mkdir "${xcsh}"
    cd "${xcsh}"
    zcat ../Payload | cpio -id
    cd ..
    rm -f Payload
fi

if [[ ! -e afw ]]; then
    unzip -d afw "${ipsw}" || rm -rf afw
fi

if false; then
    eval "$(grep -A 20 '<key>Restore Ramdisk</key>' "${info}" | tr -d $'\n' | sed -e '
        s/<\/dict>.*//
        s/.*<dict>//
        s/[\t ]*<key>//g
        s/<\/key>[\t ]*<string>/=/g
        s/<\/key>[\t ]*<integer>/=/g
        s/<\/string>/\n/g
        s/<\/integer>/\n/g
    ')"

    rddi=${File}
    rdiv=${IV}
    rdek=${Key}

    bin/img3decrypt e afw/"${rfdi}" "${fimg}" "${rfiv}" "${rfek}"
fi

eval "$(grep RootFilesystem -A1 "${info}" | tr -d $'\n' | sed -e '
    s/<\/dict>.*//
    s/.*<dict>//
    s/[\t ]*<key>//g
    s/<\/key>[\t ]*<string>/=/g
    s/<\/key>[\t ]*<integer>/=/g
    s/<\/string>/\n/g
    s/<\/integer>/\n/g
')"

rfdi="${RootFilesystem}"
rfck="${RootFilesystemKey}"

fimg=${mach}-${ipfv}.img

if [[ ! -e "${fimg}" ]]; then
    bin/dmg extract afw/"${rfdi}" "${fimg}" -k "${rfck}"
fi

if [[ ! -e sys ]]; then
    mkdir sys
    cd sys
    ../bin/hfs ../"${fimg}" extractall 
    cd ..
fi

rm -rf sys/usr/include
mkdir sys/usr/include

export leopardsdk=${base}/MacSDK-10.5/SDKs/MacOSX10.5.sdk
export leopardinc=${leopardsdk}/usr/include
export leopardlib=${leopardsdk}/System/Library/Frameworks

export iphoneplt=${base}/iPhoneSDK-${ipfv}/Platforms/iPhoneOS.platform
export iphonesdk=$(echo "${iphoneplt}"/Developer/SDKs/*.sdk | sort -nr | head -n 1)
export iphoneinc=${iphonesdk}/usr/include
export iphonelib=${iphonesdk}/System/Library/Frameworks

export apple=${base}/apl

cp -a "${leopardinc}"/* sys/usr/include
cd sys/usr/include
ln -s . System

for sec in security_{cdsa_client,cdsa_utilities,codesigning,keychain,utilities} securityd_client; do
    ln -s Security  "${sec}"
done

cp -af "${iphoneinc}"/* .
cp -af "${apple}"/xnu-1228.7.58/osfmk/* .
cp -af "${apple}"/xnu-1228.7.58/bsd/* .

cp -af "${apple}"/cctools-667.8.0/include/mach .
cp -af "${apple}"/cctools-667.8.0/include/mach-o .
cp -af "${iphoneinc}"/mach-o/dyld.h mach-o

cp -af "${leopardinc}"/mach/machine mach
cp -af "${leopardinc}"/mach/machine.h mach
cp -af "${leopardinc}"/machine .
cp -af "${iphoneinc}"/machine .

cp -af "${iphoneinc}"/sys/cdefs.h sys
cp -af "${leopardinc}"/sys/dtrace.h sys

cp -af "${leopardlib}"/Kernel.framework/Headers/machine/disklabel.h machine
cp -af "${apple}"/configd-210/dnsinfo/dnsinfo.h .
cp -a "${apple}"/Libc-498/include/kvm.h .
cp -a "${apple}"/launchd-258.1/launchd/src/*.h .

cp -a i386/disklabel.h arm
cp -a mach/i386/machine_types.defs mach/arm

if which mig &>/dev/null; then
    # if you don't have mig, just ignore this for now
    for defs in clock_reply exc mach_exc notify; do
        mig -server /dev/null -user /dev/null -header /dev/null \
            -sheader mach/"${defs}"_server.h mach/"${defs}".defs
    done
fi

find . \( -name '*.c' -o -name '*.s' \) -exec rm -f {} \;

mkdir Kernel
cp -a "${apple}"/xnu-1228.3.13/libsa/libsa Kernel

mkdir Security
cp -a "${apple}"/libsecurity_authorization-32564/lib/*.h Security
cp -a "${apple}"/libsecurity_cdsa_client-32432/lib/*.h Security
cp -a "${apple}"/libsecurity_cdsa_utilities-33506/lib/*.h Security
cp -a "${apple}"/libsecurity_cms-32521/lib/*.h Security
cp -a "${apple}"/libsecurity_codesigning-33803/lib/*.h Security
cp -a "${apple}"/libsecurity_cssm-32993/lib/*.h Security
cp -a "${apple}"/libsecurity_keychain-34101/lib/*.h Security
cp -a "${apple}"/libsecurity_mds-32820/lib/*.h Security
cp -a "${apple}"/libsecurity_ssl-32463/lib/*.h Security
cp -a "${apple}"/libsecurity_utilities-32820/lib/*.h Security
cp -a "${apple}"/libsecurityd-33470/lib/*.h Security

mkdir DiskArbitration
cp -a "${apple}"/DiskArbitration-183/DiskArbitration/*.h DiskArbitration

cp -a "${apple}"/xnu-1228.3.13/iokit/IOKit .
cp -a "${apple}"/IOKitUser-388.2.1/*.h IOKit

cp -a "${apple}"/IOGraphics-193.2/IOGraphicsFamily/IOKit/graphics IOKit
cp -a "${apple}"/IOHIDFamily-258.3/IOHIDSystem/IOKit/hidsystem IOKit

for proj in kext ps pwr_mgt; do
    mkdir -p IOKit/"${proj}"
    cp -a "${apple}"/IOKitUser-388/"${proj}".subproj/*.h IOKit/"${proj}"
done

ln -s IOKit/kext/bootfiles.h .

mkdir IOKit/storage
cp -a "${apple}"/IOStorageFamily-88/*.h IOKit/storage
cp -a "${apple}"/IOCDStorageFamily-39/*.h IOKit/storage
cp -a "${apple}"/IODVDStorageFamily-26/*.h IOKit/storage

mkdir DirectoryService
cp -a "${apple}"/DirectoryService-514.23/APIFramework/*.h DirectoryService

mkdir DirectoryServiceCore
cp -a "${apple}"/DirectoryService-514.23/CoreFramework/Private/*.h DirectoryServiceCore
cp -a "${apple}"/DirectoryService-514.23/CoreFramework/Public/*.h DirectoryServiceCore

mkdir SystemConfiguration
cp -a "${apple}"/configd-210/SystemConfiguration.fproj/*.h SystemConfiguration

mkdir WebCore
cp -a "${apple}"/WebCore-351.9/bindings/objc/*.h WebCore
cp -a "${apple}"/WebCore-351.9/bridge/mac/*.h WebCore

cp -aH "${leopardlib}"/CoreFoundation.framework/Headers CoreFoundation
cp -af "${apple}"/CF-476.14/*.h CoreFoundation
cp -af "${iphonelib}"/CoreFoundation.framework/Headers/* CoreFoundation

for framework in AudioToolbox AudioUnit CoreAudio Foundation; do
    cp -aH "${leopardlib}"/"${framework}".framework/Headers "${framework}"
    cp -af "${iphonelib}"/"${framework}".framework/Headers/* "${framework}"
done

for framework in AppKit Cocoa CoreData CoreVideo JavaScriptCore OpenGL QuartzCore WebKit; do
    cp -aH "${leopardlib}"/"${framework}".framework/Headers "$(basename "${framework}" .framework)"
done

for framework in AddressBook; do
    cp -aH "${iphonelib}"/"${framework}".framework/Headers "$(basename "${framework}" .framework)"
done

cp -aH "${leopardlib}"/ApplicationServices.framework/Headers ApplicationServices
for service in "${leopardlib}"/ApplicationServices.framework/Frameworks/*.framework; do
    cp -aH "${service}"/Headers "$(basename "${service}" .framework)"
done

cp -aH "${leopardlib}"/CoreServices.framework/Headers CoreServices
for service in "${leopardlib}"/CoreServices.framework/Frameworks/*.framework; do
    cp -aH "${service}"/Headers "$(basename "${service}" .framework)"
done

for subdir in css dom editing history html loader page platform{,/{graphics,text}} rendering; do
    cp -a "${apple}"/WebCore-351.9/"${subdir}"/*.h WebCore
done

cp -a "${apple}"/WebCore-351.9/css/CSSPropertyNames.in WebCore
(cd WebCore; perl "${apple}"/WebCore-351.9/css/makeprop.pl)

mkdir kjs
cp -a "${apple}"/JavaScriptCore-466.1/kjs/*.h kjs

mkdir -p wtf/unicode/icu
cp -a "${apple}"/JavaScriptCore-466.1/wtf/*.h wtf
cp -a "${apple}"/JavaScriptCore-466.1/wtf/unicode/*.h wtf/unicode
cp -a "${apple}"/JavaScriptCore-466.1/wtf/unicode/icu/*.h wtf/unicode/icu

mkdir unicode
cp -a "${apple}"/JavaScriptCore-466.1/icu/unicode/*.h unicode

rm -rf apache2 apr-1 cups curl gssapi kerberosIV krb5 libexslt libxml2 libxslt openssl php readline subversion-1 uuid wx-2.8 xar
rm -f db.h com_err.h expat*.h fastcgi.h fcgi*.h FlexLexer.h gssapi.h krb5.h profile.h python2.5 readline sqlite3*.h tk*.h tcl*.h X11
rm -f curses.h eti.h form.h menu.h ncurses* panel.h termcap.h term.h unctrl.h
#rm -f mach-o/arch.h mach-o/fat.h mach-o/loader.h

rm -f c++/4.0.0/arm-apple-darwin9
cp -a c++/4.0.0/arm-apple-darwin{8,9}
#arm-apple-darwin8/bits/c++config.h

wget -qO- http://svn.telesphoreo.org/trunk/tool/include.diff | patch -p3 -r/dev/null || true
rm -f CoreFoundation/CoreFoundation.h.orig mach/arm/thread_status.h.orig

wget -qO arm/locks.h http://svn.telesphoreo.org/trunk/tool/patches/locks.h
