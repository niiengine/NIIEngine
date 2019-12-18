/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-5-7

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        计算机辅助立体设计软件(CAD)
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/

#ifndef _NII_FONT_XMLAnalyze_H_
#define _NII_FONT_XMLAnalyze_H_

#include "NiiPreInclude.h"
#include "NiiXmlAnalyze.h"
#include "NiiFont.h"
#include "NiiString.h"

namespace NII
{
    /** 字体元素xml分析
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FontXmlAnalyze : public FontProperty, public XmlAnalyze
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