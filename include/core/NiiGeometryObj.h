/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-5-7

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

#ifndef _NII_GEOMETRYOBJ_H_
#define _NII_GEOMETRYOBJ_H_

#include "NiiPreInclude.h"
#include "NiiGeometryRaw.h"
#include "NiiGpuCustomParam.h"
#include "NiiShadowObj.h"
#include "NiiExtData.h"

namespace NII
{
    /** 几何对象
    @remark 这类对象能渲染
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GeometryObj : virtual public ExtData, public GpuCustomParam
    {
    public:
        enum Mode
        {
            M_IdentityProj  = 0x01,     ///< 最开始的那个投影(单位距阵)
            M_IdentityView  = 0x02,     ///< 最开始的那个视图(单位距阵)
            M_IdentityModel = 0x04,     ///< NII 机制的原点坐标
            M_CustomProj    = 0x08,     ///< 自定义投影距阵
            M_CustomView    = 0x10,     ///< 自定义视图距阵
            M_CustomModel   = 0x20,     ///< 自定义模型距阵
            M_AlphaValid    = 0x40,     ///< 材质决定,透明成分
            M_LightValid    = 0x80,     ///< 灯光状态是否有效,如果渲染UI/Sky这个无效
            M_CastShadow    = 0x100,    ///< 衍生设置
            M_ReceiveShadow = 0x200,    ///< 衍生设置
            M_Debug         = 0x400,    ///< 调试模式
            M_Inverse       = 0x800,    ///< 倒影scale(1, -1, 1)/反转矩阵,需要设置 ExtData 118
            M_Rotate90      = 0x1000,   ///< 旋转90度,相对于几何的原点
            M_Rotate180     = 0x2000,   ///< 旋转180度,相对于几何的原点
            M_Edge          = 0x4000,   ///< 几何边缘,相对于摄像机(视口)

            M_Ext0          = 0x10000,
            M_Ext1          = 0x20000,
            M_Ext2          = 0x40000,
            M_Ext3          = 0x80000,
            M_Ext4          = 0x100000,
            M_Ext5          = 0x200000,
            M_Ext6          = 0x400000,
            M_Ext7          = 0x800000,
            M_Ext8          = 0x1000000,
            M_Ext9          = 0x2000000
        };
    public:
        GeometryObj();
        virtual ~GeometryObj();

        /** 启用模式
        @version NIIEngine 3.0.0
        */
        void on(Mode mode);

        /** 启用模式
        @version NIIEngine 3.0.0
        */
        void off(Mode mode);

        /** 是否启用模式
        @version NIIEngine 3.0.0
        */
        bool isOn(Mode mode) const;

        /** 设置几何LOD
        @remark 如果等级足够高,离当前摄像机足够远,很可能直接变跳过,或者用个颜色像素代替
        @version NIIEngine 3.0.0
        */
        void setGeometryLod(Nidx index);

        /** 获取几何LOD
        @remark 如果等级足够高,离当前摄像机足够远,很可能直接变跳过,或者用个颜色像素代替
        @version NIIEngine 3.0.0
        */
        Nidx getGeometryLod() const;

        /** 设置灯光开始处
        @version NIIEngine 3.0.0
        */
        void setStartLight(Nidx i);

        /** 获取灯光开始处
        @verison NIIEngine 3.0.0
        */
        Nidx getStartLight() const;

        /** 获取几何数据.
        @version NIIEngine 3.0.0
        */
        virtual void getGeometry(GeometryRaw & obj) const = 0;

        /** 获取变换矩阵
        @remark 相对于世界位置
        @version NIIEngine 3.0.0
        */
        virtual void getMatrix(Matrix4f * out, NCount & count) const = 0;

        /** 获取世界变换矩阵数量
        @version NIIEngine 3.0.0
        */
        virtual NCount getMatrixCount() const;

        /** 返回几何的摄象机距离
        @remark 用于排序
        @param[in] cam
        @version NIIEngine 3.0.0
        */
        virtual NIIf distanceSquared(const Camera * cam) const = 0;

        /** 是否投射阴影
        @remark 由于材质/状态/影子投射器的缘故,可能从能投射变成不能投射
        @version NIIEngine 3.0.0
        */
        virtual bool isCastShadow() const;

        /** 是否接收阴影
        @remark 由于材质/状态的缘故,可能从能接收变成不能接收
        @version NIIEngine 3.0.0
        */
        virtual bool isReceiveShadow() const;

        /** 获取影响这个对象的灯光
        @version NIIEngine 3.0.0
        */
        virtual void getLight(LightList & ll) const = 0;

        /** 获取需要使用的材质
        @version NIIEngine 3.0.0
        */
        virtual const Material * getMaterial() const = 0;

        /** 获取实际使用的材质
        @version NIIEngine 3.0.0
        */
        virtual ShaderFusion * getShaderFusion() const;
    protected:
        ShadowObj * mShadow;    ///< 阴影辅助对象
        Nmark mGeometryMark;    ///< 扩展数据
        Nui8 mLodIndex;         ///< 辅助渲染系统处理
        Nui8 mStartLight;       ///< 开始灯光
    };
}
#endif