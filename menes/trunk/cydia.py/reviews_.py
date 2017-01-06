from saurik import *
from _reviews import *

def main():
    form = cgi.FieldStorage()
    package_id = form.getfirst('package')
    package = Package.get_by_primary_key(package_id)
    monotonic = form.getfirst('monotonic')

    if package:
        reviews = Review.gql("where package = :1 and monotonic <= :2 order by monotonic desc", package, monotonic).fetch(per + 1)
        if reviews:
            list(reviews)

saurik(main)
