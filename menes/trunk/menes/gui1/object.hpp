/* Menes GUI - C++ Sensible Abstract GUI Toolkit
 * Copyright (C) 2003  Jay Freeman (saurik)
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

#ifndef MENES_GUI1_OBJECT_HPP
#define MENES_GUI1_OBJECT_HPP

#include "hop/delegate.hpp"
#include "ext/uuid.hpp"
#include "gui1/linkage.hpp"

namespace gui1 {

class Toolkit;

class GUI1_DECLARE IObjectImpl {
  public:
    virtual ~IObjectImpl();

    virtual Toolkit &GetToolkit() const = 0;

    virtual void Create() = 0;
    virtual void Destroy() = 0;
};

class GUI1_DECLARE Object {
  private:
    Object(const Object &rhs);

  protected:
    IObjectImpl *impl_;
    Toolkit *toolkit_;
    ext::Uuid type_;

  protected:
    explicit Object(const ext::Uuid &type);

    virtual ~Object();

    virtual void SetImpl_(IObjectImpl *impl);

  public:
    const ext::Uuid &GetType() const;

    virtual Toolkit *GetPreferredToolkit() const;
    void SetPreferredToolkit(Toolkit *toolkit);

    Toolkit *GetActiveToolkit() const;

    const IObjectImpl *GetImpl() const;
    IObjectImpl *GetImpl();

    void Create();
    virtual void Destroy();
};

template <typename Base_, typename Impl_>
class ImplHolder :
    public Base_
{
  protected:
    Impl_ *impl_;

    explicit ImplHolder(const ext::Uuid &type) :
        Base_(type),
        impl_(NULL)
    {
    }

    virtual void SetImpl_(IObjectImpl *impl) {
        Base_::SetImpl_(impl);
        impl_ = dynamic_cast<Impl_ *>(impl);
    }
};

}

#endif//MENES_GUI1_OBJECT_HPP
