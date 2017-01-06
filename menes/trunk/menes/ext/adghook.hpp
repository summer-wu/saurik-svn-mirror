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

#ifndef MENES_EXT_ADGHOOK_HPP
#define MENES_EXT_ADGHOOK_HPP

#include "cxx/platform.hpp"
#include "ext/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

namespace ext {

template <typename Graph_>
class ExplicitEdge;

template <typename Graph_>
class ImplicitEdge {
    friend class ExplicitEdge<Graph_>;

  private:
    typedef ImplicitEdge<Graph_> This_;

  public:
    typedef typename Graph_::ImplicitNode ImplicitNode;
    typedef typename Graph_::ExplicitNode ExplicitNode;

    typedef ext::RedBlackMap<ExplicitNode *, unsigned> ExplicitNodeSet;

  private:
    ExplicitNodeSet edges_;

  public:
    ImplicitEdge() {
    }

    ImplicitEdge(const This_ &rhs) {
        // this does nothing on purpose
    }

    ~ImplicitEdge() {
        ImplicitNode *this_(static_cast<ImplicitNode *>(this));

        _tforeach (ExplicitNodeSet, edge, edges_)
            _repeat (edge->Second())
                edge->First()->ExplicitEdge<Graph_>::RemoveEdge_(this_);
    }

    This_ &operator =(const This_ &rhs) {
        // this does nothing on purpose
        return *this;
    }

    const ExplicitNodeSet &GetExplicit() const {
        return edges_;
    }

    void Insert(ExplicitNode *node) {
        ++edges_[node];
    }

    void Remove(ExplicitNode *node) {
        typename ExplicitNodeSet::Iterator count(edges_.Find(node));
        _assert(count != edges_.End());
        if (--count->Second() == 0)
            // XXX: this should be RemoveAt(count)
            edges_.Remove(node);
    }
};

template <typename Graph_>
class ExplicitEdge {
    friend class ImplicitEdge<Graph_>;

  private:
    typedef ExplicitEdge<Graph_> This_;

  public:
    typedef typename Graph_::ImplicitNode ImplicitNode;
    typedef typename Graph_::ExplicitNode ExplicitNode;

    typedef ext::Vector<ImplicitNode *> ImplicitNodeList;

  private:
    ImplicitNodeList edges_;

    void RemoveEdge_(ImplicitNode *node) {
        typename ImplicitNodeList::Iterator implicit(FindFirst(edges_, node));
        _assert(implicit != edges_.End());
        *implicit = NULL;
    }

  protected:
    ExplicitEdge() {
    }

    ExplicitEdge(const This_ &rhs) {
        *this = rhs;
    }

    ~ExplicitEdge() {
        size_t size(GetImplicit().GetSize());
        for (size_t i(0); i != size; ++i)
            SetEdge(i, NULL);
    }

    This_ &operator =(const This_ &rhs) {
        size_t size(rhs.GetImplicit().GetSize());
        SetSize(size);

        for (size_t i(0); i != size; ++i)
            SetEdge(i, rhs.GetImplicit()[i]);
        return *this;
    }

  public:
    const ImplicitNodeList &GetImplicit() const {
        return edges_;
    }

    void SetEdge(size_t index, ImplicitNode *node) {
        ExplicitNode *this_(static_cast<ExplicitNode *>(this));

        ImplicitNode *&old = edges_[index];
        if (old != NULL)
            old->ImplicitEdge<Graph_>::Remove(this_);

        old = node;
        if (node != NULL)
            node->ImplicitEdge<Graph_>::Insert(this_);
    }

    void InsertEdge(ImplicitNode *node) {
        ExplicitNode *this_(static_cast<ExplicitNode *>(this));

        edges_.InsertLast(node);
        if (node != NULL)
            node->ImplicitEdge<Graph_>::Insert(this_);
    }

    void RemoveEdge(ImplicitNode *node) {
        ExplicitNode *this_(static_cast<ExplicitNode *>(this));
        RemoveEdge_(node);
        if (node != NULL)
            node->ImplicitEdge<Graph_>::Remove(this_);
    }

    void SetSize(size_t size) {
        edges_.SetSize(size);
    }
};

template <typename Graph_>
class Edge :
    public ImplicitEdge<Graph_>,
    public ExplicitEdge<Graph_>
{
};

}

#endif//MENES_EXT_ADGHOOK_HPP
