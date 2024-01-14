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
#include "NiiExpatXmlPattern.h"
#include "NiiUIManager.h"
#include "NiiXmlAnalyze.h"
#include "NiiPropertyData.h"
#include "CEGUI/Exceptions.h"

#include <expat.h>

namespace NII
{
namespace NII_MEDIA
{
namespace UI
{
    //------------------------------------------------------------------------
    void nii_ext_expat_begin(void * data, const char * element, const char ** attr)
    {
        XmlAnalyze * xc = static_cast<XmlAnalyze *>(data);
        PropertyData attrs;

        for(NCount i = 0 ; attr[i] ; i += 2)
        {
            attrs.add(xc->get((const Nutf8 *)attr[i]), (const Nutf8 *)attr[i+1]);
        }
        xc->onBegin(xc->get((const Nutf8 *)element), &attrs);
    }
    //------------------------------------------------------------------------
    void nii_ext_expat_data(void * data, const char * text, int size)
    {
        XmlAnalyze * xc = static_cast<XmlAnalyze *>(data);
        String str((const Nutf8 *)text, static_cast<String::size_type>(size));
        xc->onData(str);
    }
    //------------------------------------------------------------------------
    void nii_ext_expat_end(void * data, const char * element)
    {
        XmlAnalyze * xc = static_cast<XmlAnalyze *>(data);
        xc->onEnd(xc->get((const Nutf8 *)element));
    }
    //------------------------------------------------------------------------
    ExpatParser::ExpatParser()
    {
        mPatternID = N_Pattern_Expat;
    }
    //------------------------------------------------------------------------
    ExpatParser::~ExpatParser()
    {
    }
    //------------------------------------------------------------------------
    void ExpatParser::parse(XmlAnalyze * xc, const DataStream * source)
    {
        // All stuff goes here
        XML_Parser parser = XML_ParserCreate(0); // Create a parser

        if(!parser)
        {
            N_EXCEPT(Generic, _T("Unable to create a new Expat Parser"));
        }

        XML_SetUserData(parser, (void*)xc); // Initialise user data
        XML_SetElementHandler(parser, nii_ext_expat_begin, nii_ext_expat_end);
        XML_SetCharacterDataHandler(parser, nii_ext_expat_data);

        Nui8 * data;
        if(source->isSysMemory() == false)
            source->cloneData(data);
        else
            data = static_cast<MemStream *>(source)->getData();

        // Parse the data (note that the last true parameter tels Expat that this is the last chunk of the document
        if(!XML_Parse(parser, reinterpret_cast<const char *>(data), source->size(), true))
        {
            String exception(String("XML Parsing error '") +
                String((const Nutf8 *)XML_ErrorString(XML_GetErrorCode(parser))) +
                    String("' at line ") + Nid(XML_GetCurrentLineNumber(parser)));
            // (We know it is a valid pointer, otherwise an exception would have been thrown above.)
            XML_ParserFree(parser);
            N_EXCEPT(Generic, exception);
        }

        // (We know it is a valid pointer, otherwise an exception would have been thrown above.)
        XML_ParserFree(parser);

        if(source->isSysMemory() == false)
            N_free(data);
    }
    //------------------------------------------------------------------------
    bool ExpatParser::_init()
    {
        return true;
    }
    //------------------------------------------------------------------------
    bool ExpatParser::_end()
    {
        return true;
    }
    //------------------------------------------------------------------------
}
}
}