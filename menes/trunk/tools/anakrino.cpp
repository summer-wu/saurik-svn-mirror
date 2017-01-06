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
#include "api/files.hpp"

#include "app/command.hpp"

#include "cse/string.hpp"
#include "ext/vector.hpp"

#include "opt/memory.hpp"
#include "opt/profile.hpp"
#include "opt/reports.hpp"

#include "szf/binary.hpp"
#include "szf/saveload.hpp"
#include "szf/saveload-ext.hpp"

#include <cmath>

double Fix(double value, double fault) {
    return value == 0 ? fault : value;
}

double Get(const ext::Vector<uint32_t> &m) {
    return m[0];
}

double Get(const ext::Vector<uint32_t> &m, size_t i) {
    return m[i + 1];
}

double Get(const ext::Vector<uint32_t> &m, size_t i, size_t j) {
    return m[i * 256 + j + 256 + 1];
}

double Compare(const ext::Vector<uint32_t> &m, const ext::Vector<uint32_t> &c) {
    OPT_TIMER("Compare()")

    double l(0);

    for (size_t i(0); i != 256; ++i)
        for (size_t j(0); j != 256; ++j)
            if (Get(m, i, j) != 0) {
                //double pc1(Get(c, j)    / Get(c)   );
                double pc2(Get(c, i, j) / Get(c, i));
                double pm1(Get(m, j)    / Get(m)   );
                //double pm2(Get(m, i, j) / Get(m, i));

                l += std::log(Fix(pc2 / pm1, 1 / Get(m)));
            }

    return l;
    //return l < 0 ? 0 : l;
}

app::SimpleCommand<> main_("obcl");

namespace app {
template <>
int SimpleCommand<>::Execute(const app::Options &options) {
    unsigned b(options.GetAsOr<unsigned>("b", 2));

    size_t p(1);
    for (size_t i(0), w(1); i != b; ++i)
        p += (w *= 256);

    ext::Vector<uint32_t> m(p, 0);

    if (const ext::Buffer *load = options["l"]) {
        _S<api::FileReader> fin(*load);
        _S<szf::BinaryMethod> method(&fin, _null<ios::Writer>());
        szf::Load(&method, m, "table");
    }

    {
        OPT_TIMER("AppMain()[Markov]")

        _foreach (const app::ArgumentList, file, options.GetArguments()) {
            _R<ios::Reader> in;

            if (*file == _B("-"))
                in = &api::Cin;
            else
                in = new _H<api::FileReader>(*file);

            try {
                ext::Vector<uint8_t> d(b);
                for (size_t i(b); i != 0; --i)
                    d[i - 1] = ios::Get(*in);

                _forever {
                    ++m[0];

                    for (size_t i(0), o(0), w(1), f(1); i != b; ++i, w *= 256, f += w) {
                        o += d[i] * w;
                        ++m[f + o];
                    }

                    d.RemoveLast();
                    d.InsertFirst(ios::Get(*in));
                }
            } catch (const ext::EosException &) {
            }
        }
    }

    if (const ext::Buffer *output = options["o"]) {
        _S<api::FileWriter> fout(*output);
        _S<szf::BinaryMethod> method(_null<ios::Reader>(), &fout);
        szf::Save(&method, m, "table");
    }

    if (const ext::Buffer *compare = options["c"]) {
        _S<api::FileReader> fin(*compare);
        _S<szf::BinaryMethod> method(&fin, _null<ios::Writer>());
        ext::Vector<uint32_t> c;
        szf::Load(&method, c, "table");
        api::Cout << Compare(m, c) << ios::NewLine;
    }

    return 0;
} }
