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

#ifndef MENES_DBI_MYSQL_DRIVER_HPP
#define MENES_DBI_MYSQL_DRIVER_HPP

#include "cxx/platform.hpp"
#include "dbi/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "com/metadata.hpp"
#include "com/registrar.hpp"

#include "dbi/driver.hpp"

namespace dbi {
namespace MySql {

class Connection;

class Driver :
    public dbi::Driver
{
    COM_CLASSNAME("dbi::MySql::Driver")
    COM_CLASSUUID(58,36,db,06,e1,e1,11,d9,8d,ee,00,04,23,88,3b,9f)

    COM_CLASSMETADATA()
    COM_DYNAMICINSTANCE()

  public:
    virtual _R<dbi::Connection> Connect(const cse::String &hostname, const cse::String &username, const cse::String &password, const cse::String &dbname);
};

} }

#endif//MENES_DBI_MYSQL_CONNETION_HPP
