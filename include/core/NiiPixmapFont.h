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
            ResLoadScheme * ls = 0, ResResultScheme * rs = 0, LangID lid = N_PrimaryLang);
        PixmapFont(FontID fid, const String & src, ResourceID rid, GroupID gid = GroupUnknow,
            ResLoadScheme * ls = 0, ResResultScheme * rs = 0, ScaleMode mode = SM_Unknow,
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
        bool initCmd(PropertyCmdSet * dest);

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