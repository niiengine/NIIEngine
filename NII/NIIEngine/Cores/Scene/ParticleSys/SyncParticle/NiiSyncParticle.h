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
        inline void setColour(const Colour & colour)        { mColour = colour; }

        /** 获取颜色
        @version NIIEngine 3.0.0
        */
        inline const Colour & getColour() const             { return mColour; }

        /** 使用控制器的尺寸
        @version NIIEngine 3.0.0
        */
        inline void inheritSize()                           { mSizeValid = false;  }

        /** 是否使用控制器的尺寸
        @version NIIEngine 3.0.0
        */
        inline bool isInheritSize() const                   { return !mSizeValid; }

        /** 设置尺寸
        @version NIIEngine 3.0.0
        */
        void setSize(NIIf width, NIIf height);

        /** 获取宽
        @version NIIEngine 3.0.0
        */
        inline NIIf getWidth() const                        { return mWidth; }

        /** 获取高
        @version NIIEngine 3.0.0
        */
        inline NIIf getHeight() const                       { return mHeight; }

        /** 设置位置
        @version NIIEngine 3.0.0
        */
        inline void setPos(const Vector3f & pos)            { mPosition = pos; }

        /** 获取位置
        @version NIIEngine 3.0.0
        */
        inline const Vector3f & getPos() const              { return mPosition; }

        /** 设置旋转
        @version NIIEngine 3.0.0
        */
        void setRotation(const Radian & rot);

        /** 获取旋转
        @version NIIEngine 3.0.0
        */
        inline const Radian & getRotation() const           { return mRotation; }

        /** 是否使用控制器设定好的纹理坐标
        @version NIIEngine 3.0.0
        */
        inline bool isInheritTexCoord() const               { return !mTexCoordValid; }
        
        /** 设置纹理坐标
        @version NIIEngine 3.0.0
        */
        inline void setTexCoordRect(const Rectf & rect)     { mTexCoord = rect; mTexCoordValid = true; }

        /** 获取纹理坐标
        @version NIIEngine 3.0.0
        */
        inline const Rectf & getTexCoordRect() const        { return mTexCoord;  }

        /** 使用控制器设定好的纹理坐标
        @version NIIEngine 3.0.0
        */
        inline void setCtrlTexCoord(Nidx idx)             { mCtrlTexCoord = idx; mTexCoordValid = false; }

        /** 获取控制器设定好的纹理坐标
        @version NIIEngine 3.0.0
        */
        inline Nidx getCtrlTexCoord() const                 { return mCtrlTexCoord; }
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