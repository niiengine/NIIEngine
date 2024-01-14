//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : json_node.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include "json_node.h"
#include <stdlib.h>
#include <wx/filename.h>
#include <wx/ffile.h>

JSONRoot::JSONRoot(const wxString& text)
    : mJSON(NULL)
{
    mJSON = cJSON_Parse(text.mb_str(wxConvUTF8).data());
}

JSONRoot::JSONRoot(int type)
    : mJSON(NULL)
{
    if(type == cJSON_Array)
        mJSON = cJSON_CreateArray();
    else
        mJSON = cJSON_CreateObject();
}

JSONRoot::JSONRoot(const wxFileName& filename)
    : mJSON(NULL)
{
    wxString content;
    wxFFile fp(filename.GetFullPath(), wxT("rb"));
    if(fp.IsOpened()) {
        if(fp.ReadAll(&content, wxConvUTF8)) { mJSON = cJSON_Parse(content.mb_str(wxConvUTF8).data()); }
    }

    if(!mJSON) { mJSON = cJSON_CreateObject(); }
}

JSONRoot::~JSONRoot()
{
    if(mJSON) {
        cJSON_Delete(mJSON);
        mJSON = NULL;
    }
}

void JSONRoot::save(const wxFileName& fn) const
{
    wxFFile fp(fn.GetFullPath(), wxT("w+b"));
    if(fp.IsOpened()) {
        fp.Write(getObject().getString(), wxConvUTF8);
        fp.Close();
    }
}

JSONElement JSONRoot::getObject() const
{
    if(!mJSON) { return JSONElement(NULL); }
    return JSONElement(mJSON);
}

JSONElement JSONElement::getObject(const wxString& name) const
{
    if(!mJSON) { return JSONElement(NULL); }

    cJSON* obj = cJSON_GetObjectItem(mJSON, name.mb_str(wxConvUTF8).data());
    if(!obj) { return JSONElement(NULL); }
    return JSONElement(obj);
}

void JSONRoot::clear()
{
    int type = cJSON_Object;
    if(mJSON) {
        type = mJSON->type;
        cJSON_Delete(mJSON);
        mJSON = NULL;
    }
    if(type == cJSON_Array)
        mJSON = cJSON_CreateArray();
    else
        mJSON = cJSON_CreateObject();
}

JSONElement JSONRoot::release()
{
    cJSON* p = mJSON;
    mJSON = NULL;
    return JSONElement(p);
}

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
JSONElement::JSONElement(cJSON* json)
    : mJSON(json)
    , _type(-1)
    //, _walker(NULL)
{
    if(mJSON) {
        _name = wxString(mJSON->string, wxConvUTF8);
        _type = mJSON->type;
    }
}

JSONElement::JSONElement(const wxString& name, const wxVariant& val, int type)
    : mJSON(NULL)
    , _type(type)
    //, _walker(NULL)
{
    _value = val;
    _name = name;
}

JSONElement JSONElement::getArray(int pos) const
{
    if(!mJSON) { return JSONElement(NULL); }

    if(mJSON->type != cJSON_Array) return JSONElement(NULL);

    int size = cJSON_GetArraySize(mJSON);
    if(pos >= size) return JSONElement(NULL);

    return JSONElement(cJSON_GetArrayItem(mJSON, pos));
}

bool JSONElement::isNull() const
{
    if(!mJSON) { return false; }
    return mJSON->type == cJSON_NULL;
}

bool JSONElement::toBool(bool defaultValue) const
{
    if(!mJSON) { return defaultValue; }

    if(!isBool()) { return defaultValue; }

    return mJSON->type == cJSON_True;
}

wxString JSONElement::toString(const wxString& defaultValue) const
{
    if(!mJSON) { return defaultValue; }

    if(mJSON->type != cJSON_String) { return defaultValue; }

    return wxString(mJSON->valuestring, wxConvUTF8);
}

bool JSONElement::isBool() const
{
    if(!mJSON) { return false; }

    return mJSON->type == cJSON_True || mJSON->type == cJSON_False;
}

bool JSONElement::isString() const
{
    if(!mJSON) { return false; }

    return mJSON->type == cJSON_String;
}

void JSONElement::append(const JSONElement& element)
{
    if(!mJSON) { return; }

    switch(element.getType()) {
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

void JSONElement::arrayAppend(const JSONElement& element)
{
    if(!mJSON) { return; }

    cJSON* p = NULL;
    switch(element.getType()) {
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
    if(p) { cJSON_AddItemToArray(mJSON, p); }
}

JSONElement JSONElement::createArray(const wxString& name)
{
    JSONElement arr(cJSON_CreateArray());
    arr.setName(name);
    arr.setType(cJSON_Array);
    return arr;
}

JSONElement JSONElement::createObject(const wxString& name)
{
    JSONElement obj(cJSON_CreateObject());
    obj.setName(name);
    obj.setType(cJSON_Object);
    return obj;
}

char* JSONElement::getString(bool formatted) const
{
    if(!mJSON) { return NULL; }

    if(formatted) {
        return cJSON_Print(mJSON);

    } else {
        return cJSON_PrintUnformatted(mJSON);
    }
}

void JSONElement::getString(wxString & out) const
{
    if(!mJSON) { out = wxT(""); }

    char* p = cJSON_Print(mJSON);
    out = wxString(p, wxConvUTF8);
    free(p);
}

int JSONElement::toInt(int defaultVal) const
{
    if(!mJSON) { return defaultVal; }

    if(mJSON->type != cJSON_Number) { return defaultVal; }

    return mJSON->valueint;
}

size_t JSONElement::toSize_t(size_t defaultVal) const
{
    if(!mJSON) { return defaultVal; }

    if(mJSON->type != cJSON_Number) { return defaultVal; }

    return (size_t)mJSON->valueint;
}

double JSONElement::toDouble(double defaultVal) const
{
    if(!mJSON) { return defaultVal; }

    if(mJSON->type != cJSON_Number) { return defaultVal; }

    return mJSON->valuedouble;
}

int JSONElement::getArraySize() const
{
    if(!mJSON) { return 0; }

    if(mJSON->type != cJSON_Array) return 0;

    return cJSON_GetArraySize(mJSON);
}

JSONElement& JSONElement::addProperty(const wxString& name, bool value)
{
    if(value) {
        append(JSONElement(name, value, cJSON_True));

    } else {
        append(JSONElement(name, value, cJSON_False));
    }
    return *this;
}

JSONElement& JSONElement::addProperty(const wxString& name, const wxString& value)
{
    append(JSONElement(name, value, cJSON_String));
    return *this;
}

JSONElement& JSONElement::addProperty(const wxString& name, const wxChar* value)
{
    append(JSONElement(name, wxString(value), cJSON_String));
    return *this;
}

JSONElement& JSONElement::addProperty(const wxString& name, long value)
{
    append(JSONElement(name, value, cJSON_Number));
    return *this;
}

JSONElement& JSONElement::addProperty(const wxString& name, const wxArrayString& arr)
{
    JSONElement arrEle = JSONElement::createArray(name);
    for(size_t i = 0; i < arr.GetCount(); i++) {
        arrEle.arrayAppend(arr.Item(i));
    }
    append(arrEle);
    return *this;
}

void JSONElement::arrayAppend(const wxString& value) { arrayAppend(JSONElement(wxT(""), value, cJSON_String)); }

wxArrayString JSONElement::toArrayString() const
{
    wxArrayString arr;
    if(!mJSON) { return arr; }

    if(mJSON->type != cJSON_Array) { return arr; }

    for(int i = 0; i < getArraySize(); i++) {
        arr.Add(getArray(i).toString());
    }
    return arr;
}

bool JSONElement::isObjectExist(const wxString& name) const
{
    if(!mJSON) { return false; }

    cJSON* obj = cJSON_GetObjectItem(mJSON, name.mb_str(wxConvUTF8).data());
    return obj != NULL;
}

JSONElement& JSONElement::addProperty(const wxString& name, const wxPoint& pt)
{
    wxString szStr;
    szStr << pt.x << "," << pt.y;
    return addProperty(name, szStr);
}

JSONElement& JSONElement::addProperty(const wxString& name, const wxSize& sz)
{
    wxString szStr;
    szStr << sz.x << "," << sz.y;
    return addProperty(name, szStr);
}

JSONElement& JSONElement::addProperty(const wxString& name, const JSONElement& element)
{
    if(!mJSON) { return *this; }
    cJSON_AddItemToObject(mJSON, name.mb_str(wxConvUTF8).data(), element.mJSON);
    return *this;
}

wxPoint JSONElement::toPoint() const
{
    if(!mJSON) { return wxDefaultPosition; }

    if(mJSON->type != cJSON_String) { return wxDefaultPosition; }

    wxString str = mJSON->valuestring;
    wxString x = str.BeforeFirst(',');
    wxString y = str.AfterFirst(',');

    long nX(-1), nY(-1);
    if(!x.ToLong(&nX) || !y.ToLong(&nY)) return wxDefaultPosition;

    return wxPoint(nX, nY);
}

wxColour JSONElement::toColour(const wxColour& defaultColour) const
{
    if(!mJSON) { return defaultColour; }

    if(mJSON->type != cJSON_String) { return defaultColour; }

    return wxColour(mJSON->valuestring);
}

wxSize JSONElement::toSize() const
{
    wxPoint pt = toPoint();
    return wxSize(pt.x, pt.y);
}

void JSONElement::removeProperty(const wxString& name)
{
    // delete child property
    if(!mJSON) { return; }
    cJSON_DeleteItemFromObject(mJSON, name.mb_str(wxConvUTF8).data());
}

JSONElement& JSONElement::addProperty(const wxString& name, const gsgsStringMap& stringMap)
{
    if(!mJSON) return *this;

    JSONElement arr = JSONElement::createArray(name);
    gsgsStringMap::const_iterator iter = stringMap.begin();
    for(; iter != stringMap.end(); ++iter) {
        JSONElement obj = JSONElement::createObject();
        obj.addProperty("key", iter->first);
        obj.addProperty("value", iter->second);
        arr.arrayAppend(obj);
    }
    append(arr);
    return *this;
}

gsgsStringMap JSONElement::getStringMap() const
{
    gsgsStringMap res;
    if(!mJSON) { return res; }

    if(mJSON->type != cJSON_Array) { return res; }

    for(int i = 0; i < getArraySize(); ++i) {
        wxString key = getArray(i).getObject("key").toString();
        wxString val = getArray(i).getObject("value").toString();
        res.insert(std::make_pair(key, val));
    }
    return res;
}

JSONElement& JSONElement::addProperty(const wxString& name, size_t value) { return addProperty(name, (int)value); }

JSONElement& JSONElement::addProperty(const wxString& name, const wxColour& colour)
{
    wxString colourValue;
    if(colour.IsOk()) { colourValue = colour.GetAsString(wxC2S_HTML_SYNTAX); }
    return addProperty(name, colourValue);
}
/*
JSONElement JSONElement::firstChild()
{
    _walker = NULL;
    if(!mJSON) { return JSONElement(NULL); }

    if(!mJSON->child) { return JSONElement(NULL); }

    _walker = mJSON->child;
    return JSONElement(_walker);
}

JSONElement JSONElement::nextChild()
{
    if(!_walker) { return JSONElement(NULL); }

    JSONElement element(_walker->next);
    _walker = _walker->next;
    return element;
}
*/
JSONElement& JSONElement::addProperty(const wxString& name, const char* value, const wxMBConv& conv)
{
    return addProperty(name, wxString(value, conv));
}

JSONElement JSONElement::detachProperty(const wxString& name)
{
    if(!mJSON) { return JSONElement(NULL); }
    cJSON* j = cJSON_DetachItemFromObject(mJSON, name.mb_str().data());
    return JSONElement(j);
}
