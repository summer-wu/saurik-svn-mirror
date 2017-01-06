/* Menes - C++ High-Level Utility Library
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

#include "api/posix/error.hpp"
#include "api/posix/handle.hpp"
#include "api/posix/pipe.hpp"
#include "api/posix/process.hpp"

#include "api/threads.hpp"
#include "hop/bind.hpp"
#include "ext/stackref.hpp"

#include <sys/types.h>
#include <sys/wait.h>

#include <unistd.h>

namespace api {
namespace Posix {

namespace be {

struct JoinData_ :
    public ext::ReferenceCounted
{
    api::ThreadCondition::Mutex startl_;
    api::ThreadCondition start_;

    volatile bool joinable_;
    int status_;

    _R<api::Thread> thread_;

    JoinData_() :
        start_(startl_),
        joinable_(true)
    {
    }
};

struct SpawnData_ {
    Pipe stdin_;
    Pipe stdout_;

    const cse::String &executable_;
    const _L<cse::String> &arguments_;

    api::ThreadCondition::Mutex readyl_;
    api::ThreadCondition ready_;

    int pid_;
    int status_;

    SpawnData_(const cse::String &executable, const _L<cse::String> &arguments) :
        executable_(executable),
        arguments_(arguments),
        ready_(readyl_),
        pid_(-1)
    {
    }
};

// XXX: check CFG_CROSS_THREAD_WAIT to save threads
int Spawn_(be::SpawnData_ *data, const _R<JoinData_> &join) {
    Pipe pipe;

    if (int pid = CheckError(::fork())) {
        pipe.ClearWriter();

        data->pid_ = pid;

        size_t read(pipe.GetReader()->ReadMost(reinterpret_cast<byte_t *>(&data->status_), sizeof(data->status_)));

        if (read != sizeof(data->status_)) {
            _assert(read == 0);
            data->status_ = 0;
        }

        _synchronized (join->startl_) {
            _synchronized (data->readyl_)
                data->ready_.Signal();
            join->start_.Wait();
        }

        if (join->joinable_) {
            int child(CheckError(::waitpid(pid, &join->status_, 0)));
            _assert(child == pid);
            join->joinable_ = false;
        }

        return 0;
    } else {
        pipe.ClearReader();

        _L<const char *> argv;
        argv.Reserve(data->arguments_.GetSize() + 1);

        argv.InsertLast(data->executable_.NullTerminate());
        _foreach (const _L<cse::String>, argument, data->arguments_)
            argv.InsertLast(argument->NullTerminate());

        CheckError(::dup2(data->stdin_.GetReadHandle(), 0));
        CheckError(::dup2(data->stdout_.GetWriteHandle(), 1));
        CheckError(::dup2(data->stdout_.GetWriteHandle(), 2));

        int error(::execv(data->executable_.NullTerminate(), const_cast<char **>(argv.NullTerminate())));

        // XXX: in case of other random exeptions this stream might should be a serialized exception
        _assert(error != 0);

        error = ApiTraits::GetLastError();
        pipe.GetWriter()->WriteFully(reinterpret_cast<byte_t *>(&error), sizeof(error));

        // XXX: this might should be _Exit()
        // XXX: why isn't this exit()?
        // XXX: negative exit()
        _exit(-1);
    }
}

}

Process::Process(const cse::String &executable, const _L<cse::String> &arguments) :
    join_(new _H<be::JoinData_>())
{
    be::SpawnData_ data(executable, arguments);

    reader_ = data.stdout_.GetReader();
    writer_ = data.stdin_.GetWriter();

    _synchronized (data.readyl_) {
        join_->thread_ = new _H<api::Thread>(hop::BindAll(&be::Spawn_, &data, join_));
        data.ready_.Wait();
    }

    pid_ = data.pid_;

    if (data.status_ != 0) {
        Join();
        throw Error(data.status_);
    }
}

Process::~Process() {
    if (join_->joinable_)
        _synchronized (join_->startl_)
            join_->start_.Signal();
}

int Process::Join() const {
    _assert(join_->joinable_);

    _synchronized (join_->startl_)
        join_->start_.Signal();

    join_->thread_->Join();
    return join_->status_;
}

} }
