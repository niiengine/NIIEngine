/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2015-5-7

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

#ifndef _NII_UI_COMMON_H_
#define _NII_UI_COMMON_H_

#include "NiiUIPreInclude.h"
#include "NiiColourArea.h"
#include "NiiRect.h"

namespace NII
{
    static const StateID StateCount;
namespace UI
{
    /** ����ģʽ
    @version NIIEngine 3.0.0
    */
    enum RefreshMode
    {
        RM_Alway,           ///< ���Ǹ���
        RM_Never,           ///< ������
        RM_Visible          ///< �ɼ�ʱ����
    };

    /**
    @version NIIEngine 3.0.0
    */
    enum SizeType
    {
        ST_Unknow,          ///<
        ST_LeftPadding,     ///<
        ST_TopPadding,      ///<
        ST_RightPadding,    ///<
        ST_BottomPadding,   ///<
        ST_Width,           ///<
        ST_Height,          ///<
        ST_X,               ///<
        ST_Y,               ///<
        ST_XOft,            ///<
        ST_YOft             ///<
    };

    /**
    @version NIIEngine 3.0.0
    */
    enum SizeOpType
    {
        SOT_Unknow,
        SOT_Add,
        SOT_Subtract,
        SOT_Multiply,
        SOT_Divide
    };

    /** ��ֱ�Ű�ģʽ
    @version NIIEngine 3.0.0
    */
    enum VLayout
    {
        VL_Top,             ///< 
        VL_Centre,          ///<
        VL_Bottom,          ///<
        VL_Expand,          ///<
        VL_Tile             ///< 
    };

    /** ˮƽ�Ű�ģʽ
    @version NIIEngine 3.0.0
    */
    enum HLayout
    {
        HL_Left,            ///<
        HL_Centre,          ///<
        HL_Right,           ///<
        HL_Expand,          ///<
        HL_Tile             ///< 
    };

    /** ���ִ�ֱ�Ű�ģʽ
    @version NIIEngine 3.0.0
    */
    enum VTextLayout
    {
        VTL_Top,            ///<
        VTL_Centre,         ///<
        VTL_Bottom          ///<
    };

    /** ����ˮƽ�Ű�ģʽ
    @version NIIEngine 3.0.0
    */
    enum HTextLayout
    {
        HTL_Left,           ///< 
        HTL_Right,          ///< 
        HTL_Centre,         ///< 
        HTL_Normal,         ///< 
        HTL_Wrap_Expand,    ///< 
        HTL_Wrap_Left,      ///< 
        HTL_Wrap_Right,     ///< 
        HTL_Wrap_Centre     ///< 
    };

    /**
    @version NIIEngine 3.0.0
    */
    enum FontSizeType
    {
        FST_LineSpace,      ///<
        FST_BaseLine,       ///<
        FST_TextEntent      ///<
    };

    typedef Vector2<RelPos> RelVector2;
    typedef PlaneSize<RelPos> RelPlaneSize;

    inline Vector2f abs(const RelVector2 & pos, const PlaneSizef & base, bool pixelAlign = true)
    {
        return Vector2f(pos.x.abs(base.mWidth, pixelAlign), pos.y.abs(base.mHeight, pixelAlign));
    }

    inline PlaneSizef abs(const RelPlaneSize & pos, const PlaneSizef & base, bool pixelAlign = true)
    {
        return PlaneSizef(pos.mWidth.abs(base.mWidth, pixelAlign), pos.mHeight.abs(base.mHeight, pixelAlign));
    }

    inline Rectf abs(const RelRect & pos, const PlaneSizef & base, bool pixelAlign = true)
    {
        return Rectf(
            pos.mLeft.abs(base.mWidth, pixelAlign),
            pos.mTop.abs(base.mHeight, pixelAlign),
            pos.mRight.abs(base.mWidth, pixelAlign),
            pos.mBottom.abs(base.mHeight, pixelAlign));
    }

}
}
#endif