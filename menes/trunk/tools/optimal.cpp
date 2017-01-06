/* Ninetjer - _Truly_ Free (non-GPL) Unix .NET Runtime
 * Copyright (C) 2006  Jay Freeman (saurik)
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

#include "api/files.hpp"
#include "app/simple.hpp"

#include "ext/block.hpp"
#include "ext/radix.hpp"

#include "ios/helpers.hpp"
#include "opt/profile.hpp"

#include <cmath>
#include <cstdlib>

// XXX: move to cxx
#define _not(type) (~type())
#define _until(test) while (!(test))

struct Constraint {
    cse::String name_;
    double mean_;
};

typedef ext::Block<Constraint, ext::NewArrayAllocator> Constraints;
typedef ext::Block<unsigned> Violations;

struct GetMean {
    typedef hop::FunctionTraits<double (const Constraint &)> Traits;
    static double Execute(const Constraint &constraint) {
        return constraint.mean_;
    }
};

struct Candidate {
    unsigned used_;
    cse::String name_;
    double probability_;
    Violations violations_;

    Candidate() :
        used_(0)
    {
    }
};

typedef ext::Block<Candidate, ext::NewArrayAllocator> Candidates;

struct Word {
    unsigned used_;
    cse::String name_;
    Candidates candidates_;

    Word() :
        used_(0)
    {
    }
};

typedef ext::Block<Word, ext::NewArrayAllocator> Words;

struct Weight {
    double value_;
    size_t constraint_;
};

typedef ext::Block<Weight> Weights;

struct GetWeight {
    typedef hop::FunctionTraits<double (const Weight &)> Traits;
    static double Execute(const Weight &weight) {
        return weight.value_;
    }
};

class Random {
  private:
    uint64_t seed_;

  public:
    void SetSeed(uint64_t seed) {
        seed_ = (seed ^ UINT64_C(0x5deece66d)) & ((UINT64_C(1) << 48) - 1);
    }

    uint32_t Next(unsigned bits) {
        _assume(bits < 32);
        seed_ = (seed_ * UINT64_C(0x5deece66d) + 0xb) & ((UINT64_C(1) << 48) - 1);
        return seed_ >> (48 - bits);
    }

    float NextFloat() {
        return Next(24) / float(1 << 24);
    }

    double NextDouble() {
        return ((uint64_t(Next(26)) << 27) + Next(27)) / double(UINT64_C(1) << 53);
    }
};

Random random_;

double GetRandom() {
    return random_.NextDouble();
    //return rand() / double(RAND_MAX);
}

void GetNormals(double &y1, double &y2) {
    OPT_TIMER("GetNormals()")

    double x1, x2, w;

    do {
        x1 = 2 * GetRandom() - 1;
        x2 = 2 * GetRandom() - 1;
        w = x1 * x1 + x2 * x2;
    } while (w >= 1);

    w = sqrt(-2 * log(w) / w);
    y1 = x1 * w;
    y2 = x2 * w;
}

double GetNormal() {
    static double y1, y2;
    static bool ready(false);

    if (ready = !ready) {
        GetNormals(y1, y2);
        return y1;
    } else return y2;
}

void Randomize(const Constraints &constraints, Weights &weights, double noise) {
    OPT_TIMER("Randomize()")

    static Weights sorted(constraints.GetSize());
    _assert(sorted.GetSize() == weights.GetSize());

    _foreach (Weights, weight, weights)
        weight->value_ = GetNormal() * noise + constraints[weight->constraint_].mean_;

    {
        OPT_TIMER("Randomize()[Radix]")
        //api::Cout << "Sorting" << ios::NewLine;
        ext::FloatingRadixSort(weights.Begin(), weights.End(), sorted.Begin(), GetWeight());
    }

    {
        OPT_TIMER("Randomize()[Swap]")
        ext::Swap(weights, sorted);
    }

    /*_foreach (Weights, weight, weights)
        api::Cout << weight->value_ << ": " << weight->constraint_ << ios::NewLine;*/
}

ext::Block<const Candidate *> candidates;

const Candidate *Execute(const Constraints &constraints, Weights &weights, double noise, const Word &word) {
    _assert(candidates.GetSize() >= word.candidates_.GetSize());

    Randomize(constraints, weights, noise);

    size_t remaining(word.candidates_.GetSize());
    _repeat (remaining)
        candidates[_index] = &word.candidates_[_index];

    _rforeach (Weights, weight, weights) {
        if (remaining == 1)
            break;

        unsigned minimal(_not(unsigned));

        {
            OPT_TIMER("Execute()[Minimal]")

            _repeat (remaining) {
                unsigned violations(candidates[_index]->violations_[weight->constraint_]);
                if (minimal > violations)
                    minimal = violations;
            }
        }

        {
            OPT_TIMER("Execute()[Swap]")

            for (size_t i(0); i != remaining; ++i)
                if (candidates[i]->violations_[weight->constraint_] > minimal)
                    ext::Swap(candidates[i--], candidates[--remaining]);
        }
    }

    _assert(remaining == 1);
    return candidates[0];
}

#if 0
static const unsigned Repetitions_(21000);

double GetPlasticity(unsigned index, size_t words) {
    if (index < Repetitions_ * 1/3)
        return 2;
    else if (index < Repetitions_ * 2/3)
        return 0.2;
    else
        return 0.02;
}

double GetNoise(unsigned index, size_t words) {
    if (index < Repetitions_ * 1/3)
        return 10;
    else if (index < Repetitions_ * 2/3)
        return 2;
    else
        return 2;
}
#elif 0
static const unsigned Repetitions_(20000000);

double GetPlasticity(unsigned index, size_t words) {
    return 2.0 / pow(10, index / (words * 1000));
}

double GetNoise(unsigned index, size_t words) {
    return index < words * 1000 ? 10 : 2;
}

#elif 1
static const unsigned Repetitions_(20000000);

double GetPlasticity(unsigned index, size_t words) {
    return 4 / exp(15 * double(index) / Repetitions_);
}

double GetNoise(unsigned index, size_t words) {
    return 2;
}
#endif

void Show(const Constraints &constraints) {
    api::Cout << "Constriants:" << ios::NewLine;
    _rforeach (const Constraints, constraint, constraints)
        api::Cout << "  " << constraint->name_ << " = " << constraint->mean_ << ios::NewLine;
    api::Cout << ios::NewLine;
}

int Main(const app::Options &options) {
    random_.SetSeed(UINT64_C(0xdeadbeeff00dcafe));

    _assert(options.GetArguments().GetSize() == 1);
    _S<api::FileReader> fin(options.GetArguments()[0]);

    ext::Buffer line;
    _assert(ios::ReadLine(fin, line));

    Constraints constraints(lexical_cast<size_t>(cse::String(line)));
    Constraints::Iterator c = constraints.Begin();
    _foreach (Constraints, constraint, constraints) {
        _assert(ios::ReadLine(fin, line));
        constraint->name_ = line;
        constraint->mean_ = double(100);
    }

    _assert(ios::ReadLine(fin, line));

    size_t maximal(0);

    Words words(lexical_cast<size_t>(cse::String(line)));
    _foreach (Words, word, words) {
        _assert(ios::ReadLine(fin, line));
        word->name_ = line;

        _assert(ios::ReadLine(fin, line));

        size_t candidates(lexical_cast<size_t>(cse::String(line)));
        if (maximal < candidates)
            maximal = candidates;

        word->candidates_.Reallocate(candidates);

        _foreach (Candidates, candidate, word->candidates_) {
            ios::ReadUntil(fin, '/', line);
            unsigned numerator(lexical_cast<unsigned>(cse::String(line)));

            ios::ReadUntil(fin, ' ', line);
            unsigned denominator(lexical_cast<unsigned>(cse::String(line)));

            candidate->probability_ = double(numerator) / double(denominator);

            candidate->violations_.Reallocate(constraints.GetSize());
            _foreach (Violations, violation, candidate->violations_) {
                ios::ReadUntil(fin, ' ', line);
                *violation = lexical_cast<unsigned>(cse::String(line));
            }

            _assert(ios::ReadLine(fin, line));
            candidate->name_ = line;
        }
    }

    candidates.Reallocate(maximal);

    Weights weights(constraints.GetSize());
    _foreach (Weights, weight, weights)
        weight->constraint_ = _index;

    double noise;

    _repeat (Repetitions_) {
        double plasticity(GetPlasticity(_index, words.GetSize()));
        noise = GetNoise(_index, words.GetSize());

        _assert(finite(plasticity));
        _assert(finite(noise));

        size_t windex;
        do windex = size_t(floor(GetRandom() * words.GetSize()));
        _until (windex < words.GetSize());

        if (windex >= words.GetSize()) {
            api::Cout << windex << " " << words.GetSize() << ios::NewLine;
            _assert(false);
        }

        Word &word(words[windex]);
        ++word.used_;

        const Candidate *output(Execute(constraints, weights, noise, word));

        Candidate *datum(NULL);
        double cindex(GetRandom());
        _foreach (Candidates, candidate, word.candidates_)
            if (cindex > candidate->probability_)
                cindex -= candidate->probability_;
            else {
                datum = candidate;
                break;
            }

        _assert(datum != NULL);

        ++datum->used_;

        if (output != datum)
            _foreach (Constraints, constraint, constraints)
                if (output->violations_[_index] < datum->violations_[_index])
                    constraint->mean_ -= plasticity;
                else if (output->violations_[_index] > datum->violations_[_index])
                    constraint->mean_ += plasticity;

        if (_index % 10000 == 0) {
            api::Cout << "i:" << _index << " p:" << plasticity << " n:" << noise << " W:" << word.name_ << " D:" << datum->name_ << " O:" << output->name_ << ios::NewLine;
            Show(constraints);
        }
    }

    {
        Constraints sorted(constraints.GetSize());
        ext::FloatingRadixSort(constraints.Begin(), constraints.End(), sorted.Begin(), GetMean());
        Show(sorted);
    }

    _foreach (const Words, word, words) {
        ext::Block<unsigned> counts(word->candidates_.GetSize());
        ext::FillAssign(counts.Begin(), counts.End(), 0);

        static const unsigned Verifies_(1000000);

        _repeat (Verifies_)
            ++counts[Execute(constraints, weights, noise, *word) - word->candidates_.Begin()];

        api::Cout << word->name_ << " (" << (word->used_ / double(Repetitions_) * 100) << "%):" << ios::NewLine;
        _foreach (const Candidates, candidate, word->candidates_)
            api::Cout << "  " << candidate->name_ << " (" << (candidate->used_ / double(word->used_) * 100) << "%) = " << (counts[_index] / double(Verifies_) * 100) << "% (" << (candidate->probability_ * 100) << "%)" << ios::NewLine;
        api::Cout << ios::NewLine;
    }

    return 0;
}
