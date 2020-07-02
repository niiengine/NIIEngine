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

#ifndef _NII_UI_VerticalLayoutContainer_H_
#define _NII_UI_VerticalLayoutContainer_H_

#include "NiiUIPreInclude.h"
#include "NiiUIIndexLayoutContainer.h"

namespace NII
{
namespace UI
{
    /** ¥π÷±≈≈∞Ê
    @version NIIEngine 3.0.0
    */
    class _EngineAPI VerticalLayoutContainer : public IndexLayoutContainer
    {
    public:
        VerticalLayoutContainer(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
            LangID lid = N_PrimaryLang);

        virtual ~VerticalLayoutContainer();

        /// @copydoc LayoutContainer::layoutImpl
        virtual void layoutImpl();
    };
}
}
#endif