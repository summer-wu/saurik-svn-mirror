from saurik import *

def main():
    form = cgi.FieldStorage()
    package_id = form.getfirst('package')
    package = Package.get_by_primary_key(package_id)

    head(None, [], ['appspot/rating'])
    print '</head><body>'

    print '<a href="/reviews/?package=%s"><div class="link">' % cgi.escape(urllib.quote_plus(package_id), True)

    package_rating = package.rating() if package else None

    if not package_rating:
        print '<div class="image no star"></div>'
        print '<span>Not Rated</span>'
    else:
        for star in range(0, 5):
            _class = 'one' if star < package_rating else 'zero'
            print '<div class="image %s star"></div>' % cgi.escape(_class, True)

    print     '<div class="image arrow"></div>'
    print '</div></a>'

    foot()

saurik(main)
