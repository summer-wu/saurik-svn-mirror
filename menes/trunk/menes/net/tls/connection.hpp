/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2005  Jay Freeman (saurik)
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

#ifndef MENES_NET_TLS_CONNECTION_HPP
#define MENES_NET_TLS_CONNECTION_HPP

#include "cxx/platform.hpp"
#include "net/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "api/random.hpp"
#include "ios/holder.hpp"
#include "mfa/pipe.hpp"

#include <ctime>

namespace net {
namespace Tls {

class Connection :
    public ios::ReaderHolder,
    public ios::WriterHolder
{
  private:
    struct Stream_ {
        _R<ios::Reader> reader_;
        _R<ios::Writer> writer_;

        Stream_() {
        }

        Stream_(const _R<ios::Reader> &reader, const _R<ios::Writer> &writer) :
            reader_(reader),
            writer_(writer)
        {
        }
    };

    Stream_ tlsdata_;
    Stream_ appdata_;

    struct Protocol_ {
        _R<api::Fiber> fiber_;

        _R<ios::Reader> reader_;
        _R<ios::Writer> incoming_;

        _R<ios::Writer> writer_;
    };

    _S<api::Fiber> record_;

    _S<api::Fiber> change_cipher_spec_;
    _S<api::Fiber> alert_;
    _S<api::Fiber> handshake_;

    typedef ext::Array<3, Protocol_> Protocols_;
    Protocols_ protocols_;

    int Record_() {
        ios::Reader &reader(*tlsdata_.reader_);

        _foreach (Protocols_, protocol, protocols_) {
            mfa::Pipe pipe(protocol->fiber_);
            protocol->reader_ = pipe.GetReader();
            protocol->incoming_ = pipe.GetWriter();
        }

        _forever {
            uint8_t content_type(ios::Get(reader));
            _assert(content_type >= 20 && content_type <= 23);
            _assert(!protocols_[content_type - 20].incoming_.IsEmpty());

            uint8_t protocol_version_major(ios::Get(reader));
            uint8_t protocol_version_minor(ios::Get(reader));

            _assert(protocol_version_major == 3);
            _assert(protocol_version_minor == 1);

            uint16_t length(ios::GetBig<uint16_t>(reader));

            ios::ReadToWriteFully(
                reader,
                content_type == 23 ?
                    *appdata_.writer_ :
                    *protocols_[content_type - 20].incoming_,
                length
            );
        }

        return 0;
    }

    int ChangeCipherSpec_() {
        Protocol_ &protocol(protocols_[0]);
        return 0;
    }

    int Alert_() {
        Protocol_ &protocol(protocols_[1]);
        return 0;
    }

    int Handshake_() {
        Protocol_ &protocol(protocols_[2]);

        ios::Reader &reader(*protocol.reader_);
        ios::Writer &writer(*protocol.writer_);

        uint8_t type(ios::Get(reader));
        _assert(type == 1);

        uint32_t length(ios::GetBig<uint32_t>(reader, 3));

        uint8_t protocol_version_major(ios::Get(reader));
        uint8_t protocol_version_minor(ios::Get(reader));

        _assert(protocol_version_major == 3);
        _assert(protocol_version_minor == 1);

        uint32_t random_gmt_unix_time(ios::GetBig<uint32_t>(reader));

        _S<ios::Buffer> random_random_bytes;
        random_random_bytes.Reserve(28);
        ios::ReadToWriteFully(reader, random_random_bytes, 28);

        length -= 34;

        _S<ios::Buffer> session_id;

        {
            uint8_t session_id_size(ios::Get(reader));
            _assert(session_id_size <= 32);
            length -= 1 + session_id_size;

            session_id.Reserve(session_id_size);
            ios::ReadToWriteFully(reader, session_id, session_id_size);
        }

        uint16_t cipher_suite(0x0000);

        {
            uint16_t cipher_suites_size(ios::GetBig<uint16_t>(reader));
            _assert(cipher_suites_size >= 2);
            length -= 2 + cipher_suites_size;

            _assert(cipher_suites_size % 2 == 0);
            _forall (size_t, i, 0, cipher_suites_size / 2) {
                uint16_t suite(ios::GetBig<uint16_t>(reader));
                cipher_suites_size -= 2;

                if (suite == 0x000a) {
                    cipher_suite = suite;
                    goto selected_cipher_suite;
                }
            }

            _assert(false);

          selected_cipher_suite:
            ios::DiscardFully(reader, cipher_suites_size);
        }

        _assert(cipher_suite != 0x0000);

        uint8_t compression_method(0x00);

        {
            uint8_t compression_methods_size(ios::Get(reader));
            _assert(compression_methods_size >= 1);
            length -= 1 + compression_methods_size;

            _forall (size_t, i, 0, compression_methods_size) {
                uint8_t method(ios::Get(reader));
                compression_methods_size -= 1;

                if (method == 0x00) {
                    compression_method = method;
                    goto selected_compression_method;
                }
            }

            _assert(false);

          selected_compression_method:
            ios::DiscardFully(reader, compression_methods_size);
        }

        if (length != 0) {
            uint16_t extension_list_size(ios::GetBig<uint16_t>(reader));
            length -= 2 + extension_list_size;

            while (extension_list_size != 0) {
                uint16_t extension_type(ios::GetBig<uint16_t>(reader));
                uint16_t extension_data_size(ios::GetBig<uint16_t>(reader));
                ios::DiscardFully(reader, extension_data_size);

                extension_list_size -= 4 + extension_data_size;
                length -= 4 + extension_data_size;
            }
        }

        _assert(length == 0);

        { // ServerHello
            ios::Put(writer, 2);
            ios::PutBig<uint32_t>(writer, 38, 3);

            ios::Put(writer, 3);
            ios::Put(writer, 1);

            ios::PutBig<uint32_t>(writer, ::time(NULL));
            ios::ReadToWriteFully(api::WeakRandom, writer, 28);

            ios::Put(writer, 0);

            ios::PutBig<uint16_t>(writer, cipher_suite);
            ios::Put(writer, compression_method);
        }

        { // ServerHelloDone
            ios::Put(writer, 14);
            ios::PutBig<uint16_t>(writer, 0, 3);
        }

        writer.Flush();

        return 0;
    }

  public:
    Connection(const _R<ios::Reader> &reader, const _R<ios::Writer> &writer) :
        tlsdata_(reader, writer),
        record_(hop::BindAll(&Connection::Record_, this)),
        change_cipher_spec_(hop::BindAll(&Connection::ChangeCipherSpec_, this)),
        alert_(hop::BindAll(&Connection::Alert_, this)),
        handshake_(hop::BindAll(&Connection::Handshake_, this))
    {
        protocols_[0].fiber_ = &change_cipher_spec_;
        protocols_[1].fiber_ = &alert_;
        protocols_[2].fiber_ = &handshake_;

        {
            mfa::Pipe pipe(&record_);
            ios::ReaderHolder::reader_ = pipe.GetReader();
            appdata_.writer_ = pipe.GetWriter();
        }
    }
};

} }

#endif//MENES_NET_TLS_CONNECTION_HPP
