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
#ifndef _NII_ColourArea_H_
#define _NII_ColourArea_H_

#include "NiiPreInclude.h"
#include "NiiColour.h"

namespace NII
{
    /** ͼ��ר����ɫ����
    @remark
        �Ľ���ɫ����,�м���ɫ����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ColourArea : public UIAlloc
    {
    public:
        ColourArea();
        ColourArea(const Colour & c);
        ColourArea(const Colour & tl, const Colour & tr, const Colour & bl, const Colour & br);

        /** ��ֵ
        @version NIIEngine 3.0.0
        */
        inline void set(const Colour & c) { mTL = mTR = mBL = mBR = c; }

        /** ��ȡ
        @version NIIEngine 3.0.0
        */
        inline void get(Colour & c) { c = mTL;  }

        /** ��ֵ
        @note ��ȸ����������?
        @version NIIEngine 3.0.0
        */
        inline void set(const Colour & tl, const Colour & tr, const Colour & bl, const Colour & br)
        { mTL = tl;mTR = tr; mBL = bl; mBR = br; }

        /** ��ȡ
        @note ��ȸ����������?
        @version NIIEngine 3.0.0
        */
        inline void get(Colour & tl, Colour & tr, Colour & bl, Colour & br)
        { tl = mTL; tr = mTR; bl = mBL; br = mBR; }

        /** ��ȡ����ɫ��
        @version NIIEngine 3.0.0
        */
        ColourArea get(const Rectf & rect) const;

        /** ���� +
        @version nIIEngine 3.0.0
        */
        ColourArea operator+(NIIf factor) const;

        /** ���� - 
        @version nIIEngine 3.0.0
        */
        ColourArea operator-(NIIf factor) const;

        /** ���� *
        @version nIIEngine 3.0.0
        */
        ColourArea operator*(NIIf factor) const;

        /** ���� /
        @version nIIEngine 3.0.0
        */
        ColourArea operator/(NIIf factor) const;

        /** ���� *=
        @version NIIEngine 3.0.0
        */
        ColourArea & operator*=(const ColourArea & o);

        /** ���� =
        @version NIIEngine 3.0.0
        */
        ColourArea & operator=(const ColourArea & o);
        
        /** ���� +
        @version NIIEngine 3.0.0
        */
        ColourArea operator+(const ColourArea & o) const;

        /** ���� -
        @version NIIEngine 3.0.0
        */
        ColourArea operator-(const ColourArea & o) const;

        /** ���� *
        @version NIIEngine 3.0.0
        */
        ColourArea operator*(const ColourArea & o) const;

        /** ���� /
        @version NIIEngine 3.0.0
        */
        ColourArea operator/(const ColourArea & o) const;

        /** ����͸����
        @version NIIEngine 3.0.0
        */
		inline void modulate(NIIf a) { mTL.modulate(a); mTR.modulate(a); mBL.modulate(a); mBR.modulate(a); }

        /** ����͸����
        @version NIIEngine 3.0.0
        */
        inline void setAlpha(NIIf a) { mTL.setAlpha(a); mTR.setAlpha(a); mBL.setAlpha(a); mBR.setAlpha(a); }

        /** ����͸��������
        @version NIIEngine 3.0.0
        */
        void setAlphaFactor(NIIf a);

        /** ����͸����
        @version NIIEngine 3.0.0
        */
        inline void setLeftAlpha(NIIf a) { mTL.setAlpha(a); mBL.setAlpha(a); }

        /** ����͸����
        @version NIIEngine 3.0.0
        */
        inline void setTopAlpha(NIIf a) { mTL.setAlpha(a); mTR.setAlpha(a); }

        /** ����͸����
        @version NIIEngine 3.0.0
        */
        inline void setRightAlpha(NIIf a) { mTR.setAlpha(a); mBR.setAlpha(a); }

        /** ����͸����
        @version NIIEngine 3.0.0
        */
        inline void setBottomAlpha(NIIf a) { mBL.setAlpha(a); mBR.setAlpha(a); }

        /** �Ƿ�ֻ��һ����ɫ
        @version NIIEngine 3.0.0
        */
        inline bool isOneColour() const { return mTL == mTR && mTL == mBL && mTL == mBR;  }

        /** ��ȡ��λ��ɫ
        @version NIIEngine 3.0.0
        */
        Colour get(NIIf x, NIIf y) const;
    public:
        Nid mID;
        Colour mTL;
        Colour mTR;
        Colour mBL;
        Colour mBR;
    };
}
#endif