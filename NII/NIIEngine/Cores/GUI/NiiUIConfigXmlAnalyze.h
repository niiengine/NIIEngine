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
#ifndef _NII_UI_ConfigXmlAnalyze_H_
#define _NII_UI_ConfigXmlAnalyze_H_

#include "NiiUIPreInclude.h"
#include "NiiXmlAnalyzeManager.h"

namespace NII
{
namespace UI
{
    /// UI≈‰÷√Xml∑÷Œˆ
    class _EngineAPI ConfigXmlAnalyze : public XmlAnalyze
    {
    public:
        enum ResourceType
        {
            RT_Font,
            RT_Scheme,
            RT_Style,
            RT_Layout,
            RT_Script,
            RT_PixelGroup,
            RT_Other
        };
    public:
        ConfigXmlAnalyze(LangID lid = N_PrimaryLang);
        ~ConfigXmlAnalyze();

        void initModule() const;

        void initResource() const;

        void loadResource() const;

        void execInitExec() const;

        void executeTerminateExec() const;

        inline const String & getInitExec() const       { return mInitExec; }

        inline const String & getTerminateExec() const  { return mTerminateExec; }

        /// @copydetails XmlAnalyze::getXSD
        const String & getXSD() const;

        /// @copydetails XmlAnalyze::getUnit
        XmlUnitID getUnit() const;

        /// @copydetails XmlAnalyze::getGroup
        GroupID getGroup() const;
    private:
        struct ResPath
        {
            GroupID mGroup;
            String directory;
        };

        struct ResGroup
        {
            ResourceType mType;
            GroupID mGroup;
        };

        struct AutoLoadRes
        {
            String mType;
            ResourceType mResourceType;
            GroupID mGroup;
            String mFilePattern;
        };

        /// @copydetails ConfigXmlAnalyze::PrcTagUnitCB
        void PrcTagUnitCB(const PropertyData * pdc);

        void PrcLogCB(const PropertyData * pdc);
        void PrcAutoLoadCB(const PropertyData * pdc);
        void PrcPathCB(const PropertyData * pdc);
        void PrcResGroupCB(const PropertyData * pdc);
        void PrcScriptCB(const PropertyData * pdc);
        void PrcXmlPatternCB(const PropertyData * pdc);
        void PrcImageCodecCB(const PropertyData * pdc);
        void PrcDefaultTooltipCB(const PropertyData * pdc);
        void PrcDefaultFontCB(const PropertyData * pdc);
        void PrcDefaultCursorCB(const PropertyData * pdc);
    private:
        typedef vector<ResPath>::type ResPathList;
        typedef vector<ResGroup>::type ResGroupList;
        typedef vector<AutoLoadRes>::type AutoLoadResList;

        String mLogFile;
        LogLevel mLogLevel;
        String mXmlPattern;
        String mImageCodec;
        FontID mFont;
        PixelBufferID mCursorView;
        FactoryID mTooltipFactoryID;
        String mInitExec;
        String mTerminateExec;
        ResPathList mResPathList;
        ResGroupList mResGroupList;
        AutoLoadResList mAutoLoad;
    };
}
}
#endif