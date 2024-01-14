
#include "gsgsJSON.h"
#include "gsgsCommon.h"
#include "gsgsFileUtil.h"

namespace gsgs
{
    //---------------------------------------------------------------------------
    JSON::JSON(const wxString & text)
        : mJSON(0)
    {
        mJSON = cJSON_Parse(text.mb_str(wxConvUTF8).data());
    }
    //---------------------------------------------------------------------------
    JSON::JSON(cJSON * json)
        : mJSON(json)
    {
    }
    //---------------------------------------------------------------------------
    JSON::JSON(JSONObject item)
        : mJSON(item.release())
    {
    }
    //---------------------------------------------------------------------------
    JSON::JSON(int type)
        : mJSON(0)
    {
        if (type == cJSON_Array)
            mJSON = cJSON_CreateArray();
        else if (type == cJSON_NULL)
            mJSON = cJSON_CreateNull();
        else
            mJSON = cJSON_CreateObject();
    }
    //---------------------------------------------------------------------------
    JSON::JSON(const wxFileName& filename)
    {
        wxString content;
        if (FileUtil::ReadFileContent(filename, content))
        {
            mJSON = cJSON_Parse(content.mb_str(wxConvUTF8).data());
            if (!mJSON)
            {
                mJSON = cJSON_CreateObject();
            }
        }
        else
        {
            mJSON = 0;
        }
    }
    //---------------------------------------------------------------------------
    JSON::~JSON()
    {
        if (mJSON) {
            cJSON_Delete(mJSON);
            mJSON = 0;
        }
    }
    //---------------------------------------------------------------------------
    void JSON::save(const wxFileName& fn) const
    {
        if (isValid())
        {
            wxString temp;
            getObject().getString(temp);
            FileUtil::WriteFileContent(fn, temp, wxConvUTF8);
        }
        else
        {
            FileUtil::WriteFileContent(fn, "[]");
        }
    }
    //---------------------------------------------------------------------------
    void JSON::addProperty(const JSONObject & obj)
    {
        if (mJSON)
        {
            JSONObject(mJSON).addProperty(obj);
        }
    }
    //---------------------------------------------------------------------------
    JSONObject JSON::getObject() const
    {
        if (mJSON) {
            return JSONObject(mJSON);
        }
        return JSONObject(0);
    }
    //---------------------------------------------------------------------------
    JSONObject JSON::getObject(const wxString & name) const
    {
        if (mJSON)
        {
            cJSON* obj = cJSON_GetObjectItem(mJSON, name.mb_str(wxConvUTF8).data());
            if (obj) {
                return JSONObject(obj);
            }
        }
        return JSONObject(0);
    }
    //---------------------------------------------------------------------------
    void JSON::clear()
    {
        int type = cJSON_Object;
        if (mJSON) {
            type = mJSON->type;
            cJSON_Delete(mJSON);
            mJSON = 0;
        }
        if (type == cJSON_Array)
            mJSON = cJSON_CreateArray();
        else
            mJSON = cJSON_CreateObject();
    }
    //---------------------------------------------------------------------------
    cJSON* JSON::release()
    {
        cJSON* re = mJSON;
        mJSON = 0;
        return re;
    }
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    // JSONObject
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    JSONObject::JSONObject(cJSON* json)
        : mJSON(json)
    {
        if (mJSON) {
            _name = wxString(mJSON->string, wxConvUTF8);
            _type = mJSON->type;
        }
        else
        {
            _type = -1;
        }
    }
    //---------------------------------------------------------------------------
    JSONObject::JSONObject(const wxString& name, const wxVariant& val, int type)
        : mJSON(0)
        , _type(type)
    {
        _value = val;
        _name = name;
    }
    //---------------------------------------------------------------------------
    JSONObject JSONObject::getArray(int pos) const
    {
        if (mJSON)
        {
            if (mJSON->type != cJSON_Array) return JSONObject(0);

            int size = cJSON_GetArraySize(mJSON);
            if (pos >= size) return JSONObject(0);
            return JSONObject(cJSON_GetArrayItem(mJSON, pos));
        }
        return JSONObject(0);
    }
    //---------------------------------------------------------------------------
    bool JSONObject::isNull() const
    {
        if (mJSON)
        {
            return mJSON->type == cJSON_NULL;
        }
        return false;
    }
    //---------------------------------------------------------------------------
    bool JSONObject::isBool() const
    {
        if (mJSON)
        {
            return mJSON->type == cJSON_True || mJSON->type == cJSON_False;
        }
        return false;
    }
    //---------------------------------------------------------------------------
    bool JSONObject::isString() const
    {
        if (mJSON)
        {
            return mJSON->type == cJSON_String;
        }
        return false;
    }
    //---------------------------------------------------------------------------
    bool JSONObject::isArray() const
    {
        if (mJSON)
        {
            return mJSON->type == cJSON_Array;
        }
        return false;
    }
    //---------------------------------------------------------------------------
    bool JSONObject::isNumber() const
    {
        if (mJSON)
        {
            return mJSON->type == cJSON_Number;
        }
        return false;
    }
    //---------------------------------------------------------------------------
    bool JSONObject::getValue(bool& out, bool _default) const
    {
        if (mJSON)
        {
            if (!isBool()) { out = _default; return false; }
            out = (mJSON->type == cJSON_True);
            return true;
        }
        out = _default;
        return false;
    }
    //---------------------------------------------------------------------------
    bool JSONObject::getValue(wxString & out, const wxString& _default) const
    {
        if (mJSON)
        {
            if (mJSON->type != cJSON_String) { out = _default;  return false; }
            out = wxString(mJSON->valuestring, wxConvUTF8);
            return true;
        }
        out = _default;
        return false;
    }
    //---------------------------------------------------------------------------
    void JSONObject::addProperty(JSON * obj)
    {
        addProperty(obj->getObject()); obj->release();
    }
    //---------------------------------------------------------------------------
    void JSONObject::addProperty(const JSONObject& element)
    {
        if (mJSON)
        {
            switch (element.getType()) {
            case cJSON_False:
                cJSON_AddFalseToObject(mJSON, element.getName().mb_str(wxConvUTF8).data());
                break;

            case cJSON_True:
                cJSON_AddTrueToObject(mJSON, element.getName().mb_str(wxConvUTF8).data());
                break;

            case cJSON_NULL:
                cJSON_AddNullToObject(mJSON, element.getName().mb_str(wxConvUTF8).data());
                break;

            case cJSON_Number:
                cJSON_AddNumberToObject(mJSON, element.getName().mb_str(wxConvUTF8).data(), element.getValue().GetLong());
                break;

            case cJSON_String:
                cJSON_AddStringToObject(mJSON, element.getName().mb_str(wxConvUTF8).data(),
                    element.getValue().GetString().mb_str(wxConvUTF8).data());
                break;

            case cJSON_Array:
            case cJSON_Object:
                cJSON_AddItemToObject(mJSON, element.getName().mb_str(wxConvUTF8).data(), element.mJSON);
                break;
            }
        }
    }
    //---------------------------------------------------------------------------
    void JSONObject::arrayAppend(const JSONObject & element)
    {
        if (mJSON)
        {
            cJSON* p = 0;
            switch (element.getType()) {
            case cJSON_False:
                p = cJSON_CreateFalse();
                break;
            case cJSON_True:
                p = cJSON_CreateTrue();
                break;
            case cJSON_NULL:
                p = cJSON_CreateNull();
                break;
            case cJSON_Number:
                p = cJSON_CreateNumber(element.getValue().GetDouble());
                break;
            case cJSON_String:
                p = cJSON_CreateString(element.getValue().GetString().mb_str(wxConvUTF8).data());
                break;
            case cJSON_Array:
            case cJSON_Object:
                p = element.mJSON;
                break;
            }
            if (p)
            {
                cJSON_AddItemToArray(mJSON, p);
            }
        }
    }
    //---------------------------------------------------------------------------
    void JSONObject::arrayAppend(const wxString & value)
    {
        arrayAppend(JSONObject(wxT(""), value, cJSON_String));
    }
    //---------------------------------------------------------------------------
    JSONObject JSONObject::createArray(const wxString & name)
    {
        JSONObject arr(cJSON_CreateArray());
        arr.setName(name);
        arr.setType(cJSON_Array);
        return arr;
    }
    //---------------------------------------------------------------------------
    JSONObject JSONObject::createObject(const wxString & name)
    {
        JSONObject obj(cJSON_CreateObject());
        obj.setName(name);
        obj.setType(cJSON_Object);
        return obj;
    }
    //---------------------------------------------------------------------------
    void JSONObject::getString(wxString & out, bool format) const
    {
        if (mJSON) {
            char* p = format ? cJSON_Print(mJSON) : cJSON_PrintUnformatted(mJSON);
            out = wxString(p, wxConvUTF8);
            free(p);
        }
        else
        {
            out = wxT("");
        }
    }
    //---------------------------------------------------------------------------
    char* JSONObject::getString(bool format) const
    {
        if (mJSON)
        {
            if (format)
                return cJSON_Print(mJSON);
            else
                return cJSON_PrintUnformatted(mJSON);
        }
        return 0;
    }
    //---------------------------------------------------------------------------
    cJSON * JSONObject::release()
    {
        cJSON * re = mJSON;
        mJSON = 0;
        return re;
    }
    //---------------------------------------------------------------------------
    bool JSONObject::getValue(int & out, int _default) const
    {
        if (mJSON)
        {
            if (mJSON->type != cJSON_Number) { out = _default; return false; }
            out = mJSON->valueint;
            return true;
        }
        out = _default;
        return false;
    }
    //---------------------------------------------------------------------------
    bool JSONObject::getValue(size_t & out, size_t _default) const
    {
        if (mJSON)
        {
            if (mJSON->type != cJSON_Number) { out = _default; return false; }
            out = (size_t)mJSON->valueint;
            return true;
        }
        out = _default;
        return false;
    }
    //---------------------------------------------------------------------------
    bool JSONObject::getValue(long & out, long _default) const
    {
        if (mJSON)
        {
            if (mJSON->type != cJSON_Number) { out = _default; return false; }
            out = (long)mJSON->valueint;
            return true;
        }
        out = _default;
        return false;
    }
    //---------------------------------------------------------------------------
    bool JSONObject::getValue(double & out, double _default) const
    {
        if (mJSON)
        {
            if (mJSON->type != cJSON_Number) { out = _default; return false; }
            out = mJSON->valuedouble;
            return true;
        }
        out = _default;
        return false;
    }
    //---------------------------------------------------------------------------
    int JSONObject::getArraySize() const
    {
        if (mJSON)
        {
            if (mJSON->type != cJSON_Array) return 0;
            return cJSON_GetArraySize(mJSON);
        }
        return 0;
    }
    //---------------------------------------------------------------------------
    JSONObject JSONObject::getObject(const wxString & name) const
    {
        if (mJSON)
        {
            cJSON* obj = cJSON_GetObjectItem(mJSON, name.mb_str(wxConvUTF8).data());
            if (!obj) { return JSONObject(0); }
            return JSONObject(obj);
        }
        return JSONObject(0);
    }
    //---------------------------------------------------------------------------
    JSONObject& JSONObject::addProperty(const wxString& name, bool value)
    {
        if (value) {
            addProperty(JSONObject(name, value, cJSON_True));
        }
        else {
            addProperty(JSONObject(name, value, cJSON_False));
        }
        return *this;
    }
    //---------------------------------------------------------------------------
    JSONObject& JSONObject::addProperty(const wxString& name, const wxString& value)
    {
        addProperty(JSONObject(name, value, cJSON_String));
        return *this;
    }
    //---------------------------------------------------------------------------
    JSONObject& JSONObject::addProperty(const wxString& name, const wxChar* value)
    {
        addProperty(JSONObject(name, wxString(value), cJSON_String));
        return *this;
    }
    //---------------------------------------------------------------------------
    JSONObject& JSONObject::addProperty(const wxString& name, long value)
    {
        addProperty(JSONObject(name, value, cJSON_Number));
        return *this;
    }
    //---------------------------------------------------------------------------
    JSONObject& JSONObject::addProperty(const wxString& name, const wxArrayString& arr)
    {
        JSONObject arrEle = JSONObject::createArray(name);
        for (size_t i = 0; i < arr.GetCount(); i++) {
            arrEle.arrayAppend(arr.Item(i));
        }
        addProperty(arrEle);
        return *this;
    }
    //---------------------------------------------------------------------------
    bool JSONObject::getValue(wxArrayString & out, const wxArrayString & _default, bool clearout) const
    {
        if (clearout)
        {
            out.Empty();
        }
        if (mJSON) 
        {
            if (mJSON->type != cJSON_Array) 
            { 
                if (clearout)
                {
                    out = _default;
                }
                return false; 
            }
            for (int i = 0; i < getArraySize(); i++) {
                wxString temp;
                getArray(i).getValue(temp);
                out.Add(temp);
            }
            return true;
        }
        if (clearout)
        {
            out = _default;
        }
        return false;
    }
    //---------------------------------------------------------------------------
    bool JSONObject::isObjectExist(const wxString& name) const
    {
        if (mJSON) {
            cJSON* obj = cJSON_GetObjectItem(mJSON, name.mb_str(wxConvUTF8).data());
            return obj != 0;
        }
        return false;
    }
    //---------------------------------------------------------------------------
#if wxUSE_GUI
    JSONObject& JSONObject::addProperty(const wxString& name, const wxPoint& pt)
    {
        wxString szStr;
        szStr << pt.x << "," << pt.y;
        return addProperty(name, szStr);
    }
    //---------------------------------------------------------------------------
    JSONObject& JSONObject::addProperty(const wxString& name, const wxSize& sz)
    {
        wxString szStr;
        szStr << sz.x << "," << sz.y;
        return addProperty(name, szStr);
    }
    //---------------------------------------------------------------------------
    bool JSONObject::getValue(wxPoint & out) const
    {
        if (mJSON)
        {
            if (mJSON->type != cJSON_String) { out = wxDefaultPosition; return false; }

            wxString str = mJSON->valuestring;
            wxString x = str.BeforeFirst(',');
            wxString y = str.AfterFirst(',');

            long nX(-1), nY(-1);
            if (!x.ToLong(&nX) || !y.ToLong(&nY))
            {
                out = wxDefaultPosition;
                return false;
            }
            out = wxPoint(nX, nY);
            return true;
        }
        out = wxDefaultPosition;
        return false;
    }
    //---------------------------------------------------------------------------
    bool JSONObject::getValue(wxColour & out, const wxColour & _default) const
    {
        if (mJSON) {
            if (mJSON->type != cJSON_String) { out = _default; return false; }
            out = wxColour(mJSON->valuestring);
            return true;
        }
        out = _default;
        return false;
    }
    //---------------------------------------------------------------------------
    bool JSONObject::getValue(wxSize & out) const
    {
        wxPoint pt;
        getValue(pt);
        out = wxSize(pt.x, pt.y);
        return true;
    }
#endif
    //---------------------------------------------------------------------------
    JSONObject& JSONObject::addProperty(const wxString& name, const JSONObject& element)
    {
        if (mJSON) {
            cJSON_AddItemToObject(mJSON, name.mb_str(wxConvUTF8).data(), element.mJSON);
            return *this;
        }
        return *this;
    }
    //---------------------------------------------------------------------------
    void JSONObject::removeProperty(const wxString& name)
    {
        // delete child property
        if (mJSON) {
            cJSON_DeleteItemFromObject(mJSON, name.mb_str(wxConvUTF8).data());
        }
    }
    //---------------------------------------------------------------------------
#if wxUSE_GUI
    JSONObject& JSONObject::addProperty(const wxString& name, const gsgsStringMap& stringMap)
    {
        if (mJSON)
        {
            JSONObject arr = JSONObject::createArray(name);
            gsgsStringMap::const_iterator iter = stringMap.begin();
            for (; iter != stringMap.end(); ++iter) {
                JSONObject obj = JSONObject::createObject();
                obj.addProperty("key", iter->first);
                obj.addProperty("value", iter->second);
                arr.arrayAppend(obj);
            }
            addProperty(arr);
        }
        return *this;
    }
#endif
    //---------------------------------------------------------------------------
    void JSONObject::getValue(gsgsStringMap & out) const
    {
        if (mJSON)
        {
            if (mJSON->type == cJSON_Array)
            {
                for (int i = 0; i < getArraySize(); ++i) {
                    wxString key, val;
                    getArray(i).getObject("key").getValue(key);
                    getArray(i).getObject("value").getValue(val);
                    out.insert(std::make_pair(key, val));
                }
            }
        }
    }
    //---------------------------------------------------------------------------
    JSONObject& JSONObject::addProperty(const wxString& name, size_t value)
    {
        return addProperty(name, (int)value);
    }
    //---------------------------------------------------------------------------
#if wxUSE_GUI
    JSONObject& JSONObject::addProperty(const wxString& name, const wxColour& colour)
    {
        wxString colourValue;
        if (colour.IsOk()) { colourValue = colour.GetAsString(wxC2S_HTML_SYNTAX); }
        return addProperty(name, colourValue);
    }
#endif
    //---------------------------------------------------------------------------
    JSONObject& JSONObject::addProperty(const wxString& name, const char* value, const wxMBConv& conv)
    {
        return addProperty(name, wxString(value, conv));
    }
    //---------------------------------------------------------------------------
#if wxUSE_GUI
    JSONObject& JSONObject::addProperty(const wxString& name, const wxFont& font)
    {
        return addProperty(name, FontHelper::ToString(font));
    }
    //---------------------------------------------------------------------------
    bool JSONObject::getValue(wxFont & out, const wxFont& _default) const
    {
        wxString str;
        getValue(str);
        if (str.IsEmpty())
        {
            out = _default;
            return false;
        }
        out = FontHelper::FromString(str);
        return true;
    }
#endif
    //---------------------------------------------------------------------------
    JSONObject JSONObject::detachProperty(const wxString& name)
    {
        if (mJSON)
        {
            cJSON* j = cJSON_DetachItemFromObject(mJSON, name.c_str());
            return JSONObject(j);
        }
        return JSONObject(0);
    }
    //---------------------------------------------------------------------------
}