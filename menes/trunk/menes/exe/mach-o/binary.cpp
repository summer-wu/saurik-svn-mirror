/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2007-2009  Jay Freeman (saurik)
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

#include "exe/mach-o/binary.hpp"

namespace exe {
namespace MachO {

void *Binary::GetAddress_(uint32_t address, const char *segment_name) {
    Layout::load_command *load_command = reinterpret_cast<Layout::load_command *>(mach_header_ + 1);
    uint32_t cmd;

    for (cmd = 0; cmd != Swap(mach_header_->ncmds); ++cmd) {
        if (Swap(load_command->cmd) == LC_SEGMENT) {
            Layout::segment_command *segment_command = reinterpret_cast<Layout::segment_command *>(load_command);
            if (segment_name != NULL && strncmp(segment_command->segname, segment_name, 16) != 0)
                goto next_command;

            Layout::section *sections = reinterpret_cast<Layout::section *>(segment_command + 1);

            uint32_t sect;
            for (sect = 0; sect != Swap(segment_command->nsects); ++sect) {
                Layout::section *section = &sections[sect];
                //printf("%s %u %p %p %u\n", segment_command->segname, sect, address, section->addr, section->size);
                if (address >= Swap(section->addr) && address < Swap(section->addr) + Swap(section->size)) {
                    //printf("0x%.8x %s\n", address, segment_command->segname);
                    return address - Swap(section->addr) + Swap(section->offset) + reinterpret_cast<uint8_t *>(mach_header_);
                }
            }
        }

      next_command:
        load_command = reinterpret_cast<Layout::load_command *>(reinterpret_cast<uint8_t *>(load_command) + Swap(load_command->cmdsize));
    }

    return NULL;
}

void *Binary::GetOffset_(uint32_t offset) {
    return offset + reinterpret_cast<uint8_t *>(mach_header_);
}

Binary::Binary(const cse::String &path) :
    Base_(path)
{
    uint8_t *base(reinterpret_cast<uint8_t *>(Begin()));
    Layout::fat_header *fat_header = reinterpret_cast<Layout::fat_header *>(base);

    if (Swap(fat_header->magic) == FAT_CIGAM) {
        swapped_ = !swapped_;
        goto fat;
    } else if (Swap(fat_header->magic) != FAT_MAGIC)
        mach_header_ = reinterpret_cast<Layout::mach_header *>(base);
    else fat: {
        size_t fat_narch = Swap(fat_header->nfat_arch);
        Layout::fat_arch *fat_arch = reinterpret_cast<Layout::fat_arch *>(fat_header + 1);
        size_t arch;
        for (arch = 0; arch != fat_narch; ++arch) {
            uint32_t arch_offset = Swap(fat_arch->offset);
            mach_header_ = reinterpret_cast<Layout::mach_header *>(base + arch_offset);
            goto found;
            ++fat_arch;
        }

        _assert(false);
    }

  found:
    if (Swap(mach_header_->magic) == MH_CIGAM)
        swapped_ = !swapped_;
    else _assert(Swap(mach_header_->magic) == MH_MAGIC);

    _assert(
        Swap(mach_header_->filetype) == MH_EXECUTE ||
        Swap(mach_header_->filetype) == MH_DYLIB ||
        Swap(mach_header_->filetype) == MH_BUNDLE
    );
}

Layout::mach_header *Binary::operator ->() const {
    return mach_header_;
}

ext::Vector<Layout::load_command *> Binary::GetLoadCommands() const {
    ext::Vector<Layout::load_command *> load_commands;

    Layout::load_command *load_command = reinterpret_cast<Layout::load_command *>(mach_header_ + 1);
    for (uint32_t cmd = 0; cmd != Swap(mach_header_->ncmds); ++cmd) {
        load_commands.InsertLast(load_command);
        load_command = reinterpret_cast<Layout::load_command *>(reinterpret_cast<uint8_t *>(load_command) + Swap(load_command->cmdsize));
    }

    return load_commands;
}

ext::Vector<Layout::segment_command *> Binary::GetSegments(const char *segment_name) const {
    ext::Vector<Layout::segment_command *> segment_commands;

    _foreach (const ext::Vector<Layout::load_command *>, load_command, GetLoadCommands())
        if (Swap((*load_command)->cmd) == LC_SEGMENT) {
            Layout::segment_command *segment_command = reinterpret_cast<Layout::segment_command *>(*load_command);
            if (strncmp(segment_command->segname, segment_name, 16) == 0)
                segment_commands.InsertLast(segment_command);
        }

    return segment_commands;
}

ext::Vector<Layout::section *> Binary::GetSections(const char *segment_name, const char *section_name) const {
    ext::Vector<Layout::section *> sections;

    _foreach (const ext::Vector<Layout::segment_command *>, segment, GetSegments(segment_name)) {
        Layout::section *section = reinterpret_cast<Layout::section *>(*segment + 1);

        uint32_t sect;
        for (sect = 0; sect != Swap((*segment)->nsects); ++sect) {
            if (strncmp(section->sectname, section_name, 16) == 0)
                sections.InsertLast(section);
            ++section;
        }
    }

    return sections;
}

} }
