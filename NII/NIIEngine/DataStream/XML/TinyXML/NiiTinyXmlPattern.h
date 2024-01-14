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
#ifndef _NII_TinyXMLParser_H_
#define _NII_TinyXMLParser_H_

#include "NiiPreInclude.h"
#include "NiiTinyXmlPattern.h"

#if (defined( __WIN32__ ) || defined( _WIN32 )) && !defined(CEGUI_STATIC)
    #ifdef CEGUITINYXMLPARSER_EXPORTS
        #define CEGUITINYXMLPARSER_API __declspec(dllexport)
    #else
        #define CEGUITINYXMLPARSER_API __declspec(dllimport)
    #endif
#else
    #define CEGUITINYXMLPARSER_API
#endif

namespace NII
{
    /** TinyXML
    @version NIIEngine 3.0.0
    */
    class CEGUITINYXMLPARSER_API TinyXMLParser : public XmlPattern
    {
    public:
        TinyXMLParser();
        ~TinyXMLParser();

        /// @copydetails XmlPattern::parse
        void parse(XmlAnalyze * prc, const DataStream * data);
    protected:
        /// @copydetails Pattern::_init
        bool _init();
        
        /// @copydetails Pattern::_end
        bool _end();
    };
}
#endif