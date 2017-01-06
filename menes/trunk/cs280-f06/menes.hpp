#ifndef MENES_HPP
#define MENES_HPP

#include "cxx/assert.hpp"

/* XXX: this is half-assed */

#define _comma ,
#define _copy(args...) args

namespace api {
    double GetWallTimerSeconds() {
        struct timeval now;
        gettimeofday(&now, NULL);
        return now.tv_sec + now.tv_usec / 1000000.0;
    }
}

namespace ext {

template <typename Type_>
_finline void Swap(Type_ &lhs, Type_ &rhs) {
    Type_ temp(lhs);
    lhs = rhs;
    rhs = temp;
}

template <typename Type_>
_finline const Type_ &Min(const Type_ &lhs, const Type_ &rhs) {
    return lhs < rhs ? lhs : rhs;
}

class ReferenceCounted {
    template <typename Object_>
    friend class Handle;

  private:
    unsigned count_;

  public:
    _finline ReferenceCounted() :
        count_(0)
    {
    }
};

template <typename Object_>
class Handle {
    typedef Handle<Object_> This_;

    template <typename Type_>
    friend class Handle;

  private:
    Object_ *value_;

  public:
    _finline Handle() :
        value_(NULL)
    {
    }

    _finline Handle(Object_ *value, bool mended = false) :
        value_(value)
    {
        if (value_ != NULL)
            ++value_->ReferenceCounted::count_;
    }

    _finline Handle(const This_ &rhs) :
        value_(rhs.value_)
    {
        if (value_ != NULL)
            ++value_->ReferenceCounted::count_;
    }

    template <typename Type_>
    _finline Handle(const Handle<Type_> &rhs) :
        value_(rhs.value_)
    {
        if (value_ != NULL)
            ++value_->ReferenceCounted::count_;
    }

    _finline ~Handle() {
        if (value_ != NULL && --value_->ReferenceCounted::count_ == 0)
            delete value_;
    }

    _finline Object_ *GetValue() const {
        return value_;
    }

    _finline This_ &operator =(const This_ &rhs) {
        return operator =(rhs.value_);
    }

    template <typename Type_>
    _finline This_ &operator =(Type_ *rhs) {
        if (rhs != value_) {
            if (value_ != NULL)
                --value_->ReferenceCounted::count_;
            value_ = rhs;
            if (value_ != NULL)
                ++value_->ReferenceCounted::count_;
        }

        return *this;
    }

    _finline bool IsEmpty() const {
        return value_ == NULL;
    }

    _finline Object_ *operator ->() const {
        return GetValue();
    }

    _finline operator Object_ *() const {
        return GetValue();
    }
};

#define _R ext::Handle

/* // XXX: I don't care enough
template <typename Object_>
class StackReference :
    public Object_
{
};

#define _S ext::StackReference */

template <typename Type_>
void FillAssign(Type_ *begin, Type_ *end, const Type_ &value) {
    for (; begin != end; ++begin)
        *begin = value;
}

}

#endif/*MENES_HPP*/
