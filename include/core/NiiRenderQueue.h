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

#ifndef _NII_RenderQueue_H_
#define _NII_RenderQueue_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 渲染队列组
    @version NIIEngine 3.0.0
    */
    enum RenderQueueGroup
    {
        RQG_Unknow      = 0,
        RQG_Bg          = 1,
        RQG_Sky_First   = 2,
        RQG_Ext_1       = 3,
        RQG_Ext_2       = 4,
        RQG_Ext_3       = 5,
        RQG_Ext_4       = 6,
        RQG_Ext_5       = 7,
        RQG_Geo_1       = 10,
        RQG_Ext_6       = 11,
        RQG_Ext_7       = 12,
        RQG_Ext_8       = 13,
        RQG_Ext_9       = 14,
        RQG_Ext_10      = 15,
        RQG_User        = 20,
        RQG_Ext_11      = 21,
        RQG_Ext_12      = 22,
        RQG_Ext_13      = 23,
        RQG_Ext_14      = 24,
        RQG_Ext_15      = 25,
        RQG_Geo_2       = 30,
        RQG_Ext_16      = 31,
        RQG_Ext_17      = 32,
        RQG_Ext_18      = 33,
        RQG_Ext_19      = 34,
        RQG_Ext_20      = 35,
        RQG_Sky_Last    = 40,
        RQG_Surface     = 41,
        RQG_Count       = 42,
        RQG_Max         = 255
    };
    
    /** 渲染监听
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RenderListener
    {
    public:
        virtual ~RenderListener() {}

        /** 渲染前触发
        @version NIIEngine 3.0.0
        */
        virtual void renderBegin() {}

        /** 渲染队列前触发
        @param[in] render 渲染组
        @return 返回true代表渲染这个渲染组,否则不渲染
        @version NIIEngine 3.0.0
        */
        virtual bool renderBegin(GroupID gid)
        { 
            return true;
        }

        /** 渲染队列后触发
        @param[in] render 渲染组
        @return 返回true代表重复渲染这个渲染组,否则不重复渲染
        @version NIIEngine 3.0.0
        */
        virtual bool renderEnd(GroupID gid)
        { 
            return false;
        }
        
        /** 渲染后触发
        @version NIIEngine 3.0.0
        */
        virtual void renderEnd() {}
    };

    /** 渲染队列
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RenderQueue : public RenderAlloc
    {
    public:
        typedef vector<std::pair<RenderQueueGroup, RenderGroup *> >::type GroupList;
    public:
        RenderQueue();
        virtual ~RenderQueue();

        /** 设置默认组
        @version NIIEngine 3.0.0
        */
        void setDefaultGroup(GroupID grp)           {mDefaultGroup = grp;   }

        /** 获取默认组
        @version NIIEngine 3.0.0
        */
        GroupID getDefaultGroup() const             {return mDefaultGroup;  }

        /** 设置默认等级
        @version NIIEngine 3.0.0
        */
        void setDefaultLevel(Nui16 lv)              {mDefaultLevel = lv;    }

        /** 获取默认等级
        @version NIIEngine 3.0.0
        */
        Nui16 getDefaultLevel() const               {return mDefaultLevel;  }

        /** 清理队列时是否删除所有渲染组
        @versioin NIIEngine 3.0.0
        */
        void setDestroyAllOnClear(bool r)           {mRemoveGroupOnClear = r;   }
        
        /** 清理队列时是否删除所有渲染组
        @version NIIEngine 3.0.0
        */
        bool isDestroyAllOnClear() const            {return mRemoveGroupOnClear;}
        
        /** 添加指定对象到队列
        @version NIIEngine 3.0.0
        */
        void add(GeometryObj * obj)                 {add(obj, mDefaultGroup, mDefaultLevel);}

        /** 添加指定对象到队列
        @version NIIEngine 3.0.0
        */
        void add(GeometryObj * obj, GroupID gid)    {add(obj, gid, mDefaultLevel);}

        /** 添加指定对象到队列
        @version NIIEngine 3.0.0
        */
        void add(GeometryObj * obj, GroupID gid, Nui16 level);

        /** 清理队列
        @version NIIEngine 3.0.0
        */
        void clear();

        /** 获取指定渲染队列群组
        @version NIIEngine 3.0.0
        */
        RenderGroup * getQueueGroup(GroupID qid);

        /** 获取渲染列表
        @version NIIEngine 3.0.0 高级api
        */
        const GroupList & getRenderList() const{ return mGroups;}

        /** 合并队列
        @version NIIEngine 3.0.0
        */
        void merge(const RenderQueue * o);
    protected:
        /** 创建渲染组
        @version NIIEngine 3.0.0
        */
        void createImpl(RenderGroup *& out);

        /** 创建主组
        @version NIIEngine 3.0.0
        */
        void createMainGroup();
    protected:
        GroupList mGroups;
        GroupID mDefaultGroup;
        Nui16 mDefaultLevel;
        bool mRemoveGroupOnClear;
    };
}
#endif