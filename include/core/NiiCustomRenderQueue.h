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

#ifndef _NII_CUSTOM_RENDER_QUEUE_H_
#define _NII_CUSTOM_RENDER_QUEUE_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 自定义队列
    @version NIIEngine 3.0.0
    */
    class _EngineAPI CustomQueue : public RenderAlloc
    {
    public:
        CustomQueue(GroupID gid, const String & name = StrUtil::WBLANK);

        virtual ~CustomQueue();
        
        /** 名字(辅助)
        @version NIIEngine 3.0.0
        */
        const String & getName() const          {return mName;}

        /** 获取渲染组
        @version NIIEngine 3.0.0
        */
        GroupID getRenderGroup() const          {return mRenderGroup;}

        /** 设置渲染通道
        @version NIIEngine 3.0.0
        */
        void setShaderCh(ShaderCh * ch)         {mShaderCh = ch;}

        /** 获取渲染通道
        @version NIIEngine 3.0.0
        */
        ShaderCh * getShaderCh() const          { return mShaderCh; }

        /** 是否启动阴影
        @version NIIEngine 3.0.0
        */
        void setShadowEnable(bool b)            {mShadowEnable =  b;}

        /** 是否启动阴影
        @version NIIEngine 3.0.0
        */
        bool isShadowEnable() const             {return mShadowEnable;}

        /** 执行渲染
        @version NIIEngine 3.0.0
        */
        virtual void render(RenderGroup * group, RenderPattern * pattern, SpaceManager * dst);
    protected:
        /** 执行渲染实现
        @version NIIEngine 3.0.0 高级api
        */
        virtual void renderImpl(RenderGroup * group, RenderPattern * pattern);
    protected:
        String mName;
        GroupID mRenderGroup;
        ShaderCh * mShaderCh;
        bool mShadowEnable;
    };
    typedef vector<CustomQueue *>::type CustomQueueList;
    
    /** 自定义渲染
    @version NIIEngine 3.0.0
    */
    class _EngineAPI CustomRenderQueue : public RenderAlloc
    {
    public:
        CustomRenderQueue(Nid id);
        virtual ~CustomRenderQueue();
        
        /** 获取ID
        @version NIIEngine 3.0.0
        */
        Nid getID() const                       {return mID;}

        /** 添加自定义队列
        @note 参数内存将由这个类管理
        @version NIIEngine 3.0.0
        */
        void add(CustomQueue * obj);

        /** 添加自定义队列
        @version NIIEngine 3.0.0
        */
        CustomQueue * add(GroupID qid, const String & name);

        /** 获取自定义队列
        @version NIIEngine 3.0.0
        */
        CustomQueue * get(Nidx index);

        /** 移去自定义队列
        @version NIIEngine 3.0.0
        */
        void remove(Nidx index);

        /** 移去所有自定义队列
        @version NIIEngine 3.0.0
        */
        void removeAll();

        /** 获取数量
        @version NIIEngine 3.0.0
        */
        NCount getCount() const                 {return mList.size();}

        /** 获取自定义队列
        @version NIIEngine 3.0.0 高级api
        */
        const CustomQueueList & getList()       {return mList;}
    protected:
        Nid mID;
        CustomQueueList mList;
    };
}
#endif