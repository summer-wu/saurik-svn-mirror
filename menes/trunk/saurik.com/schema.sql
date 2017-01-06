#!/bin/bash
psql saurik -f <(echo; echo; cat <<EOF

set standard_conforming_strings = on;

/*drop table "version";
drop table "data";
drop table "file";*/

drop table "tag";
drop table "node";
drop table "legacy";
drop table "request";

create table "request" (
    "id" bigserial not null
        primary key,
    "start" timestamp not null
        default now(),
    "client" inet not null,
    "agent" text null,
    "method" text not null,
    "url" text not null,
    "query" text null,
    "finish" timestamp null,
    "status" integer null,
    "referrer" text null
);

create table "legacy" (
    "pattern" text not null,
    "location" text not null
);

insert into "legacy" (
    "pattern",
    "location"
) values (
    '^\/net\/exemplar(\/.*)?$',
    '/anakrino'
);

create table "node" (
    "id" serial not null
        primary key,
    "uri" text null,
    "birth" timestamp not null
        default now(),
    "death" timestamp null
);

insert into "node" ("id", "uri") values (0, '');
insert into "node" ("uri") values ('iphone/cname');

create table "tag" (
    "id" serial not null
        primary key,
    "node" integer not null
        references "node",
    "tag" text not null,
    "birth" timestamp not null
        default now(),
    "death" timestamp null
);

/*create table "file" (
    "id" bigserial not null
        primary key,
    "type" text not null
);

create table "data" (
    "id" bigserial not null
        primary key,
    "birth" timestamp not null
        default now(),
    "object" oid not null
);

create table "version" (
    "id" bigserial not null
        primary key,
    "file" bigint not null
        references "file",
    "time" timestamp not null
        default now(),
    "data" bigint not null
        references "data"
);*/

EOF
) 2>&1 | sed -e 's/^psql:[^:]*:/schema.sql:/'
