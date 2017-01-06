/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2002-2005  Jay Freeman (saurik)
*/

/*
 *        Redistribution and use in source and binary
 * forms, with or without modification, are permitted
 * provided that the following conditions are met:
 *·
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
 *·
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

#ifndef MENES_ETL_VARIANT_HPP
#define MENES_ETL_VARIANT_HPP

#include "cxx/platform.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "etl/alignment.hpp"
#include "etl/vtable.hpp"

#include <typeinfo> //OK

namespace etl {

template <typename Cell_>
class Variant;

namespace be {

template <typename Cell_, typename Tail_>
class VariantType :
    public VariantType<Cell_, typename Tail_::Tail>
{
  private:
    typedef typename Tail_::Head Type_;
    typedef VariantType<Cell_, typename Tail_::Tail> Base_;

    static const unsigned Index_ = Length<Cell_>::value - Length<Tail_>::value;

  public:
    using VariantType<Cell_, typename Tail_::Tail>::Set;

    void Set(const Type_ &value) {
        static TypeVTableImpl<Type_> table;
        Type_ *type(reinterpret_cast<Type_ *>(Base_::data_));

        if (Base_::index_ == Index_)
            *type = value;
        else {
            Base_::Clear();
            Base_::vtable_ = &table;
            Base_::index_ = Index_;
            new (type) Type_(value);
        }
    }

    operator Type_() const {
        _assert(Base_::index_ == Index_);
        const Type_ *type(reinterpret_cast<const Type_ *>(Base_::data_));
        return *type;
    }

    /*template <template <typename Type_> class Apply_>
    inline typename Apply_<void>::Type Apply() {
        return Base_::index_ == Index_ ? &Apply_<Type_>::Execute : Base_::Apply<Apply_>();
    }*/
};

template <typename Cell_>
class VariantType<Cell_, Null> {
  public:
    union {
        char data_[MaxSize<Cell_>::value];
        typename PlainOldAlignment<Cell_>::Result align_;
    };

    TypeVTable *vtable_;
    unsigned index_;

    static const unsigned Empty = ~unsigned();

  public:
    VariantType() :
        vtable_(NULL),
        index_(Empty)
    {
    }

    ~VariantType() {
        Clear();
    }

    const std::type_info &TypeId() {
        return IsEmpty() ? typeid(void) : vtable_->TypeId();
    }

    bool IsEmpty() const {
        return index_ == Empty;
    }

    void Clear() {
        if (vtable_ != NULL) {
            vtable_->Destructor(data_);
            vtable_ = NULL;
            index_ = Empty;
        }
    }

    unsigned GetType() const {
        return index_;
    }

    void Set(const Variant<Cell_> &value) {
        if (index_ != Empty && index_ == value.index_)
            vtable_->Assignment(data_, value.data_);
        else {
            Clear();
            vtable_ = value.vtable_;
            index_ = value.index_;
            if (vtable_ != NULL)
                value.vtable_->PlacementCopyConstructor(data_, value.data_);
        }
    }

    template <template <typename Type_> class Apply_>
    inline typename Apply_<void>::Type Apply() {
        return NULL;
    }
};

}

template <typename Cell_>
class Variant :
    public be::VariantType<Cell_, Cell_>
{
  private:
    typedef Variant<Cell_> This_;
    typedef be::VariantType<Cell_, Cell_> Base_;

  public:
    Variant() {
    }

    Variant(const This_ &value) :
        Base_()
    {
        Set(value);
    }

    template <typename Type_>
    explicit Variant(const Type_ &value) {
        Set(value);
    }

    This_ &operator =(const This_ &value) {
        Set(value);
        return *this;
    }

    template <typename Type_>
    This_ &operator =(const Type_ &value) {
        Set(value);
        return *this;
    }

    /*template <template <typename Type_> class Apply_>
    typename Apply_<void>::Type Apply() {
        ForEach<Cell_, Apply<Apply_>::Type>
        return index_ == Empty_ ? NULL : Base_::Apply<Apply_>();
    }*/
};

}

#endif//MENES_ETL_VARIANT_HPP
