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

#include "api/console.hpp"
#include "api/files.hpp"

#include "app/simple.hpp"

#include "clr/appdomain.hpp"
#include "clr/flags.hpp"
#include "clr/instruction.hpp"
#include "clr/metadata.hpp"
#include "clr/storage.hpp"
#include "clr/utils.hpp"

#include "clr/net/implementation.hpp"
#include "clr/net/instructions.hpp"
#include "clr/net/parse.hpp"

#include "clr/tokens/assembly.hpp"
#include "clr/tokens/customattribute.hpp"
#include "clr/tokens/field.hpp"
#include "clr/tokens/methodimpl.hpp"
#include "clr/tokens/memberref.hpp"
#include "clr/tokens/method.hpp"
#include "clr/tokens/module.hpp"
#include "clr/tokens/property.hpp"
#include "clr/tokens/typedef.hpp"

#include "ext/refcount.hpp"
#include "ext/shared.hpp"
#include "ext/stackref.hpp"
#include "cse/string.hpp"
#include "ext/uuid.hpp"

#include "opt/memory.hpp"
#include "opt/profile.hpp"
#include "opt/reports.hpp"

#include "xml/domwriter.hpp"
#include "xml/element.hpp"
#include "xml/name.hpp"
#include "xml/parse.hpp"
#include "xml/textwriter.hpp"

using namespace clr::Flags;

static const cse::String XmlNs("http://www.anakrino.org/xml/0");

void Output(xml::Writer &xml, const clr::Element &element) {
    switch (element.GetIntrinsic()) {
        case clr::Element::Void: {
            xml::ScopeElement tag(xml, xml::Name(_B("value"), XmlNs));
            xml.SetAttribute(_B("type"), "System.Void");
        } break;

        case clr::Element::Boolean: {
            xml::ScopeElement tag(xml, xml::Name(_B("value"), XmlNs));
            xml.SetAttribute(_B("type"), "System.Boolean");
        } break;

        case clr::Element::Char: {
            xml::ScopeElement tag(xml, xml::Name(_B("value"), XmlNs));
            xml.SetAttribute(_B("type"), "System.Char");
        } break;

        case clr::Element::I1: {
            xml::ScopeElement tag(xml, xml::Name(_B("value"), XmlNs));
            xml.SetAttribute(_B("type"), "System.SByte");
        } break;

        case clr::Element::U1: {
            xml::ScopeElement tag(xml, xml::Name(_B("value"), XmlNs));
            xml.SetAttribute(_B("type"), "System.Byte");
        } break;

        case clr::Element::I2: {
            xml::ScopeElement tag(xml, xml::Name(_B("value"), XmlNs));
            xml.SetAttribute(_B("type"), "System.Int16");
        } break;

        case clr::Element::U2: {
            xml::ScopeElement tag(xml, xml::Name(_B("value"), XmlNs));
            xml.SetAttribute(_B("type"), "System.UInt16");
        } break;

        case clr::Element::I4: {
            xml::ScopeElement tag(xml, xml::Name(_B("value"), XmlNs));
            xml.SetAttribute(_B("type"), "System.Int32");
        } break;

        case clr::Element::U4: {
            xml::ScopeElement tag(xml, xml::Name(_B("value"), XmlNs));
            xml.SetAttribute(_B("type"), "System.UInt32");
        } break;

        case clr::Element::I8: {
            xml::ScopeElement tag(xml, xml::Name(_B("value"), XmlNs));
            xml.SetAttribute(_B("type"), "System.Int64");
        } break;

        case clr::Element::U8: {
            xml::ScopeElement tag(xml, xml::Name(_B("value"), XmlNs));
            xml.SetAttribute(_B("type"), "System.UInt64");
        } break;

        case clr::Element::R4: {
            xml::ScopeElement tag(xml, xml::Name(_B("value"), XmlNs));
            xml.SetAttribute(_B("type"), "System.Single");
        } break;

        case clr::Element::R8: {
            xml::ScopeElement tag(xml, xml::Name(_B("value"), XmlNs));
            xml.SetAttribute(_B("type"), "System.Double");
        } break;

        case clr::Element::String: {
            xml::ScopeElement tag(xml, xml::Name(_B("class"), XmlNs));
            xml.SetAttribute(_B("type"), "System.String");
        } break;

        case clr::Element::Ptr: {
            xml::ScopeElement tag(xml, xml::Name(_B("pointer"), XmlNs));
            Output(xml, *element.GetExtra());
        } break;

        case clr::Element::ByRef: {
            xml::ScopeElement tag(xml, xml::Name(_B("reference"), XmlNs));
            Output(xml, *element.GetExtra());
        } break;

        case clr::Element::ValueType: {
            xml::ScopeElement tag(xml, xml::Name(_B("value"), XmlNs));
            xml.SetAttribute(_B("type"), element.GetType()->GetName());
        } break;

        case clr::Element::Class: {
            xml::ScopeElement tag(xml, xml::Name(_B("class"), XmlNs));
            xml.SetAttribute(_B("type"), element.GetType()->GetName());
        } break;

        case clr::Element::Parameter: {
            xml::ScopeElement tag(xml, xml::Name(_B("parameter"), XmlNs));
            xml.SetAttribute(_B("index"), lexical_cast<cse::String>(element.GetIndex()));
        } break;

        case clr::Element::Specialize: {
            xml::ScopeElement tag(xml, xml::Name(_B("specialize"), XmlNs));
            Output(xml, *element.GetExtra());

            _foreach (const clr::ElementList, extra, element.GetExtras()) {
                xml::ScopeElement tag(xml, xml::Name(extra == element.GetExtras().Begin() ? _B("generic") : _B("argument"), XmlNs));
                Output(xml, *extra);
            }
        } break;

        case clr::Element::TypedByRef: {
            xml::ScopeElement tag(xml, xml::Name(_B("value"), XmlNs));
            xml.SetAttribute(_B("type"), "System.TypedReference");
        } break;

        case clr::Element::I: {
            xml::ScopeElement tag(xml, xml::Name(_B("value"), XmlNs));
            xml.SetAttribute(_B("type"), "System.IntPtr");
        } break;

        case clr::Element::U: {
            xml::ScopeElement tag(xml, xml::Name(_B("value"), XmlNs));
            xml.SetAttribute(_B("type"), "System.UIntPtr");
        } break;

        case clr::Element::Object: {
            xml::ScopeElement tag(xml, xml::Name(_B("class"), XmlNs));
            xml.SetAttribute(_B("type"), "System.Object");
        } break;

        case clr::Element::SZArray: {
            xml::ScopeElement tag(xml, xml::Name(_B("vector"), XmlNs));
            Output(xml, *element.GetExtra());
        } break;

        default:
            _assert(false);
    }
}

void Output(xml::Writer &xml, clr::TypeDefOrRef *type, const clr::Signature &signature) {
    {
        xml::ScopeElement tag(xml, xml::Name(_B("result"), XmlNs));
        Output(xml, signature.GetResult());
    }

    _foreach (const clr::ElementList, element, signature.GetArguments(type)) {
        xml::ScopeElement tag(xml, xml::Name(_B("argument"), XmlNs));
        Output(xml, *element);
    }
}

int Main(const app::Options &options) {
    clr::Metadata metadata(*options[0]);
    clr::Assembly *assembly(metadata.GetAssembly());

    _S<xml::TextWriter> xml(api::Cout);

    xml::ScopeElement tag(xml, xml::Name(_B("assembly"), XmlNs));
    xml.SetAttribute(_B("name"), assembly->GetName());

    _foreach (const clr::Assembly::ModuleMap, module, assembly->GetModules()) {
        xml::ScopeElement tag(xml, xml::Name(_B("module"), XmlNs));
        xml.SetAttribute(_B("name"), module->Second()->GetName());

        _foreach (const clr::Module::TypeMap, type, module->Second()->GetTypes()) {
            xml::ScopeElement tag(xml, xml::Name(_B("type"), XmlNs));
            xml.SetAttribute(_B("name"), type->Second()->GetName());

            if (clr::TypeDefOrRef *extends = type->Second()->GetExtends()) {
                xml::ScopeElement tag(xml, xml::Name(_B("extends"), XmlNs));
                xml.SetAttribute(_B("type"), extends->GetName());
            }

            _foreach (const clr::TypeDef::MethodList, method, type->Second()->GetMethodList()) {
                xml::ScopeElement tag(xml, xml::Name(_B("method"), XmlNs));
                xml.SetAttribute(_B("name"), (*method)->GetName());

                Output(xml, type->Second(), (*method)->GetSignature());

                clr::Implementation *implementation((*method)->GetImplementation());
                if (implementation == NULL)
                    continue;

                clr::Instruction *root(implementation->GetRoot());
            }
        }
    }

    return 0;
}
