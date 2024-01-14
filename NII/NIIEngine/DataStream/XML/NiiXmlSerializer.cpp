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

#include "NiiPreProcess.h"
#include "NiiXMLSerializer.h"
#include "NiiScriptHelperManager.h"
#include "NiiScriptProperty.h"
#include <iostream>

namespace NII
{
    //------------------------------------------------------------------------
    String convertEntityInText(const String & text)
    {
        String res;
        res.reserve(text.size() * 2);
        const String::const_iterator iterEnd = text.end();
        for (String::const_iterator iter = text.begin(); iter != iterEnd; ++iter)
        {
            switch (*iter)
            {
            case '<':
                res += _T("&lt;");
                break;
            case '>':
                res += _T("&gt;");
                break;
            case '&':
                res += _T("&amp;");
                break;
            case '\'':
                res += _T("&apos;");
                break;
            case '"':
                res += _T("&quot;");
                break;
            default:
                res += *iter;
            }
        }
        return res;
    }
    //------------------------------------------------------------------------
    String convertEntityInAttribute(const String & attributeValue)
    {
        String res;
        res.reserve(attributeValue.size() * 2);
        const String::const_iterator iterEnd = attributeValue.end();
        for (String::const_iterator iter = attributeValue.begin(); iter != iterEnd; ++iter)
        {
            switch (*iter)
            {
            case '<':
                res += _T("&lt;");
                break;
            case '>':
                res += _T("&gt;");
                break;
            case '&':
                res += _T("&amp;");
                break;
            case '\'':
                res += _T("&apos;");
                break;
            case '"':
                res += _T("&quot;");
                break;
            case '\n':
                res += _T("\\n");
                break;
            default:
                res += *iter;
            }
        }
        return res;
    }
    //------------------------------------------------------------------------
    XmlSerializer::XmlSerializer(const XmlSerializer & obj):
        mStream(obj.mStream)
    {

    }
    //------------------------------------------------------------------------
    XmlSerializer::XmlSerializer(Nostream * out, NCount indentSpace):
        mTagCount(0),
        mCurrentDepth(0),
        mIndentSpace(indentSpace),
        mStream(out),
        mCmdObj(0),
        mError(false),
        mCloseLast(false),
        mLastText(false)
    {
        *mStream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
        mError = !*mStream;
    }
    //------------------------------------------------------------------------
    XmlSerializer::XmlSerializer(Nostream * out, const ScriptProperty * obj, NCount indent):
        mTagCount(0),
        mCurrentDepth(0),
        mIndentSpace(indent),
        mStream(out),
        mCmdObj(obj),
        mError(false),
        mCloseLast(false),
        mLastText(false)
    {
        *mStream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
        mError = !*mStream;
    }
    //------------------------------------------------------------------------
    XmlSerializer::~XmlSerializer()
    {
        if (!mError || !mTagList.empty())
        {
            *mStream << std::endl;
        }
    }
    //------------------------------------------------------------------------
    XmlSerializer & XmlSerializer::operator=(const XmlSerializer & obj)
    {
        return *this;
    }
    //------------------------------------------------------------------------
    XmlSerializer & XmlSerializer::begin(XmlUnitID uid)
    {
        return begin(N_Only(ScriptHelper).getUnit(mCmdObj->getLangID(), uid));
    }
    //------------------------------------------------------------------------
    XmlSerializer & XmlSerializer::begin(const String & name)
    {
        if(!mError)
        {
            ++mTagCount;
            if(mCloseLast)
            {
                *mStream << '>';
            }
            if(!mLastText)
            {
                *mStream << std::endl;
                indentLine();
            }
            *mStream << '<' << name.c_str() << ' ';
            mTagList.push_back(name);
            ++mCurrentDepth;
            mCloseLast = true;
            mLastText = false;
            mError = !*mStream;
        }
        return *this;
    }
    //------------------------------------------------------------------------
    XmlSerializer & XmlSerializer::end(void)
    {
        String back =  mTagList.back();
        if(!mError)
        {
            --mCurrentDepth;
            if(mCloseLast)
            {
                *mStream << "/>";
            }
            else if(!mLastText)
            {
                *mStream << std::endl;
                indentLine();
                *mStream << "</" << back.c_str() << '>';
            }
            else
            {
                *mStream << "</" << back.c_str() << '>';
            }
            mLastText = false;
            mCloseLast = false;
            mTagList.pop_back();
            mError = !*mStream;
        }
        return *this;
    }
    //------------------------------------------------------------------------
    XmlSerializer & XmlSerializer::attribute(PropertyID pid, const String & value)
    {
        return attribute(mCmdObj->getLang(pid), value);
    }
    //------------------------------------------------------------------------
    XmlSerializer & XmlSerializer::attribute(const String & name, const String & value)
    {
        if(!mCloseLast)
        {
            mError = true;
        }
        if(!mError)
        {
            *mStream << name.c_str() << "=\""
                << convertEntityInAttribute(value).c_str()
                << "\" ";
            mLastText = false;
            mError = !*mStream;
        }
        return *this;
    }
    //------------------------------------------------------------------------

    //------------------------------------------------------------------------
    XmlSerializer & XmlSerializer::text(const String & text)
    {
        if(! mError)
        {
            if(mCloseLast)
            {
                *mStream << '>';
                mCloseLast = false;
            }
            *mStream << convertEntityInText(text).c_str();
            mLastText = true;
            mError = !*mStream;
        }
        return *this;
    }
    //------------------------------------------------------------------------
    NCount XmlSerializer::getTagCount() const
    {
        return mTagCount;
    }
    //------------------------------------------------------------------------
    void XmlSerializer::indentLine()
    {
        NCount spaceCount = mCurrentDepth * mIndentSpace;
        // There's for sure a best way to do this but it works
        for(NCount i = 0 ; i < spaceCount ; ++i)
        {
            *mStream << ' ';
        }
    }
    //------------------------------------------------------------------------
}