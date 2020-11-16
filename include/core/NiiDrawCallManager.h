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

#ifndef _NII_DRAWCALL_MANAGER_H
#define _NII_DRAWCALL_MANAGER_H

#include "NiiPreInclude.h"
#include "NiiSingleton.h"
#include "NiiThread.h"

namespace NII
{
    /** 调用绘制群组
    @remark 分离(主线程)CPU 和(绘制线程/额外的扩展线程)(GPU/CPU) 线程，可以让GPU有更多的处理空间.
        减少主线程和绘制线程的相互等待.主线程在处理繁琐运算的同时让GPU在这个时候处理FBO等命令.
    @version NIIEngine 3.0.0
    */
    class _EngineAPI DrawCallGroup : public RenderAlloc
    {
    public:
        class _EngineAPI DrawCallUnit : public RenderAlloc
        {
        public:
            enum Type
            {
                T_Colour,
                T_Alpha,
                T_Blend,
                T_Depth,
                T_Stencil,
                T_Fog,
                T_Point,
                T_Program,
                T_FBO,                  ///< renderFrameObject
                T_VBO,
                T_Indirect,
                T_UniformBuffer,
                T_TextureBuffer,
                T_TextureList,
                T_TextureSampleList,
                T_Array,                ///< Array
                T_Index,                ///< Elements
                T_IndirectArray,
                T_IndirectIndex,
                T_MultiArray,
                T_MultiIndex,
                T_MultiIndirectArray,
                T_MultiIndirectIndex,
                T_None
            };
        public:
            Type mType;
            
        };
        
        typedef vector<DrawCallUnit *>::type UnitList;
    public:
        DrawCallGroup();
        ~DrawCallGroup();
        
        /**
        @version NIIEngine 3.0.0
        */
        DrawCallGroupID getID() const               { return mID; }
        
        /**
        @version NIIEngine 3.0.0
        */
        void add(DrawCallUnit * unit);
        
        /**
        @version NIIEngine 3.0.0
        */
        DrawCallUnit * create(DrawCallUnit::Type type) const;
        
        /**
        @version NIIEngine 3.0.0
        */
        DrawCallUnit * get(Nidx idx) const  { N_assert(idx < mUnitList.size(), "");  return mUnitList[idx]; }

        /**
        @version NIIEngine 3.0.0
        */
        DrawCallUnit * getLast() const      { return mUnitList.back();}
        
        /**
        @version NIIEngine 3.0.0
        */
        void remove(Nidx idx) const;
        
        /**
        @version NIIEngine 3.0.0
        */
        NCount getCount() const                 { return mUnitList.size();}
    private:
        DrawCallGroupID mID;
        UnitList mUnitList;
    };
    typedef vector<DrawCallGroup *>::type DrawCallGroupList;
    /** 绘制调用管理器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI DrawCallManager : public ThreadMain, public Singleton<DrawCallManager>
    {
    public:
        typedef map<DrawCallGroupID, DrawCallGroup *>::type GroupList;
    public:
        DrawCallManager();
        ~DrawCallManager();
        
        /**
        @version NIIEngine 3.0.0
        */
        DrawCallGroup * create(DrawCallGroupID id);
        
        /**
        @version NIIEngine 3.0.0
        */
        DrawCallGroup * get(DrawCallGroupID id) const;
        
        /**
        @version NIIEngine 3.0.0
        */
        void remove(DrawCallGroupID id);

        /// @copydetails ThreadMain::run
        void run(void * arg);
    private:
        GroupList mGroupList;
    };
}
#endif
