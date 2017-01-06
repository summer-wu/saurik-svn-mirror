DROP TABLE courses;
DROP TABLE meetings;
DROP TABLE instructors;
DROP TABLE subjects;

CREATE TABLE subjects (
    id            UNIQUEIDENTIFIER  PRIMARY KEY                                     DEFAULT NEWID(),
    major         TEXT              NOT NULL,
    minor         TEXT,
    name          TEXT              NOT NULL,
    UNIQUE (major, minor)
);

CREATE TABLE instructors (
    id            UNIQUEIDENTIFIER  PRIMARY KEY                                     DEFAULT NEWID(),
    code          TEXT              NOT NULL,
    name          TEXT              NOT NULL
);

/*CREATE TABLE meetings (
    id            UNIQUEIDENTIFIER  PRIMARY KEY                                     DEFAULT NEWID(),
    start         TIMESTAMP,
    end           TIMESTAMP,
    building      TEXT,
    room          TEXT
);*/

CREATE TABLE courses (
    id            UNIQUEIDENTIFIER  PRIMARY KEY                                     DEFAULT NEWID(),
    quarter       TEXT              NOT NULL,
    subject       UNIQUEIDENTIFIER  NOT NULL    REFERENCES subjects,
    number        TEXT              NOT NULL,
    name          TEXT              NOT NULL
);

CREATE TABLE discussions (
    id            UNIQUEIDENTIFIER  PRIMARY KEY                                     DEFAULT NEWID(),
    course        UNIQUEIDENTIFIER  NOT NULL    REFERENCES courses,
    code          TEXT              NOT NULL
);
