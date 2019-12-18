/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-2-1

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

#ifndef _NII_SHADER_FUSION_H_
#define _NII_SHADER_FUSION_H_

#include "NiiPreInclude.h"
#include "NiiShaderCh.h"
#include "NiiGPUInfo.h"

namespace NII
{
    /** 由多个或单个着色渲染通路合成
    @version NIIEngine 3.0.0 高级api
    */
    class _EngineAPI ShaderFusion : public ExtData, public ShaderAlloc
    {
        friend class Material;
        friend class MaterialManager;
        friend class ShaderCh;
    public:
        /** 通道状态合成
        @version NIIEngine 3.0.0
        */
        class FusionShader : public ShaderAlloc
        {
        public:
            FusionShader();
            ~FusionShader();
        public:
            ShaderStep mStep;       ///< 渲染阶段
            ShaderCh * mSrc;        ///< 原始通道
            ShaderCh * mDest;       ///< 推导通道
        };
        typedef vector<FusionShader *>::type FusionShaderList;
    public:
        ShaderFusion(Material * parent);
        ShaderFusion(Material * parent, const ShaderFusion & o);

        ~ShaderFusion();

        ShaderFusion & operator=(const ShaderFusion & o);

        /** 获取所属材质
        @version NIIEngine 3.0.0
        */
        Material * getParent() const;

        /** 设置辅助名字
        @version NIIEngine 3.0.0
        */
        void setName(const String & name);

        /** 获取辅助名字
        @version NIIEngine 3.0.0
        */
        const String & getName() const;

        /** 设置方案ID
        @version NIIEngine 3.0.0
        */
        void setSchemeID(SchemeID sid);

        /** 获取方案ID
        @version NIIEngine 3.0.0
        */
        SchemeID getSchemeID() const;

        /** 获取资源组
        @version NIIEngine 3.0.0
        */
        GroupID getResourceGroup() const;

        /** 是否有效
        @version NIIEngine 3.0.0
        */
        bool isValid() const;

        /** 是否加载了
        @version NIIEngine 3.0.0
        */
        bool isLoad() const;

        /** 添加支持厂商(显卡)
        @param[in] rule
        @version NIIEngine 3.0.0
        */
        void add(const GpuVendorDetail & rule);

        /** 移去支持厂商(显卡)
        @param[in] rule
        @version NIIEngine 3.0.0
        */
        void remove(const GpuVendorDetail & rule);

        /** 获取支持厂商列表
        @version NIIEngine 3.0.0
        */
        const GpuVendorDetailList & getSupportVendor() const;

        /** 创建新通道
        @version NIIEngine 3.0.0
        */
        ShaderCh * create();

        /** 获取通道
        @param[in] index 下标索引
        @version NIIEngine 3.0.0
        */
        ShaderCh * get(Nidx index);

        /** 获取通道
        @return 名字
        @version NIIEngine 3.0.0
        */
        ShaderCh * get(const String & name);

        /** 移去通道
        @param[in] index 下标索引
        @version NIIEngine 3.0.0
        */
        void remove(Nidx index);

        /** 移去所有通道
        @version NIIEngine 3.0.0
        */
        void removeAll();

        /** 获取通道数量
        @version NIIEngine 3.0.0
        */
        NCount getCount() const;

        /** 获取原始渲染
        @version NIIEngine 3.0.0 高级api
        */
        ShaderChList & getShaderChList();

        /** 获取原始渲染
        @version NIIEngine 3.0.0
        */
        const ShaderChList & getShaderChList() const;

        /** 获取混合渲染
        @version NIIEngine 3.0.0 高级api
        */
        FusionShaderList & getFusionShaderList();

        /** 是否透明度效应
        @version NIIEngine 3.0.0
        */
        bool isTransparent() const;

        /** 是否深度写入
        @version NIIEngine 3.0.0
        */
        bool isDepthWrite() const;

        /** 是否深度测试
        @version NIIEngine 3.0.0
        */
        bool isDepthCheck() const;

        /** 是否深度排序
        @version NIIEngine 3.0.0
        */
        bool isDepthSort() const;

        /** 是否强制深度排序
        @version NIIEngine 3.0.0
        */
        bool isForceDepthSort() const;

        /** 设置阴影投射材质
        @version NIIEngine 3.0.0 高级api
        */
        void setShadowCaster(Material * obj);

        /** 获取阴影投射材质
        @version NIIEngine 3.0.0 高级api
        */
        Material * getShadowCaster() const;

        /** 设置阴影接收材质
        @version NIIEngine 3.0.0 高级api
        */
        void setShadowReceiver(Material * obj);

        /** 获取阴影接收材质
        @version NIIEngine 3.0.0 高级api
        */
        Material * getShadowReceiver() const;

        /** 设置颜色属性
        @version NIIEngine 3.0.0
        */
        void setColour(const ShaderChColour & c);

        /** 设置颜色混合模式
        @version NIIEngine 3..0.0
        */
        void setBlend(const ShaderChBlend & b);

        /** 设置点属性
        @version NIIEngine 3.0.0
        */
        void setPoint(const ShaderChPoint & p);

        /** 设置雾模式
        @version NIIEngine 3.0.0
        */
        void setFog(const ShaderChFog & f);

        /** 设置动态照明
        @version NIIEngine 3.0.0
        */
        void setLight(bool b);

        /** 设置着色类型
        @version NIIEngine 3.0.0
        */
        void setShadeMode(ShadeMode mode);

        /** 设置颜色缓存写入掩码
        @param[in] mark ColourMark 的一个或多个组合
        @version NIIEngine 3.0.0
        */
        void setColourWrite(Nmark mark);

        /** 获取颜色缓存写入掩码
        @version NIIEngine 3.0.0
        */
        Nmark getColourWrite() const;

        /** 设置顶点绕向拣选模式
        @version NIIEngine 3.0.0
        */
        void setCullingMode(CullingMode mode);

        /** 设置系统拣选模式
        @version NIIEngine 3.0.0
        */
        void setSysCullingMode(SysCullingMode mode);

        /** 设置LOD索引
        @version NIIEngine 3.0.0 高级api
        */
        void setLodIndex(Nidx index);

        /** 获取LOD索引
        @version NIIEngine 3.0.0 高级api
        */
        Nidx getLodIndex() const;

        /** 副本
        @param[in] o 副本的宿主
        @version NIIEngine 3.0.0
        */
        ShaderFusion * clone(Material * o);
    protected:
        /** 状态/性质变化了
        @version NIIEngine 3.0.0
        */
        void notify();

        /** 加入加载策略
        @version NIIEngine 3.0.0 高级api
        */
        void plan();

        /** 移出加载策略
        @version NIIEngine 3.0.0 高级api
        */
        void unplan();

        /** 加载
        @version NIIEngine 3.0.0 高级api
        */
        void load();

        /** 卸载
        @version NIIEngine 3.0.0 高级api
        */
        void unload();
    protected:
        /** 检测是否有效
        @param[in] fix 尝试修正
        @version NIIEngine 3.0.0 内部api
        */
        String check(bool fix);

        /** 合成通道
        @version NIIEngine 3.0.0 内部api
        */
        void fusion();
    protected:
        String mName;
        SchemeID mSchemeID;
        Material * mParent;
        Material * mShadowCaster;
        Material * mShadowReceiver;
        FusionShaderList mFusionChList;
        GpuVendorDetailList mVendorList;
        ShaderChList mChList;
        Nui16 mLodIndex;
        Nmark mMark;
    };
}
#endif