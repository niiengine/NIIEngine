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

#ifndef _NII_Font_Manager_H_
#define _NII_Font_Manager_H_

#include "NiiPreInclude.h"
#include "NiiResourceManager.h"
#include "NiiFont.h"

namespace NII
{
    /** 字体管理类
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FontManager : public ResourceManager, public Singleton<FontManager>
    {
    public:
        FontManager();
        ~FontManager();

        /** 加载所有已经定义的资源
        @version NIIEngine 3.0.0
        */
        void createAllDefine(const String & pattern, GroupID gid);

        /** 从脚本中创建字体
        @version NIIEngine 3.0.0
        */
        Font * createFromScript(const String & file, GroupID gid);

        /** 创建TTF字体
        @param[in] fid 字体ID
        @param[in] size 字体大小
        @param[in] antialiase 反锯齿
        @param[in] fontfile 字体文件
        @param[in] nativeRes 相对尺寸
        @version NIIEngine 3.0.0
        */
        Font * createFreeType(FontID fid, NIIf size, NCount pdi, bool antialiase,
            const String & fontfile, GroupID gid = GroupUnknow, ScaleMode sm = SM_Unknow,
                const PlaneSizef & nativeRes = PlaneSizef(640.0f, 480.0f));

        /** 创建像素字体
        @param[in] fid 字体ID
        @param[in] imagesrc 图片文件,可以多个用空格分开
        @param[in] nativeRes 相对尺寸
        @version NIIEngine 3.0.0
        */
        Font * createPixmap(FontID fid, const String & imagesrc, GroupID gid = GroupUnknow,
            ScaleMode sm = SM_Unknow, const PlaneSizef & nativeRes = PlaneSizef(640.0f, 480.0f));

        /** 获取字体
        @version NIIEngine 3.0.0
        */
        Font * getFont(FontID fid) const;

        /** 字体是否存在
        @version NIIEngine 3.0.0
        */
        bool isFontExist(FontID fid) const;

        /** 删除字体
        @version NIIEngine 3.0.0
        */
        void destroyFont(FontID fid);

        /** 视图大小改变
        @version NIIEngine 3.0.0
        */
        void notifyView(const PlaneSizef & size);

        /** 字体渲染大小改变
        @version NIIEngine 3.0.0
        */
        void notifySize(const Font * obj);

        /** 设置字体资源组
        @param[in] gid
        @version NIIEngine 3.0.0
        */
		static void setGroup(GroupID gid);

        /** 获取字体资源组
        @version NIIEngine 3.0.0
        */
		static GroupID getGroup();

        /** 获取默认字体
        @version NIIEngine 3.0.0
        */
        Font * getDefault() const;

        /** 写入到 XML 脚本
        @param[in] fid 字体ID
        @param[in] out xml序列
        @version NIIEngine 3.0.0
        */
        void write(FontID fid, XmlSerializer * out, LangID lid = N_PrimaryLang) const;

        /// @copydetails ScriptParserSys::parse
        void parse(DataStream * stream, GroupID gid, PtrList & out);

        /// @copydetails Singleton::getOnly
        static FontManager & getOnly();

        /// @copydetails Singleton::getOnlyPtr
        static FontManager * getOnlyPtr();
    protected:
		/// @copydetails ResourceManager::init
		void init();

        /// 内部方法
        Resource * createImpl(ResourceID rid, GroupID gid, ResourceLoadScheme * ls,
            ResourceResultScheme * rs, const PropertyData * params);

        /// @copydetails ResourceManager::addImpl
        virtual void addImpl(Resource * obj);

        /// @copydetails ResourceManager::removeImpl
        virtual void removeImpl(Resource * obj);
    protected:
        typedef map<FontID, Font *>::type FontMap;
    protected:
        static GroupID ResourceGroup;
		FontMap mFontList;
        Font * mDefault;
    };
}

#endif