/* Oscar - Multi-Threaded Implementation of OSCAR
 * Copyright (C) 2002-2005  Jay Freeman (saurik)
*/

/*
 *        Redistribution and use in source and binary
 * forms, with or without modification, are permitted
 * provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the
 *    above copyright notice, this list of conditions
 *    and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the
 *    above copyright notice, this list of conditions
 *    and the following disclaimer in the documentation
 *    and/or other materials provided with the
 *    distribution.
 * 3. The name of the author may not be used to endorse
 *    or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef MENES_NET_OSCAR_FLAP_HPP
#define MENES_NET_OSCAR_FLAP_HPP

#include "cxx/platform.hpp"
#include "net/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "api/locks.hpp"
#include "api/socket.hpp"

#include "ext/listset.hpp"
#include "ext/map.hpp"
#include "ext/redblackset.hpp"

#include "ios/buffer.hpp"
#include "ios/getput.hpp"

#include "net/oscar/capability.hpp"

#include <list>
#include <queue>

namespace net {
namespace Oscar {

class Session;

class LvTuple {
  protected:
    _S<ios::Buffer> data_;

  public:
    LvTuple() {
    }

    LvTuple(ios::Reader &stream) :
        data_(stream.GetIterator(), ios::GetBig<uint16_t>(stream))
    {
    }

    LvTuple(const char *data, uint16_t length) :
        data_(data, length)
    {
    }

    operator cse::String() const {
        return data_;
    }

    operator _S<ios::Buffer>() const {
        return data_;
    }

    operator uint32_t() const {
        uint32_t value;
        for (ios::Buffer::ConstIterator byte(data_.Begin()); byte != data_.End(); ++byte)
            value *= 256 + *byte;
        return value;
    }

    operator Capabilities() const {
        // XXX: this is broken (duh)
        return Capabilities();
    }

    bool IsEmpty() const {
        return data_.IsEmpty();
    }

    void WriteTo(ios::Writer &stream) const {
        ios::PutBig<uint16_t>(stream, data_.GetSize());
        data_.WriteTo(stream);
    }

    LvTuple &operator =(uint16_t rhs) {
        data_.Clear();
        ios::PutBig(data_, rhs);
        return *this;
    }

    LvTuple &operator =(uint32_t rhs) {
        data_.Clear();
        ios::PutBig(data_, rhs);
        return *this;
    }

    LvTuple &operator =(const Capabilities &rhs) {
        data_.Clear();
        for (Capabilities::ConstIterator capability(rhs.Begin()); capability != rhs.End(); ++capability)
            // XXX: this might need to take into account endian-ness... no?
            data_.Write(reinterpret_cast<const char *>(capability->GetData()), sizeof(Capability));
        return *this;
    }

    LvTuple &operator =(const ext::Buffer &rhs) {
        data_ = rhs;
        return *this;
    }

    void Clear() {
        data_.Clear();
    }
};

class TlvParser {
    friend ios::PrintWriter &operator <<(ios::PrintWriter &os, const TlvParser &tlv);

  private:
    typedef ext::RedBlackMap<uint16_t, LvTuple> LvMap_;

  public:
    typedef LvMap_::Iterator Iterator;
    typedef LvMap_::ConstIterator ConstIterator;

  private:
    LvMap_ lvs_;

  public:
    TlvParser() {
    }

    TlvParser(const LvTuple &lv) {
        // XXX: this makes me sad
        _S<ios::Buffer> segment(lv.operator _S<ios::Buffer>());
        ReadFrom(segment);
    }

    TlvParser(ios::Buffer &segment) {
        ReadFrom(segment);
    }

    TlvParser(ios::Reader &stream, uint16_t number) {
        ReadFrom(stream, number);
    }

    void ReadFrom(ios::Reader &stream, uint16_t number) {
        Clear();

        for (uint16_t i(0); i != number; ++i) {
            uint16_t type(ios::GetBig<uint16_t>(stream));
            bool inserted(lvs_.Insert(type, LvTuple(stream)).Second());
            _assert(inserted);
        }
    }

    void ReadFrom(ios::Buffer &segment) {
        Clear();

        while (!segment.IsEmpty()) {
            uint16_t type(ios::GetBig<uint16_t>(segment));
            bool inserted(lvs_.Insert(type, LvTuple(segment)).Second());
            _assert(inserted);
        }
    }

    void WriteTo(ios::Writer &stream) const {
        for (LvMap_::ConstIterator lv(lvs_.Begin()); lv != lvs_.End(); ++lv) {
            ios::PutBig(stream, lv->First());
            lv->Second().WriteTo(stream);
        }
    }

    const LvTuple &operator [](uint16_t type) const {
        return lvs_[type];
    }

    LvTuple &operator [](uint16_t type) {
        return lvs_[type];
    }

    Iterator Begin() {
        return lvs_.Begin();
    }

    Iterator End() {
        return lvs_.End();
    }

    void Clear() {
        lvs_.Clear();
    }

    operator _S<ios::Buffer>() const {
        _S<ios::Buffer> value;
        WriteTo(value);
        return value;
    }
};

ios::PrintWriter &operator <<(ios::PrintWriter &os, const TlvParser &tlv);

struct SnacHeader {
    uint16_t family;
    uint16_t subtype;
    uint16_t flags;
    uint32_t request;

    static uint32_t lastrequest_;
    static const size_t Size = sizeof(uint16_t) * 3 + sizeof(uint32_t);

    SnacHeader() :
        family(0),
        subtype(0),
        flags(0),
        request(++lastrequest_)
    {
    }

    SnacHeader(uint16_t family, uint16_t subtype) :
        family(family),
        subtype(subtype),
        flags(0),
        request(lastrequest_++)
    {
    }

    SnacHeader(uint16_t family, uint16_t subtype, uint32_t request) :
        family(family),
        subtype(subtype),
        flags(0),
        request(request)
    {
    }

    SnacHeader(ios::Reader &stream) :
        family(ios::GetBig<uint16_t>(stream)),
        subtype(ios::GetBig<uint16_t>(stream)),
        flags(ios::GetBig<uint16_t>(stream)),
        request(ios::GetBig<uint32_t>(stream))
    {
    }

    void WriteTo(ios::Writer &stream) const {
        ios::PutBig<uint16_t>(stream, family);
        ios::PutBig<uint16_t>(stream, subtype);
        ios::PutBig<uint16_t>(stream, flags);
        ios::PutBig<uint32_t>(stream, request);
    }
};

struct SnacPacket {
    SnacHeader snac;
    _S<ios::Buffer> data;

    SnacPacket() {
    }

    SnacPacket(const SnacHeader &snac, const ios::Buffer &data = _S<ios::Buffer>()) :
        snac(snac),
        data(data)
    {
    }

    SnacPacket(uint16_t family, uint16_t subtype, const ios::Buffer &data = _S<ios::Buffer>()) :
        snac(family, subtype),
        data(data)
    {
    }

    SnacPacket(ios::Buffer &packet) :
        snac(packet),
        data(packet)
    {
    }
};

typedef ext::ListSet<uint16_t> FamilySet;

struct FlapHeader {
    uint8_t start_;
    uint8_t channel_;
    uint16_t sequence_;
    uint16_t length_;

    FlapHeader() :
        start_(0x2a)
    {
    }

    FlapHeader(uint8_t channel, uint16_t sequence, uint16_t length) :
        start_(0x2a),
        channel_(channel),
        sequence_(sequence),
        length_(length)
    {
    }

    FlapHeader(ios::Reader &stream) {
        Read(stream);
    }

    void Read(ios::Reader &stream) {
        start_ = ios::GetBig<uint8_t>(stream);
        _assert(start_ == 0x2a);
        channel_ = ios::GetBig<uint8_t>(stream);
        sequence_ = ios::GetBig<uint16_t>(stream);
        length_ = ios::GetBig<uint16_t>(stream);
    }

    void WriteTo(ios::Writer &stream) {
        ios::PutBig<uint8_t>(stream, start_);
        ios::PutBig<uint8_t>(stream, channel_);
        ios::PutBig<uint16_t>(stream, sequence_);
        ios::PutBig<uint16_t>(stream, length_);
    }
};

class FlapStream {
  public:
    static const uint32_t Family = 0x0001;

  protected:
    _S<api::TcpSocket> socket_;

    uint16_t sequence_;
    Session &session_;

    cse::String address_;
    _S<ios::Buffer> cookie_;

    struct RateClass_ {
        uint32_t windowsize;
        uint32_t clear;
        uint32_t alert;
        uint32_t limit;
        uint32_t disconnect;
        uint32_t current;
        uint32_t max;
        char unknown[5];
        //struct snacpair *members;
    };

    typedef ext::ListMap<uint16_t, RateClass_> RateMap_;
    RateMap_ rates_;

    FamilySet families_;

    api::ThreadMutex lock_;

    void WriteHeader(uint8_t channel, uint16_t length) {
        FlapHeader(channel, sequence_++, length).WriteTo(socket_);
    }

    uint8_t Read(ios::Buffer &segment) {
        FlapHeader header(socket_);
        segment.Assign(socket_.GetIterator(), header.length_);
        return header.channel_;
    }

  public:
    FlapStream(Session &session, const cse::String &address, const ios::Buffer &cookie);

    const FamilySet &GetFamilies() const;
    const cse::String &GetAddress() const;
    void Handle();

    void Write(uint8_t channel, ios::Buffer &segment) {
        ext::ScopeLock<api::ThreadMutex> guard_(lock_);
        WriteHeader(channel, segment.GetSize());
        segment.WriteTo(socket_);
        socket_.Flush();
    }

    // XXX: this should check for socket error and return appropriately
    bool Write(const SnacPacket &packet) {
        ext::ScopeLock<api::ThreadMutex> guard_(lock_);
        uint16_t length(static_cast<uint16_t>(packet.data.GetSize()));
        WriteHeader(2, SnacHeader::Size + length);
        packet.snac.WriteTo(socket_);
        socket_.Write(packet.data.Begin(), length);
        socket_.Flush();
        return true;
    }
};

} }

#endif//MENES_NET_OSCAR_FLAP_HPP
