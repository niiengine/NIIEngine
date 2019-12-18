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

#ifndef _NII_PixmapFont_H_
#define _NII_PixmapFont_H_

#include "NiiPreInclude.h"
#include "NiiFont.h"

namespace NII
{
    /** ���ؼ�����
    @version NIIEngine 3.0.0
    */
    class PixmapFont : public Font
    {
    public:
        PixmapFont(FontID fid, ResourceID rid, GroupID gid = GroupUnknow,
            ResourceLoadScheme * ls = 0, ResourceResultScheme * rs = 0, LangID lid = N_PrimaryLang);
        PixmapFont(FontID fid, const String & src, ResourceID rid, GroupID gid = GroupUnknow,
            ResourceLoadScheme * ls = 0, ResourceResultScheme * rs = 0, ScaleMode mode = SM_Unknow,
                const PlaneSizef & nativeRes = PlaneSizef(640.0f, 480.0f), LangID lid = N_PrimaryLang);
        ~PixmapFont();

        /** �����ַ�
        @param[in] texidx ��������,��Ӧ�������õ���������
        @param[in] value �ַ���ֵ
        @param[in] coord ��������
        @param[in] oft ����λ��ƫ��
        @param[in] extent ���
        @version NIIEngine 3.0.0
        */
        void defineCodePoint(Nidx texidx, CodePoint value, const Rectf & coord,
            const Vector2f & oft, NIIf extent);

        /** ��������ID
        @note �����������õ�ǰ����
        @version NIIEngine 3.0.0 �߼�api
        */
        void setTexture(ResourceID * rid, NCount count);

        /** ��ȡ����ID
        @version NIIEngine 3.0.0 �߼�api
        */
        void getTexture(TextureList & out) const;
    protected:
        /// @copydetails Resource::loadImpl
        void loadImpl();

        /// @copydetails Resource::unloadImpl
        void unloadImpl();

        /// @copydetails Resource::calcSize
        NCount calcSize() const;
    protected:
        /// @copydetails PropertyCmdObj::init
        bool init(PropertyCmdSet * dest);

        /// @copydetails Font::rasterCodePointImpl
        void rasterCodePointImpl(CodePoint start, CodePoint end);

        /// @copydetails Font::reflushCodePointImpl
        void reflushCodePointImpl();

        /// @copydetails Font::getCodePointWidth
        NIIf getCodePointWidth(const FontGlyph * value) const;

        /// @copydetails Font::getCodePointHeight
        NIIf getCodePointHeight(const FontGlyph * value) const;

        /// @copydetails Font::getCodePointExtentImpl
        NIIf getCodePointExtentImpl(const FontGlyph * value) const;
    protected:
        NIIf mLastXScale;
        NIIf mLastYScale;
    };
}
#endif