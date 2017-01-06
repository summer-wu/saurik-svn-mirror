/* Ninetjer - _Truly_ Free (non-GPL) Unix .NET Runtime
 * Copyright (C) 2004-2005  Jay Freeman (saurik)
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

#include "api/exename.hpp"
#include "api/files.hpp"

#include "app/simple.hpp"
#include "ios/helpers.hpp"
#include "net/http/urlencode.hpp"
#include "opt/memory.hpp"

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include <libgnomeui/gnome-ui-init.h>
#include <libgnomevfs/gnome-vfs-init.h>
#include <libgnomevfs/gnome-vfs-utils.h>

#include <cstdlib>
#include <memory>

#include <Platform.h>
#include <Scintilla.h>
#include <ScintillaWidget.h>
#include <SciLexer.h>

#include <terminal-widget.h>

#include <sys/types.h>
#include <dirent.h>

extern "C" {
#include <libnautilus-private/nautilus-cell-renderer-pixbuf-emblem.h>
#include <file-manager/fm-tree-model.h>
}

// http://scentric.net/tutorial/treeview-tutorial.html

const GdkColor palette[TERMINAL_PALETTE_SIZE] = {
    {0, 0x0000, 0x0000, 0x0000},
    {0, 0xcdcd, 0x0000, 0x0000},
    {0, 0x0000, 0xcdcd, 0x0000},
    {0, 0xcdcd, 0xcdcd, 0x0000},
    {0, 0x0000, 0x0000, 0xcdcd},
    {0, 0xcdcd, 0x0000, 0xcdcd},
    {0, 0x0000, 0xcdcd, 0xcdcd},
    {0, 0xfafa, 0xebeb, 0xd7d7},
    {0, 0x4040, 0x4040, 0x4040},
    {0, 0xffff, 0x0000, 0x0000},
    {0, 0x0000, 0xffff, 0x0000},
    {0, 0xffff, 0xffff, 0x0000},
    {0, 0x0000, 0x0000, 0xffff},
    {0, 0xffff, 0x0000, 0xffff},
    {0, 0x0000, 0xffff, 0xffff},
    {0, 0xffff, 0xffff, 0xffff}
};

const GdkColor foreground = {0, 0xd9d9, 0xd9d9, 0xd9d9};
const GdkColor background = {0, 0x0000, 0x0000, 0x0000};

cse::String project;

GtkWindow *window(NULL);
GtkWidget *tabs(NULL);
GtkWidget *tree(NULL);
FMTreeModel *model(NULL);
GtkTreeModel *sorted(NULL);

GtkWidget *build(NULL);

ext::RedBlackMap<cse::String, GtkWidget *> files;
ext::RedBlackMap<GtkWidget *, cse::String> back;

void CommandDied(GtkWidget *terminal, void *arg) {
    ::gtk_notebook_remove_page(GTK_NOTEBOOK(tabs), ::gtk_notebook_page_num(GTK_NOTEBOOK(tabs), terminal));
    files.Remove(back[terminal]);
    back.Remove(terminal);
}

PangoFontDescription *GetSystemFont() {
    GConfClient *conf(::gconf_client_get_default());
    char *name(::gconf_client_get_string(conf, "/desktop/gnome/interface/monospace_font_name", NULL));

    PangoFontDescription *desc(NULL);

    if (name != NULL) {
        desc = pango_font_description_from_string (name);
        ::g_free(name);
    }

    if (desc == NULL)
        desc = pango_font_description_from_string ("Monospace 10");

    return desc;
}

pid_t Spawn(GtkWidget *terminal, const cse::String &working, const cse::String &path, _L<cse::String> arguments) {
    int process;
    GError *error(NULL);

    arguments.InsertFirst(path);
    ext::Vector<const char *> pointers;
    pointers.Reserve(arguments.GetSize());

    _foreach (_L<cse::String>, argument, arguments)
        pointers.InsertLast(argument->NullTerminate());

    ::terminal_widget_fork_command(terminal, false, false, path.NullTerminate(), const_cast<char **>(pointers.NullTerminate()), environ, working.NullTerminate(), &process, &error);

    return process;
}

GtkWidget *NewTerminal() {
    GtkWidget *terminal(::terminal_widget_new());
    ::terminal_widget_set_colors(terminal, &foreground, &background, palette);
    ::terminal_widget_set_pango_font(terminal, GetSystemFont());
    return terminal;
}

GtkWidget *Spawn(const cse::String &working, const cse::String &path, _L<cse::String> arguments) {
    GtkWidget *terminal(NewTerminal());
    Spawn(terminal, working, path, arguments);
    ::terminal_widget_connect_child_died(terminal, G_CALLBACK(CommandDied), NULL);
    return terminal;
}

void AssociateTab(const cse::String &name, GtkWidget *widget) {
    GtkWidget *label(::gtk_label_new(name.NullTerminate()));
    ::gtk_notebook_append_page(GTK_NOTEBOOK(tabs), widget, label);
    ::gtk_widget_show(widget);
    ::gtk_widget_show(label);
    ::gtk_notebook_set_current_page(GTK_NOTEBOOK(tabs), ::gtk_notebook_get_n_pages(GTK_NOTEBOOK(tabs)) - 1);
}

void AssociateUri(const cse::String &uri, GtkWidget *widget) {
    cse::String name(uri.FindLast('/') + 1, uri.End());
    AssociateTab(name, widget);

    files[uri] = widget;
    back[widget] = uri;
}

class Scintilla :
    public ios::Writer
{
  private:
    GtkWidget *widget_;

  public:
    Scintilla() :
        widget_(::scintilla_new())
    {
        StyleClearAll();
        SetLexer(SCLEX_CPP);
        SetKeywords(_B("int char"));

        StyleSetFore(SCE_C_COMMENT, 0x007f00);
        StyleSetFore(SCE_C_COMMENTLINE, 0x007f00);
        StyleSetFore(SCE_C_COMMENTDOC, 0x3f703f);

        StyleSetFore(SCE_C_NUMBER, 0x007f7f);
        StyleSetFore(SCE_C_WORD, 0x00007f);
        StyleSetFore(SCE_C_STRING, 0x7f007f);
        StyleSetFore(SCE_C_CHARACTER, 0x7f007f);
        StyleSetFore(SCE_C_OPERATOR, 0x000000);
        StyleSetFore(SCE_C_PREPROCESSOR, 0x7f7f00);
    }

    intptr_t SendMessage(unsigned message, uintptr_t wparam, intptr_t lparam) {
        return ::scintilla_send_message(SCINTILLA(widget_), message, wparam, lparam);
    }

    intptr_t SendMessage(unsigned message, uintptr_t wparam, const cse::String &lparam) {
        return SendMessage(message, wparam, reinterpret_cast<intptr_t>(lparam.NullTerminate()));
    }

    void AddText(size_t length, const char *data) {
        SendMessage(SCI_ADDTEXT, length, reinterpret_cast<intptr_t>(data));
    }

    void SetKeywords(const cse::String &keywords) {
        SendMessage(SCI_SETKEYWORDS, 0, keywords);
    }

    void SetLexer(uintptr_t lexer) {
        SendMessage(SCI_SETLEXER, lexer, 0);
    }

    void SetSelectionEnd(int position) {
        SendMessage(SCI_SETSELECTIONEND, position, 0);
    }

    void SetSelectionStart(int position) {
        SendMessage(SCI_SETSELECTIONSTART, position, 0);
    }

    void StyleClearAll() {
        SendMessage(SCI_STYLECLEARALL, 0, 0);
    }

    void StyleResetDefault() {
        SendMessage(SCI_STYLERESETDEFAULT, 0, 0);
    }

    void StyleSetBold(unsigned style, bool bold) {
        SendMessage(SCI_STYLESETBOLD, style, bold);
    }

    void StyleSetCase(unsigned style, bool _case) {
        SendMessage(SCI_STYLESETCASE, style, _case);
    }

    void StyleSetChangable(unsigned style, bool changeable) {
        SendMessage(SCI_STYLESETCHANGEABLE, style, changeable);
    }

    void StyleSetCharacterSet(unsigned style, int charset) {
        SendMessage(SCI_STYLESETCHARACTERSET, style, charset);
    }

    void StyleSetEolFilled(unsigned style, bool eolfilled) {
        SendMessage(SCI_STYLESETEOLFILLED, style, eolfilled);
    }

    void StyleSetFont(unsigned style, const cse::String &font) {
        SendMessage(SCI_STYLESETFONT, style, font);
    }

    void StyleSetFore(unsigned style, int color) {
        SendMessage(SCI_STYLESETFORE, style, color);
    }

    void StyleSetHotSpot(unsigned style, bool hotspot) {
        SendMessage(SCI_STYLESETHOTSPOT, style, hotspot);
    }

    void StyleSetItalic(unsigned style, bool italic) {
        SendMessage(SCI_STYLESETITALIC, style, italic);
    }

    void StyleSetSize(unsigned style, int points) {
        SendMessage(SCI_STYLESETSIZE, style, points);
    }

    void StyleSetUnderline(unsigned style, bool underline) {
        SendMessage(SCI_STYLESETUNDERLINE, style, underline);
    }

    void StyleSetVisible(unsigned style, bool visible) {
        SendMessage(SCI_STYLESETVISIBLE, style, visible);
    }

    virtual size_t Write(const byte_t *data, size_t length) {
        AddText(length, data);
        return length;
    }

    virtual void Flush() {
    }

    operator GtkWidget *() const {
        return widget_;
    }
};

void Refocus() {
    ::gtk_widget_grab_focus(::gtk_notebook_get_nth_page(GTK_NOTEBOOK(tabs), ::gtk_notebook_get_current_page(GTK_NOTEBOOK(tabs))));
}

void SelectFile(GtkTreeView *tree, GtkTreePath *path, GtkTreeViewColumn *column, void *arg) {
    GtkTreeIter iterator;
    ::gtk_tree_model_get_iter(sorted, &iterator, path);

    GtkTreeIter child;
    ::gtk_tree_model_sort_convert_iter_to_child_iter(GTK_TREE_MODEL_SORT(sorted), &child, &iterator);

    NautilusFile *file(::fm_tree_model_iter_get_file(model, &child));
    const char *uri(::nautilus_file_get_uri(file));

    // XXX: this incorrectly uses the UrlDecoder concept
    cse::String name(ios::Filter<net::Http::UrlDecoder>(uri + 7));

    if (DIR *dir = ::opendir(name.NullTerminate())) {
        ::closedir(dir);

        if (::gtk_tree_view_row_expanded(tree, path))
            ::gtk_tree_view_collapse_row(tree, path);
        else
            ::gtk_tree_view_expand_row(tree, path, FALSE);
    } else if (GtkWidget *widget = files[uri]) {
        ::gtk_notebook_set_current_page(GTK_NOTEBOOK(tabs), ::gtk_notebook_page_num(GTK_NOTEBOOK(tabs), widget));
    } else {
        Scintilla scintilla;

        api::FileReader reader(name);
        ios::ReadToWrite(reader, scintilla);

        AssociateUri(uri, scintilla);

        scintilla.SetSelectionStart(0);
        scintilla.SetSelectionEnd(0);

        Refocus();
    }
}

static int CompareFiles(GtkTreeModel *model, GtkTreeIter *lhs, GtkTreeIter *rhs, gpointer callback_data) {
    if (lhs->user_data == NULL || rhs->user_data == NULL)
        return -1;
    if (::fm_tree_model_iter_is_root(FM_TREE_MODEL(model), lhs) || ::fm_tree_model_iter_is_root(FM_TREE_MODEL(model), rhs))
        return 0;

    NautilusFile *lfile(::fm_tree_model_iter_get_file(FM_TREE_MODEL(model), lhs));
    NautilusFile *rfile(::fm_tree_model_iter_get_file(FM_TREE_MODEL(model), rhs));

    int result;
    if (lfile == rfile)
        result = 0;
    else if (lfile == NULL)
        result = -1;
    else if (rfile == NULL)
        result = +1;
    else
        result = nautilus_file_compare_for_sort(lfile, rfile, NAUTILUS_FILE_SORT_BY_DISPLAY_NAME, FALSE, FALSE);

    ::nautilus_file_unref(lfile);
    ::nautilus_file_unref(rfile);

    return result;
}

void AddDirectory(const cse::String &name, const cse::String &path) {
    char *uri(::gnome_vfs_get_uri_from_local_path(path.NullTerminate()));
    ::fm_tree_model_add_root_uri(model, uri, name.NullTerminate(), "gnome-fs-directory");
    ::g_free(uri);
}

GtkWidget *AppendMenu(GtkWidget *menu, const char *text, const cse::String &name = cse::String(), void (*function)(GtkWidget *, void *) = NULL, void *arg = NULL, guint key = 0, GdkModifierType modifier = GdkModifierType(0)) {
    if (key != 0) {
        cse::String path(_B("<Menes-Develop>/") + name + "/" + text);
        ::gtk_accel_map_add_entry(path.NullTerminate(), key, modifier);
    }

    GtkWidget *item(::gtk_menu_item_new_with_mnemonic(text));

    ::gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);

    if (function != NULL)
        ::g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(function), arg);

    return item;
}

void PreviousTab(GtkWidget *item, void *arg) {
    if (int pages = ::gtk_notebook_get_n_pages(GTK_NOTEBOOK(tabs))) {
        int page(::gtk_notebook_get_current_page(GTK_NOTEBOOK(tabs)));
        ::gtk_notebook_set_current_page(GTK_NOTEBOOK(tabs), (page - 1) % pages);
        Refocus();
    }
}

void NextTab(GtkWidget *item, void *arg) {
    if (int pages = ::gtk_notebook_get_n_pages(GTK_NOTEBOOK(tabs))) {
        int page(::gtk_notebook_get_current_page(GTK_NOTEBOOK(tabs)));
        ::gtk_notebook_set_current_page(GTK_NOTEBOOK(tabs), (page + 1) % pages);
        Refocus();
    }
}

void SaveFile(GtkWidget *item, void *arg) {
    // XXX: implement something
}

void CloseTab(GtkWidget *item, void *arg) {
    // XXX: implement something
}

void BuildComplete(GtkWidget *terminal, void *arg) {
    // XXX: implement something
}

void Compile(GtkWidget *item, void *arg) {
    // XXX: don't run while building
    // XXX: start some animated icon
    Spawn(build, project, "/usr/bin/make", _L<cse::String>());
}

void Debug(GtkWidget *item, void *arg) {
    _L<cse::String> arguments;
    arguments.InsertLast("debug");
    AssociateTab("<Debug>", Spawn(project, "/usr/bin/make", arguments));
}

void CloseWindow(GtkWidget *widget, void *arg) {
    ::gtk_main_quit();
}

GdkModifierType operator |(GdkModifierType lhs, GdkModifierType rhs) {
    return static_cast<GdkModifierType>(static_cast<int>(lhs) | static_cast<int>(rhs));
}

gint KeyPress(GtkWidget *widget, GdkEventKey *event, void *arg) {
    bool stop(true);
    if (event->keyval == GDK_Tab && (event->state & GDK_MODIFIER_MASK) == GDK_CONTROL_MASK)
        NextTab(widget, NULL);
    else if (event->keyval == GDK_ISO_Left_Tab && (event->state & GDK_MODIFIER_MASK) == (GDK_CONTROL_MASK | GDK_SHIFT_MASK))
        PreviousTab(widget, NULL);
    else
        stop = false;
    if (stop)
        ::gtk_signal_emit_stop_by_name(GTK_OBJECT(window), "key_press_event");
    return 0;
}

int Main(const app::Options &options) {
    project = options.GetArguments().IsEmpty() ? ::get_current_dir_name() : options.GetArguments()[0];

    {
        int argc(1);
        cse::String exe(api::GetExecutablePath().GetAll());
        const char *args[2] = {exe.NullTerminate(), NULL};
        char **argv(const_cast<char **>(args));
        ::gnome_program_init("develop", "0.9", LIBGNOMEUI_MODULE, argc, argv, NULL);
        ::gtk_init(&argc, &argv);
    }

    ::gnome_vfs_init();

    window = GTK_WINDOW(::gtk_window_new(GTK_WINDOW_TOPLEVEL));
    ::gtk_window_set_title(window, ("Develop - (" + project + ")").NullTerminate());
    ::g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(CloseWindow), NULL);
    ::gtk_signal_connect(GTK_OBJECT(window), "key_press_event", G_CALLBACK(KeyPress), NULL);

    GtkWidget *box(::gtk_vbox_new(FALSE, 0));
    ::gtk_container_add(GTK_CONTAINER(window), box);

    GtkAccelGroup *accelerators(::gtk_accel_group_new());
    ::gtk_window_add_accel_group(window, accelerators);

    GtkWidget *menubar(::gtk_menu_bar_new());
    ::g_signal_connect(menubar, "can_activate_accel", G_CALLBACK(gtk_true), NULL);
    ::gtk_box_pack_start(GTK_BOX(box), menubar, FALSE, FALSE, 0);

    {
        GtkWidget *item(AppendMenu(menubar, "File"));
        GtkWidget *menu(::gtk_menu_new());
        ::gtk_menu_set_accel_group(GTK_MENU(menu), accelerators);
        ::gtk_menu_set_accel_path(GTK_MENU(menu), "<Menes-Develop>/File");
        ::gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), menu);
        AppendMenu(menu, "Save", "File", &SaveFile, NULL, GDK_s, GDK_MOD1_MASK);
        AppendMenu(menu, "Close", "File", &Compile, NULL, GDK_F4, GDK_CONTROL_MASK);
    }

    {
        GtkWidget *item(AppendMenu(menubar, "Build"));
        GtkWidget *menu(::gtk_menu_new());
        ::gtk_menu_set_accel_group(GTK_MENU(menu), accelerators);
        ::gtk_menu_set_accel_path(GTK_MENU(menu), "<Menes-Develop>/Build");
        ::gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), menu);
        AppendMenu(menu, "Compile", "Build", &Compile, NULL, GDK_b, GDK_CONTROL_MASK | GDK_SHIFT_MASK);
        AppendMenu(menu, "Debug", "Build", &Debug, NULL, GDK_F5);
    }

    {
        GtkWidget *item(AppendMenu(menubar, "Window"));
        GtkWidget *menu(::gtk_menu_new());
        ::gtk_menu_set_accel_group(GTK_MENU(menu), accelerators);
        ::gtk_menu_set_accel_path(GTK_MENU(menu), "<Menes-Develop>/Window");
        ::gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), menu);
        AppendMenu(menu, "Previous", "Window", &PreviousTab, NULL, GDK_Tab, GDK_CONTROL_MASK | GDK_SHIFT_MASK);
        AppendMenu(menu, "Next", "Window", &NextTab, NULL, GDK_Tab, GDK_CONTROL_MASK);
    }

    GtkWidget *vpane(::gtk_vpaned_new());
    ::gtk_box_pack_end(GTK_BOX(box), vpane, TRUE, TRUE, 0);

    GtkWidget *hpane(::gtk_hpaned_new());
    ::gtk_paned_add1(GTK_PANED(vpane), hpane);

    build = NewTerminal();
    ::gtk_widget_set_size_request(build, -1, 100);
    ::gtk_paned_add2(GTK_PANED(vpane), build);
    ::terminal_widget_connect_child_died(build, G_CALLBACK(BuildComplete), NULL);

    tabs = GTK_WIDGET(::gtk_notebook_new());
    ::gtk_notebook_set_tab_pos(GTK_NOTEBOOK(tabs), GTK_POS_TOP);
    ::gtk_widget_set_size_request(tabs, -1, 750);
    ::gtk_paned_add2(GTK_PANED(hpane), tabs);

    GtkWidget *scroll(::gtk_scrolled_window_new(NULL, NULL));
    ::gtk_paned_add1(GTK_PANED(hpane), scroll);
    ::gtk_widget_set_size_request(scroll, 250, -1);

    model = ::fm_tree_model_new();
    sorted = ::gtk_tree_model_sort_new_with_model(GTK_TREE_MODEL(model));
    tree = ::gtk_tree_view_new_with_model(sorted);
    ::g_signal_connect(G_OBJECT(tree), "row-activated", G_CALLBACK(SelectFile), NULL);
    ::g_object_unref(sorted);

    AddDirectory("Project Folder", project);
    AddDirectory("System Includes", "/usr/include");
    AddDirectory("Home Directory", "/home/saurik");

    ::g_object_unref(model);

    ::gtk_tree_sortable_set_default_sort_func(GTK_TREE_SORTABLE(sorted), &CompareFiles, tree, NULL);
    ::gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(tree), FALSE);
    GtkTreeViewColumn *column(::gtk_tree_view_column_new());

    {
        GtkCellRenderer *renderer(::nautilus_cell_renderer_pixbuf_emblem_new());
        ::gtk_tree_view_column_pack_start(column, renderer, FALSE);
        ::gtk_tree_view_column_set_attributes(column, renderer, "pixbuf", FM_TREE_MODEL_CLOSED_PIXBUF_COLUMN, "pixbuf_expander_closed", FM_TREE_MODEL_CLOSED_PIXBUF_COLUMN, "pixbuf_expander_open", FM_TREE_MODEL_OPEN_PIXBUF_COLUMN, "pixbuf_emblem", FM_TREE_MODEL_EMBLEM_PIXBUF_COLUMN, NULL);
    }

    {
        GtkCellRenderer *renderer(::gtk_cell_renderer_text_new());
        ::gtk_tree_view_column_pack_start(column, renderer, TRUE);
        ::gtk_tree_view_column_set_attributes(column, renderer, "text", FM_TREE_MODEL_DISPLAY_NAME_COLUMN, "style", FM_TREE_MODEL_FONT_STYLE_COLUMN, "weight", FM_TREE_MODEL_FONT_WEIGHT_COLUMN, NULL);
    }

    ::gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);
    ::gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scroll), tree);

    ::gtk_widget_show_all(GTK_WIDGET(window));
    ::gtk_window_maximize(window);

    ::gtk_main();
    return 0;
}
