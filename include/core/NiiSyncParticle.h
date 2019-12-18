/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-3-7

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        计算机辅助立体设计软件(CAD)
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
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