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

#ifndef MENES_EXT_REDBLACKSET_HPP
#define MENES_EXT_REDBLACKSET_HPP

#include "cxx/platform.hpp"
#include "ext/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "hop/function.hpp"
#include "ext/set.hpp"
#include "ext/singularptr.hpp"

namespace ext {

namespace be {

enum BinaryTreeChild_ {
    BinaryTreeLeft_ = 0,
    BinaryTreeRight_ = 1
};

template <typename Pointer_>
Pointer_ &Farthest_(be::BinaryTreeChild_ child, Pointer_ *node) {
    if (*node == NULL)
        return *node;
    while ((*node)->children_[child] != NULL)
        node = &(*node)->children_[child];
    return *node;
}

template <typename Node_, typename Element_>
class RedBlackSetIterator_ :
    public SmartPointer< Element_, Element_ *, RedBlackSetIterator_<Node_, Element_> >
{
    friend class SmartPointer< Element_, Element_ *, RedBlackSetIterator_<Node_, Element_> >;

  private:
    typedef RedBlackSetIterator_<Node_, Element_> This_;

    Node_ *node_;

    inline Element_ *GetSmartPointer_() const {
        return &node_->value_;
    }

  public:
    RedBlackSetIterator_(Node_ *node) :
        node_(node)
    {
    }

    template <typename NodeF_, typename ElementF_>
    RedBlackSetIterator_(const RedBlackSetIterator_<NodeF_, ElementF_> &rhs) :
        node_(rhs.GetNodePointer())
    {
    }

    Node_ *GetNodePointer() const {
        return node_;
    }

    This_ &operator ++() {
        // XXX: this const is required... it forces Farthest_() to be const-correct
        if (Node_ * const right = node_->children_[be::BinaryTreeRight_])
            node_ = Farthest_(be::BinaryTreeLeft_, &right);
        else {
            while (node_->WhichChild() == be::BinaryTreeRight_)
                node_ = node_->parent_;
            node_ = node_->parent_;
        }

        return *this;
    }

    bool operator ==(const This_ &rhs) const {
        return node_ == rhs.node_;
    }

    _finline bool operator !=(const This_ &rhs) const {
        return !operator ==(rhs);
    }
};

template <typename Element_>
class BinaryTreeValue_;

template <typename Element_>
class BinaryTreeNode_ {
  private:
    typedef BinaryTreeNode_<Element_> This_;
    typedef BinaryTreeValue_<Element_> Derived_;

  public:
    enum Color {
        Red,
        Black
    } color_;

    Derived_ *parent_;
    Derived_ *children_[2];

    BinaryTreeNode_(Derived_ *parent, Color color = Red) :
        color_(color),
        parent_(parent)
    {
        children_[0] = NULL;
        children_[1] = NULL;
    }

    inline BinaryTreeChild_ WhichChild() const {
        _assert(parent_ != NULL);
        return this == parent_->children_[BinaryTreeLeft_] ?
            BinaryTreeLeft_ :
            BinaryTreeRight_;
    }

    Derived_ *&GetSibling() const {
        return parent_->children_[Other_(WhichChild())];
    }

    Color GetColor() const {
        return this == NULL ? Black : color_;
    }

    bool IsBlack() const {
        return this == NULL || color_ == Black;
    }

    bool IsRed() const {
        return this != NULL && color_ == Red;
    }

    unsigned GetSize() const {
        unsigned size(0);
        if (children_[0] != NULL)
            ++size;
        if (children_[1] != NULL)
            ++size;
        return size;
    }
};

template <typename Element_>
class BinaryTreeValue_ :
    public BinaryTreeNode_<Element_>
{
  private:
    typedef BinaryTreeValue_<Element_> This_;
    typedef BinaryTreeNode_<Element_> Base_;

    void CopyChildren(const This_ &rhs, This_ &to, const Base_ &from) {
        Base_::children_[0] = rhs.children_[0]->Copy(this, to, from);
        Base_::children_[1] = rhs.children_[1]->Copy(this, to, from);
    }

  public:
    Element_ value_;

    template <typename Type_>
    BinaryTreeValue_(This_ *parent, const Type_ &value) :
        Base_(parent),
        value_(value)
    {
    }

    BinaryTreeValue_(This_ *parent, const This_ &rhs, This_ &to, const Base_ &from) :
        Base_(parent, rhs.color_),
        value_(rhs.value_)
    {
        CopyChildren(rhs, to, from);
    }

    void TreeParents() {
        if (Base_::children_[0] != NULL)
            Base_::children_[0]->parent_ = this;
        if (Base_::children_[1] != NULL)
            Base_::children_[1]->parent_ = this;
    }

    This_ *Copy(This_ *parent, This_ &to, const Base_ &from) const {
        if (this == NULL)
            return NULL;

        if (parent == &from)
            parent = &to;

        if (this != &from)
            return new This_(parent, *this, to, from);

        to.parent_ = parent;
        to.CopyChildren(*this, to, from);
        return &to;
    }
};

}

template <
    typename Element_,
    typename Key_,
    typename KeyOf_,
    typename LessThan_
> class RedBlackSet :
    public Set<Element_, Key_>
{
  private:
    typedef RedBlackSet<
        Element_,
        Key_,
        KeyOf_,
        LessThan_
    > This_;

    typedef Set<Element_, Key_> Base_;

    typedef be::BinaryTreeNode_<Element_> NodeBase_;
    typedef be::BinaryTreeValue_<Element_> Node_;

  public:
    typedef be::RedBlackSetIterator_<const Node_, const Element_> ConstIterator;
    typedef be::RedBlackSetIterator_<Node_, Element_> Iterator;

  private:
    NodeBase_ stub_;
    Node_ *root_;
    size_t size_;

    _finline static be::BinaryTreeChild_ Other_(be::BinaryTreeChild_ child) {
        return static_cast<be::BinaryTreeChild_>(1 - child);
    }

    void Output_(ios::PrintWriter &out, Node_ *node, unsigned indent = 0) const {
        if (node == NULL)
            out << "[null]";
        else {
            out << "[" << node << "(" << node->parent_ << "):" << (node->color_ == Node_::Black ? "X " : "O ");
            if (node == &stub_)
                out << "stub";
            else
                out << "value_:" << node->value_;

            if (node->GetSize() != 0) {
                out << ios::NewLine;

                _forall (unsigned, i, 0, indent + 1)
                    out << "  ";
                Output_(out, node->children_[be::BinaryTreeLeft_], indent + 1);
                out << ios::NewLine;

                _forall (unsigned, i, 0, indent + 1)
                    out << "  ";
                Output_(out, node->children_[be::BinaryTreeRight_], indent + 1);
                out << ios::NewLine;

                _forall (unsigned, i, 0, indent)
                    out << "  ";
            }

            out << "]";
        }
    }

    etl::Pair<Node_ **, Node_ *> Find_(const Key_ &key) const {
        return const_cast<This_ *>(this)->Find_(key);
    }

    etl::Pair<Node_ **, Node_ *> Find_(const Key_ &key) {
        Node_ **node = &root_, *parent(NULL);
        while (*node != NULL)
            if (*node == &stub_ || LessThan_::Execute(key, KeyOf_::Execute((*node)->value_)))
                node = &(parent = *node)->children_[be::BinaryTreeLeft_];
            else if (LessThan_::Execute(KeyOf_::Execute((*node)->value_), key))
                node = &(parent = *node)->children_[be::BinaryTreeRight_];
            else break;
        return etl::Pair<Node_ **, Node_ *>(node, parent);
    }

    inline static void Rotate_(be::BinaryTreeChild_ child, Node_ *&node) {
        Node_ *&other = node->children_[Other_(child)];
        Node_ *&extra = other->children_[child];

        other->parent_ = node->parent_;
        node->parent_ = other;

        if (extra != NULL)
            extra->parent_ = node;

        ext::Rotate(node, other, extra);
    }

    etl::Pair<Node_ *, bool> Insert_(const Element_ &value) {
        etl::Pair<Node_ **, Node_ *> find(Find_(KeyOf_::Execute(value)));
        if (*find.First() != NULL)
            return etl::Pair<Node_ *, bool>(*find.First(), false);

        ++size_;
        return etl::Pair<Node_ *, bool>(*find.First() = new Node_(find.Second(), value), true);
    }

    Node_ *&GetPointer_(Node_ *node) {
        Node_ *parent(node->parent_);
        if (parent == NULL)
            return root_;
        return parent->children_[node->WhichChild()];
    }

    void Swap_(Node_ *&lhs, Node_ *&rhs) {
        Node_ *left(lhs), *right(rhs);

        ext::Swap(lhs, rhs);
        ext::Swap(*static_cast<NodeBase_ *>(lhs), *static_cast<NodeBase_ *>(rhs));

        left->TreeParents();
        right->TreeParents();
    }

    static void Blacken_(Node_ *node) {
        node->color_ = Node_::Black;
        node->parent_->color_ = Node_::Red;
    }

    void Balance_(be::BinaryTreeChild_ child, Node_ *&node) {
        Node_ *uncle(node->parent_->parent_->children_[Other_(child)]);
        if (uncle->IsBlack()) {
            if (node->WhichChild() != child)
                Rotate_(child, GetPointer_(node = node->parent_));
            Blacken_(node->parent_);
            Rotate_(Other_(child), GetPointer_(node->parent_->parent_));
        } else {
            Blacken_(node->parent_);
            uncle->color_ = Node_::Black;
            node = node->parent_->parent_;
        }
    }

    void Fixup_(be::BinaryTreeChild_ child, Node_ *&node, Node_ *parent) {
        Node_ **sibling = &parent->children_[Other_(child)];

        if ((*sibling)->color_ == Node_::Black) {
            parent->color_ = Node_::Red;

            Node_ *next(parent->parent_);
            Rotate_(child, GetPointer_(parent));
            parent = next;

            sibling = &parent->children_[Other_(child)];
        }

        if (
            (*sibling)->children_[be::BinaryTreeLeft_]->IsBlack() &&
            (*sibling)->children_[be::BinaryTreeRight_]->IsBlack()
        ) {
            (*sibling)->color_ = Node_::Red;
            node = parent;
        } else {
            if ((*sibling)->children_[be::BinaryTreeRight_]->IsBlack()) {
                (*sibling)->children_[be::BinaryTreeLeft_]->color_ = Node_::Black;
                (*sibling)->color_ = Node_::Red;
                Rotate_(Other_(child), *sibling);
                sibling = &node->parent_->children_[Other_(child)];
            }

            (*sibling)->color_ = parent->color_;
            parent->color_ = Node_::Black;
            (*sibling)->children_[be::BinaryTreeRight_]->color_ = Node_::Black;
            Rotate_(child, GetPointer_(parent));

            node = root_;
        }
    }

    void Delete_(Node_ *node) {
        if (node == NULL)
            return;

        Delete_(node->children_[be::BinaryTreeLeft_]);
        Delete_(node->children_[be::BinaryTreeRight_]);

        if (node != &stub_)
            delete node;
    }

  public:
    RedBlackSet() :
        stub_(NULL, Node_::Black),
        root_(static_cast<Node_ *>(&stub_)),
        size_(0)
    {
    }

    // XXX: arguably operator =() should be split to remove duplicate initialization
    RedBlackSet(const This_ &rhs) :
        Base_(),
        stub_(NULL, Node_::Black),
        root_(static_cast<Node_ *>(&stub_)),
        size_(0)
    {
        operator =(rhs);
    }

    ~RedBlackSet() {
        Clear();
    }

    etl::Pair<Iterator, bool> Insert(const Element_ &value) {
        etl::Pair<Node_ *, bool> inserted(Insert_(value));
        if (inserted.Second()) {
            Node_ *node(inserted.First());

            while (
                node->parent_->IsRed() &&
                node->parent_->parent_ != NULL
            )
                Balance_(node->parent_->WhichChild(), node);

            root_->color_ = Node_::Black;
        }

        return inserted;
    }

    ConstIterator Find(const Key_ &key) const {
        Node_ *node(*Find_(key).First());
        return node == NULL ? End() : node;
    }

    Iterator Find(const Key_ &key) {
        Node_ *node(*Find_(key).First());
        return node == NULL ? End() : node;
    }

    bool Remove(const Key_ &key) {
        be::BinaryTreeChild_ child(be::BinaryTreeRight_);
        Node_ *old;

        {
            Node_ *&node = *Find_(key).First();
            if (node == NULL)
                return false;
            old = node;

            if (node->children_[be::BinaryTreeLeft_] != NULL)
                if (Node_ *&right = node->children_[be::BinaryTreeRight_])
                    // XXX: choice is arbitrary (but check child)
                    Swap_(be::Farthest_(be::BinaryTreeLeft_, &right), node);
                else child = be::BinaryTreeLeft_;
        }

        Node_ *node(old->children_[child]);
        be::BinaryTreeChild_ which;

        if (old->parent_ == NULL) {
            // XXX: this assignment is only to remove a warning
            which = be::BinaryTreeLeft_;
            GetPointer_(old) = node;
        } else {
            which = old->WhichChild();
            old->parent_->children_[which] = node;
        }

        if (node != NULL)
            node->parent_ = old->parent_;

        if (old->color_ == Node_::Black)
            if (node->IsRed())
                node->color_ = Node_::Black;
            else {
                if (old->parent_ != NULL) {
                    Fixup_(which, node, old->parent_);

                    while (
                        node->parent_ != NULL &&
                        node->color_ == Node_::Black
                    )
                        Fixup_(node->WhichChild(), node, node->parent_);
                }

                node->color_ = Node_::Black;
            }

        delete old;
        --size_;

        return true;
    }

    ConstIterator Begin() const {
        // XXX: I am starting to hate C++
        This_ *this_(const_cast<This_ *>(this));
        return be::Farthest_(be::BinaryTreeLeft_, &this_->root_);
    }

    Iterator Begin() {
        return be::Farthest_(be::BinaryTreeLeft_, &root_);
    }

    ConstIterator End() const {
        return static_cast<const Node_ *>(&stub_);
    }

    Iterator End() {
        return static_cast<Node_ *>(&stub_);
    }

    size_t GetSize() const {
        return size_;
    }

    bool IsEmpty() const {
        return GetSize() == 0;
    }

    void Clear() {
        Delete_(root_);
        size_ = 0;

        stub_ = NodeBase_(NULL, Node_::Black);
        root_ = static_cast<Node_ *>(&stub_);
    }

    template <typename KeyF_>
    bool Contains(const Key_ &key) const {
        return Find(key) != End();
    }

    virtual bool Contains(const Key_ &key) const {
        return Contains<Key_>(key);
    }

    void Output(ios::PrintWriter &out) const {
        Output_(out, root_);
        out << ios::NewLine;
    }

    This_ &operator =(const This_ &rhs) {
        Clear();
        root_ = rhs.root_->Copy(NULL, static_cast<Node_ &>(stub_), rhs.stub_);
        size_ = rhs.size_;
        return *this;
    }
};

}

namespace etl {
template <typename Element_>
struct IsMemMovable< ext::be::BinaryTreeNode_<Element_> > {
    static const bool value = true;
}; }

namespace etl {
template <typename Element_, typename Key_, typename KeyOf_, typename LessThan_>
struct IsMemMovable< ext::RedBlackSet<Element_, Key_, KeyOf_, LessThan_> > {
    static const bool value = true;
}; }

#define _M ext::RedBlackMap

#endif//MENES_EXT_REDBLACKSET_HPP
