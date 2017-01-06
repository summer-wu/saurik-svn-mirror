/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2004  Jay Freeman (saurik)
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

#ifndef MENES_CLR_NET_INSTRUCTIONS_HPP
#define MENES_CLR_NET_INSTRUCTIONS_HPP

#include "cxx/platform.hpp"
#include "clr/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "clr/element.hpp"
#include "clr/instruction.hpp"
#include "clr/signature.hpp"

#include "clr/codeds/methoddeforref.hpp"

#include "ext/byteorder.hpp"
#include "ext/workarounds.hpp"

#include "ios/getput.hpp"

namespace clr {

class FieldDefOrRef;
class Member;
class Module;
class TypeDefOrRef;

namespace Net {

enum Overflow {
    NoOverflow,
    SignedOverflow,
    UnsignedOverflow
};

class Compare :
    public Instruction
{
    EXT_TYPEDEFTHIS(Compare)
    EXT_CLONEABLE()

  public:
    enum Type {
        EqualTo,
        NotEqualTo,
        GreaterThan,
        LessThanOrEqualTo,
        LessThan,
        GreaterThanOrEqualTo
    };

  private:
    Type type_;
    bool notsigned_;

  protected:
    virtual unsigned GetStackUp() const {
        return 1;
    }

    virtual unsigned GetStackDown() const {
        return 2;
    }

  public:
    Compare(Type type, bool notsigned) :
        type_(type),
        notsigned_(notsigned)
    {
    }

    Type GetType() const {
        return type_;
    }
};

#define BasedInstruction(name, base, up, down) \
class name : \
    public base \
{ \
    EXT_TYPEDEFTHIS(name) \
    EXT_CLONEABLE() \
\
  protected: \
    virtual unsigned GetStackUp() const { \
        return up; \
    } \
\
    virtual unsigned GetStackDown() const { \
        return down; \
    } \
};

#define SimpleInstruction(name, up, down) \
    BasedInstruction(name, Instruction, up, down)

SimpleInstruction(ArgumentList, 1, 0)
SimpleInstruction(Break, 0, 0)
SimpleInstruction(CheckFinite, 1, 1)
SimpleInstruction(CopyBlock, 0, 3)
SimpleInstruction(EndFilter, 0, 1)
SimpleInstruction(EndFinally, 0, 0)
SimpleInstruction(InitializeBlock, 0, 3)
SimpleInstruction(Leave, 0, 0)
SimpleInstruction(LoadLength, 1, 1)
SimpleInstruction(LoadNull, 1, 0)
SimpleInstruction(LocalAlloc, 1, 1)
SimpleInstruction(NoOp, 0, 0)
SimpleInstruction(Pop, 0, 1)
SimpleInstruction(ReferenceAnyType, 1, 1)
SimpleInstruction(Rethrow, 0, 0)
SimpleInstruction(Throw, 0, 1)

// XXX: this is technically a prefix
SimpleInstruction(Volatile, 1, 1)

SimpleInstruction(Return, 0, 0)
BasedInstruction(ReturnVoid, Return, 0, 0)
BasedInstruction(ReturnValue, Return, 0, 1)

template <typename Type_>
SimpleInstruction(StoreIndirect, 0, 2)

SimpleInstruction(LoadIndirect, 1, 1)

template <typename Type_>
BasedInstruction(LoadIndirectType, LoadIndirect, 1, 1)

SimpleInstruction(StoreElement, 0, 3)

template <typename Type_>
BasedInstruction(StoreElementType, StoreElement, 0, 3)

SimpleInstruction(LoadElement, 1, 2)

template <typename Type_>
BasedInstruction(LoadElementType, LoadElement, 1, 2)

#undef SimpleInstruction

#define IndexedInstruction(name, up, down) \
class name : \
    public Instruction \
{ \
    EXT_TYPEDEFTHIS(name) \
    EXT_CLONEABLE() \
\
  private: \
    uint32_t index_; \
\
  protected: \
    virtual unsigned GetStackUp() const { \
        return up; \
    } \
\
    virtual unsigned GetStackDown() const { \
        return down; \
    } \
\
  public: \
    name(uint32_t index) : \
        index_(index) \
    { \
    } \
\
    uint32_t GetIndex() const { \
        return index_; \
    } \
};

#define IndexedLoadStore(type) \
IndexedInstruction(Store ## type, 0, 1) \
IndexedInstruction(Load ## type, 1, 0) \
IndexedInstruction(Load ## type ## Address, 1, 0)

IndexedInstruction(Dup, 2, 1)
IndexedLoadStore(Argument)
IndexedLoadStore(Local)

#undef IndexedLoadStore
#undef IndexedInstruction

#define TokenInstruction(type, capital, simple, name, up, down) \
class name : \
    public Instruction \
{ \
    EXT_TYPEDEFTHIS(name) \
    EXT_CLONEABLE() \
\
  private: \
    type *simple ## _; \
\
  protected: \
    virtual unsigned GetStackDown() const { \
        return down; \
    } \
\
    virtual unsigned GetStackUp() const { \
        return up; \
    } \
\
  public: \
    name(type *simple) : \
        simple ## _(simple) \
    { \
    } \
\
    type *Get ## capital() const { \
        return simple ## _; \
    } \
};

TokenInstruction(Member, Member, member, LoadToken, 1, 0)

#define DefRefInstruction(type, simple, name, up, down) \
TokenInstruction(type ## DefOrRef, type, simple, name, up, down)

DefRefInstruction(Field, field, LoadField, 1, 1)
DefRefInstruction(Field, field, LoadFieldAddress, 1, 1)
DefRefInstruction(Field, field, StoreField, 0, 2)

DefRefInstruction(Field, field, LoadStaticField, 1, 0)
DefRefInstruction(Field, field, LoadStaticFieldAddress, 1, 0)
DefRefInstruction(Field, field, StoreStaticField, 0, 1)

DefRefInstruction(Method, method, Jump, 0, 0)
DefRefInstruction(Method, method, LoadFunction, 1, 0)

DefRefInstruction(Type, type, Box, 1, 1)
DefRefInstruction(Type, type, CastClass, 1, 1)
DefRefInstruction(Type, type, CopyObject, 0, 2)
DefRefInstruction(Type, type, InitializeObject, 0, 1)
DefRefInstruction(Type, type, IsInstance, 1, 1)
DefRefInstruction(Type, type, LoadElementAddress, 1, 2)
DefRefInstruction(Type, type, LoadObject, 1, 1)
DefRefInstruction(Type, type, MakeReferenceAny, 1, 1)
DefRefInstruction(Type, type, NewArray, 1, 1)
DefRefInstruction(Type, type, ReferenceAnyValue, 1, 1)
DefRefInstruction(Type, type, SizeOf, 1, 0)
DefRefInstruction(Type, type, StoreObject, 0, 2)
DefRefInstruction(Type, type, Unbox, 1, 1)

// XXX: this is technically a prefix
DefRefInstruction(Type, type, Constrained, 0, 0)

#undef DefRefInstruction
#undef TokenInstruction

#define MethodInstruction(name, up, down) \
class name : \
    public Instruction \
{ \
    EXT_TYPEDEFTHIS(name) \
    EXT_CLONEABLE() \
\
  private: \
    MethodDefOrRef *method_; \
\
  protected: \
    virtual unsigned GetStackUp() const { \
        const Signature &signature(method_->GetSignature()); \
        return (signature.GetResult().IsVoid() ? 0 : 1) + up; \
    } \
\
    virtual unsigned GetStackDown() const { \
        return method_->GetSignature().GetArguments().GetSize() - down; \
    } \
\
  public: \
    name(MethodDefOrRef *method) : \
        method_(method) \
    { \
    } \
\
    MethodDefOrRef *GetMethod() { \
        return method_; \
    } \
};

MethodInstruction(Call, 0, 0)
MethodInstruction(NewObject, 1, 1)

#undef MethodInstruction

class CallIndirect :
    public Instruction
{
    EXT_TYPEDEFTHIS(CallIndirect)
    EXT_CLONEABLE()

  private:
    _S<Signature> signature_;

  protected:
    virtual unsigned GetStackUp() const {
        return (signature_.GetResult().IsVoid() ? 0 : 1);
    }

    virtual unsigned GetStackDown() const {
        // XXX: this is wrong
        return signature_.GetArguments().GetSize();
    }

  public:
    CallIndirect(const Signature &signature) :
        signature_(signature)
    {
    }

    const Signature &GetSignature() const {
        return signature_;
    }
};

template <typename Type_>
class LoadConstant :
    public Instruction
{
    EXT_TYPEDEFTHIS(LoadConstant<Type_>)
    EXT_CLONEABLE()

  private:
    Type_ value_;

  protected:
    virtual unsigned GetStackUp() const {
        return 1;
    }

    virtual unsigned GetStackDown() const {
        return 0;
    }

  public:
    LoadConstant(const Type_ &value) :
        value_(value)
    {
    }

    LoadConstant(const char * &code) :
        value_(ios::GetLittle<Type_>(code))
    {
    }

    const Type_ &GetValue() const {
        return value_;
    }
};

class BranchIf :
    public Instruction
{
    EXT_TYPEDEFTHIS(BranchIf)
    EXT_CLONEABLE()

  private:
    Instruction *target_;

  protected:
    virtual unsigned GetStackUp() const {
        return 0;
    }

    virtual unsigned GetStackDown() const {
        return 1;
    }

  public:
    BranchIf(Instruction *target) {
        ext::Edge<ExecuteGraph>::SetSize(2);
        ext::Edge<ExecuteGraph>::SetEdge(1, target);
    }

    Instruction *GetTarget() {
        return ext::Edge<ExecuteGraph>::GetImplicit()[1];
    }
};

class UnaryOperation :
    public Instruction
{
    EXT_TYPEDEFTHIS(UnaryOperation)
    EXT_CLONEABLE()

  public:
    enum Type {
        Negate,
        Not
    };

  private:
    Type type_;

  protected:
    virtual unsigned GetStackUp() const {
        return 1;
    }

    virtual unsigned GetStackDown() const {
        return 1;
    }

  public:
    UnaryOperation(Type type) :
        type_(type)
    {
    }

    Type GetType() const {
        return type_;
    }
};

class BinaryOperation :
    public Instruction
{
    EXT_TYPEDEFTHIS(BinaryOperation)
    EXT_CLONEABLE()

  public:
    enum Type {
        Add,
        Subtract,
        Multiply,
        Divide,
        Remained,

        And,
        Or,
        XOr,
        ShiftLeft,
        ShiftRight,

        DivideUnsigned,
        RemainedUnsigned,
        ShiftRightUnsigned
    };

  private:
    Type type_;
    Overflow overflow_;

  protected:
    virtual unsigned GetStackUp() const {
        return 1;
    }

    virtual unsigned GetStackDown() const {
        return 2;
    }

  public:
    BinaryOperation(Type type, Overflow overflow = NoOverflow) :
        type_(type),
        overflow_(overflow)
    {
    }

    Type GetType() const {
        return type_;
    }
};

class Convert :
    public Instruction
{
    EXT_TYPEDEFTHIS(Convert)
    EXT_CLONEABLE()

  private:
    Overflow overflow_;
    bool notsigned_;

  protected:
    virtual unsigned GetStackUp() const {
        return 1;
    }

    virtual unsigned GetStackDown() const {
        return 1;
    }

  public:
    Convert(Overflow overflow, bool notsigned = false) :
        overflow_(overflow),
        notsigned_(notsigned)
    {
    }
};

template <typename Type_>
class ConvertType :
    public Convert
{
    EXT_TYPEDEFTHIS(ConvertType<Type_>)
    EXT_CLONEABLE()

  public:
    ConvertType(Overflow overflow, bool notsigned = false) :
        Convert(overflow, notsigned)
    {
    }
};

class Switch :
    public Instruction
{
    EXT_TYPEDEFTHIS(Switch)
    EXT_CLONEABLE()

  protected:
    virtual unsigned GetStackUp() const {
        return 0;
    }

    virtual unsigned GetStackDown() const {
        return 1;
    }

  public:
    Switch(Metadata &metadata, const char *base, const char * &code, InstructionList &cache);
};

}

}

#endif//MENES_CLR_NET_INSTRUCTIONS_HPP
