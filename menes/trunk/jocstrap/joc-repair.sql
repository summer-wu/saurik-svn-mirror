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
        lm.static = rm.static and lc.id < rc.id or
        lm.static and not rm.static
);

/* XXX: support C++ templates */
delete from "variable" where "type" like '%<%';
delete from "method" where "types" like '%<%';

/* XXX: anonymous structs */
delete from "variable" where "type" like '%{?%';
delete from "method" where "types" like '%{?%';

/* XXX: structs defined later in the same type */
delete from "variable" where "type" like '%DeprecatedStringData%';
delete from "method" where "types" like '%DeprecatedStringData%';

update "method" set
    "types" = '@"' || (
        select "category"."class"
        from "category"
        where "category"."id" = "method"."category"
    ) || '"' || substr("types", 2, -1)
where
    ("name" = 'init' or "name" glob 'init[A-Z]*') and
    "types" glob '@[^"]*';

/* these methods cause a load of trouble that I have no clue about :( */
delete from "method" where "name" in (
    'position'
);

/* this field mentions a class that I don't have a definition for */
delete from "variable"
where (
    select "name"
    from "class"
    where "class"."id" = "variable"."class"
) = 'AVExternalAudio' and
    "variable"."name" = '_priv';

/* OfficeImport actually thinks it is providing this */
delete from "class" where "name" = 'NSFontManager';

/* int->void is contra-variant... Apple needs to pay more attention */
update "method" set "types" = 'v8@0:4' where "name" = 'fetchSynchronously';

/* near as I can tell, this should return a boolean, but people keep waffling between BOOL (char) and unsigned int */
update "method" set "types" = 'B8@0:4' where "name" = 'becomeFirstResponder';

/* EDProtection is compiled with someone who supports actual booleans */
update "method" set "types" = 'c8@0:4' where "name" = 'isLocked';

/* ISRequest (the interface) says char, but everyone else says void */
update "method" set "types" = 'v8@0:4' where "name" = 'resume';

/* MusicNowPlayingArtLayer bucks the UIKit trend */
update "method" set "types" = 'i16@0:4i8^{__GSEvent=}12' where "name" = 'swipe:withEvent:';

/* PSDetailController is the one oddball, but I have to be covariant */
update "method" set "types" = 'v8@0:4' where "name" = 'popController';

/* NSDistantObject requires class to return id... it sucks */
update "method" set "types" = '@8@0:4' where "name" = 'class';

/* NSNumber and NSValue are doing something weird here? */
update "method" set "types" = '#8@0:4' where "name" = 'classForCoder';

/* I have no clue what type this should be... but damned if it isn't freaky */
update "method" set "types" = '@12@0:4^v8' where "name" = 'initWithHitTestResult:' and "types" = '@12@0:48';

/* these (unused) classes can't be released once allocated... */
delete from "class" where "name" in ('NSMessageBuilder', 'NSInvocationBuilder', 'WebDownload');

/* UIScroller doesn't like GSEvent as much as UI*Table does */
update "method" set "types" = 'i16@0:4i8^{__GSEvent=}12' where "name" = 'contentMouseUpInView:withEvent:';

/* NSString's subclasses use char * instead of void * */
update "method" set "types" = 'c40@0:4^v8I12^I16I20I24{_NSRange=II}28^{_NSRange=II}36' where "name" = 'getBytes:maxLength:usedLength:encoding:options:range:remainingRange:';

/* both __mvFlags and __MVFlags exist, and Mac is case-insensitive... I hate Apple */
update "variable" set "type" = 'I' where "type" like '%{__MFlags=%';
update "variable" set "type" = 'I' where "type" like '%{__MVFlags=%';

commit;
