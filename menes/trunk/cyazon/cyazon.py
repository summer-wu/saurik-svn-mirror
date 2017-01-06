# Cyazon - Python Amazon FPS Client
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

import base64, hashlib, hmac, urllib, urllib2
from datetime import datetime
from xml.dom import minidom

def undom(dom):
    children = dom.childNodes
    if len(children) == 0:
        return None
    elif len(children) == 1 and children[0].localName == None:
        return children[0].nodeValue
    else:
        node = {}
        for child in dom.childNodes:
            name = child.localName
            if name != None:
                value = undom(child)
                #node.setdefault(name, []).append(value)
                node[name] = value
    return node

def parse(file):
    return undom(minidom.parse(file))

def sign_(set, secret):
    return base64.b64encode(hmac.new(secret, ''.join(
        item[2] + str(item[1]) for item in sorted((
            item[0].lower(), item[1], item[0]
        ) for item in set.items())
    ), hashlib.sha1).digest())

def sign(set, secret):
    return sign_(set, secret)

class AWSError(Exception):
    def __init__(self, code, message):
        self.code = code
        self.message = message

    def __str__(self):
        return str(self.code) + ': ' + str(self.message)

def merge_(params, **args):
    for item in args.items():
        if isinstance(item[1], dict):
            super = item[0]
            for sub in item[1].items():
                params[super + '.' + sub[0]] = sub[1]
        else:
            params[item[0]] = str(item[1])

class connect:
    def __init__(self, access, secret):
        self.access = access
        self.secret = secret

    def sign(self, set):
        return sign_(set, self.secret)

    def pcb(self, ref, limit, next, **args):
        params = {
            'callerKey': self.access,
            'callerReference': ref,
            'globalAmountLimit': limit,
            'pipelineName': 'MultiUse',
            'returnURL': next,
            'version': '2009-01-09',
        }

        merge_(params, **args)

        params['awsSignature'] = sign(params, self.secret)

        return 'https://authorize.payments.amazon.com/cobranded-ui/actions/start?' + urllib.urlencode(params)
        #return 'https://authorize.payments-sandbox.amazon.com/cobranded-ui/actions/start?' + urllib.urlencode(params)

    def fps(self, action, **args):
        params = {
            'Action': action,
            'AWSAccessKeyId': self.access,
            'SignatureVersion': 1,
            'Timestamp': datetime.now().strftime('%Y-%m-%dT%H:%M:%SZ'),
            'Version': '2008-09-17',
        }

        merge_(params, **args)

        params['Signature'] = sign(params, self.secret)

        try:
            #url = urllib2.urlopen('https://fps.sandbox.amazonaws.com/?' + urllib.urlencode(params))
            url = urllib2.urlopen('https://fps.amazonaws.com/?' + urllib.urlencode(params))
        except urllib2.HTTPError, e:
            try:
                dom = parse(e)
            finally:
                e.close()
            err = dom['Response']['Errors']['Error']
            raise AWSError(err['Code'], err['Message'])

        try:
            dom = parse(url)
        finally:
            url.close()

        response = dom[action + 'Response']
        name = action + 'Result'
        return response[name] if name in response else None
