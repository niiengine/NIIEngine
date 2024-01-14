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
#ifndef gsgsJSON_H
#define gsgsJSON_H

#include "gsgsPreInclude.h"
#include "cJSON.h"
#include <wx/variant.h>
#include <wx/gdicmn.h>
#if wxUSE_GUI
#include <wx/arrstr.h>
#include <wx/colour.h>
#include <wx/font.h>
#endif

namespace gsgs
{
    class JSON;
    class _gsgsAPI JSONObject
    {
    public:
        JSONObject(cJSON * json);
        JSONObject(const wxString & name, const wxVariant & val, int type);
        virtual ~JSONObject() {}

        inline void setName(const wxString& name)       { _name = name; }
        inline const wxString & getName() const         { return _name; }

        inline void setType(int type)                   { _type = type; }
        inline int getType() const                      { return _type; }

        inline const wxVariant & getValue() const       { return _value; }
        inline void setValue(const wxVariant & value)   { _value = value; }
       
        JSONObject getObject(const wxString & name) const;
        bool isObjectExist(const wxString & name) const;
        JSONObject getArray(int pos) const; 
        int getArraySize() const;

        bool getValue(bool & out, bool _default = false) const;
        bool getValue(int & out, int _default = -1) const;
        bool getValue(size_t & out, size_t _default = 0) const;
        bool getValue(long & out, long _default = 0) const;
        bool getValue(double & out, double _default = -1.0) const;
        bool getValue(wxString & out, const wxString & _default = wxEmptyString) const;
        bool getValue(wxArrayString & out, const wxArrayString& _default = wxArrayString(), bool clearout = true) const;
    #if wxUSE_GUI
        bool getValue(wxColour & out, const wxColour& _default = wxNullColour) const;
        bool getValue(wxFont & out, const wxFont& _default = wxNullFont) const;
        bool getValue(wxSize & out) const;
        bool getValue(wxPoint & out) const;
    #endif

        bool isNull() const;
        bool isBool() const;
        bool isString() const;
        bool isNumber() const;
        bool isArray() const;

        void addProperty(JSON * obj);

        void addProperty(const JSONObject & obj);

        JSONObject& addProperty(const wxString& name, const wxString& in);
        JSONObject& addProperty(const wxString& name, const wxChar* in);
        JSONObject& addProperty(const wxString& name, int in) { return addProperty(name, (long)in); }
        JSONObject& addProperty(const wxString& name, long in);
        JSONObject& addProperty(const wxString& name, size_t in);
        JSONObject& addProperty(const wxString& name, bool in);
        JSONObject& addProperty(const wxString& name, const wxArrayString& in);
        JSONObject& addProperty(const wxString& name, const gsgsStringMap& in);
        JSONObject& addProperty(const wxString& name, const JSONObject& in);
        JSONObject& addProperty(const wxString& name, const char* in, const wxMBConv& conv = wxConvUTF8);
    #if wxUSE_GUI
        JSONObject& addProperty(const wxString& name, const wxSize& in);
        JSONObject& addProperty(const wxString& name, const wxPoint& in);
        JSONObject& addProperty(const wxString& name, const wxColour& in);
        JSONObject& addProperty(const wxString& name, const wxFont& in);
    #endif
        
        void removeProperty(const wxString & name);

        JSONObject detachProperty(const wxString & name);
        
        void arrayAppend(const wxString & value);
        void arrayAppend(const JSONObject & element);

        bool isValid() const { return mJSON != NULL; }
        
        cJSON * release();

        void getString(wxString & out, bool format = true) const;

        char * getString(bool format = true) const;

        void getValue(gsgsStringMap & out) const;

        static JSONObject createObject(const wxString & name = wxT(""));

        static JSONObject createArray(const wxString & name = wxT(""));
    protected:
        cJSON* mJSON;
        int _type;
        wxString _name;
        wxVariant _value;
    };

    class _gsgsAPI JSON
    {
    public:
        JSON(int type);
        JSON(const wxString & text);
        JSON(const wxFileName & filename);
        JSON(JSONObject item);
        JSON(cJSON * json);
        virtual ~JSON();

        inline bool isValid() const { return mJSON != NULL; }

        void addProperty(const JSONObject & obj);
        JSONObject getObject() const;
        JSONObject getObject(const wxString & name) const;

        void save(const wxFileName & fn) const;
        
        void clear();

        cJSON* release();
    protected:
        JSON(const JSON& src) {}
        JSON& operator=(const JSON& src) { return *this; }
    protected:
        cJSON* mJSON;
    };
}
#endif