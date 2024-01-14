#include "gsgsMultiBook.h"
#include "gsgsEditorConfig.h"
#include "gsgsLogManager.h"
#include <wx/wupdlock.h>
#include <wx/app.h>

namespace gsgs
{
    // -------------------------------------------------------------------------------
    MultiBook::MultiBook(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style,
                             const wxString& name)
        : wxPanel(parent, id, pos, size, wxTAB_TRAVERSAL, name)
        , m_style(style)
        , m_selection(wxNOT_FOUND)
        , m_defaultPage(nullptr)
    {
        m_splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3DSASH | wxSP_LIVE_UPDATE);
        SetSizer(new wxBoxSizer(wxHORIZONTAL));
        GetSizer()->Add(m_splitter, 1, wxEXPAND, 0);

        // Create the books
        m_leftBook = CreateNotebook(m_splitter);
        m_rightBook = CreateNotebook(m_splitter);

        // Hide the right book and initialise the splitter
        m_rightBook->Show(false);
        m_splitter->Initialize(m_leftBook);
        wxTheApp->Bind(wxEVT_SET_FOCUS, &MultiBook::OnFocus, this);
        m_history.reset(new TabHistory());
    }
    // -------------------------------------------------------------------------------
    MultiBook::~MultiBook() { wxTheApp->Unbind(wxEVT_SET_FOCUS, &MultiBook::OnFocus, this); }
    // -------------------------------------------------------------------------------
    //-------------------------------------
    // Helper functions
    //-------------------------------------
    bool MultiBook::GetActiveBook(Notebook** book, size_t& bookIndex) const
    {
        if(m_selection == wxNOT_FOUND) return false;
        size_t modPageIndex;
        return GetBookByPageIndex(m_selection, book, bookIndex, modPageIndex);
    }
    // -------------------------------------------------------------------------------
    bool MultiBook::GetBookByPageIndex(size_t pageIndex, Notebook** book, size_t& bookIndex, size_t& modPageIndex) const
    {
        std::vector<Notebook*> books = { m_leftBook, m_rightBook };
        for(size_t i = 0; i < books.size(); ++i) {
            Notebook* b = books[i];
            if(pageIndex < b->GetPageCount()) {
                *book = b;
                bookIndex = i;
                modPageIndex = pageIndex;
                return b;
            }
            pageIndex -= b->GetPageCount();
        }
        return false;
    }
    // -------------------------------------------------------------------------------
    bool MultiBook::GetActivePageBook(Notebook** book, size_t& bookIndex, size_t& modPageIndex) const
    {
        return GetBookByPageIndex(m_selection, book, bookIndex, modPageIndex);
    }
    // -------------------------------------------------------------------------------
    void MultiBook::MovePageToNotebook(Notebook* srcbook, size_t index, Notebook* destbook)
    {
        if(!srcbook || !destbook) return;

        bool updateViewNeeded = (destbook->GetPageCount() == 0);
        wxString text = srcbook->GetPageText(index);
        wxBitmap bmp = srcbook->GetPageBitmap(index);
        wxWindow* page = srcbook->GetPage(index);

        srcbook->RemovePage(index, false);
        destbook->AddPage(page, text, true, bmp);

        // Make the newly added tab the focused one
        page->CallAfter(&wxWindow::SetFocus);

        // If we add the first page to the destination notebook or removing the last page from the source notebook
        // a view update is required
        if(updateViewNeeded || (srcbook->GetPageCount() == 0)) { CallAfter(&MultiBook::UpdateView); }
    }
    // -------------------------------------------------------------------------------
    void MultiBook::UpdateView()
    {
        if(m_rightBook->GetPageCount() == 0) {
            if(m_splitter->IsSplit()) { m_splitter->Unsplit(); }
        } else if(!m_splitter->IsSplit()) {
            m_splitter->SplitVertically(m_leftBook, m_rightBook);
        }
    }
    // -------------------------------------------------------------------------------
    int MultiBook::BookIndexToGlobalIndex(size_t bookIndex, size_t pageIndex) const
    {
        // Sanity
        std::vector<Notebook*> books = { m_leftBook, m_rightBook };
        if(bookIndex >= books.size()) { return wxNOT_FOUND; }

        int globalIndex = pageIndex;
        for(size_t i = 0; i < bookIndex; ++i) {
            globalIndex += books[i]->GetPageCount();
        }
        return globalIndex;
    }
    // -------------------------------------------------------------------------------
    int MultiBook::BookIndexToGlobalIndex(Notebook* book, size_t pageIndex) const
    {
        bool found = false;
        int globalIndex = pageIndex;

        std::vector<Notebook*> books = { m_leftBook, m_rightBook };
        for(size_t i = 0; i < books.size(); ++i) {
            if(book == books[i]) {
                found = true;
                break;
            } else {
                globalIndex += books[i]->GetPageCount();
            }
        }

        // Sanity
        if(!found) { return wxNOT_FOUND; }
        return globalIndex;
    }
    // -------------------------------------------------------------------------------
    //-----------------------------------------
    // API
    //-----------------------------------------
    bool MultiBook::CanMoveToTabGroupLeft() const
    {
        Notebook* book;
        size_t bookIndex;
        size_t modPageIndex;
        if(!GetActivePageBook(&book, bookIndex, modPageIndex)) { return false; }
        return (book == m_rightBook); // We are in the right notebook
    }
    // -------------------------------------------------------------------------------
    bool MultiBook::CanMoveToTabGroupRight() const
    {
        Notebook* book;
        size_t bookIndex;
        size_t modPageIndex;
        if(!GetActivePageBook(&book, bookIndex, modPageIndex)) { return false; };
        return (book == m_leftBook) && (m_leftBook->GetPageCount() > 1);
    }
    // -------------------------------------------------------------------------------
    void MultiBook::MoveToRightTabGroup()
    {
        if(!CanMoveToTabGroupRight()) { return; }
        MovePageToNotebook(m_leftBook, m_leftBook->GetSelection(), m_rightBook);
    }
    // -------------------------------------------------------------------------------
    void MultiBook::MoveToLeftTabGroup()
    {
        if(!CanMoveToTabGroupLeft()) { return; }
        MovePageToNotebook(m_rightBook, m_rightBook->GetSelection(), m_leftBook);
    }
    // -------------------------------------------------------------------------------
    void MultiBook::AddPage(wxWindow* page, const wxString& label, bool selected, const wxBitmap& bmp)
    {
        // Hide the default page if needed
        wxWindowUpdateLocker locker(this);
        ShowNotebook();
        m_leftBook->AddPage(page, label, selected, bmp);
        m_history->Push(page);
    }
    // -------------------------------------------------------------------------------
    bool MultiBook::InsertPage(size_t index, wxWindow* page, const wxString& label, bool selected, const wxBitmap& bmp)
    {
        Notebook* b;
        size_t modindex;
        size_t bookindex;
        wxWindowUpdateLocker locker(this);
        ShowNotebook();
        if(GetBookByPageIndex(index, &b, bookindex, modindex)) {
            DoShowWindow(m_defaultPage, false);
            DoShowWindow(m_splitter, true);
            bool res = b->InsertPage(modindex, page, label, selected, bmp);
            if(res) { m_history->Push(page); }
            return res;
        } else {
            AddPage(page, label, selected, bmp);
            return true;
        }
    }
    // -------------------------------------------------------------------------------
    wxWindow* MultiBook::GetPage(size_t index) const
    {
        Notebook* book;
        size_t modIndex;
        size_t bookIndex;
        if(!GetBookByPageIndex(index, &book, bookIndex, modIndex)) { return nullptr; }
        return book->GetPage(modIndex);
    }
    // -------------------------------------------------------------------------------
    bool MultiBook::DeletePage(size_t page, bool notify)
    {
        Notebook* book;
        size_t modIndex;
        size_t bookIndex;
        if(!GetBookByPageIndex(page, &book, bookIndex, modIndex)) { return false; }

        // Update the history
        wxWindow* pageToDelete = book->GetPage(modIndex);
        m_history->Pop(pageToDelete);

        // Delete the page
        bool res = book->DeletePage(modIndex, notify);
        UpdateView();
        return res;
    }
    // -------------------------------------------------------------------------------
    wxWindow* MultiBook::GetCurrentPage() const
    {
        Notebook* book;
        size_t bookIndex;
        if(GetActiveBook(&book, bookIndex)) { return book->GetCurrentPage(); }
        return nullptr;
    }
    // -------------------------------------------------------------------------------
    size_t MultiBook::GetPageCount() const { return m_leftBook->GetPageCount() + m_rightBook->GetPageCount(); }
    // -------------------------------------------------------------------------------
    int MultiBook::GetSelection() const { return m_selection; }
    // -------------------------------------------------------------------------------
    size_t MultiBook::GetAllTabs(TabInfoVec_t& tabs)
    {
        tabs.clear();
        TabInfoVec_t all_tabs;
        std::vector<Notebook*> books = { m_leftBook, m_rightBook };
        std::for_each(books.begin(), books.end(), [&](Notebook* b) {
            TabInfoVec_t t;
            b->GetAllTabs(t);
            all_tabs.insert(all_tabs.end(), t.begin(), t.end());
        });
        tabs.swap(all_tabs);
        return tabs.size();
    }
    // -------------------------------------------------------------------------------
    bool MultiBook::SetPageToolTip(size_t page, const wxString& tooltip)
    {
        Notebook* book;
        size_t bookIndex;
        size_t modIndex;
        if(GetBookByPageIndex(page, &book, bookIndex, modIndex)) { return book->SetPageToolTip(modIndex, tooltip); }
        return false;
    }
    // -------------------------------------------------------------------------------
    int MultiBook::SetSelection(size_t tabIdx)
    {
        Notebook* book;
        size_t bookIndex;
        size_t modIndex;
        if(GetBookByPageIndex(tabIdx, &book, bookIndex, modIndex)) {
            // Update the current selection
            bool pageChanged = (m_selection != (int)tabIdx);

            // Grab a pointer of the page we want to make active
            wxWindow* focusedPage = book->GetPage(modIndex);

            // And perform the actual selection change
            int oldSelection = book->GetSelection();
            if(oldSelection != (int)modIndex) {
                m_selection = tabIdx;
                m_history->Pop(focusedPage);
                m_history->Push(focusedPage);
                return book->SetSelection(modIndex);
            } else {
                // There is no point on calling Notebook::SetSelection since it is already selected
                // However, in the term of 'multi book' control, we might need to generate the events ourselves here
                // But only if we actually modified the selection
                if(pageChanged) {
                    wxBookCtrlEvent changingEvent(wxEVT_BOOK_PAGE_CHANGING);
                    changingEvent.SetEventObject(this);
                    changingEvent.SetOldSelection(m_selection);
                    changingEvent.SetSelection(tabIdx);
                    GetEventHandler()->ProcessEvent(changingEvent);
                    if(!changingEvent.IsAllowed()) { return wxNOT_FOUND; } // User vetoed

                    // Update the history
                    m_history->Pop(focusedPage);
                    m_history->Push(focusedPage);

                    // Update the selection before we fire the event again
                    // Or we might end up with stackoverflow...
                    m_selection = tabIdx;
                    wxBookCtrlEvent changedEvent(wxEVT_BOOK_PAGE_CHANGED);
                    changedEvent.SetEventObject(this);
                    changedEvent.SetOldSelection(m_selection);
                    changedEvent.SetSelection(tabIdx);
                    GetEventHandler()->ProcessEvent(changedEvent);
                }
            }
        }
        return wxNOT_FOUND;
    }
    // -------------------------------------------------------------------------------
    bool MultiBook::SetPageText(size_t page, const wxString& text)
    {
        Notebook* book;
        size_t bookIndex;
        size_t modIndex;
        if(GetBookByPageIndex(page, &book, bookIndex, modIndex)) { return book->SetPageText(modIndex, text); }
        return false;
    }
    // -------------------------------------------------------------------------------
    wxString MultiBook::GetPageText(size_t page) const
    {
        Notebook* book;
        size_t bookIndex;
        size_t modIndex;
        if(GetBookByPageIndex(page, &book, bookIndex, modIndex)) { return book->GetPageText(modIndex); }
        return wxEmptyString;
    }
    // -------------------------------------------------------------------------------
    bool MultiBook::DeleteAllPages()
    {
        wxWindowUpdateLocker locker(this);
        std::vector<Notebook*> books = { m_leftBook, m_rightBook };
        std::for_each(books.begin(), books.end(), [&](Notebook* book) { book->DeleteAllPages(); });
        // Update the history
        m_history->Clear();
        UpdateView();
        return true;
    }
    // -------------------------------------------------------------------------------
    void MultiBook::SetStyle(size_t style)
    {
        m_style = style;
        std::vector<Notebook*> books = { m_leftBook, m_rightBook };
        std::for_each(books.begin(), books.end(), [&](Notebook* book) { book->SetStyle(m_style); });
    }
    // -------------------------------------------------------------------------------
    size_t MultiBook::GetStyle() const { return m_style; }
    // -------------------------------------------------------------------------------
    int MultiBook::GetPageIndex(wxWindow* window) const
    {
        std::vector<Notebook*> books = { m_leftBook, m_rightBook };
        for(size_t i = 0; i < books.size(); ++i) {
            int index = books[i]->GetPageIndex(window);
            if(index != wxNOT_FOUND) { return BookIndexToGlobalIndex(i, index); }
        }
        return wxNOT_FOUND;
    }
    // -------------------------------------------------------------------------------
    int MultiBook::GetPageIndex(const wxString& label) const
    {
        std::vector<Notebook*> books = { m_leftBook, m_rightBook };
        for(size_t i = 0; i < books.size(); ++i) {
            int index = books[i]->GetPageIndex(label);
            if(index != wxNOT_FOUND) { return BookIndexToGlobalIndex(i, index); }
        }
        return wxNOT_FOUND;
    }
    // -------------------------------------------------------------------------------
    void MultiBook::SetPageBitmap(size_t index, const wxBitmap& bmp)
    {
        Notebook* book;
        size_t bookIndex;
        size_t modIndex;
        if(GetBookByPageIndex(index, &book, bookIndex, modIndex)) { book->SetPageBitmap(modIndex, bmp); }
    }
    // -------------------------------------------------------------------------------
    bool MultiBook::RemovePage(size_t page, bool notify)
    {
        Notebook* book;
        size_t bookIndex;
        size_t modIndex;
        if(GetBookByPageIndex(page, &book, bookIndex, modIndex)) {
            // Make sure that after we remove the page from its notebook
            // it has a valid parent (UpdateView() below might destory its parent
            // Notebook control)
            wxWindow* removedPage = book->GetPage(modIndex);

            // Update the history
            m_history->Pop(removedPage);

            removedPage->Reparent(this);
            bool res = book->RemovePage(modIndex, notify);
            UpdateView();
            return res;
        }
        return false;
    }
    // -------------------------------------------------------------------------------
    bool MultiBook::MoveActivePage(int newIndex)
    {
        // FIXME: implement this
        return false;
    }
    // -------------------------------------------------------------------------------
    Notebook* MultiBook::CreateNotebook(wxWindow* parent)
    {
        Notebook* book = new Notebook(parent, wxID_ANY, wxDefaultSize, wxDefaultPosition, wxDefaultSize, m_style);
        book->SetStyle(m_style);
        book->Bind(wxEVT_BOOK_PAGE_CLOSING, &MultiBook::OnEventProxy, this);
        book->Bind(wxEVT_BOOK_PAGE_CLOSED, &MultiBook::OnEventProxy, this);
        book->Bind(wxEVT_BOOK_PAGE_CHANGED, &MultiBook::OnEventProxy, this);
        book->Bind(wxEVT_BOOK_PAGE_CHANGING, &MultiBook::OnEventProxy, this);
        book->Bind(wxEVT_BOOK_PAGE_CLOSE_BUTTON, &MultiBook::OnEventProxy, this);
        book->Bind(wxEVT_BOOK_TABAREA_DCLICKED, &MultiBook::OnEventProxy, this);
        book->Bind(wxEVT_BOOK_TAB_DCLICKED, &MultiBook::OnEventProxy, this);
        book->Bind(wxEVT_BOOK_TAB_CONTEXT_MENU, &MultiBook::OnEventProxy, this);
        return book;
    }
    // -------------------------------------------------------------------------------
    void MultiBook::OnEventProxy(wxBookCtrlEvent& event)
    {
        if(event.GetEventObject() == this) {
            // Avoid loops
            event.Skip();
            return;
        }
        int selection = event.GetSelection();
        int oldSelection = event.GetOldSelection();

        Notebook* book = dynamic_cast<Notebook*>(event.GetEventObject());
        if(!book) {
            gsgsWARNING() << "MultiBook::OnEventProxy no notebook event object!";
            return;
        }

        // Convert the event
        wxBookCtrlEvent proxyEvent(event.GetEventType());
        proxyEvent.SetEventObject(this);
        proxyEvent.SetSelection(wxNOT_FOUND);
        proxyEvent.SetOldSelection(wxNOT_FOUND);
        if(selection != wxNOT_FOUND) { proxyEvent.SetSelection(BookIndexToGlobalIndex(book, selection)); }
        if(oldSelection != wxNOT_FOUND) { proxyEvent.SetOldSelection(BookIndexToGlobalIndex(book, oldSelection)); }

        // Process the event
        if((event.GetEventType() == wxEVT_BOOK_TAB_CONTEXT_MENU) || (event.GetEventType() == wxEVT_BOOK_PAGE_CHANGED)) {
            // Use ProcessEvent
            GetEventHandler()->ProcessEvent(proxyEvent);
        } else if((event.GetEventType() == wxEVT_BOOK_PAGE_CLOSING) || (event.GetEventType() == wxEVT_BOOK_PAGE_CHANGING)) {
            // Handle with ProcessEvent with Veto option
            GetEventHandler()->ProcessEvent(proxyEvent);
            if(!proxyEvent.IsAllowed()) { event.Veto(); }
        } else {
            // Handle with AddPendingEvent
            GetEventHandler()->AddPendingEvent(proxyEvent);
            if(event.GetEventType() == wxEVT_BOOK_PAGE_CLOSED) {
                // A page was closed
                CallAfter(&MultiBook::UpdateView);
            }
        }
    }
    // -------------------------------------------------------------------------------
    void MultiBook::OnFocus(wxFocusEvent& e)
    {
        e.Skip();
        wxWindow* focusedWindow = wxWindow::FindFocus();
        CHECK_PTR_RET(focusedWindow);

        wxWindow* parent = focusedWindow->GetParent();
        while(parent) {
            Notebook* book = dynamic_cast<Notebook*>(parent);
            if(book && IsOurNotebook(book)) {
                // This book is one of ours...
                int index = book->GetSelection();
                if(index != wxNOT_FOUND) {
                    int oldSelection = m_selection;
                    m_selection = BookIndexToGlobalIndex(book, index);
                    if((m_selection != wxNOT_FOUND) && (m_selection != oldSelection)) {
                        // Selection has changed, notify about this
                        wxBookCtrlEvent event(wxEVT_BOOK_PAGE_CHANGED);
                        event.SetEventObject(this);
                        event.SetSelection(m_selection);
                        event.SetOldSelection(oldSelection);
                        GetEventHandler()->ProcessEvent(event);
                    }
                }
                break;
            }
            parent = parent->GetParent();
        }
    }
    // -------------------------------------------------------------------------------
    bool MultiBook::IsOurNotebook(Notebook* book) const
    {
        std::vector<Notebook*> books = { m_leftBook, m_rightBook };
        for(size_t i = 0; i < books.size(); ++i) {
            if(book == books[i]) { return true; }
        }
        return false;
    }
    // -------------------------------------------------------------------------------
    wxBitmap MultiBook::GetPageBitmap(size_t page) const
    {
        Notebook* book;
        size_t bookIndex;
        size_t modIndex;
        if(GetBookByPageIndex(page, &book, bookIndex, modIndex)) { return book->GetPageBitmap(modIndex); }
        return wxNullBitmap;
    }
    // -------------------------------------------------------------------------------
    void MultiBook::SetDefaultPage(wxWindow* page)
    {
        bool needToShow = false;
        if(page) { page->Reparent(this); }
        // if we already have a default page, remove it
        if(m_defaultPage && GetSizer()->GetItem(m_defaultPage)) {
            // Detach it from the main sizer
            DoShowWindow(m_defaultPage, false);
            needToShow = true;
        }
        m_defaultPage = page;
        if(needToShow) { DoShowWindow(m_defaultPage, true); }
    }
    // -------------------------------------------------------------------------------
    void MultiBook::DoShowWindow(wxWindow* win, bool show)
    {
        if(!win) return;
        if(show) {
            if(GetSizer()->GetItem(win)) return; // already visible
            GetSizer()->Add(win, 1, wxEXPAND);
            win->Show();
        } else {
            if(!GetSizer()->GetItem(win)) return;
            GetSizer()->Detach(win);
            win->Hide();
        }
    }
    // -------------------------------------------------------------------------------
    void MultiBook::ShowNotebook()
    {
        DoShowWindow(m_defaultPage, false);
        DoShowWindow(m_splitter, true);
    }
    // -------------------------------------------------------------------------------
    void MultiBook::ShowDefaultPage(bool show)
    {
        // Avoid unneeded flickers
        wxWindowUpdateLocker locker(this);
        DoShowWindow(m_splitter, !show);
        DoShowWindow(m_defaultPage, show);
    }
    // -------------------------------------------------------------------------------
}