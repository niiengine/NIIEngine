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

#ifndef _NII_Rapid_XmlPattern_H_
#define _NII_Rapid_XmlPattern_H_

#include "NiiPreInclude.h"
#include "NiiRapidXmlPattern.h"

#if (defined( __WIN32__ ) || defined( _WIN32 )) && !defined(CEGUI_STATIC)
    #ifdef CEGUIRAPIDXMLPARSER_EXPORTS
        #define CEGUIRAPIDXMLPARSER_API __declspec(dllexport)
    #else
        #define CEGUIRAPIDXMLPARSER_API __declspec(dllimport)
    #endif
#else
    #define CEGUIRAPIDXMLPARSER_API
#endif

namespace NII
{
    /**
    @version NIIEngine 3.0.0
    */
    class CEGUIRAPIDXMLPARSER_API RapidXMLParser : public XmlPattern
    {
    public:
        RapidXMLParser();
        ~RapidXMLParser();

        /// @copydetails XmlPattern::parse
        void parse(XmlAnalyze * prc, const DataStream * src);
    protected:
        /// @copydetails XmlPattern::_init
        bool _init();
        
        /// @copydetails XmlPattern::_end
        bool _end();
    };
}
#endif