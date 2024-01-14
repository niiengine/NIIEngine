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
#include "NiiDriverVersion.h"

namespace NII
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // DriverVersion
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    DriverVersion::DriverVersion()
    {
        major = minor = release = build = 0;
    }
    //------------------------------------------------------------------------
    bool DriverVersion::isOlder(DriverVersion v) const
    {
        if(major < v.major)
            return true;
        else if (major == v.major && minor < v.minor)
            return true;
        else if (major == v.major && minor == v.minor && release < v.release)
            return true;
        else if (major == v.major && minor == v.minor && release == v.release && build < v.build)
                return true;
        return false;
    }
    //------------------------------------------------------------------------
    bool DriverVersion::isOlder(NIIi _major, NIIi _minor) const
    {
        if(major < _major)
            return true;
        else if (major == _major && minor < _minor)
            return true;
        return false;
    }
    //------------------------------------------------------------------------
    bool DriverVersion::isNewer(DriverVersion o) const
    {
        if(major > v.major)
            return true;
        else if (major == v.major && minor > v.minor)
            return true;
        else if (major == v.major && minor == v.minor && release > v.release)
            return true;
        else if (major == v.major && minor == v.minor && release == v.release && build >= v.build)
            return true;
        return false;
    }
    //------------------------------------------------------------------------
    bool DriverVersion::isNewer(NIIi _major, NIIi _minor) const
    {
        if(major > _major)
            return true;
        else if (major == _major && minor > _minor)
            return true;
        return false;
    }
    //------------------------------------------------------------------------
    String DriverVersion::toString() const
    {
        StringStream str;
        str << major << "." << minor << "." << release << "." << build;
        return str.str();
    }
    //------------------------------------------------------------------------
    void DriverVersion::fromString(const String & str)
    {
        StringList tokens;
        StrUtil::split(str, tokens, _T("."));
        if(!tokens.empty())
        {
            StrConv::conv(tokens[0], major);
            if(tokens.size() > 1)
                StrConv::conv(tokens[1], minor);
            if(tokens.size() > 2)
                StrConv::conv(tokens[2], release);
            if(tokens.size() > 3)
                StrConv::conv(tokens[3], build);
        }

    }
    //------------------------------------------------------------------------
}