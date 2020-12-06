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
    /** 调用绘制单元
    @version NIIEngine 3.0.0
    */
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
            T_Array,                ///< Arrays
            T_Index,                ///< Elements
            T_IndirectArray,
            T_IndirectIndex,
            T_MultiArray,
            T_MultiIndex,
            T_MultiIndirectArray,
            T_MultiIndirectIndex,
            T_Ext1,
            T_Ext2,
            T_Ext3,
            T_Ext4,
            T_Ext5,
            T_Ext6,
            T_Ext7,
            T_Ext8,
            T_Ext9,
            T_Ext10,
            T_Ext11,
            T_Ext12,
            T_Ext13,
            T_Ext14,
            T_Ext15,
            T_Ext16,
            T_Ext17,
            T_Ext18,
            T_Ext19,
            T_Ext20,
            T_Ext21,
            T_Ext22,
            T_Ext23,
            T_Ext24,
            T_Ext25,
            T_Ext26,
            T_Ext27,
            T_Ext28,
            T_Ext29,
            T_Ext30,
            T_Ext31,
            T_Ext32
        };
        
        /** 获取类型
        @version NIIEngine 3.0.0
        */
        inline Type getType() const { return mType; }
        
        /** 运行调用绘制单元
        @version NIIEngine 3.0.0
        */
        virtual void run(DrawCallGroup * group) = 0;
    public:
        Type mType;
            
    };
    /** 调用绘制群组
    @remark 分离(主线程)CPU 和(绘制线程/额外的扩展线程)(GPU/CPU) 线程，可以让GPU有更多的处理空间.
        减少主线程和绘制线程的相互等待.主线程在处理繁琐运算的同时让GPU在这个时候处理FBO等命令.
    @version NIIEngine 3.0.0
    */
    class _EngineAPI DrawCallGroup : public RenderAlloc
    {
    public:
        typedef vector<DrawCallUnit *>::type UnitList;
    public:
        DrawCallGroup();
        ~DrawCallGroup();
        
        /** 设置操作的渲染系统
        @version NIIEngine 3.0.0
        */
        void setOpSys(RenderSys * sys)              {mOpSys = sys;}
        
        /** 获取操作的渲染系统
        @version NIIEngine 3.0.0
        */
        RenderSys * getOpSys() const                { return mOpSys;}
        
        /** 执行调用
        @version NIIEngine 3.0.0
        */
        void run();
        
        /** 组id
        @version NIIEngine 3.0.0
        */
        DrawCallGroupID getID() const               { return mID; }
        
        /** 创建调用绘制单元
        @version NIIEngine 3.0.0
        */
        DrawCallUnit * create(DrawCallUnit::Type type);
        
        /** 获取索引下标调用绘制单元
        @version NIIEngine 3.0.0
        */
        DrawCallUnit * get(Nidx idx) const          { N_assert(idx < mUnitList.size(), "");  return mUnitList[idx]; }

        /** 获取最后一个调用绘制单元
        @version NIIEngine 3.0.0
        */
        DrawCallUnit * getLast() const              { return mUnitList.back();}
        
        /** 移去调用绘制单元
        @version NIIEngine 3.0.0
        */
        void remove(Nidx idx);
        
        /** 获取调用绘制单元数量
        @version NIIEngine 3.0.0
        */
        NCount getCount() const                     { return mUnitList.size();}
    private:
        RenderSys * mOpSys;
        DrawCallGroupID mID;
        UnitList mUnitList;
    };
    typedef vector<DrawCallGroup *>::type DrawCallGroupList;
    
    /** 调用绘制单元工厂类
    @note DrawCallUnit 类是实例对象,所以需要这个类辅助
    @version NIIEngine 3.0.0
    */
    class _EngineAPI DrawCallUnitFactory : public VFSAlloc
    {
    public:
        DrawCallUnitFactory() {}
        virtual ~DrawCallUnitFactory() {}

        /** 类型
        @version NIIEngine 3.0.0
        */
        virtual DrawCallUnit::Type getType() const = 0;

        /** 创建实例
        @version NIIEngine 3.0.0
        */
        virtual DrawCallUnit * create() = 0;

        /** 删除实例
        @version NIIEngine 3.0.0
        */
        virtual void destroy(DrawCallUnit * obj) = 0;
    };
    
    /** 绘制调用管理器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI DrawCallManager : public ThreadMain, public Singleton<DrawCallManager>
    {
        friend DrawCallGroup;
    public:
        typedef map<DrawCallGroupID, DrawCallGroup *>::type GroupList;
        typedef map<DrawCallUnit::Type, DrawCallUnitFactory *>::type FactoryList;
    public:
        DrawCallManager();
        ~DrawCallManager();
        
        /** 添加工厂
        @version NIIEngine 3.0.0
        */
        void add(DrawCallUnitFactory * factory);

        /** 移去工厂
        @version NIIEngine 3.0.0
        */
        void remove(DrawCallUnitFactory * factory);

        /** 创建调用绘制群组
        @version NIIEngine 3.0.0
        */
        DrawCallGroup * create(DrawCallGroupID id);

        /** 获取调用绘制群组
        @version NIIEngine 3.0.0
        */
        DrawCallGroup * get(DrawCallGroupID id) const;

        /** 移去调用绘制群组
        @version NIIEngine 3.0.0
        */
        void destroy(DrawCallGroupID id);

        /// @copydetails ThreadMain::run
        void run(void * arg);
    protected:
        /** 创建调用绘制单元
        @version NIIEngine 3.0.0
        */
        DrawCallUnit * createUnit(DrawCallUnit::Type type) const;
        
        /** 删除调用绘制单元
        @version NIIEngine 3.0.0
        */
        void destroyUnit(DrawCallUnit * unit) const;
    private:
        ThreadMutex mInputMutex;
        ThreadMutex mOpMutex;
        FactoryList mFactoryList;
        GroupList mGroupList;
    };
}
#endif
