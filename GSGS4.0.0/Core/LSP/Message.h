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

#ifndef MESSAGE_H
#define MESSAGE_H

#include "gsgsPreInclude.h"
#include "JSONObject.h"
#include "gsgsJSON.h"
#include <string>

namespace LSP
{
class _gsgsAPI Message : public LSP::Serializable
{
    wxString m_jsonrpc = "2.0";

public:
    static int GetNextID();

public:
    Message();
    virtual ~Message();
    virtual JSONObject ToJSON(const wxString& name) const;
    virtual void FromJSON(const JSONObject& json);

    /**
     * @brief serialize this message into string
     */
    virtual std::string ToString() const = 0;

    template <typename T> T* As() const { return dynamic_cast<T*>(const_cast<Message*>(this)); }
};

}; // namespace LSP

#endif // MESSAGE_H
