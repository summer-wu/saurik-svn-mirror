/* Ramsey - Ramsey Graph Counter Example Generator
 * Copyright (C) 2004-2007  Jay Freeman (saurik)
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

#include "api/exename.hpp"
#include "api/zlib/deflate.hpp"

#include "app/simple.hpp"
#include "enc/encode/base64.hpp"
#include "ext/bitarray.hpp"
#include "ios/string.hpp"

#include <cmath>
#include <cstdlib>
#include <iostream>

#include <gtk/gtk.h>

typedef unsigned Person;

static const size_t GraphSize = 103;
static const unsigned CliqueSize = 6;

typedef ext::BitArray<GraphSize> People;

template <typename Data_>
class DataGraph {
  private:
    Data_ data_[GraphSize][GraphSize];

  public:
    _finline DataGraph() {
    }

    _finline void Reset() {
        _forall (size_t, i, 0, GraphSize)
            _forall (size_t, j, i + 1, GraphSize)
                data_[i][j] = 0;
    }

    _finline const Data_ &operator ()(Person from, Person to) const {
        return data_[from][to];
    }

    _finline Data_ &operator ()(Person from, Person to) {
        return data_[from][to];
    }

    _finline const Data_ &Get(Person from, Person to) const {
        return from < to ? operator ()(from, to) : operator ()(to, from);
    }

    _finline Data_ &Get(Person from, Person to) {
        return from < to ? operator ()(from, to) : operator ()(to, from);
    }

    void Output(ios::PrintWriter &out) const {
        _forall (size_t, i, 0, GraphSize)
            _forall (size_t, j, i + 1, GraphSize)
                out << (data_[i][j] ? "1" : "0");
    }

    _finline size_t GetSize() const {
        return GraphSize;
    }
};

typedef DataGraph<bool> FullGraph;
typedef DataGraph<int> CountGraph;

ios::PrintWriter &operator <<(ios::PrintWriter &lhs, const FullGraph &rhs) {
    /*_S<enc::Base64Encoder> base64(&lhs);
    _S<api::Z::DeflateWriter> deflate(&base64);

    byte_t data(0);
    unsigned count(0);

    _forall (size_t, i, 0, rhs.GetSize())
        _forall (size_t, j, i + 1, rhs.GetSize()) {
            data = (data << 1) + rhs(i, j) ? 1 : 0;
            if (++count == sizeof(byte_t)) {
                deflate.Put<byte_t>(data);
                data = 0;
                count = 0;
            }
        }
    return lhs;*/

    _forall (size_t, i, 1, rhs.GetSize()) {
        if (i != 1)
            lhs << "|";
        _forall (size_t, j, 0, i)
            lhs << (rhs(j, i) ? "1" : "0");
    }
    return lhs;
}

class CyclicGraph {
  private:
    bool data_[GraphSize];

  public:
    _finline CyclicGraph() {
    }

    _finline void Reset() {
        _forall (size_t, i, 0, GraphSize)
            data_[i] = false;
    }

    _finline bool operator ()(Person from, Person to) const {
        return operator [](to - from);
    }

    _finline bool operator [](size_t index) const {
        return data_[index];
    }

    _finline bool &operator [](size_t index) {
        return data_[index];
    }

    _finline size_t GetSize() const {
        return GraphSize;
    }
};

ios::PrintWriter &operator <<(ios::PrintWriter &lhs, const CyclicGraph &rhs) {
    _forall (size_t, i, 1, rhs.GetSize() / 2 + 1)
        lhs << (rhs[i] ? "1" : "0");
    return lhs;
}

template <typename Graph_>
void Display(ios::PrintWriter &out, const Graph_ &rhs, size_t size = ~size_t()) {
    if (size == ~size_t())
        size = rhs.GetSize();

    out << size << ios::NewLine << " ";
    _forall (int, i, 0, size)
        out << (i % 10);

    _forall (Person, i, 0, size) {
        out << ios::NewLine << (i % 10);
        _forall (Person, j, 0, size) {
            if (j <= i)
                out << " ";
            else if (rhs(i, j))
                out << ":";
            else
                out << "0";
        }
    }

    out << ios::NewLine;
}

// XXX: template the hell out of this to work with any size graph
template <bool Clique_>
bool HasCliquesWithLast(const FullGraph &graph, size_t limit = GraphSize) {
    OPT_TIMER_GROUP("HasCliquesWithLast()", Clique_ ? "true" : "false")

    --limit;

    _forall (size_t, i1, 0, limit) if (
        graph(i1, limit) == Clique_
    ) if (CliqueSize <= 2)
        return true;
    else _forall (size_t, i2, i1 + 1, limit) if (
        graph(i1, i2) == Clique_ &&
        graph(i2, limit) == Clique_
    ) if (CliqueSize <= 3)
        return true;
    else _forall (size_t, i3, i2 + 1, limit) if (
        graph(i1, i3) == Clique_ &&
        graph(i2, i3) == Clique_ &&
        graph(i3, limit) == Clique_
    ) if (CliqueSize <= 4)
        return true;
    else _forall (size_t, i4, i3 + 1, limit) if (
        graph(i1, i4) == Clique_ &&
        graph(i2, i4) == Clique_ &&
        graph(i3, i4) == Clique_ &&
        graph(i4, limit) == Clique_
    ) if (CliqueSize <= 5)
        return true;
    else _forall (size_t, i5, i4 + 1, limit) if (
        graph(i1, i5) == Clique_ &&
        graph(i2, i5) == Clique_ &&
        graph(i3, i5) == Clique_ &&
        graph(i4, i5) == Clique_ &&
        graph(i5, limit) == Clique_
    ) if (CliqueSize <= 6)
        return true;
    else _assert(false);

    return false;
}

// XXX: template the hell out of this to work with any size graph
template <bool Clique_>
bool HasCliquesWithThisBaseAndWithLast(const FullGraph &graph, size_t i1, size_t limit = GraphSize) {
    OPT_TIMER_GROUP("HasCliquesWithThisBaseAndWithLast()", Clique_ ? "true" : "false")

    --limit;

    _forall (size_t, i2, 0, i1) if (
        graph(i2, i1) == Clique_ &&
        graph(i2, limit) == Clique_
    ) if (CliqueSize <= 3)
        return true;
    else _forall (size_t, i3, i2 + 1, i1) if (
        graph(i3, i1) == Clique_ &&
        graph(i2, i3) == Clique_ &&
        graph(i3, limit) == Clique_
    ) if (CliqueSize <= 4)
        return true;
    else _forall (size_t, i4, i3 + 1, i1) if (
        graph(i4, i1) == Clique_ &&
        graph(i2, i4) == Clique_ &&
        graph(i3, i4) == Clique_ &&
        graph(i4, limit) == Clique_
    ) if (CliqueSize <= 5)
        return true;
    else _forall (size_t, i5, i4 + 1, i1) if (
        graph(i5, i1) == Clique_ &&
        graph(i2, i5) == Clique_ &&
        graph(i3, i5) == Clique_ &&
        graph(i4, i5) == Clique_ &&
        graph(i5, limit) == Clique_
    ) if (CliqueSize <= 6)
        return true;
    else _assert(false);

    return false;
}

// XXX: template the hell out of this to work with any size graph
template <bool Clique_, typename Graph_>
unsigned CountCliquesWithConflict(const Graph_ &graph, CountGraph &conflict, size_t limit = GraphSize) {
    unsigned cliques(0);

    _forall (size_t, i0, 0, limit)
    if (CliqueSize <= 1) {
        ++cliques;
    } else _forall (size_t, i1, i0 + 1, limit) if (
        graph(i0, i1) == Clique_
    ) if (CliqueSize <= 2) {
        ++conflict(i0, i1);
        ++cliques;
    } else _forall (size_t, i2, i1 + 1, limit) if (
        graph(i0, i2) == Clique_ &&
        graph(i1, i2) == Clique_
    ) if (CliqueSize <= 3) {
        ++conflict(i0, i1);
        ++conflict(i0, i2);
        ++conflict(i1, i2);
        ++cliques;
    } else _forall (size_t, i3, i2 + 1, limit) if (
        graph(i0, i3) == Clique_ &&
        graph(i1, i3) == Clique_ &&
        graph(i2, i3) == Clique_
    ) if (CliqueSize <= 4) {
        ++conflict(i0, i1);
        ++conflict(i0, i2);
        ++conflict(i1, i2);
        ++conflict(i0, i3);
        ++conflict(i1, i3);
        ++conflict(i2, i3);
        ++cliques;
    } else _forall (size_t, i4, i3 + 1, limit) if (
        graph(i0, i4) == Clique_ &&
        graph(i1, i4) == Clique_ &&
        graph(i2, i4) == Clique_ &&
        graph(i3, i4) == Clique_
    ) if (CliqueSize <= 5) {
        ++conflict(i0, i1);
        ++conflict(i0, i2);
        ++conflict(i1, i2);
        ++conflict(i0, i3);
        ++conflict(i1, i3);
        ++conflict(i2, i3);
        ++conflict(i0, i4);
        ++conflict(i1, i4);
        ++conflict(i2, i4);
        ++conflict(i3, i4);
        ++cliques;
    } else _forall (size_t, i5, i4 + 1, limit) if (
        graph(i0, i5) == Clique_ &&
        graph(i1, i5) == Clique_ &&
        graph(i2, i5) == Clique_ &&
        graph(i3, i5) == Clique_ &&
        graph(i4, i5) == Clique_
    ) if (CliqueSize <= 6) {
        ++conflict(i0, i1);
        ++conflict(i0, i2);
        ++conflict(i1, i2);
        ++conflict(i0, i3);
        ++conflict(i1, i3);
        ++conflict(i2, i3);
        ++conflict(i0, i4);
        ++conflict(i1, i4);
        ++conflict(i2, i4);
        ++conflict(i3, i4);
        ++conflict(i0, i5);
        ++conflict(i1, i5);
        ++conflict(i2, i5);
        ++conflict(i3, i5);
        ++conflict(i4, i5);
        ++cliques;
    } else _assert(false);

    return cliques;
}

// XXX: template the hell out of this to work with any size graph
template <bool Clique_, typename Graph_>
unsigned CountCliques(const Graph_ &graph, size_t limit = GraphSize) {
    unsigned cliques(0);

    _forall (size_t, i0, 0, limit)
    if (CliqueSize <= 1)
        ++cliques;
    else _forall (size_t, i1, i0 + 1, limit) if (
        graph(i0, i1) == Clique_
    ) if (CliqueSize <= 2)
        ++cliques;
    else _forall (size_t, i2, i1 + 1, limit) if (
        graph(i0, i2) == Clique_ &&
        graph(i1, i2) == Clique_
    ) if (CliqueSize <= 3)
        ++cliques;
    else _forall (size_t, i3, i2 + 1, limit) if (
        graph(i0, i3) == Clique_ &&
        graph(i1, i3) == Clique_ &&
        graph(i2, i3) == Clique_
    ) if (CliqueSize <= 4)
        ++cliques;
    else _forall (size_t, i4, i3 + 1, limit) if (
        graph(i0, i4) == Clique_ &&
        graph(i1, i4) == Clique_ &&
        graph(i2, i4) == Clique_ &&
        graph(i3, i4) == Clique_
    ) if (CliqueSize <= 5)
        ++cliques;
    else _forall (size_t, i5, i4 + 1, limit) if (
        graph(i0, i5) == Clique_ &&
        graph(i1, i5) == Clique_ &&
        graph(i2, i5) == Clique_ &&
        graph(i3, i5) == Clique_ &&
        graph(i4, i5) == Clique_
    ) if (CliqueSize <= 6)
        ++cliques;
    else _assert(false);

    return cliques;
}

// XXX: template the hell out of this to work with any size graph
template <bool Clique_>
bool HasCliques(const FullGraph &graph, size_t limit = GraphSize) {
    _forall (size_t, i0, 0, limit)
    if (CliqueSize <= 1)
        return true;
    else _forall (size_t, i1, i0 + 1, limit) if (
        graph(i0, i1) == Clique_
    ) if (CliqueSize <= 2)
        return true;
    else _forall (size_t, i2, i1 + 1, limit) if (
        graph(i0, i2) == Clique_ &&
        graph(i1, i2) == Clique_
    ) if (CliqueSize <= 3)
        return true;
    else _forall (size_t, i3, i2 + 1, limit) if (
        graph(i0, i3) == Clique_ &&
        graph(i1, i3) == Clique_ &&
        graph(i2, i3) == Clique_
    ) if (CliqueSize <= 4)
        return true;
    else _forall (size_t, i4, i3 + 1, limit) if (
        graph(i0, i4) == Clique_ &&
        graph(i1, i4) == Clique_ &&
        graph(i2, i4) == Clique_ &&
        graph(i3, i4) == Clique_
    ) if (CliqueSize <= 5)
        return true;
    else _forall (size_t, i5, i4 + 1, limit) if (
        graph(i0, i5) == Clique_ &&
        graph(i1, i5) == Clique_ &&
        graph(i2, i5) == Clique_ &&
        graph(i3, i5) == Clique_ &&
        graph(i4, i5) == Clique_
    ) if (CliqueSize <= 6)
        return true;
    else _assert(false);

    return false;
}

// XXX: template the hell out of this to work with any size graph
template <bool Clique_>
bool HasCliques(const CyclicGraph &graph, size_t limit = GraphSize) {
    _forall (size_t, i1, 1, limit) if (
        graph(0, i1) == Clique_
    ) if (CliqueSize <= 2)
        return true;
    else _forall (size_t, i2, i1 + 1, limit) if (
        graph(0, i2) == Clique_ &&
        graph(i1, i2) == Clique_
    ) if (CliqueSize <= 3)
        return true;
    else _forall (size_t, i3, i2 + 1, limit) if (
        graph(0, i3) == Clique_ &&
        graph(i1, i3) == Clique_ &&
        graph(i2, i3) == Clique_
    ) if (CliqueSize <= 4)
        return true;
    else _forall (size_t, i4, i3 + 1, limit) if (
        graph(0, i4) == Clique_ &&
        graph(i1, i4) == Clique_ &&
        graph(i2, i4) == Clique_ &&
        graph(i3, i4) == Clique_
    ) if (CliqueSize <= 5)
        return true;
    else _forall (size_t, i5, i4 + 1, limit) if (
        graph(0, i5) == Clique_ &&
        graph(i1, i5) == Clique_ &&
        graph(i2, i5) == Clique_ &&
        graph(i3, i5) == Clique_ &&
        graph(i4, i5) == Clique_
    ) if (CliqueSize <= 6)
        return true;
    else _assert(false);

    return false;
}

template <typename Graph_>
bool Check(const Graph_ &graph) {
    return !HasCliques<true>(graph) && !HasCliques<false>(graph);
}

namespace CyclicSearch {

unsigned checked_(0);
unsigned skipped_(0);

static const unsigned Slots_(GraphSize / 2);
static const unsigned Ticks_(Slots_ / 2);

bool resume_(false);
unsigned starters_[Ticks_] = {0};

bool stop_(false);
unsigned stoppers_[Ticks_] = {0};

double factorial_[Slots_ + 1];
double total_;

double nCr(int n, int r) {
    return r > n ? 0 : factorial_[n] / (factorial_[n - r] * factorial_[r]);
}

void SetupFactorial() {
    factorial_[0] = 1;
    _forall (unsigned, i, 0, Slots_)
        factorial_[i + 1] = factorial_[i] * (i + 1);

    total_ = nCr(Slots_ - 1, Ticks_ - 1);
}

double Complete(const CyclicGraph &graph) {
    double remain(0);

    unsigned ticks(Ticks_);
    unsigned last(0);

    _forall (unsigned, i, 2, Slots_ + 1)
        if (graph[i]) {
            last = i;
            --ticks;
            remain += nCr(Slots_ - i, ticks);
        }

    remain += nCr(Slots_ - last - 1, ticks);

    return (1 - remain / total_) * 100;
}

bool Search(CyclicGraph &graph, unsigned start = 1, unsigned ticks = Ticks_ - 1) {
    if (stop_) {
        unsigned *stopper(stoppers_);
        _forall (unsigned, i, 1, start + 1) {
            if (graph[i]) {
                if (i <= *stopper)
                    break;
                ++stopper;
            } else if (i == *stopper + 1)
                return false;
        }

        if (stopper == stoppers_ + Ticks_)
            return false;
    }

    if (ticks == 0) {
        resume_ = false;

        if (Check(graph)) {
            api::Cout << "W(..,..):" << graph << ios::NewLine;
            Display(api::Cout, graph);
            exit(0);
        }

        if ((++checked_ & 0x1fff) == 0)
            api::Cout << "C(  ,  ):" << graph << ios::NewLine;

        if (checked_ == ~unsigned())
            return false;
    } else {
        unsigned placed(Ticks_ - ticks);
        size_t people(start + 1);
        unsigned remain(Slots_ - start);

        static const unsigned filter_[] = {5, 7, 7, 7, 8, 9};

        if (
            remain - CliqueSize < sizeof(filter_) / sizeof(filter_[0]) && (
                ticks >= filter_[remain - CliqueSize] ||
                remain - ticks >= filter_[remain - CliqueSize]
            ) ||
            placed >= CliqueSize && HasCliques<true>(graph) ||
            people - placed >= CliqueSize && HasCliques<false>(graph, people)
        ) {
            if ((++skipped_ & 0x1fff) == 0)
                api::Cout << "S(" << ios::SetWidth(2) << ticks << "," << ios::SetWidth(2) << people << "):" << graph << ios::NewLine;

            resume_ = false;
            return true;
        }

        if (resume_)
            start = starters_[placed];

        _forall (unsigned, i, start, Slots_ - (ticks - 1)) {
            graph[i + 1] = true;
            graph[GraphSize - (i + 1)] = true;

            if (!Search(graph, i + 1, ticks - 1))
                return false;

            graph[GraphSize - (i + 1)] = false;
            graph[i + 1] = false;
        }
    }

    return true;
}

bool Parse(unsigned values[Ticks_], const cse::String &starters) {
    if (starters.GetSize() != Slots_) {
        api::Cerr << "resume string wrong size" << ios::NewLine;
        return false;
    }

    unsigned j(0);

    _forall (unsigned, i, 0, Slots_)
        switch (starters[i]) {
            default:
                api::Cerr << "invalid resume string character" << ios::NewLine;
                return false;
            case '1':
                values[j++] = i;
            case '0':
                ;
        }

    return true;
}

int Main(const app::Options &options) {
    if (options.GetArguments().GetSize() > 0) {
        resume_ = true;
        if (!Parse(starters_, options.GetArguments()[0]))
            return -1;
    }

    if (options.GetArguments().GetSize() > 1) {
        stop_ = true;
        if (!Parse(stoppers_, options.GetArguments()[1]))
            return -1;
    }

    if (options.GetArguments().GetSize() > 2) {
        api::Cerr << "wrong number of arguments" << ios::NewLine;
        return -1;
    }

    SetupFactorial();

    CyclicGraph graph;
    graph.Reset();
    graph[1] = true;
    graph[GraphSize - 1] = true;

    Search(graph);

    if (stop_)
        api::Cout << "F(..,..):" << options.GetArguments()[1] << ios::NewLine;
    else {
        api::Cout << "F(..,..):";

        api::Cout << "1";
        _forall (unsigned, i, 0, Slots_ - Ticks_)
            api::Cout << "0";
        _forall (unsigned, i, 0, Ticks_ - 1)
            api::Cout << "1";
        api::Cout << ios::NewLine;
    }

    return 0;
}

}

namespace FullSearch {

unsigned checked_(0);
unsigned highest_(1);

unsigned rows_[GraphSize] = {0};

bool Search(FullGraph &graph, unsigned column, unsigned row, unsigned ticks, int yes, int no) {
    if (ticks == 0) {
        if ((++checked_ & 0x1fffff) == 0)
            api::Cout << "C:" << graph << ios::NewLine;

        ++column;

        if (
            !HasCliquesWithLast<true>(graph, column) &&
            !HasCliquesWithLast<false>(graph, column)
        ) {
            if (highest_ < column) {
                highest_ = column;
                Display(api::Cout, graph, column);
            }

            if (column == GraphSize)
                return true;

            /*if ((column % 2) != 1)
                return Search(graph, column, 0, column / 2, yes, no);
            else {*/
                if (yes <= no && Search(graph, column, 0, column / 2 + 1, yes, no))
                    return true;
                if (yes >= no && Search(graph, column, 0, column / 2, yes, no))
                    return true;
                return false;
            //}
        }
    } else if (row != column) {
        //if (rows_[row] >= (column - row) / 2)
            if (Search(graph, column, row + 1, ticks, yes, no + 1))
                return true;

        //if (rows_[row] <= (column - row) / 2) {
            ++rows_[row];
            graph(row, column) = true;
            if (Search(graph, column, row + 1, ticks - 1, yes + 1, no))
                return true;
            graph(row, column) = false;
            --rows_[row];
        //}
    }

    return false;
}

bool Search2(FullGraph &graph, unsigned column = 1, unsigned row = 0, unsigned ticks = 0, bool borrow = true) {
    if ((++checked_ & 0x1fffff) == 0)
        api::Cout << "C:" << graph << ios::NewLine;

    if (ticks != 0) {
        if (!HasCliquesWithThisBaseAndWithLast<true>(graph, row, column + 1)) {
            graph(row, column) = true;
            if (Search2(graph, column, row + 1, ticks - 1))
                return true;
            graph(row, column) = false;
        }

        if (column - row > ticks)
            if (Search2(graph, column, row + 1, ticks, borrow))
                return true;
    } else {
        unsigned next(column + 1);

        if (!HasCliquesWithLast<false>(graph, next)) {
            if (highest_ < next) {
                highest_ = next;
                Display(api::Cout, graph, next);

                if (next == GraphSize)
                    return true;
            }

            if (next % 2 == 0) {
                if (Search2(graph, next, 0, next / 2, borrow))
                    return true;
            } else if (borrow) {
                if (Search2(graph, next, 0, next / 2, false))
                    return true;
            } else {
                if (Search2(graph, next, 0, next / 2 + 1, true))
                    return true;
            }
        }
    }

    return false;
}

int Main(const app::Options &options) {
    FullGraph graph;
    graph.Reset();

    rows_[0] = 1;
    graph(0, 1) = true;

    if (Search(graph, 1, 0, 0, 1, 0))
        _assert(Check(graph));
    Search2(graph);
    return 0;
}

}

namespace ManualSearch {

FullGraph graph_;
GtkWidget *cliques_[2];
GtkWidget *flats_[2];

GdkColor conflict_[10];
GdkColor waiting_[10];

GdkColor bgnormal_;
GdkColor fgnormal_;

GtkWidget *toggles_[GraphSize][GraphSize];

GtkWidget *usage_;
unsigned filled_(0);
unsigned total_(0);

GtkWidget *updation_;
unsigned update_(0);

typedef etl::Pair<unsigned, unsigned> Coordinate;

void UpdateBase() {
    ::gtk_label_set_text(GTK_LABEL(usage_), (_S<ios::String>() << (filled_ * 100 / total_) << "%").NullTerminate());
}

void Update0() {
    UpdateBase();

    CountGraph conflict;
    conflict.Reset();

    ::gtk_label_set_text(GTK_LABEL(cliques_[0]), lexical_cast<cse::String>(CountCliquesWithConflict<false>(graph_, conflict)).NullTerminate());

    _forall (size_t, i, 0, GraphSize)
        _forall (size_t, j, i + 1, GraphSize)
            conflict(i, j) = -conflict(i, j);

    ::gtk_label_set_text(GTK_LABEL(cliques_[1]), lexical_cast<cse::String>(CountCliquesWithConflict<true>(graph_, conflict)).NullTerminate());

    int maxconflict(0);
    int minconflict(0);

    _forall (size_t, i, 0, GraphSize)
        _forall (size_t, j, i + 1, GraphSize) {
            int conflicted(conflict(i, j));
            if (maxconflict < conflicted)
                maxconflict = conflicted;
            else if (minconflict > conflicted)
                minconflict = conflicted;
        }

    ::gtk_label_set_text(GTK_LABEL(flats_[0]), lexical_cast<cse::String>(-minconflict).NullTerminate());
    ::gtk_label_set_text(GTK_LABEL(flats_[1]), lexical_cast<cse::String>(maxconflict).NullTerminate());

    _forall (size_t, i, 0, GraphSize)
        _forall (size_t, j, i + 1, GraphSize) {
            int confliction(conflict(i, j));
            ::gtk_widget_modify_bg(toggles_[i][j], GTK_STATE_ACTIVE, confliction > 0 ? &conflict_[confliction * 9 / maxconflict] : &bgnormal_);
            ::gtk_widget_modify_bg(toggles_[i][j], GTK_STATE_NORMAL, confliction < 0 ? &waiting_[confliction * 9 / minconflict] : &fgnormal_);
        }
}

void Update1() {
    UpdateBase();

    unsigned bfalse(CountCliques<false>(graph_));
    unsigned btrue(CountCliques<true>(graph_));

    ::gtk_label_set_text(GTK_LABEL(cliques_[0]), lexical_cast<cse::String>(bfalse).NullTerminate());
    ::gtk_label_set_text(GTK_LABEL(cliques_[1]), lexical_cast<cse::String>(btrue).NullTerminate());

    int maxconflict(0);

    CountGraph conflict;
    _forall (size_t, i, 0, GraphSize)
        _forall (size_t, j, i + 1, GraphSize) {
            int confliction;
            if (graph_(i, j)) {
                graph_(i, j) = false;
                confliction = CountCliques<false>(graph_) - bfalse;
                graph_(i, j) = true;
            } else {
                graph_(i, j) = true;
                confliction = CountCliques<true>(graph_) - btrue;
                graph_(i, j) = false;
            }

            if (confliction < 0)
                confliction = 0;

            if (maxconflict < confliction)
                maxconflict = confliction;

            conflict(i, j) = confliction;
        }

    ::gtk_label_set_text(GTK_LABEL(flats_[0]), "");
    ::gtk_label_set_text(GTK_LABEL(flats_[1]), lexical_cast<cse::String>(maxconflict).NullTerminate());

    _forall (size_t, i, 0, GraphSize)
        _forall (size_t, j, i + 1, GraphSize) {
            int confliction(conflict(i, j));
            ::gtk_widget_modify_bg(toggles_[i][j], GTK_STATE_ACTIVE, confliction != 0 ? &conflict_[confliction * 9 / maxconflict] : &bgnormal_);
            ::gtk_widget_modify_bg(toggles_[i][j], GTK_STATE_NORMAL, confliction != 0 ? &waiting_[confliction * 9 / maxconflict] : &fgnormal_);
        }
}

void Update() {
    api::Cout << "updating..." << ios::NewLine;

    switch (update_) {
        case 0: Update0(); break;
        case 1: Update1(); break;
    }

    api::Cout << "updated!" << ios::NewLine;
}

//void Switch(GtkWidget *toggle, void *arg) {
gint Switch(gpointer data) {
    update_ = (update_ + 1) % 2;
    //update_ = *reinterpret_cast<unsigned *>(arg);
    Update();
    return TRUE;
}

bool paused_(false);

void Modify(GtkWidget *toggle, void *arg) {
    if (paused_)
        return;

    Coordinate *coordinate(reinterpret_cast<Coordinate *>(arg));
    api::Cout << coordinate->First() << " " << coordinate->Second() << ios::NewLine;
    bool &value(graph_(coordinate->First(), coordinate->Second()));
    value ? --filled_ : ++filled_;
    value = !value;
    Update();
}

GtkWidget *swapw_(NULL);
unsigned swapi_;

void Swap(GtkWidget *toggle, void *arg) {
    unsigned person(*reinterpret_cast<unsigned *>(arg));

    if (!::gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(toggle))) {
        if (swapi_ == person)
            swapw_ = NULL;
        return;
    }

    api::Cout << "in" << ios::NewLine;

    if (swapw_ == NULL) {
        swapw_ = toggle;
        swapi_ = person;
    } else {
        ::gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(swapw_), FALSE);
        ::gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggle), FALSE);

        if (swapi_ < person)
            ext::Swap(swapi_, person);

        unsigned i0(0), j0(person);
        unsigned i1(0), j1(swapi_);
        bool t0(false), t1(false);

        api::Cout << j0 << " " << j1 << ios::NewLine;

        _forall (size_t, i, 1, GraphSize) {
            restart:

            if (i0 == j0) {
                ++j0;
                t0 = true;
            }

            if (i1 == j1) {
                ++j1;
                t1 = true;
            }

            if (i0 == person && j0 == swapi_) {
                t0 ? ++j0 : ++i0;
                goto restart;
            }

            if (i1 == person && j1 == swapi_) {
                t1 ? ++j1 : ++i1;
                goto restart;
            }

            paused_ = true;
            if (graph_(i0, j0) != graph_(i1, j1)) {
                api::Cout << "(" << i0 << "," << j0 << ") (" << i1 << "," << j1 << ")" << ios::NewLine;
                ext::Swap(graph_(i0, j0), graph_(i1, j1));
                ::gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggles_[i0][j0]), graph_(i0, j0));
                ::gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggles_[i1][j1]), graph_(i1, j1));
            }
            paused_ = false;

            t0 ? ++j0 : ++i0;
            t1 ? ++j1 : ++i1;
        }

        swapw_ = NULL;

        Update();
    }

    api::Cout << "out" << ios::NewLine;
}

void Close(GtkWidget *widget, void *arg) {
    ::gtk_main_quit();
}

int Main(const app::Options &options) {
    {
        int argc(1);
        cse::String exe(api::GetExecutablePath().GetPath());
        const char *args[2] = {exe.NullTerminate(), NULL};
        char **argv(const_cast<char **>(args));
        ::gtk_init(&argc, &argv);
    }

    graph_.Reset();

    GtkWidget *window(::gtk_window_new(GTK_WINDOW_TOPLEVEL));
    ::g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(&Close), NULL);

    GtkWidget *box(::gtk_vbox_new(FALSE, 0));
    ::gtk_container_add(GTK_CONTAINER(window), box);

    ::gdk_color_parse("black", &bgnormal_);
    ::gdk_color_parse("white", &fgnormal_);

    _forall (unsigned, i, 0, 10) {
        conflict_[i].red = 0;
        conflict_[i].green = 65535 * i / 9;
        conflict_[i].blue = 65535;

        waiting_[i].red = 65535;
        waiting_[i].green = 65535 * i / 9;
        waiting_[i].blue = 0;
    }

    conflict_[9].red = 65535 / 4;
    waiting_[9].blue = 65535 / 4;

    GtkWidget *styles(::gtk_hbox_new(FALSE, 0));
    ::gtk_box_pack_start(GTK_BOX(box), styles, FALSE, FALSE, 0);

    /*GSList *group(NULL);

    _forall (unsigned, i, 0, 2) {
        GtkWidget *style(::gtk_radio_button_new(group));
        ::gtk_box_pack_start(GTK_BOX(styles), style, FALSE, FALSE, 0);
        group = ::gtk_radio_button_get_group(GTK_RADIO_BUTTON(style));
        ::gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(style), i == 0);
        ::g_signal_connect(G_OBJECT(style), "clicked", G_CALLBACK(&Switch), new unsigned(i));
    }*/

    ::g_timeout_add(500, &Switch, NULL);

    GtkWidget *swaps(::gtk_hbox_new(FALSE, 0));
    ::gtk_box_pack_start(GTK_BOX(box), swaps, FALSE, FALSE, 0);

    _forall (size_t, i, 0, GraphSize) {
        GtkWidget *toggle(::gtk_toggle_button_new());
        ::gtk_box_pack_start(GTK_BOX(swaps), toggle, FALSE, FALSE, 0);

        ::g_signal_connect(G_OBJECT(toggle), "clicked", G_CALLBACK(&Swap), new unsigned(i));
    }

    _forall (size_t, i, 1, GraphSize) {
        GtkWidget *row(::gtk_hbox_new(FALSE, 0));
        ::gtk_box_pack_start(GTK_BOX(box), row, FALSE, FALSE, 0);

        _rforall (size_t, j, i, GraphSize) {
            GtkWidget *toggle(::gtk_toggle_button_new());
            ::gtk_box_pack_end(GTK_BOX(row), toggle, FALSE, FALSE, 0);

            ::g_signal_connect(G_OBJECT(toggle), "clicked", G_CALLBACK(&Modify), new Coordinate(i - 1, j));
            toggles_[i - 1][j] = toggle;

            ++total_;
        }
    }

    GtkWidget *failed(::gtk_hbox_new(TRUE, 0));
    cliques_[0] = ::gtk_label_new(NULL);
    ::gtk_box_pack_start(GTK_BOX(failed), cliques_[0], FALSE, FALSE, 0);
    cliques_[1] = ::gtk_label_new(NULL);
    ::gtk_box_pack_start(GTK_BOX(failed), cliques_[1], FALSE, FALSE, 0);
    ::gtk_box_pack_start(GTK_BOX(box), failed, FALSE, FALSE, 0);

    usage_ = ::gtk_label_new(NULL);
    ::gtk_box_pack_start(GTK_BOX(box), usage_, FALSE, FALSE, 0);

    GtkWidget *flat(::gtk_hbox_new(TRUE, 0));
    flats_[0] = ::gtk_label_new(NULL);
    ::gtk_box_pack_start(GTK_BOX(flat), flats_[0], FALSE, FALSE, 0);
    flats_[1] = ::gtk_label_new(NULL);
    ::gtk_box_pack_start(GTK_BOX(flat), flats_[1], FALSE, FALSE, 0);
    ::gtk_box_pack_start(GTK_BOX(box), flat, FALSE, FALSE, 0);

    Update();

    ::gtk_widget_show_all(window);
    ::gtk_main();

    return 0;
}

}

namespace DistributedSearch {

using CyclicSearch::nCr;
using CyclicSearch::SetupFactorial;
using CyclicSearch::Slots_;
using CyclicSearch::Ticks_;
using CyclicSearch::total_;

bool graph_[Slots_] = {0};

unsigned splitarity_(1024);
double each_;
double left_;

void Search(unsigned start, unsigned ticks) {
    _assert(ticks != 0);

    _forall (unsigned, i, start, Slots_ - (ticks - 1)) {
        double about(nCr(Slots_ - (i + 1), ticks - 1));

        if (about - 0.5 < left_) {
            if ((left_ -= about) - 0.5 < 0) {
                _forall (unsigned, i, 0, Slots_)
                    api::Cout << (graph_[i] ? "1" : "0");
                api::Cout << ios::NewLine;
                left_ = each_;
            }
        } else {
            graph_[i] = true;
            Search(i + 1, ticks - 1);
            graph_[i] = false;
        }
    }
}

int Main(const app::Options &options) {
    SetupFactorial();
    each_ = std::ceil(total_ / splitarity_);
    left_ = each_;

    _forall (unsigned, i, 0, Ticks_)
        api::Cout << "1";
    _forall (unsigned, i, 0, Slots_ - Ticks_)
        api::Cout << "0";
    api::Cout << ios::NewLine;

    graph_[0] = true;
    Search(1, Ticks_);

    api::Cout << "1";
    _forall (unsigned, i, 0, Slots_ - Ticks_)
        api::Cout << "0";
    _forall (unsigned, i, 0, Ticks_ - 1)
        api::Cout << "1";
    api::Cout << ios::NewLine;

    return 0;
}

}

int Main(const app::Options &options) {
    return CyclicSearch::Main(options);
}
