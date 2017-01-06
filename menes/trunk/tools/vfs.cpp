/* Ninetjer - _Truly_ Free (non-GPL) Unix .NET Runtime
 * Copyright (C) 2005  Jay Freeman (saurik)
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

#include "cxx/standard.hh"
#pragma hdrstop("build/cxx/standard.pch")

#include "api/uuid.hpp"
#include "app/simple.hpp"

#include "dbi/connection.hpp"
#include "dbi/driver.hpp"
#include "dbi/resultset.hpp"

#include "ext/uuid.hpp"
#include "vfs/system.hpp"

#include "xml/document.hpp"
#include "xml/parse.hpp"
#include "xml/resolver.hpp"
#include "xml/textwriter.hpp"

namespace be {

ext::Uuid CreateEntity_(const _R<dbi::Connection> &connection) {
    ext::Uuid id(api::Uuid::CreateSequential());
    connection->Parse("INSERT INTO entity (id) VALUES (?)")->Execute(id);
    return id;
}

ext::Uuid GetName_(const _R<dbi::Connection> &connection, const xml::Name &name) {
    {
        _R<dbi::ResultSet> names(connection->Parse("SELECT id FROM name WHERE namespace = ? AND localname = ?")->Execute(name.GetSpace(), name.GetLocal()));
        if (names->MoveNext())
            return names->Get<ext::Uuid>("id");
    }

    ext::Uuid id(CreateEntity_(connection));
    connection->Parse("INSERT INTO name (id, namespace, localname) VALUES (?, ?, ?)")->Execute(id, name.GetSpace(), name.GetLocal());
    return id;
}

}

class Importer :
    public xml::Writer
{
  private:
    _R<dbi::Connection> connection_;
    unsigned position_;

    _L<ext::Uuid> nodes_;

    ext::Uuid CreateNode_(const ext::Uuid *parent) {
        ext::Uuid id(be::CreateEntity_(connection_));
        ext::Uuid document(nodes_.IsEmpty() ? id : nodes_.First());
        connection_->Parse("INSERT INTO node (id, document, parent, position, next) VALUES (?, ?, ?, ?, ?)")->Execute(id, document, parent, position_, position_ + 1);
        ++position_;
        return id;
    }

    ext::Uuid GetName_(const xml::Name &name) {
        return be::GetName_(connection_, name);
    }

    void CreateElement_(const xml::Name &name, bool attribute) {
        ext::Uuid id(CreateNode_(&nodes_.Last()));
        connection_->Parse("INSERT INTO element (id, name, attribute) VALUES (?, ?, ?)")->Execute(id, GetName_(name), attribute);
        nodes_.InsertLast(id);
    }

    void Close_() {
        connection_->Parse("UPDATE node SET next = ? WHERE id = ?")->Execute(position_, nodes_.Last());
        nodes_.RemoveLast();
    }

    void Text_(const cse::String &value, unsigned type) {
        ext::Uuid id(CreateNode_(&nodes_.Last()));
        connection_->Parse("INSERT INTO text (id, value, type) VALUES (?, ?, ?)")->Execute(id, value, type);
    }

  public:
    Importer(const _R<dbi::Connection> &connection) :
        connection_(connection),
        position_(0)
    {
        nodes_.InsertLast(CreateNode_(NULL));
    }

    ~Importer() {
        Close_();
        _assert(nodes_.IsEmpty());
    }

    ext::Uuid GetId() const {
        return nodes_.First();
    }

    virtual void OpenElement(const xml::Name &name) {
        CreateElement_(name, false);
    }

    virtual void CloseElement() {
        Close_();
    }

    virtual void OpenAttribute(const xml::Name &name) {
        CreateElement_(name, true);
    }

    virtual void CloseAttribute() {
        Close_();
    }

    virtual void OutputText(const cse::String &text) {
        Text_(text, 0);
    }

    virtual void OutputComment(const cse::String &comment) {
        Text_(comment, 1);
    }
};

// XXX: this can be made non-recursive with some trickery based on node.next
void Export(const _R<dbi::Connection> &connection, const ext::Uuid &id, xml::Writer &xml) {
    ext::Uuid document;
    unsigned first, last;

    {
        _R<dbi::ResultSet> parent(connection->Parse("SELECT document, position, next FROM node WHERE id = ?")->Execute(id));

        _assert(parent->MoveNext());

        document = parent->Get<ext::Uuid>("document");
        first = parent->Get<unsigned>("position");
        last = parent->Get<unsigned>("next");
    }

    _R<dbi::ResultSet> children(connection->Parse(_B(
        "SELECT node.id, node.position, node.next, text.value, text.type, element.attribute, name.namespace, name.localname "
            "FROM node "
            "LEFT OUTER JOIN text "
                "ON text.id = node.id "
            "LEFT OUTER JOIN element "
                "ON element.id = node.id "
            "LEFT OUTER JOIN name "
                "ON name.id = element.name "
            "WHERE node.document = ? AND node.position >= ? AND node.position < ?"
            "ORDER BY node.position"
    ))->Execute(document, first, last));

    _L< etl::Pair<unsigned, bool> > nodes;

    while (children->MoveNext()) {
        unsigned position(children->Get<unsigned>(_B("position")));

        while (!nodes.IsEmpty() && position == nodes.Last().First()) {
            if (nodes.Last().Second())
                xml.CloseAttribute();
            else
                xml.CloseElement();
            nodes.RemoveLast();
        }

        if (!children->IsNull(_B("type"))) {
            cse::String value(children->GetString(_B("value")));
            switch (children->Get<unsigned>(_B("type"))) {
                case 0: xml.OutputText(value); break;
                case 1: xml.OutputComment(value); break;
                //case 2: xml.OutputPi(value); break;
            }
        } else if (!children->IsNull(_B("attribute"))) {
            bool attribute(children->Get<bool>(_B("attribute")));
            nodes.InsertLast(etl::Pair<unsigned, bool>(children->Get<unsigned>(_B("next")), attribute));

            cse::String _namespace(children->GetString(_B("namespace")));
            cse::String localname(children->GetString(_B("localname")));

            xml::Name name(localname, _namespace);

            if (attribute)
                xml.OpenAttribute(name);
            else
                xml.OpenElement(name);
        }
    }

    while (!nodes.IsEmpty()) {
        if (nodes.Last().Second())
            xml.CloseAttribute();
        else
            xml.CloseElement();
        nodes.RemoveLast();
    }
}

int Main(const app::Options &options) {
    _R<dbi::Connection> connection(dbi::GetDriver("dbi::PgSql::Driver")->Connect("", "saurik", "", "thoth"));
    dbi::Transaction lock(*connection);

    if (options.GetArguments().IsEmpty()) {
        api::Cerr << "Type 'vfs help' for usage." << ios::NewLine;
        return -1;
    }

    cse::String command(*options[0]);

    if (command == "help") {
        //if (options.GetArguments().GetSize() == 1) {
            api::Cout << "usage: vfs <subcommand> [options] [args]" << ios::NewLine;
            api::Cout << "Type \"vfs help <subcommand>\" for help on a specific subcommand." << ios::NewLine;
            api::Cout << ios::NewLine;
            api::Cout << "Available subcommands:" << ios::NewLine;
            api::Cout << "   cache <data> <mimetype>" << ios::NewLine;
            api::Cout << "   export <id> >{xml}" << ios::NewLine;
            api::Cout << "   get <parent> <namespace> <localname>" << ios::NewLine;
            api::Cout << "   import <{xml}" << ios::NewLine;
            api::Cout << "   read <id> >{binary}" << ios::NewLine;
            api::Cout << "   set <parent> <namespace> <localname> <value>" << ios::NewLine;
            api::Cout << "   write <{binary}" << ios::NewLine;
            return 0;
        //}
    } else if (command == "cache") {
        // XXX: lexical_cast<>()
        ext::Uuid id(be::CreateEntity_(connection));
        ext::Uuid data(lexical_cast<ext::Uuid>(cse::String(*options[1])));
        cse::String mimetype(*options[2]);

        connection->Parse("INSERT INTO document (id, data, mimetype) VALUES (?, ?, ?)")->Execute(id, data, mimetype);

        api::Cout << id << ios::NewLine;

        return 0;
    } else if (command == "export") {
        // XXX: lexical_cast<>()
        ext::Uuid id(lexical_cast<ext::Uuid>(cse::String(*options[1])));
        _S<xml::TextWriter> text(api::Cout);
        Export(connection, id, text);
        return 0;
    } else if (command == "get") {
        // XXX: lexical_cast<>()
        ext::Uuid parent(lexical_cast<ext::Uuid>(cse::String(*options[1])));
        cse::String _namespace(*options[2]);
        cse::String localname(*options[3]);

        _R<dbi::ResultSet> property(connection->Parse("SELECT id, value FROM property WHERE parent = ? AND namespace = ? AND localname = ?")->Execute(parent, _namespace, localname));

        if (property->MoveNext()) {
            api::Cout << property->GetString("value") << ios::NewLine;
            return 0;
        } else if (connection->Parse("SELECT COUNT(*) FROM entity WHERE id = ?")->Execute(parent)->Get<unsigned>() == 0)
            api::Cerr << "unknown entity" << ios::NewLine;
        else
            api::Cerr << "unknown property" << ios::NewLine;

        return -1;
    } else if (command == "import") {
        _S<Importer> importer(connection);
        _S<xml::Resolver> resolver(&importer);

        xml::Parse(api::Cin, resolver);
        api::Cout << importer.GetId() << ios::NewLine;

        return 0;
    } else if (command == "read") {
        // XXX: lexical_cast<>()
        ext::Uuid id(lexical_cast<ext::Uuid>(cse::String(*options[1])));

        if (connection->Parse("SELECT COUNT(*) FROM data WHERE id = ?")->Execute(id)->Get<unsigned>() == 0) {
            _R<dbi::ResultSet> document(connection->Parse("SELECT data FROM document WHERE id = ?")->Execute(id));

            if (document->MoveNext())
                id = document->Get<ext::Uuid>("data");
            else {
                api::Cerr << "unknown identifier" << ios::NewLine;
                return -1;
            }
        }

        vfs::System system(connection);
        system.ReadFileInto(id, api::Cout);
        return 0;
    } else if (command == "set") {
        ext::Uuid id;
        // XXX: lexical_cast<>()
        ext::Uuid parent(lexical_cast<ext::Uuid>(cse::String(*options[1])));
        cse::String _namespace(*options[2]);
        cse::String localname(*options[3]);
        cse::String value(*options[4]);

        _R<dbi::ResultSet> property(connection->Parse("SELECT id FROM property WHERE parent = ? AND namespace = ? AND localname = ?")->Execute(parent, _namespace, localname));

        if (property->MoveNext()) {
            id = property->Get<ext::Uuid>("id");
            connection->Parse("UPDATE entity SET modified = NOW() WHERE id = ?")->Execute(id);
            connection->Parse("UPDATE property SET value = ? WHERE id = ?")->Execute(value, id);
        } else {
            id = be::CreateEntity_(connection);
            connection->Parse("INSERT INTO property (id, parent, namespace, localname, value) VALUES (?, ?, ?, ?, ?)")->Execute(id, parent, _namespace, localname, value);
        }

        api::Cout << id << ios::NewLine;

        return 0;
    } else if (command == "write") {
        vfs::System system(connection);

        ext::Uuid id(system.WriteFileFrom(api::Cin));
        api::Cout << id << ios::NewLine;

        return 0;
    }

    // ERR: warning: control reaches end of non-void function

    api::Cerr << "unknown command: '" << command << "'" << ios::NewLine;
    api::Cerr << "Type 'vfs help' for usage." << ios::NewLine;
    return -1;
}
