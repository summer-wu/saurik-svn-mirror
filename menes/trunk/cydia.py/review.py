from saurik import *

def stars(label, name, value, options):

    print '<div>'
    print     '<label>%s</label>' % cgi.escape(label, False)
    print     '<select name="%s" class="saved" default="">' % cgi.escape(name, True)
    print         '<option value="">&#x272c;&#x272c;&#x272c;&#x272c;&#x272c; (Unrated)</option>'

    for stars in range(5, -1, -1):
        print '<option value="%u"' % stars,
        if stars == value:
            print 'selected="selected"',
        sys.stdout.softspace = False
        print '>',
        for i in range(0, stars):
            sys.stdout.softspace = False
            print '&#x2605;',
        for i in range(stars, 5):
            sys.stdout.softspace = False
            print '&#x2606;',
        print '(',
        sys.stdout.softspace = False
        if options[stars]:
            print cgi.escape(options[stars], False),
        else:
            print '%u stars' % stars,
        sys.stdout.softspace = False
        print ')</option>'

    print     '</select>'
    print '</div>'

def main():
    form = cgi.FieldStorage()
    path = os.environ["PATH_INFO"]
    package_id = form.getfirst("package")
    uri = path + urllib.quote_plus('?package=' + package_id) + '*'
    package = Package.get_by_primary_key(package_id)
    user, account, device = auth()
    review = Review.get_by_primary_key(package_id, account) if package else None

    if review:
        title = review.title
        concept = review.concept
        reality = review.reality
        comment = review.comment
    else:
        title = ""
        concept = -1
        reality = -1
        comment = ""

    head('Add Review', ['menes/menes', 'menes/iphone', 'appspot/review'], ['menes/style'])
    print '</head><body bgcolor="white">'

    print '<form name="review" method="POST" action="/submit/">'

    print '<input type="hidden" name="package" value="%s"/>' % cgi.escape(package_id, True)
    print '<input type="hidden" name="version"/>'

    print '<dialog>'
    print     '<fieldset>'
    # XXX: this has complicated (and incorrect, fuck you Google), escaping
    print         '<a href="javascript:logout(\'%s\')">' % users.create_logout_url(uri)
    print             '<label>Account</label>'
    print             '<div>%s</div>' % account.key().id(), cgi.escape(user.nickname(), False)
    print         '</a>'

    print         '<div>'
    print             '<label>Title</label>'
    print             '<input type="text" name="title" class="saved" default="a short title for your review" value="%s" autocapitalize="off"/>' % cgi.escape(title, True)
    print         '</div>'

    stars('Concept', 'concept', concept, ['Lame', None, None, None, None, 'Ingenious'])
    stars('Reality', 'reality', reality, ['Failure', None, None, None, None, 'Flawless'])

    print         '<textarea name="comment" style="min-height: 160px" class="saved" default="Provide a detailed package review here.&#x0a;Multiple lines and long posts are allowed." autocapitalize="off">%s</textarea>' % cgi.escape(comment, False)
    print     '</fieldset>'
    print '</dialog>'

    print '</form>'

    foot()

saurik(main)
