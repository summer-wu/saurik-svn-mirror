/* Menes Optimization - C++ Profiling and Verification
 * Copyright (C) 2002-2004  Jay Freeman (saurik)
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

#ifndef MENES_OPT_PROFILE_HPP
#define MENES_OPT_PROFILE_HPP

#include "cxx/platform.hpp"
#include "opt/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

// XXX: move to x86 and generalize timing utils
// XXX: add cpuid serializing instructions?
#if defined(_MSC_VER) && defined(_M_IX86)
#define MENES_RDTSC(x) { \
    uint64_t value; \
    { \
        /*__asm cpuid*/ \
        __asm rdtsc \
        __asm mov dword ptr [value+4], edx \
        __asm mov dword ptr [value], eax \
        /*__asm cpuid*/ \
    } \
    x = value; \
}
#elif defined(__GNUC__) && defined(__x86_64__)
#define MENES_RDTSC(x) \
    /*__asm__ volatile ("cpuid" : : "a" (0) : "ebx", "ecx", "edx");*/ \
    __asm__ volatile ("rdtsc; shlq $32, %%rdx; orq %%rdx, %%rax" : "=a" (x) : : "rdx"); \
    /*__asm__ volatile ("cpuid" : : "a" (0) : "ebx", "ecx", "edx");*/
#elif defined(__GNUC__) && defined(__i386__)
#define MENES_RDTSC(x) \
    /*__asm__ volatile ("cpuid" : : "a" (0) : "ebx", "ecx", "edx");*/ \
    __asm__ volatile ("rdtsc" : "=A" (x)); \
    /*__asm__ volatile ("cpuid" : : "a" (0) : "ebx", "ecx", "edx");*/
#elif defined(__GNUC__) && defined(__ppc__)
#include "api/timing.hpp"
#define MENES_RDTSC(x) \
    x = api::WallTimer::GetValue();
#endif

namespace opt {

OPT_DECLARE double CpuFrequency();

struct OPT_DECLARE TimingData {
    // XXX: uint64_t -> double :(
    int64_t total_;
    uint64_t max_;
    uint32_t calls_;

    inline TimingData() {
        Reset();
    }

    inline void Reset() {
        total_ = 0;
        max_ = 0;
        calls_ = 0;
    }

    TimingData &operator +=(const TimingData &rhs) {
        total_ += rhs.total_;
        if (max_ < rhs.max_)
            max_ = rhs.max_;
        calls_ += rhs.calls_;
        return *this;
    }
};

struct OPT_DECLARE Performance {
    const char *name_;
    const char *group_;

    bool entered_;

    TimingData current_;
    TimingData previous_;

    Performance();
    explicit Performance(const char *name, const char *group = NULL);

    Performance(const Performance &rhs);

    void Install_();

    ~Performance();

    operator uint64_t();
};

// XXX: feasibility check on 2-level inline
class ScopeTimer {
  private:
    Performance &timer_;
    uint64_t clock_;
    bool stopped_;

  public:
    inline ScopeTimer(Performance &timer) :
        timer_(timer),
        stopped_(timer_.entered_)
    {
        ++timer_.current_.calls_;
        if (stopped_)
            return;
        timer_.entered_ = true;
        MENES_RDTSC(clock_);
    }

    inline ~ScopeTimer() {
        uint64_t clock;
        MENES_RDTSC(clock);
        if (stopped_)
            return;

        clock -= clock_;
        timer_.current_.total_ += clock;
        if (clock > timer_.current_.max_)
            timer_.current_.max_ = clock;

        timer_.entered_ = false;
    }

    inline void Cancel() {
        stopped_ = true;
    }

    inline void Stop() {
        uint64_t clock;
        MENES_RDTSC(clock);
        if (stopped_)
            return;

        clock -= clock_;
        timer_.current_.total_ += clock;
        if (clock > timer_.current_.max_)
            timer_.current_.max_ = clock;

        timer_.entered_ = false;
        stopped_ = true;
    }
};

}

#ifdef NPROFILE

#define OPT_DECLAREPERFORMANCE(mods, var)

#define OPT_DEFINEPERFORMANCE(var, name)
#define OPT_DEFINEPERFORMANCE_GROUP(var, name, set)
#define OPT_TEMPLATEPERFORMANCE_GROUP(arg, var, name, set)

#define OPT_SCOPETIMER(name)

#define OPT_TIMER(name)
#define OPT_TIMER_GROUP(name, set)

#define OPT_TIMER_STOP()
#define OPT_TIMER_PAUSE()
#define OPT_TIMER_CANCEL()

#else

#define OPT_DECLAREPERFORMANCE(mods, var) \
    mods opt::Performance var;

#define OPT_DEFINEPERFORMANCE(var, name) \
    opt::Performance var(name);

#define OPT_DEFINEPERFORMANCE_GROUP(var, name, set) \
    opt::Performance var(name, set);

// XXX: due to a stupidity in the spec this can't be a call to OPT_DEFINEPERFORMANCE_GROUP
#define OPT_TEMPLATEPERFORMANCE_GROUP(arg, var, name, set) \
    template <arg> \
    opt::Performance var(name, set);

#define OPT_SCOPETIMER(var) \
    opt::ScopeTimer stimer_(var);

#define OPT_TIMER(name) \
    static opt::Performance ptimer_(name); \
    OPT_SCOPETIMER(ptimer_);

#define OPT_TIMER_GROUP(name, set) \
    static opt::Performance ptimer_(name, set); \
    OPT_SCOPETIMER(ptimer_);

#define OPT_TIMER_STOP() \
    stimer_.Stop();

#define OPT_TIMER_CANCEL() \
    stimer_.Cancel();

#endif

#endif//MENES_OPT_PROFILE_HPP
