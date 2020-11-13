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
    /** �Զ������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI CustomQueue : public RenderAlloc
    {
    public:
        CustomQueue(GroupID gid, const String & name = StrUtil::WBLANK);

        virtual ~CustomQueue();
        
        /** ����(����)
        @version NIIEngine 3.0.0
        */
        const String & getName() const          {return mName;}

        /** ��ȡ��Ⱦ��
        @version NIIEngine 3.0.0
        */
        GroupID getRenderGroup() const          {return mRenderGroup;}

        /** ������Ⱦͨ��
        @version NIIEngine 3.0.0
        */
        void setShaderCh(ShaderCh * ch)         {mShaderCh = ch;}

        /** ��ȡ��Ⱦͨ��
        @version NIIEngine 3.0.0
        */
        ShaderCh * getShaderCh() const          { return mShaderCh; }

        /** �Ƿ�������Ӱ
        @version NIIEngine 3.0.0
        */
        void setShadowEnable(bool b)            {mShadowEnable =  b;}

        /** �Ƿ�������Ӱ
        @version NIIEngine 3.0.0
        */
        bool isShadowEnable() const             {return mShadowEnable;}

        /** ִ����Ⱦ
        @version NIIEngine 3.0.0
        */
        virtual void render(RenderGroup * group, RenderPattern * pattern, SpaceManager * dst);
    protected:
        /** ִ����Ⱦʵ��
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void renderImpl(RenderGroup * group, RenderPattern * pattern);
    protected:
        String mName;
        GroupID mRenderGroup;
        ShaderCh * mShaderCh;
        bool mShadowEnable;
    };
    typedef vector<CustomQueue *>::type CustomQueueList;
    
    /** �Զ�����Ⱦ
    @version NIIEngine 3.0.0
    */
    class _EngineAPI CustomRenderQueue : public RenderAlloc
    {
    public:
        CustomRenderQueue(Nid id);
        virtual ~CustomRenderQueue();
        
        /** ��ȡID
        @version NIIEngine 3.0.0
        */
        Nid getID() const                       {return mID;}

        /** ����Զ������
        @note �����ڴ潫����������
        @version NIIEngine 3.0.0
        */
        void add(CustomQueue * obj);

        /** ����Զ������
        @version NIIEngine 3.0.0
        */
        CustomQueue * add(GroupID qid, const String & name);

        /** ��ȡ�Զ������
        @version NIIEngine 3.0.0
        */
        CustomQueue * get(Nidx index);

        /** ��ȥ�Զ������
        @version NIIEngine 3.0.0
        */
        void remove(Nidx index);

        /** ��ȥ�����Զ������
        @version NIIEngine 3.0.0
        */
        void removeAll();

        /** ��ȡ����
        @version NIIEngine 3.0.0
        */
        NCount getCount() const                 {return mList.size();}

        /** ��ȡ�Զ������
        @version NIIEngine 3.0.0 �߼�api
        */
        const CustomQueueList & getList()       {return mList;}
    protected:
        Nid mID;
        CustomQueueList mList;
    };
}
#endif