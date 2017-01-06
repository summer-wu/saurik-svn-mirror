/* Ninetjer - _Truly_ Free (non-GPL) Unix .NET Runtime
 * Copyright (C) 2002-2004  Jay Freeman (saurik)
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

#include "clr/ecma.hpp"
#include "clr/pefile.hpp"

#include "opt/profile.hpp"

namespace clr {

PeFile::PeFile(const cse::String &path) :
    file_(path)
{
    OPT_TIMER("clr::PeFile::PeFile()")

    char *base(file_.Begin());
    exe::DosHeader *doshdr(reinterpret_cast<exe::DosHeader *>(base));
    filehdr_ = reinterpret_cast<exe::FileHeader *>(base += doshdr->e_lfanew);
    opthdr_ = reinterpret_cast<exe::OptionalHeader *>(base += sizeof(exe::FileHeader));
    exe::SectionHeader *secthdrs(reinterpret_cast<exe::SectionHeader *>(base += filehdr_->SizeOfOptionalHeader));

    unsigned sections(filehdr_->NumberOfSections);
    for (unsigned i(0); i < sections; ++i) {
        exe::SectionHeader &secthdr = secthdrs[i];

        size_t length;
        for (length = 0; length != 8; ++length)
            if (secthdr.Name[length] == '\0')
                break;

        cse::String name(secthdr.Name, length, ext::ReadOnlyExternal);

        Section_ section;
        section.data_.Assign(file_.Begin() + secthdr.PointerToRawData, secthdr.VirtualSize, ext::ReadOnly);
        section.rva_ = secthdr.VirtualAddress;

        sections_.Insert(name, section);

        EcmaAssert("Partition II, 24.3", secthdr.PointerToLinenumbers == 0);
        EcmaAssert("Partition II, 24.3", secthdr.NumberOfLinenumbers == 0);
    }

    EcmaAssert("Partition II, 24.2.1", ::memcmp(doshdr, DosStub1, DosStub1Size) == 0);
    EcmaAssert("Partition II, 24.2.1", ::memcmp(doshdr + 1, DosStub2, DosStub2Size) == 0);

    EcmaAssert("Partition II, 24.2.1", filehdr_->SignatureStamp == 0x00004550);
    EcmaAssert("Partition II, 24.2.2", filehdr_->Machine == 0x014c);
    EcmaAssert("Partition II, 24.2.2", filehdr_->PointerToSymbolTable == 0);
    EcmaAssert("Partition II, 24.2.2", filehdr_->NumberOfSymbols == 0);

    // ECMA: typo in "Partition II, 24.2.2.1": 0x008 -> 0x0008
    EcmaAssert("Partition II, 24.2.2.1", (filehdr_->Characteristics & 0x012e) == 0x012e);
    EcmaAssert("Partition II, 24.2.2.1", (filehdr_->Characteristics & ~0x212e) == 0);

    EcmaAssert("Partition II, 24.2.3.1", opthdr_->Magic == 0x010b);
    EcmaAssert("Partition II, 24.2.3.1", opthdr_->MajorLinkerVersion == 6);
    EcmaAssert("Partition II, 24.2.3.1", opthdr_->MinorLinkerVersion == 0);

    EcmaAssert("Partition II, 24.2.3.1", opthdr_->BaseOfCode == ((filehdr_->Characteristics & exe::DLL) == 0 ? 0x00400000 : 0x10000000));

    const uint8_t *entrypoint(reinterpret_cast<const uint8_t *>(FindRva(opthdr_->AddressOfEntryPoint)));
    EcmaAssert("Partition II, 24.2.3.1", entrypoint[0] == 0xff);
    EcmaAssert("Partition II, 24.2.3.1", entrypoint[1] == 0x25);

    uint32_t EntryPointRva(*reinterpret_cast<const luint32_t *>(entrypoint + 2));

    EcmaAssert("Partition II, 24.2.3.2", opthdr_->ImageBase == 0x00400000);
    EcmaAssert("Partition II, 24.2.3.2", opthdr_->SectionAlignment == 0x00002000);
    EcmaAssert("Partition II, 24.2.3.2", opthdr_->FileAlignment == 0x200 || opthdr_->FileAlignment == 0x1000);
    EcmaAssert("Partition II, 24.2.3.2", opthdr_->MajorOperatingSystemVersion == 4);
    EcmaAssert("Partition II, 24.2.3.2", opthdr_->MinorOperatingSystemVersion == 0);
    EcmaAssert("Partition II, 24.2.3.2", opthdr_->MajorImageVersion == 0);
    EcmaAssert("Partition II, 24.2.3.2", opthdr_->MinorImageVersion == 0);
    EcmaAssert("Partition II, 24.2.3.2", opthdr_->MajorSubsystemVersion == 4);
    EcmaAssert("Partition II, 24.2.3.2", opthdr_->MinorSubsystemVersion == 0);
    EcmaAssert("Partition II, 24.2.3.2", opthdr_->Win32VersionValue == 0);
    EcmaAssert("Partition II, 24.2.3.2", opthdr_->CheckSum == 0);
    EcmaAssert("Partition II, 24.2.3.2", opthdr_->Subsystem == exe::WindowsCE_GUI || opthdr_->Subsystem == exe::Windows_GUI);
    EcmaAssert("Partition II, 24.2.3.2", opthdr_->DllCharacteristics == 0);
    EcmaAssert("Partition II, 24.2.3.2", opthdr_->SizeOfStackReserve == 0x100000);
    EcmaAssert("Partition II, 24.2.3.2", opthdr_->SizeOfStackCommit == 0x1000);
    EcmaAssert("Partition II, 24.2.3.2", opthdr_->SizeOfHeapReserve == 0x100000);
    EcmaAssert("Partition II, 24.2.3.2", opthdr_->SizeOfHeapCommit == 0x1000);
    EcmaAssert("Partition II, 24.2.3.2", opthdr_->LoaderFlags == 0);
    EcmaAssert("Partition II, 24.2.3.2", opthdr_->NumberOfRvaAndSizes == 0x10);

    EcmaAssert("Partition II, 24.2.3.3", opthdr_->DataDirectories[exe::DataDirectory::Export].VirtualAddress == 0);
    EcmaAssert("Partition II, 24.2.3.3", opthdr_->DataDirectories[exe::DataDirectory::Export].Size == 0);
    EcmaAssert("Partition II, 24.2.3.3", opthdr_->DataDirectories[exe::DataDirectory::Resource].VirtualAddress == 0);
    EcmaAssert("Partition II, 24.2.3.3", opthdr_->DataDirectories[exe::DataDirectory::Resource].Size == 0);
    EcmaAssert("Partition II, 24.2.3.3", opthdr_->DataDirectories[exe::DataDirectory::Exception].VirtualAddress == 0);
    EcmaAssert("Partition II, 24.2.3.3", opthdr_->DataDirectories[exe::DataDirectory::Exception].Size == 0);
    EcmaAssert("Partition II, 24.2.3.3", opthdr_->DataDirectories[exe::DataDirectory::Security].VirtualAddress == 0);
    EcmaAssert("Partition II, 24.2.3.3", opthdr_->DataDirectories[exe::DataDirectory::Security].Size == 0);
    EcmaAssert("Partition II, 24.2.3.3", opthdr_->DataDirectories[exe::DataDirectory::Debug].VirtualAddress == 0);
    EcmaAssert("Partition II, 24.2.3.3", opthdr_->DataDirectories[exe::DataDirectory::Debug].Size == 0);
    EcmaAssert("Partition II, 24.2.3.3", opthdr_->DataDirectories[exe::DataDirectory::Architecture].VirtualAddress == 0);
    EcmaAssert("Partition II, 24.2.3.3", opthdr_->DataDirectories[exe::DataDirectory::Architecture].Size == 0);
    EcmaAssert("Partition II, 24.2.3.3", opthdr_->DataDirectories[exe::DataDirectory::GlobalPtr].VirtualAddress == 0);
    EcmaAssert("Partition II, 24.2.3.3", opthdr_->DataDirectories[exe::DataDirectory::GlobalPtr].Size == 0);
    EcmaAssert("Partition II, 24.2.3.3", opthdr_->DataDirectories[exe::DataDirectory::TLS].VirtualAddress == 0);
    EcmaAssert("Partition II, 24.2.3.3", opthdr_->DataDirectories[exe::DataDirectory::TLS].Size == 0);
    EcmaAssert("Partition II, 24.2.3.3", opthdr_->DataDirectories[exe::DataDirectory::LoadConfig].VirtualAddress == 0);
    EcmaAssert("Partition II, 24.2.3.3", opthdr_->DataDirectories[exe::DataDirectory::LoadConfig].Size == 0);
    EcmaAssert("Partition II, 24.2.3.3", opthdr_->DataDirectories[exe::DataDirectory::BoundImport].VirtualAddress == 0);
    EcmaAssert("Partition II, 24.2.3.3", opthdr_->DataDirectories[exe::DataDirectory::BoundImport].Size == 0);
    EcmaAssert("Partition II, 24.2.3.3", opthdr_->DataDirectories[exe::DataDirectory::DelayImport].VirtualAddress == 0);
    EcmaAssert("Partition II, 24.2.3.3", opthdr_->DataDirectories[exe::DataDirectory::DelayImport].Size == 0);
    EcmaAssert("Partition II, 24.2.3.3", opthdr_->DataDirectories[exe::DataDirectory::Reserved].VirtualAddress == 0);
    EcmaAssert("Partition II, 24.2.3.3", opthdr_->DataDirectories[exe::DataDirectory::Reserved].Size == 0);

    const exe::ImportDescriptor *importhdr(reinterpret_cast<const exe::ImportDescriptor *>(FindRva(opthdr_->DataDirectories[exe::DataDirectory::Import].VirtualAddress)));
    EcmaAssert("Partition II, 24.2.3.3", importhdr != NULL);
    if (importhdr != NULL) {
        FindRva(importhdr->FirstThunk);

        EcmaAssert("Partition II, 24.3.1", importhdr[0].TimeDateStamp == 0);
        EcmaAssert("Partition II, 24.3.1", importhdr[0].ForwarderChain == 0);
        EcmaAssert("Partition II, 24.3.1", ::strcmp("mscoree.dll", FindRva(importhdr[0].Name)) == 0);
        EcmaAssert("Partition II, 24.3.1", importhdr[0].FirstThunk == opthdr_->DataDirectories[exe::DataDirectory::IAT].VirtualAddress);

        EcmaAssert("Partition II, 24.3.1", importhdr[1].OriginalFirstThunk == 0);
        EcmaAssert("Partition II, 24.3.1", importhdr[1].TimeDateStamp == 0);
        EcmaAssert("Partition II, 24.3.1", importhdr[1].ForwarderChain == 0);
        EcmaAssert("Partition II, 24.3.1", importhdr[1].Name == 0);
        EcmaAssert("Partition II, 24.3.1", importhdr[1].FirstThunk == 0);
    }
}

const char *PeFile::FindRva(uint32_t rva) const {
    _foreach (const Sections_, section, sections_)
        if (section->Second().rva_ <= rva && rva < (section->Second().rva_ + section->Second().data_.GetSize()))
            return section->Second().data_.Begin() + (rva - section->Second().rva_);
    return NULL;
}

const char *PeFile::FindVA(uint32_t va) const {
    return FindRva(va - opthdr_->ImageBase);
}

ext::SharedByteBlock PeFile::operator [](const cse::String &name) const {
    Sections_::ConstIterator section(sections_.Find(name));
    return section == sections_.End() ? ext::SharedByteBlock() : section->Second().data_;
}

ext::SharedByteBlock PeFile::operator [](unsigned index) const {
    if (index >= opthdr_->NumberOfRvaAndSizes)
        return ext::SharedByteBlock();
    exe::DataDirectory &datadir = opthdr_->DataDirectories[index];
    return ext::SharedByteBlock(FindRva(datadir.VirtualAddress), datadir.Size, ext::ReadOnly);
}

}
