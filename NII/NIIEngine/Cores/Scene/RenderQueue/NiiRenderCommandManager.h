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
    class _EngineAPI RenderCommand : public RenderAlloc
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
            T_ShaderCh,
            T_FBO,                  ///< Frame Buffer Object (RenderTarget)
            T_PBO,                  ///< Pixel Buffer Object
            T_VBO,                  ///< Vertex Buffer Object
            T_EBO,                  ///< Elements Buffer Object
            T_VAO,                  ///< Vertex Array Object
            T_IBO,                  ///< Indirect Buffer Object
            T_GEO,                  ///< GeometryRaw
            T_StructBuffer,
            T_TextureBuffer,
            T_TextureList,
            T_TextureSampleList,
            T_Array,                ///< Arrays
            T_Index,                ///< Elements
            T_ArrayIndirect,
            T_IndexIndirect,
            T_MultiArray,
            T_MultiIndex,
            T_MultiArrayIndirect,
            T_MultiIndexIndirect,
            T_FenceNotify,
            T_FenceWait,            ///< CPU
            T_FenceDevWait,         ///< GPU
            T_ThreadWait,           ///< CPU
            T_ThreadNotify,         ///< CPU
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
        inline Type getType() const                 { return mType; }
        
        /** 运行调用绘制单元
        @version NIIEngine 3.0.0
        */
        virtual void run(RenderCommandGroup * group) = 0;
    public:
        Type mType;
    };
    
    typedef vector<RenderCommand *>::type RenderCommandList;
    typedef vector<RenderCommandGroup *>::type DrawCallGroupList;
    
    /** 调用绘制群组
    @remark 分离(主线程)CPU 和(绘制线程/额外的扩展线程)(GPU/CPU) 线程，可以让GPU有更多的处理空间.
        减少主线程和绘制线程的相互等待.主线程在处理繁琐运算的同时让GPU在这个时候处理FBO, Staging / GLsync 等命令.
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RenderCommandGroup : public RenderAlloc
    {
    public:
        RenderCommandGroup();
        ~RenderCommandGroup();
        
        /**
        @version NIIEngine 6.0.0
        */
        inline void addCondition(RenderCommandGroup * rcg)  { mConditionList.push_back(rcg);}
        
        /**
        @version NIIEngine 6.0.0
        */
        inline void clearCondition()                        { mConditionList.clear(); }
        
        /**
        @version NIIEngine 6.0.0
        */
        inline void addCommand(RenderCommandGroup * rcg)    { mCommandList.push_back(rcg);}
        
        /**
        @version NIIEngine 6.0.0
        */
        inline void clearCommand()                          { mCommandList.clear(); }
        
        /** 组id
        @version NIIEngine 3.0.0
        */
        inline DrawCallGroupID getID() const                { return mID; }
        
        /** 设置操作的渲染系统
        @version NIIEngine 3.0.0
        */
        inline void setRender(RenderSys * sys)              { mRenderSys = sys; }
        
        /** 获取操作的渲染系统
        @version NIIEngine 3.0.0
        */
        inline RenderSys * getRender() const                { return mRenderSys;}
        
        /** 执行调用
        @version NIIEngine 3.0.0
        */
        void run();
        
        /** 创建调用绘制单元
        @param[in] fence 是否创建一个通知
        @version NIIEngine 3.0.0
        */
        RenderCommand * create(RenderCommand::Type type, bool fence = false);
        
        /** 获取索引下标调用绘制单元
        @version NIIEngine 3.0.0
        */
        inline RenderCommand * get(Nidx idx) const           { N_assert(idx < mUnitList.size(), "");  return mUnitList[idx]; }

        /** 获取最后一个调用绘制单元
        @version NIIEngine 3.0.0
        */
        inline RenderCommand * getLast() const               { return mUnitList.back();}
        
        /** 移去调用绘制单元
        @version NIIEngine 3.0.0
        */
        void remove(Nidx idx);
        
        /** 获取调用绘制单元数量
        @version NIIEngine 3.0.0
        */
        inline NCount getCount() const                      { return mUnitList.size();}
    private:
        RenderSys * mRenderSys;
        DrawCallGroupList mConditionList;
        DrawCallGroupList mCommandList;
        DrawCallGroupID mID;
        RenderCommandList mUnitList;
    };
    
    /** 调用绘制单元工厂类
    @note RenderCommand 类是实例对象,所以需要这个类辅助
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RenderCommandFactory : public VFSAlloc
    {
    public:
        RenderCommandFactory() {}
        virtual ~RenderCommandFactory() {}

        /** 类型
        @version NIIEngine 3.0.0
        */
        virtual RenderCommand::Type getType() const = 0;

        /** 创建实例
        @version NIIEngine 3.0.0
        */
        virtual RenderCommand * create() = 0;

        /** 删除实例
        @version NIIEngine 3.0.0
        */
        virtual void destroy(RenderCommand * obj) = 0;
    };
    
    /** 绘制调用管理器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RenderCommandManager : public ThreadMain, public Singleton<RenderCommandManager>
    {
        friend RenderCommandGroup;
    public:
        typedef map<DrawCallGroupID, RenderCommandGroup *>::type GroupList;
        typedef map<RenderCommand::Type, RenderCommandFactory *>::type FactoryList;
    public:
        RenderCommandManager();
        ~RenderCommandManager();
        
        /** 添加工厂
        @version NIIEngine 3.0.0
        */
        void add(RenderCommandFactory * factory);

        /** 移去工厂
        @version NIIEngine 3.0.0
        */
        void remove(RenderCommandFactory * factory);

        /** 创建调用绘制群组
        @version NIIEngine 3.0.0
        */
        RenderCommandGroup * create(DrawCallGroupID id);

        /** 获取调用绘制群组
        @version NIIEngine 3.0.0
        */
        RenderCommandGroup * get(DrawCallGroupID id) const;

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
        RenderCommand * createUnit(RenderCommand::Type type) const;
        
        /** 删除调用绘制单元
        @version NIIEngine 3.0.0
        */
        void destroyUnit(RenderCommand * unit) const;
    private:
        ThreadMutex mInputMutex;
        ThreadMutex mOpMutex;
        FactoryList mFactoryList;
        GroupList mGroupList;
    };
}
#endif
