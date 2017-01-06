/* Ninetjer - _Truly_ Free (non-GPL) Unix .NET Runtime
 * Copyright (C) 2004-2005  Jay Freeman (saurik)
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

#include "cxx/standard.hh"
#pragma hdrstop("build/cxx/standard.pch")

#include "api/console.hpp"
#include "api/environment.hpp"
#include "api/exename.hpp"
#include "api/files.hpp"
#include "api/fiber.hpp"
#include "api/locks.hpp"
#include "api/pipe.hpp"
#include "api/process.hpp"
#include "api/socket.hpp"
#include "api/threads.hpp"

#include "app/command.hpp"
#include "cse/string.hpp"
#include "enc/encrypt/rc4.hpp"
#include "etl/composite.hpp"

#include "ext/linear.hpp"
#include "ext/radix.hpp"
#include "ext/redblackset.hpp"
#include "ext/refcount.hpp"
#include "ext/scopes.hpp"
#include "ext/shared.hpp"
#include "ext/stackref.hpp"
#include "ext/uuid.hpp"

#include "hop/bind.hpp"

#include "ios/helpers.hpp"
#include "ios/stdadapters.hpp"

#include "net/mime/application/x-www-form-urlencoded.hpp"
#include "net/tls/connection.hpp"

#include "opt/memory.hpp"
#include "opt/profile.hpp"
#include "opt/reports.hpp"

#include "xml/document.hpp"
#include "xml/domwriter.hpp"
#include "xml/element.hpp"
#include "xml/name.hpp"
#include "xml/node.hpp"
#include "xml/nodeset.hpp"
#include "xml/parse.hpp"
#include "xml/resolver.hpp"
#include "xml/textwriter.hpp"

#include <unistd.h>

app::SimpleCommand<> main_;

#define TST_CHECK(test) \
    if (!(test)) \
        api::Cout << "FAIL: " << #test << ios::NewLine;

struct Type0 :
    public ext::ReferenceCounted
{
};

struct Type1 {
    ext::ReferenceCounted *method0(int arg0, _R<ext::ReferenceCounted> arg1) {
        TST_CHECK(arg0 == 0x31337)
        return arg1;
    }
};

int Ack(const _R<ios::Reader> &reader, const _R<ios::Writer> &writer) {
    api::Cout << "start" << ios::NewLine;
    ios::ReadToWrite(*reader, *writer);
    api::Cout << "end" << ios::NewLine;
    return 0;
}

void Word(const _R<api::Fiber> &fiber) {
    api::Cout << fiber.GetValue() << ios::NewLine;
}

int Throw() {
    _assert(false);
    return 0;
}

int thread() {
    api::Cout << "ark" << ios::NewLine;
    return 0;
}

namespace app {
template <>
int app::SimpleCommand<>::Execute(const app::Options &options) {
    _S<api::Thread> th(&thread);
    _repeat(10) {
    ::sleep(2);
    api::Cout << th.GetReferenceCount() << ios::NewLine;
    }
    th.Join();
    api::Cout << th.GetReferenceCount() << ios::NewLine;
    return 0;

    _S<api::Process> process("bob");

    _S<net::Mime::Application::XWwwFormUrlencoded> form(_B(""));
    /*{
        mfa::Pipe pipe;
        _S<api::Fiber> fiber(hop::BindAll(&Ack, pipe.GetReader(), &api::Cout));
        fiber.Continue();
        ios::ReadToWrite(api::Cin, *pipe.GetWriter());
        //throw ext::StringException("Word");
        pipe.ClearWriter();
    }*/

    /*int *q32(0);
    *q32 = 5;*/

    /*_S<api::Fiber> fiber(&Throw);
    api::Cout << "Yes?" << ios::NewLine;
    fiber.Call();*/

    api::Cout << "No!" << ios::NewLine;

    _S<api::TcpSocket> server;
    server.SetLocalAddress(api::InternetAddress(api::InternetAddress::Loopback, 4567), true);
    server.Listen();

    _S<api::Socket> client(server, _null<api::Address>());
    net::Tls::Connection connection(&client, &client);
    ios::ReadToWrite(*connection.GetReader(), api::Cout);

    return 0;

    cse::String path(api::GetExecutablePath().GetPath());
    api::Cout << static_cast<void *>(path.Begin().GetDataIterator()) << ios::NewLine;
    api::Cout << static_cast<void *>(path.End().GetDataIterator()) << ios::NewLine;
    _foreach (cse::String, l, path)
        if (*l == '/')
            api::Cout << *l << ios::NewLine;
    api::Cout << static_cast<void *>(ext::FindFirstWith<ext::EqualTo>(path, '/').GetDataIterator()) << ios::NewLine;
    api::Cout << api::GetExecutablePath().GetParent().GetPath() << ios::NewLine;
    return 0;

    _S<ext::LinearAllocator> linear;

    void *a(linear.Allocate(50));
    void *b(linear.Allocate(50));

    api::Cout << a << ios::NewLine;
    api::Cout << b << ios::NewLine;

    linear.Deallocate(b);
    b = linear.Allocate(50);

    api::Cout << b << ios::NewLine;

    linear.Deallocate(a);
    a = linear.Allocate(50);

    api::Cout << a << ios::NewLine;

    linear.Deallocate(a);
    a = linear.Allocate(50);

    api::Cout << a << ios::NewLine;

    linear.Deallocate(b);
    linear.Deallocate(a);

    a = linear.Allocate(50);
    api::Cout << a << ios::NewLine;

    linear.Deallocate(a);

    return 0;

    api::Cout << "hop::Function" << ios::NewLine;

    _R<ext::ReferenceCounted> object0(new _H<Type0>());
    Type1 object1;
    TST_CHECK(hop::BindAll(&Type1::method0, &object1, 0x31337, object0)() == object0)

    api::Cout << "api::Process" << ios::NewLine;

    {
        _S<api::Process> process("/bin/ls");
        ios::ReadToWrite(*process.GetReader(), api::Cout);
        api::Cout << "2" << ios::NewLine;
        process.Join();
        api::Cout << "3" << ios::NewLine;
    }
    api::Cout << "4" << ios::NewLine;
    return 0;

    _R<xml::Document> doc(xml::Parse("data/tests/test.xml"));
    xml::NodeSet nodes(*doc/"dude"/"comment()");
    api::Cout << nodes.GetSize() << ios::NewLine;
    return 0;

    api::Cout << alg::ilog2(uint32_t(127)) << " :" << alg::ilog2(uint32_t(128)) << " " << alg::ilog2(uint32_t(129)) << ios::NewLine;
    //api::Cout << api::TheEnvironment[*options[0]] << ios::NewLine;

    _R<xml::Document> test(xml::Parse("data/tests/test.xml"));
    _R<xml::Node> ack(*test/"dude");
    cse::String wow(*ack/"arf");
    api::Cout << wow << ios::NewLine;

    return 0;

    _assert(false);

    /*void *arr[16];
    api::Cout << backtrace(arr, 16) << ios::NewLine;*/

    api::StackTrace trace;

    throw ext::InvalidArgumentException();

    api::CriticalSection lock;
    api::Cout << &lock << ios::NewLine;
    //for (bool done(false); !done; )
        //for (const ext::Scope &_scope = ext::be::ScopeLock_(lock); !done; done = true)
    if (const ext::Scope &_scope = ext::be::ScopeLock_(lock)); else
            api::Cout << "hello" << ios::NewLine;
    /*for (;;) {
        std::cout << adapter.sgetc() << std::endl;
        std::cout.flush();
    }*/

    /*std::string line;
    std::getline(sock, line);
    api::Cout << line << ios::NewLine;*/

    return 0;
} }
