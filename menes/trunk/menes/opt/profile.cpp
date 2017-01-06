/* Menes Optimization - C++ Profiling and Verification
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

#include "cxx/standard.hh"
#pragma hdrstop("build/cxx/standard.pch")

#include "cxx/platform.hpp"

#include "api/locks.hpp"
#include "api/module.hpp"
#include "api/threads.hpp"
#include "api/timing.hpp"

#include "app/configuration.hpp"

#include "ext/allocator.hpp"
#include "ext/listset.hpp"
#include "ext/map.hpp"
#include "ext/radix.hpp"
#include "ext/singleton.hpp"
#include "cse/string.hpp"
#include "ext/vector.hpp"

#include "opt/profile.hpp"
#include "opt/reports.hpp"

namespace opt {

namespace be {
#ifdef _WIN32
    struct Exports_ {
        api::Win32::Export<BOOL (__stdcall *)(LPFILETIME, LPFILETIME, LPFILETIME)> GetSystemTimes_;

        Exports_() :
            GetSystemTimes_(_B("kernel32.dll"), _B("GetSystemTimes"))
        {
        }
    };
#endif

    void SystemTimes_(uint64_t &idle, uint64_t &kernel, uint64_t &user) {
#ifdef _WIN32
        if (_1<Exports_>().GetSystemTimes_ == NULL)
            idle = kernel = user = 0;
        else if (FALSE == _1<Exports_>().GetSystemTimes_(reinterpret_cast<LPFILETIME>(&idle), reinterpret_cast<LPFILETIME>(&kernel), reinterpret_cast<LPFILETIME>(&user)))
            throw api::Win32::Error();
#else
        idle = kernel = user = 0;
#endif
    }

    void ProcessTimes_(uint64_t &kernel, uint64_t &user) {
#ifdef _WIN32
        FILETIME ftcreation, ftexit;
        if (FALSE == ::GetProcessTimes(::GetCurrentProcess(), &ftcreation, &ftexit, reinterpret_cast<LPFILETIME>(&kernel), reinterpret_cast<LPFILETIME>(&user)))
            throw api::Win32::Error();
#else
        kernel = user = 0;
#endif
    }

    struct ThreadData {
        _R<api::Thread> thread_;
        cse::String name_;

        uint64_t kernel_;
        uint64_t user_;

        ThreadData() :
            kernel_(0),
            user_(0)
        {
        }
    };

    typedef api::WallTimer Timer_;

    struct Register_ {
        uint64_t clocks;

        typedef ext::Vector< Performance *, ext::Block< Performance *, ext::TypedAllocator<ext::MallocAllocator> > > ReadingList_;
        ReadingList_ readings_;

        typedef ext::ListMap<cse::String, ThreadData> ThreadMap_;
        ThreadMap_ threads_;

        api::CriticalSection readinglock_;

        struct Recording {
            uint64_t cycles;

            struct {
                uint64_t idle;
                uint64_t kernel;
                uint64_t user;
            } system;

            struct {
                uint64_t kernel;
                uint64_t user;
            } process;
        } beginning, recorded;

        Register_() {
            clocks = Timer_::GetValue();

            MENES_RDTSC(beginning.cycles);

            SystemTimes_(beginning.system.idle, beginning.system.kernel, beginning.system.user);

            beginning.process.kernel = 0;
            beginning.process.user = 0;
        }
    };
}

class PerformanceTotal {
  public:
    typedef hop::FunctionTraits<uint64_t (const Performance *)> Traits;

  private:
    TimingData Performance::*version_;

  public:
    PerformanceTotal(TimingData Performance::*version) :
        version_(version)
    {
    }

    _finline uint64_t Execute(const Performance *value) {
        const TimingData &data(value->*version_);
        return data.total_;
    }
};

OPT_DECLARE double CpuFrequency() {
    // XXX: uint64_t -> double :(
    int64_t cycles;
    MENES_RDTSC(cycles);
    uint64_t clocks(be::Timer_::GetValue());

    // XXX: research possible high uptime double precision issue
    //elapsed &= UINT64_C(0x0001ffffffffffff);

    return (cycles - _1<be::Register_>().beginning.cycles) / double(clocks - _1<be::Register_>().clocks) * be::Timer_::GetFrequency();
}

OPT_DECLARE void initialize(const cse::String &name) {
    be::ThreadData &data(_1<be::Register_>().threads_[name]);

    data.thread_ = &api::Thread::Current();
    data.name_ = name;
}

OPT_DECLARE void Record() {
    be::Register_::Recording &recording(_1<be::Register_>().recorded);

    MENES_RDTSC(recording.cycles);

    be::SystemTimes_(recording.system.idle, recording.system.kernel, recording.system.user);
    be::ProcessTimes_(recording.process.kernel, recording.process.user);

    {
        ext::ScopeLock<api::CriticalSection> readingguard(_1<be::Register_>().readinglock_);
        for (be::Register_::ReadingList_::ConstIterator reading(_1<be::Register_>().readings_.Begin()); reading != _1<be::Register_>().readings_.End(); ++reading) {
            (*reading)->previous_ += (*reading)->current_;
            (*reading)->current_.Reset();
        }
    }

    for (be::Register_::ThreadMap_::Iterator thread(_1<be::Register_>().threads_.Begin()); thread != _1<be::Register_>().threads_.End(); ++thread) {
        be::ThreadData &data(thread->Second());
        data.thread_->GetTimingInfo(data.kernel_, data.user_);
    }
}

void Report_(ios::PrintWriter &out, TimingData Performance::*version, bool recorded) {
    OPT_TIMER("opt::Report_()")

    // this is in kHz
    double freq(CpuFrequency() / 1000);

    be::Register_::Recording &recording(recorded ? _1<be::Register_>().recorded : _1<be::Register_>().beginning);

    out << ios::SetPrecision(7);
    out << "| CPU MHz: " << (freq / 1000) << ios::NewLine;

    static const unsigned GroupWidth_(_C("menes/opt/profiler/@group-width", 17));
    static const unsigned NameWidth_(_C("menes/opt/profiler/@name-width", 40));

    // XXX: this sucks even MORE ass
    ext::Block<char> buffer(ext::max(GroupWidth_, NameWidth_));

    out << "+-";
    _forall (unsigned, i, 0, GroupWidth_)
        out << "-";
    out << "-+------------------------------------------+--------------------+--------------------+---------+---------+-----------+" << ios::NewLine;

    out << "  ";
    _forall (unsigned, i, 0, GroupWidth_)
        out << " ";
    out << "                     Name                      Kernel Time (ms)      User Time (ms)      Total     %-age               " << ios::NewLine;

    out << "+-";
    _forall (unsigned, i, 0, GroupWidth_)
        out << "-";
    out << "-+-";
    _forall (unsigned, i, 0, NameWidth_)
        out << "-";
    out << "-+--------------------+--------------------+---------+---------+-----------+" << ios::NewLine;

    uint64_t system;

    {
        uint64_t idle, kernel, user;
        be::SystemTimes_(idle, kernel, user);

        idle -= recording.system.idle;
        kernel -= recording.system.kernel;
        user -= recording.system.user;

        system = kernel + user;

        out << "| ";
        _forall (unsigned, i, 0, GroupWidth_)
            out << " ";
        out << " | <System>                                ";

        out << ios::SetPrecision(7);
        out << " | " << ios::SetWidth(18) << (kernel / 10000.0);
        out << " | " << ios::SetWidth(18) << (user / 10000.0);
        out << " | " << ios::SetWidth(7) << unsigned((kernel + user) / 10000.0);

        out << ios::SetPrecision(3);
        out << " | " << ios::SetWidth(7) << (idle + system == 0 ? 0.0 : system * 100.0 / (idle + system));

        out << " |           |" << ios::NewLine;
    }

    uint64_t process;

    {
        uint64_t kernel, user;
        be::ProcessTimes_(kernel, user);

        kernel -= recording.process.kernel;
        user -= recording.process.user;

        process = kernel + user;

        out << "| ";
        _forall (unsigned, i, 0, GroupWidth_)
            out << " ";
        out << " | \\- <Process>                            ";

        out << ios::SetPrecision(7);
        out << " | " << ios::SetWidth(18) << (kernel / 10000.0);
        out << " | " << ios::SetWidth(18) << (user / 10000.0);
        out << " | " << ios::SetWidth(7) << unsigned((kernel + user) / 10000.0);

        out << ios::SetPrecision(3);
        out << " | " << ios::SetWidth(7) << (system == 0 ? 0.0 : process * 100.0 / system);

        out << " |           |" << ios::NewLine;
    }

    _foreach (be::Register_::ThreadMap_, thread, _1<be::Register_>().threads_) {
        const be::ThreadData &data(thread->Second());

        uint64_t kernel, user;
        data.thread_->GetTimingInfo(kernel, user);

        if (recorded) {
            kernel -= data.kernel_;
            user -= data.user_;
        }

        out << "| ";
        _forall (unsigned, i, 0, GroupWidth_)
            out << " ";
        out << " |    \\- ";

        out << ios::SetAlignment(ios::Left) << ios::SetWidth(34) << data.name_ << ios::SetAlignment(ios::Right);

        out << ios::SetPrecision(7);
        out << " | " << ios::SetWidth(18) << (kernel / 10000.0);
        out << " | " << ios::SetWidth(18) << (user / 10000.0);
        out << " | " << ios::SetWidth(7) << unsigned((kernel + user) / 10000.0);

        out << ios::SetPrecision(3);
        out << " | " << ios::SetWidth(7) << ((kernel + user) * 100.0 / process);

        out << " |           |" << ios::NewLine;
    }

    out << "+-";
    _forall (unsigned, i, 0, GroupWidth_)
        out << "-";
    out << "-+------------------------------------------+--------------------+--------------------+---------+---------+-----------+" << ios::NewLine;
    out << "  ";
    out << ios::SetAlignment(ios::Center) << ios::SetWidth(GroupWidth_) << "Group" << ios::SetAlignment(ios::Right);
    out << "                     Name                        Average (ms)         Maximum (ms)       Total     %-age     # Calls   " << ios::NewLine;

    out << "+-";
    _forall (unsigned, i, 0, GroupWidth_)
        out << "-";
    out << "-+------------------------------------------+--------------------+--------------------+---------+---------+-----------+" << ios::NewLine;

    out << "| ";
    _forall (unsigned, i, 0, GroupWidth_)
        out << " ";
    out << " | <Program>                               ";

    int64_t elapsed;
    MENES_RDTSC(elapsed);
    elapsed -= recording.cycles;

    double total(elapsed / freq);

    ext::ScopeLock<api::CriticalSection> readingguard(_1<be::Register_>().readinglock_);
    be::Register_::ReadingList_ &readings(_1<be::Register_>().readings_);

    unsigned calls(0);
    _foreach (be::Register_::ReadingList_, reading, readings)
        calls += ((**reading).*version).calls_;

    out << ios::SetPrecision(7);
    out << " | " << ios::SetWidth(18) << total;
    out << " | " << ios::SetWidth(18) << total;

    out << " | " << ios::SetWidth(7) << unsigned(total);

    out << " |  99.999";
    out << " | " << ios::SetWidth(9) << calls;

    out << " |" << ios::NewLine;

    {
        // XXX: this shouldn't have to _initialize_ the data...
        be::Register_::ReadingList_ temp(readings.GetSize());
        ext::CustomRadixSort<11, 6>(readings.Begin(), readings.End(), temp.Begin(), PerformanceTotal(version));
        ext::Swap(readings, temp);
    }

    _rforeach (be::Register_::ReadingList_, reading, readings) {
        Performance &read(**reading);
        TimingData &timing(read.*version);

        if (timing.calls_ == 0)
            continue;

        double total(timing.total_ / freq);

        out << ios::SetAlignment(ios::Left);

        out << "| ";
        if (read.group_ == NULL)
            out << ios::SetWidth(GroupWidth_) << "(null)";
        else {
            size_t length(::strlen(read.group_));
            if (length <= GroupWidth_)
                out << ios::SetWidth(GroupWidth_) << read.group_;
            else {
                // XXX: this sucks ass
                ::memcpy(buffer.Begin(), read.group_, GroupWidth_ - 3);
                buffer[GroupWidth_ - 3] = '\0';
                out << buffer << "...";
            }
        }

        out << " | ";
        if (read.name_ == NULL)
            out << ios::SetWidth(NameWidth_) << "(null)";
        else {
            size_t length(::strlen(read.name_));
            if (length <= NameWidth_)
                out << ios::SetWidth(NameWidth_) << read.name_;
            else {
                // XXX: this sucks ass
                ::memcpy(buffer.Begin(), read.name_, NameWidth_ - 3);
                buffer[NameWidth_ - 3] = '\0';
                out << buffer << "...";
            }
        }

        out << ios::SetAlignment(ios::Right);

        out << ios::SetPrecision(7);
        out << " | " << ios::SetWidth(18) << (total / timing.calls_);
        out << " | " << ios::SetWidth(18) << (timing.max_ / freq);

        out << " | " << ios::SetWidth(7) << unsigned(total);

        out << ios::SetPrecision(3);
        out << " | " << ios::SetWidth(7) << (timing.total_ * 100.0 / elapsed);

        out << " | " << ios::SetWidth(9) << timing.calls_;
        out << " |" << ios::NewLine;
    }

    out << "+-";
    _forall (unsigned, i, 0 ,GroupWidth_)
        out << "-";
    out << "-+-";
    _forall (unsigned, i, 0, NameWidth_)
        out << "-";
    out << "-+--------------------+--------------------+---------+---------+-----------+" << ios::NewLine;
}

OPT_DECLARE void Report(ios::PrintWriter &out) {
    Record();
    return Report_(out, &Performance::previous_, false);
}

OPT_DECLARE void Snapshot(ios::PrintWriter &out) {
    return Report_(out, &Performance::current_, true);
}

Performance::Performance() :
    name_(NULL),
    group_(NULL),
    entered_(false)
{
    Install_();
}

Performance::Performance(const char *name, const char *group) :
    name_(name),
    group_(group),
    entered_(false)
{
    Install_();
}

Performance::Performance(const Performance &rhs) :
    name_(rhs.name_),
    group_(rhs.group_),
    entered_(false)
{
    Install_();
}

void Performance::Install_() {
    /*if (_1<be::Register_>() == NULL)
        _1<be::Register_>() = etl::ObjectAllocator<be::Register_, ext::MallocAllocator>::Allocate();*/

    ext::ScopeLock<api::CriticalSection> readingguard(_1<be::Register_>().readinglock_);
    _1<be::Register_>().readings_.InsertLast(this);
}

Performance::~Performance() {
    ext::ScopeLock<api::CriticalSection> readingguard(_1<be::Register_>().readinglock_);
    _1<be::Register_>().readings_.RemoveFirst(this);
}

Performance::operator uint64_t() {
    return current_.total_;
}

}
