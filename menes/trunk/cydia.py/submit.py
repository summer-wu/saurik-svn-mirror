from saurik import *

def main():
    form = cgi.FieldStorage()
    user, account, device = auth()

    package_id = form.getfirst('package')
    package = Package.goi_by_primary_key(package_id, id=package_id)
    version = form.getfirst('version')

    title = form.getfirst('title')
    concept = form.getfirst('concept')
    reality = form.getfirst('reality')
    comment = form.getfirst('comment')

    concept = int(concept)
    reality = int(reality)

    monotonic = account.monotonic()

    review = Review.iou_by_primary_key(package_id, account,
        package=package,
        account=account,
        monotonic=monotonic,
        device=device,
        version=version,
        title=title,
        concept=concept,
        reality=reality,
        comment=comment,
        account_role=account.role,
        device_firmware=device.firmware,
        device_machine=device.machine,
    )

    head('Submitted', [], [])
    print '</head><body>'

    foot()

saurik(main)
