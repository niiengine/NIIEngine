/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2014-5-7

�ı�����: utf-8

������˾: �������ǿƼ����޹�˾

�������: ����������

��̷��: ͳ��ʽ

����ģʽ: �ֲ�ʽ

�ڲ��ɷ�: UI���� ������� ��Ƶ���� ������� �¼���������(��ɢ�����)

��Ҫ�ɷ�: c++(80%) c(20%)

��;: ����ϵͳ����(��Χ����ϵͳ�ں�api)
      ��άӦ�����
        �������������������(CAD)
        ������Ϣϵͳ���(GIS)
        ��Ӱ���������ع����
        ������Ϸ���

ƫ����;: ������Ϸ���

��ҳ: www.niiengine.com ��������: niiengine@gmail.com OR niiengine@163.com

��Ȩ��ʽ:��ҵ��Ȩ(www.niiengine.com/license)(3��)
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
        FontManager();
        ~FontManager();

        /** ���������Ѿ��������Դ
        @version NIIEngine 3.0.0
        */
        void createAllDefine(const String & pattern, GroupID gid);

        /** �ӽű��д�������
        @version NIIEngine 3.0.0
        */
        Font * createFromScript(const String & file, GroupID gid);

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
        void notifyView(const PlaneSizef & size);

        /** ������Ⱦ��С�ı�
        @version NIIEngine 3.0.0
        */
        void notifySize(const Font * obj);

        /** ����������Դ��
        @param[in] gid
        @version NIIEngine 3.0.0
        */
		static void setGroup(GroupID gid);

        /** ��ȡ������Դ��
        @version NIIEngine 3.0.0
        */
		static GroupID getGroup();

        /** ��ȡĬ������
        @version NIIEngine 3.0.0
        */
        Font * getDefault() const;

        /** д�뵽 XML �ű�
        @param[in] fid ����ID
        @param[in] out xml����
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

        /// �ڲ�����
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