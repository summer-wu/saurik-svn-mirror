import cgi, os, re, sys, urllib, hashlib
from datetime import datetime

from google.appengine.api import users
from google.appengine.ext import db

def insert_or_update_(model, key_name, **kw):
    review = model.get_by_key_name(key_name)
    if not review:
        review = model(key_name=key_name, **kw)
    else:
        keys = kw.keys()
        for key in keys:
            setattr(review, key, kw[key])
    review.put()
    return review
def insert_or_update(cls, key_name, **kw):
    return db.run_in_transaction(insert_or_update_, cls, key_name, **kw)
def get_by_primary_key(cls, *args):
    return cls.get_by_key_name(cls.primary_key(*args))
def goi_by_primary_key(cls, *args, **kw):
    return cls.get_or_insert(cls.primary_key(*args), **kw)
def iou_by_primary_key(cls, *args, **kw):
    return cls.insert_or_update(cls.primary_key(*args), **kw)
def saurik(main):
    db.Model.insert_or_update = classmethod(insert_or_update)
    db.Model.get_by_primary_key = classmethod(get_by_primary_key)
    db.Model.goi_by_primary_key = classmethod(goi_by_primary_key)
    db.Model.iou_by_primary_key = classmethod(iou_by_primary_key)
    main()

def head(title, scripts, styles):
    print 'Content-Type: text/html'
    print 'Cache-Control: private, proxy-revalidate'
    print ''
    print '<html><head>'
    print '<meta name="viewport" content="width=device-width, minimum-scale=1.0, maximum-scale=1.0"/>'
    if title:
        print '<title>%s</title>' % title
    for script in scripts:
        print '<script type="text/javascript" src="http://cydia.saurik.com/%s.js"></script>' % script
    for style in styles:
        print '<link rel="stylesheet" type="text/css" href="http://cydia.saurik.com/%s.css"/>' % style
    print '<base target="_blank"/>'

def foot():
    print '</body></html>'

def debug():
    for key in os.environ.keys():
        print key, ' = ', os.environ[key]

def auth():
    user = users.get_current_user()
    if not user:
        #host = os.environ['HTTP_HOST']
        path = os.environ['PATH_INFO']
        #uri = 'http://' + host + path
        uri = path

        print 'Location: ' + users.create_login_url(uri)
        print ''

        exit()

    role = os.environ["HTTP_X_ROLE"]
    account = GoogleUser.goi_account_by_primary_key(user, role=role)

    agent = os.environ["HTTP_USER_AGENT"]
    match = re.search('.*iPhone OS ([0-9_]*).*', agent)
    firmware = match.group(1).replace('_', '.') if match else None

    device_id = os.environ['HTTP_X_UNIQUE_ID']
    machine = os.environ["HTTP_X_MACHINE"]
    device = Device.iou_by_primary_key(device_id, id=device_id, firmware=firmware, machine=machine, account=account)

    return user, account, device

class FiveStarRatingProperty(db.IntegerProperty):
    def __init__(self, **kw):
        super(FiveStarRatingProperty, self).__init__(choices=set([0, 1, 2, 3, 4, 5]), **kw)

class IPAddressProperty(db.StringProperty):
    def validate(self, value):
        if not super(IPAddressProperty, self).validate(value):
            return False
        # XXX: implement IP address validation
        return True

class Account(db.Model):
    # control
    added = db.DateTimeProperty(required=True, auto_now_add=True)
    index = db.IntegerProperty(required=True, default=0)

    # implicit
    role = db.StringProperty(required=True)

    # explicit
    nickname = db.StringProperty(required=False)

    # automatic
    reviews_count = db.IntegerProperty(required=True, default=0)

    @staticmethod
    def monotonic_(id):
        account = Account.get_by_id(id)
        index = account.index
        account.index = index + 1
        account.put()
        return index

    def monotonic(self):
        id = self.key().id()
        return '0|' + str(datetime.today()) + '|' + str(id) + '|' + str(db.run_in_transaction(Account.monotonic_, id))

class GoogleUser(db.Model):
    # primary
    user = db.UserProperty(required=True)

    # control
    added = db.DateTimeProperty(required=True, auto_now_add=True)

    # explicit
    account = db.ReferenceProperty(Account, required=False) #XXX

    @staticmethod
    def primary_key(user):
        sha1 = hashlib.sha1()
        sha1.update(user.email())
        return 'sha1:' + sha1.hexdigest()

    @staticmethod
    def goi_account_by_primary_key_(user, hash, **kw):
        gooser = GoogleUser.get_by_key_name(hash)
        if gooser:
            account = db.get(gooser.account)
            keys = kw.keys()
            for key in keys:
                setattr(account, key, kw[key])
            account.put()
        else:
            account = Account(**kw)
            account.put()
            gooser = GoogleUser(parent=account, key_name=hash, user=user, account=account)
            gooser.put()
        return account

    @staticmethod
    def goi_account_by_primary_key(user, **kw):
        hash = GoogleUser.primary_key(user)
        return db.run_in_transaction(GoogleUser.goi_account_by_primary_key_, user, hash, **kw)

class Package(db.Model):
    # primary
    id = db.StringProperty(required=True)

    # control
    noticed = db.DateTimeProperty(required=True, auto_now_add=True)

    # automatic
    reviews_count = db.IntegerProperty(required=True, default=0)
    reviews_avg_concept = db.FloatProperty(required=False)
    reviews_avg_reality = db.FloatProperty(required=False)
    reviews_avg_minimum = db.FloatProperty(required=False)

    def rating():
        return self.reviews_avg_minimum

    @staticmethod
    def primary_key(id):
        return id

class Device(db.Model):
    # primary
    id = db.StringProperty(required=True)

    # control
    noticed = db.DateTimeProperty(required=True, auto_now_add=True)

    # implicit
    firmware = db.StringProperty(required=True)
    machine = db.StringProperty(required=True)

    # explicit
    account = db.ReferenceProperty(Account, required=False, collection_name='devices')

    @staticmethod
    def primary_key(id):
        return id

class Rating(db.Model):
    # primary
    package = db.ReferenceProperty(Package, required=True, collection_name='ratings')
    shard = db.IntegerProperty(required=True, default=0)

    # internal
    index = db.IntegerProperty(required=True, default=0)

    # automatic
    reviews_count = db.IntegerProperty(required=True, default=0)
    reviews_sum_concept = db.IntegerProperty(required=True, default=0)
    reviews_sum_reality = db.IntegerProperty(required=True, default=0)
    reviews_sum_minimum = db.IntegerProperty(required=True, default=0)

    @staticmethod
    def primary_key(package_id, index):
        return package_id + '|' + index

class Review(db.Model):
    # primary
    package = db.ReferenceProperty(Package, required=True, collection_name='reviews')
    account = db.ReferenceProperty(Account, required=True, collection_name='reviews')

    # control
    posted = db.DateTimeProperty(required=True, auto_now_add=True)
    updated = db.DateTimeProperty(required=True, auto_now=True)
    rating = db.IntegerProperty(required=True, default=0)
    monotonic = db.StringProperty(required=True)

    # implicit
    device = db.ReferenceProperty(Device, required=True, collection_name='reviews')
    version = db.StringProperty(required=True)

    # explicit
    title = db.StringProperty(required=True)
    concept = FiveStarRatingProperty(required=True)
    reality = FiveStarRatingProperty(required=True)
    comment = db.TextProperty(required=True)

    # automatic
    account_role = db.StringProperty(required=True)
    device_firmware = db.StringProperty(required=True)
    device_machine = db.StringProperty(required=True)

    @staticmethod
    def primary_key(package_id, account):
        return package_id + '|' + str(account.key().id())
