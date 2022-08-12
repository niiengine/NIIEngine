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
        typedef map<FontID, Font *>::type FontMap;
    public:
        FontManager();
        ~FontManager();

        /** 加载所有已经定义的资源
        @version NIIEngine 3.0.0
        */
        void createDefine(const String & pattern, GroupID gid);

        /** 从脚本中创建字体
        @version NIIEngine 3.0.0
        */
        Font * create(const String & file, GroupID gid);

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
        void notifyView(const PlaneSizei & size);

        /** 字体渲染大小改变
        @version NIIEngine 3.0.0
        */
        void notifySize(const Font * obj);

        /** 设置字体资源组
        @param[in] gid
        @version NIIEngine 3.0.0
        */
        static inline void setGroup(GroupID gid){ ResourceGroup = gid;}

        /** 获取字体资源组
        @version NIIEngine 3.0.0
        */
        static inline GroupID getGroup()        { return ResourceGroup;}

        /** 获取默认字体
        @version NIIEngine 3.0.0
        */
        Font * getDefault() const               { return mDefault;}
        
        /** 获取列表 
        @version NIIEngine 3.0.0
        */
        const FontMap & getList() const         { return mFontList;}

        /** 写入到 XML 脚本
        @param[in] fid 字体ID
        @param[in] out xml序列
        @version NIIEngine 3.0.0
        */
        void write(FontID fid, XmlSerializer * out, LangID lid = N_PrimaryLang) const;

        /// @copydetails ScriptParserSys::parse
        void parse(DataStream * stream, GroupID gid, PtrList & out);
    protected:
        /// @copydetails ResourceManager::init
        void init();

        /// 内部方法
        Resource * createImpl(ResourceID rid, GroupID gid, ResLoadScheme * ls,
            ResResultScheme * rs, const PropertyData * params);

        /// @copydetails ResourceManager::addImpl
        virtual void addImpl(Resource * obj);

        /// @copydetails ResourceManager::removeImpl
        virtual void removeImpl(Resource * obj);
    protected:
        static GroupID ResourceGroup;
        FontMap mFontList;
        Font * mDefault;
    };
}

#endif