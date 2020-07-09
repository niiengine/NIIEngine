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

#ifndef _NII_SYNCPARTICLE_H_
#define _NII_SYNCPARTICLE_H_

#include "NiiPreInclude.h"
#include "NiiVector3.h"
#include "NiiColour.h"

namespace NII
{
    /** 同态粒子
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SyncParticle : public ParticleAlloc
    {
        friend class ParticleCtrl;
        friend class ParticleCtrlGeo;
    public:
        SyncParticle();
        SyncParticle(const Vector3f & pos, ParticleCtrl * ctrl, const Colour & c = Colour::White);

        ~SyncParticle();

        /** 设置颜色
        @version NIIEngine 3.0.0
        */
        void setColour(const Colour & c);

        /** 获取颜色
        @version NIIEngine 3.0.0
        */
        const Colour & getColour() const;

        /** 使用控制器的尺寸
        @version NIIEngine 3.0.0
        */
        void inheritSize();

        /** 是否使用控制器的尺寸
        @version NIIEngine 3.0.0
        */
        bool isInheritSize() const;

        /** 设置尺寸
        @version NIIEngine 3.0.0
        */
        void setSize(NIIf width, NIIf height);

        /** 获取宽
        @version NIIEngine 3.0.0
        */
        NIIf getWidth() const;

        /** 获取高
        @version NIIEngine 3.0.0
        */
        NIIf getHeight() const;

        /** 设置位置
        @version NIIEngine 3.0.0
        */
        void setPos(const Vector3f & pos);

        /** 获取位置
        @version NIIEngine 3.0.0
        */
        const Vector3f & getPos() const;

        /** 设置旋转
        @version NIIEngine 3.0.0
        */
        void setRotation(const Radian & rot);

        /** 获取旋转
        @version NIIEngine 3.0.0
        */
        const Radian & getRotation() const;

        /** 是否使用控制器设定好的纹理坐标
        @version NIIEngine 3.0.0
        */
        bool isInheritTexCoord() const;
        
        /** 设置纹理坐标
        @version NIIEngine 3.0.0
        */
        void setTexCoordRect(const Rectf & rect);

        /** 获取纹理坐标
        @version NIIEngine 3.0.0
        */
        const Rectf & getTexCoordRect() const;

        /** 使用控制器设定好的纹理坐标
        @version NIIEngine 3.0.0
        */
        void setCtrlTexCoord(Nidx index);

        /** 获取控制器设定好的纹理坐标
        @version NIIEngine 3.0.0
        */
        Nidx getCtrlTexCoord() const;
    protected:
        ParticleCtrl * mCtrl;
        NIIf mWidth;
        NIIf mHeight;        
        Vector3f mPosition;
        Vector3f mDirection;
        Colour mColour;
        Radian mRotation;
        Rectf mTexCoord;
        Nidx mCtrlTexCoord;
        bool mSizeValid;
        bool mTexCoordValid;
    };
}
#endif