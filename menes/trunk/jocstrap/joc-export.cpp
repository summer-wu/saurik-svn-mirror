/* Copyright & Warranty {{{ */
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
/* }}} */
/* Include Statements {{{ */
#include "minimal/stdlib.h"
#include "minimal/mapping.h"
#include "minimal/sqlite3.h"
#include "minimal/string.h"

#include "apr_hash.h"

#include <sqlite3.h>

#include <set>
#include <string>
#include <vector>

#include <cctype>
#include <cstring>

#include "sig/parse.h"
/* }}} */

char *java_escape(const char *name) {
    if (name == NULL)
        return NULL;
    else if (
        strcmp(name, "class") != 0 &&
        strcmp(name, "clone") != 0 &&
        strcmp(name, "continue") != 0 &&
        strcmp(name, "finalize") != 0 &&
        strcmp(name, "new") != 0 &&
        strcmp(name, "null") != 0 &&
        strcmp(name, "package") != 0 &&
        strcmp(name, "String") != 0 &&
        strcmp(name, "toString") != 0 &&
        strcmp(name, "wait") != 0
    )
        return strdup(name);
    else {
        char *escaped;
        asprintf(&escaped, "$%s", name);
        return escaped;
    }
}

/* Type Serialization {{{ */
void jni_type(FILE *file, struct sig_type *type) {
    switch (type->primitive) {
        case typename_P: fprintf(file, "jclass"); break;
        case union_P: fprintf(file, "jobject"); break;
        case string_P: fprintf(file, "jstring"); break;
        case selector_P: fprintf(file, "jobject"); break;
        case object_P: fprintf(file, "jobject"); break;
        case boolean_P: fprintf(file, "jboolean"); break;
        case uchar_P: fprintf(file, "jbyte"); break;
        case uint_P: fprintf(file, "jint"); break;
        case ulong_P: fprintf(file, "jint"); break;
        case ulonglong_P: fprintf(file, "jlong"); break;
        case ushort_P: fprintf(file, "jshort"); break;
        case array_P: fprintf(file, "jarray"); break;
        case pointer_P: fprintf(file, "jobject"); break;
        case bit_P: fprintf(file, "jint"); break;
        case char_P: fprintf(file, "jbyte"); break;
        case double_P: fprintf(file, "jdouble"); break;
        case float_P: fprintf(file, "jfloat"); break;
        case int_P: fprintf(file, "jint"); break;
        case long_P: fprintf(file, "jint"); break;
        case longlong_P: fprintf(file, "jlong"); break;
        case short_P: fprintf(file, "jshort"); break;
        case void_P: fprintf(file, "void"); break;
        case struct_P: fprintf(file, "jobject"); break;
    }
}

void aggregate(apr_pool_t *pool, enum joc_primitive primitive, const char *name, struct sig_signature *signature, const char *types);

char *objc_name_anonymous(apr_pool_t *pool, enum joc_primitive primitive, struct sig_signature *signature) {
    const char *types = sig_unparse_signature(pool, signature);
    ssize_t length = APR_HASH_KEY_STRING;
    unsigned hash = apr_hashfunc_default(types, &length);

    char *name;
    asprintf(&name, "_joc_0x%x", hash);
    aggregate(pool, primitive, name, signature, NULL);
    return name;
}

char *objc_type(apr_pool_t *pool, struct sig_type *type, char *name, unsigned indent = 0) {
    char *label = NULL;
    bool space(true);

    switch (type->primitive) {
        case typename_P:
            label = strdup("Class");
        break;

        case union_P:
            label = "union";
        goto aggregate;

        case string_P:
            space = false;
            label = strdup("char *");
        break;

        case selector_P:
            label = strdup("SEL");
        break;

        case object_P:
            if (type->name == NULL)
                label = strdup("id");
            else {
                space = false;
                asprintf(&label, "%s *", type->name);
            }
        break;

        case boolean_P:
            label = strdup("bool");
        break;

        case uchar_P:
            label = strdup("unsigned char");
        break;

        case uint_P:
            label = strdup("unsigned int");
        break;

        case ulong_P:
            label = strdup("unsigned long");
        break;

        case ulonglong_P:
            label = strdup("unsigned long long");
        break;

        case ushort_P:
            label = strdup("unsigned short");
        break;

        case array_P:
            if (name == NULL)
                asprintf(&label, "[%zu]", type->data.data.size);
            else {
                asprintf(&label, *name == '*' ? "(%s)[%zu]" : "%s[%zu]", name, type->data.data.size);
                free(name);
            }
        return objc_type(pool, type->data.data.type, label);

        case pointer_P:
            space = false;
            if (type->data.data.type == NULL)
                label = strdup("void *");
            else {
                if (name == NULL)
                    label = strdup("*");
                else {
                    asprintf(&label, "*%s", name);
                    free(name);
                }

                return objc_type(pool, type->data.data.type, label);
            }
        break;

        case bit_P:
            if (name == NULL)
                asprintf(&label, "int : %zu", type->data.data.size);
            else {
                asprintf(&label, "int %s : %zu", name, type->data.data.size);
                free(name);
            }
        return label;

        case char_P:
            label = strdup("char");
        break;

        case double_P:
            label = strdup("double");
        break;

        case float_P:
            label = strdup("float");
        break;

        case int_P:
            label = strdup("int");
        break;

        case long_P:
            label = strdup("long");
        break;

        case longlong_P:
            label = strdup("long long");
        break;

        case short_P:
            label = strdup("short");
        break;

        case void_P:
            label = strdup("void");
        break;

        case struct_P:
            label = "struct";
        goto aggregate;

        aggregate:
            if (type->name != NULL)
                asprintf(&label, "%s %s", label, type->name);
            else if (indent == 0) {
                char *name = objc_name_anonymous(pool, type->primitive, &type->data.signature);
                asprintf(&label, "%s %s", label, name);
                free(name);
            } else {
                char *fields = strdup("");

                for (size_t offset = 0; offset != type->data.signature.count; ++offset) {
                    struct sig_element *element = &type->data.signature.elements[offset];
                    char *name;
                    if (element->name != NULL)
                        name = strdup(element->name);
                    else
                        asprintf(&name, "_joc_%zu", offset);
                    name = objc_type(pool, element->type, name, indent + 1);
                    char *temp = fields;
                    asprintf(&fields, "%s%*s%s;\n", fields, (indent + 1) * 4, "", name);
                    free(temp);
                    free(name);
                }

                asprintf(&label, "%s {\n%s%*s}", label, fields, indent * 4, "");
            }
        break;
    }

    if (name == NULL)
        return label;
    else {
        char *value;
        asprintf(&value, space ? "%s %s" : "%s%s", label, name);
        free(name);
        return value;
    }
}

void java_type(apr_pool_t *pool, FILE *file, struct sig_type *type, bool reference, bool exact) {
    switch (type->primitive) {
        case typename_P: fprintf(file, "Class"); break;

        case union_P:
            goto aggregate;

        case string_P:
            fprintf(file, "joc.Pointer");
            if (exact)
                fprintf(file, "<Byte>");
        break;

        case selector_P: fprintf(file, "joc.Selector"); break;

        case object_P:
            if (type->name == NULL)
                fprintf(file, "Object");
            else
                fprintf(file, "%s", type->name);
        break;

        case boolean_P: fprintf(file, reference ? "java.lang.Boolean" : "boolean"); break;
        case uchar_P: fprintf(file, reference ? "java.lang.Byte" : "byte"); break;
        case uint_P: fprintf(file, reference ? "java.lang.Integer" : "int"); break;
        case ulong_P: fprintf(file, reference ? "java.lang.Integer" : "int"); break;
        case ulonglong_P: fprintf(file, reference ? " java.lang.Long" : "long"); break;
        case ushort_P: fprintf(file, reference ? "java.lang.Short" : "short"); break;

        case array_P:
            java_type(pool, file, type->data.data.type, false, exact);
            fprintf(file, "[]");
        break;

        case pointer_P:
            fprintf(file, "joc.Pointer");
            if (exact) {
                fprintf(file, "<");
                if (type->data.data.type == NULL)
                    fprintf(file, "?");
                else
                    java_type(pool, file, type->data.data.type, true, exact);
                fprintf(file, ">");
            }
        break;

        case bit_P: fprintf(file, reference ? "java.lang.Integer" : "int"); break;
        case char_P: fprintf(file, reference ? "java.lang.Byte" : "byte"); break;
        case double_P: fprintf(file, reference ? "java.lang.Double" : "double"); break;
        case float_P: fprintf(file, reference ? "java.lang.Float" : "float"); break;
        case int_P: fprintf(file, reference ? "java.lang.Integer" : "int"); break;
        case long_P: fprintf(file, reference ? "java.lang.Integer" : "int"); break;
        case longlong_P: fprintf(file, reference ? "java.lang.Long" : "long"); break;
        case short_P: fprintf(file, reference ? "java.lang.Short" : "short"); break;
        case void_P: fprintf(file, "void"); break;

        case struct_P:
            goto aggregate;

        aggregate: {
            char *struct_escape;
            if (type->name == NULL)
                struct_escape = objc_name_anonymous(pool, type->primitive, &type->data.signature);
            else {
                char *angle = strchr(type->name, '<');
                if (angle == NULL) {
                    struct_escape = java_escape(type->name);
                } else {
                    angle = strndup_(type->name, angle - type->name);
                    struct_escape = java_escape(angle);
                    free(angle);
                }
            }
            fprintf(file, "%s", struct_escape);
            free(struct_escape);
        } break;
    }
}

void cast_type(apr_pool_t *pool, FILE *file, struct sig_type *type, bool reference) {
    switch (type->primitive) {
        case boolean_P:
            if (reference)
                fprintf(file, "(boolean) (java.lang.Boolean) ");
        break;

        case bit_P:
            /* XXX: bit type */
            if (reference)
                fprintf(file, "(int) (java.lang.Integer) ");
        break;

        case char_P:
        case uchar_P:
            if (reference)
                fprintf(file, "(byte) (java.lang.Byte) ");
        break;

        case long_P:
        case ulong_P:
        case int_P:
        case uint_P:
            if (reference)
                fprintf(file, "(int) (java.lang.Integer) ");
        break;

        case longlong_P:
        case ulonglong_P:
            if (reference)
                fprintf(file, "(long) (java.lang.Long) ");
        break;

        case short_P:
        case ushort_P:
            if (reference)
                fprintf(file, "(short) (java.lang.Short) ");
        break;

        case float_P:
            if (reference)
                fprintf(file, "(float) (java.lang.Float) ");
        break;

        case double_P:
            if (reference)
                fprintf(file, "(double) (java.lang.Double) ");
        break;

        case object_P:
            if (type->name != NULL)
                fprintf(file, "(%s) ", type->name);
        break;

        case array_P:
            if (!reference)
                break;
        default:
            fprintf(file, "(");
            java_type(pool, file, type, false, true);
            fprintf(file, ") ");
        break;
    }
}

bool reference_type(struct sig_type *type) {
    switch (type->primitive) {
        case array_P:
        case object_P:
        case pointer_P:
        case string_P:
        case struct_P:
        case union_P:
            return true;
        default:
            return false;
    }
}

bool aggregate_type(struct sig_type *type) {
    switch (type->primitive) {
        case array_P:
        case struct_P:
        case union_P:
            return true;
        default:
            return false;
    }
}
/* }}} */

sqlite3 *database_;

sqlite3_stmt *framework_stmt_;
sqlite3_stmt *prototype_stmt_;
sqlite3_stmt *type_stmt_;
sqlite3_stmt *class_stmt_;
sqlite3_stmt *variable_stmt_;
sqlite3_stmt *method_stmt_;

sqlite3_stmt *struct_delete_stmt_;
sqlite3_stmt *struct_select_stmt_;
sqlite3_stmt *struct_insert_stmt_;

bool do_java_(true);
bool do_objc_(true);

/* Struct Serialization {{{ */
void struct_element(apr_pool_t *pool, void *baton, FILE *file, struct sig_element *element, char *prefix, char *name) {
    bool *comma = (bool *) baton;
    if (!*comma)
        *comma = true;
    else
        fprintf(file, ",");
    fprintf(file, "\n        ");
    java_type(pool, file, element->type, false, true);
    fprintf(file, " ");
    fprintf(file, prefix, '$');
    fprintf(file, "%s", name);
}

void struct_copy(apr_pool_t *pool, void *baton, FILE *file, struct sig_element *element, char *prefix, char *name) {
    fprintf(file, "        this.");
    fprintf(file, prefix, '.');
    fprintf(file, "%s = ", name);
    fprintf(file, prefix, '$');
    fprintf(file, "%s;\n", name);
}

char *field_name(struct sig_element *element, size_t offset) {
    if (element->name != NULL && *element->name != '\0')
        return java_escape(element->name);
    else {
        char *name;
        asprintf(&name, "field%zu", offset);
        return name;
    }
}

void struct_signature(apr_pool_t *pool, FILE *file, char *prefix, struct sig_signature *signature, void (*callback)(apr_pool_t *, void *, FILE *, struct sig_element *, char *, char *), void *baton) {
    size_t offset;
    for (offset = 0; offset != signature->count; ++offset) {
        struct sig_element *element = &signature->elements[offset];
        char *name = field_name(element, offset);
        if (
            element->type->primitive == struct_P ||
            element->type->primitive == union_P
        ) {
            char *postfix;
            asprintf(&postfix, "%s%s%%1$c", prefix, name);

            struct sig_signature signature;

            sqlcall(sqlite3_bind_string(struct_select_stmt_, 1, element->type->name));
            int struct_step;
            sqlcall(struct_step = sqlite3_step(struct_select_stmt_));

            if (struct_step == SQLITE_DONE) {
                signature = element->type->data.signature;
                sqlcall(sqlite3_reset(struct_select_stmt_));
            } else {
                char *types = sqlite3_column_string(struct_select_stmt_, 0);
                sqlcall(sqlite3_reset(struct_select_stmt_));
                if (types == NULL)
                    goto done;
                sig_parse(pool, &signature, types);
                free(types);
            }

            struct_signature(pool, file, postfix, &signature, callback, baton);
          done:
            free(postfix);
        } else {
            callback(pool, baton, file, element, prefix, name);
        }
        free(name);
    }
}

void objc_print_prototype(FILE *objc, std::set<std::string> *structs, std::set<std::string> *classes) {
    _foreach (name, *structs)
        fprintf(objc, "#include <struct_/%s.h>\n", name->c_str());
    if (!structs->empty())
        fprintf(objc, "\n");

    _foreach (name, *classes)
        fprintf(objc, "@class %s;\n", name->c_str());
    if (!classes->empty())
        fprintf(objc, "\n");
}

void objc_scan_prototype(
    apr_pool_t *pool,
    struct sig_type *type,
    std::set<std::string> *structs,
    std::set<std::string> *classes,
    bool skip = false
) {
    switch (type->primitive) {
        case union_P:
        case struct_P:
            if (!skip)
                if (type->name != NULL)
                    structs->insert(type->name);
                else {
                    char *name = objc_name_anonymous(pool, type->primitive, &type->data.signature);
                    structs->insert(name);
                    free(name);
                }
            for (size_t offset = 0; offset != type->data.signature.count; ++offset)
                objc_scan_prototype(pool, type->data.signature.elements[offset].type, structs, classes, skip);
        break;

        case object_P:
            if (type->name != NULL)
               classes->insert(type->name);
        break;

        case pointer_P:
            if (type->data.data.type != NULL)
                objc_scan_prototype(pool, type->data.data.type, structs, classes, true);
        break;

        case array_P:
            objc_scan_prototype(pool, type->data.data.type, structs, classes, false);
        break;

        default:
        break;
    }
}

void objc_aggregate(apr_pool_t *pool, enum joc_primitive primitive, const char *name, struct sig_signature *signature) {
    char *path;
    asprintf(&path, "source/hdr/struct_/%s.h", name);
    FILE *objh = fopen(path, "w");
    if (objh == NULL) {
        printf("fopen(\"%s\", \"w\")\n", path);
        _assert(false);
    }
    free(path);

    fprintf(objh, "#ifndef OBJC_%s\n", name);
    fprintf(objh, "#define OBJC_%s\n", name);
    fprintf(objh, "\n");

    std::set<std::string> structs;
    std::set<std::string> classes;

    for (size_t offset = 0; offset != signature->count; ++offset) {
        struct sig_element *element = &signature->elements[offset];
        objc_scan_prototype(pool, element->type, &structs, &classes);
    }

    objc_print_prototype(objh, &structs, &classes);

    fprintf(objh, "%s %s {\n", primitive == struct_P ? "struct" : "union", name);

    for (size_t offset = 0; offset != signature->count; ++offset) {
        struct sig_element *element = &signature->elements[offset];
        char *name = field_name(element, offset);
        name = objc_type(pool, element->type, name, 1);
        fprintf(objh, "    %s;\n", name);
        free(name);
    }

    fprintf(objh, "};\n");
    fprintf(objh, "\n");
    fprintf(objh, "#endif/*OBJC_%s*/\n", name);

    fclose(objh);
}

void java_aggregate(apr_pool_t *pool, enum joc_primitive primitive, const char *name, struct sig_signature *signature) {
    char *struct_escape = java_escape(name);

    char *path;
    asprintf(&path, "source/obc/%s.java", struct_escape);
    FILE *java = fopen(path, "w");
    _assert(java != NULL);
    free(path);

    fprintf(java, "package obc;\n");
    fprintf(java, "\n");

    fprintf(java, "public final class %s\n", struct_escape);
    fprintf(java, "    extends joc.Struct<%s>\n", struct_escape);
    fprintf(java, "{\n");

    fprintf(java, "    public %s() {\n", struct_escape);

    for (size_t offset = 0; offset != signature->count; ++offset) {
        struct sig_element *element = &signature->elements[offset];
        if (element->type->primitive != struct_P)
            continue;
        char *name = field_name(element, offset);
        char *struct_escape;
        if (element->type->name == NULL)
            struct_escape = objc_name_anonymous(pool, element->type->primitive, &element->type->data.signature);
        else {
            char *angle = strchr(element->type->name, '<');
            if (angle == NULL) {
                struct_escape = java_escape(element->type->name);
            } else {
                angle = strndup_(element->type->name, angle - element->type->name);
                struct_escape = java_escape(angle);
                free(angle);
            }
        }
        fprintf(java, "        this.%s = new %s();\n", name, struct_escape);
        free(struct_escape);
        free(name);
    }

    fprintf(java, "    }\n");
    fprintf(java, "\n");

    fprintf(java, "    @SuppressWarnings(\"unchecked\")\n");
    fprintf(java, "    public void setFields(Object[] values) {\n");

    for (size_t offset = 0; offset != signature->count; ++offset) {
        struct sig_element *element = &signature->elements[offset];
        char *name = field_name(element, offset);
        fprintf(java, "        this.%s = ", name);
        cast_type(pool, java, element->type, true);
        fprintf(java, "values[%zu];\n", offset);
        free(name);
    }

    fprintf(java, "    }\n");
    fprintf(java, "\n");

    fprintf(java, "    @SuppressWarnings(\"unchecked\")\n");
    fprintf(java, "    public %s clone()\n", struct_escape);
    fprintf(java, "        throws java.lang.CloneNotSupportedException\n");
    fprintf(java, "    {\n");
    fprintf(java, "        %s clone = (%s) super.clone();\n", struct_escape, struct_escape);

    for (size_t offset = 0; offset != signature->count; ++offset) {
        struct sig_element *element = &signature->elements[offset];
        char *name = field_name(element, offset);
        fprintf(java, "        clone.%s = ", name);
        fprintf(java, "this.%s", name);
        if (aggregate_type(element->type))
            fprintf(java, ".clone()");
        fprintf(java, ";\n");
        free(name);
    }

    fprintf(java, "        return clone;\n");
    fprintf(java, "    }\n");
    fprintf(java, "\n");

    if (signature->count == 0) {
        fprintf(java, "    public Class[] getTypes() {\n");
        fprintf(java, "        return null;\n");
        fprintf(java, "    }\n");
        fprintf(java, "\n");

        fprintf(java, "    public Object[] getFields() {\n");
        fprintf(java, "        return null;\n");
        fprintf(java, "    }\n");
    } else {
        fprintf(java, "    public Class[] getTypes() {\n");
        fprintf(java, "        return new Class[] {\n");

        for (size_t offset = 0; offset != signature->count; ++offset) {
            struct sig_element *element = &signature->elements[offset];
            char *name = field_name(element, offset);
            fprintf(java, "        ");
            java_type(pool, java, element->type, true, false);
            fprintf(java, ".class");
            free(name);
            if (offset != signature->count - 1)
                fprintf(java, ",");
            fprintf(java, "\n");
        }

        fprintf(java, "        };\n");
        fprintf(java, "    }\n");
        fprintf(java, "\n");

        fprintf(java, "    public Object[] getFields() {\n");
        fprintf(java, "        return new Object[] {\n");

        for (size_t offset = 0; offset != signature->count; ++offset) {
            struct sig_element *element = &signature->elements[offset];
            fprintf(java, "            ");
            char *name = field_name(element, offset);
            fprintf(java, "%s", name);
            free(name);
            if (offset != signature->count - 1)
                fprintf(java, ",");
            fprintf(java, "\n");
        }

        fprintf(java, "        };\n");
        fprintf(java, "    }\n");

        fprintf(java, "\n");
        fprintf(java, "    public %s(", struct_escape);

        bool comma = false;
        struct_signature(pool, java, "", signature, &struct_element, &comma);

        fprintf(java, "\n    ) {\n");
        fprintf(java, "        this();\n");
        fprintf(java, "\n");

        struct_signature(pool, java, "", signature, &struct_copy, NULL);

        fprintf(java, "    }\n");
        fprintf(java, "\n");

        for (size_t offset = 0; offset != signature->count; ++offset) {
            struct sig_element *element = &signature->elements[offset];
            fprintf(java, "    public ");
            java_type(pool, java, element->type, false, true);
            fprintf(java, " ");
            char *name = field_name(element, offset);
            fprintf(java, "%s;\n", name);
            free(name);
        }
    }

    fprintf(java, "}\n");
    fclose(java);

    free(struct_escape);
}

void aggregate(apr_pool_t *pool, enum joc_primitive primitive, const char *name, struct sig_signature *signature, const char *types) {
    sqlcall(sqlite3_bind_string(struct_select_stmt_, 1, name));

    int struct_step;
    sqlcall(struct_step = sqlite3_step(struct_select_stmt_));
    sqlcall(sqlite3_reset(struct_select_stmt_));
    if (struct_step != SQLITE_DONE)
        return;

    sqlcall(sqlite3_bind_string(struct_insert_stmt_, 2, name));
    sqlcall(sqlite3_bind_string(struct_insert_stmt_, 3, types));

    sqlcall(sqlite3_step(struct_insert_stmt_));
    sqlcall(sqlite3_reset(struct_insert_stmt_));

    if (do_java_)
        java_aggregate(pool, primitive, name, signature);
    if (do_objc_)
        objc_aggregate(pool, primitive, name, signature);
}
/* }}} */

void java_string(FILE *file, const char *value) {
    for (;; ++value)
        switch (*value) {
            case '\0':
                return;
            case '"':
            case '\\':
                fprintf(file, "\\%c", *value);
            break;
            default:
                fputc(*value, file);
        }
}

void objc_class(apr_pool_t *pool) {
    char *super_framework = sqlite3_column_string(class_stmt_, 1);
    char *super_class = sqlite3_column_string(class_stmt_, 2);
    char *class_name = sqlite3_column_string(class_stmt_, 3);
    char *framework_name = sqlite3_column_string(class_stmt_, 5);

    char *path;
    asprintf(&path, "source/hdr/%s/%s.h", framework_name, class_name);
    FILE *objh = fopen(path, "w");
    if (objh == NULL) {
        printf("fopen(\"%s\", \"w\")\n", path);
        _assert(false);
    }
    free(path);

    fprintf(objh, "#ifndef OBC_%s_%s\n", framework_name, class_name);
    fprintf(objh, "#define OBC_%s_%s\n", framework_name, class_name);
    fprintf(objh, "\n");

    fprintf(objh, "#include <stdbool.h>\n");
    fprintf(objh, "#include <objc/objc.h>\n");
    fprintf(objh, "\n");

    if (super_class != NULL) {
        fprintf(objh, "#include <%s/%s.h>\n", super_framework, super_class);
        fprintf(objh, "\n");
    }

    std::set<std::string> structs;
    std::set<std::string> classes;

    _forever {
        int variable_step;
        sqlcall(variable_step = sqlite3_step(variable_stmt_));
        if (variable_step == SQLITE_DONE)
            break;

        char *variable_type = sqlite3_column_string(variable_stmt_, 1);

        const char *type_parse = variable_type;
        struct sig_type *type = sig_parse_type(pool, &type_parse, '\0', false);

        objc_scan_prototype(pool, type, &structs, &classes);
    }

    sqlcall(sqlite3_reset(variable_stmt_));

    objc_print_prototype(objh, &structs, &classes);

    fprintf(objh, "@interface %s", class_name);
    if (super_class != NULL)
        fprintf(objh, " : %s", super_class);
    fprintf(objh, " {\n");

    _forever {
        int variable_step;
        sqlcall(variable_step = sqlite3_step(variable_stmt_));
        if (variable_step == SQLITE_DONE)
            break;

        char *variable_name = sqlite3_column_string(variable_stmt_, 0);
        char *variable_type = sqlite3_column_string(variable_stmt_, 1);

        const char *type_parse = variable_type;
        struct sig_type *type = sig_parse_type(pool, &type_parse, '\0', false);

        variable_name = objc_type(pool, type, variable_name, 1);
        fprintf(objh, "    %s;\n", variable_name);

        free(variable_type);
        free(variable_name);
    }

    sqlcall(sqlite3_reset(variable_stmt_));

    fprintf(objh, "}\n");
    fprintf(objh, "\n");

    _forever {
        int method_step;
        sqlcall(method_step = sqlite3_step(method_stmt_));
        if (method_step == SQLITE_DONE)
            break;

        bool method_static = sqlite3_column_boolean(method_stmt_, 0);
        char *method_name = sqlite3_column_string(method_stmt_, 1);
        char *method_types = sqlite3_column_string(method_stmt_, 2);

        struct sig_signature signature;
        sig_parse(pool, &signature, method_types);

        struct sig_type *return_type = signature.elements[0].type;

        char *message_name = strdup(method_name);
        char *message_begin = message_name;

        if (method_static)
            fprintf(objh, "+ (");
        else
            fprintf(objh, "- (");

        char *name = objc_type(pool, return_type, NULL);
        fprintf(objh, "%s)", name);
        free(name);

        for (size_t index = 0;; ++index) {
            char *colon = strchr(message_begin, ':');
            struct sig_type *type = signature.elements[index + 3].type;

            if (colon == NULL) {
                fprintf(objh, " %s", message_begin);
                break;
            } else {
                *colon = '\0';

                char *name = objc_type(pool, type, NULL);
                fprintf(objh, " %s:(%s)arg%zu", message_begin, name, index);
                free(name);

                message_begin = colon + 1;
                if (*message_begin == '\0')
                    break;
            }
        }

        fprintf(objh, ";\n");

        free(message_name);

        free(method_types);
        free(method_name);
    }

    sqlcall(sqlite3_reset(method_stmt_));

    fprintf(objh, "\n");
    fprintf(objh, "@end\n");

    fprintf(objh, "\n");
    fprintf(objh, "#endif/*OBC_%s_%s*/\n", framework_name, class_name);
    fclose(objh);
}

void java_class(apr_pool_t *pool) {
    char *super_class = sqlite3_column_string(class_stmt_, 2);
    char *class_name = sqlite3_column_string(class_stmt_, 3);
    char *root_class = sqlite3_column_string(class_stmt_, 4);
    char *framework_name = sqlite3_column_string(class_stmt_, 5);

    char *class_escape = java_escape(class_name);
    char *super_escape = java_escape(super_class);
    char *root_escape = java_escape(root_class);

    char *path;
    asprintf(&path, "source/obc/%s.java", class_escape);
    FILE *java = fopen(path, "w");
    _assert(java != NULL);
    free(path);

    asprintf(&path, "source/joc/mix/%s.java", class_escape);
    FILE *mix = fopen(path, "r");
    bool mixed = mix != NULL;
    if (mixed)
        fclose(mix);

    fprintf(java, "package obc;\n");
    fprintf(java, "\n");
    fprintf(java, "public class %s\n", class_escape);
    if (mixed)
        fprintf(java, "    extends joc.mix.%s\n", class_escape);
    else if (super_class != NULL)
        fprintf(java, "    extends %s\n", super_escape);
    else
        fprintf(java, "    extends joc.Strap\n");
    fprintf(java, "{\n");
    fprintf(java, "    private static Object framework_ = new osx.%s();\n", framework_name);

    _forever {
        int method_step;
        sqlcall(method_step = sqlite3_step(method_stmt_));
        if (method_step == SQLITE_DONE)
            break;

        bool method_static = sqlite3_column_boolean(method_stmt_, 0);
        char *method_name = sqlite3_column_string(method_stmt_, 1);
        char *method_types = sqlite3_column_string(method_stmt_, 2);

        size_t name_length = strlen(method_name);
        char *camel_name = (char *) malloc(name_length + 1);
        for (size_t name_index = 0; name_index != name_length; ++name_index) {
            char name_char = method_name[name_index];
            camel_name[name_index] = name_char == ':' ? '$' : name_char;
        }
        camel_name[name_length] = '\0';

        struct sig_signature signature;
        sig_parse(pool, &signature, method_types);

        char *camel_escape = java_escape(camel_name);

        fprintf(java, "\n");

        fprintf(java, "    /**\n");
        fprintf(java, "     * ");

        struct sig_type *return_type = signature.elements[0].type;

        char *message_name = strdup(method_name);
        char *message_begin = message_name;

        if (method_static)
            fprintf(java, "+ (");
        else
            fprintf(java, "- (");

        char *name = objc_type(pool, return_type, NULL);
        fprintf(java, "%s)", name);
        free(name);

        for (size_t index = 0;; ++index) {
            char *colon = strchr(message_begin, ':');
            struct sig_type *type = signature.elements[index + 3].type;

            if (colon == NULL) {
                fprintf(java, " %s", message_begin);
                break;
            } else {
                *colon = '\0';

                char *name = objc_type(pool, type, NULL);
                fprintf(java, " %s:(%s)arg%zu", message_begin, name, index);
                free(name);

                message_begin = colon + 1;
                if (*message_begin == '\0')
                    break;
            }
        }

        free(message_name);

        fprintf(java, " \n");
        fprintf(java, "     */\n");

        fprintf(java, "    @SuppressWarnings(\"unchecked\")\n");
        fprintf(java, "    @joc.Message(name=\"%s\", types=\"", method_name);
        java_string(java, method_types);
        fprintf(java, "\")\n");
        fprintf(java, "    public ");
        if (method_static)
            fprintf(java, "static ");
        java_type(pool, java, return_type, false, true);
        fprintf(java, " ");
        /*if (method_static)
            fprintf(java, "$");*/
        if (method_static)
            fprintf(java, "$");
        fprintf(java, "%s(", camel_escape);

        for (size_t index = 3; index != signature.count; ++index) {
            if (index != 3)
                fprintf(java, ", ");
            java_type(pool, java, signature.elements[index].type, false, true);
            fprintf(java, " arg%zu", index - 3);
        }

        fprintf(java, ") {\n");

        fprintf(java, "        ");
        if (return_type->primitive != void_P) {
            fprintf(java, "return ");
            cast_type(pool, java, return_type, true);
        }
        fprintf(java, "joc.Runtime.msgSend(");
        if (method_static)
            fprintf(java, "%s.class, %s.class", class_escape, class_escape);
        else
            fprintf(java, "this, incorrect_ ? null : %s.class", class_escape);
        fprintf(java, ", \"%s\"", method_name);

        for (size_t index = 3; index != signature.count; ++index)
            fprintf(java, ", arg%zu", index - 3);

        fprintf(java, ");\n");
        fprintf(java, "    }\n");

        free(camel_escape);
        free(camel_name);

        free(method_types);
        free(method_name);
    }

    sqlcall(sqlite3_reset(method_stmt_));

    /*fprintf(meta, "}\n");
    fclose(meta);*/

    fprintf(java, "}\n");
    fclose(java);

    free(root_escape);
    free(super_escape);
    free(class_escape);

    free(super_class);
    free(class_name);
}

int main(int argc, char *argv[]) {
    _assert(argc == 1);

    sig_aggregate = &aggregate;

    apr_initialize();

    apr_pool_t *pool;
    apr_pool_create(&pool, NULL);

    sqlcall(sqlite3_open("joc.db", &database_));

    sqlcall(sqlite3_prepare(database_,
        "select "
            "\"framework\".\"id\", "
            "\"framework\".\"name\", "
            "\"framework\".\"root\", "
            "\"framework\".\"path\" "
        "from \"framework\""
    , -1, &framework_stmt_, NULL));

    sqlcall(sqlite3_prepare(database_,
        "select "
            "\"class\".\"name\" "
        "from \"class\" "
        "where \"class\".\"framework\" = ?"
    , -1, &prototype_stmt_, NULL));

    sqlcall(sqlite3_prepare(database_,
        "select "
            "\"variable\".\"name\", "
            "\"variable\".\"type\" "
        "from \"variable\""
    , -1, &type_stmt_, NULL));

    sqlcall(sqlite3_prepare(database_,
        "select "
            "\"class\".\"id\", "
            "\"super_framework\".\"name\", "
            "\"super_class\".\"name\", "
            "\"class\".\"name\", "
            "\"class\".\"root\", "
            "\"framework\".\"name\" "
        "from \"class\" "
        "join \"framework\" on "
            "\"class\".\"framework\" = \"framework\".\"id\" "
        "left join \"class\" as \"super_class\" on "
            "\"class\".\"super\" = \"super_class\".\"name\" "
        "left join \"framework\" as \"super_framework\" on "
            "\"super_class\".\"framework\" = \"super_framework\".\"id\""
    , -1, &class_stmt_, NULL));

    sqlcall(sqlite3_prepare(database_,
        "select "
            "\"variable\".\"name\", "
            "\"variable\".\"type\" "
        "from \"variable\" "
        "where not \"variable\".\"static\" and "
            "\"variable\".\"class\" = ? "
        "order by \"variable\".\"offset\""
    , -1, &variable_stmt_, NULL));

    sqlcall(sqlite3_prepare(database_,
        "select "
            "\"method\".\"static\", "
            "\"method\".\"name\", "
            "\"method\".\"types\" "
        "from \"method\" "
        "join \"category\" on "
            "\"method\".\"category\" = \"category\".\"id\" "
        "where \"category\".\"class\" = ? "
        "order by "
            "\"method\".\"static\" desc, "
            "\"method\".\"name\" asc"
    , -1, &method_stmt_, NULL));

    sqlcall(sqlite3_prepare(database_,
        "delete from \"struct\""
    , -1, &struct_delete_stmt_, NULL));

    sqlcall(sqlite3_prepare(database_,
        "select \"types\" "
        "from \"struct\" "
        "where \"struct\".\"name\" = ?"
    , -1, &struct_select_stmt_, NULL));

    sqlcall(sqlite3_prepare(database_,
        "insert into \"struct\" ("
            "\"id\", "
            "\"name\", "
            "\"types\""
        ") values (?, ?, ?)"
    , -1, &struct_insert_stmt_, NULL));

    sqlcall(sqlite3_step(struct_delete_stmt_));
    sqlcall(sqlite3_reset(struct_delete_stmt_));

    _forever {
        int type_step;
        sqlcall(type_step = sqlite3_step(type_stmt_));
        if (type_step == SQLITE_DONE)
            break;

        //char *type_name = sqlite3_column_string(type_stmt_, 0);
        char *type_type = sqlite3_column_string(type_stmt_, 1);

        const char *type_parse = type_type;
        sig_parse_type(pool, &type_parse, '\0', false);
    }

    sqlcall(sqlite3_reset(type_stmt_));

    _forever {
        int framework_step;
        sqlcall(framework_step = sqlite3_step(framework_stmt_));
        if (framework_step == SQLITE_DONE)
            break;

        int framework_id = sqlite3_column_int(framework_stmt_, 0);
        sqlcall(sqlite3_bind_int(prototype_stmt_, 1, framework_id));

        char *framework_name = sqlite3_column_string(framework_stmt_, 1);
        char *framework_path = sqlite3_column_string(framework_stmt_, 3);

        if (do_java_) {
            char *path;

            asprintf(&path, "source/osx/%s.java", framework_name);
            FILE *java = fopen(path, "w");
            free(path);

            _assert(java != NULL);

            fprintf(java, "package osx;\n");
            fprintf(java, "\n");
            fprintf(java, "public class %s {\n", framework_name);
            fprintf(java, "    static long handle_ = joc.Runtime.dlopen(\"%s\");\n", framework_path);
            fprintf(java, "\n");
            fprintf(java, "    public %s() {\n", framework_name);
            //fprintf(java, "        System.out.println(handle_);\n");
            fprintf(java, "    }\n");

            /*asprintf(&path, "source/ad9/%s.sig", framework_name);
            FILE *sig = fopen(path, "r");
            free(path);

            if (sig != NULL) {
                // XXX: buffer overflow
                char type;
                char symbol[1024];
                char value[1024];

                _forever {
                    int scan = fscanf(sig, "%c %s %s\n", &type, symbol, value);
                    if (scan == EOF)
                        break;
                    _assert(scan == 3);

                    fprintf(java, "\n");

                    switch (type) {
                        case 'C':
                            fprintf(java, "    static final int %s = %s;\n", "", "");
                        break;

                        case 'F':
                            fprintf(java, "    }\n");
                        break;

                        case 'V':
                            fprintf(java, "    static %s %s() {\n", "", "");
                            fprintf(java, "        return (%s) joc.Runtime.load(%s, %s.class);\n", "", "", "");
                            fprintf(java, "    }\n");
                            fprintf(java, "\n");
                            fprintf(java, "    static %s %s(%s value) {\n", "", "", "");
                            fprintf(java, "        joc.Runtime.store(%s, value);\n", "");
                            fprintf(java, "    }\n");
                        break;

                        default:
                            _assert(false);
                    }
                }
            }*/

            fprintf(java, "}\n");

            fclose(java);
        }

        if (do_objc_) {
            char *path;
            asprintf(&path, "source/hdr/%s", framework_name);
            mkdir(path, 0644);
            free(path);

            asprintf(&path, "source/hdr/%s/%s.h", framework_name, framework_name);
            FILE *objh = fopen(path, "w");
            _assert(objh != NULL);
            free(path);

            fprintf(objh, "#ifndef OBJC_%s_%s\n", framework_name, framework_name);
            fprintf(objh, "#define OBJC_%s_%s\n", framework_name, framework_name);
            fprintf(objh, "\n");

            _forever {
                int prototype_step;
                sqlcall(prototype_step = sqlite3_step(prototype_stmt_));
                if (prototype_step == SQLITE_DONE)
                    break;
                _assert(prototype_step == SQLITE_ROW);

                char *class_name = sqlite3_column_string(prototype_stmt_, 0);
                fprintf(objh, "#include <%s/%s.h>\n", framework_name, class_name);
            }

            sqlcall(sqlite3_reset(prototype_stmt_));

            fprintf(objh, "\n");
            fprintf(objh, "#endif/*OBJC_%s_%s*/\n", framework_name, framework_name);

            fclose(objh);
        }
    }

    sqlcall(sqlite3_reset(framework_stmt_));

    _forever {
        int class_step;
        sqlcall(class_step = sqlite3_step(class_stmt_));
        if (class_step == SQLITE_DONE)
            break;
        _assert(class_step == SQLITE_ROW);

        int class_id = sqlite3_column_int(class_stmt_, 0);
        sqlcall(sqlite3_bind_int(variable_stmt_, 1, class_id));

        char *class_name = sqlite3_column_string(class_stmt_, 3);
        sqlcall(sqlite3_bind_string(method_stmt_, 1, class_name));

        if (do_java_)
            java_class(pool);
        if (do_objc_)
            objc_class(pool);
    }

    sqlcall(sqlite3_reset(class_stmt_));

    sqlcall(sqlite3_finalize(struct_insert_stmt_));
    sqlcall(sqlite3_finalize(struct_select_stmt_));
    sqlcall(sqlite3_finalize(struct_delete_stmt_));

    sqlcall(sqlite3_finalize(method_stmt_));
    sqlcall(sqlite3_finalize(variable_stmt_));
    sqlcall(sqlite3_finalize(class_stmt_));
    sqlcall(sqlite3_finalize(type_stmt_));
    sqlcall(sqlite3_finalize(prototype_stmt_));
    sqlcall(sqlite3_finalize(framework_stmt_));

    sqlcall(sqlite3_close(database_));

    return 0;
}
