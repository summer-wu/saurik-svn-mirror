DROP TABLE subjects;
CREATE TABLE subjects (
    code        TEXT    PRIMARY KEY,
    name        TEXT    NOT NULL
);

DROP TABLE instructors;
CREATE TABLE instructors (
    code        TEXT    PRIMARY KEY,
    name        TEXT    NOT NULL,
    subjects    TEXT    NOT NULL
);

DROP USER platinum;
CREATE USER platinum WITH PASSWORD 'zdbvi8wnvs5d8b' NOCREATEDB NOCREATEUSER;

GRANT ALL ON subjects, instructors TO platinum;
