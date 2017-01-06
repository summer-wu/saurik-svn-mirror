/*DROP TABLE fs_user;
DROP TABLE fs_image;
DROP TABLE fs_mimemessage_header;
DROP TABLE fs_mimemessage;
DROP TABLE fs_multipart_bodypart;
DROP TABLE fs_multipart;
DROP TABLE fs_text;
DROP TABLE fs_filename;
DROP TABLE fs_directory;
DROP TABLE fs_contenttype_parameter;
DROP TABLE fs_contenttype;
DROP TABLE fs_file;
DROP TABLE fs_entity;*/

CREATE TABLE entity (
    id            UNIQUEIDENTIFIER  PRIMARY KEY,
    created       TIMESTAMP         NOT NULL                                        DEFAULT NOW(),
    modified      TIMESTAMP         NOT NULL                                        DEFAULT NOW()
    /*deleted       TIMESTAMP*/
);

CREATE TABLE data (
    id            UNIQUEIDENTIFIER  PRIMARY KEY REFERENCES entity,
    data          OID               NOT NULL,
    length        BIGINT            NOT NULL,
    sha1hash      BYTEA             NOT NULL,
    md5hash       BYTEA             NOT NULL,
    UNIQUE (length, sha1hash, md5hash)
);

CREATE TABLE object (
    id            UNIQUEIDENTIFIER  PRIMARY KEY REFERENCES entity,
    serialized    BYTEA             NOT NULL
);

CREATE TABLE filetype (
    id            UNIQUEIDENTIFIER  PRIMARY KEY REFERENCES entity,
    base          UNIQUEIDENTIFIER              REFERENCES filetype,
    mimetype      TEXT
);

CREATE TABLE document (
    id            UNIQUEIDENTIFIER  PRIMARY KEY REFERENCES entity,
    /*reader        UNIQUEIDENTIFIER  NOT NULL    REFERENCES object,*/
    data          UNIQUEIDENTIFIER  NOT NULL    REFERENCES data,
    /*filetype      UNIQUEIDENTIFIER  NOT NULL    REFERENCES filetype*/
    mimetype      TEXT              NOT NULL
);


CREATE TABLE name (
    id            UNIQUEIDENTIFIER  PRIMARY KEY REFERENCES entity,
    namespace     TEXT              NOT NULL,
    localname     TEXT              NOT NULL,
    UNIQUE (namespace, localname)
);

CREATE TABLE property (
    id            UNIQUEIDENTIFIER  PRIMARY KEY REFERENCES entity,
    parent        UNIQUEIDENTIFIER              REFERENCES entity,
    name          UNIQUEIDENTIFIER  NOT NULL    REFERENCES name,
    value         TEXT              NOT NULL,
    UNIQUE (parent, name)
);

CREATE TABLE node (
    id            UNIQUEIDENTIFIER  PRIMARY KEY REFERENCES entity,
    document      UNIQUEIDENTIFIER  NOT NULL    REFERENCES node,
    parent        UNIQUEIDENTIFIER              REFERENCES node,
    position      INTEGER           NOT NULL,
    next          INTEGER           NOT NULL,
    UNIQUE (document, position)
);

CREATE TABLE element (
    id            UNIQUEIDENTIFIER  PRIMARY KEY REFERENCES node,
    name          UNIQUEIDENTIFIER  NOT NULL    REFERENCES name,
    attribute     BOOLEAN           NOT NULL
);

CREATE TABLE text (
    id            UNIQUEIDENTIFIER  PRIMARY KEY REFERENCES node,
    value         TEXT              NOT NULL,
    type          INTEGER           NOT NULL
);

CREATE TABLE xml (
    id            UNIQUEIDENTIFIER  PRIMARY KEY REFERENCES entity,
    data          UNIQUEIDENTIFIER  NOT NULL    REFERENCES data,
    root          UNIQUEIDENTIFIER  NOT NULL    REFERENCES node
);
