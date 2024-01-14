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
#ifndef _NII_ExpatParser_H_
#define _NII_ExpatParser_H_

#include "NiiExpatXmlPattern.h"

#if (defined( __WIN32__ ) || defined( _WIN32 )) && !defined(CEGUI_STATIC)
    #ifdef CEGUIEXPATPARSER_EXPORTS
        #define CEGUIEXPATPARSER_API __declspec(dllexport)
    #else
        #define CEGUIEXPATPARSER_API __declspec(dllimport)
    #endif
#else
    #define CEGUIEXPATPARSER_API
#endif

namespace NII
{
    /** Expat
    @version NIIEngine 3.0.0
    */
    class CEGUIEXPATPARSER_API ExpatParser : public XmlPattern
    {
    public:
        ExpatParser(void);
        ~ExpatParser(void);

        /// @copydetails XmlPattern::parse
        void parse(XmlAnalyze * prc, const DataStream * source);
    protected:
        /// @copydetails Pattern::_init
        bool _init();
        
        /// @copydetails Pattern::_end
        bool _end();
    };
}
#endif