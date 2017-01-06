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

create table "framework" (
    "id" integer not null
        primary key,
    "name" text not null,
    "root" text not null,
    "path" text not null
);

create table "dependency" (
    "id" integer not null
        primary key,
    "framework" integer not null,
    "name" text not null
);

create table "class" (
    "id" integer not null
        primary key,
    "framework" integer not null,
    "super" text null,
    "name" text not null,
    "root" text null
);

create table "category" (
    "id" integer not null
        primary key,
    "framework" integer not null,
    "name" text null,
    "class" text not null
);

create table "method" (
    "id" integer not null
        primary key,
    "category" integer not null,
    "static" boolean not null,
    "name" text not null,
    "types" text not null
);

create table "variable" (
    "id" integer not null
        primary key,
    "class" integer not null,
    "static" boolean not null,
    "name" text not null,
    "type" text not null,
    "offset" integer not null
);

create table "struct" (
    "id" integer not null
        primary key,
    "name" text null,
    "types" text null
);

/*create table "field" (
    "id" integer not null
        primary key,
    "struct" integer not null,
    "name" text not null,
    "type" text not null
);*/

create unique index "framework_name" on "framework" ("name");
create unique index "class_name" on "class" ("name");
create index "category_class" on "category" ("class");
create index "method_name" on "method" ("name");
create index "struct_name" on "struct" ("name");
