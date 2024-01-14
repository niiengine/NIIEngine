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
#include "NiiConfigFile.h"
#include "NiiResourceSchemeManager.h"
#include "NiiDefaultStream.h"
#include "NiiException.h"

#include <iostream>

namespace NII
{
    //-----------------------------------------------------------------------
    ConfigFile::ConfigFile()
    {
    }
    //-----------------------------------------------------------------------
    ConfigFile::~ConfigFile()
    {
        SectionSetList::iterator i, iend = mSettings.end();
        for(i = mSettings.begin(); i != iend; ++i)
        {
            N_delete_t(i->second, SetList);
        }
    }
    //-----------------------------------------------------------------------
    void ConfigFile::clear()
    {
        SectionSetList::iterator i, iend = mSettings.end();
        for(i = mSettings.begin(); i != iend; ++i)
        {
             N_delete_t(i->second, SetList);
        }
        mSettings.clear();
    }
    //-----------------------------------------------------------------------
    void ConfigFile::load(const String & filename, const String & sep, bool trim)
    {
        /* Open the configuration file */
        std::ifstream fp;
        // Always open in binary mode
        fp.open(filename.c_str(), std::ios::in | std::ios::binary);
        if(!fp)
            N_EXCEPT(InvalidFile, _T("'") + filename + _I18n("' file not found!"));

        // Wrap as a stream
        DataStream * stream = N_new FileDataStream(&fp, filename, false);

#if N_PLAT == N_PLAT_SYMBIAN
        // seems readLine doesn't work correctly in SYMBIAN with files
        DataStream * memoryStream = N_new MemStream(stream);

        load(memoryStream, sep, trim);

        N_delete stream;
        N_delete memoryStream;
        return;
#endif
        load(stream, sep, trim);
        N_delete stream;
    }
    //-----------------------------------------------------------------------
    void ConfigFile::load(const String & file, GroupID gid, const String & sep, bool trim)
    {
        DataStream * stream = N_Only(ResourceScheme).open(file, gid);
        load(stream, sep, trim);
        N_delete stream;
    }
    //-----------------------------------------------------------------------
    void ConfigFile::load(const DataStream * stream, const String & sep, bool trim)
    {
        /* Clear current settings map */
        clear();

        String currentSection = StrUtil::WBLANK;
        SetList * current = N_new_t(SetList)();
        mSettings[currentSection] = current;

        /* Process the file line for line */
        String line, optName, optVal;
        while (!stream->end())
        {
            line = stream->readLine();
            /* Ignore comments & blanks */
            if (line.length() > 0 && line.at(0) != '#' && line.at(0) != '@')
            {
                if (line.at(0) == '[' && line.at(line.length()-1) == ']')
                {
                    // Section
                    currentSection = line.substr(1, line.length() - 2);
                    SectionSetList::const_iterator seci = mSettings.find(currentSection);
                    if (seci == mSettings.end())
                    {
                        current = N_new_t(SetList)();
                        mSettings[currentSection] = current;
                    }
                    else
                    {
                        current = seci->second;
                    }
                }
                else
                {
                    /* Find the first seperator character and split the string there */
                    String::size_type separator_pos = line.find_first_of(sep, 0);
                    if(separator_pos != String::npos)
                    {
                        optName = line.substr(0, separator_pos);
                        /* Find the first non-seperator character following the name */
                        String::size_type nonseparator_pos = line.find_first_not_of(sep, separator_pos);
                        /* ... and extract the value */
                        /* Make sure we don't crash on an empty setting (it might be a valid value) */
                        optVal = (nonseparator_pos == String::npos) ? _T("") : line.substr(nonseparator_pos);
                        if(trim)
                        {
                            StrUtil::trim(optVal);
                            StrUtil::trim(optName);
                        }
                        current->insert(SetList::value_type(optName, optVal));
                    }
                }
            }
        }
    }
    //-----------------------------------------------------------------------
    String ConfigFile::getValue(const String & key, const String & section, const String & defaultValue) const
    {
        SectionSetList::const_iterator seci = mSettings.find(section);
        if (seci == mSettings.end())
        {
            return defaultValue;
        }
        else
        {
            SetList::const_iterator i = seci->second->find(key);
            if (i == seci->second->end())
            {
                return defaultValue;
            }
            else
            {
                return i->second;
            }
        }
    }
    //-----------------------------------------------------------------------
    StringList ConfigFile::getValues(const String & key, const String & section) const
    {
        StringList ret;

        SectionSetList::const_iterator seci = mSettings.find(section);
        if (seci != mSettings.end())
        {
            SetList::const_iterator i = seci->second->find(key);
            while (i != seci->second->end() && i->first == key)
            {
                ret.push_back(i->second);
                ++i;
            }
        }
        return ret;
    }
    //-----------------------------------------------------------------------
    const ConfigFile::SetList & ConfigFile::getValueList(const String & section) const
    {
        static SetList re;
        SectionSetList::const_iterator seci = mSettings.find(section);
        if (seci != mSettings.end())
        {
            return *seci->second;
        }
        return re;
    }
    //-----------------------------------------------------------------------
    const ConfigFile::SectionSetList & ConfigFile::getSectionList() const
    {
        return mSettings;
    }
    //-----------------------------------------------------------------------
}