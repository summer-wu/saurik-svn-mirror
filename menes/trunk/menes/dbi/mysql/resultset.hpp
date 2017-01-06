/* Menes Database - Database Abstraction Layer
 * Copyright (C) 2004-2005  Jay Freeman (saurik)
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

#ifndef MENES_DBI_MYSQL_RESULTSET_HPP
#define MENES_DBI_MYSQL_RESULTSET_HPP

#include "cxx/platform.hpp"
#include "dbi/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "dbi/resultset.hpp"

#include "mysql_version.h"
#include "mysql.h"
#include "dbi/mysql/undefine.hpp"

namespace dbi {
namespace MySql {

class ResultSet :
    public dbi::ResultSet
{
  public:
    using dbi::ResultSet::GetString;
    using dbi::ResultSet::IsNull;

  private:
    MYSQL_RES *result_;

    MYSQL_ROW row_;

  public:
    ResultSet(MYSQL_RES *result);
    ~ResultSet();

    virtual cse::String GetNameByIndex(unsigned column) const;
    virtual unsigned GetIndexByName(const cse::String &name) const;

    virtual bool IsNull(unsigned column) const;

    virtual _R<dbi::Blob> GetBlob(unsigned column) const;
    virtual cse::String GetString(unsigned column) const;

    virtual bool MoveNext();
};

} }

#endif//MENES_DBI_MYSQL_RESULTSET_HPP
