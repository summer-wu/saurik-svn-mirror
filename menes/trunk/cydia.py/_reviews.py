import cgi

per = 25

def list(reviews):
    if len(reviews) == per + 1:
        extra = reviews[-1]
        reviews = reviews[:per]
    else:
        extra = None

    for review in reviews:
        print '<fieldset>'
        print     '<div>'
        print         '<label>%s</label>' % cgi.escape(review.key().name(), False)
        print     '</div>'
        print '</fieldset>'

    if extra:
        print '<fieldset>'
        print     '<div>'
        print     '</div>'
        print '</fieldset>'
