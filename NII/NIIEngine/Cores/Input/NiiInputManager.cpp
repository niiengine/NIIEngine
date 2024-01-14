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

#include "NiiPreProcess.h"
#include "NiiInputManager.h"
#include "NiiViewWindow.h"
#include "NiiKeyboardControlPattern.h"
#include "NiiMouseControlPattern.h"
#include "NiiJoyPadControlPattern.h"
#include "NiiTouchControlPattern.h"

namespace NII
{
    class _EngineInner DummyManager : public InputManager
    {
    public:
        DummyManager() {}

        ///@copydetails DummyManager::getCount
        NCount getCount(ControlDevType type) const
        {
            return 0;
        }

        ///@copydetails DummyManager::init
        void init()
        {

        }
    protected:
        ///@copydetails DummyManager::createImpl
        NII_MEDIA::ControlPattern * createImpl(const ViewWindow *, ControlDevType type)
        {
            switch (type)
            {
            case CDT_Keyboard:
                return N_new NII_MEDIA::DummyKeyboardControlPattern();
            case CDT_Mouse:
                return N_new NII_MEDIA::DummyMouseControlPattern();
            case CDT_JoyPad:
                return N_new NII_MEDIA::DummyJoyPadControlPattern();
            case CDT_JoyHandle:
                return N_new NII_MEDIA::DummyJoyHandleControlPattern();
            case CDT_JoyWheel:
                return N_new NII_MEDIA::DummyJoyWheelControlPattern();
            case CDT_Touch:
                return N_new NII_MEDIA::DummyTouchControlPattern();
            default:
                return 0;
            }
            return 0;
        }
    };
    //-----------------------------------------------------------------------
    template<> InputManager * Singleton<InputManager>::mOnly = 0;
    //-----------------------------------------------------------------------
    InputManager::InputManager()
    {
        DummyPatternManager = N_new DummyManager();
        mMouse = N_new NII_MEDIA::DummyMouseControlItem();
        mKeyboard = N_new NII_MEDIA::DummyKeyboardControlItem();
        mJoyPad = N_new NII_MEDIA::DummyJoyPadControlItem();
        mJoyHandle = N_new NII_MEDIA::DummyJoyHandleControlItem();
        mJoyWheel = N_new NII_MEDIA::DummyJoyWheelControlItem();
        mTouch = N_new NII_MEDIA::DummyMultiTouchControlItem();
    }
    //-----------------------------------------------------------------------
    InputManager::~InputManager()
    {
        WindowControlList::iterator it, itend = mControls.end();
        for(it = mControls.begin(); it != itend; ++it)
        {
            (it->second).clear();
        }
        mControls.clear();

        N_delete mMouse;
        N_delete mKeyboard;
        N_delete mJoyPad;
        N_delete mJoyHandle;
        N_delete mJoyWheel;
        N_delete mTouch;
    }
    //-----------------------------------------------------------------------
    void InputManager::suspend(const ViewWindow * main)
    {
        WindowControlList::iterator i = mControls.find(main);
        if(i != mControls.end())
        {
            ControlList::iterator z, zend = (i->second).end();
            for(z = (i->second).begin(); z != zend; ++z)
                (*z)->_pause();
        }
    }
    //-----------------------------------------------------------------------
    void InputManager::active(const ViewWindow * main)
    {
        WindowControlList::iterator i = mControls.find(main);
        if(i != mControls.end())
        {
            ControlList::iterator z, zend = i->second.end();
            for(z = i->second.begin(); z != zend; ++z)
            {
                (*z)->bind(main);
                (*z)->_reset();
                (*z)->_start();
            }
        }
    }
    //-----------------------------------------------------------------------
    NII_MEDIA::ControlPattern * InputManager::create(const ViewWindow * main,
        ControlDevType type)
    {
        NCount cout = getCount(type);
        NCount current = getCount(main, type);
        NII_MEDIA::ControlPattern * item = NULL;
        if(cout > current)
        {
            WindowControlList::iterator it = mControls.find(main);
            if(it != mControls.end())
            {
                // 创建一个新的
                NII_MEDIA::ControlPattern * item = createImpl(main, type);
                // 添加新记录
                (it->second).push_back(item);
            }
            else
            {
                ControlList temp;
                // 创建一个新的
                NII_MEDIA::ControlPattern * item = createImpl(main, type);
                temp.push_back(item);
                // 创建一个窗体和控制处理管理器的映射
                mControls.insert(WindowControlList::value_type(main, temp));
            }
            if(main->isActive())
                item->_start();
        }
        // 无任何有效的输入设备
        return item;
    }
    //-----------------------------------------------------------------------
    void InputManager::retrieve(const ViewWindow * main, NII_MEDIA::ControlPattern * dest)
    {
        WindowControlList::iterator c = mControls.find(main);
        if(c != mControls.end())
        {
            ControlList::iterator it, itend = (c->second).end();
            for(it = (c->second).begin(); it != itend; ++it)
            {
                if((*it) == dest)
                {
                    (c->second).erase(it);
                    break;
                }
            }
            if ((c->second).empty())
            {
                mControls.erase(c);
            }
        }
    }
    //-----------------------------------------------------------------------
    NII_MEDIA::ControlPattern * InputManager::get(const ViewWindow * main,
        ControlDevType type, Nid id) const
    {
        WindowControlList::const_iterator c = mControls.find(main);
        if(c != mControls.end())
        {
            if (id == 0)
            {
                ControlList::const_iterator it, itend = c->second.end();
                for (it = c->second.begin(); it != itend; ++it)
                {
                    if ((*it)->getType() == type)
                        return *it;
                }
            }
            else
            {
                ControlList::const_iterator it, itend = c->second.end();
                for (it = c->second.begin(); it != itend; ++it)
                {
                    if (((*it)->getType() == type) && ((*it)->mID == id))
                        return *it;
                }
            }
        }
        return NULL;
    }
    //-----------------------------------------------------------------------
    void InputManager::apply(const ViewWindow * main, NII_MEDIA::ControlItem * item)
    {
        WindowControlList::iterator c = mControls.find(main);
        if(c != mControls.end())
        {
            ControlList::iterator it, itend = (c->second).end();
            for(it = (c->second).begin(); it != itend; ++it)
            {
                if(item->getType() == (*it)->getType())
                    (*it)->apply(item);
            }
        }
    }
    //-----------------------------------------------------------------------
    NCount InputManager::getCount(const ViewWindow * main, ControlDevType type) const
    {
        NCount num = 0;
        WindowControlList::const_iterator c = mControls.find(main);
        if(c != mControls.end())
        {
            ControlList::const_iterator it, itend = c->second.end();
            for(it = c->second.begin(); it != itend; ++it)
            {
                if((*it)->getType() == type)
                    ++ num;
            }
        }
        return num;
    }
    //-----------------------------------------------------------------------
}