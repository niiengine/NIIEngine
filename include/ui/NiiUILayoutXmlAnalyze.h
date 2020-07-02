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

#ifndef _NII_UILayoutXmlAnalyze_H_
#define _NII_UILayoutXmlAnalyze_H_

#include "NiiUIPreInclude.h"
#include "NiiUIWidgetManager.h"
#include "NiiXmlAnalyze.h"
#include "NiiUILayoutProperty.h"

namespace NII
{
namespace UI
{
    /**
    @version NIIEngine 3.0.0
    */
    typedef bool PropertyCallback(Widget * widget, PropertyID pid, const String & pvalue, void * custom);

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI UILayoutXmlAnalyze : public UILayoutProperty, public XmlAnalyze
    {
    public:
        UILayoutXmlAnalyze(LangID lid = N_PrimaryLang, PropertyCallback * callback = 0, void * userdata = 0);
        virtual ~UILayoutXmlAnalyze();

        /// @copydetails XmlAnalyze::getXSD
        virtual const String & getXSD() const;

        /// @copydetails XmlAnalyze::getUnit
        virtual XmlUnitID getUnit() const;

        /// @copydetails XmlAnalyze::getGroup
        virtual GroupID getGroup() const;

        /// @copydetails XmlAnalyze::onData
        virtual void onData(const String & text);

        /** 
        @version NIIEngine 3.0.0
        */
        Widget * getRootWindow() const;
    private:
        void operator=(const UILayoutXmlAnalyze &) {}

        void onBegin(XmlAnalyze * parent, PropertyID pid, const PropertyData * pdc);

        /** 处理标签
        @param[in] own 父标签
        @param[in] pid 属性ID
        @version NIIEngine 3.0.0
        */
        void onEnd(XmlAnalyze * parent, PropertyID pid, const PropertyData * pdc);

        /// @copydetails XmlAnalyze::PrcTagUnitCB
        void PrcTagUnitCB(const PropertyData * pdc);

        /** 
        @version NIIEngine 3.0.0
        */
        void PrcWidgetCB(const PropertyData * pdc);

        /** 
        @version NIIEngine 3.0.0
        */
        void PrcAutoWidgetCB(const PropertyData * pdc);

        /** 
        @version NIIEngine 3.0.0
        */
        void PrcUserDataCB(const PropertyData * pdc);

        /** 
        @version NIIEngine 3.0.0
        */
        void PrcPropertyCB(const PropertyData * pdc);

        /** 
        @version NIIEngine 3.0.0
        */
        void PrcLayoutImportCB(const PropertyData * pdc);

        /** 
        @version NIIEngine 3.0.0
        */
        void PrcEventCB(const PropertyData * pdc);

        /**
        @version NIIEngine 3.0.0
        */
        void PrcWidgetEndCB();

        /** 
        @version NIIEngine 3.0.0
        */
        void PrcAutoWidgetEnd();

        /**
        @version NIIEngine 3.0.0
        */
        void PrcUserDataEndCB();

        /**
        @version NIIEngine 3.0.0
        */
        void PrcPropertyEnd();

        /** 
        @version NIIEngine 3.0.0
        */
        void cleanBuild();
    private:
        typedef std::pair<Widget *, bool> WidgetObj;
        typedef std::vector<WidgetObj> WidgetList;
    private:
        Widget * mRootWindow;
        void * mCustomData;
        WidgetList mWidgetList;
        PropertyCallback * mPropertyCB;
        PropertyID mLastProperty;
        String mLastPropertyValue;
    };
}
}
#endif