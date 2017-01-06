# Bufferin - Schema-less Protocol Buffer Library
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

def envarint(val, pre=None):
    from array import array
    buf = pre if pre != None else array('c')
    while True:
        now = val & 0x7f
        val >>= 7
        if val != 0:
            now |= 0x80
        buf.append(chr(now))
        if val == 0:
            break
    if pre == None:
        return buf.tostring()

def enstring(val, pre=None):
    from array import array
    buf = pre if pre != None else array('c')
    envarint(len(val), buf)
    buf.fromstring(val)
    if pre == None:
        return buf.tostring()

def enbuffer_(key, val, buf):
    if val == None:
        return
    if isinstance(val, int):
        envarint(key << 3 | 0, buf)
        envarint(val, buf)
    elif isinstance(val, str) or isinstance(val, unicode):
        envarint(key << 3 | 2, buf)
        enstring(val, buf)
    elif isinstance(val, tuple):
        if len(val) != 1:
            raise ValueError
        envarint(key << 3 | 2, buf)
        enstring(enbuffer(val[0]), buf)
    elif isinstance(val, dict):
        if val.get(-1, False):
            envarint(key << 3 | 2, buf)
            enstring(enbuffer(val), buf)
        else:
            envarint(key << 3 | 3, buf)
            enbuffer(val, buf)
            envarint(key << 3 | 4, buf)
    else:
        raise ValueError

def enbuffer(dat, pre=None):
    from array import array
    buf = pre if pre != None else array('c')
    for key in sorted(dat.keys()):
        if key != -1:
            set = dat[key]
            if isinstance(set, list):
                for val in set:
                    enbuffer_(key, val, buf)
            else:
                enbuffer_(key, set, buf)

    if pre == None:
        return buf.tostring()

class destream:
    def __init__(self, fd):
        self.fd_ = fd
        self.byte_ = None
        self.offset_ = 0
    def pull(self):
        if self.byte_ != None:
            byte = self.byte_
            self.byte_ = None
            return byte
        else:
            val = self.fd_.read(1)
            if val == None or val == '':
                return None
            self.offset_ += 1
            return ord(val)
    def push(self, byte):
        self.byte_ = byte
    def read(self, size):
        assert self.byte_ == None
        val = self.fd_.read(size)
        assert len(val) == size
        self.offset_ += size
        return val
    def offset(self):
        return self.offset_

class destring:
    def __init__(self, string):
        self.string_ = string
        self.offset_ = 0
    def pull(self):
        if self.offset_ == len(self.string_):
            return None
        val = self.string_[self.offset_]
        self.offset_ += 1
        return ord(val)
    def read(self, size):
        end = self.offset_ + size
        assert end <= len(self.string_)
        val = self.string_[self.offset_:end]
        self.offset_ = end
        return val
    def offset(self):
        return self.offset_

def devarint(rdr, bgn=None):
    val = 0
    sft = 0
    while True:
        if bgn == None:
            nxt = rdr.pull()
        else:
            nxt = bgn
            bgn = None
        val |= (nxt & ~0x80) << sft
        sft += 7
        if nxt & 0x80 == 0:
            break
    return val

def debuffer(rdr, bps=[], off=None):
    if isinstance(rdr, str):
        rdr = destring(rdr)
    elif isinstance(rdr, buffer):
        rdr = destring(rdr)
    elif isinstance(rdr, file):
        rdr = destream(rdr)

    # This line of code is the first real evidence I have found that Python deserves to be known.
    bps = [map(int, bps.split('/')) if isinstance(bps, str) else bps for bps in bps if len(bps) != 0]

    end = rdr.offset() + off if off != None and off != -1 else None
    buf = {}

    while True:
        if end == None:
            nxt = rdr.pull()
            if nxt == None:
                break
        else:
            if rdr.offset() == end:
                break
            nxt = None

        key = devarint(rdr, nxt)
        assert key != 0

        fld = key >> 3
        typ = key & 0x7

        if off == -1 and typ == 4:
            break

        sps = [pth[1:] for pth in bps if pth[0] == fld]
        hit = [] in sps

        if typ == 0:
            val = devarint(rdr)
        elif typ == 1:
            raise NotImplementedError
        elif typ == 2:
            siz = devarint(rdr)
            if hit:
                val = debuffer(rdr, sps, siz)
                val[-1] = True
            else:
                val = rdr.read(siz)
        elif typ == 3:
            val = debuffer(rdr, sps, -1)
        elif typ == 5:
            raise NotImplementedError
        else:
            raise ValueError

        buf.setdefault(fld, []).append(val)

    return buf

def bufferin(val):
    if isinstance(val, buffer) or isinstance(val, file) or isinstance(val, str):
        return debuffer(val)
    else:
        return enbuffer(val)
    
