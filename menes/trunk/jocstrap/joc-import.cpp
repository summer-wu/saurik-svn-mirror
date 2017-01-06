/* JocStrap - Java/Objective-C Bootstrap
 * Copyright (C) 2007  Jay Freeman (saurik)
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

#include "minimal/stdlib.h"
#include "minimal/mapping.h"
#include "minimal/sqlite3.h"

#include <cstring>
#include <vector>

struct fat_header {
    uint32_t magic;
    uint32_t nfat_arch;
};

#define FAT_MAGIC 0xcafebabe
#define FAT_CIGAM 0xbebafeca

struct fat_arch {
    uint32_t cputype;
    uint32_t cpusubtype;
    uint32_t offset;
    uint32_t size;
    uint32_t align;
};

struct mach_header {
    uint32_t magic;
    uint32_t cputype;
    uint32_t cpusubtype;
    uint32_t filetype;
    uint32_t ncmds;
    uint32_t sizeofcmds;
    uint32_t flags;
};

#define	MH_MAGIC 0xfeedface
#define MH_CIGAM 0xcefaedfe

#define	MH_EXECUTE    0x2
#define	MH_DYLIB      0x6
#define	MH_BUNDLE     0x8
#define	MH_DYLIB_STUB 0x9

struct load_command {
    uint32_t cmd;
    uint32_t cmdsize;
};

#define LC_REQ_DYLD  0x80000000

#define	LC_SEGMENT         0x01
#define	LC_LOAD_DYLIB	   0x0c
#define	LC_ID_DYLIB	   0x0d
#define LC_UUID		   0x1b
#define LC_REEXPORT_DYLIB (0x1f | LC_REQ_DYLD)

struct dylib {
    uint32_t name;
    uint32_t timestamp;
    uint32_t current_version;
    uint32_t compatibility_version;
};

struct dylib_command {
    uint32_t cmd;
    uint32_t cmdsize;
    struct dylib dylib;
};

struct uuid_command {
    uint32_t cmd;
    uint32_t cmdsize;
    uint8_t uuid[16];
};

struct segment_command {
    uint32_t cmd;
    uint32_t cmdsize;
    char segname[16];
    uint32_t vmaddr;
    uint32_t vmsize;
    uint32_t fileoff;
    uint32_t filesize;
    uint32_t maxprot;
    uint32_t initprot;
    uint32_t nsects;
    uint32_t flags;
};

#define	SEG_DATA "__DATA"
#define	SEG_OBJC "__OBJC"

struct section {
    char sectname[16];
    char segname[16];
    uint32_t addr;
    uint32_t size;
    uint32_t offset;
    uint32_t align;
    uint32_t reloff;
    uint32_t nreloc;
    uint32_t flags;
    uint32_t reserved1;
    uint32_t reserved2;
};

#define SECT_OBJC_MODULES "__module_info"
#define SECT_DATA_OBJC_CLASSLIST "__objc_classlist"
#define SECT_DATA_OBJC_CATLIST "__objc_catlist"

struct objc_symtab {
    uint32_t sel_ref_cnt;
    uint32_t refs;
    uint16_t cls_def_cnt;
    uint16_t cat_def_cnt;
    uint32_t defs[0];
};

struct objc_module {
    uint32_t version;
    uint32_t size;
    uint32_t name;
    uint32_t symtab;
};

struct objc_class {
    uint32_t isa;
    uint32_t super_class;
    uint32_t name;
    int32_t version;
    int32_t info;
    int32_t instance_size;
    uint32_t ivars;
    uint32_t methodLists;
    uint32_t cache;
    uint32_t protocols;
};

struct objc_ivar {
    uint32_t ivar_name;
    uint32_t ivar_type;
    int32_t ivar_offset;
};

struct objc_ivar_list {
    int32_t ivar_count;
    objc_ivar ivar_list[0];
};

#define CLS_CLASS 0x1

struct objc_category {
    uint32_t category_name;
    uint32_t class_name;
    uint32_t instances_methods;
    uint32_t class_methods;
    uint32_t protocols;
};

struct objc_method {
    uint32_t method_name;
    uint32_t method_types;
    uint32_t method_imp;
};

struct objc_method_list {
    uint32_t obsolete;
    int32_t method_count;
    objc_method method_list[0];
};

template <typename Target_>
class Pointer;

class Framework {
  private:
    void *base_;
    mach_header *mach_header_;
    bool swapped_;

  public:
    int16_t Swap(int16_t value) const {
        return Swap(static_cast<uint16_t>(value));
    }

    int32_t Swap(int32_t value) const {
        return Swap(static_cast<uint32_t>(value));
    }

    uint16_t Swap(uint16_t value) const {
        return !swapped_ ? value :
            ((value >>  8) & 0x00ff) |
            ((value <<  8) & 0xff00);
    }

    uint32_t Swap(uint32_t value) const {
        if (!swapped_)
            return value;
        else {
            value = ((value >>  8) & 0x00ff00ff) |
                    ((value <<  8) & 0xff00ff00);
            value = ((value >> 16) & 0x0000ffff) |
                    ((value << 16) & 0xffff0000);
            return value;
        }
    }

    Framework(const char *framework_path) :
        swapped_(false)
    {
        base_ = map(framework_path, 0, _not(size_t), NULL, true);
        fat_header *fat_header = reinterpret_cast<struct fat_header *>(base_);

        if (Swap(fat_header->magic) == FAT_CIGAM) {
            swapped_ = !swapped_;
            goto fat;
        } else if (Swap(fat_header->magic) != FAT_MAGIC)
            mach_header_ = (mach_header *) base_;
        else fat: {
            size_t fat_narch = Swap(fat_header->nfat_arch);
            fat_arch *fat_arch = reinterpret_cast<struct fat_arch *>(fat_header + 1);
            size_t arch;
            for (arch = 0; arch != fat_narch; ++arch) {
                uint32_t arch_offset = Swap(fat_arch->offset);
                if (Swap(fat_arch->cputype) == 7 && Swap(fat_arch->cpusubtype) == 3) {
                    mach_header_ = (mach_header *) ((uint8_t *) base_ + arch_offset);
                    goto found;
                }
                ++fat_arch;
            }
            exit(1);
        }

      found:
        if (Swap(mach_header_->magic) == MH_CIGAM)
            swapped_ = !swapped_;
        else _assert(Swap(mach_header_->magic) == MH_MAGIC);

        _assert(
            Swap(mach_header_->filetype) == MH_EXECUTE ||
            Swap(mach_header_->filetype) == MH_DYLIB ||
            Swap(mach_header_->filetype) == MH_DYLIB_STUB ||
            Swap(mach_header_->filetype) == MH_BUNDLE
        );
    }

    std::vector<struct load_command *> GetLoadCommands() {
        std::vector<struct load_command *> load_commands;

        struct load_command *load_command = reinterpret_cast<struct load_command *>(mach_header_ + 1);
        for (uint32_t cmd = 0; cmd != Swap(mach_header_->ncmds); ++cmd) {
            load_commands.push_back(load_command);
            load_command = (struct load_command *) ((uint8_t *) load_command + Swap(load_command->cmdsize));
        }

        return load_commands;
    }

    std::vector<segment_command *> GetSegments(const char *segment_name) {
        std::vector<struct segment_command *> segment_commands;

        _foreach (load_command, GetLoadCommands())
            if (Swap((*load_command)->cmd) == LC_SEGMENT) {
                segment_command *segment_command = reinterpret_cast<struct segment_command *>(*load_command);
                if (strncmp(segment_command->segname, segment_name, 16) == 0)
                    segment_commands.push_back(segment_command);
            }

        return segment_commands;
    }

    std::vector<section *> GetSections(const char *segment_name, const char *section_name) {
        std::vector<section *> sections;

        _foreach (segment, GetSegments(segment_name)) {
            section *section = (struct section *) (*segment + 1);

            uint32_t sect;
            for (sect = 0; sect != Swap((*segment)->nsects); ++sect) {
                if (strncmp(section->sectname, section_name, 16) == 0)
                    sections.push_back(section);
                ++section;
            }
        }

        return sections;
    }

    template <typename Target_>
    Pointer<Target_> GetPointer(uint32_t address, const char *segment_name = NULL) {
        load_command *load_command = (struct load_command *) (mach_header_ + 1);
        uint32_t cmd;

        for (cmd = 0; cmd != Swap(mach_header_->ncmds); ++cmd) {
            if (Swap(load_command->cmd) == LC_SEGMENT) {
                segment_command *segment_command = (struct segment_command *) load_command;
                if (segment_name != NULL && strncmp(segment_command->segname, segment_name, 16) != 0)
                    goto next_command;

                section *sections = (struct section *) (segment_command + 1);

                uint32_t sect;
                for (sect = 0; sect != Swap(segment_command->nsects); ++sect) {
                    section *section = &sections[sect];
                    if (address >= Swap(section->addr) && address < Swap(section->addr) + Swap(section->size)) {
                        //printf("0x%.8x %s\n", address, segment_command->segname);
                        return Pointer<Target_>(this, reinterpret_cast<Target_ *>(address - Swap(section->addr) + Swap(section->offset) + (char *) mach_header_));
                    }
                }
            }

          next_command:
            load_command = (struct load_command *) ((char *) load_command + Swap(load_command->cmdsize));
        }

        return Pointer<Target_>(this);
    }

    template <typename Target_>
    Pointer<Target_> GetOffset(uint32_t offset) {
        return Pointer<Target_>(this, reinterpret_cast<Target_ *>(offset + (uint8_t *) mach_header_));
    }
};

template <typename Target_>
class Pointer {
  private:
    const Framework *framework_;
    const Target_ *pointer_;

  public:
    Pointer(const Framework *framework = NULL, const Target_ *pointer = NULL) :
        framework_(framework),
        pointer_(pointer)
    {
    }

    operator const Target_ *() const {
        return pointer_;
    }

    const Target_ *operator ->() const {
        return pointer_;
    }

    Pointer<Target_> &operator ++() {
        ++pointer_;
        return *this;
    }

    template <typename Value_>
    Value_ Swap(Value_ value) {
        return framework_->Swap(value);
    }
};

std::vector<Framework *> frameworks_;

template <typename Target_>
Pointer<Target_> GetPointer(uint32_t address, const char *segment_name = NULL) {
    _foreach (framework, frameworks_)
        if (Pointer<Target_> pointer = (*framework)->GetPointer<Target_>(address, segment_name))
            return pointer;
    return Pointer<Target_>();
}

sqlite3 *database_;

sqlite3_stmt *framework_stmt_;
sqlite3_stmt *class_stmt_;
sqlite3_stmt *variable_stmt_;
sqlite3_stmt *category_stmt_;
sqlite3_stmt *method_stmt_;

void each_method_list(Pointer<objc_method_list> objc_method_list) {
    if (objc_method_list != NULL) {
        size_t method;
        for (method = 0; method != static_cast<size_t>(objc_method_list.Swap(objc_method_list->method_count)); ++method) {
            const objc_method *objc_method = &objc_method_list->method_list[method];

            Pointer<char> method_name = GetPointer<char>(objc_method_list.Swap(objc_method->method_name));
            sqlcall(sqlite3_bind_string(method_stmt_, 4, method_name));

            Pointer<char> method_types = GetPointer<char>(objc_method_list.Swap(objc_method->method_types));
            sqlcall(sqlite3_bind_string(method_stmt_, 5, method_types));

            sqlcall(sqlite3_step(method_stmt_));
            sqlcall(sqlite3_reset(method_stmt_));
        }
    }
}

void each_class(Pointer<objc_class> objc_class, bool meta) {
    sqlcall(sqlite3_bind_boolean(method_stmt_, 3, meta));
    sqlcall(sqlite3_bind_boolean(variable_stmt_, 3, meta));

    Pointer<objc_method_list> objc_method_list = GetPointer<struct objc_method_list>(objc_class.Swap(objc_class->methodLists));
    each_method_list(objc_method_list);

    Pointer<struct objc_ivar_list> objc_ivar_list = GetPointer<struct objc_ivar_list>(objc_class.Swap(objc_class->ivars));
    if (objc_ivar_list != NULL) {
        size_t ivar_count = objc_ivar_list.Swap(objc_ivar_list->ivar_count);

        size_t ivar;
        for (ivar = 0; ivar != ivar_count; ++ivar) {
            const struct objc_ivar *objc_ivar = &objc_ivar_list->ivar_list[ivar];

            Pointer<char> ivar_name = GetPointer<char>(objc_ivar_list.Swap(objc_ivar->ivar_name));
            sqlcall(sqlite3_bind_string(variable_stmt_, 4, ivar_name));

            Pointer<char> ivar_type = GetPointer<char>(objc_ivar_list.Swap(objc_ivar->ivar_type));
            sqlcall(sqlite3_bind_string(variable_stmt_, 5, ivar_type));

            sqlcall(sqlite3_bind_int(variable_stmt_, 6, objc_ivar->ivar_offset));

            sqlcall(sqlite3_step(variable_stmt_));
            sqlcall(sqlite3_reset(variable_stmt_));
        }
    }
}

void import_class(bool newabi, Pointer<struct objc_class> objc_class) {
    _assert(objc_class != NULL);
    Pointer<struct objc_class> class_isa = GetPointer<struct objc_class>(objc_class.Swap(objc_class->isa));

    Pointer<char> super_class;
    Pointer<char> class_name;
    Pointer<char> root_class;

    if (newabi) {
        _assert(false);
    } else {
        super_class = GetPointer<char>(objc_class.Swap(objc_class->super_class));
        class_name = GetPointer<char>(objc_class.Swap(objc_class->name));
        root_class = GetPointer<char>(class_isa.Swap(class_isa->isa));
    }

    sqlcall(sqlite3_bind_string(class_stmt_, 3, super_class));
    sqlcall(sqlite3_bind_string(class_stmt_, 4, class_name));
    sqlcall(sqlite3_bind_string(class_stmt_, 5, root_class));

    sqlcall(sqlite3_step(class_stmt_));
    sqlcall(sqlite3_reset(class_stmt_));

    int class_id = sqlite3_last_insert_rowid(database_);
    sqlcall(sqlite3_bind_int(variable_stmt_, 2, class_id));

    sqlcall(sqlite3_bind_null(category_stmt_, 3));
    sqlcall(sqlite3_bind_string(category_stmt_, 4, class_name));

    sqlcall(sqlite3_step(category_stmt_));
    sqlcall(sqlite3_reset(category_stmt_));

    int category_id = sqlite3_last_insert_rowid(database_);
    sqlcall(sqlite3_bind_int(method_stmt_, 2, category_id));

    each_class(objc_class, false);
    each_class(class_isa, true);

    _assert(class_isa->instance_size == 48);
}

void import_category(bool newabi, Pointer<struct objc_category> objc_category) {
    Pointer<char> category_name;
    Pointer<char> class_name;

    if (newabi) {
        _assert(false);
    } else {
        category_name = GetPointer<char>(objc_category.Swap(objc_category->category_name));
        class_name = GetPointer<char>(objc_category.Swap(objc_category->class_name));
    }

    sqlcall(sqlite3_bind_string(category_stmt_, 3, category_name));
    sqlcall(sqlite3_bind_string(category_stmt_, 4, class_name));

    sqlcall(sqlite3_step(category_stmt_));
    sqlcall(sqlite3_reset(category_stmt_));

    int category_id = sqlite3_last_insert_rowid(database_);
    sqlcall(sqlite3_bind_int(method_stmt_, 2, category_id));

    Pointer<struct objc_method_list> category_instances_methods = GetPointer<struct objc_method_list>(objc_category.Swap(objc_category->instances_methods));
    sqlcall(sqlite3_bind_boolean(method_stmt_, 3, false));
    each_method_list(category_instances_methods);

    Pointer<struct objc_method_list> category_class_methods = GetPointer<struct objc_method_list>(objc_category.Swap(objc_category->class_methods));
    sqlcall(sqlite3_bind_boolean(method_stmt_, 3, true));
    each_method_list(category_class_methods);
}

int main(int argc, const char *argv[]) {
    _assert(argc == 4);

    const char *framework_name = argv[1];
    const char *framework_root = argv[2];
    const char *framework_path = argv[3];

    sqlcall(sqlite3_open("joc.db", &database_));

    sqlcall(sqlite3_prepare(database_, "insert into \"framework\" (\"id\", \"name\", \"root\", \"path\") values (?, ?, ?, ?)", -1, &framework_stmt_, NULL));
    sqlcall(sqlite3_bind_null(framework_stmt_, 1));

    sqlcall(sqlite3_prepare(database_, "insert into \"class\" (\"id\", \"framework\", \"super\", \"name\", \"root\") values (?, ?, ?, ?, ?)", -1, &class_stmt_, NULL));
    sqlcall(sqlite3_bind_null(class_stmt_, 1));

    sqlcall(sqlite3_prepare(database_, "insert into \"category\" (\"id\", \"framework\", \"name\", \"class\") values (?, ?, ?, ?)", -1, &category_stmt_, NULL));
    sqlcall(sqlite3_bind_null(category_stmt_, 1));

    sqlcall(sqlite3_prepare(database_, "insert into \"variable\" (\"id\", \"class\", \"static\", \"name\", \"type\", \"offset\") values (?, ?, ?, ?, ?, ?)", -1, &variable_stmt_, NULL));
    sqlcall(sqlite3_bind_null(variable_stmt_, 1));

    sqlcall(sqlite3_prepare(database_, "insert into \"method\" (\"id\", \"category\", \"static\", \"name\", \"types\") values (?, ?, ?, ?, ?)", -1, &method_stmt_, NULL));
    sqlcall(sqlite3_bind_null(method_stmt_, 1));

    sqlcall(sqlite3_exec(database_, "begin", NULL, NULL, NULL));

    char *framework_file;
    asprintf(&framework_file, "%s%s", framework_root, framework_path);

    Framework framework(framework_file);
    frameworks_.push_back(&framework);

    sqlcall(sqlite3_bind_string(framework_stmt_, 2, framework_name));
    sqlcall(sqlite3_bind_string(framework_stmt_, 3, framework_root));
    sqlcall(sqlite3_bind_string(framework_stmt_, 4, framework_path));

    sqlcall(sqlite3_step(framework_stmt_));
    sqlcall(sqlite3_reset(framework_stmt_));

    int framework_id = sqlite3_last_insert_rowid(database_);
    sqlcall(sqlite3_bind_int(class_stmt_, 2, framework_id));
    sqlcall(sqlite3_bind_int(category_stmt_, 2, framework_id));

    _foreach (classlist_section, framework.GetSections(SEG_DATA, SECT_DATA_OBJC_CLASSLIST)) {
        size_t i, count = framework.Swap((*classlist_section)->size) / sizeof(uint32_t);
        Pointer<uint32_t> objc_classlist = framework.GetOffset<uint32_t>(framework.Swap((*classlist_section)->offset));
        for (i = 0; i != count; ++i) {
            Pointer<objc_class> objc_class = GetPointer<struct objc_class>(objc_classlist.Swap(objc_classlist[i]));
            import_class(true, objc_class);
        }
    }

    _foreach (catlist_section, framework.GetSections(SEG_DATA, SECT_DATA_OBJC_CATLIST)) {
        size_t i, count = framework.Swap((*catlist_section)->size) / sizeof(uint32_t);
        Pointer<uint32_t> objc_catlist = framework.GetOffset<uint32_t>(framework.Swap((*catlist_section)->offset));
        for (i = 0; i != count; ++i) {
            Pointer<struct objc_category> objc_category = GetPointer<struct objc_category>(objc_catlist.Swap(objc_catlist[i]));
            import_category(true, objc_category);
        }
    }

    _foreach (module_section, framework.GetSections(SEG_OBJC, SECT_OBJC_MODULES)) {
        size_t i, count = framework.Swap((*module_section)->size) / sizeof(struct objc_module);
        Pointer<struct objc_module> objc_module = framework.GetOffset<struct objc_module>(framework.Swap((*module_section)->offset));
        for (i = 0; i != count; ++i) {
            Pointer<struct objc_symtab> objc_symtab = GetPointer<struct objc_symtab>(objc_module.Swap(objc_module->symtab), SEG_OBJC);
            if (objc_symtab == NULL)
                goto next_module;

            uint32_t def;
            for (def = 0; def != objc_symtab.Swap(objc_symtab->cls_def_cnt); ++def) {
                Pointer<struct objc_class> objc_class = GetPointer<struct objc_class>(objc_symtab.Swap(objc_symtab->defs[def]));
                import_class(false, objc_class);
            }

            for (; def != objc_symtab.Swap(objc_symtab->cls_def_cnt) + objc_symtab.Swap(objc_symtab->cat_def_cnt); ++def) {
                Pointer<struct objc_category> objc_category = GetPointer<struct objc_category>(objc_symtab.Swap(objc_symtab->defs[def]));
                import_category(false, objc_category);
            }

          next_module:
            ++objc_module;
        }
    }

    sqlite3_exec(database_, "commit", NULL, NULL, NULL);

    sqlcall(sqlite3_finalize(method_stmt_));
    sqlcall(sqlite3_finalize(variable_stmt_));
    sqlcall(sqlite3_finalize(category_stmt_));
    sqlcall(sqlite3_finalize(class_stmt_));
    sqlcall(sqlite3_finalize(framework_stmt_));

    sqlcall(sqlite3_close(database_));

    return 0;
}
