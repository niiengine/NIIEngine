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

#ifndef _NII_XML_SYS_H_
#define _NII_XML_SYS_H_

#include "NiiPreInclude.h"
#include "NiiPattern.h"

namespace NII
{
    /** XML������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI XmlPattern : public Pattern
    {
    public:
        XmlPattern();
        virtual ~XmlPattern();

        /** ��ʼ��
        @version NIIEngine 3.0.0
        */
        bool startup();

        /** �ر�
        @version NIIEngine 3.0.0
        */
        void shutdown();

        /** ����
        @version NIIEngine 3.0.0
        */
        virtual void parse(XmlAnalyze * prc, const DataStream * data) = 0;
    protected:
        /// @copydetails Pattern::_update
        void _update();
        
        /// @copydetails Pattern::getPID
        PatternID getPID() const;
    private:
        PatternID mPatternID;
        bool mInit;
    };
}
#endif