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

#ifndef _NII_Libxml_XML_SYS_H_
#define _NII_Libxml_XML_SYS_H_

#include "NiiPreInclude.h"
#include "NiiLibxmlXmlPattern.h"

#if defined( __WIN32__ ) || defined( _WIN32 )
    #ifdef CEGUILIBXMLPARSER_EXPORTS
        #define CEGUILIBXMLPARSER_API __declspec(dllexport)
    #else
        #define CEGUILIBXMLPARSER_API __declspec(dllimport)
    #endif
#else
    #define CEGUILIBXMLPARSER_API
#endif

namespace NII
{
    /** Libxml
    @version NIIEngine 3.0.0
    */
    class CEGUILIBXMLPARSER_API LibxmlParser : public XmlPattern
    {
    public:
        LibxmlParser();
        ~LibxmlParser();

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