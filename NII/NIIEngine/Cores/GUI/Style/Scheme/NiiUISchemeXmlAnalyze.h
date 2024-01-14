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
#ifndef _NII_UI_SCHEME_XMLCOM_H_
#define _NII_UI_SCHEME_XMLCOM_H_

#include "NiiUIPreInclude.h"
#include "NiiXmlAnalyzeManager.h"
#include "NiiUIScheme.h"

namespace NII
{
namespace UI
{
    /** UI²ßÂÔXml·ÖÎö
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SchemeXmlAnalyze : public XmlAnalyze
    {
    public:
        SchemeXmlAnalyze(LangID lid = N_PrimaryLang);
        ~SchemeXmlAnalyze();

        inline SchemeID getObjID() const        { return mScheme->getID(); }

        inline Scheme * getObject() const       { return mScheme;}

        /// @copydetails XmlAnalyze::getXSD
        const String & getXSD() const;

        /// @copydetails XmlAnalyze::getUnit
        XmlUnitID getUnit() const;

        /// @copydetails XmlAnalyze::getGroup
        GroupID getGroup() const;
    private:
        /// @copydetails XmlAnalyze::PrcTagUnitCB
        void PrcTagUnitCB(const PropertyData * pdc);

        void PrcViewCB(const PropertyData * pdc);
        void PrcPixelGroupCB(const PropertyData * pdc);
        void PrcFontCB(const PropertyData * pdc);
        void PrcWidgetSrcCB(const PropertyData * pdc);
        void PrcWidgetFactoryCB(const PropertyData * pdc);
        void PrcWidgetViewModuleCB(const PropertyData * pdc);
        void PrcWidgetViewFactory(const PropertyData * pdc);
        void PrcWidgetAliasCB(const PropertyData * pdc);
        void PrcWidgetViewCB(const PropertyData * pdc);
        void PrcStyleCB(const PropertyData * pdc);
    private:
        Scheme * mScheme;
    };
}
}
#endif