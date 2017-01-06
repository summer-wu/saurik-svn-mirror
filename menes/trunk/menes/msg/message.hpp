/* Menes Messaging - Remote Method Call for C++
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

#ifndef MENES_MSG_MESSAGE_HPP
#define MENES_MSG_MESSAGE_HPP

#include "hop/bind.hpp"
#include "hop/function.hpp"

#include "ios/buffer.hpp"
#include "msg/types.hpp"

#include "szf/binary.hpp"
#include "szf/saveload-etl.hpp"
#include "szf/saveload-ext.hpp"

namespace msg {

typedef ios::Buffer MessageBuffer;

template <typename Prototype_>
class Message :
    public hop::FunctionInvoke<
        Message<Prototype_>,
        typename hop::FunctionTraits<
            etl::Cons<MessageBuffer, typename hop::FunctionTraits<Prototype_>::Args>
        >::Prototype
    >
{
  private:
    typedef Message<Prototype_> This_;

    typedef hop::FunctionInvoke<
        Message<Prototype_>,
        typename hop::FunctionTraits<
            etl::Cons<MessageBuffer, typename hop::FunctionTraits<Prototype_>::Args>
        >::Prototype
    > Base_;

    typedef hop::FunctionTraits<Prototype_> Traits_;

    ext::Uuid id_;

  public:
    Message(uint32_t data1, uint16_t data2, uint16_t data3, uint64_t data4) :
        id_(data1, data2, data3, data4)
    {
    }

    _S<MessageBuffer> InvokeValue(hop::ArgStruct<typename Traits_::Args> args) const {
        MessageBuffer buffer;
        _S<szf::BinaryMethod> method(_null<ios::Reader>(), &buffer);
        size_t size1(buffer.GetSize());
        szf::Save(&method, id_, "@id");
        size_t size2(buffer.GetSize());
        szf::Save(&method, args, "args");
        size_t size3(buffer.GetSize());
        return buffer;
    }

    const ext::Uuid &GetId() const {
        return id_;
    }

    static typename Traits_::Return Invoke(_S<MessageBuffer> msg, const hop::Function<Prototype_> &func) {
        hop::ArgStruct<typename Traits_::Args> args;
        _S<szf::BinaryMethod> method(&msg, _null<ios::Writer>());
        size_t size1(msg.GetSize());
        szf::Load(&method, args, "args");
        size_t size2(msg.GetSize());
        return func.InvokeValue(args);
    }
};

}

#define MSG_BEGIN() \
  public: \
    virtual void Invoke_(const msg::msgid_t &mid, const msg::MessageBuffer &msg) {

#define MSG_DEFAULT(func) \
        func(mid, msg);

#define MSG_PASSTHRU(base) \
        MSG_DEFAULT(base::Invoke_);

#define MSG_MESSAGE(msg_t, func) \
        if (msg_t.GetId() == mid) \
            return msg_t.Invoke(msg, hop::Bind<0>(func, this));

#define MSG_MEMBER(msg_t, member) \
        if (msg_t.GetId() == mid) \
            return (member)->*(msg);

#define MSG_END() \
    }

#endif//MENES_MSG_MESSAGE_HPP
