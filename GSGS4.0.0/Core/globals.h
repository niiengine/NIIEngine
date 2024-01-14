/*
-----------------------------------------------------------------------------
A
     __      _   _   _   ______
    |   \   | | | | | | |  ____)                    _
    | |\ \  | | | | | | | |         ___      ___   (_)   ___
    | | \ \ | | | | | | | |____    / _ \   / ___ \  _   / _ \   ___
    | |  \ \| | | | | | |  ____)  | / \ | | |  | | | | | / \ | / _ )
    | |   \ | | | | | | | |_____  | | | | | |__| | | | | | | | | __/
    |_|    \ _| |_| |_| |_______) |_| |_|  \___| | |_| |_| |_| |___|
                                             __/ |                 
                                            \___/   
                                                
                                                
                                                                 F i l e


Copyright: NIIEngine Team Group

Home page: www.niiengine.com 

Email: niiengine@gmail.com OR niiengine@163.com

Licence: commerce(www.niiengine.com/license)(Three kinds)
------------------------------------------------------------------------------
*/
#ifndef GLOBALS_H
#define GLOBALS_H

#include "gsgsPreInclude.h"
#include "gsgsPluginCommon.h"
#include "gsgsWorkspace.h"
#include "imanager.h"
#include <wx/ctrlsub.h>
#include <wx/dcgraph.h>
#include <wx/propgrid/propgrid.h>
#include <wx/variant.h>
#include <wx/infobar.h>

class wxDataViewCtrl;
class wxStyledTextCtrl;
class IProcess;
class IProcessCallback;
class wxListCtrl;

namespace gsgs
{
    typedef void (wxObject::*EventFunc_t)(wxClientData* arg);

    class _gsgsAPI BOM
    {
        wxMemoryBuffer m_bom;

    public:
        BOM(const char* buffer, size_t len);
        BOM();
        ~BOM();

        void Clear();
        int Len() const;
        wxFontEncoding Encoding();
        static wxFontEncoding Encoding(const char* buff);
        void SetData(const char* buffer, size_t len);
        void* GetData() const { return m_bom.GetData(); }
        bool IsEmpty() const { return m_bom.IsEmpty(); }
    };

    class _gsgsAPI EventDisabler
    {
    public:
        EventDisabler();
        ~EventDisabler();
    };

    /**
     * \brief send command event to the application (wxTheApp),
     * \param eventId
     * \param clientData any user data. Must NOT be freed by the handler
     * \return same as wxTheApp->ProcessEvent()
     */
    _gsgsAPI bool SendCmdEvent(int eventId, void* clientData = NULL);

    /**
     * @brief send command event to the application (wxTheApp),
     * @param eventId
     * @param clientData any user data. Must NOT be freed by the handler
     * @param str this string will be available by the handlers by calling event.GetString()
     * @return same as wxTheApp->ProcessEvent()
     */
    _gsgsAPI bool SendCmdEvent(int eventId, void* clientData, const wxString& str);

    /**
     * \brief post command event to the application (wxTheApp),
     * \param eventId
     * \param clientData allocated data on the heap. Must be freed by the handler
     */
    _gsgsAPI void PostCmdEvent(int eventId, void* clientData = NULL);

    /**
     * \brief set column text
     * \param list the list
     * \param indx row ID
     * \param column column ID
     * \param rText the text
     * \param imgId image id
     */
    _gsgsAPI void SetColumnText(wxListCtrl* list, long indx, long column, const wxString& rText,
        int imgId = wxNOT_FOUND);

    /**
     * \brief return column's text
     * \param list the list control
     * \param index the row ID
     * \param column the column ID
     * \return the column's text
     */
    _gsgsAPI wxString GetColumnText(wxListCtrl* list, long index, long column);

    /**
     * \brief append row to list control
     * \param list the list
     * \return new row index
     */
    _gsgsAPI long AppendListCtrlRow(wxListCtrl* list);

    /**
     * \brief read file from disk using appropriate file conversion
     * \param fileName file name
     * \param content output string
     * \param encoding
     * \return true on success, false otherwise
     */
    _gsgsAPI bool ReadFileWithConversion(const wxString& fileName, wxString& content,
        wxFontEncoding encoding = wxFONTENCODING_DEFAULT, BOM* bom = NULL);

    /**
     * \brief compare a file with a wxString using md5
     * \param filePath file's full path
     * \param str a wxString, perhaps containing an editor's content
     * \return true if the current content of the file is identical to str, false otherwise
     */
    _gsgsAPI bool CompareFileWithString(const wxString& filePath, const wxString& str);

    /**
     * \brief delete directory using shell command
     * \param path directory path
     * \return true on success, false otherwise
     */
    _gsgsAPI bool RemoveDirectory(const wxString& path);

    /**
     * \brief return true of id is a valid cpp identifier
     */
    _gsgsAPI bool IsValidCppIndetifier(const wxString& id);

    /**
     * \brief return true of word is a C++ keyword
     * \param word
     * \return
     */
    _gsgsAPI bool IsCppKeyword(const wxString& word);

    /**
     * \brief return true of id is a valid cpp file
     */
    _gsgsAPI bool IsValidCppFile(const wxString& id);

    /**
     * [DEPRECATED] DONT USE THIS METHOD ANYMORE - USE IMacroManager
     * Expand variables to their real value, if expanding fails
     * the return value is same as input. The variable is expanded
     * in the project context
     */
    _gsgsAPI wxString ExpandVariables(const wxString& expression, ProjectPtr proj, IEditor* editor,
        const wxString& filename = wxEmptyString);

    /**
     * * [DEPRECATED] DONT USE THIS METHOD ANYMORE - USE IMacroManager
     * \brief accepts expression string and expand all known marcos (e.g. $(ProjectName))
     * \param expression expression
     * \param projectName project name (to be used for $(ProjectName) macro)
     * \param fileName file name, to help expand the $(CurrentFile) macro family
     * \return an expanded string. If a macro is unknown it is replaced by empty string
     */
    _gsgsAPI wxString ExpandAllVariables(const wxString& expression, CxxWS* workspace,
        const wxString& projectName, const wxString& selConf, const wxString& fileName);

    /**
     * \brief copy entire directory content (recursievly) from source to target
     * \param src source path
     * \param target target path
     * \return true on success, false otherwise
     */
    _gsgsAPI bool CopyDir(const wxString& src, const wxString& target);

    /**
     * \brief create a directory
     * \param path directory path
     */
    _gsgsAPI void Mkdir(const wxString& path);

    /**
     * \brief write file content with optinal backup
     * \param file_name
     * \param content
     * \param backup
     * \return true on success, false otherwise
     */
    _gsgsAPI bool WriteFileWithBackup(const wxString& file_name, const wxString& content, bool backup);

    /**
     * \brief copy text to the clipboard
     * \return true on success false otherwise
     */
    _gsgsAPI bool CopyToClipboard(const wxString& text);

    /**
     * \brief make colour lighter
     * \param color
     * \param level
     * \return modified colour
     */
    _gsgsAPI wxColour MakeColourLighter(wxColour color, float level);

    /**
     * @brief return true if filename is readonly false otherwise
     */
    _gsgsAPI bool IsFileReadOnly(const wxFileName& filename);

    /**
     * \brief fill an array with a semi-colon separated string
     * \param arr [out] the array to fill
     * \param str the string to split
     */
    _gsgsAPI void FillFromSmiColonString(wxArrayString& arr, const wxString& str);

    /**
     * \brief return a string semi-colon separated of the given array
     */
    _gsgsAPI wxString ArrayToSmiColonString(const wxArrayString& array);

    /**
     * \brief Remove all semi colons of the given string
     */
    _gsgsAPI void StripSemiColons(wxString& str);

    /**
     * \brief Normalize the given path (change all \ by /)
     */
    _gsgsAPI wxString NormalizePath(const wxString& path);

    /**
     * \brief Returns the file modification time in seconds after the epoch.
     */
    _gsgsAPI time_t GetFileModificationTime(const wxString& filename);
    _gsgsAPI time_t GetFileModificationTime(const wxFileName& filename);

    /**
     * @brief wrap a given command in the shell command (e.g. cmd /c "command")
     */
    _gsgsAPI void WrapInShell(wxString& cmd);

    /**
     * @brief return the current user name without any special characters
     * @return
     */
    _gsgsAPI const wxString & GetUserName();

    /**
     * @brief return list of projects available based on the installed tempaltes
     * @param list list of projects
     * @param imageMap when provided, returns the image index (set in the lstImages) mapped to the project type
     */
    _gsgsAPI void GetProjectTemplateList(std::list<ProjectPtr>& list);

    /**
     * @brief extract file from Zip and place it under targetDir
     * @param zipPath path to the Zip file (fullpath)
     * @param filename the file name to search in the archive
     * @param targetDir where to place the extracted file
     * @param targetFileName the path of the file that was actually extracted
     * @return true on success, false otherwise
     */
    _gsgsAPI bool ExtractFileFromZip(const wxString& zipPath, const wxString& filename, const wxString& targetDir,
        wxString& targetFileName);

    /**
     * @brief set the native Windows theme for the application
     * @param win [input]
     */
    _gsgsAPI void MSWSetNativeTheme(wxWindow* win, const wxString& theme = wxT("Explorer"));

    /**
     * @brief make relative only if a subpath of reference_path (or is reference_path itself)
     * @brief also, make normalise first, and abolish any symlink
     * @param fn wxFileName to alter
     * @param reference_path the path to which to make relative
     */
    _gsgsAPI bool MakeRelativeIfSensible(wxFileName& fn, const wxString& reference_path);

    /**
     * @brief joins array element into a string using 'glue' as the array elements
     * separator
     */
    _gsgsAPI wxString wxImplode(const wxArrayString& arr, const wxString& glue = wxT("\n"));

    /**
     * @brief executes a command under the proper shell and return string as the output
     */
    _gsgsAPI wxString wxShellExec(const wxString& cmd, const wxString& projectName);

    /**
     * @class StringManager
     * @brief Stores unlocalised strings for serialisation, while managing localised ones in the gui
     */
    class _gsgsAPI StringManager
    {
    protected:
        wxArrayString m_unlocalisedStringArray;
        wxControlWithItems* p_control;
        size_t m_size;

    public:
        StringManager()
            : m_size(0)
        {
        }

        /**
         * @brief Store the data, and load the strings into the control, localised
         * @param size size of the string array
         * @param strings the string array
         * @param current the option currently used
         * @param control the gui element, probably a wxChoice
         */
        void AddStrings(size_t size, const wxString* const strings, const wxString& current, wxControlWithItems* control);
        /**
         * @brief Returns the unlocalised string corresponding to the selection
         * @return an unlocalised string
         */
        wxString GetStringSelection() const;
        /**
         * @brief Sets m_control's selection to that corresponding to the unlocalised string
         * @param str an unlocalised string
         * @param dfault the default used if str is not found
         */
        void SetStringSelection(const wxString& str, size_t dfault = 0);
    };

    /**
     * @brief Prepends the wxString to the wxArrayString, first removing any matching entry
     * @param oldarray contains any entries
     * @param str the new entry
     * @param maxsize the maximum number of items allowed in the arraystring. 0 means no maximum
     * @return the amended entries
     */
    _gsgsAPI wxArrayString ReturnWithStringPrepended(const wxArrayString& oldarray, const wxString& str,
        const size_t maxsize);

    /**
     * @brief return true if filename is a symbolic link
     */
    _gsgsAPI bool wxIsFileSymlink(const wxFileName& filename);

    /**
     * @brief convert filename to the real path if filename is a symbolic link
     */
    _gsgsAPI wxFileName wxReadLink(const wxFileName& filename);

    /**
     * @brief makes-absolute filepath, and dereferences it and any symlinked dirs in the path
     */
    _gsgsAPI wxString RealPath(const wxString& filepath);

    _gsgsAPI wxString DbgPrependCharPtrCastIfNeeded(const wxString& expr, const wxString& exprType);

    /**
     * @brief create wxVariant from wxString + wxBitmap
     */
    _gsgsAPI wxVariant MakeIconText(const wxString& text, const wxBitmap& bmp);

    /**
     * @brief create wxVariant from wxString + int
     */
    _gsgsAPI wxVariant MakeBitmapIndexText(const wxString& text, int imgIndex);

    /**
     * @brief create wxVariant from label+checkbox+imgindex
     */
    _gsgsAPI wxVariant MakeCheckboxVariant(const wxString& label, bool checked, int imgIndex);

    /**
     * @brief queue a call to a function to be executed on the next event loop
     */
    _gsgsAPI void PostCall(wxObject* instance, EventFunc_t func, wxClientData* arg);
    _gsgsAPI void PostCall(wxObject* instance, EventFunc_t func);

    /**
     * @brief split lines (using CR|LF as the separator), taking into considertaion line continuation
     * @param trim trim the lines with set to true
     */
    _gsgsAPI wxArrayString SplitString(const wxString& inString, bool trim = true);

    /**
     * @brief launch terminal for debugging purposes and return its TTY. This function does nothing under Windows
     */
    _gsgsAPI void LaunchTerminalForDebugger(const wxString& title, wxString& tty, wxString& realPts, long& pid);

    /**
     * @brief prompt the user with a wxRichMessageDialog with a checkbox "Don't show this message again"
     * @param message the message to show to the user
     * @param checkboxLabel the message to display next to the checkbox
     * @param dlgId a unique string ID which will be used for storing the user value in case he checks the 'dont annoy me
     * again' checkbox
     * @param yesLabel set a different label to the "Yes" button
     * @param noLabel set a different label to the "No" button
     * @param cancelLabel set a different label to the "Cancel" button
     * @param style the wxRichMessageDialog style
     * @param checkboxInitialValue
     * @return wxRichMessageDialog::ShowModal() return value
     */
    _gsgsAPI wxStandardID PromptMessageDialog(
        const wxString& message, const wxString& dlgId, const wxString& yesLabel = _("Yes"),
        const wxString& noLabel = _("No"), const wxString& cancelLabel = _("Cancel"),
        const wxString& checkboxLabel = _("Don't ask me again"),
        long style = wxYES_NO | wxCANCEL | wxICON_QUESTION | wxYES_DEFAULT, bool checkboxInitialValue = false);

    /**
     * @brief prompt the user with a PromptMessageDialog minus the 'Cancel' button
     * @param message the message to show to the user
     * @param checkboxLabel the message to display next to the checkbox
     * @param dlgId a unique string ID which will be used for storing the user value in case he checks the 'dont annoy me
     * again' checkbox
     * @param yesLabel set a different label to the "Yes" button
     * @param noLabel set a different label to the "No" button
     * @param style the wxRichMessageDialog style
     * @param checkboxInitialValue
     * @return wxRichMessageDialog::ShowModal() return value
     */
    _gsgsAPI wxStandardID PromptMessageDialogWithoutCancel(
        const wxString& message, const wxString& dlgId, const wxString& yesLabel = _("Yes"),
        const wxString& noLabel = _("No"), const wxString& checkboxLabel = _("Remember my answer and don't ask me again"),
        long style = wxYES_NO | wxICON_QUESTION | wxYES_DEFAULT, bool checkboxInitialValue = false);

    /**
     * @brief wrap string with quotes if needed
     */
    _gsgsAPI wxString& WrapWithQuotes(wxString& str);

    /**
     * @brief wrap string with quotes if needed
     */
    _gsgsAPI wxString& EscapeSpaces(wxString& str);

    /**
     * @brief return an expression from a given position.
     * e.g. if the caret is on a line:
     * variable.m_name.m_value|
     * the | represents the cart, this function will return the entire expression: variable.m_name.m_value
     */
    _gsgsAPI wxString GetCppExpressionFromPos(long pos, wxStyledTextCtrl* ctrl, bool forCC);

    /**
     * @brief return true if running under Cygwin environment
     * This function returns false under Linux/OSX and under Windows it checks the
     * output of the command 'uname -s'
     */
    _gsgsAPI bool IsCygwinEnv();

    /**
     * @brief return true if running under MSYS environment
     * This function returns false under Linux/OSX and under Windows it checks the
     * output of the command 'uname -s'
     */
    _gsgsAPI bool IsMSYSEnv();

    /**
     * @brief change the command so it will run in the background
     */
    _gsgsAPI wxString MakeCommandRunInBackground(const wxString& cmd);

    /**
     * @brief enable use of checkbox for boolean properties
     */
    _gsgsAPI void wxPGPropertyBooleanUseCheckbox(wxPropertyGrid* grid);

    /**
     * @brief adjust the h-scroll bar of the control to fit the content
     * @param ctrl
     */
    _gsgsAPI void CalcHScrollBar(wxStyledTextCtrl* ctrl);

    _gsgsAPI wxString GetUserText(const wxString& title, const wxString& message,
        const wxString& initialValue = "", int charsToSelect = wxNOT_FOUND, wxWindow* parent = NULL);

    /**
     * @brief set the plugin manager
     * @param manager
     */
    _gsgsAPI void SetManager(IManager* manager);

    /**
     * @brief return the instance to the plugin manager. A convinience method
     */
    _gsgsAPI IManager* GetManager();

    /**
     * @brief remove terminal colours from buffer
     */
    _gsgsAPI void StripTerminalColouring(const wxString& buffer, wxString& modbuffer);

    /**
     * @brief return true if the name is valid
     */
    _gsgsAPI bool IsVaidProjectName(const wxString& name);

    /**
     * @brief return the real bitmap size for Hi-DPI screens
     */
    _gsgsAPI int GetScaledSize(int size);

    /**
     * @param signo singal number
     * @brief send signo to the
     * @param processID the process ID to kill
     * @param kill_whole_group kill the process group
     * @param as_superuser send the signal as superuser
     */
    _gsgsAPI void Kill(int processID, wxSignal signo, bool kill_whole_group = false, bool as_superuser = false);

    /**
     * @brief set the new font encoding name for the editors
     */
    _gsgsAPI void SetEditorFontEncoding(const wxString& encoding);

    /**
     * @brief locate an executable on the system using the PATH environment variable
     * @param name the exe name to locate (you can ommit the .exe on Windows, it will be added automatically)
     * @param exepath [output]
     * @param hint list of directories to search
     */
    _gsgsAPI bool FindExec(const wxString& name, wxFileName& exepath, const wxArrayString& hint = wxArrayString());

    /**
     * @brief given a menu and an item ID, return its position
     * @return return the position or wxNOT_FOUND
     */
    _gsgsAPI int FindMenuItemPosition(wxMenu* menu, int menuItemId);

    /**
     * @brief an efficient way to tokenize string into words (separated by SPACE and/or TAB)
     * @code
     * wxString str = "My String That Requires Tokenize";
     * wxString word; // The output
     * size_t offset = 0;
     * while(NextWord(str, offset, word)) {
     *      // Do something with "word" here
     * }
     * @codeend
     * @param str the string to tokenize
     * @param offset used internally, allocate one on the stack and initialise it to 0
     * @param word [output]
     * @return true if a word was found
     */
    _gsgsAPI bool NextWord(const wxString& str, size_t& offset, wxString& word);

    /**
     * @brief join strings with "\n" or "\r\n" (depends on eol)
     * eol can be wxSTC_EOL_CRLF, wxSTC_EOL_LF etc
     */
    _gsgsAPI wxString JoinLinesWithEOL(const wxArrayString& lines, int eol);

    /**
     * @brief fit the dataview columns width to match their content
     */
    _gsgsAPI void FitColumnWidth(wxDataViewCtrl* ctrl);

    /**
     * @brief return the screen size, taking dual or more screens into account
     */
    _gsgsAPI wxSize GetDisplaySize();

    /**
     * @brief returna top level window best size using its parent's size as reference
     */
    _gsgsAPI void SetTLWindowBestSizeAndPosition(wxWindow* win);

    /**
     * @brief set dialog best size and position
     * @param win
     */
    _gsgsAPI void SetDialogBestSizeAndPosition(wxDialog* win);
}
#endif // GLOBALS_H
