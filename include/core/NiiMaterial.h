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
#include "NiiShaderFusion.h"

namespace NII
{
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
        ShaderFusion * get(Nidx index);

        /** 按索引,获取支持技术
        @version NIIEngine 3.0.0
        */
        ShaderFusion * getValid(Nidx index);

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
        /// @copydetails Resource::touch
        void touch(bool load = true);

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