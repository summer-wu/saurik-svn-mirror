#!/bin/bash

if [[ $# -ne 2 ]]; then
    echo "usage: ./wikicyte.sql <database> <username>" 1>&2
    exit 1
fi

db=$1
un=$2

psql "${db}" -U "${un}" -f <(echo; echo; cat <<\EOF

set standard_conforming_strings = on;

create schema "wikicyte";

create table "wikicyte"."search" (
    "query" text not null,
    "host" inet not null,
    "when" timestamp with time zone not null
        default now()
);

create table "wikicyte"."page" (
    "id" bigserial not null
        primary key,
    "uri" text not null,

    "user" text null,
    "host" inet null,
    "when" timestamp with time zone null
        default now(),

    "from" bigint null
        references "wikicyte"."page",
    "then" timestamp with time zone null,

    "data" text null,
    "search" tsvector null
);

-- The tsearch2 full text search backend that PostgreSQL
-- comes with uses special "GiST" indices to rapidly scan
-- for values that match specific word expressions.

create index "page(search)" on "wikicyte"."page" using gist("search");

create table "wikicyte"."status" (
    "page" bigint not null
        references "wikicyte"."page",

    "user" text null,
    "host" inet null,
    "when" timestamp with time zone null
        default now(),

    "approved" boolean null
);

create table "wikicyte"."redirect" (
    "before" text not null,
    "after" text not null,
    "when" timestamp with time zone null
        default now()
);

-- This function serves as a kind of parameterized view,
-- allowing you to get a list of all pages in the system
-- and their approval state /as of a specific timestamp/.
--
-- Unfortunately, PostgreSQL has to do a "function scan"
-- over this "view" as it isnt /actually/ a view :.
--
-- This means that the performance of this structure is
-- quite poor. Note that we are still only talking like
-- 13ms for the current site, but it is something that
-- should be kept in mind.
--
-- In specific, full text search queries against functions
-- cannot be optimized by our GiST index. That sucks.

create function "wikicyte"."approved"(
    timestamp with time zone,
    out "id" bigint,
    out "uri" text,
    out "when" timestamp with time zone,
    out "data" text,
    out "search" tsvector,
    out "from" bigint,
    out "user" text,
    out "host" inet,
    out "approved" boolean
) returns setof record as $$
    select
        "page"."id",
        "page"."uri",
        "page"."when",
        "page"."data",
        "page"."search",
        "page"."from",
        "page"."user",
        "page"."host",
        "status"."approved"
    from "wikicyte"."status"
    join (
        select
            "page",
            max("when") as "when"
        from "wikicyte"."status"
        where
            "when" <= $1
        group by "page"
    ) as "status_" on
        "status_"."page" = "status"."page" and
        "status_"."when" = "status"."when"
    right join "wikicyte"."page" on
        "page"."id" = "status"."page"
    where (
        "page"."when" <= $1 or
        "page"."when" is null
    )
$$ LANGUAGE SQL;

create function "wikicyte"."current"(
    timestamp with time zone,
    out "id" bigint,
    out "uri" text,
    out "when" timestamp with time zone,
    out "data" text,
    out "search" tsvector,
    out "from" bigint,
    out "user" text,
    out "host" inet
) returns setof record as $$
    select
        "page"."id",
        "page"."uri",
        "page"."when",
        "page"."data",
        "page"."search",
        "page"."from",
        "page"."user",
        "page"."host"
    from (
        select
            "uri",
            max("when") as "when"
        from "wikicyte"."approved"($1)
        where
            ("approved" is null or "approved")
        group by "uri"
    ) as "current"
    join "wikicyte"."page" on
        "page"."uri" = "current"."uri" and (
            "page"."when" = "current"."when" or
            "page"."when" is null and "current"."when" is null
        )
    order by "page"."uri"
$$ LANGUAGE SQL;

EOF
) 2>&1 #| sed -e 's@^psql:[^:]*:@'"$0"':@'
