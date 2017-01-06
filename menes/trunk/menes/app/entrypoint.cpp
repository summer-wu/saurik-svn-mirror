/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2003-2005  Jay Freeman (saurik)
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

#include "api/stacktrace.hpp"

#include "app/application.hpp"
#include "app/configuration.hpp"
#include "app/entrypoint.hpp"
#include "app/fatality.hpp"
#include "app/setup.hpp"

#include "hop/bind.hpp"
#include "ext/exception.hpp"
#include "ios/string.hpp"
#include "log/logging.hpp"

#include <exception> //OK
#include <typeinfo> //OK

namespace app {

class APP_DECLARE AssertionFailedException :
    public ext::StringException
{
  public:
    AssertionFailedException() {
    }

    AssertionFailedException(const cse::String &string) :
        ext::StringException(string)
    {
    }
};

namespace {

Application *application_(NULL);

static void Terminate_() {
    api::StackTrace trace;
    api::Cerr << trace << ios::NewLine;
    app::ReportFatality("std::terminate()", cse::String());
}

static void Failure_(const char *msg) {
    /*if (_C("menes/opt/assert/@log", false))
        LOG_THREAD() << "Assertion Failed: " << (msg == NULL ? "(null)" : msg) << ios::NewLine;*/

    if (_C("menes/opt/assert/@break", false))
        _breakpoint();

    if (_C("menes/opt/assert/@throw", true))
        if (msg == NULL)
            throw AssertionFailedException();
        else
            throw AssertionFailedException(cse::String(msg, ext::ReadOnlyExternal));
}

}

namespace be {

APP_DECLARE void SetApplication_(Application *application) {
    _assert(application_ == NULL);
    application_ = application;
}

int AppMain_(int argc, const char *argv[]) {
    std::set_terminate(&app::Terminate_);
    ::be::Failure_ = &Failure_;

    // XXX: this code assumes that argv[0] ~== app::GetExecutableFileName()
    //      maybe this loop needs to go into the platform specific area?
    _assert(argc > 0);

    ArgumentList args;
    for (int i(1); i != argc; ++i)
        args.InsertLast(cse::String(argv[i], ext::ReadOnlyExternal));
    be::SetArguments(args);

    _assert(application_ != NULL);

    be::Pers_ &pers(be::GetPerss_().process_);

    _foreach (be::Pers_, per, pers)
        (*per)->Start();

    int retval = ThreadMain_(hop::BindAll(&Application::Run, application_, args));

    _rforeach (be::Pers_, per, pers)
        (*per)->Stop();

    return retval;
}

int ThreadMain_(const hop::Function<int ()> &main) {
    be::Pers_ &pers(be::GetPerss_().thread_);

    _foreach (be::Pers_, per, pers)
        (*per)->Start();

    int retval(Protect_(main));

    _rforeach (be::Pers_, per, pers)
        (*per)->Stop();

    return retval;
}

int Protect_(const hop::Function<int ()> &main) {
    int retval(-1);

    try {
        retval = main();
    } catch (const ext::Exception &exception) {
        _S<ios::Buffer> report;
        report << exception;
        // XXX: this should arguably be done by ReportFatality
        LOG_THREAD() << "Unhandled Exception" << report << ios::NewLine << ios::Flush;
        app::ReportFatality("Unhandled Exception", report);
    } catch (const std::exception &exception) {
        LOG_THREAD() << "Unhandled Exception[" << typeid(exception).name() << "]: \"" << exception.what() << "\"" << ios::NewLine;
    } catch (...) {
        LOG_THREAD() << "Unhandled Exception[???]: ??? (try debug mode)" << ios::NewLine;
    }

    return retval;
}

}

}
