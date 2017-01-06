#!/usr/bin/python

import cgi
import httplib
import plistlib
import string
import sys

if len(sys.argv) != 3:
    print "usage: %s <BuildManifest.plist> <ECID>" % (sys.argv[0],)
    exit(0)

list = {
    '@HostIpAddress': '192.168.1.100',
    '@HostPlatformInfo': 'windows',
    '@Locality': 'en_US',
    '@VersionInfo': '3.8',
    'ApProductionMode': True,
    'ApSecurityDomain': 1
}

ipsw = plistlib.readPlist(sys.argv[1])
for mode in ipsw['BuildIdentities']:
    if mode['Info']['RestoreBehavior'] == 'Erase':
        list['ApBoardID'] = string.atoi(mode['ApBoardID'], 0)
        list['ApChipID'] = string.atoi(mode['ApChipID'], 0)
        fest = mode['Manifest']
        for name in fest:
            file = fest[name]
            if 'PartialDigest' in file:
                list[name] = file

list['ApECID'] = ecid = string.atoi(sys.argv[2], 16)

data = plistlib.writePlistToString(list)

http = httplib.HTTPConnection('gs.apple.com')

http.request('POST', '/TSS/controller?action=2', data, {
    'User-Agent': 'InetURL/1.0',
    'Content-Type': 'text/xml; charset="utf-8"'
})

print http.getresponse().read()
