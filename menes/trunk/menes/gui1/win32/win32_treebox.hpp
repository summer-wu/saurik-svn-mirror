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

#ifndef MENES_GUI1_WIN32_TREEBOX
#define MENES_GUI1_WIN32_TREEBOX

#include "gui1/treebox.hpp"
#include "gui1/treemodel.hpp"

#include "gui1/win32/win32_widget.hpp"

#include <commctrl.h>

namespace gui1 {
namespace Win32 {

class TreeBoxImpl :
    public WidgetImpl,
    virtual public ITreeBoxImpl,
    public TreeView
{
  private:
    struct TreeBoxData {
        TreeModel *model_;
        bool expanded_;

        explicit TreeBoxData(TreeModel *model) :
            model_(model),
            expanded_(false)
        {
        }
    };

  private:
    HTREEITEM root_;
    mutable TreeBoxData data_;

    void InsertChild_(HTREEITEM parent, HTREEITEM after, TreeModel *model);
    void EraseChild_(HTREEITEM hitem);

    TreeBoxData *GetData(HTREEITEM hitem) const;
    TreeModel *GetModel(HTREEITEM hitem) const;

    HTREEITEM GetItem(TreeModel *model);

    void BuildItem(HTREEITEM hitem, TreeModel *model = NULL);

  protected:
    virtual bool OnOCM_NOTIFY(WPARAM wParam, LPARAM lParam, LRESULT &lResult);

  public:
    explicit TreeBoxImpl(Object &front);

    virtual void Destroy();

    virtual void InsertChild(void *baton, TreeModel *child, TreeModel *before = NULL);
    virtual void EraseChild(void *baton, TreeModel *child);
    virtual void UpdateInfo(void *baton);

    virtual void BindModel(TreeModel *model);

    virtual TreeModel *GetSelected() const;
    virtual void SetSelected(TreeModel *model);
};

} }

#endif//MENES_GUI1_WIN32_TREEBOX
