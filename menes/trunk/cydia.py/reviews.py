from saurik import *
from _reviews import *

def main():
    form = cgi.FieldStorage()
    package_id = form.getfirst('package')
    package = Package.get_by_primary_key(package_id)

    head('Reviews', ['menes/menes', 'appspot/reviews'], ['menes/style'])
    print '</head><body>'

    print '<dialog>'
    print     '<panel>'

    if package:
        reviews = Review.gql("where package = :1 order by monotonic desc", package).fetch(per + 1)
        if reviews:
            list(reviews)
        else:
            print '<fieldset>'
            print     '<div>'
            print     '</div>'
            print '</fieldset>'

    print     '</panel>'
    print '</dialog>'

saurik(main)
