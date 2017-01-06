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

#include "gui1/treemodel.hpp"

namespace gui1 {

/*void TreeModel::SetTreeChild_(unsigned index, TreeModel *child) {
    InsertTreeChild_(index, child);
    EraseTreeChild_(child);
}*/

void TreeModel::InsertTreeChild_(TreeModel *child, TreeModel *before) {
    for (ViewMap_::Iterator view(views_.Begin()); view != views_.End(); ++view)
        view->First()->InsertChild(view->Second(), child, before);
}

void TreeModel::EraseTreeChild_(TreeModel *child) {
    for (ViewMap_::Iterator view(views_.Begin()); view != views_.End(); ++view)
        view->First()->EraseChild(view->Second(), child);
}

void TreeModel::UpdateTreeInfo_() {
    for (ViewMap_::Iterator view(views_.Begin()); view != views_.End(); ++view)
        view->First()->UpdateInfo(view->Second());
}

TreeModel::List TreeModel::GetTreeChildren() {
    unsigned size(GetTreeSize());
    // XXX: should use a constructor
    List children;
    children.SetSize(size);
    for (unsigned child(0); child != size; ++child)
        children[child] = GetTreeChild(child);
    return children;
}

void TreeModel::SubscribeView(TreeView *view, void *baton) const {
    views_.Insert(view, baton);
}

void TreeModel::UnsubscribeView(TreeView *view) const {
    views_.Remove(view);
}

void *TreeModel::GetViewBaton(TreeView *view) const {
    return views_.GetOr(view, NULL);
}

}
