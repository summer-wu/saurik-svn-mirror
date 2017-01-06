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

#include "cxx/standard.hh"
#pragma hdrstop("build/cxx/standard.pch")

#include "clr/codeds/fielddeforref.hpp"
#include "clr/codeds/member.hpp"
#include "clr/codeds/methoddeforref.hpp"
#include "clr/codeds/typedeforref.hpp"

#include "clr/net/instructions.hpp"
#include "clr/net/parse.hpp"

#include "clr/tokens/standalonesig.hpp"

#include "clr/metadata.hpp"
#include "clr/utils.hpp"

#include "ext/byteorder.hpp"
#include "ext/exception.hpp"
#include "ext/vector.hpp"

#include "opt/profile.hpp"

namespace clr {
namespace Net {

namespace be {

Instruction *ParseInstruction_(Metadata &metadata, const char *base, const char *code, InstructionList &cache) {
    Instruction * &begin = cache[code - base];
    if (begin != NULL)
        return begin;

    Instruction *end(NULL);

    //api::Cout << "0x" << ios::SetBase(16) << ios::SetFill('0') << ios::SetWidth(2) << *reinterpret_cast<const uint8_t *>(code) << ios::SetFill(' ') << ios::SetBase(10) << ios::NewLine;

    switch (*reinterpret_cast<const uint8_t *>(code++)) {
        case 0x0: { // nop
            end = begin = new NoOp();
        } break;

        case 0x1: { // break
            end = begin = new Break();
        } break;

        case 0x2: { // ldarg.0
            end = begin = new LoadArgument(0);
        } break;

        case 0x3: { // ldarg.1
            end = begin = new LoadArgument(1);
        } break;

        case 0x4: { // ldarg.2
            end = begin = new LoadArgument(2);
        } break;

        case 0x5: { // ldarg.3
            end = begin = new LoadArgument(3);
        } break;

        case 0x6: { // ldloc.0
            end = begin = new LoadLocal(0);
        } break;

        case 0x7: { // ldloc.1
            end = begin = new LoadLocal(1);
        } break;

        case 0x8: { // ldloc.2
            end = begin = new LoadLocal(2);
        } break;

        case 0x9: { // ldloc.3
            end = begin = new LoadLocal(3);
        } break;

        case 0xa: { // stloc.0
            end = begin = new StoreLocal(0);
        } break;

        case 0xb: { // stloc.1
            end = begin = new StoreLocal(1);
        } break;

        case 0xc: { // stloc.2
            end = begin = new StoreLocal(2);
        } break;

        case 0xd: { // stloc.3
            end = begin = new StoreLocal(3);
        } break;

        case 0xe: { // ldarg.s <int8>
            uint32_t index(ios::GetLittle<uint8_t>(code));
            end = begin = new LoadArgument(index);
        } break;

        case 0xf: { // ldarga.s <int8>
            uint32_t index(ios::GetLittle<uint8_t>(code));
            end = begin = new LoadArgumentAddress(index);
        } break;

        case 0x10: { // starg.s <int8>
            uint32_t index(ios::GetLittle<uint8_t>(code));
            end = begin = new StoreArgument(index);
        } break;

        case 0x11: { // ldloc.s <int8>
            uint32_t index(ios::GetLittle<uint8_t>(code));
            end = begin = new LoadLocal(index);
        } break;

        case 0x12: { // ldloca.s <int8>
            uint32_t index(ios::GetLittle<uint8_t>(code));
            end = begin = new LoadLocalAddress(index);
        } break;

        case 0x13: { // stloc.s <int8>
            uint32_t index(ios::GetLittle<uint8_t>(code));
            end = begin = new StoreLocal(index);
        } break;

        case 0x14: { // ldnull
            end = begin = new LoadNull();
        } break;

        case 0x15: { // ldc.i4.m1
            end = begin = new LoadConstant<int32_t>(-1);
        } break;

        case 0x16: { // ldc.i4.0
            end = begin = new LoadConstant<int32_t>(0);
        } break;

        case 0x17: { // ldc.i4.1
            end = begin = new LoadConstant<int32_t>(1);
        } break;

        case 0x18: { // ldc.i4.2
            end = begin = new LoadConstant<int32_t>(2);
        } break;

        case 0x19: { // ldc.i4.3
            end = begin = new LoadConstant<int32_t>(3);
        } break;

        case 0x1a: { // ldc.i4.4
            end = begin = new LoadConstant<int32_t>(4);
        } break;

        case 0x1b: { // ldc.i4.5
            end = begin = new LoadConstant<int32_t>(5);
        } break;

        case 0x1c: { // ldc.i4.6
            end = begin = new LoadConstant<int32_t>(6);
        } break;

        case 0x1d: { // ldc.i4.7
            end = begin = new LoadConstant<int32_t>(7);
        } break;

        case 0x1e: { // ldc.i4.8
            end = begin = new LoadConstant<int32_t>(8);
        } break;

        case 0x1f: { // ldc.i4.s <int8>
            uint32_t value(ios::GetLittle<int8_t>(code));
            end = begin = new LoadConstant<int32_t>(value);
        } break;

        case 0x20: { // ldc.i4
            end = begin = new LoadConstant<int32_t>(code);
        } break;

        case 0x21: { // idc.i8
            end = begin = new LoadConstant<int64_t>(code);
        } break;

        case 0x22: { // ldc.r4
            end = begin = new LoadConstant<float>(code);
        } break;

        case 0x23: { // ldc.r8
            end = begin = new LoadConstant<double>(code);
        } break;

        case 0x25: { // dup
            end = begin = new Dup(0);
        } break;

        case 0x26: { // pop
            end = begin = new Pop();
        } break;

        case 0x27: { // jmp <method>
            uint32_t token(ios::GetLittle<uint32_t>(code));
            MethodDefOrRef *method(metadata.GetToken<MethodDefOrRef>(token));
            return end = begin = new Jump(method);
        } break;

        case 0x28: { // call <method>
            uint32_t token(ios::GetLittle<uint32_t>(code));
            MethodDefOrRef *method(metadata.GetToken<MethodDefOrRef>(token));
            end = begin = new Call(method);
        } break;

        case 0x29: { // calli <method>
            uint32_t token(ios::GetLittle<uint32_t>(code));
            StandAloneSig *signature(metadata.GetToken<StandAloneSig>(token));
            end = begin = new CallIndirect(signature->GetSignature());
        } break;

        case 0x2a: { // ret
            return end = begin = new Return();
        } break;

        case 0x2b: { // br.s <label>
            int32_t offset(ios::GetLittle<int8_t>(code));
            return ParseInstruction_(metadata, base, code + offset, cache);
        } break;

        case 0x2c: { // brfalse.s <label>
            int32_t offset(ios::GetLittle<int8_t>(code));
            begin = new UnaryOperation(UnaryOperation::Not);
            end = new BranchIf(ParseInstruction_(metadata, base, code + offset, cache));
            begin->SetNext(end);
        } break;

        case 0x2d: { // brtrue.s <label>
            int32_t offset(ios::GetLittle<int8_t>(code));
            end = begin = new BranchIf(ParseInstruction_(metadata, base, code + offset, cache));
        } break;

        case 0x2e: { // beq.s <label>
            int32_t offset(ios::GetLittle<int8_t>(code));
            begin = new Compare(Compare::EqualTo, false);
            end = new BranchIf(ParseInstruction_(metadata, base, code + offset, cache));
            begin->SetNext(end);
        } break;

        case 0x2f: { // bge.s <label>
            int32_t offset(ios::GetLittle<int8_t>(code));
            begin = new Compare(Compare::GreaterThanOrEqualTo, false);
            end = new BranchIf(ParseInstruction_(metadata, base, code + offset, cache));
            begin->SetNext(end);
        } break;

        case 0x30: { // bgt.s <label>
            int32_t offset(ios::GetLittle<int8_t>(code));
            begin = new Compare(Compare::GreaterThan, false);
            end = new BranchIf(ParseInstruction_(metadata, base, code + offset, cache));
            begin->SetNext(end);
        } break;

        case 0x31: { // ble.s <label>
            int32_t offset(ios::GetLittle<int8_t>(code));
            begin = new Compare(Compare::LessThanOrEqualTo, false);
            end = new BranchIf(ParseInstruction_(metadata, base, code + offset, cache));
            begin->SetNext(end);
        } break;

        case 0x32: { // blt.s <label>
            int32_t offset(ios::GetLittle<int8_t>(code));
            begin = new Compare(Compare::LessThan, false);
            end = new BranchIf(ParseInstruction_(metadata, base, code + offset, cache));
            begin->SetNext(end);
        } break;

        case 0x33: { // bne.un.s <label>
            int32_t offset(ios::GetLittle<int8_t>(code));
            begin = new Compare(Compare::EqualTo, true);
            Instruction *temp(new UnaryOperation(UnaryOperation::Not));
            end = new BranchIf(ParseInstruction_(metadata, base, code + offset, cache));

            begin->SetNext(temp);
            temp->SetNext(end);
        } break;

        case 0x34: { // bge.un.s <label>
            int32_t offset(ios::GetLittle<int8_t>(code));
            begin = new Compare(Compare::GreaterThanOrEqualTo, true);
            end = new BranchIf(ParseInstruction_(metadata, base, code + offset, cache));
            begin->SetNext(end);
        } break;

        case 0x35: { // bgt.un.s <label>
            int32_t offset(ios::GetLittle<int8_t>(code));
            begin = new Compare(Compare::GreaterThan, true);
            end = new BranchIf(ParseInstruction_(metadata, base, code + offset, cache));
            begin->SetNext(end);
        } break;

        case 0x36: { // ble.un.s <label>
            int32_t offset(ios::GetLittle<int8_t>(code));
            begin = new Compare(Compare::LessThanOrEqualTo, true);
            end = new BranchIf(ParseInstruction_(metadata, base, code + offset, cache));
            begin->SetNext(end);
        } break;

        case 0x37: { // blt.un.s <label>
            int32_t offset(ios::GetLittle<int8_t>(code));
            begin = new Compare(Compare::LessThan, true);
            end = new BranchIf(ParseInstruction_(metadata, base, code + offset, cache));
            begin->SetNext(end);
        } break;

        case 0x38: { // br <label2>
            int32_t offset(ios::GetLittle<int32_t>(code));
            return ParseInstruction_(metadata, base, code + offset, cache);
        } break;

        case 0x39: { // brfalse <label2>
            int32_t offset(ios::GetLittle<int32_t>(code));
            begin = new UnaryOperation(UnaryOperation::Not);
            end = new BranchIf(ParseInstruction_(metadata, base, code + offset, cache));
            begin->SetNext(end);
        } break;

        case 0x3a: { // brtrue <label2>
            int32_t offset(ios::GetLittle<int32_t>(code));
            end = begin = new BranchIf(ParseInstruction_(metadata, base, code + offset, cache));
        } break;

        case 0x3b: { // beq <label2>
            int32_t offset(ios::GetLittle<int32_t>(code));
            begin = new Compare(Compare::EqualTo, false);
            end = new BranchIf(ParseInstruction_(metadata, base, code + offset, cache));
            begin->SetNext(end);
        } break;

        case 0x3c: { // bge <label2>
            int32_t offset(ios::GetLittle<int32_t>(code));
            begin = new Compare(Compare::GreaterThanOrEqualTo, false);
            end = new BranchIf(ParseInstruction_(metadata, base, code + offset, cache));
            begin->SetNext(end);
        } break;

        case 0x3d: { // bgt <label2>
            int32_t offset(ios::GetLittle<int32_t>(code));
            begin = new Compare(Compare::GreaterThan, false);
            end = new BranchIf(ParseInstruction_(metadata, base, code + offset, cache));
            begin->SetNext(end);
        } break;

        case 0x3e: { // ble <label2>
            int32_t offset(ios::GetLittle<int32_t>(code));
            begin = new Compare(Compare::LessThanOrEqualTo, false);
            end = new BranchIf(ParseInstruction_(metadata, base, code + offset, cache));
            begin->SetNext(end);
        } break;

        case 0x3f: { // blt <label2>
            int32_t offset(ios::GetLittle<int32_t>(code));
            begin = new Compare(Compare::LessThan, false);
            end = new BranchIf(ParseInstruction_(metadata, base, code + offset, cache));
            begin->SetNext(end);
        } break;

        case 0x40: { // bne.un <label2>
            int32_t offset(ios::GetLittle<int32_t>(code));
            begin = new Compare(Compare::EqualTo, true);
            Instruction *temp(new UnaryOperation(UnaryOperation::Not));
            end = new BranchIf(ParseInstruction_(metadata, base, code + offset, cache));

            begin->SetNext(temp);
            temp->SetNext(end);
        } break;

        case 0x41: { // bge.un <label2>
            int32_t offset(ios::GetLittle<int32_t>(code));
            begin = new Compare(Compare::GreaterThanOrEqualTo, true);
            end = new BranchIf(ParseInstruction_(metadata, base, code + offset, cache));
            begin->SetNext(end);
        } break;

        case 0x42: { // bgt.un <label2>
            int32_t offset(ios::GetLittle<int32_t>(code));
            begin = new Compare(Compare::GreaterThan, true);
            end = new BranchIf(ParseInstruction_(metadata, base, code + offset, cache));
            begin->SetNext(end);
        } break;

        case 0x43: { // ble.un <label2>
            int32_t offset(ios::GetLittle<int32_t>(code));
            begin = new Compare(Compare::LessThanOrEqualTo, true);
            end = new BranchIf(ParseInstruction_(metadata, base, code + offset, cache));
            begin->SetNext(end);
        } break;

        case 0x44: { // blt.un <label2>
            int32_t offset(ios::GetLittle<int32_t>(code));
            begin = new Compare(Compare::LessThan, true);
            end = new BranchIf(ParseInstruction_(metadata, base, code + offset, cache));
            begin->SetNext(end);
        } break;

        case 0x45: { // switch
            end = begin = new Switch(metadata, base, code, cache);
        } break;

        case 0x46: { // ldind.i1
            end = begin = new LoadIndirectType<int8_t>();
        } break;

        case 0x47: { // ldind.u1
            end = begin = new LoadIndirectType<uint8_t>();
        } break;

        case 0x48: { // ldind.i2
            end = begin = new LoadIndirectType<int16_t>();
        } break;

        case 0x49: { // ldind.u2
            end = begin = new LoadIndirectType<uint16_t>();
        } break;

        case 0x4a: { // ldind.i4
            end = begin = new LoadIndirectType<int32_t>();
        } break;

        case 0x4b: { // ldind.u4
            end = begin = new LoadIndirectType<uint32_t>();
        } break;

        case 0x4c: { // ldind.i8
            end = begin = new LoadIndirectType<int64_t>();
        } break;

        case 0x4d: { // ldind.i
            // XXX: this is really really bad
            end = begin = new LoadIndirectType<signed>();
        } break;

        case 0x4e: { // ldind.r4
            end = begin = new LoadIndirectType<float>();
        } break;

        case 0x4f: { // ldind.r8
            end = begin = new LoadIndirectType<double>();
        } break;

        case 0x50: { // ldind.ref
            // XXX: this is really really bad
            end = begin = new LoadIndirectType<void *>();
        } break;

        case 0x51: { // stind.ref
            // XXX: this is really really bad
            end = begin = new StoreIndirect<void *>();
        } break;

        case 0x52: { // stind.il
            end = begin = new StoreIndirect<int8_t>();
        } break;

        case 0x53: { // stind.i2
            end = begin = new StoreIndirect<int16_t>();
        } break;

        case 0x54: { // stind.i4
            end = begin = new StoreIndirect<int32_t>();
        } break;

        case 0x55: { // stind.i8
            end = begin = new StoreIndirect<int64_t>();
        } break;

        case 0x56: { // stind.r4
            end = begin = new StoreIndirect<float>();
        } break;

        case 0x57: { // stind.r8
            end = begin = new StoreIndirect<double>();
        } break;

        case 0x58: { // add
            end = begin = new BinaryOperation(BinaryOperation::Add);
        } break;

        case 0x59: { // sub
            end = begin = new BinaryOperation(BinaryOperation::Subtract);
        } break;

        case 0x5a: { // mul
            end = begin = new BinaryOperation(BinaryOperation::Multiply);
        } break;

        case 0x5b: { // div
            end = begin = new BinaryOperation(BinaryOperation::Divide);
        } break;

        case 0x5c: { // div.un
            end = begin = new BinaryOperation(BinaryOperation::DivideUnsigned);
        } break;

        case 0x5d: { // rem
            end = begin = new BinaryOperation(BinaryOperation::Remained);
        } break;

        case 0x5e: { // rem.un
            end = begin = new BinaryOperation(BinaryOperation::RemainedUnsigned);
        } break;

        case 0x5f: { // and
            end = begin = new BinaryOperation(BinaryOperation::And);
        } break;

        case 0x60: { // or
            end = begin = new BinaryOperation(BinaryOperation::Or);
        } break;

        case 0x61: { // xor
            end = begin = new BinaryOperation(BinaryOperation::XOr);
        } break;

        case 0x62: { // shl
            end = begin = new BinaryOperation(BinaryOperation::ShiftLeft);
        } break;

        case 0x63: { // shr
            end = begin = new BinaryOperation(BinaryOperation::ShiftRight);
        } break;

        case 0x64: { // shr.un
            end = begin = new BinaryOperation(BinaryOperation::ShiftRightUnsigned);
        } break;

        case 0x65: { // neg
            end = begin = new UnaryOperation(UnaryOperation::Negate);
        } break;

        case 0x66: { // not
            end = begin = new UnaryOperation(UnaryOperation::Not);
        } break;

        case 0x67: { // conv.il
            end = begin = new ConvertType<int8_t>(NoOverflow);
        } break;

        case 0x68: { // conv.i2
            end = begin = new ConvertType<int16_t>(NoOverflow);
        } break;

        case 0x69: { // conv.i4
            end = begin = new ConvertType<int32_t>(NoOverflow);
        } break;

        case 0x6a: { // conv.i8
            end = begin = new ConvertType<int64_t>(NoOverflow);
        } break;

        case 0x6b: { // conv.r4
            end = begin = new ConvertType<float>(NoOverflow);
        } break;

        case 0x6c: { // conv.r8
            end = begin = new ConvertType<double>(NoOverflow);
        } break;

        case 0x6d: { // conv.u4
            end = begin = new ConvertType<uint32_t>(NoOverflow);
        } break;

        case 0x6e: { // conv.u8
            end = begin = new ConvertType<uint64_t>(NoOverflow);
        } break;

        case 0x6f: { // callvirt <method>
            uint32_t token(ios::GetLittle<uint32_t>(code));
            MethodDefOrRef *method(metadata.GetToken<MethodDefOrRef>(token));
            end = begin = new Call(method);
        } break;

        case 0x70: { // cpobj <type>
            uint32_t token(ios::GetLittle<uint32_t>(code));
            TypeDefOrRef *type(metadata.GetToken<TypeDefOrRef>(token));
            end = begin = new CopyObject(type);
        } break;

        case 0x71: { // ldobj <type>
            uint32_t token(ios::GetLittle<uint32_t>(code));
            TypeDefOrRef *type(metadata.GetToken<TypeDefOrRef>(token));
            end = begin = new LoadObject(type);
        } break;

        case 0x72: { // ldstr <string>
            OPT_TIMER("clr::Net::ParseInstruction_()[ldstr]")
            uint32_t token(ios::GetLittle<uint32_t>(code));
            cse::String string(metadata.GetUserString(TokenRow(token)));
            end = begin = new LoadConstant<cse::String>(string);
        } break;

        case 0x73: { // newobj <method>
            uint32_t token(ios::GetLittle<uint32_t>(code));
            MethodDefOrRef *method(metadata.GetToken<MethodDefOrRef>(token));
            end = begin = new NewObject(method);
        } break;

        case 0x74: { // castclass <type>
            uint32_t token(ios::GetLittle<uint32_t>(code));
            TypeDefOrRef *type(metadata.GetToken<TypeDefOrRef>(token));
            end = begin = new CastClass(type);
        } break;

        case 0x75: { // isinst <type>
            uint32_t token(ios::GetLittle<uint32_t>(code));
            TypeDefOrRef *type(metadata.GetToken<TypeDefOrRef>(token));
            end = begin = new IsInstance(type);
        } break;

        case 0x76: { // conv.r.un
            end = begin = new ConvertType<float>(NoOverflow, true);
        } break;

        case 0x79: { // unbox <type>
            uint32_t token(ios::GetLittle<uint32_t>(code));
            TypeDefOrRef *type(metadata.GetToken<TypeDefOrRef>(token));
            end = begin = new Unbox(type);
        } break;

        case 0x7a: { // throw
            return end = begin = new Throw();
        } break;

        case 0x7b: { // ldfld <field>
            uint32_t token(ios::GetLittle<uint32_t>(code));
            FieldDefOrRef *field(metadata.GetToken<FieldDefOrRef>(token));
            end = begin = new LoadField(field);
        } break;

        case 0x7c: { // ldflda <field>
            uint32_t token(ios::GetLittle<uint32_t>(code));
            FieldDefOrRef *field(metadata.GetToken<FieldDefOrRef>(token));
            end = begin = new LoadFieldAddress(field);
        } break;

        case 0x7d: { // stfld <field>
            uint32_t token(ios::GetLittle<uint32_t>(code));
            FieldDefOrRef *field(metadata.GetToken<FieldDefOrRef>(token));
            end = begin = new StoreField(field);
        } break;

        case 0x7e: { // ldsfld <field>
            uint32_t token(ios::GetLittle<uint32_t>(code));
            FieldDefOrRef *field(metadata.GetToken<FieldDefOrRef>(token));
            end = begin = new LoadStaticField(field);
        } break;

        case 0x7f: { // ldsflda <field>
            uint32_t token(ios::GetLittle<uint32_t>(code));
            FieldDefOrRef *field(metadata.GetToken<FieldDefOrRef>(token));
            end = begin = new LoadStaticFieldAddress(field);
        } break;

        case 0x80: { // stsfld <field>
            uint32_t token(ios::GetLittle<uint32_t>(code));
            FieldDefOrRef *field(metadata.GetToken<FieldDefOrRef>(token));
            end = begin = new StoreStaticField(field);
        } break;

        case 0x81: { // stobj <type>
            uint32_t token(ios::GetLittle<uint32_t>(code));
            TypeDefOrRef *type(metadata.GetToken<TypeDefOrRef>(token));
            end = begin = new StoreObject(type);
        } break;

        case 0x82: { // conv.ovf.i1.un
            end = begin = new ConvertType<int8_t>(UnsignedOverflow);
        } break;

        case 0x83: { // conv.ovf.i2.un
            end = begin = new ConvertType<int16_t>(UnsignedOverflow);
        } break;

        case 0x84: { // conv.ovf.i4.un
            end = begin = new ConvertType<int32_t>(UnsignedOverflow);
        } break;

        case 0x85: { // conv.ovf.i8.un
            end = begin = new ConvertType<int64_t>(UnsignedOverflow);
        } break;

        case 0x86: { // conv.ovf.u1.un
            end = begin = new ConvertType<uint8_t>(UnsignedOverflow);
        } break;

        case 0x87: { // conv.ovf.u2.un
            end = begin = new ConvertType<uint16_t>(UnsignedOverflow);
        } break;

        case 0x88: { // conv.ovf.u4.un
            end = begin = new ConvertType<uint32_t>(UnsignedOverflow);
        } break;

        case 0x89: { // conv.ovf.u8.un
            end = begin = new ConvertType<uint64_t>(UnsignedOverflow);
        } break;

        case 0x8a: { // conv.ovf.i.un
            // XXX: this is really really bad
            end = begin = new ConvertType<signed>(UnsignedOverflow);
        } break;

        case 0x8b: { // conv.ovf.u.un
            // XXX: this is really really bad
            end = begin = new ConvertType<unsigned>(UnsignedOverflow);
        } break;

        case 0x8c: { // box <type>
            uint32_t token(ios::GetLittle<uint32_t>(code));
            TypeDefOrRef *type(metadata.GetToken<TypeDefOrRef>(token));
            end = begin = new Box(type);
        } break;

        case 0x8d: { // newarr <type>
            uint32_t token(ios::GetLittle<uint32_t>(code));
            TypeDefOrRef *type(metadata.GetToken<TypeDefOrRef>(token));
            end = begin = new NewArray(type);
        } break;

        case 0x8e: { // ldlen
            end = begin = new LoadLength();
        } break;

        case 0x8f: { // ldelema <type>
            uint32_t token(ios::GetLittle<uint32_t>(code));
            TypeDefOrRef *type(metadata.GetToken<TypeDefOrRef>(token));
            end = begin = new LoadElementAddress(type);
        } break;

        case 0x90: { // ldelem.i1
            end = begin = new LoadElementType<int8_t>();
        } break;

        case 0x91: { // ldelem.u1
            end = begin = new LoadElementType<uint8_t>();
        } break;

        case 0x92: { // ldelem.i2
            end = begin = new LoadElementType<int16_t>();
        } break;

        case 0x93: { // ldelem.u2
            end = begin = new LoadElementType<uint16_t>();
        } break;

        case 0x94: { // ldelem.i4
            end = begin = new LoadElementType<int32_t>();
        } break;

        case 0x95: { // ldelem.u4
            end = begin = new LoadElementType<uint32_t>();
        } break;

        case 0x96: { // ldelem.i8
            end = begin = new LoadElementType<int64_t>();
        } break;

        case 0x97: { // ldelem.i
            // XXX: this is really really bad
            end = begin = new LoadElementType<signed>();
        } break;

        case 0x98: { // ldelem.r4
            end = begin = new LoadElementType<float>();
        } break;

        case 0x99: { // ldelem.r8
            end = begin = new LoadElementType<double>();
        } break;

        case 0x9a: { // ldelem.ref
            // XXX: this is really really bad
            end = begin = new LoadElementType<void *>();
        } break;

        case 0x9b: { // stelem.i
            // XXX: this is really really bad
            end = begin = new StoreElementType<signed>();
        } break;

        case 0x9c: { // stelem.i1
            end = begin = new StoreElementType<int8_t>();
        } break;

        case 0x9d: { // stelem.i2
            end = begin = new StoreElementType<int16_t>();
        } break;

        case 0x9e: { // stelem.i4
            end = begin = new StoreElementType<int32_t>();
        } break;

        case 0x9f: { // stelem.i8
            end = begin = new StoreElementType<int64_t>();
        } break;

        case 0xa0: { // stelem.r4
            end = begin = new StoreElementType<float>();
        } break;

        case 0xa1: { // stelem.r8
            end = begin = new StoreElementType<double>();
        } break;

        case 0xa2: { // stelem.ref
            // XXX: this is really really bad
            end = begin = new StoreElementType<void *>();
        } break;

        case 0xa3: { // ldelem.any
            end = begin = new LoadElement();
        } break;

        case 0xa4: { // stelem.any
            end = begin = new StoreElement();
        } break;

        case 0xa5: { // unbox.any
            end = begin = new Unbox(NULL);
        } break;

        case 0xb3: { // conv.ovf.i1
            end = begin = new ConvertType<int8_t>(SignedOverflow);
        } break;

        case 0xb4: { // conv.ovf.u1
            end = begin = new ConvertType<uint8_t>(SignedOverflow);
        } break;

        case 0xb5: { // conv.ovf.i2
            end = begin = new ConvertType<int16_t>(SignedOverflow);
        } break;

        case 0xb6: { // conv.ovf.u2
            end = begin = new ConvertType<uint16_t>(SignedOverflow);
        } break;

        case 0xb7: { // conv.ovf.i4
            end = begin = new ConvertType<int32_t>(SignedOverflow);
        } break;

        case 0xb8: { // conv.ovf.u4
            end = begin = new ConvertType<uint32_t>(SignedOverflow);
        } break;

        case 0xb9: { // conv.ovf.i8
            end = begin = new ConvertType<int64_t>(SignedOverflow);
        } break;

        case 0xba: { // conv.ovf.u8
            end = begin = new ConvertType<uint64_t>(SignedOverflow);
        } break;

        case 0xc2: { // refanyval <type>
            uint32_t token(ios::GetLittle<uint32_t>(code));
            TypeDefOrRef *type(metadata.GetToken<TypeDefOrRef>(token));
            end = begin = new ReferenceAnyValue(type);
        } break;

        case 0xc3: { // ckfinite
            end = begin = new CheckFinite();
        } break;

        case 0xc6: { // mkrefany <type>
            uint32_t token(ios::GetLittle<uint32_t>(code));
            TypeDefOrRef *type(metadata.GetToken<TypeDefOrRef>(token));
            end = begin = new MakeReferenceAny(type);
        } break;

        case 0xd0: { // ldtoken <token>
            uint32_t token(ios::GetLittle<uint32_t>(code));
            Member *type(metadata.GetToken<Member>(token));
            end = begin = new LoadToken(type);
        } break;

        case 0xd1: { // conv.u2
            end = begin = new ConvertType<uint16_t>(NoOverflow);
        } break;

        case 0xd2: { // conv.u1
            end = begin = new ConvertType<uint8_t>(NoOverflow);
        } break;

        case 0xd3: { // conv.i
            // XXX: this is really really bad
            end = begin = new ConvertType<signed>(NoOverflow);
        } break;

        case 0xd4: { // conv.ovf.i
            // XXX: this is really really bad
            end = begin = new ConvertType<signed>(SignedOverflow);
        } break;

        case 0xd5: { // conv.ovf.u
            // XXX: this is really really bad
            end = begin = new ConvertType<unsigned>(SignedOverflow);
        } break;

        case 0xd6: { // add.ovf
            end = begin = new BinaryOperation(BinaryOperation::Add, SignedOverflow);
        } break;

        case 0xd7: { // add.ovf.un
            end = begin = new BinaryOperation(BinaryOperation::Add, UnsignedOverflow);
        } break;

        case 0xd8: { // mul.ovf
            end = begin = new BinaryOperation(BinaryOperation::Multiply, SignedOverflow);
        } break;

        case 0xd9: { // mul.ovf.un
            end = begin = new BinaryOperation(BinaryOperation::Multiply, UnsignedOverflow);
        } break;

        case 0xda: { // sub.ovf
            end = begin = new BinaryOperation(BinaryOperation::Subtract, SignedOverflow);
        } break;

        case 0xdb: { // sub.ovf.un
            end = begin = new BinaryOperation(BinaryOperation::Subtract, UnsignedOverflow);
        } break;

        case 0xdc: { // endfinally
            end = begin = new EndFinally();
        } break;

        case 0xdd: { // leave <label>
            int32_t offset(ios::GetLittle<int32_t>(code));
            end = begin = new Leave();
            code += offset;
        } break;

        case 0xde: { // leave.s <label>
            int32_t offset(ios::GetLittle<int8_t>(code));
            end = begin = new Leave();
            code += offset;
        } break;

        case 0xdf: { // stind.i
            // XXX: this is really really bad
            end = begin = new StoreIndirect<signed>();
        } break;

        case 0xe0: { // conv.u
            // XXX: this is really really bad
            end = begin = new ConvertType<unsigned>(NoOverflow);
        } break;

        case 0xfe: {
            //api::Cout << "     0x" << ios::SetBase(16) << ios::SetFill('0') << ios::SetWidth(2) << *reinterpret_cast<const uint8_t *>(code) << ios::SetFill(' ') << ios::SetBase(10) << ios::NewLine;

            switch (*reinterpret_cast<const uint8_t *>(code++)) {
                case 0x00: { // arglist
                    end = begin = new ArgumentList();
                } break;

                case 0x01: { // ceq
                    end = begin = new Compare(Compare::EqualTo, false);
                } break;

                case 0x02: { // cgt
                    end = begin = new Compare(Compare::GreaterThan, false);
                } break;

                case 0x03: { // cgt.un
                    end = begin = new Compare(Compare::GreaterThan, true);
                } break;

                case 0x04: { // clt
                    end = begin = new Compare(Compare::LessThan, false);
                } break;

                case 0x05: { // clt.un
                    end = begin = new Compare(Compare::LessThan, true);
                } break;

                case 0x06: { // ldftn <method>
                    uint32_t token(ios::GetLittle<uint32_t>(code));
                    MethodDefOrRef *method(metadata.GetToken<MethodDefOrRef>(token));
                    end = begin = new LoadFunction(method);
                } break;

                case 0x07: { // ldvirtftn <method>
                    uint32_t token(ios::GetLittle<uint32_t>(code));
                    MethodDefOrRef *method(metadata.GetToken<MethodDefOrRef>(token));
                    end = begin = new LoadFunction(method);
                } break;

                case 0x09: { // ldarg <int32>
                    uint32_t index(ios::GetLittle<uint32_t>(code));
                    end = begin = new LoadArgument(index);
                } break;

                case 0x0a: { // ldarga <int32>
                    uint32_t index(ios::GetLittle<uint32_t>(code));
                    end = begin = new LoadArgumentAddress(index);
                } break;

                case 0x0b: { // starg <int32>
                    uint32_t index(ios::GetLittle<uint32_t>(code));
                    end = begin = new StoreArgument(index);
                } break;

                case 0x0c: { // ldloc <int32>
                    uint32_t index(ios::GetLittle<uint32_t>(code));
                    end = begin = new LoadLocal(index);
                } break;

                case 0x0d: { // ldloca <int32>
                    uint32_t index(ios::GetLittle<uint32_t>(code));
                    end = begin = new LoadLocalAddress(index);
                } break;

                case 0x0e: { // stloc <int32>
                    uint32_t index(ios::GetLittle<uint32_t>(code));
                    end = begin = new StoreLocal(index);
                } break;

                case 0x0f: { // localloc
                    end = begin = new LocalAlloc();
                } break;

                case 0x11: { // endfilter
                    end = begin = new EndFilter();
                } break;

                case 0x12: { // unaligned.
                    api::Cout << "unaligned." << ios::NewLine;
                    _assert(false);
                } break;

                case 0x13: { // volatile.
                    end = begin = new Volatile();
                } break;

                case 0x14: { // tail.
                    api::Cout << "tail." << ios::NewLine;
                    _assert(false);
                } break;

                case 0x15: { // initobj <type>
                    uint32_t token(ios::GetLittle<uint32_t>(code));
                    TypeDefOrRef *type(metadata.GetToken<TypeDefOrRef>(token));
                    end = begin = new InitializeObject(type);
                } break;

                case 0x16: { // constrained. <type>
                    uint32_t token(ios::GetLittle<uint32_t>(code));
                    TypeDefOrRef *type(metadata.GetToken<TypeDefOrRef>(token));
                    end = begin = new Constrained(type);
                    api::Cout << "constrained. 0x" << ios::SetBase(16) << token << ios::SetBase(10) << ios::NewLine;
                } break;

                case 0x17: { // cpblk
                    end = begin = new CopyBlock();
                } break;

                case 0x18: { // initblk
                    end = begin = new InitializeBlock();
                } break;

                case 0x19: { // no. <int16>
                    uint16_t value(ios::GetLittle<uint16_t>(code));
                    api::Cout << "no. #" << value << ios::NewLine;
                } break;

                case 0x1a: { // rethrow
                    end = begin = new Rethrow();
                } break;

                case 0x1c: { // sizeof <type>
                    uint32_t token(ios::GetLittle<uint32_t>(code));
                    TypeDefOrRef *type(metadata.GetToken<TypeDefOrRef>(token));
                    end = begin = new SizeOf(type);
                } break;

                case 0x1d: { // refanytype
                    end = begin = new ReferenceAnyType();
                } break;
            }
        } break;
    }

    if (begin == NULL)
        throw ext::NotImplementedException(_B("Unknown Instruction"));
    if (end == NULL)
        end = begin;

    end->SetNext(ParseInstruction_(metadata, base, code, cache));
    return begin;
}

}

Instruction *ParseInstruction(Metadata &metadata, const ext::SharedByteBlock &code) {
    OPT_TIMER("clr::Net::ParseInstruction()")
    InstructionList cache(code.GetSize(), _null<Instruction>());
    return be::ParseInstruction_(metadata, code.Begin(), code.Begin(), cache);
}

} }
