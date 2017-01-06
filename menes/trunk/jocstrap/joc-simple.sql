begin;

/* handle category overrides */
delete from "method" where "id" in (
    select distinct lm.id
    from method as lm
    join method as rm on
        lm.name = rm.name
    join category as lc on
        lc.id = lm.category
    join category as rc on
        rc.id = rm.category and
        lc.class = rc.class
    where
        lm.static = rm.static and lc.id < rc.id
);

/* XXX: support C++ templates */
/*delete from "variable" where "type" like '%<%';
delete from "method" where "types" like '%<%';*/

insert into "include" ("name", "header") values ('CGPoint', 'CoreGraphics/CGGeometry.h');
insert into "include" ("name", "header") values ('CGRect', 'CoreGraphics/CGGeometry.h');
insert into "include" ("name", "header") values ('CGSize', 'CoreGraphics/CGGeometry.h');
insert into "include" ("name", "header") values ('_NSRange', 'Foundation/NSRange.h');
insert into "include" ("name", "header") values ('NSObject', 'Foundation/NSObject.h');
insert into "include" ("name", "header") values ('CGAffineTransform', 'CoreGraphics/CGAffineTransform.h');
commit;
