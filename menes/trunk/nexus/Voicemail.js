var println = function(s) { java.lang.System.out.println(s); }
var sql = Packages.SQLite;

var accounts = obc.MailAccount.activeAccounts();
var account = accounts.objectAtIndex$(4);
var root = account.rootMailboxUid();
var vmail = root.childWithName$("Voicemail");
var store = account.storeForMailboxUid$(vmail);
var messages = store.copyOfAllMessages();

var ab = new sql.Database();
ab.open("/var/root/Library/AddressBook/AddressBook.sqlitedb", 0666); try {
    var st = ab.prepare("select first, last from ABPerson as p join ABMultiValue as mv on mv.record_id = p.rowid where value = ?");

var voices = [];
for (var i = 0, e = messages.count(); i != e; ++i) {
    var message = messages.objectAtIndex$(i);
    if (message.sender() == 'mailbox@my.simulscribe.com') {
        var subject = message.subject().toString();
        var phone = subject.substring(17, 17 + 14);
        st.bind(1, phone);
        var name;
        if (!st.step())
            name = "<Unknown>";
        else {
            var first = st.column_string(0);
            var last = st.column_string(1);
            if (first == null)
                name = last;
            else {
                name = first;
                if (last != null)
                    name += " " + last;
            }
        }
        var body = message.messageBody();
        var text = null;
        /*try {
            var html = body.htmlContent().objectAtIndex$(0);
            var content = new obc.NSString().initWithData$encoding$(html.htmlData(), 4).toString();
            text = content.subSequence(18, content.indexOf('<', 18) - 13);
        } catch (e) {}*/
        voices.push({
            phone: phone,
            name: name,
            sent: message.dateSent().descriptionWithCalendarFormat$timeZone$locale$("%H:%M:%S", null, null),
            message: message,
            text: text
        });
        st.reset();
    }
}

} finally { ab.close(); }

var outer = obc.UIHardware.fullScreenApplicationContentRect();
var inner = outer.clone();
inner.origin.x = inner.origin.y = 0;

var window = new obc.UIWindow().initWithContentRect$(inner);
window.orderFront$(application);
window.makeKey$(application);
window._setHidden$(NO);

var navsize = obc.UINavigationBar().defaultSize();
var navbar = new obc.UINavigationBar().initWithFrame$(new obc.CGRect(0, 0, inner.size.width, navsize.height));
navbar.setBarStyle$(1);

var navitem = new obc.UINavigationItem().initWithTitle$("Voicemail");
navbar.pushNavigationItem$(navitem);

var col = new obc.UITableColumn().initWithTitle$identifier$width$("Name", "name", 320);

var table = new obc.UITable().initWithFrame$(new obc.CGRect(0, navsize.height, inner.size.width, inner.size.height - navsize.height));
table.setSeparatorStyle$(1);
table.addTableColumn$(col);

table.setDataSource$({
    numberOfRowsInTable$: function(table) {
        return voices.length;
    },

    table$cellForRow$column$: function(table, row, col) {
        var cell = new obc.UIImageAndTextTableCell().init();
        var voice = voices[row];
        cell.setTitle$(voice.name + " " + voice.sent);
        return cell;
    }
});

table.setDelegate$({
    tableRowSelected$: function(notification) {
        var table = notification.object();
        var voice = voices[table.selectedRow()];
        var message = voice.message;

        var body = message.messageBody();
        var file = body.htmlContent().objectAtIndex$(1).fileWrapperForcingDownload$(YES)
        file.writeToFile$atomically$updateFilenames$("/tmp/voicemail.mp3", YES, YES)
        var controller = new obc.AVController().init();
        var wavitem = new obc.AVItem().initWithPath$error$("/tmp/voicemail.mp3", null);
        wavitem.setVolume$(100);
        controller.setCurrentItem$(wavitem);
        controller.setCurrentTime$(0);
        controller.play$(null);
    }
});

table.reloadData();

var view = new obc.UIView().initWithFrame$(inner);
view.addSubview$(navbar);
view.addSubview$(table);

window.setContentView$(view);
