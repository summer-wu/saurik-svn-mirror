# Copyright Notice {{{
# WikiCyte - Awesome WWW Content Management System
# Copyright (C) 2009  Jay Freeman (saurik)

#        Redistribution and use in source and binary
# forms, with or without modification, are permitted
# provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the
#    above copyright notice, this list of conditions
#    and the following disclaimer.
# 2. Redistributions in binary form must reproduce the
#    above copyright notice, this list of conditions
#    and the following disclaimer in the documentation
#    and/or other materials provided with the
#    distribution.
# 3. The name of the author may not be used to endorse
#    or promote products derived from this software
#    without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS''
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
# BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
# TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# }}}

# Import Statements {{{

import cgi, os, random, re, string, sys, urllib2, uuid

from mod_python import apache, psp, Session, util

from datetime import datetime
from xml.sax import saxutils

try:
    import wikidiff2
except:
    wikidiff2 = None

cynfig = apache.import_module('~/cynfig.py')

library = [os.path.dirname(__file__) + '/library']

markdown = apache.import_module('markdown/__init__', path=library)
pytz = apache.import_module('pytz/__init__', path=library)
smartypants = apache.import_module('smartypants', path=library)

try :
    tidy = apache.import_module('tidy/__init__', path=library)
except OSError:
    tidy = None

try:
    markdown_extensions = cynfig.markdown_extensions
except AttributeError:
    markdown_extensions = []

try:
    anonymous_users = cynfig.anonymous_users
except AttributeError:
    anonymous_users = []

#markdown.logger.setLevel(markdown.CRITICAL)

# }}}
# Content Protocol Escapes {{{

def exml(val):
    return saxutils.escape(val) if val != None else ''

def eatt(val):
    return saxutils.quoteattr(val) if val != None else '""'

def euri(val):
    return exml(urllib2.quote(val)) if val != None else ''

def etim(val):
    if cynfig.local_timezone != None:
        val = pytz.utc.localize(val)
        val = val.astimezone(cynfig.local_timezone)
    return val

def ejss(val):
    # XXX: this is not correct
    return exml(val)

def ecol(val):
    hsh = hash(val)
    r = ((hsh >> 16) & 255) * .95
    g = ((hsh >>  8) & 255) * .95
    b = ((hsh >>  0) & 255) * .95
    return '%.2x%.2x%.2x' % (r, g, b)

base = {'exml': exml, 'euri': euri, 'etim': etim, 'ecol': ecol}

# }}}
# Database Connections {{{

import psycopg2, psycopg2.pool
from psycopg2.extras import DictCursor

psycopg2.extensions.register_type(psycopg2.extensions.UNICODE)

def getdb():
    sql = psycopg2.connect(**cynfig.psycopg2_dsn)
    sql.set_client_encoding('UNICODE')
    return sql

def putdb(sql):
    sql.close()

def push(stmt, args, path=None):
    sql = getdb()
    try:
        csr = sql.cursor()
        try:
            csr.execute(stmt, args)
            if path != None:
                csr.execute("select currval(pg_get_serial_sequence(%s, %s))", path)
                rows = csr.fetchall()
            sql.commit()
            if path != None:
                return rows[0][0]
        finally:
            csr.close()
    finally:
        putdb(sql)

def cursor(sql):
    return sql.cursor(cursor_factory=DictCursor)

def pull(stmt, args):
    sql = getdb()
    try:
        csr = cursor(sql)
        try:
            csr.execute(stmt, args)
            rows = csr.fetchall()
            sql.commit()
            return rows
        finally:
            csr.close()
    finally:
        putdb(sql)

def yank(stmt, args):
    rows = pull(stmt, args)
    return rows[0] if len(rows) != 0 else None

# }}}
# Proxy X-Real-IP Address {{{

# Getting someone's IP address isn't as easy as you'd
# assume. While we can ask Apache for the IP address,
# we may be behind a reverse proxy of some form (maybe
# a cache or a load balancer).
#
# To get the real IP address, we can use the X-Real-IP
# header, but we can't /trust/ that information as then
# a user could claim to be anyone the want by just
# supplying that header with their requests.
#
# We therefore have a section of the configuration file
# for "known" proxies: these are proxies that are part
# of the infrastructure setup by the administrator of
# the webserver running WikiCyte.

try:
    known_proxies = cynfig.known_proxies
except AttributeError:
    known_proxies = []

def realip(req):
    host = req.connection.remote_ip
    if host in known_proxies:
        if 'X-Unique-ID' in req.headers_in:
            host = req.headers_in['X-Unique-ID']
    return host

# }}}

def parm(form, name):
    val = form.getfirst(name)
    return str(val) if val != None else None

def menu(req, uri, name, icon):
    uri = '/_' + euri(name) + uri

    req.write("""<li><a href=%s title=%s onclick="
        window.location = '%s?target=' + window.location.pathname;
        return false;
    "><img src=%s height="12" width="12"/></a></li>""" % (
        eatt(uri), eatt(name), ejss(uri), eatt('http://cache.saurik.com/silk/' + euri(icon) + '.png')
    ))

def save(req, form, uri):
    rev = parm(form, 'from')
    then = parm(form, 'now')
    data = parm(form, 'data')

    if data != None:
        data = clean(data)
    then = datetime.fromtimestamp(float(then))

    req.get_basic_auth_pw()
    user = req.user
    host = req.connection.remote_ip

    text = re.sub(r'{[^}]*}', r'', data) if data != None else None

    push('''
        insert into "wikicyte"."page" (
            "uri", "user", "host", "data", "search", "then", "from"
        ) values (
            %(uri)s, %(user)s, %(host)s, %(data)s, to_tsvector(%(text)s), %(then)s,
                coalesce((select "id" from "wikicyte"."page" where "then" = %(then)s order by "when" desc limit 1), %(rev)s)
        )
    ''', locals())

def head(req):
    req.content_type = "text/html; charset=UTF-8"
    req.send_http_header()

def clean(data):
    return data.replace('\r\n', '\n').replace('\r', '\n')

def edit(req, uri):
    req.get_basic_auth_pw()
    user = req.user

    if anon(user):
        return apache.HTTP_FORBIDDEN

    here = os.path.dirname(__file__)
    form = util.FieldStorage(req)

    if req.method == 'POST':
        save(req, form, uri)
        target = parm(form, 'target')
        if target == None:
            target = uri
        util.redirect(req, target)
        return apache.OK

    page = luri(uri)
    uri = exml(uri)

    target = parm(form, 'target')
    if target != None:
        target = exml(target)

    if page == None:
        rev = None
        data = ''
    else:
        rev = int(page['id'])
        data = exml(page['data'])

    head(req)
    data = clean(data)

    if data.startswith('\n'):
        data = '\n' + data
    data = data.encode('utf8')

    psp.PSP(req, filename=(here + '/wikicyte-edit.psp')).run(vars=dict({
        'uri': uri,
        'rev': rev,
        'data': data,
        'target': target
    }, **base))

    return apache.OK

def history(req, uri):
    if wikidiff2 == None:
        return apache.HTTP_NOT_FOUND

    here = os.path.dirname(__file__)

    # XXX: naive datetime
    history = pull('''
        select "id", "uri", "from", "user", "host", "when"::timestamp, "approved"
        from "wikicyte"."approved"(now())
        where
            "uri" = %(uri)s or %(uri)s is null
        order by "when" desc nulls last
    ''', locals())

    head(req)

    psp.PSP(req, filename=(here + '/wikicyte-history.psp')).run(vars=dict({
        'uri': uri,
        'history': history
    }, **base))

    return apache.OK

def pid(id):
    return yank('''
        select
            "data"
        from "wikicyte"."page"
        where
            "id" = %(id)s
    ''', locals())
    
def diff(req, lid, rid):
    if wikidiff2 == None:
        return apache.HTTP_NOT_FOUND

    here = os.path.dirname(__file__)

    lpage = pid(lid)
    rpage = pid(rid)
    head(req)

    ldata = lpage['data'].encode('utf8')
    rdata = rpage['data'].encode('utf8')

    diff = wikidiff2.wikidiff2_do_diff(ldata, rdata, 2)
    diff = diff.replace('"></td>', '">&#160;</td>')

    psp.PSP(req, filename=(here + '/wikicyte-diff.psp')).run(vars=dict({
        'diff': diff
    }, **base))

    return apache.OK

def time(req, uri):
    # XXX: implement using uri!
    head(req)

    # XXX: naive datetime
    when = yank('''
        select "when"::timestamp
        from "wikicyte"."approved"(now())
        where
            ("approved" is null or "approved")
        order by "when" desc nulls last
        limit 1
    ''', locals())
    
    if when == None:
        req.write('&lt;unknown&gt;')
    else:
        when = when['when']
        when = etim(when)
        req.write(when.strftime('%B %d, %Y'))

    return apache.OK

def prev(req, rev):
    page = lrev(rev)
    uri = page['uri']
    return show(req, uri, page)

def approve(req, rev, value):
    req.get_basic_auth_pw()
    user = req.user
    host = req.connection.remote_ip

    if anon(user):
        return apache.HTTP_FORBIDDEN

    push('''
        insert into "wikicyte"."status" (
            "page", "approved", "user", "host"
        ) values (
            %(rev)s, %(value)s, %(user)s, %(host)s
        )
    ''', locals())

    return apache.OK

def back(req, mode, uri):
    if mode == '':
        return puri(req, '/_' + uri)
    elif mode == 'approve':
        rev = uri.split('/')
        bool = {'null': None, 'true': True, 'false': False}
        return approve(req, int(rev[1]), bool[rev[2]])
    elif mode == 'diff':
        rev = uri.split('/')
        return diff(req, int(rev[1]), int(rev[2]))
    elif mode == 'edit':
        return edit(req, uri)
    elif mode == 'history':
        return history(req, uri)
    elif mode == 'page':
        return prev(req, int(uri[1:]))
    elif mode == 'time':
        return time(req, uri)
    else:
        return apache.DECLINED

def lrev(rev):
    return yank("""
        select "uri", "when", "data"
        from "wikicyte"."page"
        where "id" = %(rev)s
    """, locals())

def luri(uri):
    return yank("""
        select "id", "uri", "when", "data"
        from "wikicyte"."approved"(now())
        where
            "uri" = %(uri)s and
            ("approved" is null or "approved")
        order by "when" desc nulls last
        limit 1
    """, locals())

def puri(req, uri):
    # XXX: [-1:0] didn't work
    if req.uri[len(req.uri) - 1] != '/':
        path = req.uri + '/'
        user = req.user
        if anon(user) and luri(path) == None:
            return apache.HTTP_NOT_FOUND
        else:
            util.redirect(req, path)
            return apache.OK

    page = luri(uri)
    return show(req, uri, page)

def anon(user):
    return user == None or user in anonymous_users

def smrt(html):
    html = smartypants.smartyPants(html)

    #html = re.sub(r'([^-])--([^-])', r'\1&#8212;\2', html)

    # XXX: only squash space if previous #?
    html = html.replace(' 1/2 ', '&#189; ')

    html = html.replace(' x ', '&#160;&#215;&#160;')
    html = html.replace('i.e.', '<em>i.e.</em>')
    html = html.replace('e.g.', '<em>e.g.</em>')

    html = re.sub(r'([0-9])-([0-9])', r'\1&#8211;\2', html)

    html = html.encode('utf8')

    if tidy != None:
        html = str(tidy.parseString(html,
            numeric_entities=1,
            output_xhtml=1,
            preserve_entities=1,
            show_body_only=1,
            char_encoding='utf8'
        ))

    return html

def show(req, uri, page):
    form = util.FieldStorage(req)
    mode = parm(form, 'mode')

    xuri = exml(uri)
    root = req.document_root()

    req.get_basic_auth_pw()
    user = req.user

    if page != None:
        when = page['when']
        data = page['data']
    elif anon(user):
        return apache.HTTP_NOT_FOUND
    else:
        file = open('%(root)s/default.mdml' % locals(), 'r')
        try:
            when = None
            data = file.read()
        finally:
            file.close()

    md = markdown.Markdown(markdown_extensions + ['meta'])

    meta = {}
    crumbs = []

    path = uri
    if path.endswith('/'):
        path = path[0:-1]
    path = path.split('/')

    for i in range(1, len(path)):
        suri = exml('/'.join(path[0:i]) + '/')
        last = luri(suri)
        if last == None:
            continue
        md.convert(last['data'])
        meta.update(md.Meta)
        if suri != '/' and 'title' in md.Meta:
            crumbs = [(suri, md.Meta['title'][0])] + crumbs

    html = data

    html = html.replace('\n', '\r\n')
    html = md.convert(html)
    html = smrt(html)

    meta.update(md.Meta)

    if len(crumbs) != 0 and not 'title' in md.Meta:
        crumbs = crumbs[:-1]

    head(req)
    cynfig.before(req)

    if when != None:
        when = when.strftime('%B %d, %Y')
    cyte(req, meta, base=req.construct_url(xuri), href=uri, user=user, time=when, mode=mode)

    req.write(html)
    req.write('''</div>''')

    for crumb in crumbs:
        req.write('''<a xmlns="http://www.w3.org/1999/xhtml" class="crumb" href="%s">%s</a>''' % (exml(crumb[0]), exml(crumb[1])))

    if not anon(user):
        req.write('''<ul class="menucyte">''')
        if wikidiff2 != None:
            menu(req, uri, 'history', 'calendar_view_week')
        menu(req, uri, 'edit', 'pencil')
        menu(req, uri, 'move', 'arrow_right')
        req.write('''<li>&#160;</li>''')
        req.write('''<li><span>%(xuri)s</span></li>''' % locals())
        req.write('''</ul>''')

    req.write('''</div>''')

    return apache.OK

def cyte(req, meta, **extra):
    req.write('''<div''')
    req.write(''' xmlns="http://www.w3.org/1999/xhtml"''')
    req.write(''' xmlns:cyte="http://www.wikicyte.com/2009/cyte"''')
    req.write(''' xmlns:cyml="http://www.wikicyte.com/2009/cyml"''')
    for item in extra.items():
        if item[1] != None:
            req.write(''' cyte:%s=%s''' % (item[0], eatt(item[1])))
    for item in meta.items():
        # XXX: filter illegal attribute names (or escape?)
        req.write(''' cyml:%s=%s''' % (item[0], eatt(item[1][0])))
    req.write('''>''')
    req.write('''<div class="wikicyte">''')

def search(req):
    form = util.FieldStorage(req)
    query = parm(form, 'q')

    host = realip(req)

    if query == None:
        # XXX: implement this?
        return apache.OK

    push('''
        insert into "wikicyte"."search" (
            "query", "host"
        ) values (
            %(query)s, %(host)s
        )
    ''', locals())

    terms = query.replace('"', '').split(' ')

    head(req)
    cynfig.before(req)

    md = markdown.Markdown(markdown_extensions + ['meta'])

    # XXX: this is stupid
    page = luri('/_search/')
    if page == None:
        meta = {}
    else:
        md.convert(page['data'])
        meta = dict(md.Meta)

    title = 'Search results: ' + query
    if 'title' in meta:
        meta['title'][0] = title
    else:
        meta['title'] = [title]

    cyte(req, meta, search=query)

    pages = pull('''
        select
            "current"."uri",
            "current"."data",
            ts_rank_cd("current"."search", "query") as "rank"
        from
            "wikicyte"."current"(now()) as "current",
            plainto_tsquery(%(query)s) as "query"
        where
            "current"."search" @@ "query" and
            substring("current"."uri" from 1 for 2) != '/_'
        order by
            "rank" desc
    ''', locals())

    rx = re.compile('(' + '|'.join(terms) + ')', re.IGNORECASE)
    rm = re.compile(r'{[^}]*}|<[^>]*>|\([^\)]*/[^\)]*\)|[][*#]')

    html = []

    if len(pages) == 0:
        html.append("<h2>No results found.</h2>")
    else:
        html.append('''<ul class="results">''')

        for page in pages:
            xuri = exml(page['uri'])
            data = page['data']
            if data == None:
                continue
            md.convert(data)
            if not 'title' in md.Meta:
                continue
            name = md.Meta['title']
            if name == None:
                continue
            name = name[0]

            context = None
            for line in data.split('\n'):
                line = line.encode('utf8')
                line = rm.sub(r'', line)
                line, count = rx.subn(r'<span class="term">\1</span>', exml(line))
                if count != 0:
                    context = line.decode('utf8')
                    break

            rank = page['rank']

            html.append('''<li><p><a href="%(xuri)s">%(name)s</a></p>''' % locals())
            if context != None:
                html.append('''<p class="second">%(context)s</p>''' % locals())
            html.append('''</li>''')

        html.append('''</ul>''')

    html = ''.join(html)
    html = smrt(html)
    req.write(html)

    req.write('''</div>''')
    req.write('''<span class="title">Search results: <em>%s</em></span>''' % (exml(query),))
    req.write('''</div>''')

    return apache.OK

def sitemap(req):
    head(req)
    cynfig.before(req)

    md = markdown.Markdown(markdown_extensions + ['meta'])

    page = luri('/_sitemap/')
    if page == None:
        meta = {}
    else:
        md.convert(page['data'])
        meta = dict(md.Meta)

    if 'title' in meta:
        meta['title'][0] = 'Site map'
    else:
        meta['title'] = ['Site map']

    cyte(req, meta)

    pages = pull('''
        select
            "current"."uri",
            "current"."data"
        from "wikicyte"."current"(now()) as "current"
        where
            substring("uri" from 1 for 2) != '/_' and
            "uri" != '/'
        order by "current"."uri"
    ''', locals())

    uris = ['/']

    html = []
    html.append('''<ul>''')

    for page in pages:
        xuri = exml(page['uri'])
        if page['data'] == None:
            continue
        md.convert(page['data'])
        if not 'title' in md.Meta:
            continue
        name = md.Meta['title']
        if name == None:
            continue
        name = name[0]

        last = uris[-1]
        while not xuri.startswith(uris[-1]):
            uris = uris[:-1]
            html.append('''</ul></li>''')

        html.append('''<li><a href="%(xuri)s">%(name)s</a><ul>''' % locals())
        uris += [xuri]

    for last in range(1, len(uris)):
        html.append('''</ul></li>''')

    html.append('''</ul>''')

    html = ''.join(html)
    html = smrt(html)

    req.write(html)

    req.write('''</div>''')
    req.write('''</div>''')

    return apache.OK

def query(req):
    req.get_basic_auth_pw()
    user = req.user

    host = realip(req)

    if anon(user) and not host in ('68.6.35.253', '74.208.44.105'):
        return apache.HTTP_FORBIDDEN

    form = util.FieldStorage(req)
    query = parm(form, 'q')

    head(req)

    req.write('<html><head>')
    req.write('</head><body>')
    req.write('<table>')
    req.write('<tr>')

    sql = getdb()
    try:
        csr = cursor(sql)
        try:
            csr.execute(query, locals())
            for col in csr.description:
                req.write('<th>%s</th>' % (exml(col[0]),))
            rows = csr.fetchall()
        finally:
            csr.close()
    finally:
        putdb(sql)

    req.write('</tr>')

    for row in rows:
        req.write('<tr>')
        for value in row:
            req.write('<td>%s</td>' % (exml(str(value)),))
        req.write('</tr>')

    req.write('</table>')
    req.write('</body></html>')

    return apache.OK

def handler(req):
    uri = req.uri

    if uri.startswith('/_'):
        uri = uri[2:]
        slash = uri.find('/')
        if slash != -1:
            return back(req, uri[0:slash], uri[slash:])
        elif uri == 'query':
            return query(req)
        elif uri == 'history':
            return history(req, None)
        elif uri == 'search':
            return search(req)
        elif uri == 'sitemap':
            return sitemap(req)
        elif uri == 'time':
            return time(req, None)
        return apache.HTTP_NOT_FOUND
    else:
        return puri(req, uri)
