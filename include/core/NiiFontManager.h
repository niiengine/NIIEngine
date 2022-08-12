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
    /** ���������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FontManager : public ResourceManager, public Singleton<FontManager>
    {
    public:
        typedef map<FontID, Font *>::type FontMap;
    public:
        FontManager();
        ~FontManager();

        /** ���������Ѿ��������Դ
        @version NIIEngine 3.0.0
        */
        void createDefine(const String & pattern, GroupID gid);

        /** �ӽű��д�������
        @version NIIEngine 3.0.0
        */
        Font * create(const String & file, GroupID gid);

        /** ����TTF����
        @param[in] fid ����ID
        @param[in] size �����С
        @param[in] antialiase �����
        @param[in] fontfile �����ļ�
        @param[in] nativeRes ��Գߴ�
        @version NIIEngine 3.0.0
        */
        Font * createFreeType(FontID fid, NIIf size, NCount pdi, bool antialiase,
            const String & fontfile, GroupID gid = GroupUnknow, ScaleMode sm = SM_Unknow,
                const PlaneSizef & nativeRes = PlaneSizef(640.0f, 480.0f));

        /** ������������
        @param[in] fid ����ID
        @param[in] imagesrc ͼƬ�ļ�,���Զ���ÿո�ֿ�
        @param[in] nativeRes ��Գߴ�
        @version NIIEngine 3.0.0
        */
        Font * createPixmap(FontID fid, const String & imagesrc, GroupID gid = GroupUnknow,
            ScaleMode sm = SM_Unknow, const PlaneSizef & nativeRes = PlaneSizef(640.0f, 480.0f));

        /** ��ȡ����
        @version NIIEngine 3.0.0
        */
        Font * getFont(FontID fid) const;

        /** �����Ƿ����
        @version NIIEngine 3.0.0
        */
        bool isFontExist(FontID fid) const;

        /** ɾ������
        @version NIIEngine 3.0.0
        */
        void destroyFont(FontID fid);

        /** ��ͼ��С�ı�
        @version NIIEngine 3.0.0
        */
        void notifyView(const PlaneSizei & size);

        /** ������Ⱦ��С�ı�
        @version NIIEngine 3.0.0
        */
        void notifySize(const Font * obj);

        /** ����������Դ��
        @param[in] gid
        @version NIIEngine 3.0.0
        */
        static inline void setGroup(GroupID gid){ ResourceGroup = gid;}

        /** ��ȡ������Դ��
        @version NIIEngine 3.0.0
        */
        static inline GroupID getGroup()        { return ResourceGroup;}

        /** ��ȡĬ������
        @version NIIEngine 3.0.0
        */
        Font * getDefault() const               { return mDefault;}
        
        /** ��ȡ�б� 
        @version NIIEngine 3.0.0
        */
        const FontMap & getList() const         { return mFontList;}

        /** д�뵽 XML �ű�
        @param[in] fid ����ID
        @param[in] out xml����
        @version NIIEngine 3.0.0
        */
        void write(FontID fid, XmlSerializer * out, LangID lid = N_PrimaryLang) const;

        /// @copydetails ScriptParserSys::parse
        void parse(DataStream * stream, GroupID gid, PtrList & out);
    protected:
        /// @copydetails ResourceManager::init
        void init();

        /// �ڲ�����
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