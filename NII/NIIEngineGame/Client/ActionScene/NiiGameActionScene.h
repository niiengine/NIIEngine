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

#ifndef NII_Action_SCENE_H
#define NII_Action_SCENE_H

namespace NIIGame
{
    /** 一个动作场景
    @version NIIEngineGame 1.0.0
    */
    class _NIIGameExport ActionScene : public CommandObj, public EventObj
    {
    public:
        enum SceneObjectType
        {
            SOT_Sky,        ///< 场景天空型
            SOT_Best,       ///< 场景
            SOT_Book,       ///< 
            SOT_Chair,      ///< 
            SOT_Fluid,      ///< 
            SOT_Human,      ///< 
            SOT_Lamp,       ///< 
            SOT_Machine,    ///< 
            SOT_Music,      ///<
            SOT_Solid,      ///<
            SOT_Ball,       ///<
            SOT_Sky         ///<
        };
        typedef map<Nid, GameObj *>::type ObjectList;
        typedef map<Nid, SpaceManager *>::type SceneList;        
    public:
        ActionScene(){}

        virtual ~ActionScene();

        /** 设置环境因子(环境反射颜色)
        @param id 需要设置的场景的id
        @param type 场景对象类型
        @param value 颜色值
        */
        virtual setEnv(Nid id, SceneObjectType type, Colour value);

        /** 设置环境因子(天空)
        @param id 需要设置的场景的id
        @param type 场景对象类型
        @param effect 效果
        */
        virtual setEnv(Nid id, SceneObjectType type, GameEffect effect);

        /** 
        @param id
        @param type
        */
        virtual setEnv(NIIui id, SceneObjectType type, );

        /**
        @param id
        @param type
        */
        virtual setEnv(NIIui id, SceneObjectType type, );

        /** 附加一个对象到动作场景中
        @remark 如果这个游戏对象已经存在,将不会做任何事情
        @param src 一个还没附加到这个动作场景的游戏对象
        */
        ActionScene * operator +(const GameObj * src);

        /** 从动作场景中分离一个游戏对象
        @remark 如果这个游戏对象
        @param src 一个附加到这个动作场景的对象
        */
        ActionScene * operator -(const GameObj * src);

        /** 创建一个游戏对象
        @param 
        */
        virtual GameObj * createGameObject();

        /**
        @param dest 
        */
        virtual void destroy(GameObj * dest);
    public:
        const ViewWindow * getWindow() const;
    protected:
        Nid mID;                    ///< 这个动作场景的ID
        ObjectList mExtObjects;     ///< 附加到这个动作场景中的外部对象
        ObjectList mOwnObjects;     ///< 这个动作场景自己创建的内部对象 
        SceneList mScene;           ///< 一个动作场景需要的图形场景成分(必须是同个窗口概念体里的)
        ViewWindow * mWindow;       ///< 一个战争场景只能有一个窗口概念体
    };
}
#endif