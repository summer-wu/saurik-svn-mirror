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

#include "clr/metadata.hpp"

#include "opt/profile.hpp"

#include "clr/tokens/assembly.hpp"
#include "clr/tokens/assemblyos.hpp"
#include "clr/tokens/assemblyprocessor.hpp"
#include "clr/tokens/assemblyref.hpp"
#include "clr/tokens/assemblyrefos.hpp"
#include "clr/tokens/assemblyrefprocessor.hpp"
#include "clr/tokens/classlayout.hpp"
#include "clr/tokens/constant.hpp"
#include "clr/tokens/customattribute.hpp"
#include "clr/tokens/declsecurity.hpp"
#include "clr/tokens/event.hpp"
#include "clr/tokens/eventmap.hpp"
#include "clr/tokens/exportedtype.hpp"
#include "clr/tokens/field.hpp"
#include "clr/tokens/fieldlayout.hpp"
#include "clr/tokens/fieldmarshal.hpp"
#include "clr/tokens/fieldrva.hpp"
#include "clr/tokens/file.hpp"
#include "clr/tokens/implmap.hpp"
#include "clr/tokens/interfaceimpl.hpp"
#include "clr/tokens/manifestresource.hpp"
#include "clr/tokens/memberref.hpp"
#include "clr/tokens/method.hpp"
#include "clr/tokens/methodimpl.hpp"
#include "clr/tokens/methodsemantics.hpp"
#include "clr/tokens/module.hpp"
#include "clr/tokens/moduleref.hpp"
#include "clr/tokens/nestedclass.hpp"
#include "clr/tokens/param.hpp"
#include "clr/tokens/property.hpp"
#include "clr/tokens/propertymap.hpp"
#include "clr/tokens/standalonesig.hpp"
#include "clr/tokens/typedef.hpp"
#include "clr/tokens/typeref.hpp"
#include "clr/tokens/typespec.hpp"
#include "clr/utils.hpp"

namespace clr {

Token *Metadata::GetToken_(uint32_t token) {
    OPT_TIMER("clr::Metadata::GetToken_()")

    uint32_t row(TokenRow(token));
    if (row == 0)
        return NULL;

    //std::cout << "GetToken(0x" << std::setw(8) << std::setfill('0') << std::setbase(16) << token << ")" << std::endl;

    uint32_t table(TokenTable(token));
    Token * volatile &cache = cache_[table][row-1];
    if (cache != NULL)
        return cache;

    switch (table) {
        case 0x00: cache = new Module(); break;
        case 0x01: cache = new TypeRef(); break;
        case 0x02: cache = new TypeDef(); break;
      //case 0x03: NULL
        case 0x04: cache = new Field(); break;
      //case 0x05: NULL
        case 0x06: cache = new Method(); break;
      //case 0x07: NULL
        case 0x08: cache = new Param(); break;
        case 0x09: cache = new InterfaceImpl(); break;
        case 0x0a: cache = new MemberRef(); break;
        case 0x0b: cache = new Constant(); break;
        case 0x0c: cache = new CustomAttribute(); break;
        case 0x0d: cache = new FieldMarshal(); break;
        case 0x0e: cache = new DeclSecurity(); break;
        case 0x0f: cache = new ClassLayout(); break;

        case 0x10: cache = new FieldLayout(); break;
        case 0x11: cache = new StandAloneSig(); break;
        case 0x12: cache = new EventMap(); break;
      //case 0x13: NULL
        case 0x14: cache = new Event(); break;
        case 0x15: cache = new PropertyMap(); break;
      //case 0x16: NULL
        case 0x17: cache = new Property(); break;
        case 0x18: cache = new MethodSemantics(); break;
        case 0x19: cache = new MethodImpl(); break;
        case 0x1a: cache = new ModuleRef(); break;
        case 0x1b: cache = new TypeSpec(); break;
        case 0x1c: cache = new ImplMap(); break;
        case 0x1d: cache = new FieldRva(); break;
      //case 0x1e: NULL
      //case 0x1f: NULL

        case 0x20: cache = new Assembly(); break;
        case 0x21: cache = new AssemblyProcessor(); break;
        case 0x22: cache = new AssemblyOS(); break;
        case 0x23: cache = new AssemblyRef(); break;
        case 0x24: cache = new AssemblyRefProcessor(); break;
        case 0x25: cache = new AssemblyRefOS(); break;
        case 0x26: cache = new File(); break;
        case 0x27: cache = new ExportedType(); break;
        case 0x28: cache = new ManifestResource(); break;
        case 0x29: cache = new NestedClass(); break;
    }

    OPT_TIMER_STOP();

    _assert(cache != NULL);
    cache->Parse(*this, token);

    return cache;
}

}
