/* Menes GUI - C++ Sensible Abstract GUI Toolkit
 * Copyright (C) 2002-2003  Jay Freeman (saurik)
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

#include "gui1/win32/win32_treebox.hpp"

#include "api/win32/error.hpp"
#include "cse/utf16.hpp"

// XXX: Fucking A-Hole!
#define SendMessage SendMessageW

namespace gui1 {
namespace Win32 {

void TreeBoxImpl::InsertChild_(HTREEITEM parent, HTREEITEM after, TreeModel *model) {
    TVINSERTSTRUCT insert;
    insert.hParent = parent;
    insert.hInsertAfter = after;
    insert.item.mask = TVIF_TEXT | TVIF_CHILDREN | TVIF_PARAM;
    insert.item.pszText = LPSTR_TEXTCALLBACK;
    insert.item.cChildren = I_CHILDRENCALLBACK; //(model->GetTreeSize() == 0 ? FALSE : TRUE);
    insert.item.lParam = reinterpret_cast<LPARAM>(new TreeBoxData(model));

    HTREEITEM child(TreeView_InsertItem(handle_, &insert));
    if (child == NULL)
        throw api::Win32::Error();

    model->SubscribeView(this, child);
}

void TreeBoxImpl::EraseChild_(HTREEITEM hitem) {
    HTREEITEM last(hitem);

    for (;;) {
        TreeBoxData *data = GetData(last);
        data->model_->UnsubscribeView(this);
        /* XXX: bite me
        if (last != TVI_ROOT)
            delete data;*/

        HTREEITEM next = TreeView_GetChild(handle_, last);
        if (next == NULL) {
            while (last != hitem && (last == TVI_ROOT || NULL == (next = TreeView_GetNextSibling(handle_, last)))) {
                if (last != TVI_ROOT) {
                    last = TreeView_GetParent(handle_, last);
                    if (last == NULL)
                        last = TVI_ROOT;
                }
            }

            if (last == hitem) {
                if (FALSE == TreeView_DeleteItem(handle_, hitem))
                    throw api::Win32::Error();
                return;
            }
        }

        last = next;
    }
}

TreeBoxImpl::TreeBoxData *TreeBoxImpl::GetData(HTREEITEM hitem) const {
    if (hitem == TVI_ROOT)
        return &data_;

    TVITEMEX item;
    item.mask = TVIF_PARAM;
    item.hItem = hitem;

    if (FALSE == TreeView_GetItem(handle_, &item))
        throw api::Win32::Error();

    if ((item.mask & TVIF_PARAM) != 0 && item.lParam != 0)
        return reinterpret_cast<TreeBoxData *>(item.lParam);
    return NULL;
}

TreeModel *TreeBoxImpl::GetModel(HTREEITEM hitem) const {
    TreeBoxData *data = GetData(hitem);
    return data == NULL ? NULL : data->model_;
}

HTREEITEM TreeBoxImpl::GetItem(TreeModel *model) {
    HTREEITEM hitem = reinterpret_cast<HTREEITEM>(model->GetViewBaton(this));
    if (hitem != NULL)
        return hitem;

    TreeModel *parent = model->GetTreeParent();
    if (parent == NULL)
        // XXX: throw exception?
        return NULL;

    BuildItem(GetItem(parent), parent);
    return reinterpret_cast<HTREEITEM>(model->GetViewBaton(this));
}

void TreeBoxImpl::BuildItem(HTREEITEM hitem, TreeModel *model) {
    if (model == NULL)
        model = GetModel(hitem);

    TreeModel::List children = model->GetTreeChildren();
    for (TreeModel::List::Iterator child(children.Begin()); child != children.End(); ++child)
        InsertChild_(hitem, TVI_LAST, *child);

    GetData(hitem)->expanded_ = true;
}

bool TreeBoxImpl::OnOCM_NOTIFY(WPARAM wParam, LPARAM lParam, LRESULT &lResult) {
    LPNMHDR header = reinterpret_cast<LPNMHDR>(lParam);
    switch (header->code) {
        case TVN_GETDISPINFOW: {
            LPNMTVDISPINFOW tvhdr = reinterpret_cast<LPNMTVDISPINFOW>(lParam);
            TreeModel *model = reinterpret_cast<TreeBoxData *>(tvhdr->item.lParam)->model_;

            if ((tvhdr->item.mask & TVIF_TEXT) != 0) {
                static wchar_t buff[102400];
                _snwprintf(buff, 102400, L"%s", cse::Utf16String(model->GetTreeText()).NullTerminate());
                tvhdr->item.pszText = buff;
            }

            if ((tvhdr->item.mask & TVIF_CHILDREN) != 0)
                tvhdr->item.cChildren = (model->GetTreeSize() == 0 ? FALSE : TRUE);
        } break;

        case TVN_ITEMEXPANDINGW: {
            LPNMTREEVIEWW tvhdr = reinterpret_cast<LPNMTREEVIEWW>(lParam);
            if (tvhdr->action == TVE_EXPAND) {
                TreeBoxData *data = reinterpret_cast<TreeBoxData *>(tvhdr->itemNew.lParam);
                if (!data->expanded_) {
                    BuildItem(tvhdr->itemNew.hItem, data->model_);
                    data->expanded_ = true;
                }
            }
        } break;

        case TVN_SELCHANGEDW: {
            LPNMTREEVIEWW tvhdr = reinterpret_cast<LPNMTREEVIEWW>(lParam);
            TreeModel *model = reinterpret_cast<TreeBoxData *>(tvhdr->itemNew.lParam)->model_;
            static_cast<TreeBox &>(front_).OnTreeBoxSelect(model);
        } break;
    }

    return WidgetImpl::OnOCM_NOTIFY(wParam, lParam, lResult);
}

TreeBoxImpl::TreeBoxImpl(Object &front) :
    WidgetImpl(front, WC_TREEVIEWW, true, WS_VISIBLE | WS_CHILD | WS_BORDER | TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT),
    root_(TVI_ROOT),
    data_(NULL)
{
}

void TreeBoxImpl::Destroy() {
    if (handle_ != NULL)
        BindModel(NULL);
    return WidgetImpl::Destroy();
}

void TreeBoxImpl::InsertChild(void *baton, TreeModel *child, TreeModel *before) {
    HTREEITEM hitem(reinterpret_cast<HTREEITEM>(baton));
    TreeBoxData *data = GetData(hitem);

    if (data->expanded_) {
        HTREEITEM hafter;
        if (before == NULL)
            hafter = TVI_LAST;
        else {
            hafter = TVI_FIRST;

            for (HTREEITEM hbefore = TreeView_GetChild(handle_, hitem);
                    hbefore != NULL;
                    hbefore = TreeView_GetChild(handle_, hafter = hbefore)
            )
                if (GetModel(hbefore) == before)
                    break;
        }

        InsertChild_(hitem, hafter, child);
    }

    if (FALSE == ::InvalidateRect(handle_, NULL, TRUE))
        throw api::Win32::Error();
    /*if (FALSE == ::UpdateWindow(handle_))
        throw api::Win32::Error();*/
}

void TreeBoxImpl::EraseChild(void *baton, TreeModel *child) {
    HTREEITEM hitem(reinterpret_cast<HTREEITEM>(baton));

    hitem = TreeView_GetChild(handle_, hitem);
    while (GetModel(hitem) != child)
        hitem = TreeView_GetNextSibling(handle_, hitem);

    EraseChild_(hitem);
}

void TreeBoxImpl::UpdateInfo(void *baton) {
    HTREEITEM hitem(reinterpret_cast<HTREEITEM>(baton));
    if (FALSE == ::InvalidateRect(handle_, NULL, TRUE))
        throw api::Win32::Error();
}

void TreeBoxImpl::BindModel(TreeModel *model) {
    /*TreeModel *before = GetModel(root_);
    if (before != NULL) {
        if (before == model)
            return;
        before->Unsubscribe(this, root_);
    }*/

    if (data_.model_ != NULL)
        EraseChild_(TVI_ROOT);

    data_ = TreeBoxData(model);

    if (model != NULL) {
        model->SubscribeView(this, root_);
        BuildItem(root_, model);
    }
}

TreeModel *TreeBoxImpl::GetSelected() const {
    HTREEITEM hitem = TreeView_GetSelection(handle_);
    return hitem == NULL ? NULL : GetModel(hitem);
}

void TreeBoxImpl::SetSelected(TreeModel *model) {
    HTREEITEM hitem(model == NULL ? NULL : GetItem(model));
    if (FALSE == TreeView_SelectItem(handle_, hitem))
        throw api::Win32::Error();
}

} }
