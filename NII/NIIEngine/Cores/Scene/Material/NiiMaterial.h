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

#ifndef _NII_MATERIAL_H_
#define _NII_MATERIAL_H_

#include "NiiPreInclude.h"
#include "NiiResource.h"
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
        class Fusion : public ShaderAlloc
        {
        public:
            Fusion();
            ~Fusion();
        public:
            ShaderStep mStep;       ///< 渲染阶段
            ShaderCh * mSrc;        ///< 原始通道
            ShaderCh * mDest;       ///< 推导通道
        };
        typedef vector<Fusion *>::type FusionShaderList;
    public:
        ShaderFusion(Material * parent);
        ShaderFusion(Material * parent, const ShaderFusion & o);

        ~ShaderFusion();

        ShaderFusion & operator=(const ShaderFusion & o);

        /** 获取所属材质
        @version NIIEngine 3.0.0
        */
        inline Material * getParent() const         { return mParent; }

        /** 设置辅助名字
        @version NIIEngine 3.0.0
        */
        inline void setName(const String & name)    { mName = name;  }

        /** 获取辅助名字
        @version NIIEngine 3.0.0
        */
        inline const String & getName() const       { return mName; }

        /** 设置方案ID
        @version NIIEngine 3.0.0
        */
        void setSchemeID(SchemeID sid);

        /** 获取方案ID
        @version NIIEngine 3.0.0
        */
        inline SchemeID getSchemeID() const         { return mSchemeID;  }

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
        inline const GpuVendorDetailList & getSupportVendor() const { return mVendorList;  }

        /** 创建新通道
        @version NIIEngine 3.0.0
        */
        ShaderCh * create();

        /** 获取通道
        @param[in] index 下标索引
        @version NIIEngine 3.0.0
        */
        inline ShaderCh * get(Nidx index)           { N_assert1(index < mChList.size()); return mChList[index]; }

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
        inline NCount getCount() const              { return mChList.size();  }

        /** 获取原始渲染
        @version NIIEngine 3.0.0 高级api
        */
        inline ShaderChList & getShaderChList()     { return mChList;  }

        /** 获取原始渲染
        @version NIIEngine 3.0.0
        */
        inline const ShaderChList & getShaderChList() const { return mChList;  }

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
        bool isDepthEnable() const;

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
        inline void setShadowCaster(Material * obj)     { mShadowCaster = obj;  }

        /** 获取阴影投射材质
        @version NIIEngine 3.0.0 高级api
        */
        inline Material * getShadowCaster() const       { return mShadowCaster;  }

        /** 设置阴影接收材质
        @version NIIEngine 3.0.0 高级api
        */
        inline void setShadowReceiver(Material * obj)   { mShadowReceiver = obj;  }

        /** 获取阴影接收材质
        @version NIIEngine 3.0.0 高级api
        */
        inline Material * getShadowReceiver() const     { return mShadowReceiver;  }

        /** 设置颜色属性
        @version NIIEngine 3.0.0
        */
        void setColour(ShaderCh::SetOperatorType type, const ShaderChColour * refIn);

        /** 设置颜色混合模式
        @version NIIEngine 3..0.0
        */
        void setBlend(ShaderCh::SetOperatorType type, const ShaderChBlend * refIn);

        /** 设置点属性
        @version NIIEngine 3.0.0
        */
        void setPoint(ShaderCh::SetOperatorType type, const ShaderChPoint * refIn);

        /** 设置雾模式
        @version NIIEngine 3.0.0
        */
        void setFog(ShaderCh::SetOperatorType type, const ShaderChFog * refIn);

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
        void setLodIndex(Nidx16 index);

        /** 获取LOD索引
        @version NIIEngine 3.0.0 高级api
        */
        inline Nidx16 getLodIndex() const             { return mLodIndex;  }

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

        /** 关联对象
        @note 如果 ShaderCh 是单独存在的,不使用这个方式
        @param[in] dst
        @param[in] obj
        @version NIIEngine 3.0.0 内部api
        */
        void attach(ShaderCh * obj);

        /** 回收对象自动删除内存
        @note 如果 ShaderCh 是单独存在的,不使用这个方式
        @param[in] dst
        @param[in] obj
        @version NIIEngine 3.0.0 内部api
        */
        void detach(ShaderCh * obj);

        /** 执行回收机制
        @version NIIEngine 3.0.0 内部api
        */
        static void recover();
    protected:
        typedef map<ShaderCh *, ShaderFusionList>::type ShaderChLink;
    protected:
        String mName;
        SchemeID mSchemeID;
        Material * mParent;
        Material * mShadowCaster;
        Material * mShadowReceiver;
        FusionShaderList mFusionChList;
        GpuVendorDetailList mVendorList;
        ShaderChList mChList;
        Nidx16 mLodIndex;
        Nmark mMark;
        static ShaderChLink mChLink;
        static ShaderChList mDetachList;
    };

    /** 材质
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Material : public Resource
    {
        friend class SpaceManager;
        friend class MaterialManager;
    public:
        typedef map<Nidx, ShaderFusion *>::type LodList;
        typedef map<SchemeID, LodList *>::type SchemeFusions;

        struct LessSort
        {
            _EngineAPI bool operator()(const Material * x, const Material * y) const;
        };
    public:
        Material(ResourceID rid, GroupID gid, ResLoadScheme * ls = 0, ResResultScheme * rs = 0, 
            LangID lid = N_PrimaryLang);

        ~Material();

        Material & operator =(const Material & o);

        /** 检测合成混合
        @version NIIEngine 3.0.0
        */
        bool check(bool fix = true);

        /** 创建合成混合
        @version NIIEngine 3.0.0
        */
        ShaderFusion * create();

        /** 删除合成混合
        @param[in] index
        @version NIIEngine 3.0.0
        */
        void destroy(Nidx index);

        /** 获取指定索引合成效果
        @version NIIEngine 3.0.0
        */
        inline ShaderFusion * get(Nidx index)       { N_assert1(index < mFusions.size()); return mFusions[index]; }

        /** 按索引,获取支持技术
        @version NIIEngine 3.0.0
        */
        inline ShaderFusion * getValid(Nidx index)  { N_assert1(index < mValidFusions.size()); return mValidFusions[index]; }

        /** 获取指定名字索引效果
        @version NIIEngine 3.0.0
        */
        ShaderFusion * get(const String & name);

        /** 删除所有混合
        @version NIIEngine 3.0.0
        */
        void destroyAll();

        /** 获取合成效果数量
        @version NIIEngine 3.0.0
        */
        inline NCount getCount() const      { return mFusions.size();  }

        /** 检索支持混合数目
        @version NIIEngine 3.0.0
        */
        inline NCount getValidCount() const { return mValidFusions.size();  }

        /** 获取LOD混合
        @param[in] obj
        @param[in] lod
        @version NIIEngine 3.0.0
        */
        ShaderFusion * getLOD(const GeometryObj * obj = 0, Nidx lod = 0) const;

        /** 获取混合列表
        @version NIIEngine 3.0.0
        */
        inline const ShaderFusionList & getList() const         { return mFusions;  }

        /** 获取有效的混合列表
        @version NIIEngine 3.0.0
        */
        inline const ShaderFusionList & getValidList() const    { return mValidFusions;  }

        /** 设置这个材质的LOD策略
        @version NIIEngine 3.0.0
        */
        void setLodScheme(MaterialLodScheme * obj);

        /** 获取这个材质的LOD策略
        @version NIIEngine 3.0.0
        */
        inline MaterialLodScheme * getLodScheme() const         { return mLodScheme;  }

        /** 获取指定方案的LOD数量
        @version NIIEngine 3.0.0
        */
        NCount getLodCount(SchemeID sid) const;

        /** 为每个技术中每个通道,设置 环境反射颜色 属性
        @param[in] c 3色参
        @version NIIEngine 3.0.0
        */
        void setColour(const ShaderChColour & c);

        /** 为每个技术中每个通道,设置点大小属性
        @version NIIEngine 3.0.0
        */
        void setPoint(const ShaderChPoint & p);

        /** 设置每通道应用的雾模式
        @version NIIEngine 3.0.0
        */
        void setFog(const ShaderChFog & f);

        /** 是否有透明效应
        @version NIIEngine 3.0.0
        */
        bool isTransparent() const;

        /** 设置是否接收阴影
        @version NIIEngine 3.0.0
        */
        void setReceiveShadow(bool b);

        /** 获取是否接收阴影
        @version NIIEngine 3.0.0
        */
        bool isReceiveShadow() const;

        /** 设置透明物是否也产生阴影
        @remark 一般而言,透明物产生的阴影颜色应该和透明物反射的颜色大体相同
        @version NIIEngine 3.0.0
        */
        void setAlphaShadowCast(bool b);

        /** 获取透明物是否也产生阴影
        @remark 一般而言,透明物产生的阴影颜色应该和透明物反射的颜色大体相同
        @version NIIEngine 3.0.0
        */
        bool isAlphaShadowCast() const;

        /** 设置是否动态灯光
        @version NIIEngine 3.0.0
        */
        void setLight(bool b);

        /** 设置灯光描影需要的类型
        @version NIIEngine 3.0.0
        */
        void setShadeMode(ShadeMode mode);

        /** 设置每通道基于`顶点绕组`的拣选模式
        @version NIIEngine 3.0.0
        */
        void setCullingMode(CullingMode mode);

        /** 设置手动拣选模式.由CPU执行而不是硬件
        @version NIIEngine 3.0.0
        */
        void setSysCullingMode(SysCullingMode mode);

        /** 设置颜色缓存写入掩码
        @param[in] mark ColourMark 的一个或多个组合
        @version NIIEngine 3.0.0
        */
        void setColourWrite(Nmark mark);

        /** 副本
        @param[in] rid 新资源ID
        @param[in] gid 新资源组ID
        */
        Material * clone(ResourceID rid, GroupID gid = GroupUnknow) const;
    public:
        /// @copydetails Resource::ref
        void ref(bool load = true);

        /// @copydetails Resource::notify
        void notify();
    protected:
        /// @see Resource::planImpl
        void planImpl();

        /// @see Resource::unplanImpl
        void unplanImpl();

        /// @see Resource::loadImpl
        void loadImpl();

        /// @see Resource:unloadImpl
        void unloadImpl();

        /// @see Resource::calcSize
        NCount calcSize() const;

        /** 关联对象
        @note 如果 ShaderFusion 是单独存在的,不使用这个方式
        @param[in] dst
        @param[in] obj
        @version NIIEngine 3.0.0
        */
        void attach(ShaderFusion * obj);

        /** 回收对象自动删除内存
        @note 如果 ShaderFusion 是单独存在的,不使用这个方式
        @param[in] dst
        @param[in] obj
        @version NIIEngine 3.0.0
        */
        void detach(ShaderFusion * obj);
    protected:
        typedef map<ShaderFusion *, MaterialList>::type ShaderFusionLink;
    protected:
        ShaderFusionList mFusions;
        ShaderFusionList mValidFusions;
        SchemeFusions mSchemeFusions;
        MaterialLodScheme * mLodScheme;
        Nmark mMark;
        String mLastError;
        static ShaderFusionLink mFusionLink;
    };
}
#endif