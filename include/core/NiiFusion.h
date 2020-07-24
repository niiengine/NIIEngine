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

#ifndef _NII_Fusion_H_
#define _NII_Fusion_H_

#include "NiiPreInclude.h"
#include "NiiTexture.h"

namespace NII
{
    /** 帧合成对象
    @version NIIEngine 3.0.0
     */
    class _EngineAPI Fusion : public Resource
    {
    public:
        typedef vector<FrameFusionShader *>::type FrameList;
    public:
        Fusion(ResourceID rid, GroupID gid, 
            ResLoadScheme * ls = 0, ResResultScheme * rs = 0, LangID lid = N_PrimaryLang);
        ~Fusion();

        /** 创建合成帧
        @param[out] obj 创建合成帧
        @version NIIEngine 3.0.0
        */
        FrameFusionShader * create();

        /** 删除指定索引合成帧
        @param[in] index 指定索引
        @version NIIEngine 3.0.0
        */
        void destroy(Nidx index);

        /** 删除所有合成帧
        @version NIIEngine 3.0.0
        */
        void destroyAll();

        /** 获取合成帧
        @version NIIEngine 3.0.0
        */
        FrameFusionShader * get(Nidx index) const;

        /** 合成帧数量
        @version NIIEngine 3.0.0
        */
        NCount getCount() const;

        /** 获取合成帧数量
        @version NIIEngine 3.0.0
        */
        const FrameList & getList() const;

        /** 获取支持的合成帧数量
        @version NIIEngine 3.0.0
        */
        NCount getValidCount() const;

        /** 获取当前支持的混合
        @version NIIEngine 3.0.0
        */
        const FrameList & geValidList() const;

        /** 获取支持的合成帧.
        @version NIIEngine 3.0.0
        */
        FrameFusionShader * getValid(Nidx idx) const;

        /** 获取支持合成帧
        @param[in] sid 方案ID
        @version NIIEngine 3.0.0
        */
        FrameFusionShader * getValid(SchemeID sid = 0) const;

        /** 获取渲染纹理
        @param[in] name
        @param[in] multiIdx
        */
        Texture * getTexture(const String & name, Nidx multiIdx) const;

        /** 获取渲染对象
        @version NIIEngine 3.0.0
        */
        FrameObj * getFrameObj(const String & name) const;
    protected:
        /// @see Resource::loadImpl
        void loadImpl();

        /// @see Resource::unloadImpl
        void unloadImpl();

        /// @see Resource::calcSize
        NCount calcSize() const;
    private:
        typedef map<String, Texture *>::type TextureList;
        typedef map<String, MultiTextureFrame *>::type MultiTexFrameList;

        FrameList mFusions;
        FrameList mValidFusions;
        TextureList mTextureList;
        MultiTexFrameList mMultiTexFrameList;
        bool mCompilationRequired;
    };
}
#endif