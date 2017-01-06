/* Diapexis - Portable COM Reimplementation
 * Copyright (C) 2001-2005  Jay Freeman (saurik)
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

#include "api/exename.hpp"
#include "api/files.hpp"
#include "api/module.hpp"
#include "api/stacktrace.hpp"
#include "api/uuid.hpp"

#include "app/configuration.hpp"
#include "app/setup.hpp"

#include "com/registrar.hpp"
#include "com/reports.hpp"

#include "ext/foreach.hpp"
#include "ext/listset.hpp"
#include "ext/map.hpp"
#include "ext/redblackset.hpp"
#include "ext/singleton.hpp"

#include "ios/printwriter.hpp"
#include "log/logging.hpp"

#include "opt/memory.hpp"
#include "opt/profile.hpp"

#include <typeinfo> //OK

namespace com {

ClassNotFoundException::ClassNotFoundException(const ext::Uuid &id) :
    ext::StringException(lexical_cast<cse::String>(id))
{
}

ClassNotFoundException::ClassNotFoundException(const cse::String &name) :
    ext::StringException(name)
{
}

namespace {

// XXX: this should probably be handles to ClassFactory again...
typedef ext::RedBlackMap<clsid_t, ClassFactory *> FactoryMap_;
typedef ext::RedBlackMap<cse::String, clsid_t> ClassMap_;
typedef ext::ListSet< _R<api::Module> > LibrarySet_;

typedef ext::RedBlackSet<Object *> Objects_;

struct Register_ {
    FactoryMap_ factories_;
    ClassMap_ classes_;
    LibrarySet_ libraries_;

    Objects_ objects_;

    const bool trackleaks_;

    Register_() :
        trackleaks_(_C("menes/com/tracking/@leaks", false))
    {
        opt::SetupAllocation();
    }
};

}

typedef void (*FPEXIFY)();
typedef void (*FDEPEXIFY)();

namespace {
    struct ReferenceInfo_ {
        const std::type_info *type_;

        typedef ext::RedBlackSet<Object *> Parents_;
        Parents_ parents_;

        unsigned count_;

        ReferenceInfo_() :
            type_(NULL),
            count_(0)
        {
        }
    };

    typedef ext::RedBlackMap<Object *, ReferenceInfo_> References_;

    void PrintReferences(ios::PrintWriter &out, References_ &references, Object *parent, unsigned depth = 0) {
        _foreach (References_, reference, references) {
            const ReferenceInfo_ &info(reference->Second());
            if (
                parent == NULL && info.parents_.IsEmpty() ||
                parent != NULL && info.parents_.Contains(parent)
            ) {
                out << ios::SetWidth(depth * 3) << "";
                out << ios::SetFill('0') << ios::SetBase(16);
                out << "0x" << ios::SetWidth(8) << reference->First();
                out << ios::SetFill(' ') << ios::SetBase(10);
                out << "[" << info.count_ << "]: " << info.type_->name() << ios::NewLine;

                PrintReferences(out, references, reference->First(), depth + 1);
            }
        }
    }

    class PerProcess :
        public app::PerProcess
    {
      public:
        virtual void Start() {
            Load();
        }

        virtual void Stop() {
        }
    } perprocess_;
}

void Terminate() {
    _1<Register_>().factories_.Clear();

    {
        References_ references;

        typedef ext::RedBlackMap<const std::type_info *, unsigned> Counts_;
        Counts_ counts;

        for (Objects_::Iterator object(_1<Register_>().objects_.Begin()); object != _1<Register_>().objects_.End(); ++object) {
            ReferenceInfo_ &info(references[*object]);
            info.type_ = &typeid(**object);
            ++counts[info.type_];
            // XXX: info.count_ = GetRefCount_(*object);
            //LOG_THREAD() << "Leaked: " << info.type_->name() << ios::NewLine;
        }

        /*while (!_1<Register_>().objects_.IsEmpty()) {
            Object *object(*std::min_element(_1<Register_>().objects_.Begin(), _1<Register_>().objects_.End(), RefCountPredicate_()));

            while (object->Release() != 0);

            for (References_::Iterator reference(references.Begin()); reference != references.End(); ++reference) {
                ReferenceInfo_ &info(reference->second);
                if (info.after_ == 0)
                    continue;

                if (_1<Register_>().objects_.find(reference->first) == _1<Register_>().objects_.End()) {
                    info.after_ = 0;
                    if (reference->first == object)
                        continue;
                } else {
                    refcn_t count(GetRefCount_(reference->first));
                    if (count == info.after_)
                        continue;
                    info.after_ = count;
                }

                info.parents_.Insert(object);
            }
        }*/

        _S<api::FileWriter> file(_S<ios::String>() << api::GetExecutablePath().GetParent().GetPath() << "/ObjectLeaks.log");
        _S<ios::FormatWriter> out(file);

        /*out << "==============================================================================" << ios::NewLine;
        out << "--- Tree Listing =============================================================" << ios::NewLine;
        out << ios::NewLine;

        PrintReferences(out, references, NULL);
        out << ios::NewLine;*/

        out << "==============================================================================" << ios::NewLine;
        out << "--- Summarized Listing =======================================================" << ios::NewLine;
        out << ios::NewLine;

        _foreach (Counts_, count, counts)
            out << ios::SetWidth(6) << count->Second() << ": " << count->First()->name() << ios::NewLine;
        out << ios::NewLine;

        out << "==============================================================================" << ios::NewLine;
        out << "--- Full Listing =============================================================" << ios::NewLine;
        out << ios::NewLine;

        _foreach (References_, reference, references) {
            const ReferenceInfo_ &info(reference->Second());

            out << ios::SetFill('0') << ios::SetBase(16);
            out << "0x" << ios::SetWidth(8) << reference->First();
            out << ios::SetFill(' ') << ios::SetBase(10);
            out << "[" << info.count_ << "]: " << info.type_->name() << ios::NewLine;

            if (!info.parents_.IsEmpty()) {
                out << "   Parents:";
                _foreach (const ReferenceInfo_::Parents_, parent, info.parents_) {
                    out << ios::SetFill('0') << ios::SetBase(16);
                    out << " 0x" << ios::SetWidth(8) << *parent;
                    out << ios::SetFill(' ') << ios::SetBase(10);
                }
                out << ios::NewLine;
            }
        }
    }

    _1<Register_>().classes_.Clear();
    _1<Register_>().libraries_.Clear();
}

namespace be {
    const std::type_info *type_;
}

void ForwardTally(Object *object) {
    if (&typeid(object) == be::type_)
        _breakpoint();
    if (_1<Register_>().trackleaks_)
        _1<Register_>().objects_.Insert(object);
}

void ReverseTally(Object *object) {
    if (_1<Register_>().trackleaks_)
        _1<Register_>().objects_.Remove(object);
}

void Load() {
    cse::String base(api::GetExecutablePath().GetParent().GetPath());
    ext::Vector<cse::String> modules(app::GetConfigList<cse::String>(_B("menes/com/loadlibrary/library")));
    _foreach (ext::Vector<cse::String>, module, modules)
        Load(_S<ios::String>() << base << "/" << *module);
}

void Load(const cse::String &path) {
    _1<Register_>().libraries_.Insert(new _H<api::Module>(path, api::Module::Extension));
}

clsid_t Register(const cse::String &name, clsid_t guid, const _R<ClassFactory> &factory) {
    if (guid == ext::Uuid::Null)
        guid = api::Uuid::CreateRandom();
    if (_1<Register_>().classes_.Insert(name, guid).First()->Second() != guid)
        return ext::Uuid::Null;
    if (!_1<Register_>().factories_.Insert(guid, factory).Second())
        return ext::Uuid::Null;
    return guid;
}

clsid_t Register(const char *name, clsid_t guid, const _R<ClassFactory> &factory) {
    return Register(cse::String(name, ext::ReadOnlyExternal), guid, factory);
}

clsid_t Resolve(const cse::String &name) {
    OPT_TIMER("com::Resolve()")
    ClassMap_::ConstIterator guid(_1<Register_>().classes_.Find(name));
    return guid == _1<Register_>().classes_.End() ? ext::Uuid::Null : guid->Second();
}

_R<ClassFactory> GetClassObject(clsid_t guid) {
    if (guid == ext::Uuid::Null)
        return NULL;

    FactoryMap_::ConstIterator factory(_1<Register_>().factories_.Find(guid));
    if (factory == _1<Register_>().factories_.End())
        return _R<ClassFactory>();

    return factory->Second();
}

_R<Object> CreateInstance(const cse::String &name) {
    OPT_TIMER("com::CreateInstance(string)")
    return CreateInstance(Resolve(name));
}

_R<Object> CreateInstance(clsid_t guid) {
    if (guid == ext::Uuid::Null)
        return _R<Object>();

    return GetClassObject(guid)->Create();
}

}
