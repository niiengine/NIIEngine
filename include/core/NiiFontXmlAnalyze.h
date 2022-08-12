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

#ifndef _NII_FONT_XMLAnalyze_H_
#define _NII_FONT_XMLAnalyze_H_

#include "NiiPreInclude.h"
#include "NiiXmlAnalyzeManager.h"
#include "NiiFont.h"
#include "NiiString.h"

namespace NII
{
    /** 字体元素xml分析
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FontXmlAnalyze : public XmlAnalyze
    {
    public:
        FontXmlAnalyze(LangID lid = N_PrimaryLang, FontType type = FT_TrueType);
        ~FontXmlAnalyze();

        /** 获取解析字体对象
        @version NIIEngine 3.0.0
        */
        const FontList & getObjectList() const;

        /// @copydetails XmlAnalyze::getXSD
        const String & getXSD() const;

        /// @copydetails XmlAnalyze::getUnit
        XmlUnitID getUnit() const;

        /// @copydetails XmlAnalyze::getGroup
        GroupID getGroup() const;

        /** 输出到Xml序列
        @version NIIEngine 3.0.0
        */
        void write(XmlSerializer * out, const Font * obj) const;
    protected:
        /// @copydetails XmlAnalyze::PrcTagUnitCB
        void PrcTagUnitCB(const PropertyData * pdc);

        /** 处理字符定义
        @version NIIEngine 3.0.0
        */
        void PrcDefineCB(const PropertyData * pdc);

        void writeTrueType(XmlSerializer * out, const Font * src) const;

        void writePixel(XmlSerializer * out, const Font * src) const;
    private:
        Font * mCurrentFont;
        FontList mFontList;
        FontType mFontType;
    };
}
#endif