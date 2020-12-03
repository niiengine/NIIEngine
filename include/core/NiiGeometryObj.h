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

#ifndef _NII_GEOMETRYOBJ_H_
#define _NII_GEOMETRYOBJ_H_

#include "NiiPreInclude.h"
#include "NiiGeometryRaw.h"
#include "NiiGpuCustomParam.h"
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
        void on(Mode mode)                      { mGeometryMark |= mode;  }

        /** 启用模式
        @version NIIEngine 3.0.0
        */
        void off(Mode mode)                     { mGeometryMark &= ~mode;  }

        /** 是否启用模式
        @version NIIEngine 3.0.0
        */
        bool isOn(Mode mode) const              { return mGeometryMark & mode; }

        /** 设置几何LOD
        @remark 如果等级足够高,离当前摄像机足够远,很可能直接变跳过,或者用个颜色像素代替
        @version NIIEngine 3.0.0
        */
        void setGeometryLod(Nidx16 index)       { mLodIndex = index;  }

        /** 获取几何LOD
        @remark 如果等级足够高,离当前摄像机足够远,很可能直接变跳过,或者用个颜色像素代替
        @version NIIEngine 3.0.0
        */
        Nidx16 getGeometryLod() const           { return mLodIndex; }

        /** 设置灯光开始索引
        @version NIIEngine 3.0.0
        */
        void setLightBegin(Nidx16 i)            { mLightBegin = i; }

        /** 获取灯光开始索引
        @verison NIIEngine 3.0.0
        */
        Nidx16 getLightBegin() const            { return mLightBegin; }

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
        Nmark mGeometryMark;    ///< 扩展数据
        Nidx16 mLodIndex;       ///< 辅助渲染系统处理
        Nidx16 mLightBegin;     ///< 开始灯光
    };
}
#endif