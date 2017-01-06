/* Ninetjer - _Truly_ Free (non-GPL) Unix .NET Runtime
 * Copyright (C) 2002-2005  Jay Freeman (saurik)
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

#include "clr/signature.hpp"
#include "clr/storage.hpp"

namespace clr {

ValueSlot::ValueSlot() {
}

ValueSlot::ValueSlot(const Element &type) :
    type_(type)
{
}

ValueSlot &ValueSlot::operator = (const StackSlot &slot) {
    StoreValue(slot, type_, &value_);
    return *this;
}

ValueSlot::operator StackSlot() const {
    return LoadValue(type_, &value_);
}

void ValueSlot::StoreValue(StackSlot slot, const Element &type, void *value) {
    ValueUnion *val = reinterpret_cast<ValueUnion *>(value);

    switch (type.GetIntrinsic()) {
        case Element::Void:
            _assert(false);
        break;

        case Element::I1:
        switch (slot.typ) {
            case StackSlot::I4: val->int8_ = slot.i32; break;
            case StackSlot::I:  val->int8_ = slot.nat; break;
            default:            _assert(false); break;
        } break;

        case Element::Boolean:
        switch (slot.typ) {
            case StackSlot::I4: val->bool_ = slot.i32 != 0; break;
            case StackSlot::I:  val->bool_ = slot.nat != 0; break;
            default:            _assert(false); break;
        } break;

        case Element::U1:
        switch (slot.typ) {
            case StackSlot::I4: val->uint8_ = slot.i32; break;
            case StackSlot::I:  val->uint8_ = slot.nat; break;
            default:            _assert(false); break;
        } break;

        case Element::I2:
        switch (slot.typ) {
            case StackSlot::I4: val->int16_ = slot.i32; break;
            case StackSlot::I:  val->int16_ = slot.nat; break;
            default:            _assert(false); break;
        } break;

        case Element::U2:
        switch (slot.typ) {
            case StackSlot::I4: val->uint16_ = slot.i32; break;
            case StackSlot::I:  val->uint16_ = slot.nat; break;
            default:            _assert(false); break;
        } break;

        case Element::Char:
        switch (slot.typ) {
            case StackSlot::I4: val->char_ = slot.i32; break;
            case StackSlot::I:  val->char_ = slot.nat; break;
            default:            _assert(false); break;
        } break;

        case Element::I4:
        switch (slot.typ) {
            case StackSlot::I4: val->int32_ = slot.i32; break;
            case StackSlot::I:  val->int32_ = slot.nat; break;
            default:            _assert(false); break;
        } break;

        case Element::U4:
        switch (slot.typ) {
            case StackSlot::I4: val->uint32_ = slot.i32; break;
            case StackSlot::I:  val->uint32_ = slot.nat; break;
            default:            _assert(false); break;
        } break;

        case Element::I8:
        switch (slot.typ) {
            case StackSlot::I8: val->int64_ = slot.i64; break;
            default:            _assert(false); break;
        } break;

        case Element::U8:
        switch (slot.typ) {
            case StackSlot::I8: val->uint64_ = slot.i64; break;
            default:            _assert(false); break;
        } break;

        case Element::I:
        switch (slot.typ) {
            case StackSlot::I4: val->int_ = slot.i32; break;
            case StackSlot::I:  val->int_ = slot.nat; break;
            default:            _assert(false); break;
        } break;

        case Element::U:
        switch (slot.typ) {
            case StackSlot::I4: val->uint_ = slot.i32; break;
            case StackSlot::I:  val->uint_ = slot.nat; break;
            default:            _assert(false); break;
        } break;

        case Element::R4:
        switch (slot.typ) {
            case StackSlot::F:  val->float32_ = slot.flt; break;
            default:            _assert(false); break;
        } break;

        case Element::R8:
        switch (slot.typ) {
            case StackSlot::F:  val->float64_ = slot.flt; break;
            default:            _assert(false); break;
        } break;

        case Element::String:
        case Element::Class:
        case Element::Object:
        case Element::SZArray:
        switch (slot.typ) {
            case StackSlot::O:  val->object_ = slot.obj; break;
            default:            _assert(false); break;
        } break;

        case Element::Ptr:
        case Element::ByRef:

        case Element::ValueType:

        case Element::FnPtr:

        default:
            _assume(false);
    }
}

StackSlot ValueSlot::LoadValue(const Element &type, const void *value) {
    const ValueUnion *val = reinterpret_cast<const ValueUnion *>(value);

    switch (type.GetIntrinsic()) {
        case Element::Void:
            return StackSlot(StackSlot::V);
        break;

        case Element::Boolean: {
            StackSlot slot(StackSlot::I4);
            slot.i32 = val->bool_;
            return slot;
        } break;

        case Element::Char: {
            StackSlot slot(StackSlot::I4);
            slot.i32 = val->char_;
            return slot;
        } break;

        case Element::I1: {
            StackSlot slot(StackSlot::I4);
            slot.i32 = val->int8_;
            return slot;
        } break;

        case Element::U1: {
            StackSlot slot(StackSlot::I4);
            slot.i32 = val->uint8_;
            return slot;
        } break;

        case Element::I2: {
            StackSlot slot(StackSlot::I4);
            slot.i32 = val->int16_;
            return slot;
        } break;

        case Element::U2: {
            StackSlot slot(StackSlot::I4);
            slot.i32 = val->uint16_;
            return slot;
        } break;

        case Element::I4: {
            StackSlot slot(StackSlot::I4);
            slot.i32 = val->int32_;
            return slot;
        } break;

        case Element::U4: {
            StackSlot slot(StackSlot::I4);
            slot.i32 = val->uint32_;
            return slot;
        } break;

        case Element::I8: {
            StackSlot slot(StackSlot::I8);
            slot.i64 = val->int64_;
            return slot;
        } break;

        case Element::U8: {
            StackSlot slot(StackSlot::I8);
            slot.i64 = val->uint64_;
            return slot;
        } break;

        case Element::R4: {
            StackSlot slot(StackSlot::F);
            slot.flt = val->float32_;
            return slot;
        } break;

        case Element::R8: {
            StackSlot slot(StackSlot::F);
            slot.flt = val->float64_;
            return slot;
        } break;

        case Element::String: {
            StackSlot slot(StackSlot::O);
            slot.obj = val->object_;
            return slot;
        } break;

        case Element::Ptr:
        case Element::ByRef:
        case Element::ValueType:
            _assert(false);
        break;

        case Element::Class: {
            StackSlot slot(StackSlot::O);
            slot.obj = val->object_;
            return slot;
        } break;

        case Element::I: {
            StackSlot slot(StackSlot::I);
            slot.nat = val->int_;
            return slot;
        } break;

        case Element::U: {
            StackSlot slot(StackSlot::I);
            slot.nat = val->uint_;
            return slot;
        } break;

        case Element::FnPtr:
            _assert(false);
        break;

        case Element::Object: {
            StackSlot slot(StackSlot::O);
            slot.obj = val->object_;
            return slot;
        } break;

        case Element::SZArray: {
            StackSlot slot(StackSlot::O);
            slot.obj = val->object_;
            return slot;
        } break;

        default:
            _assume(false);
    }

    throw;
}

}
