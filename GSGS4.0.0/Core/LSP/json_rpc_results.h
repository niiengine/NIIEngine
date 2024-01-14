#ifndef JSONRPC_RESULTS_H
#define JSONRPC_RESULTS_H

#include "gsgsPreInclude.h"
#include "gsgsJSON.h"
#include "LSP/JSONObject.h"
#include <wx/sharedptr.h>
#include "basic_types.h"

namespace LSP
{
//===----------------------------------------------------------------------------------
// Result
//===----------------------------------------------------------------------------------
class _gsgsAPI Result : public Serializable
{
public:
    typedef wxSharedPtr<Result> Ptr_t;

public:
    Result() {}
    virtual ~Result() {}
    template <typename T> T* As() const { return dynamic_cast<T*>(const_cast<Result*>(this)); }
    virtual JSONObject ToJSON(const wxString& name) const { return JSONObject(nullptr); }
};

//===----------------------------------------------------------------------------------
// ResultString
//===----------------------------------------------------------------------------------
class _gsgsAPI ResultString : public Result
{
    wxString m_text;

public:
    ResultString(const wxString& text) {}
    virtual ~ResultString() {}
    void FromJSON(const JSONObject& json);
    ResultString& SetText(const wxString& text)
    {
        this->m_text = text;
        return *this;
    }
    const wxString& GetText() const { return m_text; }
};

//===----------------------------------------------------------------------------------
// ResultNumber
//===----------------------------------------------------------------------------------
class _gsgsAPI ResultNumber : public Result
{
    int m_number = 0;

public:
    ResultNumber(const wxString& text) {}
    virtual ~ResultNumber() {}
    void FromJSON(const JSONObject& json);
    ResultNumber& SetNumber(int number)
    {
        this->m_number = number;
        return *this;
    }
    int GetNumber() const { return m_number; }
};

//===----------------------------------------------------------------------------------
// ResultNumber
//===----------------------------------------------------------------------------------
class _gsgsAPI ResultBoolean : public Result
{
    bool m_value = false;

public:
    ResultBoolean(const wxString& text) {}
    virtual ~ResultBoolean() {}
    void FromJSON(const JSONObject& json);

    ResultBoolean& SetValue(bool value)
    {
        this->m_value = value;
        return *this;
    }
    bool GetValue() const { return m_value; }
};
}; // namespace LSP

#endif // JSONRPC_RESULTS_H
