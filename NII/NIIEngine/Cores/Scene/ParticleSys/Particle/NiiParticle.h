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

#ifndef _NII_Particle_H_
#define _NII_Particle_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 单独粒子
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Particle : public ParticleAlloc
    {
        friend class ParticleSpace;
    public:
        /** 粒子类型
        @version NIIEngine 3.0.0
        */
        enum Type
        {
            T_Alone,
            T_Glint,
            T_Spread
        };

        Particle();

        /** 设置效果控制
        @version NIIEngine 3.0.0 高级api
        */
        inline void setEffect(ParticleEffect * eff)         { mEffect = eff; }

        /** 获取效果控制器
        @version NIIEngine 3.0.0 高级api
        */
        inline ParticleEffect * getEffect() const           { return mEffect; }

        /** 设置是否自动产生的
        @version NIIEngine 3.0.0
        */
        inline void setAuto(bool b)                         { mAuto = b; }

        /** 获取是否自动产生的
        @version NIIEngine 3.0.0
        */
        inline bool isAuto() const                          { return mAuto; }
        
        /** 设置大小
        @version NIIEngine 3.0.0
        */
        void setSize(NIIf width, NIIf height);

        /** 获取宽
        @version NIIEngine 3.0.0
        */
        inline NIIf getWidth() const                        { return mWidth;  }

        /** 获取高
        @version NIIEngine 3.0.0
        */
        inline NIIf getHeight() const                       { return mHeight;  }

        /** 是否继承大小
        @version NIIEngine 3.0.0
        */
        inline void inheritSize(bool b)                     { mInheritSize = b; }

        /** 是否继承大小
        @version NIIEngine 3.0.0
        */
        inline bool isInheritSize() const                   { return mInheritSize;  }

        /** 获取控制器的宽度
        @version NIIEngine 3.0.0
        */
        NIIf getCtrlWidth() const;
        
        /** 获取控制器的高度
        @version NIIEngine 3.0.0
        */
        NIIf getCtrlHeight() const;
        
        /** 设置颜色
        @version NIIEngine 3.0.0
        */
        inline void setColour(const Colour & c)             { mColour = c; }

        /** 获取颜色
        @version NIIEngine 3.0.0
        */
        inline const Colour & getColour() const             { return mColour;}

        /** 设置旋转
        @version NIIEngine 3.0.0
        */
        void setRotation(const Radian & r);

        /** 获取旋转
        @version NIIEngine 3.0.0
        */
        inline const Radian & getRotation() const           { return mRotation;  }

        /** 设置旋转因子
        @version NIIEngine 3.0.0
        */
        inline void setRotationFactor(NIIf f)               { mRotFactor = f; }

        /** 获取旋转因子
        @version NIIEngine 3.0.0
        */
        inline NIIf getRotationFactor() const               { return mRotFactor.getValue(); }

        /** 设置控制系统
        @version NIIEngine 3.0.0 高级api
        */
        inline void setCtrl(ParticleSpace * space)          { mCtrl = space; }
    public:
        Colour mColour;
        Vector3f mPosition;
        Vector3f mDirection;
        Radian mRotation;
        TimeDurMS mTotalTTL;
        TimeDurMS mRemainTTL;
    protected:
        ParticleSpace * mCtrl;
        ParticleEffect * mEffect;
        Type mType;
        NIIf mWidth;
        NIIf mHeight;
        Radian mRotFactor;
        bool mInheritSize;
        bool mAuto;
    };
    typedef list<Particle *>::type ParticleList;
}
#endif